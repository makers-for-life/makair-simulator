

#include "../includes/logger.h"

Logger::Logger() : m_file("") {}

Logger::~Logger() {}

void Logger::close() {
    m_file.close();
    std::cout << "file closed\n";
}

// creation of the log file
void Logger::init() {
    m_file.open("../data/simulation_output/log_simulator.csv");
    std::cout << "file opend\n";
    m_file << "#time(s);pressure(cmH2O);flow(ml.s-1);openning_insiration_valve(%);openning_"
              "expiration_valve(%);blower_speed(%)\n";
}

void Logger::write_log(float time, ActuatorsData actuators, SensorsData sensors) {
    m_file << time << ";" << sensors.inspiratoryPressure << ";" << sensors.inspiratoryFlow << ";"
           << actuators.inspirationValve << ";" << actuators.expirationValve << ";"
           << actuators.blower << "\n";
}