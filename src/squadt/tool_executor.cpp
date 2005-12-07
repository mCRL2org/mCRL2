#include <algorithm>

#include <boost/filesystem/operations.hpp>

#include "tool_executor.h"
#include "tool_manager.h"
#include "process.h"

ToolExecutor::ToolExecutor(size_t mncp) {
  maximum_concurrent_processes = mncp;
}

ToolExecutor::~ToolExecutor() {
  std::map < Process*, long >::const_iterator b = processes.end();
  std::map < Process*, long >::const_iterator i = processes.begin();

  while (i != b) {
    Process::Kill((*i).second, wxSIGTERM);
  }
}

long ToolExecutor::Execute(ToolManager& tool_manager, unsigned int tool_identifier, std::string arguments, std::ostream& stream) throw (ExecutionError) {
  boost::filesystem::path tool_path(tool_manager.GetTool(tool_identifier)->GetLocation());

  wxString  command(tool_path.string().c_str(), wxConvLocal);
  Process*  new_process = new Process(wxPROCESS_REDIRECT);

  long process_id = wxExecute(command.Append(wxT(" ")).Append(wxString(arguments.c_str(), wxConvLocal)), wxEXEC_ASYNC, new_process);
 
  if (0 <= process_id) {
    processes[new_process] = process_id;
 
    throw (ExecutionError());
  }
  else {
    delete new_process;
  }

  return (process_id);
}

void ToolExecutor::Remove(Process* process_pointer) {
  processes.erase(processes.find(process_pointer));
}

void ToolExecutor::Terminate(Process* process_pointer) {
  Process::Kill(processes[process_pointer], wxSIGTERM);

  /* Should wait perhaps for process to actually terminate */
  processes.erase(processes.find(process_pointer));
}

void ToolExecutor::TerminateAll() {
  std::map < Process*, long >::const_iterator b = processes.end();
  std::map < Process*, long >::const_iterator i = processes.begin();

  while (i != b) {
    Process::Kill((*i).second, wxSIGTERM);

    ++i;
  }
}

