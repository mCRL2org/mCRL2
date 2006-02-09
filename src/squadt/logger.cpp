#include <algorithm>

#include <wx/sckstrm.h>

#include "logger.h"

namespace squadt {

  #define SERVER_ID 3333
  #define SOCKET_ID 4444

  IMPLEMENT_CLASS(Logger, wxTopLevelWindow)

  BEGIN_EVENT_TABLE(Logger, wxTopLevelWindow)
  END_EVENT_TABLE()

  /*
   * Should make hostname as well as service customisable in the future.
   */
  Logger::Logger(std::ostream& stream) : wxTopLevelWindow(NULL, wxID_ANY, wxT("")), log_stream(stream) {
    wxIPV4address address = wxIPV4address();

    address.LocalHost();
    address.Service(DEFAULT_LOG_PORT);

    listener = new wxSocketServer(address, wxSOCKET_NOWAIT|wxSOCKET_REUSEADDR);

    if (!listener->Ok()) {
      std::cerr << "Error: socket creation on port " << address.Service() << " failed!\n";
    }

    Connect(SERVER_ID, wxEVT_SOCKET, wxSocketEventHandler(Logger::OnListenEvent));
    Connect(SOCKET_ID, wxEVT_SOCKET, wxSocketEventHandler(Logger::OnSocketEvent));

    listener->SetEventHandler(*this, SERVER_ID);
    listener->SetNotify(wxSOCKET_CONNECTION_FLAG|wxSOCKET_LOST_FLAG);
    listener->Notify(true);

    log_window = NULL;
  }

  void Logger::OnListenEvent(wxSocketEvent& event) {
    if (event.GetSocketEvent() == wxSOCKET_CONNECTION) {
      wxSocketBase* connection = listener->Accept(false);
   
      if (connection) {
        connection->SetEventHandler(*this, SOCKET_ID);
        connection->SetNotify(wxSOCKET_INPUT_FLAG|wxSOCKET_OUTPUT_FLAG);
        connection->Notify(true);
   
        connections.push_back(connection);
      }
      else {
        std::cerr << "Error: connection lost?!\n";
      }
    }
    else {
      event.GetSocket()->Destroy();

      connections.erase(std::find(connections.begin(), connections.end(), event.GetSocket()));
    }
  }

  void Logger::OnSocketEvent(wxSocketEvent& event) {
    if (event.GetSocketEvent() == wxSOCKET_INPUT) {
      /* Sub-optimal but the only alternative is polling... */
      char   buffer[256];

      buffer[255] = '\0';

      wxSocketBase& socket = *event.GetSocket();
   
      socket.Read(&buffer[0], 255);

      if (log_window != NULL) {
        while (!socket.Error() && socket.LastCount() == 255) {

          log_stream.write((char*) buffer, 255);
          log_window->WriteText(wxString(buffer, wxConvLocal));
          log_window->Update();

          socket.Read(&buffer[0], 255);
        }

        if (socket.LastCount() < 255) {
          buffer[socket.LastCount()] = '\0';
       
          log_stream.write((char*) buffer, socket.LastCount());
          log_window->WriteText(wxString(buffer, wxConvLocal));
          log_window->Update();
        }
      }
      else {
        while (!socket.Error() && socket.LastCount() == 255) {
          log_stream.write((char*) buffer, 255);

          socket.Read(&buffer, 255);
        }

        if (socket.LastCount() < 255) {
          log_stream.write((char*) buffer, socket.LastCount());

          log_stream << buffer[0];
        }
      }
    }
    else {
    }
  }

  Logger::~Logger() {
    const std::list < wxSocketBase* >::const_iterator b = connections.end();
          std::list < wxSocketBase* >::const_iterator i = connections.begin();

    while (i != b) {
      (*i)->Destroy();

      ++i;
    }

    listener->Destroy();
  }
}

