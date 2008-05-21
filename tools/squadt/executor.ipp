// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file executor.ipp
/// \brief Add your file description here.

#include "executor.hpp"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace squadt {
  /// \cond INTERNAL_DOCS

  namespace execution {

    /** \brief Implementation class with the executor class (handle-body idiom) */
    class executor_impl : public utility::visitable {
      friend class executor;

      template < typename R, typename S >
      friend class utility::visitor;

      private:

        /** \brief The maximum number of processes that is allowed to run concurrently */
        unsigned int                                                                      maximum_instance_count;
 
        /** \brief List of active processes */
        std::list < boost::shared_ptr < process > >                                       processes;
 
        /** \brief Data of processes that will be started */
        std::deque < boost::function < void (boost::shared_ptr < executor_impl >&) > >    delayed_commands;

      private:

        /** \brief handler that is invoked when a process is terminated */
        void handle_process_termination(boost::weak_ptr < executor_impl > w, boost::shared_ptr < process >);

        /** \brief handler that is invoked when a process is terminated */
        void handle_process_termination(boost::weak_ptr < executor_impl > w, boost::weak_ptr < task_monitor >, boost::shared_ptr < process >);
        /** \brief Start a new process */
        void start_process(const command&, boost::shared_ptr < executor_impl >&);
    
        /** \brief Start a new process with a listener */
        void start_process(const command&, task_monitor::sptr&, boost::shared_ptr < executor_impl >&);

        /** \brief Start processing commands if the queue contains any waiters */
        void start_delayed(boost::shared_ptr < executor_impl >&);
 
        /** \brief Remove a process from the list */
        void remove(process*);
  
      public:

        /** \brief Constructor */
        executor_impl(unsigned int);
 
        /** \brief Execute a tool */
        void execute(const command&, boost::shared_ptr < task_monitor >&, bool, boost::shared_ptr < executor_impl >&);
    
        /** \brief Execute a command */
        void execute(const command&, bool, boost::shared_ptr < executor_impl >&);

        /** \brief Maximum number of tool instances */
        size_t get_maximum_instance_count() const;
 
        /** \brief Maximum number of tool instances */
        void set_maximum_instance_count(size_t);
    
        /** \brief Terminate a specific process */
        void terminate(process*);
    
        /** \brief Terminate a specific process */
        void terminate(process::wptr);
    
        /** \brief Terminate all processes */
        void terminate_all();
    };
  }
  /// \endcond
}
