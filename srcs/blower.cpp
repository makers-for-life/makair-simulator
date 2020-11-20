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

uint16_t Blower::getSpeed() const { return m_speed; }

void Blower::stop() {
    // TODO
}
