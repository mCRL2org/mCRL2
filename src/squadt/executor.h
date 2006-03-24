#ifndef TOOL_EXECUTOR_H
#define TOOL_EXECUTOR_H

#include <algorithm>
#include <ostream>
#include <deque>
#include <functional>
#include <string>
#include <list>

#include <boost/bind.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "process_listener.h"

namespace squadt {
  namespace execution {

    /**
     * \brief Basic process execution component
     *
     * Design choices:
     *  - only a predetermined fixed number of processes can is run concurrently,
     *    which means that sometimes execution is delayed
     *  - the execute method is non-blocking; consequently nothing can be
     *    concluded about the execution of the command except through a
     *    process_listener object
     **/
    class executor {

      private:

        /** \brief Type for storing a command and a pointer to a state listener  */
        typedef std::pair < const std::string, process_listener* > delayed_command;
 
      private:

        /** \brief The maximum number of processes that is allowed to run concurrently */
        unsigned int maximum_concurrent_processes;
 
        /** \brief List of active processes */
        std::list < process::ptr >     processes;
 
        /** \brief Data of processes that will be started */
        std::deque < delayed_command > delayed_commands;

      private:
    
        /** \brief Start a new process */
        inline void start_process(const std::string&);
    
        /** \brief Start a new process with a listener */
        inline void start_process(const std::string&, process_listener&);

        /** \brief Start a new process with a listener if s is not 0 */
        inline void start_process(const std::string&, process_listener* s);
    
        /** \brief handler that is invoked when a process is terminated */
        inline void handle_process_termination(process* p);
  
        /** \brief handler that is invoked when a process is terminated */
        inline void handle_process_termination(process* p, process_listener* s);
  
        /** \brief Start processing commands if the queue contains any waiters */
        inline void start_delayed();
 
        /** \brief Remove a process from the list */
        inline void remove(process*);
    
      public:
    
        /** \brief Constructor */
        inline executor(unsigned int = 1);
 
        /** \brief Destructor */
        inline ~executor();
    
        /** \brief Execute a tool */
        inline void execute(const std::string&, process_listener* = 0);
    
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
 
    /**
     * @param[in] p the process to remove
     **/
    inline void executor::remove(process* p) {
      processes.erase(std::find_if(processes.begin(), processes.end(),
                              boost::bind(std::equal_to < process * >(), p,
                                      boost::bind(&process::ptr::get, _1))));
    }
 
    /**
     * @param[in] c the command to execute
     **/
    inline void executor::start_process(const std::string& c) {
      process::ptr p = process::execute(boost::bind(&executor::handle_process_termination, this, _1), c);
    
      if (p.get() != 0) {
        processes.push_back(p);
      }
    }
 
    /**
     * @param[in] c the command to execute
     * @param[in] s reference to a process listener
     **/
    inline void executor::start_process(const std::string& c, process_listener& s) {
      process::ptr p = process::execute(boost::bind(&executor::handle_process_termination, this, _1, &s), c);
    
      s.set_process(p);

      if (p.get() != 0) {
        processes.push_back(p);
 
        s.report_change(process::running);
      }
      else {
        s.report_change(process::aborted);
      }
    }
 
    /**
     * @param[in] c the command to execute
     * @param[in] s a pointer to a process listener or 0
     **/
    inline void executor::start_process(const std::string& c, process_listener* s) {
      if (s == 0) {
        start_process(c);
      }
      else {
        start_process(c, *s);
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
        std::pair < std::string, process_listener* > p = delayed_commands.front();
 
        delayed_commands.pop_front();
 
        start_process(p.first, p.second);
      }
    }
    
    inline void executor::terminate(process* p) {
      remove(p);
    }
 
    /**
     * @param p a pointer to a process object
     **/
    void executor::handle_process_termination(process* p) {
      remove(p);
 
      start_delayed();
    }

    /**
     * @param p a pointer to a process object
     * @param l a pointer to a listener for process state changes
     **/
    void executor::handle_process_termination(process* p, process_listener* l) {
      remove(p);
 
      start_delayed();
 
      l->report_change(p->get_status());
    }
 
    /**
     * @param c the command that is to be executed
     * @param l a pointer to a listener for process state changes
     **/
    void executor::execute(const std::string& c, process_listener* l) {
      if (processes.size() < maximum_concurrent_processes) {
        start_process(c, l);
      }
      else {
        /* queue command for later execution */
        delayed_commands.push_back(std::pair < const std::string, process_listener* >(c, l));
      }
    }
  }
}
  
#endif
