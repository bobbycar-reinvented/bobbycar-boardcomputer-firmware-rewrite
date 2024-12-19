#pragma once

// 3rdparty lib includes
#include <bobbycar-serial.h>
#include <espchrono.h>

// local includes
#include "config/config.h"
#include "modeinterface.h"

#if defined(CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_CAN) && defined(CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_UART)
#error "CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_CAN and CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_UART are mutually exclusive"
#endif

#if !defined(CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_CAN) && !defined(CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_UART)
#error "At least one of CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_CAN and CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_UART must be defined"
#endif

#ifdef CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_UART
#warning "UART communication is deprecated and might not even compile"
#endif

struct Controller
{
    Controller(
#ifdef CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_UART
            HardwareSerial &serial,
#endif
    );

#ifdef CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_UART
    std::reference_wrapper<HardwareSerial> serial;
#endif

    bobbycar::protocol::serial::Command command{};

#ifdef CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_CAN
    espchrono::millis_clock::time_point lastCanFeedback{};
#endif
    bool feedbackValid{};
    bobbycar::protocol::serial::Feedback feedback{};


#ifdef CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_UART
    FeedbackParser parser{serial, feedbackValid, feedback};
#endif

    float getCalibratedVoltage() const;
};

class Controllers : std::array<Controller, 2>
{
public:
    Controllers(const Controllers &) = delete;
    Controllers &operator=(const Controllers &) = delete;

    void front() = delete;
    void back() = delete;

    Controller &unswapped_front{operator[](0)};
    Controller &unswapped_back{operator[](1)};

    explicit Controllers() : std::array<Controller, 2>{}
    {}

    Controller &correctedFront() const
    {
        using namespace config;

        return configs.controllerHardware.swapFrontBack.value() ? unswapped_back : unswapped_front;
    }
    Controller &correctedBack() const
    {
        using namespace config;

        return configs.controllerHardware.swapFrontBack.value() ? unswapped_front : unswapped_back;
    }
};

extern Controllers controllers;

struct FrontControllerGetter { static Controller &get() { return controllers.unswapped_front; }};
struct BackControllerGetter { static Controller &get() { return controllers.unswapped_back; }};

extern driving_modes::ModeInterface *lastMode;
extern driving_modes::ModeInterface *currentMode;