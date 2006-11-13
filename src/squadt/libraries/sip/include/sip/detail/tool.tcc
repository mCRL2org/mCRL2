#include <xml2pp/text_reader.h>

#include <sip/detail/common.h>
#include <sip/detail/basic_messenger.tcc>
#include <sip/tool.h>
#include <sip/layout_base.h>
#include <sip/detail/message.h>
#include <sip/detail/command_line_interface.tcc>

namespace sip {
  namespace tool {

    class communicator_impl : public sip::messaging::basic_messenger_impl< sip::message > {
      friend class communicator;
      friend class messaging::scheme< sip::message >;
 
      private:

        /** \brief The last received set of controller capabilities */
        controller::capabilities::ptr current_controller_capabilities;
 
        /** \brief The object that descibed the capabilities of the current tool */
        tool::capabilities            current_tool_capabilities;
 
        /** \brief This object reflects the current configuration */
        configuration::sptr           current_configuration;
 
        /** \brief Unique identifier for the running tool, obtained via the command line */
        long                          instance_identifier;
 
      private:

        /** \brief Send details about the amount of space that the controller currently has reserved for this tool */
        void request_tool_capabilities_handler();

        /** \brief Handler for incoming data resulting from user interaction with the display relayed by the controller */
        void receive_display_data_handler(const sip::messenger::message_ptr&, layout::tool_display::sptr);

        /** \brief Extract a configuration from an offer_configuration message */
        void receive_configuration_handler(const sip::messenger::message_ptr& m);

      public:
 
        /** \brief Constructor */
        communicator_impl();

        /** \brief Constructor that takes controller connection arguments from the command line */
        bool activate(int&, char** const);

        /** \brief Constructor that takes controller connection arguments from the command line */
        bool activate(char*&);

        /** \brief Attempts to build a connection using a scheme object */
        bool activate(command_line_interface::scheme_ptr const&, long const&);

        /** \brief Send a layout specification for the display space reserved for this tool */
        void send_display_layout(layout::tool_display::sptr);
 
        /** \brief Sends the empty layout specification for the display space */
        void send_clear_display();
    };

    /**
     * \attention please use connect() to manually establish a connection with a controller
     **/
    inline communicator_impl::communicator_impl() : current_tool_capabilities() {
 
      /* Register event handlers for some message types */
      add_handler(sip::message_request_tool_capabilities, boost::bind(&communicator_impl::request_tool_capabilities_handler, this));
      add_handler(sip::message_offer_configuration, boost::bind(&communicator_impl::receive_configuration_handler, this, _1));
    }
 
    /**
     * \param[in,out] argv a pointer to the list of command line arguments
     **/
    inline bool communicator_impl::activate(char*& argv) {
      command_line_interface::argument_extractor e(argv);
 
      return (activate(e.get_scheme(), e.get_identifier())); 
    }

    /**
     * \param[in,out] argc the number of command line arguments
     * \param[in,out] argv a pointer to the list of command line arguments
     **/
    inline bool communicator_impl::activate(int& argc, char** const argv) {
      command_line_interface::argument_extractor e(argc, argv);
 
      return (activate(e.get_scheme(), e.get_identifier())); 
    }

    /**
     * \param[in] s a scheme object that represents the method of connecting
     * \param[in] id an identifier used in the connection
     **/
    inline bool communicator_impl::activate(command_line_interface::scheme_ptr const& s, long const& id) {
      if (s.get() != 0) {
        s->connect(this);
 
        instance_identifier = id;
 
        /* Identify the tool instance to the controller */
        sip::message m(boost::str(boost::format("%u") % id), sip::message_instance_identification);
 
        send_message(m);
      }
 
      return (s.get() != 0);
    }

    inline void communicator_impl::send_display_layout(layout::tool_display::sptr d) {
      message m(d->write(), sip::message_display_layout);

      send_message(m);

      clear_handlers(sip::message_display_update);

      add_handler(sip::message_display_update, boost::bind(&communicator_impl::receive_display_data_handler, this, _1, d));
    }

    inline void communicator_impl::send_clear_display() {
      layout::tool_display display;

      clear_handlers(sip::message_display_update);

      message m(display.write(), sip::message_display_layout);

      send_message(m);
    }

    /* Send a specification of the tools capabilities */
    inline void communicator_impl::request_tool_capabilities_handler() {
      message m(current_tool_capabilities.write(), sip::message_response_tool_capabilities);
 
      send_message(m);
    }
 
    /**
     * @param[in] m shared pointer to the message
     * @param[out] d tool display on which to execute changes
     **/
    inline void communicator_impl::receive_display_data_handler(const sip::messenger::message_ptr& m, layout::tool_display::sptr d) {
      std::vector < sip::layout::element const* > elements;
      
      xml2pp::text_reader reader(m->to_string().c_str());

      d->update(reader, elements);
    }
    
    /**
     * @param[in] m shared pointer reference to an offer_configuration message
     **/
    inline void communicator_impl::receive_configuration_handler(const sip::messenger::message_ptr& m) {
      assert(m->get_type() == sip::message_offer_configuration);

      if (m.get() != 0) {
        current_configuration = sip::configuration::read(m->to_string());
      }
    }
  } 
}
