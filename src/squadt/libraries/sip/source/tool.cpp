#include <boost/ref.hpp>

#include <xml2pp/xml_text_reader.h>

#include <sip/tool.h>
#include <sip/detail/message.h>
#include <sip/detail/basic_messenger.tcc>

namespace sip {
  using namespace sip::messenger;

  /**
   * \attention please use connect() to manually establish a connection with a controller
   **/
  tool_communicator::tool_communicator() : current_status(status_initialising), current_capabilities(0) {

    /* Register event handlers for some message types */
    set_handler(boost::bind(&tool_communicator::reply_tool_capabilities, this), sip::request_tool_capabilities);
    set_handler(boost::bind(&tool_communicator::accept_interaction_data, this, _1), sip::send_interaction_data);
  }

  /**
   * The following connection options are recognised and extracted from the command line arguments:
   *
   *  - --sip-connect=\<scheme\>, where \<scheme\> is one of
   *    - socket://\<host\>:\<port\> (for a socket connection)
   *    - compatible (for standard input/output communication)
   *
   * @param argc the number of command line arguments
   * @param argv a pointer to the list of command line arguments
   * \attention the specific command line options are removed, so and argc and argv are modified
   **/
  tool_communicator::tool_communicator(int& argc, char** argv) : current_status(status_initialising), current_capabilities(0) {
  }

  tool_communicator::~tool_communicator() {
  }

  /* Request details about the amount of space that the controller currently has reserved for this tool */
  void tool_communicator::request_controller_capabilities() {
    sip_message m(sip::request_controller_capabilities);

    send_message(m);

    /* Await the reply */
    await_message(sip::reply_controller_capabilities);

    xml2pp::text_reader reader(pop_message().to_string());

    reader.read();

    if (current_capabilities != 0) {
      delete current_capabilities;
    }

    current_capabilities = controller_capabilities::from_xml(reader);
  }

  /* Send a specification of the tools capabilities */
  void tool_communicator::reply_tool_capabilities() {
  }

  /* Send a specification of the current configuration (it may change during tool execution) */
  void tool_communicator::send_accept_configuration() {
  }

  /* Send a layout specification for the display space reserved for this tool */
  void tool_communicator::send_display_layout(layout::display_layout&) {
    std::ostringstream data;

//    current_display_layout.to_xml(data);

    message m(sip::send_display_layout);

    m.set_content(data.str());

    send_message(m);
  }

  /* Send a signal that the tool is about to terminate */
  void tool_communicator::send_signal_termination() {
    send_message(boost::cref(message(sip::send_signal_termination)));
  }

  /* Send a status report to the controller */
  void tool_communicator::send_report(sip::report&) {
  }

  void tool_communicator::accept_interaction_data(sip_messenger::message_ptr&) {
  }
}

