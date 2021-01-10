#include "../includes/Arduino.h"


uint64_t micros() {
    uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    return us; 
}

uint64_t millis()
{
    uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::
                  now().time_since_epoch()).count();
    return ms; 
}

// Serial

SerialFake Serial;
SerialFake Serial1;
SerialFake Serial2;
SerialFake Serial3;
SerialFake Serial4;
SerialFake Serial5;
SerialFake Serial6;
TIM_TypeDef TIM5;

SerialFake::SerialFake() { m_open = false; }
SerialFake::SerialFake(char* p_serialName) {
    m_serialName = p_serialName;

    /*m_serialPort.SetBaudRate(BaudRate::BAUD_115200);
    m_serialPort.SetFlowControl(FlowControl::FLOW_CONTROL_HARDWARE);
    m_serialPort.SetParity(Parity::PARITY_NONE);
    m_serialPort.SetStopBits(StopBits::STOP_BITS_1);
    m_serialPort.Setm_serialPortBlockingStatus(true);*/

    m_timeoutMs = 1;
    m_open = true;
    m_peekBufferIndex = -1;
}

void SerialFake::close() { m_serialPort.closeDevice(); }

void SerialFake::begin(int32_t p_baudrate) {
    if (m_open) {
        int open = m_serialPort.openDevice(m_serialName, p_baudrate);
        if ( open!= 1) {
            cout << "Error openning serial device " << m_serialName << "error:"<<open<<endl;
            exit(1);
        } else {
            cout << "Success openning serial device " << m_serialName << endl;
        }
    }
}

// void print(uint16_t s) {}
void SerialFake::print(const char* str) {
    if (m_open) {
        m_serialPort.writeString(str);
    }
}

void SerialFake::write(uint8_t data) {
    if (m_open) {
        uint8_t next_char[1];
        next_char[0] = data;
        m_serialPort.writeBytes(next_char, 1);
    }
}

void SerialFake::write(const uint8_t* str, size_t size) {
    for (size_t i = 0; i < size; i++) {
        write(str[i]);
    }
}

uint8_t SerialFake::read() {
    if (m_open) {
        if (m_peekBufferIndex >= 0) {
            uint8_t return_value = m_peekBuffer[m_peekBufferIndex];
            m_peekBufferIndex--;
            return return_value;
        } else {
            uint8_t next_char[1];  // variable to store the read result
            m_serialPort.readBytes(next_char, 1, m_timeoutMs);
            return (uint8_t)next_char[0];
        }
    }
}
uint8_t SerialFake::peek() {
    if (m_open) {
        uint8_t next_char[1];
        m_serialPort.readBytes(next_char, 1, m_timeoutMs);
        if (m_peekBufferIndex < 0) {
            m_peekBufferIndex = 0;
        } else {
            m_peekBufferIndex++;
        }
        m_peekBuffer[m_peekBufferIndex] = next_char[0];
        return (uint8_t)next_char[0];
    }
}
void SerialFake::readBytes(uint8_t* buffer, size_t size) {

    for (size_t i = 0; i < size; i++) {
        buffer[i] = (uint8_t)read();
    }
}
int32_t SerialFake::available() {
    if (m_open) {
        return m_serialPort.available();
    }
}

void digitalWrite(int a, int b) {}
