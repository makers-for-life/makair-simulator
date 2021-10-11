/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file model.h
 * @brief Simulate the model of the respirator and the patient.
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================

#include "../includes/helper.h"
#include "../includes/patient_model.h"

#include <cstdint>
#include <unistd.h>
#define PREVIOUS_VALVE_POSITION_TABLE_SIZE 200

/// Model
class Model {
 public:
    /// Default constructor
    Model();

    void init(PatientModel& p_patientModel);

    SensorsData compute(ActuatorsData cmds, float dt);

 private:
    // Patient model containing of datas (resistance, compliance, etc...)
    PatientModel* m_patientModel;

    // parameters of the circuit
    float m_Vcircuit;  // Volume of air in the circuit in m3

    // parameters of the sensors
    float m_K_pres = 1e-1;      // mmH2O / Pa
    float m_K_flow = 60 * 1e6;  // ml / m3

    int32_t m_previousInspiratoryValvePositionLastValues[PREVIOUS_VALVE_POSITION_TABLE_SIZE];
    int32_t m_previousInspiratoryValvePositionLastValuesIndex;
    int32_t m_previousExpiratoryValvePositionLastValues[PREVIOUS_VALVE_POSITION_TABLE_SIZE];
    int32_t m_previousExpiratoryValvePositionLastValuesIndex;
    float m_previousPbl;
    float m_previousInspiratoryFlow;
    float m_previousVentilatorPressure;
    float m_previousPreviousVentilatorPressure;
    float m_previousVentilatorFlow;
    float m_previousMusclePressure;
    float m_musclePressure;
};
