#include <iostream>

#include "process.h"
#include "tool_executor.h"

namespace squadt {
  
  Process* Process::Execute(ToolExecutor& t, const std::string& acommand, Specification* tt) throw (ExecutionException*) {
    wxString  command(acommand.c_str(), wxConvLocal);
    Process*  new_process = new Process(t, tt, wxPROCESS_REDIRECT);
  
    new_process->process_id = wxExecute(command, wxEXEC_ASYNC, new_process);
  
    if (new_process->process_id < 0) {
      throw (new ExecutionError());
    }
  
    return (new_process);
  }
  
  void Process::OnTerminate(int pid, int status) {
    /* Clean up */
    tool_executor.Signal(this, Completed);
    tool_executor.Remove(this);
  
    wxIPV4address address = wxIPV4address();
  
    address.LocalHost();
    address.Service(DEFAULT_PORT);
  
    wxSocketClient* socket = new wxSocketClient(wxSOCKET_NOWAIT);
  
    if (socket->Connect(address, true)) {
      /* Sub-optimal but the only alternative is polling... */
      wxInputStream*       error_stream = GetErrorStream();
      wxSocketOutputStream socket_stream(*socket);
  
      socket_stream.Write(*error_stream);
    }
    else {
      std::cerr << "Error: could not connect to logger!\n";
    }
  
    socket->Destroy();
  
    delete this;
  }
}

