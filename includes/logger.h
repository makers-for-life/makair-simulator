/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file model.h
 * @brief Print the sensor and actuator data in a csv file
 *****************************************************************************/
#pragma once

// INCLUDES ===================================================================
#include<iostream>
#include<fstream>   //used for file handling
#include<string>    //used for strings


#include "../includes/helper.h"


/// Model
class Logger {
  public:

    Logger();
    ~Logger();

    void init();
    void close();

    void write_log(float time, ActuatorsData actuators, SensorsData sensors);

  private:

    std::ofstream m_file;

};
