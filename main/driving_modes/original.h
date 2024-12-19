#pragma once

// local includes
#include "driving_modes/modeinterface.h"

namespace driving_modes {

class OriginalMode final : public ModeInterface
{
    using Base = ModeInterface;

public:
    void start() override;

    void update() override;

    const std::string displayName() const override
    {
        return "Original";
    }
};

extern OriginalMode originalMode;
} // namespace driving_modes
