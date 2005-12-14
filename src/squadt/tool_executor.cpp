#include <algorithm>

#include "tool_executor.h"
#include "specification.h"
#include "process.h"

ToolExecutor::ToolExecutor(size_t mncp) {
  maximum_concurrent_processes = mncp;
  running_processes            = 0;
}

ToolExecutor::~ToolExecutor() {
  std::vector < Process* >::const_iterator b = processes.end();
  std::vector < Process* >::const_iterator i = processes.begin();

  while (i != b) {
    Process::Kill((*i)->GetId(), wxSIGTERM);
  }
}

void ToolExecutor::StartProcess(const std::string& command, Specification* target) throw (ExecutionException*) {
  Process* new_process = Process::Execute(*this, command, target);

  if (new_process != NULL) {
    processes.push_back(new_process);

    ++running_processes;
  }
}

/* Start processing commands if the queue contains any waiters */
void ToolExecutor::CheckQueue() {
  if (0 < queue.size()) {
    try {
      StartProcess(queue.front().command, queue.front().target);
    }
    catch (ExecutionException*) {
      CheckQueue();
    }

    queue.pop_front();
  }
  else {
    --running_processes;
  }
}

void ToolExecutor::Execute(std::string command, Specification* target) throw (ExecutionException*) {
  if (running_processes < maximum_concurrent_processes) {
    StartProcess(command, target);
  }
  else {
    ProcessData data = {command, target};

    /* queue command for later execution */
    queue.push_back(data);

    throw (new ExecutionDelayed());
  }
}

void ToolExecutor::Remove(Process* process_pointer) {
  std::vector < Process*>::const_iterator b = processes.end();
  std::vector < Process*>::const_iterator i = processes.begin();

  processes.erase(std::find(processes.begin(), processes.end(), process_pointer));
}

void ToolExecutor::Terminate(Process* process_pointer) {
  Process::Kill(process_pointer->GetId(), wxSIGTERM);

  /* Should wait perhaps for process to actually terminate */
  processes.erase(std::find(processes.begin(), processes.end(), process_pointer));
}

void ToolExecutor::TerminateAll() {
  std::vector < Process*>::const_iterator b = processes.end();
  std::vector < Process*>::const_iterator i = processes.begin();

  while (i != b) {
    Process::Kill((*i)->GetId(), wxSIGTERM);

    ++i;
  }

  processes.clear();
}

/* Signal a change is status for a process */
void ToolExecutor::Signal(Process* process_pointer, ProcessStatus status) {
  switch (status) {
    case Completed:
      /* Have a tool_manager update the status */
//      process_pointer->GetTarget()->SetStatus(up_to_date);

      CheckQueue();

      break;
    case Aborted:
//      process_pointer->GetTarget()->SetStatus(not_up_to_date);

      CheckQueue();

      break;
  }
}

