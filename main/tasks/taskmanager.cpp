#include "taskmanager.h"

// sdkconfig includes
#include "sdkconfig.h"

constexpr auto TAG = "taskmanager";

// esp-idf includes
#include <esp_log.h>

// 3rdparty lib includes
#include <espchrono.h>

// local includes
#include "can/can.h"

namespace {

[[maybe_unused]] void noop()
{
}

#if defined(CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_CAN) && defined(CONFIG_BOBBYCAR_CAN_UPDATE_INTERVAL_MS) &&          \
        CONFIG_BOBBYCAR_CAN_UPDATE_INTERVAL_MS > 32
#warning The specified BOBBYCAR_CAN_UPDATE_INTERVAL_MS is more than 32ms, meaning it will update less than 30 times per second. Use with caution.
#endif

using namespace espcpputils;
using namespace std::chrono_literals;

BobbySchedulerTask tasksArray[]{
#ifdef CONFIG_BOBBYCAR_COMMUNICATION_PROTOCOL_CAN
        BobbySchedulerTask{"can", can::initCan, can::updateCan, CONFIG_BOBBYCAR_CAN_UPDATE_INTERVAL_MS * 1ms, false},
#endif
};

} // namespace

cpputils::ArrayView<BobbySchedulerTask> tasks{tasksArray};

void sched_pushStats(const bool printTasks)
{
    if (printTasks) ESP_LOGI(TAG, "begin listing tasks...");

    for (auto &task: tasks) task.pushStats(printTasks);

    if (printTasks) ESP_LOGI(TAG, "end listing tasks");
}
