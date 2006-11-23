#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

#include "task_monitor.tcc"

namespace squadt {
  namespace execution {

    task_monitor::task_monitor() : sip::controller::communicator(new task_monitor_impl) {
    }

    void task_monitor::disconnect(execution::process*) {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->connected = false;

      impl->disconnect();
    }

    /**
     * @param[in] p shared pointer to the process
     **/
    void task_monitor::attach_process(const process::sptr& p) {
      assert(p.get() != 0);

      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->attach_process(p);
    }

    /** \brief Terminates a running process */
    void task_monitor::terminate_process() {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->terminate_process();
    }

    /**
     * @param[in] b whether the function should block untill the process has registered or not
     *
     * \return A pointer to the associated process, or 0 on program failure
     **/
    process::sptr task_monitor::get_process(const bool b) const {
      if (b) {
        boost::dynamic_pointer_cast < task_monitor_impl > (impl)->await_process();
      }

      return (boost::dynamic_pointer_cast < task_monitor_impl > (impl)->associated_process);
    }

    void task_monitor::await_process() const {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->await_process();
    }

    void task_monitor::finish() {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->finish();
    }

    void task_monitor::shutdown() {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->shutdown();
    }

    /**
     * \param[in] ts the maximum number of seconds to block
     **/
    void task_monitor::await_connection(unsigned int const& ts) {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->await_connection(ts);
    }

    void task_monitor::await_connection() {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->await_connection();
    }

    /**
     * @param[in] e the other end point of the connection
     **/
    void task_monitor::signal_connection(sip::message::end_point e) {
      boost::shared_ptr < task_monitor_impl > m = boost::dynamic_pointer_cast < task_monitor_impl > (impl);

      m->signal_connection(m, e);

//      get_logger->log(1, boost::str(boost::format("connection established with %s pid(%u)\n")
//                % m->associated_process->get_executable_name() % m->associated_process->get_identifier()));
    }

    /**
     * @param[in] s the current status of the process
     **/
    void task_monitor::signal_change(const execution::process::status s) {
      boost::shared_ptr < task_monitor_impl > m = boost::dynamic_pointer_cast < task_monitor_impl > (impl);

      m->signal_change(m, s);
    }

    /**
     * @param[in] h the function object that is executed after the process status has changed
     **/
    void task_monitor::on_status_change(boost::function < void () > h) {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->on_status_change(h);
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    void task_monitor::on_connection(boost::function < void () > h) {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->on_connection(h);
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    inline void task_monitor::once_on_connection(boost::function < void () > h) {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->once_on_connection(h);
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    void task_monitor::on_completion(boost::function < void () > h) {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->on_completion(h);
    }

    /**
     * @param[in] h the function object that is executed once a connection is established
     **/
    void task_monitor::once_on_completion(boost::function < void () > h) {
      boost::dynamic_pointer_cast < task_monitor_impl > (impl)->once_on_completion(h);
    }


    bool task_monitor::is_connected() const {
      return (boost::dynamic_pointer_cast < task_monitor_impl > (impl)->connected);
    }

    bool task_monitor::is_busy() const {
      return (!boost::dynamic_pointer_cast < task_monitor_impl > (impl)->done);
    }
  }
}
