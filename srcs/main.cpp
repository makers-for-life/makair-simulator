/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file main.cpp
 * @brief Main function of the makair simulator
 *****************************************************************************/

// INCLUDES

#include "../includes/main.h"

uint8_t TXserialBuffer[SERIAL_BUFFER_SIZE];
int32_t TXserialBufferIndex = 0;
uint8_t RXserialBuffer[SERIAL_BUFFER_SIZE];
int32_t RXserialBufferIndex = 0;

#ifndef SIMULATOR_WASM
// Main function
int main(int argc, char* argv[]) {

    Serial6 = SerialFake();
    // Default values
    int resistance = 10;                // in cmh2O/L/s
    int compliance = 70;                // in mL/cmH2O
    int spontaneousBreathRate = 0;      // in cycle/min
    int spontaneousBreathEffort = 0;    // in cmH2O
    int spontaneousBreathDuration = 0;  // in ms

    cout << "Welcome to Makair simulator" << endl << endl;
    for (int i = 0; i < argc; i++) {
        if (string(argv[i]) == "--help" || string(argv[i]) == "-h") {
            cout << "Usage : makair-simulator -p [PORT NAME] -r [RESISTANCE] -c [COMPLIANCE] "
                    "-spontaneousBreathRate [SPONTANEOUS_BREATH_RATE] -spontaneousBreathEffort "
                    "[SPONTANEOUS_BREATH_EFFORT] -spontaneousBreathDuration "
                    "[SPONTANEOUS_BREATH_DURATION]"
                 << endl
                 << endl;
            cout << "Example : makair-simulator -p /dev/ttyACM0 -r 10 -c 70 "
                    "--spontaneousBreathRate 20 --spontaneousBreathEffort 5 "
                    "--spontaneousBreathDuration 200"
                 << endl
                 << endl;
            cout << "Resistance in cmh2O/L/s, between " << MIN_RESISTANCE_VALUE << " and "
                 << MAX_RESISTANCE_VALUE << " cmh2O/L/s" << endl;
            cout << "Compliance in mL/cmH2O, between " << MIN_COMPLIANCE_VALUE << " and "
                 << MAX_COMPLIANCE_VALUE << " mL/cmH2O" << endl;
            cout << "spontaneousBreathRate between " << MIN_SPONTANEOUS_BREATH_RATE_VALUE << " and "
                 << MAX_SPONTANEOUS_BREATH_RATE_VALUE << " cycle/min" << endl;
            cout << "spontaneousBreathEffort between " << MIN_SPONTANEOUS_BREATH_EFFORT_VALUE
                 << " and " << MAX_SPONTANEOUS_BREATH_EFFORT_VALUE << " cmH2O" << endl;
            cout << "spontaneousBreathDuration between " << MIN_SPONTANEOUS_BREATH_DURATION_VALUE
                 << " and " << MAX_SPONTANEOUS_BREATH_DURATION_VALUE << " ms" << endl;
            return 0;
        }
    }

    if (argc % 2 != 0) {
        for (int i = 0; i < (argc - 1) / 2; i++) {
            if (string(argv[1 + i * 2]) == "-p") {
                Serial6 = SerialFake(argv[1 + i * 2 + 1]);
            } else if (string(argv[1 + i * 2]) == "-c") {
                int32_t complianceValue = stoi(string(argv[1 + i * 2 + 1]));
                if (complianceValue >= MIN_COMPLIANCE_VALUE && complianceValue <= 150) {
                    compliance = complianceValue;
                } else {
                    cout << "Error : compliance should be between " << MIN_COMPLIANCE_VALUE
                         << " and " << MAX_COMPLIANCE_VALUE << " mL/cmH2O" << endl;
                    return 1;
                }

            } else if (string(argv[1 + i * 2]) == "-r") {
                int32_t resistanceValue = stoi(string(argv[1 + i * 2 + 1]));
                if (resistanceValue >= MIN_RESISTANCE_VALUE
                    && resistanceValue <= MAX_RESISTANCE_VALUE) {
                    resistance = resistanceValue;
                } else {
                    cout << "Error : Resistance should be between " << MIN_RESISTANCE_VALUE
                         << " and " << MAX_RESISTANCE_VALUE << " cmh2O/L/s" << endl;
                    return 1;
                }
            } else if (string(argv[1 + i * 2]) == "--spontaneousBreathRate") {
                int32_t spontaneousBreathRateValue = stoi(string(argv[1 + i * 2 + 1]));
                if (spontaneousBreathRateValue >= MIN_SPONTANEOUS_BREATH_RATE_VALUE
                    && spontaneousBreathRateValue <= MAX_SPONTANEOUS_BREATH_RATE_VALUE) {
                    spontaneousBreathRate = spontaneousBreathRateValue;
                } else {
                    cout << "Error : spontaneousBreathRate should be between "
                         << MIN_SPONTANEOUS_BREATH_RATE_VALUE << " and "
                         << MAX_SPONTANEOUS_BREATH_RATE_VALUE << " cycle/min" << endl;
                    return 1;
                }
            } else if (string(argv[1 + i * 2]) == "--spontaneousBreathEffort") {
                int32_t spontaneousBreathEffortValue = stoi(string(argv[1 + i * 2 + 1]));
                if (spontaneousBreathEffortValue >= MIN_SPONTANEOUS_BREATH_EFFORT_VALUE
                    && spontaneousBreathEffortValue <= MAX_SPONTANEOUS_BREATH_EFFORT_VALUE) {
                    spontaneousBreathEffort = spontaneousBreathEffortValue;
                } else {
                    cout << "Error : spontaneousBreathEffort should be between "
                         << MIN_SPONTANEOUS_BREATH_EFFORT_VALUE << " and "
                         << MAX_SPONTANEOUS_BREATH_EFFORT_VALUE << " cmh2O" << endl;
                    return 1;
                }
            } else if (string(argv[1 + i * 2]) == "--spontaneousBreathDuration") {
                int32_t spontaneousBreathDurationValue = stoi(string(argv[1 + i * 2 + 1]));
                if (spontaneousBreathDurationValue >= MIN_SPONTANEOUS_BREATH_DURATION_VALUE
                    && spontaneousBreathDurationValue <= MAX_SPONTANEOUS_BREATH_DURATION_VALUE) {
                    spontaneousBreathDuration = spontaneousBreathDurationValue;
                } else {
                    cout << "Error : spontaneousBreathDuration should be between "
                         << MIN_SPONTANEOUS_BREATH_DURATION_VALUE << " and "
                         << MAX_SPONTANEOUS_BREATH_DURATION_VALUE << " ms" << endl;
                    return 1;
                }
            } else {
                cout << "Wrong input" << string(argv[1 + i * 2]) << endl;
                cout << "Check --help" << endl;
            }
        }
    } else {
        cout << "Wrong usage, check --help" << endl;
        return 1;
    }
    std::cout << "program started\n";
    activationController.changeState(1);
    PatientModel patientModel(resistance, 2.0, compliance, 0.0, spontaneousBreathRate,
                              spontaneousBreathEffort, spontaneousBreathDuration);
    simulator.startAndRun(patientModel);
    Serial6.close();
    return 0;
}

#else

extern "C" {

int run_simulator() {
    std::cout << "program started\n";
    int resistance = 10;                // in cmh2O/L/s
    int compliance = 70;                // in mL/cmH2O
    int spontaneousBreathRate = 0;      // in cycle/min
    int spontaneousBreathEffort = 0;    // in cmH2O
    int spontaneousBreathDuration = 0;  // in ms

    Serial6 = SerialFake("emscripten", TXserialBuffer, &TXserialBufferIndex, RXserialBuffer,
                         &RXserialBufferIndex, SERIAL_BUFFER_SIZE);
    activationController.changeState(1);
    PatientModel patientModel(resistance, 2.0, compliance, 0.0, spontaneousBreathRate,
                              spontaneousBreathEffort, spontaneousBreathDuration);
    simulator.startAndRun(patientModel);
    return 0;
}
}

#endif
