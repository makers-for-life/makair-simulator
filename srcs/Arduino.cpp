#include "../includes/Arduino.h"

// Serial

SerialFake Serial;
SerialFake Serial1;
SerialFake Serial2;
SerialFake Serial3;
SerialFake Serial4;
SerialFake Serial5;
SerialFake Serial6;
TIM_TypeDef TIM5;

SerialFake::SerialFake(char* serialName) {
    serialPort.Open(serialName);
    serialPort.SetBaudRate(BaudRate::BAUD_115200);
    serialPort.SetFlowControl(FlowControl::FLOW_CONTROL_HARDWARE);
    serialPort.SetParity(Parity::PARITY_NONE);
    serialPort.SetStopBits(StopBits::STOP_BITS_1);
    serialPort.SetSerialPortBlockingStatus(true);

    timeout_ms = 10;
    open = true;
    peek_buffer_index = -1;
}

void SerialFake::close() { serialPort.Close(); }

// Digital

void digitalWrite(int a, int b) {}
