/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simu_state_machine.cpp
 * @brief Manage the state machine of the simulator.
 *****************************************************************************/

// INCLUDES ===================================================================

#include "../includes/simu_state_machine.h"
#include <iostream>

// FUNCTIONS ==================================================================

SimuStateMachine::SimuStateMachine()
    : m_running(true)
    , m_state(SETUP)
    , m_time(0)
    , m_cycle_start_time(0)
    , m_volume(0)
    , m_cycle_uncount(0)
{}

void SimuStateMachine::init(int max_cycle) {
    // for simulation
    m_cycle_uncount = max_cycle;
    
    // for controller
    HardwareTimer *hardwareTimer3;
    inspiratoryValve = PressureValve(hardwareTimer3, TIM_CHANNEL_INSPIRATORY_VALVE,
                                     PIN_INSPIRATORY_VALVE, VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    inspiratoryValve.setup();
    expiratoryValve = PressureValve(hardwareTimer3, TIM_CHANNEL_EXPIRATORY_VALVE,
                                    PIN_EXPIRATORY_VALVE, VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    expiratoryValve.setup();
}

ActuatorsData SimuStateMachine::compute(SensorsData sensors, float dt_s){
    int dt = dt_s * 1000; // here time is in ms
    updateTime(dt);

    int pressure = sensors.inspirationPressure;
    mainController.updatePressure(pressure);
    int inspiratoryflow = sensors.inspirationFlow;
    mainController.updateInspiratoryFlow(inspiratoryflow);
    updateVolume(inspiratoryflow, dt);

    int tick;

    switch (m_state){
        case SETUP:
            mainController.setup();

            m_state = STOPPED;
            break;

        case STOPPED:
            mainController.stop(getTime());

            if (!shouldStop())
                m_state = INIT_CYCLE;
            else
                m_running = false;
                
            break;

        case INIT_CYCLE:
            // for simulation
            m_cycle_uncount--;

            // for controller
            mainController.initRespiratoryCycle();
            m_cycle_start_time = getTime();
            resetVolume();

            m_state = BREATH;
            break;

        case BREATH:
            tick = (getTime() - m_cycle_start_time) / 10u;

            if (tick >= mainController.ticksPerCycle())
                m_state = END_CYCLE;
            else {
                mainController.updateCurrentDeliveredVolume(getVolume());
                mainController.updateDt(dt*1000); // milli to micro
                mainController.updateTick(tick);
                mainController.compute();
            }

            if (mainController.triggered()) {
                m_state = TRIGGED_RAISED;
            }
            break;

        case TRIGGED_RAISED:
            if (shouldStop())
                m_state = STOPPED;
            else
                m_state = END_CYCLE;
            break;

        case END_CYCLE:
            mainController.endRespiratoryCycle(getTime());

            if (shouldStop())
                m_state = STOPPED;
            else
                m_state = INIT_CYCLE;
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

bool SimuStateMachine::shouldStop(){
    if (m_cycle_uncount <= 0)
        return true;
    else
        return false;
}