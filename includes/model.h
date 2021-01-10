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
#include "../includes/Arduino.h"
#define PREVIOUS_VALVE_POSITION_TABLE_SIZE 100

/// Model
class Model {
 public:
    /// Default constructor
    Model();

    void init(int32_t p_resistance, int32_t p_compliance, int32_t p_inertance);

    SensorsData compute(ActuatorsData cmds, float dt);

 private:
    // parameters of the patient
    float m_Rf;        // resistance of leaking in Pa.(m.s-1)-1
    float m_R;         // resistance of the patient in Pa.(m.s-1)-1
    float m_C;         // compilance of the patient in m3.Pa-1
    float m_Ce;        // compilance of the circuit in m3.Pa-1
    float m_Vpatient;  // Volume of air in the lungs of the patient above rest volume in m3
    float m_Vcircuit;  // Volume of air in the circuit in m3

    // parameters of the sensors
    float m_K_pres;  // mmH2O / Pa
    float m_K_flow;  // ml / m3

    int32_t m_previousInspiratoryValvePositionLastValues[PREVIOUS_VALVE_POSITION_TABLE_SIZE];
    int32_t m_previousInspiratoryValvePositionLastValuesIndex;
    int32_t m_previousExpiratoryValvePositionLastValues[PREVIOUS_VALVE_POSITION_TABLE_SIZE];
    int32_t m_previousExpiratoryValvePositionLastValuesIndex;
    float m_previousPbl;
    float m_previousInspiratoryFlow;
    float m_previousVentilatorPressure;
    float m_previousPreviousVentilatorPressure;
    float m_previousVentilatorFlow;
};
