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

    Serial6 = SerialFake();
    int resistance = 10;
    int compliance = 70;
    cout << "Welcome to Makair simulator" << endl;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "--help" || string(argv[i]) == "-h") {
            cout << "Usage : makair-simulator -p [PORT NAME] -r [RESISTANCE] -c [COMPLIANCE]"
                 << endl;
            cout << "Example : makair-simulator -p /dev/ttyACM0 -r 10 -c 70" << endl;
            return 0;
        }
    }

    if (argc % 2 != 0) {
        for (int i = 0; i < (argc - 1) / 2; i++) {
            if (string(argv[1 + i * 2]) == "-p") {
                Serial6 = SerialFake(argv[1 + i * 2 + 1]);
            } else if (string(argv[1 + i * 2]) == "-c") {
                compliance = stoi(string(argv[1 + i * 2 + 1]));
            } else if (string(argv[1 + i * 2]) == "-r") {
                resistance = stoi(string(argv[1 + i * 2 + 1]));
            } else {
                cout << " Wrong input" << string(argv[1 + i * 2]) << endl;
                cout << "Usage : makair-simulator -p [PORT NAME] -r [RESISTANCE] -c [COMPLIANCE]"
                     << endl;
                cout << "Example : makair-simulator -p /dev/ttyACM0 -r 10 -c 70" << endl;
            }
        }
    } else {
        cout << argc << "    " << argv[1] << endl;
        cout << "Wrong usage" << endl;
        cout << "Usage : makair-simulator -p [PORT NAME] -r [RESISTANCE] -c [COMPLIANCE]" << endl;
        cout << "Example : makair-simulator -p /dev/ttyACM0 -r 10 -c 70" << endl;
        return 1;
    }

    Simulator simulator;
    std::cout << "program started\n";
    // activationController.changeState(1);
    simulator.run(resistance, compliance);
    Serial6.close();
    return 0;
}
