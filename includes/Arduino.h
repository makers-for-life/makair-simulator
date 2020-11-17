#pragma once
#define MICROSEC_COMPARE_FORMAT 0
#include <cstdint>
#include <algorithm>
using namespace std;

class HardwareTimer{

  public:
    void setCaptureCompare(uint16_t a, uint16_t b, uint16_t c){a += b + c;}
};