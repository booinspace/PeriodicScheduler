#include "Network.h"
#include "Common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory>
#include <cstring>

//! Maintain only the last 10 resolves for metrics
static constexpr uint32_t kMaxResolveTimes = 5;

//! @brief Create a new instance of a Network class
std::unique_ptr<Network> NetworkBuilder::Create(const std::string& domain, const std::chrono::milliseconds quantum)
{
    return std::make_unique<Network>(domain, quantum);
}

Network::Network(const std::string& domain, const std::chrono::milliseconds quantum) : Task(quantum),
domain_(domain)
{
    host_ = new struct gaicb;
}

Network::~Network(void)
{
    delete host_->ar_request;
    delete host_;
}

//! @brief Execute this class behaviour
bool Network::execute()
{
    start_.push_back(std::chrono::steady_clock::now());

    char ip[INET_ADDRSTRLEN];
    if (ResolveDomainAsync(domain_, ip, sizeof(ip)) == 0)
    {
        STATUS_LOG("Resolved %s attempts: %ld\n", ip, start_.size());
        end_.push_back(std::chrono::steady_clock::now());

        // Work out the final resolve times
        CalculateResolveTime();

        // If the domain resolved, reset the start and end times so that the task will run again
        CompleteTask();

        return true;
    }

    end_.push_back(std::chrono::steady_clock::now());
    return false;
}

//! @brief Calculate the seconds and nanoseconds from a chrono duration
const timespec Network::CalculateTimespec(std::chrono::milliseconds duration)
{
    struct timespec ts;
    ts.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    ts.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(duration % std::chrono::seconds(1)).count();
    return ts;
}

//! @brief Mark this task as complete so it can be re-executed
void Network::CompleteTask(void)
{
    start_.clear();
    end_.clear();
    host_->ar_name = 0;
}

//! @brief Calculate the resolve time for all executions of this task until completion
void Network::CalculateResolveTime(void)
{
    // Work out the resolved time by adding the difference from all executions to resolve
    int32_t resolve_time = 0;
    for (uint32_t i = 0; i < start_.size(); i++)
    {
        resolve_time += std::chrono::duration_cast<std::chrono::milliseconds>(end_[i] - start_[i]).count();
    }

    // Maintain a lock on updating shared metrics across this task across threads
    std::lock_guard<std::mutex> lock(metrics_mutex_);

    metrics_.min = std::min(resolve_time, metrics_.min);
    metrics_.max = std::max(resolve_time, metrics_.max);

    // Remove entries from resolve times to maintain the fixed last 10 executions
    if (resolve_times_.size() > kMaxResolveTimes)
    {
        resolve_times_.pop_front();
    }
    resolve_times_.push_back(resolve_time);

    // Calculate the mean for average
    int32_t total_time = 0;
    for (int32_t time : resolve_times_)
    {
        total_time += time;
    }
    metrics_.avg = total_time / resolve_times_.size();
}

int Network::ResolveDomainAsync(const std::string& fqdn, char* ip, size_t ip_size)
{
    // Check if the resolve has finished
    int ret = gai_error(host_);
    if (!ret && host_->ar_name != 0)
    {
        // Request has resolved
        ret = getnameinfo(host_->ar_result->ai_addr, host_->ar_result->ai_addrlen, ip, ip_size, nullptr, 0, NI_NUMERICHOST);
        return 0;
    }
    else if (ret == EAI_INPROGRESS)
    {
        STATUS_ERR("Still in progress\n");
        return -1;
    }

    // Otherwise lets setup a new resolve
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    host_->ar_name = fqdn.c_str();
    host_->ar_request = &hints;

    ret = getaddrinfo_a(GAI_NOWAIT, &host_, 1, nullptr);
    if (ret)
    {
        STATUS_ERR("Failed to setup DNS resolve: %s\n", gai_strerror(ret));
        return -1;
    }

    struct timespec ts = CalculateTimespec(std::chrono::milliseconds(quantum_));
    ret = gai_suspend(&host_, 1, &ts);
    if (ret != 0) {
        STATUS_ERR("Timed out resolving: %s\n", fqdn.c_str());
        return -1;
    }

    // Check the resolve status
    if (gai_error(host_))
    {
        STATUS_LOG("Error resolving %s\n", gai_strerror(ret));
    }
    else
    {
        // Success
        ret = getnameinfo(host_->ar_result->ai_addr, host_->ar_result->ai_addrlen, ip, ip_size, nullptr, 0, NI_NUMERICHOST);
        return 0;
    };

    return -1;
}