#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <boost/shared_ptr.hpp>

#include <sip/detail/basic_messenger.h>

#include "task.h"
#include "tool.h"

namespace squadt {

  class tool;

  /**
   * \brief Simple processor that queries a tool's capabilities
   *
   * Extracts the tool information that is important for operation of the tool
   * manager.
   **/
  class extractor : public execution::task {

    public:

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < extractor > ptr;

    private:

      /** \brief handler that accomplishes the actual task */
      inline void handle_store_tool_capabilities(sip::message_ptr& m, tool& t);

      /** \brief Checks the process status and removes */
      inline void report_change(const execution::process::status);

      /** \brief Unblocks waiters and requests a tool to prepare termination */
      inline void finish();

    public:

      /** \brief Constructor */
      inline extractor(tool&);

      /** \brief Function that blocks until all needed data is gathered from a tool */
      inline void await_completion();
  };

  /**
   * @param[in] t reference to the tool object to use for storage
   **/
  inline extractor::extractor(tool& t) {
    set_handler(bind(&extractor::handle_store_tool_capabilities, this, _1, t), sip::reply_tool_capabilities);

    request_tool_capabilities();
  }

  /**
   * @param[in] m the message that was just delivered
   * @param[in,out] t the tool object in which to store the result
   **/
  inline void extractor::handle_store_tool_capabilities(sip::message_ptr& m, tool& t) {
    xml2pp::text_reader reader(m->to_string().c_str());
 
    reader.read();
 
    t.capabilities = sip::tool::capabilities::read(reader);
  }

  inline void extractor::finish() {
    /* Let the tool know that it should prepare for termination */
    request_termination();

    boost::mutex::scoped_lock l(register_lock);

    /* Signal completion to waiters */
    register_condition->notify_all();
  }

  /**
   * @param[in] s the current status of the process
   **/
  inline void extractor::report_change(const execution::process::status s) {
    if (s == execution::process::completed || s == execution::process::aborted) {
      /* Unblock any remaining waiters */
      boost::mutex::scoped_lock l(register_lock);

      /* Signal completion to waiters */
      register_condition->notify_all();
    }
  }

  /**
   * \pre associated_process.lock().get() must be unequal 0
   **/
  inline void extractor::await_completion() {
    boost::mutex::scoped_lock l(register_lock);

    assert (associated_process.lock().get() != 0);

    if (!register_condition.get()) {
      register_condition = boost::shared_ptr < boost::condition > (new boost::condition());
    }

    register_condition->wait(l);
  }
}

#endif
