#include "Process.h"
#include "Common.h"

Process::Process(uint32_t frequency) : Task(frequency)
{
    STATUS_LOG("Process::Process called");
}

void Process::execute()
{
    STATUS_LOG("Process::Execute");
}