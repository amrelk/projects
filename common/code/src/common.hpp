#pragma once

#include "errors.hpp"
#include "typedefs.hpp"

extern "C" void SystemClock_Config();
extern "C" [[maybe_unused]]
void Reset_Handler();

inline u16 min(const u16 a, const u16 b) { return a > b ? b : a; }

