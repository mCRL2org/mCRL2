// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file task_monitor.cpp
/// \brief Add your file description here.

#include "task_monitor.ipp"

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

namespace squadt {
  namespace execution {

    task_monitor::task_monitor() : tipi::controller::communicator(boost::shared_ptr < tipi::controller::communicator_impl > (new task_monitor_impl)) {
    }

    void task_monitor::disconnect(boost::weak_ptr < execution::process > const& p) {
      boost::static_pointer_cast < task_monitor_impl > (impl)->disconnect(p.lock());
    }

    /**
     * @param[in] p shared pointer to the process
     **/
    void task_monitor::attach_process(const process::sptr& p) {
      assert(p.get() != 0);

      boost::static_pointer_cast < task_monitor_impl > (impl)->attach_process(p);
    }

    /** \brief Terminates a running process */
    void task_monitor::terminate_process() {
      boost::static_pointer_cast < task_monitor_impl > (impl)->finish(boost::static_pointer_cast < task_monitor_impl > (impl));
    }

    /**
     * @param[in] b whether the function should block until the process has registered or not
     *
     * \return A pointer to the associated process, or 0 on program failure
     **/
    process::sptr task_monitor::get_process(const bool b) const {
      if (b) {
        boost::static_pointer_cast < task_monitor_impl > (impl)->await_process();
      }

      return (boost::static_pointer_cast < task_monitor_impl > (impl)->associated_process);
    }

    void task_monitor::await_process() const {
      boost::static_pointer_cast < task_monitor_impl > (impl)->await_process();
    }

    /**
     * \param[in] b whether or not to wait for processes to finish
     **/
    void task_monitor::finish() {
      boost::static_pointer_cast < task_monitor_impl > (impl)->finish(
        boost::static_pointer_cast < task_monitor_impl > (impl));
    }

    void task_monitor::shutdown() {
      boost::static_pointer_cast < task_monitor_impl > (impl)->shutdown();
    }

    /**
     * \param[in] ts the maximum number of seconds to block
     **/
    bool task_monitor::await_connection(unsigned int const& ts) {
      return boost::static_pointer_cast < task_monitor_impl > (impl)->await_connection(ts);
    }

    bool task_monitor::await_connection() {
      return boost::static_pointer_cast < task_monitor_impl > (impl)->await_connection();
    }

    bool task_monitor::await_completion() {
      return boost::static_pointer_cast < task_monitor_impl > (impl)->await_completion();
    }

    /**
     * @param[in] e the other end point of the connection
     **/
    void task_monitor::signal_connection(tipi::message::end_point e) {
      boost::shared_ptr < task_monitor_impl > m = boost::static_pointer_cast < task_monitor_impl > (impl);

      if (impl) {
        m->signal_connection(m, e);
      }
    }

    /**
     * @param[in] s the current status of the process
     **/
    void task_monitor::signal_change(boost::shared_ptr < execution::process > p, const execution::process::status s) {
      boost::shared_ptr < task_monitor_impl > m = boost::static_pointer_cast < task_monitor_impl > (impl);

      m->signal_change(m, p, s);
    }

    /**
     * @param[in] h the function object that is executed after the process status has changed
     **/
    void task_monitor::on_status_change(boost::function < void () > h) {
      boost::static_pointer_cast < task_monitor_impl > (impl)->on_status_change(h);
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    void task_monitor::on_connection(boost::function < void () > h) {
      boost::static_pointer_cast < task_monitor_impl > (impl)->on_connection(h);
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor::once_on_connection(boost::function < void () > h) {
      boost::static_pointer_cast < task_monitor_impl > (impl)->once_on_connection(h);
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    void task_monitor::on_completion(boost::function < void () > h) {
      boost::static_pointer_cast < task_monitor_impl > (impl)->on_completion(h);
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    void task_monitor::once_on_completion(boost::function < void () > h) {
      boost::static_pointer_cast < task_monitor_impl > (impl)->once_on_completion(h);
    }
  }
}
