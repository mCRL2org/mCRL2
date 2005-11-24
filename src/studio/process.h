#ifndef PROCESS_H
#define PROCESS_H

#include <wx/wx.h>
#include <wx/process.h>
#include <wx/textctrl.h>
#include <wx/socket.h>
#include <wx/sckstrm.h>

#include "ui_core.h"

#define DEFAULT_PORT 6765

class Process : public wxProcess {
  public:

    Process(int flags = wxPROCESS_DEFAULT) : wxProcess(flags) {
      Redirect();
    }
 
    static inline Process* Open(const wxString& cmd, int flags = wxEXEC_ASYNC) {
      return (reinterpret_cast < Process* > (wxProcess::Open(cmd, flags)));
    }

    /* Overwritten on terminate function */
    void OnTerminate(int pid, int status);
};

#endif
