#ifndef LOGGER_H
#define LOGGER_H

#include <iosfwd>
#include <list>

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/socket.h>
#include <wx/textctrl.h>

namespace squadt {

  #define DEFAULT_LOG_PORT 6765

  /*
   * Braindead... have to derive from wxTopLevelWindow because event handling does not
   * work properly otherwise
   */
  class Logger : public wxTopLevelWindow {
    DECLARE_CLASS(Logger)
    DECLARE_EVENT_TABLE()

    private:
      wxSocketServer* listener;

      std::list < wxSocketBase* > connections;

      /* Optional the text control on which to output the log */
      wxTextCtrl*   log_window;

      std::ostream& log_stream;

    public:

      Logger(std::ostream& stream = std::cerr);
      ~Logger();

      /* Set log window */
      inline void SetLogWindow(wxTextCtrl* window) {
        log_window = window;
      }

      /* Unset log window */
      inline void UnSetLogWindow() {
        log_window = NULL;
      }

      void OnListenEvent(wxSocketEvent& event);

      void OnSocketEvent(wxSocketEvent& event);
  };
}

#endif
