#include <wx/wx.h>
#include <wx/process.h>
#include <wx/textctrl.h>

#include "ui_core.h"

class ChildProcess : public wxProcess {
  public:

    /* Overwritten on terminate function */
    void OnTerminate(int, int) {
      /* Clean up */
      tool_executor.Remove(this);
    }

    ChildProcess(int flags) : wxProcess(flags) {
    }
 
    static inline ChildProcess* Open(const wxString& cmd, int flags = wxEXEC_ASYNC) {
      return (reinterpret_cast < ChildProcess* > (wxProcess::Open(cmd, flags)));
    }
};

