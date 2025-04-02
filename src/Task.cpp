#include "Task.h"
#include "Common.h"

#include <fstream>
#include <sstream>
#include <vector>

//! Maintain a static mutex for the metrics file
static std::mutex metrics_file_mutex_;

//! Maintain a maximum queue for historic metrics
static constexpr uint32_t kMaxTaskMetrics = 5;

//! @brief Constructor for a Task
Task::Task(const std::chrono::milliseconds quantum, const std::string& name)
: quantum_(quantum),
  name_(name)
{
    metrics_ = {};
    metrics_.min = std::numeric_limits<int32_t>::max();
    metrics_.max = 0;
    metrics_.avg = 0;

    std::ifstream in_file("metrics", std::ios::in);
    if (in_file.is_open())
    {
        std::string line;
        while (std::getline(in_file, line))
        {
            std::istringstream ss(line);
            std::string task_name;
            if (std::getline(ss, task_name, ':'))
            {
                if (task_name == name)
                {
                    STATUS_LOG("Loaded metrics for %s\n", name.c_str());
                    ss >> metrics_.min >> metrics_.avg >> metrics_.max;
                }
            }
        }
        in_file.close();
    }
}

//! @brief Get the task metrics
TaskMetrics Task::GetMetrics(void) const
{
    return metrics_;
}

//! @brief Get the name of the task
const std::string& Task::GetName()
{
    return name_;
}

void Task::CalculateMetrics()
{
    // Work out the task time by adding the difference from all executions of this task until completion
    int32_t time = 0;
    for (uint32_t i = 0; i < start_.size(); i++)
    {
        time += std::chrono::duration_cast<std::chrono::milliseconds>(end_[i] - start_[i]).count();
    }

    TaskMetrics metrics;
    metrics.min = std::min(time, metrics_.min);
    metrics.max = std::max(time, metrics_.max);

    // Remove entries to maintain max queue size
    if (task_times_.size() > kMaxTaskMetrics)
    {
        task_times_.pop_front();
    }
    task_times_.push_back(time);

    // Calculate the mean for average, if restarted we need to fill the queue again
    int32_t total_time = 0;
    for (int32_t time : task_times_)
    {
        total_time += time;
    }
    metrics.avg = total_time / task_times_.size();

    UpdateMetrics(metrics);
}

void Task::UpdateMetrics(TaskMetrics& metrics)
{
    std::lock_guard<std::mutex> lock(metrics_file_mutex_);
    metrics_ = metrics;

    std::ifstream in_file("metrics", std::ios::in);
    std::vector<std::string> lines;
    bool updated = false;

    if (in_file.is_open()) {
        std::string line;
        // Read all lines and store them
        while (std::getline(in_file, line)) {
            std::istringstream iss(line);
            std::string task_name;
            if (std::getline(iss, task_name, ':')) {
                if (task_name == name_) {
                    // If the task name matches, update the line with new data
                    std::ostringstream updatedLine;
                    updatedLine << name_ << ": " << metrics.min << " " << metrics.avg << " " << metrics.max;
                    lines.push_back(updatedLine.str());
                    updated = true;
                } else {
                    lines.push_back(line);
                }
            }
        }
        in_file.close();
    }

    // Add the task to the list if doesnt already exist
    if (!updated) {
        lines.push_back(name_ + ": " + std::to_string(metrics.min) + " " + std::to_string(metrics.avg) + " " + std::to_string(metrics.max));
    }

    // Write the updated contents back to the file
    std::ofstream out_file("metrics", std::ios::trunc);
    if (out_file.is_open()) {
        for (const std::string& updatedLine : lines) {
            out_file << updatedLine << "\n";
        }
        out_file.close();
    }
}