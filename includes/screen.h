/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file screen.h
 * @brief Display and LCD screen related functions
 *
 * This relies on the LiquidCrystal library (https://github.com/arduino-libraries/LiquidCrystal).
 * LCD screen must have 4 lines of 20 characters.
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// External
//#include <LiquidCrystal.h>

// Internal
#include "../includes/parameters.h"

// INITIALISATION =============================================================

/// Instance of the screen controller
// extern LiquidCrystal screen;

// FUNCTIONS ==================================================================

/**
 * Start the screen
 *
 * @warning It must be called once to be able to use the screen
 */
void startScreen() {}

/// Erase everything that is on the screen
void resetScreen() {}

/**
 * Display the current step of the breathing
 *
 * @param pressure            The current pressure [mmH2O]
 * @param cyclesPerMinute     Next desired number of cycles per minute
 */
void displayCurrentPressure(__attribute__((unused)) uint16_t pressure,
                            __attribute__((unused)) uint16_t cyclesPerMinute) {}

/**
 * Display the current injected air volume
 *
 * @param volumeMassFlow      The number of liter breathed in this cycle
 * @param cyclesPerMinute     Next desired number of cycles per minute
 */
void displayCurrentVolume(__attribute__((unused)) int32_t volumeMassFlow,
                          __attribute__((unused)) uint16_t cyclesPerMinute) {}

/**
 * Display the current settings
 *
 * @param peakPressureMax      PeakPressureMax [mmH2O]
 * @param plateauPressureMax   Next maximal plateau pressure [mmH2O]
 * @param peepMin              Next desired Positive End Expiratory Pressure (PEEP) [mmH2O]
 */
void displayCurrentSettings(__attribute__((unused)) uint16_t peakPressureMax,
                            __attribute__((unused)) uint16_t plateauPressureMax,
                            __attribute__((unused)) uint16_t peepMin) {}

/**
 * Display relevant values from the ongoing cycle
 *
 * @param peakPressure     The peak pressure [mmH2O]
 * @param plateauPressure  The plateau pressure [mmH2O]
 * @param peep             The Positive End Expiratory Pressure (PEEP) [mmH2O]
 */
void displayCurrentInformation(__attribute__((unused)) uint16_t peakPressure,
                               __attribute__((unused)) uint16_t plateauPressure,
                               __attribute__((unused)) uint16_t peep) {}

/**
 * Display triggered alarm codes
 *
 * @param p_alarmCodes          List of alarm codes to display
 * @param p_nbTriggeredAlarms   Number of triggered alarms
 */
void displayAlarmInformation(__attribute__((unused)) uint8_t p_alarmCodes[],
                             __attribute__((unused)) uint8_t p_nbTriggeredAlarms) {}

/**
 * Display error when pressure offset is unstable
 *
 * @param p_minOffsetValue Minimum measured offset value
 * @param p_maxOffsetValue Maximum measured offset value
 */
void displayPressureOffsetUnstable(__attribute__((unused)) uint32_t p_minOffsetValue,
                                   __attribute__((unused)) uint32_t p_maxOffsetValue) {}

/**
 * Display pressure offset
 *
 * @param p_inspiratoryPressureSensorOffset Measured offset value
 */
void displayPressureOffset(__attribute__((unused)) int32_t p_inspiratoryPressureSensorOffset) {}

/**
 * Display error when flow meter fails
 *
 * @param p_flowMeterFlowAtStarting Measured flow offset value
 * @param p_flowMeterFlowWithBlowerOn Maximum measured flow offset value
 */
void displayFlowMeterFail(__attribute__((unused)) int32_t p_flowMeterFlowAtStarting,
                          __attribute__((unused)) int32_t p_flowMeterFlowWithBlowerOn) {}

/**
 * Display flow meter offset
 *
 * @param p_flowMeterFlowOffset Measured flow offset value
 */
void displayFlowMeterOffset(__attribute__((unused)) int32_t p_flowMeterFlowOffset) {}

/// Display the "calibration in progress" message
void displayPatientMustBeUnplugged() {}

/// Show that EOL mode was triggered
void displayEndOfLineTestMode() {}

/// Display error when battery level is too discharged
void displayBatteryDeepDischarge() {}

/// Display error when machine was restarted by watchdog
void displayWatchdogError() {}

/**
 * Display a message when the machine is stopped
 */
void displayMachineStopped() {}

/// Force clear the alarm display cache
void clearAlarmDisplayCache() {}

/**
 * Convert and round a pressure value
 *
 * @param pressure Pressure in mmH2O
 * @return Rounded pressure in cmH2O
 */
uint16_t convertAndRound(__attribute__((unused)) uint16_t pressure) { return 0u; }
