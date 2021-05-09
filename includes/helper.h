/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file helper.h
 * @brief Functions and structures to help making the simulator.
 *****************************************************************************/
#pragma once

// STRUCTURES =================================================================

struct SensorsData {
    int inspiratoryFlow = -1;
    int expiratoryFlow = -1;
    int inspiratoryPressure = -1;
};

struct ActuatorsData {  // in % of max value
    int blower = -1;
    int inspirationValve = -1;
    int expirationValve = -1;
};

// FUNCTIONS ==================================================================
