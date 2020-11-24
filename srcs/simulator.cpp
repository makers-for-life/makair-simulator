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
    m_cmds.blower=0;
    m_cmds.expirationValve=0;
    m_cmds.inspirationValve=0;
    m_time = 0.;
    m_dt = 0.001; //10ms
}

void Simulator::loop(){
    //running model
    m_sensors = m_model.compute(m_cmds, m_dt);

    //computing the commands in function of the state of the model. Here, open loop
    m_cmds = m_state_machine.compute(m_sensors, m_dt, m_time);

    //Writing the results in a csv file
    m_logger.write_log(m_time, m_cmds, m_sensors);

    //moving to the next time step
    m_time += m_dt;
    
}
