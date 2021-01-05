/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simu_state_machine.cpp
 * @brief Manage the state machine of the simulator.
 *****************************************************************************/

// INCLUDES ===================================================================

#include "../includes/simu_state_machine.h"
#include "../includes/activation.h"
#include "../includes/telemetry.h"
#include <iostream>
#include <tgmath.h>

// FUNCTIONS ==================================================================

SimuStateMachine::SimuStateMachine()
    : m_running(true),
      m_state(SETUP),
      m_time(0),
      m_cycle_start_time(0),
      m_volume(0),
      m_cycle_uncount(0) {}

void SimuStateMachine::init(int max_cycle) {
    // for simulation
    m_cycle_uncount = max_cycle;

    // for controller
    HardwareTimer* hardwareTimer3;
    inspiratoryValve = PressureValve(hardwareTimer3, TIM_CHANNEL_INSPIRATORY_VALVE,
                                     PIN_INSPIRATORY_VALVE, VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    inspiratoryValve.setup();
    expiratoryValve = PressureValve(hardwareTimer3, TIM_CHANNEL_EXPIRATORY_VALVE,
                                    PIN_EXPIRATORY_VALVE, VALVE_OPEN_STATE, VALVE_CLOSED_STATE);
    expiratoryValve.setup();
    initTelemetry();
}

ActuatorsData SimuStateMachine::compute(SensorsData sensors, float dt_s) {
    int dt = dt_s * 1000;  // here time is in ms
    updateTime(dt);

    int pressure = sensors.inspirationPressure;
    mainController.updatePressure(pressure);
    int inspiratoryflow = sensors.inspirationFlow;
    int expiratoryflow = sensors.expirationFlow;
    mainController.updateInspiratoryFlow(inspiratoryflow);
    mainController.updateExpiratoryFlow(expiratoryflow);

    updateVolume(inspiratoryflow, dt);
    updateVolumeExpi(expiratoryflow, dt);

    int tick;

    switch (m_state) {
    case SETUP:
        mainController.setup();

        m_state = STOPPED;
        break;

    case STOPPED:

        if (!shouldStop() && activationController.isRunning()) {
            m_state = INIT_CYCLE;
        } else {
            mainController.stop(getTime());
        }

        break;

    case INIT_CYCLE:
        cout << "new cycle" << endl;
        // for simulation
        if (m_cycle_uncount != -1) {
            m_cycle_uncount--;
        }

        // for controller
        mainController.initRespiratoryCycle();
        m_cycle_start_time = getTime();
        resetVolume();
        resetVolumeExpi();

        m_state = BREATH;
        break;

    case BREATH:

        tick = (getTime() - m_cycle_start_time) / 10u;

        if (tick >= mainController.ticksPerCycle()) {
            m_state = END_CYCLE;

        }

        else {
            mainController.updateCurrentDeliveredVolume(getVolume());
            mainController.updateCurrentExpiratoryVolume(getVolumeExpi());
            mainController.updateDt(dt * 1000);  // milli to micro
            mainController.updateTick(tick);
            mainController.compute();
        }

        if (mainController.triggered()) {
            m_state = TRIGGED_RAISED;
        }

        if (!activationController.isRunning()) {
            m_state = SETUP;
        }
        break;

    case TRIGGED_RAISED:
        if (shouldStop() || !activationController.isRunning())
            m_state = STOPPED;
        else
            m_state = END_CYCLE;
        break;

    case END_CYCLE:
        mainController.endRespiratoryCycle(getTime());

        if (shouldStop() || activationController.isRunning())
            m_state = STOPPED;
        else
            m_state = INIT_CYCLE;
        break;

    default:
        m_state = SETUP;
        break;
    }

    ActuatorsData cmds;
    cmds.inspirationValve = getPct(inspiratoryValve.command, inspiratoryValve.minAperture(),
                                   inspiratoryValve.maxAperture());
    cmds.expirationValve = getPct(expiratoryValve.command, expiratoryValve.minAperture(),
                                  expiratoryValve.maxAperture());
    cmds.blower = getPct(blower.getSpeed(), 0, MAX_BLOWER_SPEED);

    return cmds;
}

bool SimuStateMachine::isRunning() { return !shouldStop(); }

void SimuStateMachine::updateTime(int dt) {
    m_time += dt;
    ;
}

int SimuStateMachine::getTime() { return m_time; }

void SimuStateMachine::updateVolume(int flow, int dt) { m_volume += flow * dt / (60 * 1000); }
void SimuStateMachine::updateVolumeExpi(int flow, int dt) {
    m_volumeExpi += flow * dt / (60 * 1000);
}

int SimuStateMachine::getVolume() { return m_volume; }
int SimuStateMachine::getVolumeExpi() { return m_volumeExpi; }

void SimuStateMachine::resetVolume() { m_volume = 0; }
void SimuStateMachine::resetVolumeExpi() { m_volumeExpi = 0; }

bool SimuStateMachine::shouldStop() {
    if (m_cycle_uncount <= 0 && m_cycle_uncount != -1)  // 1 means infite loop
        return true;
    else
        return false;
}

uint16_t SimuStateMachine::getPct(uint16_t val, uint16_t min, uint16_t max) {
    return 100 * (val - min) / (max - min);
}
