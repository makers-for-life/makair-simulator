/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file model.h
 * @brief Simulate the model of the respirator and the patient.
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================

#include "../includes/helper.h"

/// Model
class Model {
  public:
    /// Default constructor
    Model();

    void init();

    SensorsData compute(ActuatorsData cmds, int dt);

  private:
    
    //parameters of the patient
    float m_Rf; // resistance of leaking in Pa.(m.s-1)-1
    float m_R; // resistance of the patient in Pa.(m.s-1)-1
    float m_C; // compilance of the patient in m3.Pa-1
    float m_Vp; // Volume of air in the lungs of the patient above rest volume in m3 

    //parameters of the actuators
    float m_Kr; // coefficient of resistance in Pa.(m.s-1)-1 / %
    float m_K_blower; // coefficient of blower pressure in Pa / %

    //parameters of the sensors
    float m_K_pres; // mmH2O / Pa
    float m_K_flow; // ml / m3

};


float res_valves(int opening_valve, float K_r);