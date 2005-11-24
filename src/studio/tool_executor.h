#ifndef TOOL_EXECUTOR_H
#define TOOL_EXECUTOR_H

#include <ostream>
#include <string>
#include <map>

#include "ui_core.h"

class ToolManager;
class Process;

class ToolExecutor {
  private:
    std::map < Process*, long > processes; /* Maps a reference to a process id */

    size_t maximum_concurrent_processes;

  public:

    ToolExecutor(size_t mncp = 1);
    ~ToolExecutor();

    /* Execute a tool */
    bool Execute(ToolManager& tool_manager, unsigned int tool_identifier, std::string arguments, std::ostream&);

    /* Remove a process from the list */
    void Remove(Process* process_pointer);

    void Terminate(Process* process_pointer);

    void TerminateAll();
};

#endif
