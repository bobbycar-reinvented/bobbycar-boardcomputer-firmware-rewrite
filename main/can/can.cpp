#include "can.h"

constexpr auto TAG = "BOBBYCAN";

// sdkconfig includes
#include "sdkconfig.h"

// esp-idf includes
#include <driver/twai.h>
#include <esp_log.h>

// 3rdparty lib includes
#include <bobbycar-can.h>
#include <tickchrono.h>

// local includes
#include "config/config.h"
#include "driving_modes/controllers.h"

namespace can {

using namespace std::chrono_literals;

namespace inputs {
    std::atomic<std::optional<int16_t>> rawGas;
    std::atomic<std::optional<int16_t>> rawBrems;

    std::atomic<std::optional<float>> gas;
    std::atomic<std::optional<float>> brems;
} // namespace inputs

namespace outputs {
    // value from motor controller in RPM
    std::atomic<float> _averageSpeed;
    const std::atomic<float> &averageSpeed{_averageSpeed};

    // converted value from average_speed in km/h
    std::atomic<float> _averageSpeedKmh;
    const std::atomic<float> &averageSpeedKmh{_averageSpeedKmh};

    // average acceleration in m/s^2
    std::atomic<float> _averageAcceleration;
    const std::atomic<float> &averageAcceleration{_averageAcceleration};

    // total current in A
    std::atomic<float> _totalCurrent;
    const std::atomic<float> &totalCurrent{_totalCurrent};
} // namespace outputs

namespace can_external {
    // variables that can be read in from other modules via CAN
    std::atomic<std::optional<int16_t>> _canGas;
    const std::atomic<std::optional<int16_t>> &canGas{_canGas};

    std::atomic<std::optional<int16_t>> _canBrems;
    const std::atomic<std::optional<int16_t>> &canBrems{_canBrems};

    std::atomic<espchrono::millis_clock::time_point> _lastCanGas;
    const std::atomic<espchrono::millis_clock::time_point> &lastCanGas{_lastCanGas};

    std::atomic<espchrono::millis_clock::time_point> _lastCanBrems;
    const std::atomic<espchrono::millis_clock::time_point> &lastCanBrems{_lastCanBrems};
} // namespace can_external

bool can_initialized{false};

namespace {

    uint32_t can_sequential_error_cnt{0};
    uint32_t can_sequential_bus_errors{0};
    uint32_t can_total_error_cnt{0};

    constexpr auto CAN_TIMEOUT = CONFIG_BOBBYCAR_CAN_CONTROLLER_VALID_TIMEOUT_MS * 1ms;

    enum class ControllerType
    {
        Front,
        Back
    };

} // namespace

template<ControllerType Type>
bool parseMotorControllerCanMessage(const twai_message_t &message, Controller &controller)
{
    constexpr auto isBack = Type == ControllerType::Back;

    switch (message.identifier)
    {
        using namespace bobbycar::protocol::can;
        case MotorController<isBack, false>::Feedback::DcLink:
            controller.feedback.left.dcLink = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, true>::Feedback::DcLink:
            controller.feedback.right.dcLink = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::Speed:
            controller.feedback.left.speed = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, true>::Feedback::Speed:
            controller.feedback.right.speed = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::Error:
            controller.feedback.left.error = *((int8_t *) message.data);
            return true;
        case MotorController<isBack, true>::Feedback::Error:
            controller.feedback.right.error = *((int8_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::Angle:
            controller.feedback.left.angle = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, true>::Feedback::Angle:
            controller.feedback.right.angle = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::DcPhaA:
            controller.feedback.left.dcPhaA = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, true>::Feedback::DcPhaA:
            controller.feedback.right.dcPhaA = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::DcPhaB:
            controller.feedback.left.dcPhaB = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, true>::Feedback::DcPhaB:
            controller.feedback.right.dcPhaB = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::DcPhaC:
            controller.feedback.left.dcPhaC = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, true>::Feedback::DcPhaC:
            controller.feedback.right.dcPhaC = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::Chops:
            controller.feedback.left.chops = *((uint16_t *) message.data);
            return true;
        case MotorController<isBack, true>::Feedback::Chops:
            controller.feedback.right.chops = *((uint16_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::Hall:
            controller.feedback.left.hallA = *((uint8_t *) message.data) & 1;
            controller.feedback.left.hallB = *((uint8_t *) message.data) & 2;
            controller.feedback.left.hallC = *((uint8_t *) message.data) & 4;
            return true;
        case MotorController<isBack, true>::Feedback::Hall:
            controller.feedback.right.hallA = *((uint8_t *) message.data) & 1;
            controller.feedback.right.hallB = *((uint8_t *) message.data) & 2;
            controller.feedback.right.hallC = *((uint8_t *) message.data) & 4;
            return true;
        case MotorController<isBack, false>::Feedback::Voltage:
        case MotorController<isBack, true>::Feedback::Voltage:
            controller.feedback.batVoltage = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::Temp:
        case MotorController<isBack, true>::Feedback::Temp:
            controller.feedback.boardTemp = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::Id:
            controller.feedback.left.id = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, true>::Feedback::Id:
            controller.feedback.right.id = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, false>::Feedback::Iq:
            controller.feedback.left.iq = *((int16_t *) message.data);
            return true;
        case MotorController<isBack, true>::Feedback::Iq:
            controller.feedback.right.iq = *((int16_t *) message.data);
            return true;
        default:
            return false;
    }
}

bool parseBoardcomputerCanMessage(const twai_message_t &message)
{
    switch (message.identifier)
    {
        using namespace bobbycar::protocol::can;
        case Boardcomputer::Command::RawButtonPressed:
            // if (espgui::currentDisplay) espgui::currentDisplay->rawButtonPressed(*((const uint8_t *) message.data));
            return true;
        case Boardcomputer::Command::RawButtonReleased:
            // if (espgui::currentDisplay) espgui::currentDisplay->rawButtonReleased(*((const uint8_t *) message.data));
            return true;
        case Boardcomputer::Command::ButtonPressed:
            // if (espgui::currentDisplay)
            //     espgui::currentDisplay->buttonPressed(espgui::Button(*((const uint8_t *) message.data)));
            return true;
        case Boardcomputer::Command::ButtonReleased:
            // if (espgui::currentDisplay)
            //     espgui::currentDisplay->buttonReleased(espgui::Button(*((const uint8_t *) message.data)));
            return true;
        case Boardcomputer::Command::RawGas:
            // can_gas = *((int16_t *) message.data);
            // last_can_gas = espchrono::millis_clock::now();
            return true;
        case Boardcomputer::Command::RawBrems:
            // can_brems = *((int16_t *) message.data);
            // last_can_brems = espchrono::millis_clock::now();
            return true;
        default:
            return false;
    }
}

bool tryParseCanInput()
{
    using namespace config;

    twai_message_t message;

    const auto timeout = configs.controllerHardware.canReceiveTimeout.value();

    if (const auto receiveResult = twai_receive(&message, timeout); receiveResult != ESP_OK)
    {
        if (receiveResult != ESP_ERR_TIMEOUT)
        {
            ESP_LOGE(TAG, "twai_receive() failed with %s", esp_err_to_name(receiveResult));
        }

        if (espchrono::millis_clock::now() - controllers.unswapped_front.lastCanFeedback > CAN_TIMEOUT)
        {
            controllers.unswapped_front.feedbackValid = false;
        }
        else
        {
            ESP_LOGD(TAG, "frontTimeout: %lldms, %s", espchrono::ago(controllers.unswapped_front.lastCanFeedback) / 1ms,
                     esp_err_to_name(receiveResult));
        }

        if (espchrono::millis_clock::now() - controllers.unswapped_back.lastCanFeedback > CAN_TIMEOUT)
        {
            controllers.unswapped_back.feedbackValid = false;
        }
        else
        {
            ESP_LOGD(TAG, "backTimeout: %lldms, %s", espchrono::ago(controllers.unswapped_back.lastCanFeedback) / 1ms,
                     esp_err_to_name(receiveResult));
        }

        if (!controllers.unswapped_front.feedbackValid || !controllers.unswapped_back.feedbackValid)
        {
            return false;
        }
    }

    if (!configs.controllerHardware.recvCanCmd.value())
    {
        if (espchrono::millis_clock::now() - controllers.unswapped_front.lastCanFeedback > CAN_TIMEOUT)
            controllers.unswapped_front.feedbackValid = false;

        if (espchrono::millis_clock::now() - controllers.unswapped_back.lastCanFeedback > CAN_TIMEOUT)
            controllers.unswapped_back.feedbackValid = false;

        return false;
    }

    auto &front = controllers.correctedFront();
    auto &back = controllers.correctedBack();

    if (parseMotorControllerCanMessage<ControllerType::Front>(message, front))
    {
        if (espchrono::millis_clock::now() - back.lastCanFeedback > CAN_TIMEOUT) back.feedbackValid = false;

        front.lastCanFeedback = espchrono::millis_clock::now();
        front.feedbackValid = true;
        return true;
    }

    if (espchrono::millis_clock::now() - front.lastCanFeedback > CAN_TIMEOUT) front.feedbackValid = false;

    if (parseMotorControllerCanMessage<ControllerType::Back>(message, back))
    {
        back.lastCanFeedback = espchrono::millis_clock::now();
        back.feedbackValid = true;
        return true;
    }

    if (espchrono::millis_clock::now() - back.lastCanFeedback > CAN_TIMEOUT) back.feedbackValid = false;

    if (parseBoardcomputerCanMessage(message)) return true;

    ESP_LOGW(TAG, "Unknown CAN info received .identifier = %lu", message.identifier);

    return true;
}

void initCan()
{
    can_initialized = true;

    ESP_LOGI(TAG, "Initializing CAN bus...");

    constexpr twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NORMAL);
    constexpr twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    constexpr twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (const auto installResult = twai_driver_install(&g_config, &t_config, &f_config); installResult == ESP_OK)
    {
        ESP_LOGI(TAG, "twai_driver_install() succeeded");
    }
    else
    {
        ESP_LOGE(TAG, "twai_driver_install() failed with %s", esp_err_to_name(installResult));
        return;
    }

    if (const auto startResult = twai_start(); startResult == ESP_OK)
    {
        ESP_LOGI(TAG, "twai_start() succeeded");
    }
    else
    {
        ESP_LOGE(TAG, "twai_start() failed with %s", esp_err_to_name(startResult));

        if (const auto uninstallResult = twai_driver_uninstall(); uninstallResult == ESP_OK)
        {
            ESP_LOGI(TAG, "twai_driver_uninstall() succeeded");
        }
        else
        {
            ESP_LOGE(TAG, "twai_driver_uninstall() failed with %s", esp_err_to_name(uninstallResult));

            return;
        }

        return;
    }

    ESP_LOGI(TAG, "CAN bus initialized");

    can_initialized = true;
}

void updateCan()
{
    for (int i = 0; i < 4; i++)
    {
        if (!tryParseCanInput())
        {
            break;
        }
    }
}

esp_err_t sendCommand(const uint32_t addr, auto value)
{
    using namespace config;

    twai_message_t message;
    twai_status_info_t status_info;

    message.identifier = addr;
    message.flags = TWAI_MSG_FLAG_SS;
    message.data_length_code = sizeof(value);

    std::ranges::fill(message.data, 0);
    std::memcpy(message.data, &value, sizeof(value));

    const auto timeout = configs.controllerHardware.canTransmitTimeout.value();

    const auto timestamp_before = espchrono::millis_clock::now();
    const auto result = twai_transmit(&message, timeout);
    const auto status = twai_get_status_info(&status_info);
    const auto timestamp_after = espchrono::millis_clock::now();

    if ((result == ESP_ERR_TIMEOUT || status_info.state == TWAI_STATE_BUS_OFF) ||
        (status == ESP_OK && status_info.bus_error_count > can_sequential_bus_errors))
    {
        ++can_sequential_error_cnt;
        ++can_total_error_cnt;
        can_sequential_bus_errors = status_info.bus_error_count;

        if (can_total_error_cnt < 500 &&
            (configs.controllerHardware.canUninstallOnReset.value() && can_total_error_cnt < 100))
            ESP_LOGW(TAG, "twai_transmit() failed after %lldms with %s, seq err: %lu, total err: %lu",
                     std::chrono::floor<std::chrono::milliseconds>(timestamp_after - timestamp_before).count(),
                     esp_err_to_name(result), can_sequential_error_cnt, can_total_error_cnt);
    }
    else if (result != ESP_OK)
    {
        ESP_LOGE(TAG, "twai_transmit() failed after %lldms with %s",
                 std::chrono::floor<std::chrono::milliseconds>(timestamp_after - timestamp_before).count(),
                 esp_err_to_name(result));
    }
    else
    {
        can_sequential_error_cnt = 0;
    }

    if (can_sequential_error_cnt > CONFIG_BOBBYCAR_CAN_CONTROLLER_MAX_ERROR_COUNT)
    {
        can_sequential_error_cnt = 0;
        if (configs.controllerHardware.canBusResetOnError.value())
        {
            ESP_LOGW(TAG, "Something isn't right, trying to restart can ic...");
            if (const auto err = twai_stop(); err != ESP_OK)
            {
                ESP_LOGE(TAG, "twai_stop() failed with %s", esp_err_to_name(err));
            }
            if (configs.controllerHardware.canUninstallOnReset.value())
            {
                if (const auto err = twai_driver_uninstall(); err != ESP_OK)
                {
                    ESP_LOGE(TAG, "twai_driver_uninstall() failed with %s", esp_err_to_name(err));
                }

                constexpr twai_general_config_t g_config =
                        TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NORMAL);
                constexpr twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
                constexpr twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

                if (const auto err = twai_driver_install(&g_config, &t_config, &f_config); err != ESP_OK)
                {
                    ESP_LOGE(TAG, "twai_driver_install() failed with %s", esp_err_to_name(err));
                }
            }
            if (const auto err = twai_start(); err != ESP_OK)
            {
                ESP_LOGE(TAG, "twai_start() failed with %s", esp_err_to_name(err));
            }
        }
    }
    return result;
};

void sendCanCommands()
{
    using namespace config;

    if (!configs.controllerHardware.sendFrontCanCmd.value() && !configs.controllerHardware.sendBackCanCmd.value())
        return;

    const bool swap = configs.controllerHardware.swapFrontBack.value();
    const Controller *front = (swap ? configs.controllerHardware.sendBackCanCmd.value()
                                    : configs.controllerHardware.sendFrontCanCmd.value())
                                      ? (swap ? &controllers.unswapped_back : &controllers.unswapped_front)
                                      : nullptr;
    const Controller *back = (swap ? configs.controllerHardware.sendFrontCanCmd.value()
                                   : configs.controllerHardware.sendBackCanCmd.value())
                                     ? (swap ? &controllers.unswapped_front : &controllers.unswapped_back)
                                     : nullptr;

    using namespace bobbycar::protocol::can;

    if (front) sendCommand(MotorController<false, false>::Command::InpTgt, front->command.left.pwm);
    if (front) sendCommand(MotorController<false, true>::Command::InpTgt, front->command.right.pwm);
    if (back) sendCommand(MotorController<true, false>::Command::InpTgt, back->command.left.pwm);
    if (back) sendCommand(MotorController<true, true>::Command::InpTgt, back->command.right.pwm);

    uint16_t buttonLeds{};
    if (const auto index = configs.profileIndex.value())
    {
        switch (index)
        {
            case 0:
                buttonLeds |= std::to_underlying(Boardcomputer::Button::Profile0);
                break;
            case 1:
                buttonLeds |= std::to_underlying(Boardcomputer::Button::Profile1);
                break;
            case 2:
                buttonLeds |= std::to_underlying(Boardcomputer::Button::Profile2);
                break;
            case 3:
                buttonLeds |= std::to_underlying(Boardcomputer::Button::Profile3);
                break;
            default:
                break;
        }
    }

    static struct
    {
        struct
        {
            struct
            {
                int16_t nCruiseMotTgt{};
                bool cruiseCtrlEna{};
            } left, right;
            uint8_t freq{};
            uint8_t pattern{};
        } front, back;
        std::underlying_type_t<Boardcomputer::Button> buttonLeds{};
    } lastValues;

    static int i{};

    // anti aufklatsch when tempomat
    if ((front && front->command.left.nCruiseMotTgt != lastValues.front.left.nCruiseMotTgt) ||
        (front && front->command.right.nCruiseMotTgt != lastValues.front.right.nCruiseMotTgt) ||
        (back && back->command.left.nCruiseMotTgt != lastValues.back.left.nCruiseMotTgt) ||
        (back && back->command.right.nCruiseMotTgt != lastValues.back.right.nCruiseMotTgt))
        i = 8;
    else if ((front && front->command.left.cruiseCtrlEna != lastValues.front.left.cruiseCtrlEna) ||
             (front && front->command.right.cruiseCtrlEna != lastValues.front.right.cruiseCtrlEna) ||
             (back && back->command.left.cruiseCtrlEna != lastValues.back.left.cruiseCtrlEna) ||
             (back && back->command.right.cruiseCtrlEna != lastValues.back.right.cruiseCtrlEna))
        i = 9;
    else if ((front && front->command.buzzer.freq != lastValues.front.freq) ||
             (front && front->command.buzzer.pattern != lastValues.front.pattern) ||
             (back && back->command.buzzer.freq != lastValues.back.freq) ||
             (back && back->command.buzzer.pattern != lastValues.back.pattern))
        i = 10;
    else if (buttonLeds != lastValues.buttonLeds)
        i = 12;

    switch (i++)
    {
        case 0: {
            if (front) sendCommand(MotorController<false, false>::Command::Enable, front->command.left.enable);
            if (front) sendCommand(MotorController<false, true>::Command::Enable, front->command.right.enable);
            if (back) sendCommand(MotorController<true, false>::Command::Enable, back->command.left.enable);
            if (back) sendCommand(MotorController<true, true>::Command::Enable, back->command.right.enable);
            break;
        }
        case 1: {
            if (front) sendCommand(MotorController<false, false>::Command::CtrlTyp, front->command.left.ctrlTyp);
            if (front) sendCommand(MotorController<false, true>::Command::CtrlTyp, front->command.right.ctrlTyp);
            if (back) sendCommand(MotorController<true, false>::Command::CtrlTyp, back->command.left.ctrlTyp);
            if (back) sendCommand(MotorController<true, true>::Command::CtrlTyp, back->command.right.ctrlTyp);
            break;
        }
        case 2: {
            if (front) sendCommand(MotorController<false, false>::Command::CtrlMod, front->command.left.ctrlMod);
            if (front) sendCommand(MotorController<false, true>::Command::CtrlMod, front->command.right.ctrlMod);
            if (back) sendCommand(MotorController<true, false>::Command::CtrlMod, back->command.left.ctrlMod);
            if (back) sendCommand(MotorController<true, true>::Command::CtrlMod, back->command.right.ctrlMod);
            // handbremse::finishedMotorUpdate = true;
            break;
        }
        case 3: {
            if (front) sendCommand(MotorController<false, false>::Command::IMotMax, front->command.left.iMotMax);
            if (front) sendCommand(MotorController<false, true>::Command::IMotMax, front->command.right.iMotMax);
            if (back) sendCommand(MotorController<true, false>::Command::IMotMax, back->command.left.iMotMax);
            if (back) sendCommand(MotorController<true, true>::Command::IMotMax, back->command.right.iMotMax);
            break;
        }
        case 4: {
            if (front) sendCommand(MotorController<false, false>::Command::IDcMax, front->command.left.iDcMax);
            if (front) sendCommand(MotorController<false, true>::Command::IDcMax, front->command.right.iDcMax);
            if (back) sendCommand(MotorController<true, false>::Command::IDcMax, back->command.left.iDcMax);
            if (back) sendCommand(MotorController<true, true>::Command::IDcMax, back->command.right.iDcMax);
            break;
        }
        case 5: {
            if (front) sendCommand(MotorController<false, false>::Command::NMotMax, front->command.left.nMotMax);
            if (front) sendCommand(MotorController<false, true>::Command::NMotMax, front->command.right.nMotMax);
            if (back) sendCommand(MotorController<true, false>::Command::NMotMax, back->command.left.nMotMax);
            if (back) sendCommand(MotorController<true, true>::Command::NMotMax, back->command.right.nMotMax);
            break;
        }
        case 6: {
            if (front)
                sendCommand(MotorController<false, false>::Command::FieldWeakMax, front->command.left.fieldWeakMax);
            if (front)
                sendCommand(MotorController<false, true>::Command::FieldWeakMax, front->command.right.fieldWeakMax);
            if (back) sendCommand(MotorController<true, false>::Command::FieldWeakMax, back->command.left.fieldWeakMax);
            if (back) sendCommand(MotorController<true, true>::Command::FieldWeakMax, back->command.right.fieldWeakMax);
            break;
        }
        case 7: {
            if (front)
                sendCommand(MotorController<false, false>::Command::PhaseAdvMax, front->command.left.phaseAdvMax);
            if (front)
                sendCommand(MotorController<false, true>::Command::PhaseAdvMax, front->command.right.phaseAdvMax);
            if (back) sendCommand(MotorController<true, false>::Command::PhaseAdvMax, back->command.left.phaseAdvMax);
            if (back) sendCommand(MotorController<true, true>::Command::PhaseAdvMax, back->command.right.phaseAdvMax);
            break;
        }
        case 8: {
            if (front)
                sendCommand(MotorController<false, false>::Command::CruiseMotTgt, front->command.left.nCruiseMotTgt);
            if (front)
                sendCommand(MotorController<false, true>::Command::CruiseMotTgt, front->command.right.nCruiseMotTgt);
            if (back)
                sendCommand(MotorController<true, false>::Command::CruiseMotTgt, back->command.left.nCruiseMotTgt);
            if (back)
                sendCommand(MotorController<true, true>::Command::CruiseMotTgt, back->command.right.nCruiseMotTgt);
            break;
        }
        case 9: {
            if (front)
                sendCommand(MotorController<false, false>::Command::CruiseCtrlEna, front->command.left.cruiseCtrlEna);
            if (front)
                sendCommand(MotorController<false, true>::Command::CruiseCtrlEna, front->command.right.cruiseCtrlEna);
            if (back)
                sendCommand(MotorController<true, false>::Command::CruiseCtrlEna, back->command.left.cruiseCtrlEna);
            if (back)
                sendCommand(MotorController<true, true>::Command::CruiseCtrlEna, back->command.right.cruiseCtrlEna);
            break;
        }
        case 10: {
            if (front &&
                sendCommand(MotorController<false, false>::Command::BuzzerFreq, front->command.buzzer.freq) == ESP_OK)
                lastValues.front.freq = front->command.buzzer.freq;
            if (back &&
                sendCommand(MotorController<true, false>::Command::BuzzerFreq, back->command.buzzer.freq) == ESP_OK)
                lastValues.back.freq = back->command.buzzer.freq;
            if (front && sendCommand(MotorController<false, false>::Command::BuzzerPattern,
                                     front->command.buzzer.pattern) == ESP_OK)
                lastValues.front.pattern = front->command.buzzer.pattern;
            if (back && sendCommand(MotorController<true, false>::Command::BuzzerPattern,
                                    back->command.buzzer.pattern) == ESP_OK)
                lastValues.back.pattern = back->command.buzzer.pattern;
            return;
        }
        case 11: {
            if (front) sendCommand(MotorController<false, false>::Command::Led, front->command.led);
            if (back) sendCommand(MotorController<true, false>::Command::Led, back->command.led);
            if (front) sendCommand(MotorController<false, false>::Command::Poweroff, front->command.poweroff);
            if (back) sendCommand(MotorController<true, false>::Command::Poweroff, back->command.poweroff);
            return;
        }
        case 12:
            if (sendCommand(Boardcomputer::Feedback::ButtonLeds, buttonLeds) == ESP_OK)
                lastValues.buttonLeds = buttonLeds;
        [[fallthrough]];
        default:
            i = 0;
            break;
    }
}

} // namespace can
