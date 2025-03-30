#include "Network.h"
#include "Common.h"

Network::Network(uint32_t frequency) : Task(frequency)
{
    STATUS_LOG("Network::Network called");
}

void Network::execute()
{

    STATUS_LOG("Network::Execute");
}