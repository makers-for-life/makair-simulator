/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simu_state_machine.cpp
 * @brief Manage the state machine of the simulator.
 *****************************************************************************/

// INCLUDES ===================================================================

#include "../includes/simu_state_machine.h"

// FUNCTIONS ==================================================================

SimuStateMachine::SimuStateMachine()
  : m_running(true)
{}

void SimuStateMachine::init() {}

ActuatorsData SimuStateMachine::compute(SensorsData cmds, int dt){}

bool SimuStateMachine::isRunning(){
    return m_running;
}

