#ifndef NETWORK_h
#define NETWORK_H

#include <string>
#include <deque>
#include <optional>
#include <vector>

#include "Task.h"

class Network : public Task
{
public:
    //! @brief Constructor for the Network class
    //! @param[in] domain Domain name to resolve e.g. "google.com"
    //! @param[in] quantum The maximum time this task should execute for
    Network(const std::string& domain, const std::chrono::milliseconds quantum);

    //! @brief Destructor for the Network class
    ~Network(void);

    //! @brief Execute this class behaviour
    bool execute() override;

    //! @brief Resolve a domain name to an IPV4 address
    //! NOTE: Async function, waits for domain to resolve, otherwise returns
    int ResolveDomainAsync(const std::string& domain, char* ip, size_t ip_size);
private:
    //! @brief Convert a std::chrono::milliseconds to a tvspec (seconds and nanoseconds)
    //! @return The timespec object
    const timespec CalculateTimespec(std::chrono::milliseconds duration);

    //! @brief Mark this task as complete so it can be re-executed
    void CompleteTask(void);

    //! @brief Calculate the resolve time for all executions of this task until completion
    void CalculateResolveTime(void);

    //! Capture the start times for every execution of this task
    std::vector<std::chrono::steady_clock::time_point> start_;

    //! Capture the end times for every execution of this task
    std::vector<std::chrono::steady_clock::time_point> end_;

    //! Maintain a queue of resolve times for each execution
    std::deque<int32_t> resolve_times_;

    //! The domain to resolve
    std::string domain_;

    //! gaicb structure used for async networking operations
    struct gaicb* host_;

    bool complete_;
};

class NetworkBuilder
{
public:
    NetworkBuilder() = default;
    ~NetworkBuilder() = default;

    // Delete copy constructor
    NetworkBuilder(const NetworkBuilder& other) = delete;
    // Delete copy assignment constructor
    NetworkBuilder& operator=(const NetworkBuilder& other) = delete;
    // Delete move constructor
    NetworkBuilder(NetworkBuilder&& other) noexcept = delete;
    // Delete move assignment constructor
    NetworkBuilder& operator=(NetworkBuilder&& other) noexcept = delete;

    //! @brief Create a new instance of a Network class
    //! @return A unique pointer to a newly created network class
    std::unique_ptr<Network> Create(const std::string& domain, const std::chrono::milliseconds quantum);
};

#endif