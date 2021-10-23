
#include "../includes/activation.h"
#include "../includes/patient_model.h"
#include "../includes/simulator.h"
#include <emscripten/bind.h>

using namespace emscripten;

#include <iostream>

#define MAX_RESISTANCE_VALUE 70
#define MIN_RESISTANCE_VALUE 1
#define MAX_COMPLIANCE_VALUE 150
#define MIN_COMPLIANCE_VALUE 1
#define MAX_SPONTANEOUS_BREATH_RATE_VALUE 35
#define MIN_SPONTANEOUS_BREATH_RATE_VALUE 0
#define MAX_SPONTANEOUS_BREATH_EFFORT_VALUE 15
#define MIN_SPONTANEOUS_BREATH_EFFORT_VALUE 0
#define MAX_SPONTANEOUS_BREATH_DURATION_VALUE 500
#define MIN_SPONTANEOUS_BREATH_DURATION_VALUE 0