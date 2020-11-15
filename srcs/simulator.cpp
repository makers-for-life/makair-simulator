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
  : m_dt(0.1)
{}

void Simulator::run(){

    init();

    while (m_state_machine.isRunning())
        loop();

    m_logger.close();
}

void Simulator::init(){
    m_model.init();
    m_state_machine.init();
    m_logger.init();
    m_time = 0.;
    m_dt = 0.1;
}

void Simulator::loop(){
    m_sensors = m_model.compute(m_cmds, m_dt);
    m_cmds = m_state_machine.compute(m_sensors, m_dt, m_time);
    m_logger.write_log(m_time, m_cmds, m_sensors);
    m_time += m_dt;
    
}
