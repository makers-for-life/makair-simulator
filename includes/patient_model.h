/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file patient_model.h
 * @brief Simulate the model of the patient.
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================

#include "../includes/constants.h"
#include "../includes/helper.h"

#include <cmath>
#include <cstdint>
#include <unistd.h>

/// Model
class PatientModel {
 public:
    /// Constructor
    PatientModel(float p_Rl,
                 float p_Ct,
                 float p_Cl,
                 float p_Vpatient,
                 float p_spontaneousBreathRate,
                 float p_spontaneousBreathEffort,
                 float p_spontaneousBreathDuration);

    float computeMusclePressure();

    /// Getters
    float getRl() { return m_Rl; };
    float getCl() { return m_Cl; };
    float getCt() { return m_Ct; };
    float getVpatient() { return m_Vpatient; };
    float getMusclePressure() { return computeMusclePressure(); };

    /// Setters
    void setVpatient(float p_Vpatient) { m_Vpatient = p_Vpatient; };
    void updateTimeUs(int64_t dt) {
        timestampUs += dt;
    };  // update with time increment in microseconds

    // Setters for patient characteristics
    bool setResistance(int32_t p_resistance);
    bool setCompliance(int32_t p_compliance);
    bool setSpontaneousBreathRate(int32_t p_spontaneousBreathRate);
    bool setSpontaneousBreathEffort(int32_t p_spontaneousBreathEffort);
    bool setSpontaneousBreathDuration(int32_t p_spontaneousBreathDuration);

    // Getters for patient characteristics
    int32_t getResistance() { return m_Rl; };
    int32_t getCompliance() { return m_Cl; };
    int32_t getSpontaneousBreathRate() { return m_spontaneousBreathRate; };
    int32_t getSpontaneousBreathEffort() { return m_spontaneousBreathEffort; };
    int32_t getSpontaneousBreathDuration() { return m_spontaneousBreathDuration; };

 private:
    // parameters of the patient
    float m_Rl;        // resistance of the patient lung in Pa.(m.s-1)-1
    float m_Cl;        // compilance of the patient Lung in m3.Pa-1
    float m_Ct;        // compilance of the circuit Tubing in m3.Pa-1
    float m_Vpatient;  // Volume of air in the lungs of the patient in m3

    // Paramter of spontaneous breathing
    float m_spontaneousBreathRate;      // spontaneous breath rate in cycle/s
    float m_spontaneousBreathEffort;    // spontaneous breath effort in Pa
    float m_spontaneousBreathDuration;  // duration of the effort of spontaneous breath rate in s.

    int64_t timestampUs;  // time since starting in ms
};
