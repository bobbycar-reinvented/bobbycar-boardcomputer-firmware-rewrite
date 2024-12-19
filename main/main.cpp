// esp-idf includes
#include "config/config.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>

namespace init {

RTC_NOINIT_ATTR bool recovery;

} // namespace init

extern "C" [[noreturn]] void app_main()
{
    using namespace config;

    ESP_LOGI("main", "Hello, world!");

    // == Bobbycar Settings == //
    if (const auto result = configs.init("bobbycar"); result != ESP_OK)
    {
        ESP_LOGE("main", "config_init_settings() failed with %s", esp_err_to_name(result));
        abort();
    }

    ESP_LOGI("main", "config_init_settings() succeeded");

    // == Selected Profile == //
    const auto selectedProfileIndex = configs.profileIndex.value();

    if (selectedProfileIndex >= configs.profiles.size())
    {
        ESP_LOGE("main", "Selected profile index %d is out of bounds", selectedProfileIndex);

        abort();
    }

    selectedProfile = &configs.profiles[selectedProfileIndex];

    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
