### Overview
A C++ implementation of a periodic task scheduler that monitors both network and sytem metrics at configurable intervals. The scheduler implements round-robin approach to ensure fairness of tasks and tracks statistical data on task performance.

### How to run
`./build.sh <Debug>` to include all logging
`./build.sh` to create a release binary

The binary will be placed in `build/periodic_scheduler`

### Testing
Run the unit tests with `./run_tests.sh`

### Requirements
* Linux environment
* GCC/G++ compiler
* CMake (3.10+)
* Internet connection (for GTest dependency during build)
This application is specific for Linux environments and will not work as intended on other OS's.
Only C++ std library functions and POSIX libraries are used.

Unit tests make use of gtest, please ensure an internet connection is available so this can be downloaded

### Features
#### Tasks
* Memory usage of current process
* DNS resolution
#### Features
* Metrics logging (min, avg, max) times a task took (with metrics stored on FS for stateful restarting)
* Changing the interval of task probes, use --network_interval and --system_interval

### Scheduler design
The scheduler uses a 2 threads. One main thread the scheduler runs on which executes tasks when they have reached their interval period. Another worker thread created by std::async for each task. If this worker thread exceeds more than the quantum time period then execution is returned to the main thread where the new task is worked on. This implements on RR (Round-Robin) to ensure fairness for all tasks in the task list.

#### Justification
* The decision to use only std libraries and not boost for example was made to try and reduce the number of dependencies for this project. However, I do appreciate that boost could possibly simplify some of the IO bound networking operations and allow more code reuse if more probes were implemented like packets sent on a specific networking interface.
* Round-robin was used to simplify scheduler design with-in the task time-limits and focus on robustness. This could be improved with MLQF since all IO bound tasks like networking operations will take a lot longer than system tasks which can complete in a fraction of the same time.

### Future Improvements
* Implement additional probes
* Consider a Multi-Level Queue Feedback (MLQF) scheduler for improved performance
* Further unit test coverage of other probes and scheduling design
