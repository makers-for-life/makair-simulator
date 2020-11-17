/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simulator.h
 * @brief Simulator for the makair controller.
 *****************************************************************************/
#pragma once

// CONSTANTS ==================================================================

// #define SIMULATION true

// INCLUDES ===================================================================

#include <cstdint>
#include "../includes/helper.h"
#include "../includes/model.h"
#include "../includes/simu_state_machine.h"

/// Simulator
class Simulator {
 public:
    /// Default constructor
    Simulator();

    void run();

 private:

    void init();

    void loop();

    Model m_model;
    SimuStateMachine m_state_machine;
    // TODO: Logger m_logger;

    ActuatorsData m_cmds;
    SensorsData m_sensors;
    float m_dt;

};