#include "Process.h"
#include "Common.h"

Process::Process() : Task()
{
    STATUS_LOG("Process::Process called");
}

void Process::execute()
{
    STATUS_LOG("Process::Execute");
}