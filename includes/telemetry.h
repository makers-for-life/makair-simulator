/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file telemetry.h
 * @brief Fake: Implementation of the telemetry protocol
 *****************************************************************************/

#pragma once

#include <stdint.h>

#include "../includes/alarm_controller.h"
#include "../includes/cycle.h"

/// Current version of the telemetry protocol
#define PROTOCOL_VERSION 2u

/// Prepare Serial6 to send telemetry data
void initTelemetry(void);

/// Send a "boot" message
void sendBootMessage(void);

/// Send a "stopped" message
void sendStoppedMessage(uint8_t peakCommand,
                        uint8_t plateauCommand,
                        uint8_t peepCommand,
                        uint8_t cpmCommand,
                        uint8_t expiratoryTerm,
                        bool triggerEnabled,
                        uint8_t triggerOffset,
                        bool alarmSnoozed,
                        uint8_t cpuLoad,
                        VentilationModes ventilationMode,
                        uint8_t inspiratoryTriggerFlow,
                        uint8_t expiratoryTriggerFlow,
                        uint16_t tiMinValue,
                        uint16_t tiMaxValue,
                        uint8_t lowInspiratoryMinuteVolumeAlarmThreshold,
                        uint8_t highInspiratoryMinuteVolumeAlarmThreshold,
                        uint8_t lowExpiratoryMinuteVolumeAlarmThreshold,
                        uint8_t highExpiratoryMinuteVolumeAlarmThreshold,
                        uint8_t lowExpiratoryRateAlarmThreshold,
                        uint8_t highExpiratoryRateAlarmThreshold,
                        uint16_t targetTidalVolumeValue,
                        uint16_t lowTidalVolumeAlarmTresholdValue,
                        uint16_t highTidalVolumeAlarmTresholdValue,
                        uint16_t plateauDurationValue,
                        uint16_t leakAlarmThresholdValue);

/// Send a "data snapshot" message
void sendDataSnapshot(uint16_t centileValue,
                      int16_t pressureValue,
                      CyclePhases phase,
                      uint8_t blowerValvePosition,
                      uint8_t patientValvePosition,
                      uint8_t blowerRpm,
                      uint8_t batteryLevel,
                      int16_t inspiratoryFlowValue,
                      int16_t expiratoryFlowValue);

/// Send a "machine state snapshot" message
// cppcheck-suppress misra-c2012-2.7
void sendMachineStateSnapshot(uint32_t cycleValue,
                              uint8_t peakCommand,
                              uint8_t plateauCommand,
                              uint8_t peepCommand,
                              uint8_t cpmCommand,
                              uint16_t previousPeakPressureValue,
                              uint16_t previousPlateauPressureValue,
                              uint16_t previousPeepPressureValue,
                              uint8_t currentAlarmCodes[ALARMS_SIZE],
                              uint16_t volumeValue,
                              uint8_t expiratoryTerm,
                              bool triggerEnabled,
                              uint8_t triggerOffset,
                              uint8_t previouscpmValue,
                              bool alarmSnoozed,
                              uint8_t cpuLoad,
                              VentilationModes ventilationMode,
                              uint8_t inspiratoryTriggerFlow,
                              uint8_t expiratoryTriggerFlow,
                              uint16_t tiMinValue,
                              uint16_t tiMaxValue,
                              uint8_t lowInspiratoryMinuteVolumeAlarmThreshold,
                              uint8_t highInspiratoryMinuteVolumeAlarmThreshold,
                              uint8_t lowExpiratoryMinuteVolumeAlarmThreshold,
                              uint8_t highExpiratoryMinuteVolumeAlarmThreshold,
                              uint8_t lowExpiratoryRateAlarmThreshold,
                              uint8_t highExpiratoryRateAlarmThreshold,
                              uint16_t targetTidalVolumeValue,
                              uint16_t lowTidalVolumeAlarmTresholdValue,
                              uint16_t highTidalVolumeAlarmTresholdValue,
                              uint16_t plateauDurationValue,
                              uint16_t leakAlarmThresholdValue);

/// Send a "alarm trap" message
void sendAlarmTrap(uint16_t centileValue,
                   int16_t pressureValue,
                   CyclePhases phase,
                   uint32_t cycleValue,
                   uint8_t alarmCode,
                   AlarmPriority alarmPriority,
                   bool triggered,
                   uint32_t expectedValue,
                   uint32_t measuredValue,
                   uint32_t cyclesSinceTriggerValue);

/// Send a "control ack" message
void sendControlAck(uint8_t setting, uint16_t value);

/**
 * Convert and round a pressure in mmH2O to a pressure in cmH2O
 *
 * @param pressure A pressure in mmH2O
 * @return A pressure in cmH2O
 */
uint8_t mmH2OtoCmH2O(uint16_t pressure);
