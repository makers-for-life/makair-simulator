/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file pressure_valve.cpp
 * @brief Fake: Tools to control Pressure Valves
 *****************************************************************************/

#pragma once

// INCLUDES ===================================================================

// Associated header
#include "../includes/pressure_valve.h"

// External libraries
#include <algorithm>

// Internal libraries
#include "../includes/parameters.h"

// INITIALISATION =============================================================

PressureValve expiratoryValve;
PressureValve inspiratoryValve;

// FUNCTIONS ==================================================================

PressureValve::PressureValve() {}

PressureValve::PressureValve(uint16_t p_openApertureAngle,
                             uint16_t p_closeApertureAngle) {
    openApertureAngle = p_openApertureAngle;
    closeApertureAngle = p_closeApertureAngle;
    minApertureAngle = min(p_closeApertureAngle, p_openApertureAngle);
    maxApertureAngle = max(p_closeApertureAngle, p_openApertureAngle);
    command = p_closeApertureAngle;
    position = -1;
    positionLinear = 0;
}

void PressureValve::setup() {
    // TODO
}

void PressureValve::open() { command = openApertureAngle; }

void PressureValve::close() { command = closeApertureAngle; }

void PressureValve::open(uint16_t p_command) { command = p_command; }

/**
 * Request opening of the Pressure Valve with a given section (in mm^2)
 *
 * @param p_section The section to open the valve (in mm^2 multiplied by 100)
 */
void PressureValve::openSection(int32_t p_sectionMultiplyBy100) {
    int32_t tempCommand;
    if (p_sectionMultiplyBy100 < 1960) {
        tempCommand = 98 - 318 * p_sectionMultiplyBy100 / 10000;
    } else {
        tempCommand = 2626 - (36 * p_sectionMultiplyBy100) / 10 + 168 * ((p_sectionMultiplyBy100 * p_sectionMultiplyBy100 )/ 100) / 1000
                      - 264 * (((p_sectionMultiplyBy100 * p_sectionMultiplyBy100) / 100) * (p_sectionMultiplyBy100)/100) / 100000;
    }
    command = min(max(int32_t(minApertureAngle), tempCommand), int32_t(maxApertureAngle));
    /*Serial.print(command);
    Serial.print(",");*/
}

// Linearization has been made experimentaly
uint16_t PressureValve::openLinear(uint16_t p_command) {
    positionLinear = p_command;
    // The p_command is in [ 0 ; 125 ], but the valve is only effective in [30; 100]
    // So lets make it between 30 and 100
    // A x10 multiplier is used here for better precision with integer calculations
    // The value of intermediateValue will consequently be in [300 ; 1000]
    uint32_t intermediateValue = (((static_cast<uint32_t>(p_command) * 75u) / 125u) + 30u) * 10u;

    /* An order 3 polynom is used to correct the non linearity of the valve.
    To find this polynom, the following experimental protocol was used:
        - Run the blower max speed during the full experience
        - Plug inspiratory output on expiratory input
        - For each openning value of the valve, wait 5s for stabilisation and measure Flow

    This gives a csv with:
        openningValue[0;125], flow (mL/min)
        0, 48137
        10, 47780
        ...
        125, 127

    Then we calculated for each openning value the "linear target flow" with a basic linear
    equation:
        openningValue[0;125], flow (mL/min), targetFlow (mL/min)
        0, 48137,48137
        10, 47780, 44296
        ...
        125, 127, 0

    Then we manually found for each openning value, the openning value that should have been
    used to reach the targetFlow: openningValue[0;125], flow (mL/min), targetFlow (mL/min),
        correctedOpenningValue[0;125] 0, 48137,48137, 30 10, 47780, 44296, 55.2
        ...
        125, 127, 0, 100

    Then when made an order 3 regression between correctedOpenningValue and openningValue. This
    is the relation below:
    */
    command = (uint16_t)(
        (((76u * intermediateValue) / 10u)
         - (((985u * intermediateValue) * intermediateValue) / 100000u)
         + (((((44u * intermediateValue) * intermediateValue) / 1000u) * intermediateValue)
            / 10000u)
         - 1140u)
        / 10u);

    return command;
}

uint16_t valveAngle2MicroSeconds(uint16_t value) {
    // Faulhaber motors works with PWM
    // TODO
    return 0;
}
