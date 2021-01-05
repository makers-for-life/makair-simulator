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

    // Conversion of actuators command into physical parameters of the model
    float Re = res_valves(cmds.expirationValve, m_Kr, m_Kroffset);
    float Ri = res_valves(cmds.inspirationValve, m_Kr, m_Kroffset);
    float Pbl = blower.getBlowerPressure(m_previousInspiratoryFlowMean)
                / m_K_pres;  // m_K_blower * cmds.blower;  // dynamic of the blower is not taken
                             // into account yet

    // resistance of expiration valve and leak are in paralell
    float Ref = Re * m_Rf / (Re + m_Rf);

    // influence of blower and atm pressure on the flow (derivative of the volume)
    float P_factor = (Ref * Pbl) / (m_R * (Ref + Ri) + Ref * Ri);
    /*cout << ", Pbl=" << Pbl << ", m_R=" << m_R << ", Ri=" << Ri << ", Ref=" << Ref << ", Re=" <<
       Re
         << ", P_factor=" << m_K_flow * P_factor / 1000
         << ", cmds.inspirationValve=" << cmds.inspirationValve
         << ", cmds.expirationValve=" << cmds.expirationValve << endl;*/

    // influence of volume present in patient's lungs on the flow
    float V_factor = -m_Vp * (Ref + Ri) / (m_C * (m_R * (Ref + Ri) + Ref * Ri));

    // patient's flow
    float flow = P_factor + V_factor;

    // conputation of the new patient's lung's volume
    m_Vp += flow * dt;

    // computation of sensor data
    SensorsData output;
    output.inspiratoryPressure = m_K_pres * (flow * m_R + m_Vp / m_C);
    output.inspiratoryFlow = m_K_flow * (Pbl - output.inspiratoryPressure / m_K_pres) / Ri;
    int32_t leakFlow = m_K_flow * (Pbl - output.inspiratoryPressure / m_K_pres) / m_Rf;
    output.expiratoryFlow = m_K_flow * (output.inspiratoryPressure / m_K_pres - 0) / Re;
    if (output.inspiratoryFlow < 0) {
        cout << output.inspiratoryFlow / 1000 << "L/min";
    }
    /*cout << "volume=" << m_Vp * 1e6 << "mL, inspiratoryFlow=" << output.inspiratoryFlow / 1000
         << "L/min, expiratoryFlow=" << output.expiratoryFlow / 1000
         << "L/min, leakFlow=" << leakFlow / 1000
         << "L/min, inspiratoryPressure=" << output.inspiratoryPressure
         << "mmH2O, blowerPressre=" << Pbl * m_K_pres << "mmH2O, flow=" << flow * m_K_flow / 1000
         << "L/min, Pfactor=" << m_K_flow * P_factor / 1000
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
    if (opening_valve == 100) {
        return 1e10;
    } else {
        return ((opening_valve)*K_r + K_roffset);
    }
}