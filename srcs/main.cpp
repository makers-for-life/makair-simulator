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
int main(int argc, char* argv[]) {

    cout << "Welcome to :" << argv[0] << endl;
    if (argc == 3 && argv[1] == "-p") {
        Serial6 = SerialFake("/dev/pts/3");
    } else if (argc == 3 && argv[1] == "--help") {
        Serial6 = SerialFake("/dev/pts/3");
        cout << "Usage : makair-simulator -p [PORT NAME]" << endl;
        cout << "Example : makair-simulator -p /dev/ttyACM0" << endl;
        return 0;
    } else if (argc == 1) {
        Serial6 = SerialFake(argv[2]);
    } else {
        cout << "Wrong usage" << endl;
        cout << "Usage : makair-simulator -p [PORT NAME]" << endl;
        cout << "Example : makair-simulator -p /dev/ttyACM0" << endl;
        return 1;
    }

    Serial6 = SerialFake("/dev/pts/3");
    Simulator simulator;
    std::cout << "program started\n";
    simulator.run();
    Serial6.close();
    return 0;
}
