#include "Task.h"
#include "Network.h"
#include "Process.h"
#include "Common.h"

//! @brief Constructor for a Task
Task::Task(const std::chrono::milliseconds quantum)
: quantum_(quantum)
{
    metrics_ = {};
    metrics_.min = std::numeric_limits<int32_t>::max();
    metrics_.max = 0;
    metrics_.avg = 0;
}

//! @brief Get the task metrics
TaskMetrics Task::GetMetrics(void) const
{
    return metrics_;
}