#include "Network.h"
#include "Common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory>
#include <cstring>

//! Maintain only the last 10 resolves for metrics
static constexpr uint32_t kMaxResolveTimes = 10;

Network::Network(const std::string& domain) : Task(),
domain_(domain)
{
    host_ = new struct gaicb;
}

Network::~Network(void)
{
    delete host_;
}

//! @brief Execute this class behaviour
void Network::execute()
{
    if (!start_)
    {
        start_ = std::chrono::steady_clock::now();
    }

    char ip[INET_ADDRSTRLEN];
    if (ResolveDomainAsync(domain_, ip, sizeof(ip)) == 0)
    {
        if (!end_)
        {
            end_ = std::chrono::steady_clock::now();
        }
        int32_t resolve_time = std::chrono::duration_cast<std::chrono::milliseconds>(*end_ - *start_).count();

        // If the domain resolved, reset the start and end times so that the task will run again
        end_ = std::nullopt;
        start_ = std::nullopt;
        host_ = nullptr;

        // Remove entries from resolve times to maintain the fixed last 10 executions
        if (resolve_times_.size() > kMaxResolveTimes)
        {
            resolve_times_.pop_front();
        }

        resolve_times_.push_back(resolve_time);

        // Maintain a lock on updating shared metrics across this task across threads
        std::lock_guard<std::mutex> lock(mtx_);

        metrics_.min = std::min(resolve_time, metrics_.min);
        metrics_.max = std::max(resolve_time, metrics_.max);
        // Use the mean for average
        int32_t total_time = 0;
        for (int32_t time : resolve_times_)
        {
            total_time += time;
        }
        metrics_.avg = total_time / resolve_times_.size();
    }
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
    // It could have previously already started resolve, but we need to check to see if the value is ready
    else if (ret == EAI_INPROGRESS)
    {
        return -1;
    }
    // Otherwise lets setup a new resolve

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    host_->ar_name = fqdn.c_str();
    host_->ar_request = &hints;

    int status = getaddrinfo_a(GAI_NOWAIT, &host_, 1, nullptr);
    if (status)
    {
        STATUS_ERR("Failed to setup DNS resolve: %s\n", gai_strerror(status));
        return -1;
    }

    // 100ms timeout
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 100000000;
    ret = gai_suspend(&host_, 1, &ts);
    if (ret != 0) {
        STATUS_ERR("Failed to wait for resolve: %s\n", gai_strerror(ret));
        return -1;
    }
    if (!gai_error(host_))
    {
        // Success
        ret = getnameinfo(host_->ar_result->ai_addr, host_->ar_result->ai_addrlen, ip, ip_size, nullptr, 0, NI_NUMERICHOST);
        return 0;
    };

    return -1;
}