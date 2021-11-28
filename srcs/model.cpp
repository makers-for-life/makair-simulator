/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file model.cpp
 * @brief Simulate the model of the respirator and the patient.
 *****************************************************************************/

// INCLUDES ===================================================================

#include "../includes/model.h"
#include "../includes/blower.h"

#include <cmath>
#include <iostream>
using namespace std;

// FUNCTIONS ==================================================================

Model::Model() {

    m_previousExpiratoryValvePositionLastValuesIndex = 0;
    m_previousInspiratoryValvePositionLastValuesIndex = 0;
    for (int32_t i = 0; i < PREVIOUS_VALVE_POSITION_TABLE_SIZE; i++) {
        m_previousInspiratoryValvePositionLastValues[i] = 0;
        m_previousExpiratoryValvePositionLastValues[i] = 0;
    }

    m_previousPbl = 0.0;
    m_previousInspiratoryFlow = 0.0;
    m_previousVentilatorPressure = 0.0;
    m_previousPreviousVentilatorPressure = 0.0;
    m_previousVentilatorFlow = 0.0;
    m_previousMusclePressure = 0.0;
}

void Model::init(PatientModel& p_patientModel) {
    m_Vcircuit = 0.0;  // Volume of air in the circuit in m3
    m_patientModel = &p_patientModel;
    m_musclePressure = 0.0;  // m_patientModel->getMusclePressure();
}

SensorsData Model::compute(ActuatorsData cmds, float dt) {

    // update with time increment in microseconds
    m_patientModel->updateTimeUs(int64_t(dt * 1000000));

    // Clip commands
    int _min = 0;
    int _max = 100;
    cmds.inspirationValve = min(max(cmds.inspirationValve, _min), _max);
    cmds.expirationValve = min(max(cmds.expirationValve, _min), _max);
    cmds.blower = min(max(cmds.blower, _min), _max);

    // Store the current position of each valve, but use for this iteration the position of the
    // valve PREVIOUS_VALVE_POSITION_TABLE_SIZE iterations before. It simulated a latency in the
    // valve
    m_previousExpiratoryValvePositionLastValuesIndex++;
    m_previousInspiratoryValvePositionLastValuesIndex++;
    if (m_previousInspiratoryValvePositionLastValuesIndex >= PREVIOUS_VALVE_POSITION_TABLE_SIZE) {
        m_previousInspiratoryValvePositionLastValuesIndex = 0;
    }
    if (m_previousExpiratoryValvePositionLastValuesIndex >= PREVIOUS_VALVE_POSITION_TABLE_SIZE) {
        m_previousExpiratoryValvePositionLastValuesIndex = 0;
    }
    float suminspi = 0.0;
    float sumexpi = 0.0;
    for (int32_t i = 0; i < PREVIOUS_VALVE_POSITION_TABLE_SIZE; i++) {
        suminspi += m_previousInspiratoryValvePositionLastValues[i];
        sumexpi += m_previousExpiratoryValvePositionLastValues[i];
    }
    float previousInspiratoryValvePosition =
        suminspi
        / PREVIOUS_VALVE_POSITION_TABLE_SIZE; /*m_previousInspiratoryValvePositionLastValues
[m_previousInspiratoryValvePositionLastValuesIndex];*/
    m_previousInspiratoryValvePositionLastValues
        [m_previousInspiratoryValvePositionLastValuesIndex] = cmds.inspirationValve;

    float previousExpiratoryValvePosition =
        sumexpi / PREVIOUS_VALVE_POSITION_TABLE_SIZE; /*m_previousExpiratoryValvePositionLastValues
[m_previousExpiratoryValvePositionLastValuesIndex];*/
    m_previousExpiratoryValvePositionLastValues[m_previousExpiratoryValvePositionLastValuesIndex] =
        cmds.expirationValve;

    // Calculate blower pressure. Use the same modelisation used in the blower.cpp.  This model is
    // an order 2 polynom giving pressure as function of the flow in the inspiratory branch. The
    // coefficients of the polynom have been found expermientally with the blower at MAX_SPEED. The
    // pressure at lower speed are considered just as an offset (this could be improved).

    float newPbl =
        max(0.0, (703.0 * cmds.blower / 100.0 - 281.0 * m_previousInspiratoryFlow / 100000.0
                  - 832.0 * (m_previousInspiratoryFlow / 100.0)
                        * (m_previousInspiratoryFlow / 100.0) / 1000000.0)
                     / m_K_pres);
    float Pbl;
    // apply a progressive ramp-up on the blower
    if (newPbl > m_previousPbl) {
        Pbl = min(newPbl, m_previousPbl + 30);
    } else {
        Pbl = max(newPbl, m_previousPbl - 30);
    }

    /* The section of each valve is first calculated with
    a linear model whose coefficient has been found experimentally. Then knowing for each valve the
    pressure before and after the valve, the flow is calculated using the venturi relation :
    https://en.wikipedia.org/wiki/Venturi_effect. */
    float A1 = 3.32e-05;
    float A2surA1insp = max(0.0, -9.35E-03 * previousInspiratoryValvePosition + 0.809);
    float A2surA1exp = max(0.0, -9.35E-03 * previousExpiratoryValvePosition + 0.809);
    float rho = 1.2;
    float Qinsp = (Pbl - m_previousVentilatorPressure < 0)
                      ? 0
                      : A1
                            * sqrt((2.0 / rho) * (Pbl - m_previousVentilatorPressure)
                                   / (pow((1 / A2surA1insp), 2) - 1));
    float Qexp = (m_previousVentilatorPressure - 0 < 0)
                     ? 0
                     : A1
                           * sqrt((2.0 / rho) * (m_previousVentilatorPressure - 0)
                                  / (pow((1 / A2surA1exp), 2) - 1));

    // RRCC model, as described in paragraph 3.3.2 of
    // http://folk.ntnu.no/skoge/prost/proceedings/acc05/PDFs/Papers/0385_ThA13_1.pdf. The circuit
    // resistance is considered negligible which leads to a RCC model. The computation is made using
    // disrete derivation.
    float Rl = m_patientModel->getRl();
    float Cl = m_patientModel->getCl();
    float Ct = m_patientModel->getCt();
    float m_musclePressure = m_patientModel->getMusclePressure();

    float ventilatorFlow = Qinsp - Qexp;
    float di = ventilatorFlow - m_previousVentilatorFlow;
    float ventilatorPressure =
        (Rl * Ct * (2 * m_previousVentilatorPressure - m_previousPreviousVentilatorPressure)
         + (1 + Ct / Cl) * m_previousVentilatorPressure * dt + dt * dt * ventilatorFlow / Cl
         + Rl * di * dt + (m_previousMusclePressure - m_musclePressure) * dt)
        / ((1 + Ct / Cl) * dt + Rl * Ct);

    float circuitFlow = Ct * (ventilatorPressure - m_previousVentilatorPressure) / dt;
    float patientFlow = ventilatorFlow - circuitFlow;

    // This prevent long term deviation. This is something similar to a kalman filter.
    /*ventilatorPressure =
        (0.01 * (patientFlow * Rl + m_Vpatient / Cl) + ventilatorPressure) / 1.01;*/

    // conputation of the new patient's lung's volume
    m_patientModel->setVpatient(m_patientModel->getVpatient() + patientFlow * dt);
    // Computation of the circuit volume
    m_Vcircuit += circuitFlow * dt;

    // computation of sensor data
    SensorsData output;
    output.inspiratoryPressure = m_K_pres * (ventilatorPressure);

    output.inspiratoryFlow = m_K_flow * Qinsp;
    output.expiratoryFlow = m_K_flow * Qexp;

    m_previousInspiratoryFlow = m_K_flow * Qinsp;
    m_previousPbl = Pbl;
    m_previousVentilatorFlow = ventilatorFlow;
    m_previousPreviousVentilatorPressure = m_previousVentilatorPressure;
    m_previousVentilatorPressure = ventilatorPressure;
    m_previousMusclePressure = m_musclePressure;

    return (output);
}
