/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file model.h
 * @brief Simulate the model of the respirator and the patient.
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================

#include "../includes/helper.h"
#include <cstdint>
#include <unistd.h>
#define PREVIOUS_INSPIRATORY_FLOW_MOVING_MEAN_SIZE 1
#define PREVIOUS_VALVE_POSITION_MOVING_MEAN_SIZE 1
#define P_FACTOR_MOVING_MEAN_SIZE 1

/// Model
class Model {
 public:
    /// Default constructor
    Model();

    void init(int32_t p_resistance, int32_t p_compliance);

    SensorsData compute(ActuatorsData cmds, float dt);

 private:
    // parameters of the patient
    float m_Rf;  // resistance of leaking in Pa.(m.s-1)-1
    float m_R;   // resistance of the patient in Pa.(m.s-1)-1
    float m_C;   // compilance of the patient in m3.Pa-1
    float m_Vp;  // Volume of air in the lungs of the patient above rest volume in m3

    // parameters of the actuators
    float m_Kr;        // coefficient of resistance in Pa.(m.s-1)-1 / %
    float m_Kroffset;  // coefficient of resistance in Pa.(m.s-1)-1
    float m_K_blower;  // coefficient of blower pressure in Pa / %

    // parameters of the sensors
    float m_K_pres;  // mmH2O / Pa
    float m_K_flow;  // ml / m3
    int32_t m_previousInspiratoryFlowMean;
    int32_t m_previousInspiratoryFlowMeanMovingMean[PREVIOUS_INSPIRATORY_FLOW_MOVING_MEAN_SIZE];
    int32_t m_previousInspiratoryFlowMeanMovingMeanIndex;
    int32_t
        m_previousInspiratoryValvePositionMeanMovingMean[PREVIOUS_VALVE_POSITION_MOVING_MEAN_SIZE];
    int32_t m_previousInspiratoryValvePositionMeanMovingMeanIndex;
    int32_t
        m_previousExpiratoryValvePositionMeanMovingMean[PREVIOUS_VALVE_POSITION_MOVING_MEAN_SIZE];
    int32_t m_previousExpiratoryValvePositionMeanMovingMeanIndex;
    float m_pFactorMeanMovingMean[P_FACTOR_MOVING_MEAN_SIZE];
    int32_t m_pFactorMeanMovingMeanIndex;
    int32_t m_previousInspiratoryValvePositionMean;
    int32_t m_previousExpiratoryValvePositionMean;
    float previousPbl;
    float m_previousPresp;
};

float res_valves(int opening_valve, float K_r, float K_roffset);