#include "Process.h"
#include "Common.h"

Process::Process(const std::string &name, uint32_t frequency) : Task(name, frequency)
{
    STATUS_LOG("Process::Process called");
}

void Process::execute()
{

}