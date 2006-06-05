#include <xml2pp/text_reader.h>

#include <sip/tool.h>
#include <sip/detail/message.h>
#include <sip/detail/basic_messenger.tcc>
#include <sip/detail/command_line_interface.tcc>

namespace sip {
  namespace tool {
    using namespace sip::messaging;

    /**
     * \attention please use connect() to manually establish a connection with a controller
     **/
    communicator::communicator() : current_status(status_inactive),
                                             current_tool_capabilities() {
 
      /* Register event handlers for some message types */
      add_handler(sip::request_tool_capabilities, boost::bind(&communicator::reply_tool_capabilities, this));
    }
 
    /**
     * The following connection options are recognised and extracted from the command line arguments:
     *
     *  - --si-connect=\<scheme\>, where \<scheme\> is one of
     *    - socket://\<host\>:\<port\> (for a socket connection)
     *    - traditional:// (for standard input/output communication)
     *
     * @param argc the number of command line arguments
     * @param argv a pointer to the list of command line arguments
     * \attention the specific command line options are removed, so and argc and argv are modified
     * \return whether options were found and whether a connection is being opened with a controller
     **/
    bool communicator::activate(int& argc, char** argv) {
      command_line_interface::argument_extractor e(argc, argv);
 
      command_line_interface::scheme_ptr scheme = e.get_scheme();
 
      if (scheme.get() != 0) {
        current_status = status_initialising;
 
        scheme->connect(this);
 
        instance_identifier = e.get_identifier();
 
        /* Identify the tool instance to the controller */
        sip::message m(sip::send_instance_identifier);
 
        std::ostringstream s;
 
        s << instance_identifier;
 
        m.set_content(s.str());

        send_message(m);
 
        current_status = status_clean;
      }
 
      return (scheme.get() != 0);
    }
 
    communicator::~communicator() {
    }
 
    /* Request details about the amount of space that the controller currently has reserved for this tool */
    void communicator::request_controller_capabilities() {
      message m(sip::request_controller_capabilities);
 
      send_message(m);
 
      /* Await the reply */
      do {
        message_ptr p = await_message(sip::reply_controller_capabilities);
 
        if (p.get() != 0) {
          xml2pp::text_reader reader(p->to_string().c_str());
       
          current_controller_capabilities = controller::capabilities::read(reader);

          break;
        }
      } while (1);
    }
 
    /* Send a specification of the tools capabilities */
    void communicator::reply_tool_capabilities() {
      message m(current_tool_capabilities.write(), sip::reply_tool_capabilities);
 
      send_message(m);
    }
 
    /* Send a specification of the current configuration (it may change during tool execution) */
    void communicator::send_accept_configuration() {
      message m(current_configuration->write(), sip::send_accept_configuration);
 
      send_message(m);
    }
 
    /**
     * @param[in] c the configuration object that specifies the accepted configuration
     **/
    void communicator::send_accept_configuration(const sip::configuration& c) {
      message m(c.write(), sip::send_accept_configuration);
 
      send_message(m);
    }
 
    /**
     * Send a layout specification for the display space reserved for this
     * tool. Also sets up an event handler to update the tool display with
     * data from change events, once those are are received.
     *
     * The last communicated display layout is stored internally and is updated
     * accordingly when data is received.
     **/
    void communicator::send_display_layout(layout::tool_display::sptr d) {
      message m(d->write(), sip::send_display_layout);

      send_message(m);

      clear_handlers(sip::send_display_data);

      add_handler(sip::send_display_data, boost::bind(&communicator::accept_display_data, this, _1, d));
    }

    void communicator::clear_display() {
      layout::tool_display display;

      clear_handlers(sip::send_display_data);

      message m(display.write(), sip::send_display_layout);

      send_message(m);
    }

    /* Send a signal that the tool is about to terminate */
    void communicator::send_signal_termination() {
      message m(sip::send_signal_termination);
 
      send_message(m);
    }
 
    /* Send a status report to the controller */
    void communicator::send_report(sip::report const& r) {
      message m(r.write(), sip::send_report);
 
      send_message(m);
    }

    /**
     * @param[in] e a description of the error
     **/
    void communicator::send_error_report(std::string const& e) {
      sip::report report;

      report.set_error(e);

      send_report(report);
    }
 
    /**
     * @param[in] m shared pointer to the message
     * @param[out] d tool display on which to execute changes
     **/
    void communicator::accept_display_data(const sip::messenger::message_ptr& m, layout::tool_display::sptr d) {
      xml2pp::text_reader reader(m->to_string().c_str());

      d->update(reader);
    }

    const configuration::sptr communicator::await_configuration() {
      const sip::messenger::message_ptr m = await_message(sip::send_configuration);

      return (sip::configuration::read(m->to_string()));
    }
  }
}

