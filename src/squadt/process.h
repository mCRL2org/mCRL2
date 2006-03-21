#ifndef PROCESS_H
#define PROCESS_H

#include <cassert>

#include <boost/function.hpp>
#include <boost/ref.hpp>

#include <wx/wx.h>
#include <wx/process.h>
#include <wx/textctrl.h>
#include <wx/socket.h>
#include <wx/sckstrm.h>

#include "exception.h"

namespace squadt {
  namespace execution {

    class process;
    
    /**
     * \brief Represents a system process with a status reporting facility
     **/
    class process : public wxProcess {
      public:
         /** \brief state of the process */
         enum status {
           stopped,   ///< stopped or not yet started to run
           running,   ///< is running
           completed, ///< has terminated successfully 
           aborted    ///< was aborted
         };
 
         /** \brief Convenience type to hide shared pointer implementation */
         typedef boost::shared_ptr < process >        ptr;
 
         /** \brief Convenience type for handlers */
         typedef boost::function < void (process*) >  handler;
 
      private:
    
        /** \brief The system's proces identifier for this process */
        long int    identifier;
    
        /** \brief The status of this process */
        status      current_status;
 
        /** \brief The function that is called when the status changes */
        handler     call_back;
 
        /** \brief Overridden on terminate function */
        inline void OnTerminate(int pid, int status);
    
      public:
    
        /** \brief Constructor */
        inline process(handler, int flags = wxPROCESS_DEFAULT);
    
        /** \brief Start the process by executing a command */
        static ptr execute(handler, const std::string&) throw ();
     
        /** \brief Returns the process status */
        inline status get_status() const;
 
        /** \brief Terminates the process */
        inline void kill();
 
        /** \brief Destructor */
        inline ~process();
    };
 
    /**
     * @param h the function to call when the process terminates
     * @param f the flags for process behaviour (see wxWidgets documentation)
     **/
    inline process::process(handler h, int f) : wxProcess(f), current_status(stopped), call_back(h) {
    }
 
    inline process::~process() {
      Kill(identifier, wxSIGTERM);
    }
 
    inline process::status process::get_status() const {
      return (current_status);
    }
 
    inline void process::kill() {
      Kill(identifier, wxSIGTERM);
    }
 
    /**
     * @param c the command to execute
     * @param h the function to call when the process terminates
     **/
    inline process::ptr process::execute(handler h, const std::string& c) throw () {
      ptr p(new process(h, wxPROCESS_REDIRECT));
    
      p->identifier = wxExecute(boost::cref(wxString(c.c_str(), wxConvLocal)), wxEXEC_ASYNC, p.get());
    
      if (p->identifier < 0) {
        p->current_status = aborted;
 
        throw (new exception(exception_identifier::program_execution_failed, c));
      }
      else {
        p->current_status = running;
      }
    
      return (p);
    }
    
    inline void process::OnTerminate(int /* pid */, int /* status */) {
      current_status = completed;
 
      /* Clean up */
      call_back(this);
    }
  }
}
  
#endif
