#pragma once

// system includes
#include <string>

// 3rdparty lib includes
#include <espchrono.h>

namespace driving_modes {
class ModeInterface
{
public:
    virtual ~ModeInterface() = default;

    virtual void start() {};
    virtual void update() = 0;
    virtual void stop() {};

    virtual const std::string displayName() const = 0;

private:
    espchrono::millis_clock::time_point m_lastTime{espchrono::millis_clock::now()};
};
} // namespace driving_modes
