

#include "../includes/logger.h"

Logger::Logger():
    m_file("")
{
    std::cout << "class logger created";
}

Logger::~Logger()
{
    m_file.close();
    std::cout << "file closed";
}

void Logger::init(){
    m_file.open("log_simulator.csv");
    std::cout << "file opend";
}

void Logger::write_log(float time, ActuatorsData actuators, SensorsData sensors)
{
    m_file << time << ";" << sensors.inspirationFlow << ";" << sensors.inspirationPressure << ";"
            << actuators.inspirationValve << ";" << actuators.expirationValve << ";" << actuators.blower << "\n";
}