#include <iostream>
#include <algorithm>

#include "logger.h"

/*
 * Should make hostname as well as service customisable in the future.
 */
Logger::Logger() {
  wxIPV4address address = wxIPV4address();

  address.LocalHost();
  address.Service(DEFAULT_LOG_PORT);

  listener = new wxSocketServer(address, wxSOCKET_NOWAIT|wxSOCKET_REUSEADDR);

  if (!listener->Ok()) {
    std::cerr << "Error: socket creation on port " << address.Service() << " failed!\n";
  }

  listener->SetNotify(wxSOCKET_CONNECTION_FLAG|wxSOCKET_LOST_FLAG|wxSOCKET_INPUT_FLAG|wxSOCKET_OUTPUT_FLAG);

  Connect(wxSOCKET_CONNECTION, wxEVT_SOCKET, wxSocketEventHandler(Logger::OnConnect));
  Connect(wxSOCKET_LOST, wxEVT_SOCKET, wxSocketEventHandler(Logger::OnDisconnect));
  Connect(wxSOCKET_INPUT, wxEVT_SOCKET, wxSocketEventHandler(Logger::OnInput));
  Connect(wxSOCKET_OUTPUT, wxEVT_SOCKET, wxSocketEventHandler(Logger::OnOutput));
}

void Logger::OnConnect(wxSocketEvent& event) {
  connections.push_back(listener->Accept(false));
}

void Logger::OnDisconnect(wxSocketEvent& event) {
  connections.erase(std::find(connections.begin(), connections.end(), event.GetSocket()));
}

void Logger::OnInput(wxSocketEvent& event) {
  std::cerr << "Hello\n";
}

void Logger::OnOutput(wxSocketEvent& event) {
}

Logger::~Logger() {
  listener->Destroy();
}

