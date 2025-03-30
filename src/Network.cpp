#include "Network.h"
#include "Common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

//! Maintain only the last 10 resolves for metrics
static constexpr uint32_t kMaxResolveTimes = 10;

Network::Network(const std::string& domain) : Task(),
domain_(domain)
{
}

void Network::execute()
{
    int32_t resolve_time = ResolveDomain(domain_);

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

int32_t Network::ResolveDomain(const std::string& fqdn)
{
    auto start = std::chrono::steady_clock::now();

    struct addrinfo* res = {};
    int status = getaddrinfo(fqdn.c_str(), nullptr, nullptr, &res);
    if (status != 0)
    {
        STATUS_ERR("Failed to resolve domain: %s\n", fqdn.c_str());
        return -1;
    }
    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in* sock = (struct sockaddr_in*)res->ai_addr;
    inet_ntop(AF_INET, &sock->sin_addr, ip, sizeof(ip));

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //STATUS_LOG("Resolved domain: %s in %ldms\n", ip, duration);

    return duration;
}