/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simu_state_machine.h
 * @brief Manage the state machine of the simulator.
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================

#include "../includes/helper.h"
#include "../includes/main_controller.h"

enum SimuState {SETUP, STOPPED, INIT_CYCLE, BREATH, TRIGGED_RAISED, END_CYCLE};

/// Simulation state machine
class SimuStateMachine {
  public:
    /// Default constructor
    SimuStateMachine();

    void init(int max_cycle);

    ActuatorsData compute(SensorsData sensors, float dt_s);

    bool isRunning();


  private:
    void updateTime(int dt);
    int getTime();
    void updateVolume(int flow, int dt);
    int getVolume();
    void resetVolume();
    bool shouldStop();

    bool m_running;
    SimuState m_state;
    int m_time;
    int m_cycle_start_time;
    int m_volume;

    int m_cycle_uncount;
};