#pragma once

// system includes
#include <atomic>
#include <cstdint>
#include <optional>

// 3rdparty lib includes
#include <espchrono.h>

namespace can {
namespace inputs {
    extern std::atomic<std::optional<int16_t>> rawGas;
    extern std::atomic<std::optional<int16_t>> rawBrems;

    extern std::atomic<std::optional<float>> gas;
    extern std::atomic<std::optional<float>> brems;
} // namespace inputs

namespace outputs {
    // value from motor controller in RPM
    extern const std::atomic<float> &averageSpeed;
    // converted value from average_speed in km/h
    extern const std::atomic<float> &averageSpeedKmh;

    // average acceleration in m/s^2
    extern const std::atomic<float> &averageAcceleration;

    // total current in A
    extern const std::atomic<float> &totalCurrent;
} // namespace outputs

namespace can_external {
    // variables that can be read in from other modules via CAN
    extern const std::atomic<std::optional<int16_t>> &canGas;
    extern const std::atomic<std::optional<int16_t>> &canBrems;

    extern const std::atomic<espchrono::millis_clock::time_point> &lastCanGas;
    extern const std::atomic<espchrono::millis_clock::time_point> &lastCanBrems;
} // namespace can_external

extern bool can_initialized;

// initialize the CAN bus
void initCan();

// update the state and read the CAN bus
void updateCan();

// send commands to the motor controllers (usually done after updating the driving model)
void sendCanCommands();
} // namespace can
