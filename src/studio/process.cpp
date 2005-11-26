#include <iostream>

#include "process.h"
#include "tool_executor.h"

extern ToolExecutor tool_executor;

void Process::OnTerminate(int pid, int status) {
  /* Clean up */
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

