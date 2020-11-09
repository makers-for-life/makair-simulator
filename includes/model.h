/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file model.h
 * @brief Simulate the model of the respirator and the patient.
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================

#include "../includes/helper.h"

/// Model
class Model {
  public:
    /// Default constructor
    Model();

    void init();

    SensorsData compute(ActuatorsData cmds, int dt);
};