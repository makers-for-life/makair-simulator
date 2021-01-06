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
      m_Vp(nanf("")),

      // parameters of the actuators
      m_Kr(6e4),            // coefficient of resistance in Pa.(m3.s-1)-1 / %
      m_Kroffset(1 * 1e6),  // coefficient of resistance offsetin Pa.(m3.s-1)-1
      m_K_blower(100),      // coefficient of blower pressure in Pa / %

      // parameters of the sensors
      m_K_pres(1e-1),     // mmH2O / Pa
      m_K_flow(60 * 1e6)  // ml/min <- m3/s

{
    m_previousInspiratoryFlowMeanMovingMeanIndex = 0;
    for (int32_t i = 0; i < PREVIOUS_INSPIRATORY_FLOW_MOVING_MEAN_SIZE; i++) {
        m_previousInspiratoryFlowMeanMovingMean[i] = 0;
    }
    m_previousInspiratoryFlowMean = 0;

    m_previousExpiratoryValvePositionMeanMovingMeanIndex = 0;
    m_previousInspiratoryValvePositionMeanMovingMeanIndex = 0;
    for (int32_t i = 0; i < PREVIOUS_VALVE_POSITION_MOVING_MEAN_SIZE; i++) {
        m_previousInspiratoryValvePositionMeanMovingMean[i] = 0;
        m_previousExpiratoryValvePositionMeanMovingMean[i] = 0;
    }
    m_previousInspiratoryValvePositionMean = 0;
    m_previousExpiratoryValvePositionMean = 0;

    previousPbl = 0.0;
}

void Model::init(int32_t p_resistance, int32_t p_compliance) {
    // parameters of the patient
    m_Rf = 1000000 * 1e8;                             // resistance of leaking in Pa.(m3.s-1)-1
    m_R = ((float)p_resistance) * 98.0665 / (10e-3);  // resistance of the patient in Pa.(m3.s-1)-1
    m_C = float(p_compliance) * 1e-6 / 98.0665;       // compilance of the patient in m3.Pa-1
    m_Vp = 0.;  // Volume of air in the lungs of the patient above rest volume in m3
    cout << m_R << endl;
    cout << m_C << endl;
}

SensorsData Model::compute(ActuatorsData cmds, float dt) {

    // Clip commands
    int _min = 1;
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

    // Conversion of actuators command into physical parameters of the model
    float Re = res_valves(m_previousExpiratoryValvePositionMean, m_Kr, m_Kroffset);
    float Ri = res_valves(m_previousInspiratoryValvePositionMean, m_Kr, m_Kroffset);
    float newPbl = blower.getBlowerPressure(m_previousInspiratoryFlowMean)
                   / m_K_pres;  // m_K_blower * cmds.blower;  // dynamic of the blower is not taken
                                // into account yet
    float Pbl;
    if (newPbl > previousPbl) {
        Pbl = min(newPbl, previousPbl + 300);
    } else {
        Pbl = max(newPbl, previousPbl - 300);
    }
    previousPbl = Pbl;

    // resistance of expiration valve and leak are in paralell
    float Ref = Re * m_Rf / (Re + m_Rf);

    // influence of blower and atm pressure on the flow (derivative of the volume)
    float P_factor = (Ref * Pbl) / (m_R * (Ref + Ri) + Ref * Ri);

    // influence of volume present in patient's lungs on the flow
    float V_factor = -m_Vp * (Ref + Ri) / (m_C * (m_R * (Ref + Ri) + Ref * Ri));

    // patient's flow
    float flow = P_factor + V_factor;

    // conputation of the new patient's lung's volume
    m_Vp += flow * dt;

    cout << ", Pbl=" << Pbl << ", m_R=" << m_R << ", Ri=" << Ri << ", Ref=" << Ref << ", Re=" << Re
         << ", P_factor=" << m_K_flow * P_factor / 1000
         << ", V_factor=" << m_K_flow * V_factor / 1000 << ", flow=" << m_K_flow * flow / 1000
         << ", cmds.inspirationValve=" << m_previousInspiratoryValvePositionMean
         << ", cmds.expirationValve=" << m_previousExpiratoryValvePositionMean << endl;
    // cout << (res_valves(39, 1.0, 1.0)) << ", " << (res_valves(40, 1.0, 1.0)) << endl;

    // computation of sensor data
    SensorsData output;
    output.inspiratoryPressure = m_K_pres * (flow * m_R + m_Vp / m_C);
    output.inspiratoryFlow = m_K_flow * (Pbl - output.inspiratoryPressure / m_K_pres) / Ri;
    int32_t leakFlow = m_K_flow * (Pbl - output.inspiratoryPressure / m_K_pres) / m_Rf;
    output.expiratoryFlow = m_K_flow * (output.inspiratoryPressure / m_K_pres - 0) / Re;

    /*cout << "volume=" << m_Vp * 1e6 << "mL, inspiratoryFlow=" << output.inspiratoryFlow / 1000
         << "L/min, expiratoryFlow=" << output.expiratoryFlow / 1000
         << "L/min, flow=" << flow * m_K_flow / 1000 << "L/min, leakFlow=" << leakFlow / 1000
         << "L/min, inspiratoryPressure=" << output.inspiratoryPressure
         << "mmH2O, blowerPressre=" << Pbl * m_K_pres
         << "mmH2O, Pfactor=" << m_K_flow * P_factor / 1000
         << "L/min, V_factor=" << m_K_flow * V_factor / 1000
         << "L/min, cmdinspi= " << cmds.inspirationValve << "%, " << endl;*/

    m_previousInspiratoryFlowMeanMovingMeanIndex++;
    if (m_previousInspiratoryFlowMeanMovingMeanIndex
        >= PREVIOUS_INSPIRATORY_FLOW_MOVING_MEAN_SIZE) {
        m_previousInspiratoryFlowMeanMovingMeanIndex = 0;
    }
    m_previousInspiratoryFlowMeanMovingMean[m_previousInspiratoryFlowMeanMovingMeanIndex] =
        output.inspiratoryFlow;
    int32_t sum = 0;
    for (int32_t i = 0; i < PREVIOUS_INSPIRATORY_FLOW_MOVING_MEAN_SIZE; i++) {
        sum += m_previousInspiratoryFlowMeanMovingMean[i];
    }
    m_previousInspiratoryFlowMean = sum / PREVIOUS_INSPIRATORY_FLOW_MOVING_MEAN_SIZE;
    return (output);
}

/**
 * function thats compute the resistance of the valves depending of the
 * openning. Here the relation is linear, but it can be upgraded to a more
 * realistic model
 * @opening_valve in % of the maximum opening
 * @K_r the coefficient of resistance in Pa.(m3.s-1)-1 / %
 */
float res_valves(int opening_valve, float K_r, float K_roffset) {
    float return_value;

    float x = opening_valve;
    if (opening_valve <= 40) {
        return_value = 33408.0 * x + 387000.0;

    } else if (opening_valve <= 72.0) {
        return_value = 5.1467340524981079e+009 - 6.1181197775600815e+008 * x
                       + 3.0148832673133574e+007 * pow(x, 2) - 7.8827886257496232e+005 * pow(x, 3)
                       + 1.1536362294209976e+004 * pow(x, 4) - 8.9625225665302438e+001 * pow(x, 5)
                       + 2.8899388749241267e-001 * pow(x, 6);

    } else if (opening_valve <= 76.0) {
        return_value = 32561335.5 * x - 2310083471.0;
    } else if (opening_valve <= 80.0) {
        return_value = 721934512.0 * x - 54702444885.0;
    } else if (opening_valve <= 84.0) {
        return_value = 3235655141.5 * x - 255800095245.0;
    } else if (opening_valve <= 100.0) {
        return_value = 521589460.5 * x - 27818578041.0;
    } else {
        cout << "error in valve angle" << endl;
        return_value = 1e10;
    }
    return return_value;
}