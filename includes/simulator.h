/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file simulator.h
 * @brief Simulator for the makair controller.
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================

#include "../includes/config.h"
#include "../includes/helper.h"
#include "../includes/model.h"
#include "../includes/patient_model.h"
#include "../includes/simu_state_machine.h"
#include <cstdint>
#ifdef SIMULATOR_WASM
#include <emscripten.h>
#endif

/// Simulator
class Simulator {
 public:
    /// Default constructor
    Simulator();

    void startAndRun(PatientModel& p_patientModel);
    void init(PatientModel& p_patientModel);
    void loop();
    Model* getModel() { return &m_model; };

 private:
    Model m_model;
    SimuStateMachine m_state_machine;

    ActuatorsData m_cmds;
    SensorsData m_sensors;
    float m_dt;  // in s
    uint64_t m_last_date;
};

extern Simulator simulator;
