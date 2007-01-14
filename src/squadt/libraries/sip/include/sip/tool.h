#ifndef SIP_TOOL_H
#define SIP_TOOL_H

#include <set>

#include <utility/logger.h>

#include <sip/detail/layout_tool_display.h>
#include <sip/detail/layout_elements.h>
#include <sip/report.h>
#include <sip/tool/capabilities.h>
#include <sip/tool/category.h>

/* Interface classes for the tool side */
namespace sip {
  namespace controller {
    /** \brief Specifies capabilities */
    class capabilities;
  }

  namespace tool {

    class communicator_impl;

    /** \brief Specifies capabilities */
    class capabilities;

    /** \brief The main interface to the protocol implementation (tool-side) */
    class communicator : public sip::messenger {
      friend class layout::element;
      friend class communicator_impl;

      private:

        /** \brief Send data to update the state of the last communicated display layout */
        void send_display_data(layout::element const*);
 
        /** \brief Signal that the current configuration is complete enough for the tool to start processing */
        void send_accept_configuration(sip::configuration&);
 
        /** \brief Set the current tool configuration object */
        inline void set_configuration(boost::shared_ptr < configuration >);

      protected:
 
        /** \brief Alternate constructor */
        communicator(communicator_impl*);

      public:
 
        /** \brief Default constructor */
        communicator();
 
        /** \brief Activate the communicator via connection arguments from the command line */
        bool activate(int&, char** const);

        /** \brief Activate the communicator via connection arguments from a vector of arguments */
        bool activate(char*&);

        /** \brief Activate the communicator via connection arguments from a vector of arguments */
        bool activate(std::vector < std::string> const&);
 
        /** \brief Request details about the amount of space that the controller currently has reserved for this tool */
        void request_controller_capabilities();
 
        /** \brief Signal that the current configuration is complete enough for the tool to start processing */
        void send_accept_configuration();

        /** \brief Send a layout specification for the display space reserved for this tool */
        void send_display_layout(layout::tool_display::sptr);
 
        /** \brief Sends the empty layout specification for the display space */
        void send_clear_display();
 
        /** \brief Send a signal that the tool has finished its last operation */
        void send_signal_done(bool);
 
        /** \brief Send a signal that the tool is about to terminate */
        void send_signal_termination();
 
        /** \brief Send a status report to the controller */
        void send_report(sip::report const&) const;

        /** \brief Sends an error report to the controller */
        void send_status_report(sip::report::type, std::string const&) const;
 
        /** \brief Get the tool capabilities object that will be sent when a request is received */
        tool::capabilities& get_tool_capabilities();
 
        /** \brief Get the current tool configuration object be sent when a request is received */
        configuration& get_configuration();
 
        /** \brief Get the last communicated set of controller capabilities */
        const boost::shared_ptr < controller::capabilities > get_controller_capabilities() const;

        /** \brief Waits until a configuration is delivered and returns a pointer to it */
        void await_configuration() const;
    };
  }
}

#endif
