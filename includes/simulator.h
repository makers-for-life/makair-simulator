/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simulator.h
 * @brief Simulator for the makair controller.
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================

#include "../includes/helper.h"
#include "../includes/logger.h"
#include "../includes/model.h"
#include "../includes/simu_state_machine.h"
#include <cstdint>

/// Simulator
class Simulator {
 public:
    /// Default constructor
    Simulator();

    void run(int32_t p_resistance, int32_t p_compliance);

 private:
    void init(int32_t p_resistance, int32_t p_compliance);

    void loop();

    Model m_model;
    SimuStateMachine m_state_machine;
    Logger m_logger;

    ActuatorsData m_cmds;
    SensorsData m_sensors;
    float m_dt;  // in s
    float m_time;
    uint32_t m_last_date;
};