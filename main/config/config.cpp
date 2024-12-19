#include "config.h"

// 3rdparty lib includes
#include <configmanager_priv.h>

namespace config {
ConfigManager<ConfigContainer> configs;

helpers::ProfileConfig *selectedProfile = &configs.profiles[0];

void switchProfile(const uint8_t index)
{
    if (index >= CONFIG_BOBBYCAR_PROFILE_NUM)
    {
        ESP_LOGE(TAG, "Selected profile index %d is out of bounds", index);
        return;
    }

    selectedProfile = &configs.profiles[index];
}

} // namespace config

INSTANTIATE_CONFIGMANAGER_TEMPLATES(config::ConfigContainer)
