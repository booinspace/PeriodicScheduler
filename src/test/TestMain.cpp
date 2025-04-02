
#include <gtest/gtest.h>

#include "Task.h"
#include "Network.h"
#include "Common.h"

class NetworkTest : public Network, public ::testing::Test
{
public:
    NetworkTest() : Network("google.com", std::chrono::seconds(1))
    {}

    bool execute(std::atomic<bool>& should_stop) override
    {
        STATUS_LOG("Executing test network\n");
        return true;
    }
};

// Ensure that the scheduler
TEST_F(NetworkTest, ExecuteTest)
{
    std::atomic<bool> should_stop(false);
    bool complete = execute(should_stop);

    EXPECT_TRUE(complete);
}