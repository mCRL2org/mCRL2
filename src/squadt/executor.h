#ifndef TOOL_EXECUTOR_H
#define TOOL_EXECUTOR_H

#include <algorithm>
#include <ostream>
#include <deque>
#include <functional>
#include <string>
#include <list>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "process.h"

namespace squadt {
  namespace execution {

    /** \brief Base class for process change listeners */
    class process_change_listener {
      friend class executor;
 
      private:
        /** Called when the state changes of the associated executor */
        virtual void report_change(process::status) = 0;
 
      public:
 
        /** Abstract destructor */
        virtual ~process_change_listener() = 0;
    };
 
    inline process_change_listener::~process_change_listener() {
    }
 
    /** \brief Basic process execution component */
    class executor {

      private:

        /** \brief Type for storing a command and a pointer to a state listener  */
        typedef std::pair < std::string, process_change_listener* > delayed_command;
 
      private:

        /** \brief The maximum number of processes that is allowed to run concurrently */
        unsigned int maximum_concurrent_processes;
 
        /** \brief List of active processes */
        std::list < process::ptr > processes;
 
        /** \brief Data of processes that will be started */
        std::deque < delayed_command > delayed_commands;

      private:
    
        /** \brief Actually start a new process (run a command) */
        inline void start_process(const std::string&, process_change_listener* s);
    
        /** \brief Start processing commands if the queue contains any waiters */
        inline void start_delayed();
 
        /** \brief handler that is invoked when a process is terminated */
        inline void handle_process_termination(process_change_listener* s, process* p);
  
      public:
    
        /** \brief Constructor */
        inline executor(unsigned int = 1);
 
        /** \brief Destructor */
        inline ~executor();
    
        /** \brief Execute a tool */
        inline void execute(const std::string&, process_change_listener*);
    
        /** \brief Remove a process from the list */
        inline void remove(process*);
    
        /** \brief Terminate a specific process */
        inline void terminate(process*);
    
        /** \brief Terminate all processes */
        inline void terminate();
    };
 
    inline executor::executor(unsigned int m) : maximum_concurrent_processes(m) {
    }
    
    inline executor::~executor() {
      terminate();
    }
 
    inline void executor::remove(process* p) {
      processes.erase(std::find_if(processes.begin(), processes.end(),
                              boost::bind(std::equal_to < process * >(), p,
                                      boost::bind(&process::ptr::get, _1))));
    }
 
    inline void executor::start_process(const std::string& command, process_change_listener* s) {
      process::ptr p = process::execute(boost::bind(&executor::handle_process_termination, this, s, _1), command);
    
      if (p.get() != 0) {
        processes.push_back(p);
 
        if (s != 0) {
          s->report_change(process::running);
        }
      }
    }
 
    /**
     * The queue with commands for to be started processes is also cleared by this function.
     **/
    inline void executor::terminate() {
    
      delayed_commands.clear();
    
      std::for_each(processes.begin(), processes.end(), boost::bind(&process::kill, _1));
    }
 
    /* Start processing commands if the queue contains any waiters */
    inline void executor::start_delayed() {
      if (0 < delayed_commands.size()) {
        std::pair < std::string, process_change_listener* > p = delayed_commands.front();
 
        delayed_commands.pop_front();
 
        start_process(p.first, p.second);
      }
    }
    
    inline void executor::terminate(process* p) {
      remove(p);
    }
 
    void executor::handle_process_termination(process_change_listener* l, process* p) {
      remove(p);
 
      // start process for delayed command
      start_delayed();
 
      if (l != 0) {
        l->report_change(p->get_status());
      }
    }
 
    /**
     * @param c the command that is to be executed
     * @param l a pointer to a listener for process state changes
     **/
    void executor::execute(const std::string& c, process_change_listener* l) {
      if (processes.size() < maximum_concurrent_processes) {
        start_process(c, l);
      }
      else {
        /* queue command for later execution */
        delayed_commands.push_back(std::pair < std::string, process_change_listener* >(c, l));
      }
    }
  }
}
  
#endif
