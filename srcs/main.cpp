/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file main.cpp
 * @brief Main function of the makair simulator
 *****************************************************************************/

// INCLUDES

#include "../includes/main.h"

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
            cout << "Resistance in cmh2O/L/s, between " << MIN_RESISTANCE_VALUE << " and "
                 << MAX_RESISTANCE_VALUE << " cmh2O/L/s" << endl;
            cout << "Compliance in mL/cmH2O, between " << MIN_COMPLIANCE_VALUE << " and "
                 << MAX_COMPLIANCE_VALUE << " mL/cmH2O" << endl;
            return 0;
        }
    }

    if (argc % 2 != 0) {
        for (int i = 0; i < (argc - 1) / 2; i++) {
            if (string(argv[1 + i * 2]) == "-p") {
                Serial6 = SerialFake(argv[1 + i * 2 + 1]);
            } else if (string(argv[1 + i * 2]) == "-c") {
                int32_t complianceValue = stoi(string(argv[1 + i * 2 + 1]));
                if (complianceValue >= 1 && complianceValue <= 150) {
                    compliance = complianceValue;
                } else {
                    cout << "Error : compliance should be between " << MIN_COMPLIANCE_VALUE
                         << " and " << MAX_COMPLIANCE_VALUE << " cmh2O/L/s" << endl;
                    return 1;
                }

            } else if (string(argv[1 + i * 2]) == "-r") {
                int32_t resistanceValue = stoi(string(argv[1 + i * 2 + 1]));
                if (resistanceValue >= 1 && resistanceValue <= MAX_RESISTANCE_VALUE) {
                    resistance = resistanceValue;
                } else {
                    cout << "Error : Resistance should be between " << MIN_RESISTANCE_VALUE
                         << " and " << MAX_RESISTANCE_VALUE << " cmh2O/L/s" << endl;
                    return 1;
                }
            } else {
                cout << " Wrong input" << string(argv[1 + i * 2]) << endl;
                cout << "Usage : makair-simulator -p [PORT NAME] -r [RESISTANCE] -c [COMPLIANCE]"
                     << endl;
                cout << "Example : makair-simulator -p /dev/ttyACM0 -r 10 -c 70" << endl;
            }
        }
    } else {
        cout << "Wrong usage" << endl;
        cout << "Usage : makair-simulator -p [PORT NAME] -r [RESISTANCE] -c [COMPLIANCE]" << endl;
        cout << "Example : makair-simulator -p /dev/ttyACM0 -r 10 -c 70" << endl;

        return 1;
    }

    Simulator simulator;
    std::cout << "program started\n";
    activationController.changeState(1);
    PatientModel patientModel(resistance, 2.0, compliance, 0.0, 20, 5, 500);
    simulator.run(patientModel);
    Serial6.close();
    return 0;
}
