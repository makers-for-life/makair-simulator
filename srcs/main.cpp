/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file main.cpp
 * @brief Main function of the makair simulator
 *****************************************************************************/

// INCLUDES

#include "../includes/simulator.h"
#include <iostream>

// Main function
int main(void) {

    Serial6 = SerialFake("/dev/pts/3");
    Simulator simulator;
    std::cout << "program started\n";
    simulator.run();
    Serial6.close();
    return 0;
}
