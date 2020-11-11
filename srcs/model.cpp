/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file model.cpp
 * @brief Simulate the model of the respirator and the patient.
 *****************************************************************************/

// INCLUDES ===================================================================

#include "../includes/model.h"

// FUNCTIONS ==================================================================

Model::Model() {}

void Model::init() {
    //parameters of the patient
    float m_Rf = 1e6; // resistance of leaking in Pa.(m.s-1)-1
    float m_R = 5e3; // resistance of the patient in Pa.(m.s-1)-1
    float m_C = 100e-9; // compilance of the patient in m3.Pa-1
    float m_Vp = 0.; // Volume of air in the lungs of the patient above rest volume in m3 

    //parameters of the actuators
    float m_Kr = 1e3; // coefficient of resistance in Pa.(m.s-1)-1 / %
    float m_K_blower = 1e3; // coefficient of blower pressure in Pa / %

    //parameters of the sensors
    float m_K_pres = 1e2; // mmH2O / Pa
    float m_K_flow = 1e6; // ml / m3
}

SensorsData Model::compute(ActuatorsData cmds, int dt){

    // Conversion of actuators command into physical parameters of the model
    float Re = res_valves(cmds.expirationValve, m_Kr);
    float Ri = res_valves(cmds.inspirationValve, m_Kr);
    float Pbl = m_K_blower * cmds.blower; // dynamic of the blower is not taken into account yet

    // resistance of expiration valve and leak are in paralell
    float Ref = Re * m_Rf / (Re + m_Rf); 

    // influence of blower and atm pressure on the flow (derivative of the volume)
    float P_factor = (Ref * Pbl) / (m_R * (Ref + Ri) + Ref * Ri);

    // influence of vomule present in patient's lungs on the flow
    float V_factor = -m_Vp * (Ref + Ri) / (m_C * (m_R * (Ref + Ri) + Ref * Ri));

    // patient's flow
    float flow = P_factor + V_factor; 

    // conputation of the new patient's lung's volume
    m_Vp + flow * dt;

    // computation of sensor data
    SensorsData output;
    output.inspirationPressure = flow * m_R + m_Vp / m_C;
    output.inspirationFlow = m_K_flow * (Pbl - output.inspirationPressure) / Ri;

    return(output);
}

/**
 * function thats compute the resistance of the valves depending of the 
 * openning. Here the relation is linear, but it can be upgraded to a more
 * realistic model
 * @opening_valve in % of the maximum opening
 * @K_r the coefficient of resistance in Pa.(m.s-1)-1 / %
 */
float res_valves(int opening_valve, float K_r){
    return((100 - opening_valve)*K_r);
}