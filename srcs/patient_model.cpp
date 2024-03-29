/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simulator.cpp
 * @brief Simulator for the makair controller.
 *****************************************************************************/

// INCLUDES ===================================================================

#include "../includes/patient_model.h"
#include "../includes/Arduino.h"

// FUNCTIONS ==================================================================

/**
 * Initiate a patient Model, and convert units in internationa standard unit.
 *
 * @param p_Rl in cmH2O/mL/s
 * @param p_Ct in mL/cmH2O
 * @param p_Cl in mL/cmH2O
 * @param p_Vpatient in mL
 * @param p_spontaneousBreathRate in cycle/min
 * @param p_spontaneousBreathEffort in cmH2O
 * @param p_spontaneousBreathDuration in ms
 */
PatientModel::PatientModel(float p_Rl,
                           float p_Ct,
                           float p_Cl,
                           float p_Vpatient,
                           float p_spontaneousBreathRate,
                           float p_spontaneousBreathEffort,
                           float p_spontaneousBreathDuration) {

    // parameters of the patient
    m_Rl = p_Rl * 98.0665 / (1e-3);  // resistance of the patient lung in Pa.(m.s-1)-1
    m_Cl = p_Cl * 1e-6 / 98.0665;    // compilance of the patient Lung in m3.Pa-1
    m_Ct = p_Ct * 1e-6 / 98.0665;    // compilance of the circuit Tubing in m3.Pa-1
    m_Vpatient = p_Vpatient * 1e-6;  // Volume of air in the lungs of the patient in m3

    // Parameter of spontaneous breathing
    m_spontaneousBreathRate = p_spontaneousBreathRate / 60.0;  // spontaneous breath rate in cycle/s
    m_spontaneousBreathEffort =
        p_spontaneousBreathEffort * 98.0665;  // spontaneous breath effort in Pa
    m_spontaneousBreathDuration =
        p_spontaneousBreathDuration
        / 1000.0;     // duration of the effort of spontaneous breath rate in s.
    timestampUs = 0;  // time since starting in ms
}

/**
 * Compute pressure given by the patient muscles
 * Pressure is a sinusoidal effort during m_spontaneousBreathDuration, with an amplitude of
 * m_spontaneousBreathEffort
 *
 * @return musclePressure in Pa
 *
 */

float PatientModel::computeMusclePressure() {

    int64_t breathDurationMs = int64_t(1000.0 / m_spontaneousBreathRate);
    int64_t effortDurationMs = 1000 * m_spontaneousBreathDuration;
    int64_t timestampMs = timestampUs / 1000;

    float musclePressure;

    if (timestampMs % breathDurationMs <= effortDurationMs
        && (effortDurationMs != 0
            && breathDurationMs != 0)) {  // first condition to check if now is during spontaneous
                                          // effort, second condition to avoid 0 division

        float effortPogress = float(timestampMs % breathDurationMs) / float(effortDurationMs);

        musclePressure = m_spontaneousBreathEffort * sin(M_PI * effortPogress);
        // cout << musclePressure << endl;
    } else {

        musclePressure = 0.0;
    }

    return musclePressure;
}

// Setters for patient characteristics
bool PatientModel::setResistance(int32_t p_resistance) {
    if (p_resistance >= MIN_RESISTANCE_VALUE && p_resistance <= MAX_RESISTANCE_VALUE) {
        m_Rl = p_resistance;
        return true;
    } else {
        cout << "Error : Resistance should be between " << MIN_RESISTANCE_VALUE << " and "
             << MAX_RESISTANCE_VALUE << " cmh2O/L/s" << endl;
        return false;
    }
}

bool PatientModel::setCompliance(int32_t p_compliance) {
    if (p_compliance >= MIN_COMPLIANCE_VALUE && p_compliance <= 150) {
        m_Cl = p_compliance;
        return true;
    } else {
        cout << "Error : compliance should be between " << MIN_COMPLIANCE_VALUE << " and "
             << MAX_COMPLIANCE_VALUE << " mL/cmH2O" << endl;
        return false;
    }
}
bool PatientModel::setSpontaneousBreathRate(int32_t p_spontaneousBreathRate) {
    if (p_spontaneousBreathRate >= MIN_SPONTANEOUS_BREATH_RATE_VALUE
        && p_spontaneousBreathRate <= MAX_SPONTANEOUS_BREATH_RATE_VALUE) {
        m_spontaneousBreathRate = p_spontaneousBreathRate;
        return true;
    } else {
        cout << "Error : spontaneousBreathRate should be between "
             << MIN_SPONTANEOUS_BREATH_RATE_VALUE << " and " << MAX_SPONTANEOUS_BREATH_RATE_VALUE
             << " cycle/min" << endl;
        return false;
    }
}
bool PatientModel::setSpontaneousBreathEffort(int32_t p_spontaneousBreathEffort) {
    if (p_spontaneousBreathEffort >= MIN_SPONTANEOUS_BREATH_EFFORT_VALUE
        && p_spontaneousBreathEffort <= MAX_SPONTANEOUS_BREATH_EFFORT_VALUE) {
        m_spontaneousBreathEffort = p_spontaneousBreathEffort;
        return true;
    } else {
        cout << "Error : spontaneousBreathEffort should be between "
             << MIN_SPONTANEOUS_BREATH_EFFORT_VALUE << " and "
             << MAX_SPONTANEOUS_BREATH_EFFORT_VALUE << " cmh2O" << endl;
        return false;
    }
}
bool PatientModel::setSpontaneousBreathDuration(int32_t p_spontaneousBreathDuration) {
    if (p_spontaneousBreathDuration >= MIN_SPONTANEOUS_BREATH_DURATION_VALUE
        && p_spontaneousBreathDuration <= MAX_SPONTANEOUS_BREATH_DURATION_VALUE) {
        m_spontaneousBreathDuration = p_spontaneousBreathDuration;
        return true;
    } else {
        cout << "Error : spontaneousBreathDuration should be between "
             << MIN_SPONTANEOUS_BREATH_DURATION_VALUE << " and "
             << MAX_SPONTANEOUS_BREATH_DURATION_VALUE << " ms" << endl;
        return false;
    }
}