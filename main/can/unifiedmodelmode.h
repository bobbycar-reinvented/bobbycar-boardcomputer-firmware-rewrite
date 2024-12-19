#pragma once

// system includes
#include <string>
#include <utility>

// 3rdparty lib includes
#include <bobbycar-common.h>

enum class UnifiedModelMode : uint8_t
{
    Commutation,
    Sinusoidal,
    FocVoltage,
    FocSpeed,
    FocTorque
};

using SplittedModelMode = std::pair<bobbycar::protocol::ControlType, bobbycar::protocol::ControlMode>;

SplittedModelMode split(UnifiedModelMode mode);

std::string toString(UnifiedModelMode mode);
