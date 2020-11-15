

#include "../includes/logger.h"

Logger::Logger():
    m_file("")
{
    std::cout << "class logger created\n";
}

Logger::~Logger(){}

void Logger::close()
{
    m_file.close();
    std::cout << "file closed\n";
}

void Logger::init(){
    m_file.open("../data/simulation_output/log_simulator.csv");
    std::cout << "file opend\n";
}

void Logger::write_log(float time, ActuatorsData actuators, SensorsData sensors)
{
    m_file << time << ";" << sensors.inspirationFlow << ";" << sensors.inspirationPressure << ";"
            << actuators.inspirationValve << ";" << actuators.expirationValve << ";" << actuators.blower << "\n";
}