#include "../includes/Arduino.h"

// Serial

SerialFake Serial;
SerialFake Serial1;
SerialFake Serial2;
SerialFake Serial3;
SerialFake Serial4;
SerialFake Serial5;
SerialFake Serial6;

SerialFake::SerialFake(char* serialName) {
    serialPort.Open(serialName);
    serialPort.SetBaudRate(BaudRate::BAUD_115200);
    open = true;
}

void SerialFake::close() { serialPort.Close(); }

// Digital

void digitalWrite(int a, int b) {}
