#include <ctime>
#include <iostream>
#include <iomanip>
#include <functional>
#include <thread>
#include <chrono>

#include <boost/asio.hpp>

void log_text(std::string const& text)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << " " << text << std::endl;
}

class PeriodicTask
{
public:
    typedef std::function<void()> handler_fn;

    PeriodicTask(boost::asio::io_service& ioService
        , std::string const& name
        , int interval
        , handler_fn task)
        : ioService(ioService)
        , interval(interval)
        , task(task)
        , name(name)
        , timer(ioService)
    {
        log_text("Create PeriodicTask '" + name + "'");
        // Schedule start to be ran by the io_service
        ioService.post([this]() { this->start(); });
    }

    void execute(boost::system::error_code const& e)
    {
        if (e != boost::asio::error::operation_aborted) {
            log_text("Execute PeriodicTask '" + name + "'");

            task();

            timer.expires_at(timer.expires_at() + boost::posix_time::seconds(interval));
            start_wait();
        }
    }

    void start()
    {
        log_text("Start PeriodicTask '" + name + "'");

        // Uncomment if you want to call the handler on startup (i.e. at time 0)
        task();

        timer.expires_from_now(boost::posix_time::seconds(interval));
        start_wait();
    }

private:
    void start_wait()
    {
        timer.async_wait([&](const boost::system::error_code& error) {
            this->execute(error);
        });
    }

private:
    boost::asio::io_service& ioService;
    boost::asio::deadline_timer timer;
    handler_fn task;
    std::string name;
    int interval;
};

class PeriodicScheduler
{
public:
    void run()
    {
        io_service.run();
    }

    void addTask(std::string const& name,
        PeriodicTask::handler_fn const& task,
        int interval)
    {
        tasks.push_back(std::make_unique<PeriodicTask>(io_service, name, interval, task));
    }

private:
    boost::asio::io_service io_service;
    std::vector<std::unique_ptr<PeriodicTask>> tasks;
};

int main()
{
    PeriodicScheduler scheduler;
    scheduler.addTask("CPU", []() { log_text("* CPU USAGE"); }, 5);
    scheduler.addTask("Memory", []() { log_text("* MEMORY USAGE"); }, 10);

    log_text("Start io_service");

    scheduler.run();

    return 0;
}