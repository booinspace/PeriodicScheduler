#include "Network.h"
#include "Common.h"

Network::Network(const std::string& name, uint32_t frequency) : Task(name, frequency)
{
    STATUS_LOG("Network::Network called");
}

void Network::execute()
{
    
}