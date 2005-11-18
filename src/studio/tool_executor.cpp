#include <algorithm>

#include <boost/filesystem/operations.hpp>

#include "tool_executor.h"
#include "tool_manager.h"
#include "child_process.h"

ToolExecutor::ToolExecutor(size_t mncp) {
  maximum_concurrent_processes = mncp;
}

ToolExecutor::~ToolExecutor() {
  std::map < ChildProcess*, long >::const_iterator b = processes.end();
  std::map < ChildProcess*, long >::const_iterator i = processes.begin();

  while (i != b) {
    ChildProcess::Kill((*i).second, wxSIGTERM);
  }
}

bool ToolExecutor::Execute(ToolManager& tool_manager, unsigned int tool_identifier, std::string arguments, std::ostream& stream) {
  boost::filesystem::path tool_path(tool_manager.GetTool(tool_identifier)->GetLocation());

  if (tool_path.string() != tool_path.leaf()) {
    wxString      command(tool_path.string().c_str(), wxConvLocal);
    ChildProcess* new_process = new ChildProcess(wxPROCESS_REDIRECT);
    long          process_id  = wxExecute(command.Append(wxT(" ")).Append(wxString(arguments.c_str(), wxConvLocal)), wxEXEC_SYNC, new_process);
 
    if (0 <= process_id) {
      processes[new_process] = process_id;
 
      return (true);
    }
    else {
      delete new_process;
    }
  }

  return (false);
}

void ToolExecutor::Remove(ChildProcess* process_pointer) {
  processes.erase(processes.find(process_pointer));
}

void ToolExecutor::Terminate(ChildProcess* process_pointer) {
  ChildProcess::Kill(processes[process_pointer], wxSIGTERM);

  /* Should wait perhaps for process to actually terminate */
  processes.erase(processes.find(process_pointer));
}

void ToolExecutor::TerminateAll() {
  std::map < ChildProcess*, long >::const_iterator b = processes.end();
  std::map < ChildProcess*, long >::const_iterator i = processes.begin();

  while (i != b) {
    ChildProcess::Kill((*i).second, wxSIGTERM);

    ++i;
  }
}

