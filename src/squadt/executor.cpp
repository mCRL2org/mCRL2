#include <algorithm>
#include <ostream>
#include <deque>
#include <functional>
#include <string>
#include <list>

#include <boost/bind.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>

#include "executor.h"

namespace squadt {
  namespace execution {

    class executor_impl {
      friend class executor;

      private:

        /** Convenient short-hand type */
        typedef std::pair < command, task_monitor::sptr > command_pair;

      private:

        /** \brief The maximum number of processes that is allowed to run concurrently */
        unsigned int                   maximum_concurrent_processes;
 
        /** \brief List of active processes */
        std::list < process::ptr >     processes;
 
        /** \brief Data of processes that will be started */
        std::deque < command_pair >    delayed_commands;

      private:

        /** \brief handler that is invoked when a process is terminated */
        inline void handle_process_termination(process* p, boost::weak_ptr < executor_impl >);
  
        /** \brief handler that is invoked when a process is terminated */
        inline void handle_process_termination(process* p, task_monitor::sptr& s, boost::weak_ptr < executor_impl >);

        /** \brief Start a new process */
        inline void start_process(const command&, boost::shared_ptr < executor_impl >&);
    
        /** \brief Start a new process with a listener */
        inline void start_process(const command&, task_monitor::sptr&, boost::shared_ptr < executor_impl >&);

        /** \brief Start processing commands if the queue contains any waiters */
        inline void start_delayed(boost::shared_ptr < executor_impl >&);
 
        /** \brief Remove a process from the list */
        inline void remove(process*);
  
      public:

        /** \brief Constructor */
        inline executor_impl(unsigned int);
 
        /** \brief Execute a tool */
        void execute(const command&, boost::shared_ptr < task_monitor >&, bool, boost::shared_ptr < executor_impl >&);
    
        /** \brief Terminate a specific process */
        inline void terminate(process*);
    
        /** \brief Terminate a specific process */
        inline void terminate(process::wptr);
    
        /** \brief Terminate all processes */
        void terminate_all();
    };

    inline executor_impl::executor_impl(unsigned int m) : maximum_concurrent_processes(m) {
    }
    
    /**
     * @param[in] p the process to remove
     **/
    inline void executor_impl::remove(process* p) {
      static boost::mutex lock;

      boost::mutex::scoped_lock w(lock);

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
    inline void executor_impl::start_process(const command& c, boost::shared_ptr < executor_impl >& w) {
      process::ptr p(new process(boost::bind(&executor_impl::handle_process_termination, this, _1, w)));

      processes.push_back(p);

      p->execute(c);
    }

    /**
     * @param[in] c the command to execute
     * @param[in] l reference to a process listener
     **/
    inline void executor_impl::start_process(const command& c, task_monitor::sptr& l, boost::shared_ptr < executor_impl >& w) {
      process::ptr p(new process(boost::bind(&executor_impl::handle_process_termination, this, _1, l, w), l));

      if (l.get() != 0) {
        l->attach_process(p);
      }

      processes.push_back(p);

      p->execute(c);
    }

    /**
     * @param c the command that is to be executed
     * @param l a shared pointer a listener (or reference to) for process state changes
     * @param b whether or not to circumvent the number of running processes limit
     **/
    inline void executor_impl::execute(const command& c, task_monitor::sptr& l, bool b, boost::shared_ptr < executor_impl >& w) {
      if (b || processes.size() < maximum_concurrent_processes) {
        start_process(c, l, w);
      }
      else {
        /* queue command for later execution */
        delayed_commands.push_back(command_pair(c, l));
      }
    }

    /**
     * The queue with commands for to be started processes is also cleared by this function.
     **/
    inline void executor_impl::terminate_all() {
    
      delayed_commands.clear();

      BOOST_FOREACH(process::ptr p, processes) {
        p->terminate();
      }
    }

    /* Start processing commands if the queue contains any waiters */
    inline void executor_impl::start_delayed(boost::shared_ptr < executor_impl >& w) {
      if (0 < delayed_commands.size()) {
        command_pair c = delayed_commands.front();
 
        delayed_commands.pop_front();
 
        start_process(c.first, c.second, w);
      }
    }
    
    /**
     * @param p a pointer to a process object
     **/
    inline void executor_impl::handle_process_termination(process* p, boost::weak_ptr < executor_impl > w) {
      boost::shared_ptr < executor_impl > g = w.lock();

      if (g.get() != 0) {
        remove(p);
 
        start_delayed(g);
      }
    }

    /**
     * @param p a pointer to a process object
     * @param l a pointer to a listener for process state changes
     **/
    inline void executor_impl::handle_process_termination(process* p, task_monitor::sptr& l, boost::weak_ptr < executor_impl > w) {
      boost::shared_ptr < executor_impl > g = w.lock();

      if (g.get() != 0) {
        remove(p);
 
        start_delayed(g);
      }
    }

    executor::executor(unsigned int m) : impl(new executor_impl(m)) {
    }
    
    executor::~executor() {
      impl->terminate_all();
    }
 
    /**
     * @param[in] p the process to remove
     **/
    void executor::remove(process* p) {
      impl->remove(p);
    }

    /**
     * @param[in] c the command to execute
     **/
    void executor::start_process(const command& c) {
      impl->start_process(c, impl);
    }
 
    /**
     * @param[in] c the command to execute
     * @param[in] l reference to a process listener
     **/
    void executor::start_process(const command& c, task_monitor::sptr& l) {
      impl->start_process(c, l, impl);
    }

    /**
     * The queue with commands for to be started processes is also cleared by this function.
     **/
    void executor::terminate_all() {
      impl->terminate_all();
    }
 
    /* Start processing commands if the queue contains any waiters */
    void executor::start_delayed() {
      impl->start_delayed(impl);
    }
 
    /**
     * @param c the command that is to be executed
     * @param l a shared pointer a listener (or reference to) for process state changes
     * @param b whether or not to circumvent the number of running processes limit
     **/
    void executor::execute(const command& c, task_monitor::sptr& l, bool b) {
      impl->execute(c, l, b, impl);
    }
  }
}

