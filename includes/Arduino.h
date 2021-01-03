
/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file Arduino.h
 * @brief Fake: Arduino libraries
 *****************************************************************************/

#pragma once
#ifndef ARDUINO_H
#define ARDUINO_H

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <unistd.h>

// Linux headers
#include <SerialPort.h>
#include <SerialStream.h>

using namespace LibSerial;

using namespace std;

#define byte uint8_t

inline uint32_t millis() {
    return chrono::duration_cast<chrono::milliseconds>(
               chrono::system_clock::now().time_since_epoch())
        .count();
}

inline uint32_t micros() {
    return chrono::duration_cast<chrono::microseconds>(
               chrono::system_clock::now().time_since_epoch())
        .count();
}

inline int32_t map(int32_t value, int32_t a, int32_t b, int32_t c, int32_t d) {
    return c + value * (d - c) / (b - a);
}

// String
class String {
 public:
    String() {}
    String(int s) {}
    String(const char* s) {}
};

inline int32_t digitalPinToPinName(int32_t p) { return 0; }
inline int32_t pinmap_function(int32_t p, int32_t t) { return 0; }
inline int32_t pinmap_peripheral(int32_t p, int32_t t) { return 0; }
inline int32_t STM_PIN_CHANNEL(int32_t p) { return 0; }

// HardwareTimer
#define MICROSEC_COMPARE_FORMAT 0
#define MICROSEC_FORMAT 0
#define TICK_FORMAT 0
#define TIMER_OUTPUT_COMPARE_PWM1 0
#define TIMER_OUTPUT_COMPARE 0
#define PinMap_PWM 0
#define NC 0

class TIM_TypeDef {
 public:
    TIM_TypeDef() {}
};
extern TIM_TypeDef TIM5;

class HardwareTimer {
 public:
    HardwareTimer() {}
    HardwareTimer(TIM_TypeDef* t) {}
    HardwareTimer(TIM_TypeDef& t) {}
    void setCaptureCompare(uint16_t a, uint16_t b, uint16_t c) {}
    void setMode(uint16_t a, uint16_t b, uint16_t c) {}
    void resume() {}
    void pause() {}
    void setOverflow(int32_t p, int32_t t){};
    void setOverflow(int32_t p){};
    void setCount(int32_t p){};
    void setPreloadEnable(bool p){};
    void setPrescaleFactor(int32_t p) {}
    void attachInterrupt(void (*)(HardwareTimer*)) {}
    int32_t getTimerClkFreq() { return 0; }
};

// Serial

class SerialFake {
 public:
    SerialFake() { open = false; }
    SerialFake(char* serialName);

    void begin(int32_t baudrate) {}
    void close();

    // void print(uint16_t s) {}
    void println() {}
    void println(String s) {}
    void print(const char* str) {
        if (open) {
            serialPort.Write(str);
        }
    }

    void write(uint8_t data) {
        if (open) {
            serialPort.WriteByte(data);
        }
    }

    void write(const uint8_t* str, size_t size) {
        for (size_t i = 0; i < size; i++) {
            write(str[i]);
        }
    }

    size_t write(const char* str) {

        if (str == NULL)
            return 0;
        int length = sizeof(str) / sizeof(str[0]);
        write((const uint8_t*)str, length);
    }
    size_t write(const char* buffer, size_t size) { write((const uint8_t*)buffer, size); }

    /*void write(T* argument) {
        int nBytes = sizeof(T);
        const uint8_t* pData = (const uint8_t*)argument;

        for (int32_t i = 0; i < nBytes; i++) {
            write(pData[i]);
        }
    }*/
    inline void write(bool t) { write((uint8_t)t); }
    inline void write(int16_t t) { write((const uint8_t*)t, 2); }
    inline void write(uint16_t t) { write((const uint8_t*)t, 2); }
    inline void write(int32_t t) { write((const uint8_t*)t, 4); }
    inline void write(uint32_t t) { write((const uint8_t*)t, 4); }
    inline void write(int64_t t) { write((const uint8_t*)t, 8); }
    inline void write(uint64_t t) { write((const uint8_t*)t, 8); }
    /*inline void write(long long t) { write((uint8_t)t); }
    inline void write(unsigned long long t) { write((uint8_t)t); }*/
    // Enable write(char) to fall through to write(uint8_t)
    inline void write(char c) { write((uint8_t)c); }

    uint8_t read() {
        if (peek_buffer_index >= 0) {
            uint8_t return_value = peek_buffer[peek_buffer_index];
            peek_buffer_index--;
            return return_value;
        } else {
            uint8_t next_char;  // variable to store the read result
            serialPort.ReadByte(next_char, timeout_ms);
            return (uint8_t)next_char;
        }
    }
    uint8_t peek() {
        uint8_t next_char;
        serialPort.ReadByte(next_char, timeout_ms);
        if (peek_buffer_index < 0) {
            peek_buffer_index = 0;
        } else {
            peek_buffer_index++;
        }
        peek_buffer[peek_buffer_index] = next_char;
        return (uint8_t)next_char;
    }
    void readBytes(uint8_t* buffer, size_t size) {

        for (size_t i = 0; i < size; i++) {
            buffer[i] = (uint8_t)read();
        }
    }
    int32_t available() { return serialPort.GetNumberOfBytesAvailable(); }

 private:
    SerialPort serialPort;
    bool open;
    int timeout_ms;
    uint8_t peek_buffer[10000];
    int32_t peek_buffer_index;
};

extern SerialFake Serial;
extern SerialFake Serial1;
extern SerialFake Serial2;
extern SerialFake Serial3;
extern SerialFake Serial4;
extern SerialFake Serial5;
extern SerialFake Serial6;

// Digital

void digitalWrite(int a, int b);

// Define Pinout

#define PA0 0
#define PA1 0
#define PA2 0
#define PA3 0
#define PA4 0
#define PA5 0
#define PA6 0
#define PA7 0
#define PA8 0
#define PA9 0
#define PA10 0
#define PA11 0
#define PA12 0
#define PA13 0
#define PA14 0
#define PA15 0
#define PB0 0
#define PB1 0
#define PB2 0
#define PB3 0
#define PB4 0
#define PB5 0
#define PB6 0
#define PB7 0
#define PB8 0
#define PB9 0
#define PB10 0
#define PB11 0
#define PB12 0
#define PB13 0
#define PB14 0
#define PB15 0
#define PC0 0
#define PC1 0
#define PC2 0
#define PC3 0
#define PC4 0
#define PC5 0
#define PC6 0
#define PC7 0
#define PC8 0
#define PC9 0
#define PC10 0
#define PC11 0
#define PC12 0
#define PC13 0
#define PC14 0
#define PC15 0
#define PD0 0
#define PD1 0
#define PD2 0
#define PD3 0
#define PD4 0
#define PD5 0
#define PD6 0
#define PD7 0
#define PD8 0
#define PD9 0
#define PD10 0
#define PD11 0
#define PD12 0
#define PD13 0
#define PD14 0
#define PD15 0
#define D0 0
#define D1 0
#define D2 0
#define D3 0
#define D4 0
#define D5 0
#define PIN_BUZZER 0

#define HIGH 1
#define LOW 0

#endif