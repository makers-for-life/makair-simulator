/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file pc_bipap_controller.h
 * @brief PID for BIPAP pressure control
 *****************************************************************************/

#pragma once

#include "../includes/config.h"
#include "../includes/parameters.h"
#include "../includes/ventilation_controller.h"

/// Controller for the BIPAP mode
class PC_BIPAP_Controller final : public VentilationController {
 public:
    /// Default constructor
    PC_BIPAP_Controller();

    /// Initialize controller
    void setup() override;

    /// Begin a new breathing cycle
    void initCycle() override;

    /// Control the inhalation
    void inhale() override;

    /// Control the exhalation
    void exhale() override;

    /// End the current breathing cycle
    void endCycle() override;

 private:
    /// Determine the blower speed to adopt for next cycle
    void calculateBlowerIncrement();

    /// Number of ticks from which it is possible to trigger a new inspiration
    uint16_t m_triggerWindow;

    /// True if plateau pressure has been reached (but not necessarily converged)
    bool m_plateauPressureReached;

    /**
     * PID to control the blower valve during some specific steps of the cycle
     *
     * @param targetPressure The pressure we want (in mmH2O)
     * @param currentPressure The pressure measured by the sensor (in mmH2O)
     * @param dt Time since the last computation (in microsecond)
     */
    int32_t PCinspiratoryPID(int32_t targetPressure, int32_t currentPressure, int32_t dt);

    /**
     * PID to control the patient valve during some specific steps of the cycle
     *
     * @param targetPressure The pressure we want (in mmH2O)
     * @param currentPressure The pressure measured by the sensor (in mmH2O)
     * @param dt Time since the last computation (in microsecond)
     */
    int32_t PCexpiratoryPID(int32_t targetPressure, int32_t currentPressure, int32_t dt);

    /// Current blower speed
    uint16_t m_blowerSpeed;

    /// True if we want to reopen the inspiratory valve to create a circulation flow able to detect
    /// inspiratory flow trigger
    bool m_reOpenInspiratoryValve;

    /// Slope of the inspiration open loop (in mmH2O/s)
    int32_t m_inspiratorySlope;

    /// Current blower speed increment (to apply at the beginning of the next cycle)
    int32_t m_blowerIncrement;

    /// Error of the last computation of the blower PID
    int32_t m_inspiratoryPidIntegral;

    /// Error of the last computation of the blower PID
    int32_t m_inspiratoryPidLastError;

    /// Fast mode at start of expiration
    bool m_expiratoryPidFastMode;

    /// Fast mode at start of inspiration
    bool m_inspiratoryPidFastMode;

    /// Integral gain of the patient PID
    int32_t m_expiratoryPidIntegral;

    /// Last aperture of the blower valve
    int32_t m_inspiratoryValveLastAperture;

    /// Last aperture of the blower valve
    int32_t m_expiratoryValveLastAperture;

    /// Error of the last computation of the PID
    int32_t m_expiratoryPidLastError;

    /// Last errors in inspiratory PID
    int32_t m_inspiratoryPidLastErrors[PC_NUMBER_OF_SAMPLE_DERIVATIVE_MOVING_MEAN];

    /// Last error index in inspiratory PID
    int32_t m_inspiratoryPidLastErrorsIndex;

    /// Last errors in expiratory PID
    int32_t m_expiratoryPidLastErrors[PC_NUMBER_OF_SAMPLE_DERIVATIVE_MOVING_MEAN];

    /// Last error index in expiratory PID
    int32_t m_expiratoryPidLastErrorsIndex;

    /// Max flow during inspiration
    int32_t m_maxInspiratoryFlow;
};

extern PC_BIPAP_Controller pcBipapController;
