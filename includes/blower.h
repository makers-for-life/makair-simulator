/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file blower.h
 * @brief Fake: Tools to control the blower
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Internal libraries
#include "../includes/debug.h"
#include "../includes/parameters.h"

/**
 * Convert a speed to a value in microseconds for the blower controller
 *
 * @note There is an error margin on the max ppm, because the blower do not handle values greater
 * than 2.01 ms, and there is no quartz anywhere
 */
#define BlowerSpeed2MicroSeconds(value) map(value, 0, 1800, 1000, 1950)

// CLASS =================================================================

/// Controls a blower
class Blower {
 public:
    /// Default constructor
    Blower();

    /// Initialize the hardware timer used to control the blower
    void setup();

    /**
     * Run the blower to a given speed
     *
     * @param p_speed Speed between MIN_BLOWER_SPEED and MAX_BLOWER_SPEED
     */
    void runSpeed(uint16_t p_speed);

    /// Stops the blower
    void stop();

    /// Get speed value
    uint16_t getSpeed() const;

    /**
     * Given a flow in mL/min, return an estimated pressure just at the output of the blower. This
     * pressure has been determined using the pressure-flow characteristic of the blower
     *
     * @param p_flow inspiratory flow in mL/min
     */
    int32_t getBlowerPressure(int32_t p_flow);

 private:
    /// Hardware timer used to control the blower
   //  HardwareTimer* actuator;

    /// Channel of the hardware timer used to control the blower
    uint16_t timerChannel;

    /// Pin of the blower
    uint16_t blowerPin;

    /// Current speed
    uint16_t m_speed;

    /// Current state
    bool m_stopped;
};

extern Blower blower;
