#ifndef NETWORK_h
#define NETWORK_H

#include <string>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "Task.h"

class Network : public Task
{
public:
    Network(uint32_t frequency);
    void execute() override;
    uint32_t ResolveDomain(const std::string& fqdn);
};

#endif