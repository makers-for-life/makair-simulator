/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simu_state_machine.h
 * @brief Manage the state machine of the simulator.
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================

#include "../includes/helper.h"

/// Simulation state machine
class SimuStateMachine {
 public:
    /// Default constructor
    SimuStateMachine();

    void init();

    ActuatorsData compute(SensorsData sensors, int dt);

    bool isRunning();


  private:
    bool m_running;
};