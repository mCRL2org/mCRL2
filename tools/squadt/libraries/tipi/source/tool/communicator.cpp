// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/shared_array.hpp>

#include "tipi/tool/category.hpp"
#include "tipi/controller/capabilities.hpp"
#include "tipi/detail/tool.ipp"
#include "tipi/detail/event_handlers.hpp"

namespace tipi {
  namespace tool {

    communicator::communicator() :
        tipi::messenger(boost::shared_ptr < tipi::messaging::basic_messenger_impl < tipi::message > >(new communicator_impl)) {
    }

    communicator::communicator(boost::shared_ptr < tipi::tool::communicator_impl > const& c) :
        tipi::messenger(boost::static_pointer_cast < tipi::messaging::basic_messenger_impl < tipi::message > > (c)) {
    }

    /**
     * \param[in,out] argc the number of command line arguments
     * \param[in,out] argv a pointer to the list of command line arguments
     * \attention the specific command line options are removed, so and argc and argv are modified
     * \return whether options were found and whether a connection is being opened with a controller
     **/
    bool communicator::activate(int& argc, char** const argv) {
      return (boost::static_pointer_cast < communicator_impl > (impl)->activate(this, argc, argv));
    }

    bool communicator::activate(char*& argv) {
      return (boost::static_pointer_cast < communicator_impl > (impl)->activate(this, argv));
    }

    bool communicator::activate(std::vector < std::string > const& arguments) {
      int                        argc = arguments.size();
      boost::shared_array<char*> argv(new char*[argc]);

      for (int i = 0; i != argc; ++i) {
        argv[i] = const_cast < char* > (arguments[i].c_str());
      }
      
      return (activate(argc, argv.get()));
    }

    /**
     * \return a pointer to the tool capabilities object that is sent to the controller on request
     **/
    tool::capabilities& communicator::get_capabilities() {
      return (boost::static_pointer_cast < communicator_impl > (impl)->current_tool_capabilities);
    }
 
    /**
     * This object can be stored by the controller and subsequently be used to
     * restore this exact configuration state at the side of the tool.
     *
     * \return a reference to the current tool configuration object
     **/
    configuration& communicator::get_configuration() {
      return (*boost::static_pointer_cast < communicator_impl > (impl)->current_configuration);
    }
 
    void communicator::set_configuration(boost::shared_ptr < configuration > c) {
      boost::static_pointer_cast < communicator_impl > (impl)->current_configuration = c;
    }
 
    /**
     * \return p which is a pointer to the most recently retrieved controller capabilities object or 0
     **/
    const boost::shared_ptr < controller::capabilities > communicator::get_controller_capabilities() const {
      if (boost::static_pointer_cast < communicator_impl > (impl)->current_controller_capabilities.get() == 0) {
        throw std::runtime_error("Controller capabilities are unknown");
      }
 
      return (boost::static_pointer_cast < communicator_impl > (impl)->current_controller_capabilities);
    }
 
    /* Request details about the amount of space that the controller currently has reserved for this tool */
    void communicator::request_controller_capabilities() {
      message m(tipi::message_capabilities);
 
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(m);
 
      /* Await the reply */
      do {
        boost::shared_ptr< const message > p(await_message(tipi::message_capabilities));
 
        if (p.get() != 0) {
          boost::shared_ptr < controller::capabilities > n(new controller::capabilities);

          try {
            tipi::visitors::restore(*n, p->to_string());
          
            boost::static_pointer_cast < communicator_impl > (impl)->current_controller_capabilities = n;
          }
          catch (std::runtime_error& e) {
            get_logger().log(1, "Failure with interpretation of message: `" + std::string(e.what()) + "'\n");
          }

          break;
        }
      } while (1);
    }
 
    /* Send a specification of the current configuration (it may change during tool execution) */
    void communicator::send_configuration() {
      boost::shared_ptr < configuration > c(boost::static_pointer_cast < communicator_impl > (impl)->current_configuration);

      assert(c);

      if (c) {
        c->set_fresh(false);

        message m(tipi::visitors::store(*c), tipi::message_configuration);

        boost::static_pointer_cast < communicator_impl > (impl)->send_message(m);
      }
    }
 
    /**
     * \param[in] c the configuration object that specifies the accepted configuration
     **/
    void communicator::send_configuration(tipi::configuration& c) {
      c.set_fresh(false);

      message m(tipi::visitors::store(c), tipi::message_configuration);
 
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(m);
    }
 
    /**
     * Send a layout specification for the display space reserved for this
     * tool. Also sets up an event handler to update the tool display with
     * data from change events, once those are are received.
     *
     * The last communicated display layout is stored internally and is updated
     * accordingly when data is received.
     **/
    void communicator::send_display_layout(boost::shared_ptr< layout::tool_display > d) {
      struct trampoline {
        inline static void send_display_data(boost::shared_ptr< communicator_impl > impl, boost::weak_ptr< layout::tool_display > d, void const* e) {
          boost::shared_ptr < layout::tool_display > g(d.lock());

          if (!d.expired()) {
            std::string c;
           
            if (dynamic_cast < tipi::layout::element const* > (reinterpret_cast < tipi::layout::element const* > (e))) {
              tipi::store_visitor v(c);
           
              v.visit(*reinterpret_cast < tipi::layout::element const* > (e), reinterpret_cast < ::tipi::display::element_identifier > (e));
            }
           
            boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message(c, tipi::message_display_data));
          }
        }
      };

      d->add(boost::bind(trampoline::send_display_data, boost::static_pointer_cast < communicator_impl > (impl), d, _1));

      boost::static_pointer_cast < communicator_impl > (impl)->send_display_layout(d);
    }

    void communicator::send_clear_display() {
      boost::static_pointer_cast < communicator_impl > (impl)->send_clear_display();
    }

    /* Send a signal that the tool is about to terminate */
    void communicator::send_task_done(bool b) {
      message m((b) ? "success" : "", tipi::message_task);
 
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(m);
    }

    /* Send a signal that the tool is about to terminate */
    void communicator::send_signal_termination() {
      message m(tipi::message_termination);
 
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(m);
    }
 
    /* Send a status report to the controller */
    void communicator::send_report(tipi::report const& r) const {
      message m(tipi::visitors::store(r), tipi::message_report);
 
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(m);
    }

    /**
     * \param[in] t the report type
     * \param[in] e a description
     **/
    void communicator::send_status_report(tipi::report::type t, std::string const& e) const {
      tipi::report report(t, e);

      send_report(report);
    }

    /**
     * \param[in] e pointer to a tipi layout element of which the data is to be sent
     **/
    void communicator::send_display_data(layout::element const* e) {
      std::string        c;

      if (dynamic_cast < tipi::layout::element const* > (reinterpret_cast < tipi::layout::element const* > (e))) {
        tipi::store_visitor v(c);

        v.visit(*e,reinterpret_cast < ::tipi::display::element_identifier> (e));
      }

      boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message(c, tipi::message_display_data));
    }

    void communicator::await_configuration() const {
      // wait for configuration offer
      boost::static_pointer_cast < communicator_impl > (impl)->await_message(tipi::message_configuration);
    }
  }
}

