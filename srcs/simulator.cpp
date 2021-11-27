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

// INITIALISATION =============================================================
Simulator simulator;

// FUNCTIONS ==================================================================

Simulator::Simulator()
    : m_dt(0.01)  // 10 ms

{

    timestamp_microsecond = 0u;
    timestamp_millisecond = 0u;
    m_accelerationFactor = 1.0;
}

void Simulator::run() {

    while (m_state_machine.isRunning())
        loop();
}

void Simulator::init(PatientModel& p_patientModel) {
    m_model.init(p_patientModel);
    m_state_machine.init(-1);

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

    // moving to the next time step
    timestamp_millisecond += uint32_t(m_dt * 1000.0);
    timestamp_microsecond += uint32_t(m_dt * 1000000.0);
    while (systemMicros() - m_last_date < 10000 / m_accelerationFactor) {
        // Check serial input
        serialControlLoop();
        usleep(10);
    }

    m_last_date = systemMicros();
}

bool Simulator::setAccelerationFactor(float p_accelerationFactor) {
    if (p_accelerationFactor >= MIN_ACCELERATION_FACTOR
        && p_accelerationFactor <= MAX_ACCELERATION_FACTOR) {
        m_accelerationFactor = p_accelerationFactor;
        return true;
    } else {
        cout << "Error : Acceleration factor should be between x" << MIN_ACCELERATION_FACTOR
             << " and x" << MAX_ACCELERATION_FACTOR << endl;
        return false;
    }
}