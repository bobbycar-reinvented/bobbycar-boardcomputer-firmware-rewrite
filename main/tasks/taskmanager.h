#pragma once

// 3rdparty lib includes
#include <arrayview.h>

// local includes
#include "utils/bobbyschedulertask.h"

extern cpputils::ArrayView<BobbySchedulerTask> tasks;

void sched_pushStats(bool printTasks);