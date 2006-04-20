#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>

#include <sip/detail/basic_messenger.h>

#include "task_monitor.h"
#include "tool.h"

namespace squadt {

  class tool;

  /**
   * \brief Simple processor that queries a tool's capabilities
   *
   * Extracts the tool information that is important for operation of the tool
   * manager.
   **/
  class extractor : public execution::task_monitor {

    public:

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < extractor > ptr;

    private:

      /** \brief handler that accomplishes the actual task */
      inline void handle_store_tool_capabilities(const sip::message_ptr& m, tool& t);

    public:

      /** \brief Constructor */
      inline extractor(tool&);

      /** \brief Starts the extraction */
      inline void start();
  };

  /**
   * @param[in] t reference to the tool object to use for storage
   **/
  inline extractor::extractor(tool& t) : task_monitor() {
    set_handler(bind(&extractor::handle_store_tool_capabilities, this, _1, boost::ref(t)), sip::reply_tool_capabilities);
  }

  /**
   * \pre associated_process.get() is not 0
   **/
  inline void extractor::start() {
    /* Await connection */
    await_connection();

    if (connected) {
      request_tool_capabilities();
    }
  }

  /**
   * @param[in] m the message that was just delivered
   * @param[in,out] t the tool object in which to store the result
   **/
  inline void extractor::handle_store_tool_capabilities(const sip::message_ptr& m, tool& t) {
    xml2pp::text_reader reader(m->to_string().c_str());

    reader.read();
 
    t.capabilities = sip::tool::capabilities::read(reader);

    finish();
  }
}

#endif
