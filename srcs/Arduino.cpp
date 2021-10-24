#include "../includes/Arduino.h"

uint32_t timestamp_microsecond;
uint32_t timestamp_millisecond;

uint32_t micros() {
    /*uint32_t us = std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::steady_clock::now().time_since_epoch())
                      .count();*/
    return timestamp_microsecond;
}

uint32_t millis() {
    /*uint32_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now().time_since_epoch())
                      .count();*/
    return timestamp_millisecond;
}

uint64_t systemMicros() {
    uint32_t us = std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::steady_clock::now().time_since_epoch())
                      .count();
    return us;
}

uint64_t systemMillis() {
    uint32_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now().time_since_epoch())
                      .count();
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

SerialFake::SerialFake() {
    m_streamStderr = false;
    m_streamSerial = false;
}

SerialFake::SerialFake(char* p_serialName) {
    m_streamStderr = (string(p_serialName) == "stderr");
    m_streamSerial = !m_streamStderr;
    m_serialName = p_serialName;

    m_timeoutMs = 1;
    m_peekBufferIndex = -1;
}

// When compiling in wasm, the serial communication is managed with buffer in memory shared by the
// simulator and the UI
SerialFake::SerialFake(char* p_serialName,
                       uint8_t* p_TXserialBuffer,
                       int32_t* p_TXserialBufferIndex,
                       uint8_t* p_RXserialBuffer,
                       int32_t* p_RXserialBufferIndex,
                       int32_t p_SERIAL_BUFFER_SIZE) {
    m_TXserialBuffer = p_TXserialBuffer;
    m_TXserialBufferIndex = p_TXserialBufferIndex;
    m_RXserialBuffer = p_RXserialBuffer;
    m_RXserialBufferIndex = p_RXserialBufferIndex;
    m_SERIAL_BUFFER_SIZE = p_SERIAL_BUFFER_SIZE;
    for (int i = 0; i < m_SERIAL_BUFFER_SIZE; i++) {
        m_TXserialBuffer[i] = 0;
        m_RXserialBuffer[i] = 0;
    }
    *m_RXserialBufferIndex = -1;  // -1 is the inital state
    *m_TXserialBufferIndex = -1;  // -1 is the inital state

    m_streamSerial = true;
    m_serialName = p_serialName;

    m_timeoutMs = 1;
    m_peekBufferIndex = -1;
}

void SerialFake::close() {
#ifdef EMBEDED_FOR_RUST
    // todo
#else
    m_serialPort.closeDevice();
#endif
}

void SerialFake::begin(int32_t p_baudrate) {
    if (m_streamSerial) {
        int open;
#ifdef EMBEDED_FOR_RUST
        open = 1;
#else
        open = m_serialPort.openDevice(m_serialName, p_baudrate);
#endif
        if (open != 1) {
            cout << "Error openning serial device " << m_serialName << ", error:" << open << endl;
            cout << (m_serialName == "stderr") << endl;
            exit(1);
        } else {
            cout << "Success openning serial device " << m_serialName << endl;
        }
    }
}

// void print(uint16_t s) {}
void SerialFake::print(const char* str) {
    if (m_streamSerial) {
#ifdef EMBEDED_FOR_RUST
        // todo
#else
        m_serialPort.writeString(str);
#endif

    } else if (m_streamStderr) {
        cerr << str;
    }
}

void SerialFake::write(uint8_t data) {
    if (m_streamSerial) {
        uint8_t next_char[1];
        next_char[0] = data;
#ifdef EMBEDED_FOR_RUST
        if (*m_TXserialBufferIndex < m_SERIAL_BUFFER_SIZE - 1) {
            *m_TXserialBufferIndex = *m_TXserialBufferIndex + 1;
        } else {
            *m_TXserialBufferIndex = 0;
        }
        m_TXserialBuffer[*m_TXserialBufferIndex] = data;
#else
        m_serialPort.writeBytes(next_char, 1);
#endif

    } else if (m_streamStderr) {
        cerr << data;
    }
}

void SerialFake::write(const uint8_t* str, size_t size) {
    for (size_t i = 0; i < size; i++) {
        write(str[i]);
    }
}

uint8_t SerialFake::read() {
    if (m_streamSerial) {
        if (m_peekBufferIndex >= 0) {
            uint8_t return_value = m_peekBuffer[m_peekBufferIndex];
            m_peekBufferIndex--;
            return return_value;
        } else {
            uint8_t next_char[1];  // variable to store the read result
#ifdef EMBEDED_FOR_RUST
            if (*m_RXserialBufferIndex > 0) {
                next_char[0] = m_RXserialBuffer[*m_RXserialBufferIndex];
                *m_RXserialBufferIndex = *m_TXserialBufferIndex - 1;
            } else {
                next_char[0] = 0;
                cout << "error with firmware serial read" << endl;
            }
#else
            m_serialPort.readBytes(next_char, 1, m_timeoutMs);
#endif
            return (uint8_t)next_char[0];
        }
    }
}
uint8_t SerialFake::peek() {
    if (m_streamSerial) {
        uint8_t next_char[1];
#ifdef EMBEDED_FOR_RUST
        if (*m_RXserialBufferIndex > 0) {
            next_char[0] = m_RXserialBuffer[*m_RXserialBufferIndex];
            *m_RXserialBufferIndex = *m_TXserialBufferIndex - 1;
        } else {
            next_char[0] = 0;
            cout << "error with firmware serial read" << endl;
        }
#else
        m_serialPort.readBytes(next_char, 1, m_timeoutMs);
#endif
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
    if (m_streamSerial) {
#ifdef EMBEDED_FOR_RUST
        return *m_RXserialBufferIndex > 0;
#else
        return m_serialPort.available();
#endif
    } else {
        return false;
    }
}

void digitalWrite(int a, int b) {}
