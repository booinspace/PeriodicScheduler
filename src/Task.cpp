#include "Task.h"
#include "Network.h"
#include "Process.h"
#include "Common.h"

Task::Task()
: metrics_{}
{
}

TaskMetrics Task::GetMetrics(void) const
{
    return metrics_;
}