#include "Process.h"
#include "Common.h"

Process::Process(const std::chrono::milliseconds quantum) : Task(quantum)
{
}

bool Process::execute()
{
    //STATUS_LOG("Process::Execute\n");

    return true;
}

