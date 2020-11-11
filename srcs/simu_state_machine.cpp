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
    , m_state(SETUP)
    , m_time(0)
    , m_cycle_start_time(0)
    , m_volume(0)
{}

void SimuStateMachine::init() {}

ActuatorsData SimuStateMachine::compute(SensorsData sensors, int dt){
    updateTime(dt);

    int pressure = sensors.inspirationPressure;
    // mainController.updatePressure(pressure);
    int inspiratoryflow = sensors.inspirationFlow;
    // mainController.updateInspiratoryFlow(inspiratoryflow);
    updateVolume(inspiratoryflow, dt);

    int tick;

    switch (m_state){
        case SETUP:
            // mainController.setup();

            m_state = STOPPED;
            break;

        case STOPPED:
            // mainController.stop(getTime());

            if (isRunning())
                m_state = INIT_CYCLE;
            break;

        case INIT_CYCLE:
            // mainController.initRespiratoryCycle();
            m_cycle_start_time = getTime();
            resetVolume();

            m_state = BREATH;
            break;

        case BREATH:
            tick = (getTime() - m_cycle_start_time) / 10u;

            if (tick >= 10/*mainController.ticksPerCycle()*/)
                m_state = END_CYCLE;
            else {
                // mainController.updateCurrentDeliveredVolume(getVolume());
                // mainController.updateDt(dt*1000); // milli to micro
                // mainController.updateTick(tick);
                // mainController.compute();
            }

            if (false/*mainController.triggered()*/) {
                m_state = TRIGGED_RAISED;
            }
            break;

        case TRIGGED_RAISED:
            if (isRunning())
                m_state = END_CYCLE;
            else
                m_state = STOPPED;
            break;

        case END_CYCLE:
            // mainController.endRespiratoryCycle(getTime());

            if (isRunning())
                m_state = INIT_CYCLE;
            else
                m_state = STOPPED;
            break;

        default:
            m_state = SETUP;
            break;
    }

    ActuatorsData cmds;
    return cmds;
}

bool SimuStateMachine::isRunning(){
    return m_running;
}

void SimuStateMachine::updateTime(int dt){
    m_time += dt;;
}

int SimuStateMachine::getTime(){
    return m_time;
}

void SimuStateMachine::updateVolume(int flow, int dt){
    m_volume += flow * dt /(60 * 1000);
}

int SimuStateMachine::getVolume(){
    return m_volume;
}

void SimuStateMachine::resetVolume(){
    m_volume = 0;
}