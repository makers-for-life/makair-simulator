/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file config.h
 * @brief Main configuration
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <algorithm>
using namespace std;

/**
 * Activates debug traces
 *
 * When DEBUG = 1, additional code is added to send debug traces using serial
 * When DEBUG = 2, information are sent in serial for PID debug
 */
#define DEBUG 0

/// Simulation mode allows to run main controller on a computer
#define SIMULATION true
