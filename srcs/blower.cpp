/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file blower.cpp
 * @brief Tools to control the blower
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

Blower::Blower() {
    // TODO
}

void Blower::setup() {
    // TODO
}

void Blower::runSpeed(uint16_t p_speed) {
    // TODO
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
    // TODO
}
