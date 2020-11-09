/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file helper.h
 * @brief Functions and structures to help making the simulator.
 *****************************************************************************/
#pragma once

// STRUCTURES =================================================================

struct SensorsData {
    int inspirationFlow = -1;
    int inspirationPressure = -1;
    int expirationPressure = -1;
};

struct ActuatorsData {
    int blower = -1;
    int inspirationValve = -1;
    int expirationValve = -1;
};

// FUNCTIONS ==================================================================
