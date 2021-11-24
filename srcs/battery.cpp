/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file battery.cpp
 * @brief Fake: Battery related functions
 *****************************************************************************/

// INCLUDES ===================================================================

// Associated header
#include "../includes/battery.h"

// Externals
#include "Arduino.h"

// Internal
#include "../includes/alarm_controller.h"
#include "../includes/debug.h"
#include "../includes/parameters.h"

// PROGRAM =====================================================================

static uint32_t rawBatteryMeanVoltage = RAW_VOLTAGE_MAINS;  // Mean battery voltage in volts
static bool isRunningOnBattery = false;

void initBattery() {
    // for (uint8_t i = 0; i < BATTERY_MAX_SAMPLES; i++) {
    //     rawBatterySample[i] = 0;
    // }

    // // Running this in setup avoids triggering alarms at startup
    // for (uint8_t i = 0; i < BATTERY_MAX_SAMPLES; i++) {
    //     updateBatterySample();
    // }
}

void updateBatterySample() {}

void updateBatteryState(__attribute__((unused)) uint32_t p_cycleNumber) {}

void batteryLoop(__attribute__((unused)) uint32_t p_cycleNumber) {
    // updateBatterySample();
    // updateBatteryState(p_cycleNumber);
}

// cppcheck-suppress unusedFunction
uint32_t getBatteryLevel() { return rawBatteryMeanVoltage * RAW_BATTERY_MULTIPLIER; }

// cppcheck-suppress unusedFunction
uint32_t getBatteryLevelX10() { return rawBatteryMeanVoltage * (10.0 * RAW_BATTERY_MULTIPLIER); }

// cppcheck-suppress unusedFunction
uint32_t getBatteryLevelX100() { return rawBatteryMeanVoltage * (100.0 * RAW_BATTERY_MULTIPLIER); }

// cppcheck-suppress unusedFunction
bool isBatteryVeryLow() {
    return rawBatteryMeanVoltage < RAW_VOLTAGE_ON_BATTERY_NOT_STARTING_THRESHOLD;
}

bool isBatteryDeepDischarged() {
    return rawBatteryMeanVoltage < RAW_VOLTAGE_ON_BATTERY_STOP_THRESHOLD;
}

// cppcheck-suppress unusedFunction
bool isMainsConnected() { return (!isRunningOnBattery); }
