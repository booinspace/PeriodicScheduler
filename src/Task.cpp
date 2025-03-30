#include "Task.h"
#include "Network.h"
#include "Process.h"
#include "Common.h"

Task::Task()
{
    metrics_ = {};
    metrics_.min = std::numeric_limits<int32_t>::max();
    metrics_.max = 0;
    metrics_.avg = 0;
}

TaskMetrics Task::GetMetrics(void) const
{
    return metrics_;
}