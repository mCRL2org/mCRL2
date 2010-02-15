// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file executor.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#include <algorithm>
#include <ostream>
#include <deque>
#include <functional>
#include <string>
#include <list>

#include "boost/bind.hpp"
#include "boost/weak_ptr.hpp"
#include "boost/foreach.hpp"
#include "boost/thread/thread.hpp"

#include "tipi/detail/utility/generic_visitor.hpp"

#include "executor.ipp"

namespace squadt {
  namespace execution {

    /// \cond INTERNAL_DOCS
    inline executor_impl::executor_impl(unsigned int m) : maximum_instance_count(m) {
    }

    /**
     * \param[in] p the process to remove
     **/
    inline void executor_impl::remove(process* p) {
      for (std::list < boost::shared_ptr < process > >::iterator i = processes.begin(); i != processes.end(); ++i) {
        if (i->get() == p) {
          processes.erase(i);

          break;
        }
      }
    }

    /**
     * \param[in] c the command to execute
     * \param[in] w a pointer to the associated implementation object
     **/
    inline void executor_impl::start_process(const command& c, boost::shared_ptr < executor_impl >& w) {
      boost::shared_ptr < process > p(process::create());

      processes.push_back(p);

      p->execute(c, boost::bind(&executor_impl::handle_process_termination, this, w, _1));
    }

    /**
     * \param[in] c the command to execute
     * \param[in] l reference to a process listener
     * \param[in] w a pointer to the associated implementation object
     **/
    inline void executor_impl::start_process(const command& c, boost::shared_ptr < task_monitor >& l, boost::shared_ptr < executor_impl >& w) {
      if(!(l.get() != 0)){
        throw std::runtime_error( "Task monitor is NULL " );
      }

      boost::shared_ptr < process > p(process::create());

      if (l) {
        l->attach_process(p);
        l->get_logger().log(1, "executing command `" + c.string() + "'\n");
        l->signal_change(p, process::running);
      }

      processes.push_back(p);

      p->execute(c, boost::bind(&executor_impl::handle_process_termination, this, w, l, _1));
    }

    /**
     * \param c the command that is to be executed
     * \param l a shared pointer a listener (or reference to) for process state changes
     * \param b whether or not to circumvent the number of running processes limit
     **/
    inline void executor_impl::execute(const command& c, boost::shared_ptr < task_monitor >& l, bool b, boost::shared_ptr < executor_impl >& w) {
      if (b || processes.size() < maximum_instance_count) {
        boost::thread t(boost::bind(&executor_impl::start_process, this, c, l, w));
      }
      else {
        /* queue command for later execution */
        delayed_commands.push_back(boost::bind(&executor_impl::start_process, this, c, l, _1));
      }
    }

    /**
     * \param c the command that is to be executed
     * \param b whether or not to circumvent the number of running processes limit
     **/
    inline void executor_impl::execute(command const& c, bool b, boost::shared_ptr < executor_impl >& w) {
      boost::shared_ptr < task_monitor > p;

      if (b || processes.size() < maximum_instance_count) {
        boost::thread t(boost::bind(&executor_impl::start_process, this, c, w));
      }
      else {
        /* queue command for later execution */
        delayed_commands.push_back(boost::bind(&executor_impl::start_process, this, c, _1));
      }
    }

    /**
     * \note the queue with commands for to be started processes is also cleared by this function.
     **/
    inline void executor_impl::terminate_all() {
      std::list < boost::shared_ptr < process > > aprocesses;

      aprocesses.swap(processes);

      delayed_commands.clear();

      BOOST_FOREACH(boost::shared_ptr < process > p, aprocesses) {
        p->terminate();
      }
    }

    /**
     * Start processing commands if the queue contains any waiters
     *
     * \param[in] w a pointer to the associated implementation object
     **/
    inline void executor_impl::start_delayed(boost::shared_ptr < executor_impl >& w) {
      if (0 < delayed_commands.size()) {
        boost::function < void (boost::shared_ptr < executor_impl >&) > f = delayed_commands.front();

        delayed_commands.pop_front();

        f(w);
      }
    }

    size_t executor_impl::get_maximum_instance_count() const {
      return (maximum_instance_count);
    }

    void executor_impl::set_maximum_instance_count(size_t m) {
      maximum_instance_count = m;
    }

    /**
     * \param p a pointer to a process object
     **/
    void executor_impl::handle_process_termination(boost::weak_ptr < executor_impl > w, boost::shared_ptr < process > p) {
      boost::shared_ptr < executor_impl > alive(w.lock());

      if (alive) {
        remove(p.get());

        start_delayed(alive);
      }
    }

    /**
     * \param p a pointer to a process object
     **/
    void executor_impl::handle_process_termination(boost::weak_ptr < executor_impl > w,
                                boost::weak_ptr < task_monitor > l, boost::shared_ptr < process > p) {

      boost::shared_ptr< executor_impl > alive(w.lock());

      if (alive) {
        boost::shared_ptr< task_monitor > monitor(l.lock());

        if (monitor) {
          monitor->signal_change(p, p->get_status());
          monitor->disconnect(boost::weak_ptr< process >(p));
        }

        remove(p.get());

        start_delayed(alive);
      }
    }

    /// \endcond

    /**
     * Default constructor
     *
     * \param[in] m the maximum number of allowed concurrent processes
     **/
    executor::executor(unsigned int const& m) : impl(new executor_impl(m)) {
    }

    executor::~executor() {
      impl->terminate_all();
    }

    size_t executor::get_maximum_instance_count() const {
      return (impl->maximum_instance_count);
    }

    void executor::set_maximum_instance_count(size_t m) {
      impl->maximum_instance_count = m;
    }

    /**
     * \param[in] p the process to remove
     **/
    void executor::remove(process* p) {
      impl->remove(p);
    }

    /**
     * \param[in] c the command to execute
     **/
    void executor::start_process(const command& c) {
      impl->start_process(c, impl);
    }

    /**
     * \param[in] c the command to execute
     * \param[in] l reference to a process listener
     **/
    void executor::start_process(const command& c, boost::shared_ptr< task_monitor >& l) {
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
     * \param[in] c the command that is to be executed
     * \param[in] l a shared pointer a listener (or reference to) for process state changes
     * \param[in] b whether or not to circumvent the number of running processes limit
     **/
    void executor::execute(command const& c, boost::shared_ptr < task_monitor >& l, bool b) {
      impl->execute(c, l, b, impl);
    }

    /**
     * \param[in] c the command that is to be executed
     * \param[in] b whether or not to circumvent the number of running processes limit
     **/
    void executor::execute(command const& c, bool b) {
      impl->execute(c, b, impl);
    }
  }
}

