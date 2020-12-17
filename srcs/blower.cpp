/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file blower.cpp
 * @brief Fake: Tools to control the blower
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/blower.h"

// Internal libraries
#include "../includes/parameters.h"

// INITIALISATION =============================================================

Blower blower;

// FUNCTIONS ==================================================================

Blower::Blower() {}

Blower::Blower(HardwareTimer* p_hardwareTimer, uint16_t p_timerChannel, uint16_t p_blowerPin) {
    actuator = p_hardwareTimer;
    timerChannel = p_timerChannel;
    blowerPin = p_blowerPin;
    m_stopped = true;
    m_speed = DEFAULT_BLOWER_SPEED;
}

void Blower::setup() {
    // nothing to do for a simulation
}

void Blower::runSpeed(uint16_t p_speed) {
    if ((p_speed >= MIN_BLOWER_SPEED) && (p_speed <= MAX_BLOWER_SPEED)) {
        if (m_stopped || (m_speed != p_speed)) {
            m_speed = p_speed;
            m_stopped = false;
        }
    } else {
        if (m_stopped) {
            this->runSpeed(m_speed);
        }
    }
}

int32_t Blower::getBlowerPressure(int32_t p_flow) {
    int32_t returnValue;
    // For now the blower has only been characterize at max speed
    if (m_speed == MAX_BLOWER_SPEED) {
        // This order 2 characteruzation has been made experimentally
        returnValue = 703 - 281 * p_flow / 100000 - 832 * (p_flow / 100) * (p_flow / 100) / 1000000;
    } else {
        returnValue = 0;
    }

    return returnValue;
}

uint16_t Blower::getSpeed() const { return m_speed; }

void Blower::stop() {
    m_stopped = true;
    m_speed = DEFAULT_BLOWER_SPEED;
}
