/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simulator.cpp
 * @brief Simulator for the makair controller.
 *****************************************************************************/

// INCLUDES ===================================================================

#include "../includes/simulator.h"

// FUNCTIONS ==================================================================

Simulator::Simulator()
  : m_dt(0.01) // 10 ms
{}

void Simulator::run(){

    init();

    while (m_state_machine.isRunning())
        loop();
}

void Simulator::init(){
    m_model.init();
    m_state_machine.init();
}

void Simulator::loop(){
    m_sensors = m_model.compute(m_cmds, m_dt);
    m_cmds = m_state_machine.compute(m_sensors, m_dt);
}
