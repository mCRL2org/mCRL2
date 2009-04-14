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

#include "boost.hpp" // precompiled headers

#include "task_monitor.ipp"

#include <boost/function.hpp>

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
    void task_monitor::attach_process(const boost::shared_ptr< process >& p) {
      assert(p.get() != 0);

      boost::static_pointer_cast < task_monitor_impl > (impl)->attach_process(p);
    }

    /** \brief Terminates a running process */
    void task_monitor::terminate_process() {
      finish();
    }

    /**
     * \param[in] b whether the function should block until the process has registered or not
     *
     * \return A pointer to the associated process, or 0 on program failure
     **/
    boost::shared_ptr< process > task_monitor::get_process(const bool b) const {
      boost::shared_ptr< task_monitor_impl > limpl = boost::static_pointer_cast < task_monitor_impl >(impl);

      if (b) {
        limpl->await_process(boost::static_pointer_cast < task_monitor_impl >(impl));
      }

      return limpl->associated_process;
    }

    void task_monitor::await_process() const {
      boost::static_pointer_cast < task_monitor_impl > (impl)->
                await_process(boost::static_pointer_cast < task_monitor_impl >(impl));
    }

    /**
     * \param[in] b whether or not to wait for processes to finish
     **/
    void task_monitor::finish(bool b) {
      boost::static_pointer_cast < task_monitor_impl > (impl)->finish(
                boost::static_pointer_cast < task_monitor_impl > (impl), b);
    }

    void task_monitor::shutdown() {
      boost::static_pointer_cast < task_monitor_impl > (impl)->shutdown();
    }

    /**
     * \param[in] ts the maximum number of seconds to block
     **/
    bool task_monitor::await_connection(unsigned int const& ts) {
      return boost::static_pointer_cast < task_monitor_impl > (impl)->
                await_connection(boost::static_pointer_cast < task_monitor_impl > (impl), ts);
    }

    bool task_monitor::await_connection() {
      return boost::static_pointer_cast < task_monitor_impl > (impl)->
                await_connection(boost::static_pointer_cast < task_monitor_impl > (impl));
    }

    bool task_monitor::await_completion() {
      return boost::static_pointer_cast < task_monitor_impl > (impl)->
                await_completion(boost::static_pointer_cast < task_monitor_impl >(impl));
    }

    /**
     * @param[in] e the other end point of the connection
     **/
    void task_monitor::signal_connection(tipi::message::end_point e) {
      boost::static_pointer_cast < task_monitor_impl >(impl)->
              signal_connection(boost::static_pointer_cast < task_monitor_impl >(impl), e);
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
    void task_monitor::once_on_connection(boost::function < void () > h) {
      boost::static_pointer_cast < task_monitor_impl > (impl)->once_on_connection(h);
    }

    /**
     * @param[in] h the function object that is executed
     **/
    void task_monitor::on_completion(boost::function < void () > h) {
      boost::static_pointer_cast < task_monitor_impl > (impl)->on_completion(h);
    }

    /**
     * @param[in] h the function object that is executed
     **/
    void task_monitor::once_on_completion(boost::function < void () > h) {
      boost::static_pointer_cast < task_monitor_impl > (impl)->once_on_completion(h);
    }

    /// \cond INTERNAL
    /**
     * \pre p.get() == this
     **/
    void task_monitor_impl::await_process(boost::weak_ptr< task_monitor_impl > p) {
      boost::mutex::scoped_lock l(register_lock);

      while (associated_process.get() == 0) {
        register_condition.wait(l);
      }
    }

    /**
     * Waits until a connection has been established a timeout has occurred, or the process has terminated
     * \pre p.get() == this
     * \return whether a connection is active
     **/
    bool task_monitor_impl::await_connection(boost::weak_ptr< task_monitor_impl > p, unsigned int const& ts) {
      boost::shared_ptr< task_monitor_impl > pl(p.lock());

      if (pl) {
        boost::mutex::scoped_lock l(register_lock);

        if (!associated_process) {
          register_condition.wait(l);
        }

        /* Other side has not connected and the process has not been registered as terminated */
        if (number_of_connections() == 0) {
          connection_condition.timed_wait(l, boost::get_system_time() + boost::posix_time::seconds(ts));
        }

        return 0 < number_of_connections();
      }

      return false;
    }

    /**
     * Waits until a connection has been established, or the process has terminated
     * \pre p.get() == this
     * \return whether a connection is active
     **/
    bool task_monitor_impl::await_connection(boost::weak_ptr< task_monitor_impl > p) {
      boost::shared_ptr< task_monitor_impl > pl(p.lock());

      if (pl) {
        boost::mutex::scoped_lock l(register_lock);

        if (!associated_process) {
          register_condition.wait(l);
        }

        /* Other side has not connected and the process has not been registered as terminated */
        if (number_of_connections() == 0) {
          connection_condition.wait(l);
        }

        return 0 < number_of_connections();
      }

      return false;
    }

    /**
     * Waits until a connection has been established, or the process has terminated
     * \pre p.get() == this
     * \return whether the task has been completed successfully
     **/
    bool task_monitor_impl::await_completion(boost::weak_ptr< task_monitor_impl > p) {
      struct local {
        static void handle_task_completion(boost::weak_ptr< task_monitor_impl > t,
                        boost::shared_ptr < const tipi::message > const& m, bool& result) {

          boost::shared_ptr< task_monitor_impl > pl(t.lock());

          if (pl) {
            result = m.get() && !m->is_empty();

            boost::mutex::scoped_lock l(pl->register_lock);

            pl->completion_condition.notify_all();

            pl->clear_handlers(tipi::message_task);
          }
        }
      };

      bool result = false;

      boost::mutex::scoped_lock l(register_lock);

      add_handler(tipi::message_task, boost::bind(&local::handle_task_completion, p, _1, boost::ref(result)));

      /* Other side has not connected and the process has not been registered as terminated */
      completion_condition.wait(l);

      return result;
    }

    /**
     * \param[in] m a shared pointer to the current object
     * \pre p.get() == this
     **/
    void task_monitor_impl::signal_connection(boost::weak_ptr < task_monitor_impl > p, tipi::message::end_point) {
      boost::shared_ptr< task_monitor_impl > pl(p.lock());

      if (pl) {
        boost::mutex::scoped_lock l(register_lock);

        boost::shared_ptr< process > process(associated_process);

        if (process) {
          logger->log(1, boost::str(boost::format("connection established with `%s' (process id %u)\n")
                  % process->get_executable_name() % process->get_identifier()));
        }

        /* Service connection handlers */
        if (0 < handlers.count(connection)) {
          task_monitor_impl::service_handlers(pl, connection);
        }

        connection_condition.notify_all();
      }
    }

    void task_monitor_impl::shutdown() {
      boost::mutex::scoped_lock l(register_lock);

      handlers.clear();

      /* Signal completion to waiters */
      register_condition.notify_all();
      connection_condition.notify_all();
      completion_condition.notify_all();
    }

    /**
     * \param[in] m a shared pointer to the current object
     * \param[in] s the current status of the process
     **/
    void task_monitor_impl::signal_change(boost::shared_ptr < task_monitor_impl >& m,
                        boost::shared_ptr < execution::process > p, const execution::process::status s) {

      boost::mutex::scoped_lock l(register_lock);

      /* Service change handlers */
      if (0 < handlers.count(change)) {
        task_monitor_impl::service_handlers(m, change);
      }

      if (s == execution::process::completed || s == execution::process::aborted) {
        if (associated_process == p) {
          associated_process.reset();
        }

        /* Service connection handlers */
        if (0 < handlers.count(completion)) {
          task_monitor_impl::service_handlers(m, completion);
        }

        if (p.get()) {
          logger->log(1, boost::format("process ended `%s' (process id %u, exit status %u)\n")
                    % p->get_executable_name() % p->get_identifier() % s);
        }

        /* Signal completion to waiters */
        completion_condition.notify_all();
      }
    }

    /**
     * \param[in] m a shared pointer to this object
     * \param[in] e the event type of which to execute the handler
     **/
    void task_monitor_impl::service_handlers(boost::shared_ptr < task_monitor_impl >& m, const event_type e) {
      std::vector< handler_map::iterator > handlers_to_remove;

      for (std::pair < handler_map::iterator, handler_map::iterator > r(m->handlers.equal_range(e)); r.first != r.second; ++r.first) {
        if (r.first->second()) {
          handlers_to_remove.push_back(r.first);
        }
      }

      // works because iterators of std::multimap are not invalidated by std::multimap::erase()
      for (std::vector< handler_map::iterator >::const_iterator i = handlers_to_remove.begin(); i != handlers_to_remove.end(); ++i) {
        m->handlers.erase(*i);
      }
    }

    /**
     * \param[in] p shared pointer to the process
     **/
    void task_monitor_impl::attach_process(boost::shared_ptr< process > const& p) {
      boost::mutex::scoped_lock l(register_lock);

      associated_process = p;

      /* Wake up waiting threads */
      register_condition.notify_all();
    }

    /**
     * \param[in] b whether to wait for the process to terminate (whether or not to block)
     * \param[in] g shared pointer to this object, to ensure its existence
     **/
    void task_monitor_impl::finish(boost::shared_ptr < task_monitor_impl > const& g, bool b) {
      boost::shared_ptr< process > process;

      process.swap(associated_process);

      if (b) {
        boost::thread(boost::bind(&task_monitor_impl::terminate_process, this, g, process));
      }
      else {
        terminate_process(g, process);
      }
    }

    bool task_monitor_impl::disconnect(boost::shared_ptr < execution::process > p) {
      boost::mutex::scoped_lock l(register_lock);

      if (0 < number_of_connections()) {
        // request termination
        send_message(tipi::message_termination);

        if (p.get() && p->get_status() == process::running) {
          logger->log(1, boost::str(boost::format("termination request sent to %s (process id %u)\n")
                  % p->get_executable_name() % p->get_identifier()));
        }

        tipi::controller::communicator_impl::disconnect();

        return true;
      }

      return false;
    }

    /**
     * \brief Terminates a running process
     *
     * \param[in] g shared pointer to this object, to ensure its existence
     **/
    void task_monitor_impl::terminate_process(boost::shared_ptr < task_monitor_impl > g, boost::shared_ptr < execution::process > p) {

      // remove messages from queue
      clear_queue();

      if (disconnect(p)) {
        boost::this_thread::sleep(boost::posix_time::seconds(5));
      }

      if (p && p->get_status() == execution::process::running) {
        logger->log(1, boost::format("forcibly terminating process (tool %s with id %u)\n") %
                      p->get_executable_name() % p->get_identifier());

        p->terminate();
      }
    }
    /// \endcond
  }
}
