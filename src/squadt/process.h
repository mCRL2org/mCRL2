#ifndef PROCESS_H
#define PROCESS_H

#include <wx/wx.h>
#include <wx/process.h>
#include <wx/textctrl.h>
#include <wx/socket.h>
#include <wx/sckstrm.h>
#include <cassert>

#include "process_status.h"

namespace squadt {

  #define DEFAULT_PORT 6765

  class Specification;
  class ToolExecutor;
  class Process;
  
  class Process : public wxProcess {
    private:
  
      ToolExecutor&  tool_executor;
      long           process_id;
      Specification* target;
  
    public:
  
      /* The plan identifies the purpose of the process */
      Process(ToolExecutor& t, Specification* p, int flags = wxPROCESS_DEFAULT) : wxProcess(flags), tool_executor(t), target(p) {
      }
  
      static Process* Execute(ToolExecutor& t, const std::string&, Specification*) throw (ExecutionException*);
   
      /* Overwritten on terminate function */
      void OnTerminate(int pid, int status);
  
      inline long GetId() {
        assert(0 <= process_id);
  
        return (process_id);
      }
  
      inline Specification* GetTarget() {
        return (target);
      }
  };
}
  
#endif
