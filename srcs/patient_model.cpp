/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simulator.cpp
 * @brief Simulator for the makair controller.
 *****************************************************************************/

// INCLUDES ===================================================================

#include "../includes/patient_model.h"
#include "../includes/Arduino.h"

#include <unistd.h>

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
        / 1000.0;  // duration of the effort of spontaneous breath rate in s.
}
