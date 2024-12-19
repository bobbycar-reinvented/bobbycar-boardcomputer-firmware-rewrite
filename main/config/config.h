#pragma once

// sdkconfig
#include "sdkconfig.h"

// system includes
#include <array>

// 3rdparty lib includes
#include <configconstraints_base.h>
#include <configmanager.h>

// local includes
#include "profile.h"

#if CONFIG_BOBBYCAR_DEFAULT_PROFILE >= CONFIG_BOBBYCAR_PROFILE_NUM
#error "CONFIG_BOBBYCAR_DEFAULT_PROFILE must be less than or equal to CONFIG_BOBBYCAR_PROFILE_NUM"
#endif

namespace config {

using namespace espconfig;

namespace helpers {

#define REGISTER_CONFIG(name)                                                                                          \
    if (callable(name)) return true;

#define USE_CONSTRUCTOR using Constructor::Constructor

#define NO_CONSTRAINT                                                                                                  \
    ConfigConstraintReturnType checkValue(value_t value) const override                                                \
    {                                                                                                                  \
        return {};                                                                                                     \
    }

    template<typename T>
    class ConfigWrapperChangeableKeyAndIndex : public ConfigWrapper<T>
    {
        CPP_DISABLE_COPY_MOVE(ConfigWrapperChangeableKeyAndIndex)
        using Base = ConfigWrapper<T>;

    public:
        using Constructor = ConfigWrapperChangeableKeyAndIndex;
        using value_t = typename Base::value_t;
        using ConstraintCallback = typename Base::ConstraintCallback;

        ConfigWrapperChangeableKeyAndIndex(const size_t idx, const char *nvsKey) : m_idx{idx}, m_nvsKey{nvsKey}
        {
        }

        const char *nvsName() const override final
        {
            return (m_nvsKey + std::to_string(m_idx)).c_str();
        }

    private:
        size_t m_idx;
        const char *m_nvsKey;
    };

    template<typename T>
    class ConfigWrapperChangeableKey : public ConfigWrapper<T>
    {
        CPP_DISABLE_COPY_MOVE(ConfigWrapperChangeableKey)
        using Base = ConfigWrapper<T>;

    public:
        using value_t = typename Base::value_t;
        using ConstraintCallback = typename Base::ConstraintCallback;

        explicit ConfigWrapperChangeableKey(const char *nvsKey) : m_nvsKey{nvsKey}
        {
        }

        constexpr const char *nvsName() const override final
        {
            return m_nvsKey;
        }

    private:
        const char *m_nvsKey;
    };

    class ProfileConfig
    {
        CPP_DISABLE_COPY_MOVE(ProfileConfig)
    public:
        explicit ProfileConfig(const size_t idx) :
            limits{
                    .iMotMax{idx, "limits_iMotMax"},
                    .iDcMax{idx, "limits_iDcMax"},
                    .nMotMax{idx, "limits_nMotMax"},
                    .fieldWeakMax{idx, "limits_fWeakMax"},
                    .phaseAdvMax{idx, "limits_phaseAdvMax"},
            },
            controllerHardware{
                    .enableFrontLeft{idx, "ctrlHw_enFrontLeft"},
                    .enableFrontRight{idx, "ctrlHw_enFrontRight"},
                    .enableBackLeft{idx, "ctrlHw_enBackLeft"},
                    .enableBackRight{idx, "ctrlHw_enBackRight"},
                    .invertFrontLeft{idx, "ctrlHw_invFrontLeft"},
                    .invertFrontRight{idx, "ctrlHw_invFrontRight"},
                    .invertBackLeft{idx, "ctrlHw_invBackLeft"},
                    .invertBackRight{idx, "ctrlHw_invBackRight"},
            },
            defaultMode{
                    .modelMode{idx, "defMode_modelMode"},
                    .allowRemoteControl{idx, "defMode_allowRC"},
                    .squareGas{idx, "defMode_sqGas"},
                    .squareBrems{idx, "defMode_sqBrems"},
                    .enableSmoothingUp{idx, "defMode_enSmUp"},
                    .enableSmoothingDown{idx, "defMode_enSmDown"},
                    .enableFieldWeakSmoothingUp{idx, "defMode_enSmFUp"},
                    .enableFieldWeakSmoothingDown{idx, "defMode_enSmFDown"},
                    .smoothing{idx, "defMode_smoothing"},
                    .frontPercentage{idx, "defMode_frontPercent"},
                    .backPercentage{idx, "defMode_backPercent"},
                    .add_schwelle{idx, "defMode_addSchwelle"},
                    .gas1_wert{idx, "defMode_gas1_wert"},
                    .gas2_wert{idx, "defMode_gas2_wert"},
                    .brems1_wert{idx, "defMode_brems1_wert"},
                    .brems2_wert{idx, "defMode_brems2_wert"},
                    .fwSmoothLowerLimit{idx, "defMode_fwSmoothLowerLimit"},
            },
            m_idx{idx}
        {
        }

        // == Limits == //
        struct
        {
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return CONFIG_BOBBYCAR_DEFAULTS_IMOTMAX;
                }
            } iMotMax;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return CONFIG_BOBBYCAR_DEFAULTS_IDCMAX;
                }
            } iDcMax;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return CONFIG_BOBBYCAR_DEFAULTS_NMOTMAX;
                }
            } nMotMax;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return CONFIG_BOBBYCAR_DEFAULTS_FIELDWEAKMAX;
                }
            } fieldWeakMax;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return CONFIG_BOBBYCAR_DEFAULTS_FIELDADVMAX;
                }
            } phaseAdvMax;
        } limits;

        // == ControllerHardware == //
        struct
        {
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return true;
                }
            } enableFrontLeft;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return true;
                }
            } enableFrontRight;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return true;
                }
            } enableBackLeft;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return true;
                }
            } enableBackRight;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
#if defined(CONFIG_BOBBYCAR_DEFAULTS_INVERTFRONTLEFT) && CONFIG_BOBBYCAR_DEFAULTS_INVERTFRONTLEFT == 1
                    return true;
#else
                    return false;
#endif
                }
            } invertFrontLeft;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
#if defined(CONFIG_BOBBYCAR_DEFAULTS_INVERTFRONTRIGHT) && CONFIG_BOBBYCAR_DEFAULTS_INVERTFRONTRIGHT == 1
                    return true;
#else
                    return false;
#endif
                }
            } invertFrontRight;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
#if defined(CONFIG_BOBBYCAR_DEFAULTS_INVERTBACKLEFT) && CONFIG_BOBBYCAR_DEFAULTS_INVERTBACKLEFT == 1
                    return true;
#else
                    return false;
#endif
                }
            } invertBackLeft;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
#if defined(CONFIG_BOBBYCAR_DEFAULTS_INVERTBACKRIGHT) && CONFIG_BOBBYCAR_DEFAULTS_INVERTBACKRIGHT == 1
                    return true;
#else
                    return false;
#endif
                }
            } invertBackRight;
        } controllerHardware;

        // == DefaultMode == //
        struct
        {
            struct : ConfigWrapperChangeableKeyAndIndex<UnifiedModelMode>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return UnifiedModelMode::FocTorque;
                }
            } modelMode;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return true;
                }
            } allowRemoteControl;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return true;
                }
            } squareGas;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return true;
                }
            } squareBrems;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return true;
                }
            } enableSmoothingUp;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return true;
                }
            } enableSmoothingDown;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return false;
                }
            } enableFieldWeakSmoothingUp;
            struct : ConfigWrapperChangeableKeyAndIndex<bool>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return false;
                }
            } enableFieldWeakSmoothingDown;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return 20;
                }
            } smoothing;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT
                value_t defaultValue() const override
                {
                    return 100;
                }
            } frontPercentage;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT;
                value_t defaultValue() const override
                {
                    return 100;
                }
            } backPercentage;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT;
                value_t defaultValue() const override
                {
                    return 750;
                }
            } add_schwelle;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT;
                value_t defaultValue() const override
                {
                    return 1250;
                }
            } gas1_wert;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT;
                value_t defaultValue() const override
                {
                    return 1250;
                }
            } gas2_wert;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT;
                value_t defaultValue() const override
                {
                    return 250;
                }
            } brems1_wert;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT;
                value_t defaultValue() const override
                {
                    return 750;
                }
            } brems2_wert;
            struct : ConfigWrapperChangeableKeyAndIndex<int16_t>
            {
                USE_CONSTRUCTOR;

                bool allowReset() const override
                {
                    return true;
                }
                NO_CONSTRAINT;
                value_t defaultValue() const override
                {
                    return 800;
                }
            } fwSmoothLowerLimit;
        } defaultMode;

        bool callForEveryConfig(auto &&callable)
        {
            // == Limits == //
            REGISTER_CONFIG(limits.iMotMax)
            REGISTER_CONFIG(limits.iDcMax)
            REGISTER_CONFIG(limits.nMotMax)
            REGISTER_CONFIG(limits.fieldWeakMax)
            REGISTER_CONFIG(limits.phaseAdvMax)

            // == ControllerHardware == //
            REGISTER_CONFIG(controllerHardware.enableFrontLeft)
            REGISTER_CONFIG(controllerHardware.enableFrontRight)
            REGISTER_CONFIG(controllerHardware.enableBackLeft)
            REGISTER_CONFIG(controllerHardware.enableBackRight)
            REGISTER_CONFIG(controllerHardware.invertFrontLeft)
            REGISTER_CONFIG(controllerHardware.invertFrontRight)
            REGISTER_CONFIG(controllerHardware.invertBackLeft)
            REGISTER_CONFIG(controllerHardware.invertBackRight)

            // == DefaultMode == //
            REGISTER_CONFIG(defaultMode.modelMode)
            REGISTER_CONFIG(defaultMode.allowRemoteControl)
            REGISTER_CONFIG(defaultMode.squareGas)
            REGISTER_CONFIG(defaultMode.squareBrems)
            REGISTER_CONFIG(defaultMode.enableSmoothingUp)
            REGISTER_CONFIG(defaultMode.enableSmoothingDown)
            REGISTER_CONFIG(defaultMode.enableFieldWeakSmoothingUp)
            REGISTER_CONFIG(defaultMode.enableFieldWeakSmoothingDown)
            REGISTER_CONFIG(defaultMode.smoothing)
            REGISTER_CONFIG(defaultMode.frontPercentage)
            REGISTER_CONFIG(defaultMode.backPercentage)
            REGISTER_CONFIG(defaultMode.add_schwelle)
            REGISTER_CONFIG(defaultMode.gas1_wert)
            REGISTER_CONFIG(defaultMode.gas2_wert)
            REGISTER_CONFIG(defaultMode.brems1_wert)
            REGISTER_CONFIG(defaultMode.brems2_wert)
            REGISTER_CONFIG(defaultMode.fwSmoothLowerLimit)

            return false;
        }

    private:
        const size_t m_idx;
    };

} // namespace helpers

class ConfigContainer
{
public:
    std::array<helpers::ProfileConfig, CONFIG_BOBBYCAR_PROFILE_NUM> profiles{
#if CONFIG_BOBBYCAR_PROFILE_NUM >= 1
            helpers::ProfileConfig{0},
#endif
#if CONFIG_BOBBYCAR_PROFILE_NUM >= 2
            helpers::ProfileConfig{1},
#endif
#if CONFIG_BOBBYCAR_PROFILE_NUM >= 3
            helpers::ProfileConfig{2},
#endif
#if CONFIG_BOBBYCAR_PROFILE_NUM >= 4
            helpers::ProfileConfig{3},
#endif
#if CONFIG_BOBBYCAR_PROFILE_NUM >= 5
            helpers::ProfileConfig{4},
#endif
#if CONFIG_BOBBYCAR_PROFILE_NUM >= 6
            helpers::ProfileConfig{5},
#endif
#if CONFIG_BOBBYCAR_PROFILE_NUM >= 7
            helpers::ProfileConfig{6},
#endif
#if CONFIG_BOBBYCAR_PROFILE_NUM >= 8
            helpers::ProfileConfig{7},
#endif
#if CONFIG_BOBBYCAR_PROFILE_NUM >= 9
#error "CONFIG_BOBBYCAR_PROFILE_NUM must be less than or equal to 8"
#endif
    };

    struct : ConfigWrapper<uint8_t>
    {
        bool allowReset() const override
        {
            return true;
        }
        ConfigConstraintReturnType checkValue(value_t value) const override
        {
            return MinMaxValue<value_t, 0, CONFIG_BOBBYCAR_PROFILE_NUM - 1>(value);
        }
        value_t defaultValue() const override
        {
            return CONFIG_BOBBYCAR_DEFAULT_PROFILE;
        }
        const char *nvsName() const override
        {
            return "profileIdx";
        }
    } profileIndex;

    struct
    {
        struct : ConfigWrapper<int16_t>
        {
            bool allowReset() const override
            {
                return true;
            }
            ConfigConstraintReturnType checkValue(value_t value) const override
            {
                return MinValue<value_t, 1>(value);
            }
            value_t defaultValue() const override
            {
                return CONFIG_BOBBYCAR_DEFAULTS_WHEELDIAMETER;
            }
            const char *nvsName() const override
            {
                return "wheelDiameter";
            }
        } wheelDiameter;
        struct : ConfigWrapper<int16_t>
        {
            bool allowReset() const override
            {
                return true;
            }
            ConfigConstraintReturnType checkValue(value_t value) const override
            {
                return MinValue<value_t, 1>(value);
            }
            value_t defaultValue() const override
            {
                return CONFIG_BOBBYCAR_DEFAULTS_NUM_MAGNETIC_POLES;
            }
            const char *nvsName() const override
            {
                return "numMagnetPoles";
            }
        } wheelBase;
        struct : ConfigWrapper<bool>
        {
            bool allowReset() const override
            {
                return true;
            }
            NO_CONSTRAINT;
            value_t defaultValue() const override
            {
#if defined(CONFIG_BOBBYCAR_DEFAULTS_SWAPFRONTBACK) && CONFIG_BOBBYCAR_DEFAULTS_SWAPFRONTBACK == 1
                return true;
#else
                return false;
#endif
            }
            const char *nvsName() const override
            {
                return "swapFrontBack";
            }
        } swapFrontBack;
        struct : ConfigWrapper<bool>
        {
            bool allowReset() const override
            {
                return true;
            }
            NO_CONSTRAINT;
            value_t defaultValue() const override
            {
                return true;
            }
            const char *nvsName() const override
            {
                return "sendFrontCanCmd";
            }
        } sendFrontCanCmd;
        struct : ConfigWrapper<bool>
        {
            bool allowReset() const override
            {
                return true;
            }
            NO_CONSTRAINT;
            value_t defaultValue() const override
            {
                return true;
            }
            const char *nvsName() const override
            {
                return "sendBackCanCmd";
            }
        } sendBackCanCmd;
        struct : ConfigWrapper<bool>
        {
            bool allowReset() const override
            {
                return true;
            }
            NO_CONSTRAINT;
            value_t defaultValue() const override
            {
                return true;
            }
            const char *nvsName() const override
            {
                return "recvCanCmd";
            }
        } recvCanCmd;
        struct : ConfigWrapper<int16_t>
        {
            bool allowReset() const override
            {
                return true;
            }
            ConfigConstraintReturnType checkValue(value_t value) const override
            {
                return MinValue<value_t, 0>(value);
            }
            value_t defaultValue() const override
            {
                return CONFIG_BOBBYCAR_DEFAULTS_CAN_RECEIVE_TIMEOUT_MS;
            }
            const char *nvsName() const override
            {
                return "canTransmitTime";
            }
        } canTransmitTimeout;
        struct : ConfigWrapper<int16_t>
        {
            bool allowReset() const override
            {
                return true;
            }
            ConfigConstraintReturnType checkValue(value_t value) const override
            {
                return MinValue<value_t, 0>(value);
            }
            value_t defaultValue() const override
            {
                return CONFIG_BOBBYCAR_DEFAULTS_CAN_RECEIVE_TIMEOUT_MS;
            }
            const char *nvsName() const override
            {
                return "canReceiveTimeo";
            }
        } canReceiveTimeout;
        struct : ConfigWrapper<bool>
        {
            bool allowReset() const override
            {
                return true;
            }
            NO_CONSTRAINT;
            value_t defaultValue() const override
            {
#if defined(CONFIG_BOBBYCAR_DEFAULTS_CAN_UNINSTALL_ON_RESET) && CONFIG_BOBBYCAR_DEFAULTS_CAN_UNINSTALL_ON_RESET == 1
                return true;
#else
                return false;
#endif
            }
            const char *nvsName() const override
            {
                return "canUnstlRstErr";
            }
        } canUninstallOnReset;

        struct : ConfigWrapper<bool>
        {
            bool allowReset() const override
            {
                return true;
            }
            NO_CONSTRAINT;
            value_t defaultValue() const override
            {
#if defined(CONFIG_BOBBYCAR_DEFAULTS_CAN_RESET_ON_RESET) && CONFIG_BOBBYCAR_DEFAULTS_CAN_RESET_ON_RESET == 1
                return true;
#else
                return false;
#endif
            }
            const char *nvsName() const override
            {
                return "canBusRstErr";
            }
        } canBusResetOnError;
    } controllerHardware;

    template<typename T>
    bool callForEveryConfig(T &&callable)
    {
        for (auto &profile: profiles)
        {
            if (profile.callForEveryConfig(callable)) return true;
        }

        REGISTER_CONFIG(profileIndex)

        // == ControllerHardware == //
        REGISTER_CONFIG(controllerHardware.wheelDiameter)
        REGISTER_CONFIG(controllerHardware.wheelBase)
        REGISTER_CONFIG(controllerHardware.swapFrontBack)

        return false;
    }

    size_t getConfigCount()
    {
        size_t count{0};

        callForEveryConfig([&count](auto &) {
            count++;
            return false;
        });

        return count;
    }
};

extern ConfigManager<ConfigContainer> configs;

extern helpers::ProfileConfig* selectedProfile;

} // namespace config
