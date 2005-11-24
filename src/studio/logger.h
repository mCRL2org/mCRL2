#ifndef LOGGER_H
#define LOGGER_H

#include <list>

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/socket.h>

#define DEFAULT_LOG_PORT 6765

class Logger : public wxEvtHandler {
  private:
    wxSocketServer* listener;

    std::list < wxSocketBase* > connections;

  public:

    Logger();
    ~Logger();

    void OnConnect(wxSocketEvent& event);
    void OnDisconnect(wxSocketEvent& event);

    void OnInput(wxSocketEvent& event);
    void OnOutput(wxSocketEvent& event);
};

#endif
