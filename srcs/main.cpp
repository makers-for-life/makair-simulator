/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file main.cpp
 * @brief Main function of the makair simulator
 *****************************************************************************/

// INCLUDES

#include "../includes/activation.h"
#include "../includes/simulator.h"

#include <iostream>

// Main function
int main(int argc, char* argv[]) {

    cout << "Welcome to Makair simulator" << endl;
    if (argc == 3 && string(argv[1]) == "-p") {
        Serial6 = SerialFake(argv[2]);
    } else if (argc == 2 && argv[1] == "--help") {
        cout << "Usage : makair-simulator -p [PORT NAME]" << endl;
        cout << "Example : makair-simulator -p /dev/ttyACM0" << endl;
        return 0;
    } else if (argc == 1) {
        Serial6 = SerialFake();
    } else {
        cout << argc << "    " << argv[1] << endl;
        cout << "Wrong usage" << endl;
        cout << "Usage : makair-simulator -p [PORT NAME]" << endl;
        cout << "Example : makair-simulator -p /dev/ttyACM0" << endl;
        return 1;
    }

    Simulator simulator;
    std::cout << "program started\n";
    activationController.changeState(1);
    simulator.run(50, 100);
    Serial6.close();
    return 0;
}
