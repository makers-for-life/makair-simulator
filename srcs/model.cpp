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

Model::Model()
    :  // initialization of the state variables with nan to trigger error if not initialized
      m_Rf(nanf("")),
      m_R(nanf("")),
      m_C(nanf("")),
      m_Ce(nanf("")),
      m_Vpatient(nanf("")),

      // parameters of the actuators
      m_Kr(6e4),            // coefficient of resistance in Pa.(m3.s-1)-1 / %
      m_Kroffset(1 * 1e6),  // coefficient of resistance offsetin Pa.(m3.s-1)-1
      m_K_blower(70),       // coefficient of blower pressure in Pa / %

      // parameters of the sensors
      m_K_pres(1e-1),     // mmH2O / Pa
      m_K_flow(60 * 1e6)  // ml/min <- m3/s

{

    m_previousExpiratoryValvePositionMeanMovingMeanIndex = 0;
    m_previousInspiratoryValvePositionMeanMovingMeanIndex = 0;
    for (int32_t i = 0; i < PREVIOUS_VALVE_POSITION_MOVING_MEAN_SIZE; i++) {
        m_previousInspiratoryValvePositionMeanMovingMean[i] = 0;
        m_previousExpiratoryValvePositionMeanMovingMean[i] = 0;
    }

    m_previousInspiratoryValvePositionMean = 0;
    m_previousExpiratoryValvePositionMean = 0;

    m_previousPbl = 0.0;
    m_previousPresp = 0.0;
    m_previousInspiratoryFlow = 0.0;
    m_previousVentilatorPressure = 0.0;
    m_previousPreviousVentilatorPressure = 0.0;
    m_previousVentilatorFlow = 0.0;
}

void Model::init(int32_t p_resistance, int32_t p_compliance, int32_t p_inertance) {
    // parameters of the patient
    m_Rf = 1000000 * 1e8;                             // resistance of leaking in Pa.(m3.s-1)-1
    m_R = ((float)p_resistance) * 98.0665 / (10e-3);  // resistance of the patient in Pa.(m3.s-1)-1
    m_C = float(p_compliance) * 1e-6 / 98.0665;       // compilance of the patient in m3.Pa-1
    m_Ce = 8e-9;                                      //
    m_Vpatient = 0.0;  // Volume of air in the lungs of the patient above rest volume in m3
    m_Vcircuit = 0.0;  // Volume of air in the circuit in m3
    cout << m_R << endl;
    cout << m_C << endl;
}

SensorsData Model::compute(ActuatorsData cmds, float dt) {

    // Clip commands
    int _min = 0;
    int _max = 100;
    cmds.inspirationValve = min(max(cmds.inspirationValve, _min), _max);
    cmds.expirationValve = min(max(cmds.expirationValve, _min), _max);
    cmds.blower = min(max(cmds.blower, _min), _max);

    m_previousExpiratoryValvePositionMeanMovingMeanIndex++;
    m_previousInspiratoryValvePositionMeanMovingMeanIndex++;
    if (m_previousInspiratoryValvePositionMeanMovingMeanIndex
        >= PREVIOUS_VALVE_POSITION_MOVING_MEAN_SIZE) {
        m_previousInspiratoryValvePositionMeanMovingMeanIndex = 0;
    }
    if (m_previousExpiratoryValvePositionMeanMovingMeanIndex
        >= PREVIOUS_VALVE_POSITION_MOVING_MEAN_SIZE) {
        m_previousExpiratoryValvePositionMeanMovingMeanIndex = 0;
    }
    m_previousInspiratoryValvePositionMeanMovingMean
        [m_previousInspiratoryValvePositionMeanMovingMeanIndex] = cmds.inspirationValve;
    m_previousExpiratoryValvePositionMeanMovingMean
        [m_previousExpiratoryValvePositionMeanMovingMeanIndex] = cmds.expirationValve;
    int32_t sumInspiratory = 0;
    int32_t sumExpiratory = 0;
    for (int32_t i = 0; i < PREVIOUS_VALVE_POSITION_MOVING_MEAN_SIZE; i++) {
        sumInspiratory += m_previousInspiratoryValvePositionMeanMovingMean[i];
        sumExpiratory += m_previousExpiratoryValvePositionMeanMovingMean[i];
    }
    m_previousInspiratoryValvePositionMean =
        sumInspiratory / PREVIOUS_VALVE_POSITION_MOVING_MEAN_SIZE;
    m_previousExpiratoryValvePositionMean =
        sumExpiratory / PREVIOUS_VALVE_POSITION_MOVING_MEAN_SIZE;
    // dynamic of the blower is not taken
    // into account yet

    float newPbl = (703.0 * cmds.blower / 100.0 - 281.0 * m_previousInspiratoryFlow / 100000.0
                    - 832.0 * (m_previousInspiratoryFlow / 100.0)
                          * (m_previousInspiratoryFlow / 100.0) / 1000000.0)
                   / m_K_pres;
    float Pbl;
    if (newPbl > m_previousPbl) {
        Pbl = min(newPbl, m_previousPbl + 30);
    } else {
        Pbl = max(newPbl, m_previousPbl - 30);
    }

    // Calculate flow using venturi relation : https://en.wikipedia.org/wiki/Venturi_effect
    float A1 = 3.32e-05;
    float A2surA1insp = max(0.0, -9.35E-03 * m_previousInspiratoryValvePositionMean + 0.809);
    float A2surA1exp = max(0.0, -9.35E-03 * m_previousExpiratoryValvePositionMean + 0.809);
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

    // patient's flow
    float ventilatorFlow = Qinsp - Qexp;
    float di = ventilatorFlow - m_previousVentilatorFlow;
    float ventilatorPressure =
        (m_R * m_Ce * (2 * m_previousVentilatorPressure - m_previousPreviousVentilatorPressure)
         + (1 - m_Ce / m_C) * m_previousVentilatorPressure * dt + dt * dt * ventilatorFlow / m_C
         + m_R * di * dt)
        / ((1 - m_Ce / m_C) * dt + m_R * m_Ce);  // m_previousPatientFlow * m_R + m_Vpatient / m_C;
    float circuitFlow = m_Ce * (ventilatorPressure - m_previousVentilatorPressure) / dt;
    float patientFlow = ventilatorFlow - circuitFlow;

    // conputation of the new patient's lung's volume
    m_Vpatient += patientFlow * dt;
    m_Vcircuit += circuitFlow * dt;

    // computation of sensor data
    SensorsData output;
    output.inspiratoryPressure = m_K_pres * (patientFlow * m_R + m_Vpatient / m_C);
    /*cout << "ventilatorPressure=" << ventilatorPressure << ", Pbl=" << Pbl
         << ", m_previousPresp=" << m_previousPresp
         << ", ventilatorFlow=" << m_K_flow * ventilatorFlow / 1000
         << ", circuitFlow=" << m_K_flow * circuitFlow / 1000
         << ", patientFlow=" << m_K_flow * patientFlow / 1000 << ", m_Vpatient=" << m_Vpatient * 1e6
         << ", m_Vcircuit=" << m_Vcircuit * 1e6 << endl;*/

    output.inspiratoryFlow = m_K_flow * Qinsp;
    output.expiratoryFlow = m_K_flow * Qexp;

    m_previousInspiratoryFlow = m_K_flow * Qinsp;
    m_previousPbl = Pbl;
    // m_previousPatientFlow = patientFlow;
    m_previousVentilatorFlow = ventilatorFlow;
    m_previousPreviousVentilatorPressure = m_previousVentilatorPressure;
    m_previousVentilatorPressure = ventilatorPressure;

    return (output);
}
