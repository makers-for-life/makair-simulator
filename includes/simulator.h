/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simulator.h
 * @brief Simulator for the makair controller.
 *****************************************************************************/
#pragma once

// CONSTANTS ==================================================================

#define SIMULATOR true

// INCLUDES ===================================================================

#include "../includes/helper.h"
#include "../includes/model.h"
#include "../includes/simu_state_machine.h"
#include "../includes/logger.h"

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
    Logger m_logger;

    ActuatorsData m_cmds;
    SensorsData m_sensors;
    float m_dt;

};