#ifndef TOOL_EXECUTOR_H
#define TOOL_EXECUTOR_H

#include <ostream>
#include <deque>
#include <string>
#include <vector>

#include "process_status.h"

class Process;
class Specification;

typedef struct {
  std::string    command;
  Specification* target;
} ProcessData;

class ToolExecutor {
  private:
    std::vector < Process* > processes; /* Maps a reference to a process id */

    size_t maximum_concurrent_processes;
    size_t running_processes;

    std::deque < ProcessData > queue;   /* Commands that have to be run */

    /* Actually start a new process (run a command) */
    void StartProcess(const std::string&, Specification*) throw (ExecutionException*);

    /* Start processing commands if the queue contains any waiters */
    void CheckQueue();

  public:

    ToolExecutor(size_t mncp = 1);
    ~ToolExecutor();

    /* Execute a tool */
    void Execute(std::string, Specification*) throw (ExecutionException*);

    /* Remove a process from the list */
    void Remove(Process* process_pointer);

    /* Signal a change in status for a process */
    void Signal(Process* process_pointer, ProcessStatus status);

    void Terminate(Process* process_pointer);

    void TerminateAll();
};

#endif
