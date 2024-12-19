#include "unifiedmodelmode.h"

// esp-idf includes
#include <esp_log.h>

constexpr auto TAG = "UnifiedModelMode";

SplittedModelMode split(UnifiedModelMode mode)
{
    using bobbycar::protocol::ControlMode;
    using bobbycar::protocol::ControlType;

    switch (mode)
    {
        case UnifiedModelMode::Commutation:
            return std::make_pair(ControlType::Commutation, ControlMode::Voltage);
        case UnifiedModelMode::Sinusoidal:
            return std::make_pair(ControlType::Sinusoidal, ControlMode::Voltage);
        case UnifiedModelMode::FocVoltage:
            return std::make_pair(ControlType::FieldOrientedControl, ControlMode::Voltage);
        case UnifiedModelMode::FocSpeed:
            return std::make_pair(ControlType::FieldOrientedControl, ControlMode::Speed);
        case UnifiedModelMode::FocTorque:
            return std::make_pair(ControlType::FieldOrientedControl, ControlMode::Torque);
    }

    ESP_LOGD(TAG, "Unknown UnifiedModelMode: %i", int(mode));

    return std::make_pair(ControlType::FieldOrientedControl, ControlMode::OpenMode);
}

std::string toString(UnifiedModelMode mode)
{
    switch (mode)
    {
        case UnifiedModelMode::Commutation:
            return "Commutation";
        case UnifiedModelMode::Sinusoidal:
            return "Sinusoidal";
        case UnifiedModelMode::FocVoltage:
            return "FocVoltage";
        case UnifiedModelMode::FocSpeed:
            return "FocSpeed";
        case UnifiedModelMode::FocTorque:
            return "FocTorque";
    }

    ESP_LOGD(TAG, "Unknown UnifiedModelMode: %i", int(mode));

    return "Unknown";
}
