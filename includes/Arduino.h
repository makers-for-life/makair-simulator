
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

// Linux headers
#include <SerialPort.h>

using namespace LibSerial;

using namespace std;

#define byte char

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
// HardwareTimer

#define MICROSEC_COMPARE_FORMAT 0
#define TIMER_OUTPUT_COMPARE_PWM1 0

class HardwareTimer {
 public:
    HardwareTimer() {}
    void setCaptureCompare(uint16_t a, uint16_t b, uint16_t c) {}
    void setMode(uint16_t a, uint16_t b, uint16_t c) {}
    void resume() {}
};

// Serial

class SerialFake {
 public:
    SerialFake() {}
    SerialFake(char* serialName);

    void begin(int32_t baudrate) {}
    // void print(String s) {}
    void println() {}
    void println(String s) {}
    void close();

    void write(uint8_t argument) {
        if (open) {
            serialPort.WriteByte(argument);
        }
    }

    template <typename T>
    void write(T* argument, int size) {
        int nBytes = size * sizeof(T);
        const uint8_t* pData = (const uint8_t*)argument;

        for (int32_t i = 0; i < nBytes; i++) {
            write(pData[i]);
        }
    }

    template <typename T>
    void write(T* argument) {
        int nBytes = sizeof(T);
        const uint8_t* pData = (const uint8_t*)argument;

        for (int32_t i = 0; i < nBytes; i++) {
            write(pData[i]);
        }
    }

    template <typename T>
    void print(T* argument) {
        write(argument);
    }

    template <typename T>
    void print(T argument) {
        write(argument);
    }

 private:
    SerialPort serialPort;
    bool open = false;
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

#define HIGH 1
#define LOW 0

#endif