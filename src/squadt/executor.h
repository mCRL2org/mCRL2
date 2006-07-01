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
#include <boost/foreach.hpp>

#include "task_monitor.h"

namespace squadt {
  namespace execution {

    class command;

    /**
     * \brief Basic process execution component
     *
     * Design choices:
     *  - only a predetermined fixed number of processes can is run concurrently,
     *    which means that sometimes execution is delayed
     *  - the execute method is non-blocking; consequently nothing can be
     *    concluded about the execution of the command except through a
     *    task_monitor object
     **/
    class executor {

      private:

        /** Convenient short-hand type */
        typedef std::pair < command, task_monitor::sptr > command_pair;

      private:

        /** \brief The maximum number of processes that is allowed to run concurrently */
        unsigned int maximum_concurrent_processes;
 
        /** \brief List of active processes */
        std::list < process::ptr >     processes;
 
        /** \brief Data of processes that will be started */
        std::deque < command_pair >    delayed_commands;

      private:
    
        /** \brief Start a new process */
        inline void start_process(const command&);
    
        /** \brief Start a new process with a listener */
        inline void start_process(const command&, task_monitor::sptr&);

        /** \brief handler that is invoked when a process is terminated */
        inline void handle_process_termination(process* p);
  
        /** \brief handler that is invoked when a process is terminated */
        inline void handle_process_termination(process* p, task_monitor::sptr& s);
  
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
        template < typename T >
        inline void execute(const command&, T = process::default_monitor, bool = false);
    
        /** \brief Terminate a specific process */
        inline void terminate(process*);
    
        /** \brief Terminate a specific process */
        inline void terminate(process::wptr);
    
        /** \brief Terminate all processes */
        inline void terminate_all();
    };
 
    inline executor::executor(unsigned int m) : maximum_concurrent_processes(m) {
    }
    
    inline executor::~executor() {
      terminate_all();
    }
 
    /**
     * @param[in] p the process to remove
     **/
    inline void executor::remove(process* p) {
      std::list < process::ptr >::iterator i = std::find_if(processes.begin(), processes.end(),
                              boost::bind(std::equal_to < process* >(), p,
                                      boost::bind(&process::ptr::get, _1)));

      if (i != processes.end()) {
        processes.erase(i);
      }
    }
 
    /**
     * @param[in] c the command to execute
     **/
    inline void executor::start_process(const command& c) {
      process::ptr p(new process(boost::bind(&executor::handle_process_termination, this, _1)));
      
      processes.push_back(p);

      p->execute(c);
    }
 
    /**
     * @param[in] c the command to execute
     * @param[in] l reference to a process listener
     **/
    inline void executor::start_process(const command& c, task_monitor::sptr& l) {
      process::ptr p(new process(boost::bind(&executor::handle_process_termination, this, _1, l), l));

      if (l.get() != 0) {
        l->attach_process(p);
      }

      processes.push_back(p);

      p->execute(c);
    }

    /**
     * The queue with commands for to be started processes is also cleared by this function.
     **/
    inline void executor::terminate_all() {
    
      delayed_commands.clear();
    
      BOOST_FOREACH(process::ptr p, processes) {
        p->terminate();
      }
    }
 
    /* Start processing commands if the queue contains any waiters */
    inline void executor::start_delayed() {
      if (0 < delayed_commands.size()) {
        command_pair c = delayed_commands.front();
 
        delayed_commands.pop_front();
 
        start_process(c.first, c.second);
      }
    }
    
    /**
     * @param[in] p a weak pointer (or reference to) to the process that should be terminated
     **/
    inline void executor::terminate(process* p) {
      p->terminate();
    }
 
    /**
     * @param[in] p a shared pointer (or reference to) to the process that should be terminated
     **/
    inline void executor::terminate(process::wptr p) {
      process::ptr w = p.lock();

      if (w.get() != 0) {
        w->terminate();
      }
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
    void executor::handle_process_termination(process* p, task_monitor::sptr& l) {
      remove(p);
 
      start_delayed();
    }
 
    /**
     * @param c the command that is to be executed
     * @param l a shared pointer a listener (or reference to) for process state changes
     * @param b whether or not to circumvent the number of running processes limit
     **/
    template < typename T >
    void executor::execute(const command& c, T l, bool b) {
      if (b || processes.size() < maximum_concurrent_processes) {
        start_process(c, l);
      }
      else {
        /* queue command for later execution */
        delayed_commands.push_back(command_pair(c, l));
      }
    }
  }
}
  
#endif
