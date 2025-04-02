#include "Network.h"
#include "Common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory>
#include <cstring>

//! @brief Create a new instance of a Network
std::unique_ptr<Network> NetworkBuilder::Create(const std::string& domain, const std::chrono::milliseconds quantum)
{
    return std::make_unique<Network>(domain, quantum);
}

//! @brief Constructor for the Network class
Network::Network(const std::string& domain, const std::chrono::milliseconds quantum) : Task(quantum, domain),
domain_(domain),
complete_(false)
{
    host_ = new struct gaicb;
    std::memset(host_, 0, sizeof(struct gaicb));
}

//! @brief Destructor for the Network class
Network::~Network(void)
{
    delete host_;
}

//! @brief Execute this class behaviour
bool Network::execute(std::atomic<bool>& should_stop)
{
    // Check if the task should return execution
    if (should_stop.load()) {
        return false;
    }

    start_.push_back(std::chrono::steady_clock::now());

    char ip[INET_ADDRSTRLEN];
    if (ResolveDomainAsync(domain_, ip, sizeof(ip)) == 0)
    {
        INFO_LOG("Resolved %s %s attempts: %ld\n",  name_.c_str(), ip, start_.size());
        end_.push_back(std::chrono::steady_clock::now());

        // Work out the final task times
        CalculateMetrics();

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
    std::memset(host_, 0, sizeof(struct gaicb));
    complete_ = false;
}

//! @brief Resolve a domain name to an IPV4 address
int Network::ResolveDomainAsync(const std::string& fqdn, char* ip, size_t ip_size)
{
    // Check if the resolve has finished
    int ret = gai_error(host_);
    if (!ret && complete_)
    {
        // Request has resolved
        ret = getnameinfo(host_->ar_result->ai_addr, host_->ar_result->ai_addrlen, ip, ip_size, nullptr, 0, NI_NUMERICHOST);
        return 0;
    }
    else if (ret == EAI_INPROGRESS)
    {
        STATUS_LOG("Still in progress\n");
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
        STATUS_ERR("Error resolving %s\n", gai_strerror(ret));
    }
    else
    {
        // Success
        ret = getnameinfo(host_->ar_result->ai_addr, host_->ar_result->ai_addrlen, ip, ip_size, nullptr, 0, NI_NUMERICHOST);
        complete_ = true;
        return 0;
    };

    return -1;
}