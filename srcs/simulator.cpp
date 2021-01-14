/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simulator.cpp
 * @brief Simulator for the makair controller.
 *****************************************************************************/

// INCLUDES ===================================================================

#include "../includes/simulator.h"
#include "../includes/Arduino.h"
#include "../includes/serial_control.h"

#include <unistd.h>

// FUNCTIONS ==================================================================

Simulator::Simulator()
    : m_dt(0.01)  // 10 ms

{

    timestamp_microsecond = 0u;
    timestamp_millisecond = 0u;
}

void Simulator::run(int32_t p_resistance, int32_t p_compliance) {

    init(p_resistance, p_compliance);

    while (m_state_machine.isRunning())
        loop();

    m_logger.close();
}

void Simulator::init(int32_t p_resistance, int32_t p_compliance) {
    m_model.init(p_resistance, p_compliance, 10.0);
    m_state_machine.init(-1);
    m_logger.init();

    m_cmds.blower = 0;
    m_cmds.expirationValve = 0;
    m_cmds.inspirationValve = 0;
    m_last_date = systemMicros();
}

void Simulator::loop() {
    // running model
    for (int i = 0; i < 20; i++) {
        m_sensors = m_model.compute(m_cmds, m_dt / 20);
    }

    // computing the commands in function of the state of the model. Here, open loop
    m_cmds = m_state_machine.compute(m_sensors, m_dt);

    // Writing the results in a csv file
    // m_logger.write_log(float(timestamp_millisecond) / 1000.0, m_cmds, m_sensors);

    // moving to the next time step
    timestamp_millisecond += uint32_t(m_dt * 1000.0);
    timestamp_microsecond += uint32_t(m_dt * 1000000.0);
    while (systemMicros() - m_last_date < 10000) {
        // Check serial input
        serialControlLoop();
        usleep(10);
    }
    m_last_date = systemMicros();
}
