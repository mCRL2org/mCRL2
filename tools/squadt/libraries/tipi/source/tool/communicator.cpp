// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost.hpp" // precompiled headers

#include <boost/shared_array.hpp>

#include "tipi/detail/tool.ipp"
#include "tipi/detail/event_handlers.hpp"
#include "tipi/tool/category.hpp"
#include "tipi/controller/capabilities.hpp"
#include "tipi/display.hpp"

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

    /**
     * \param[in] argv a pointer to an unparsed command line
     * \return whether options were found and whether a connection is being opened with a controller
     **/
    bool communicator::activate(char*& argv) {
      return (boost::static_pointer_cast < communicator_impl > (impl)->activate(this, argv));
    }

    /**
     * \param[in] arguments a vector with strings representing command line options
     * \return whether options were found and whether a connection is being opened with a controller
     **/
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
      } while (true);
    }

    /* Send a specification of the current configuration (it may change during tool execution) */
    void communicator::send_configuration() {
      boost::shared_ptr < configuration > c(boost::static_pointer_cast < communicator_impl > (impl)->current_configuration);

      if(!c){
        throw std::runtime_error( "No specification of the current configuration\n" );
      }

      if (c) {
        c->fresh(false);

        message m(tipi::visitors::store(*c), tipi::message_configuration);

        boost::static_pointer_cast < communicator_impl > (impl)->send_message(m);
      }
    }

    /**
     * \param[in] c the configuration object that specifies the accepted configuration
     **/
    void communicator::send_configuration(tipi::configuration& c) {
      c.fresh(false);

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
    void communicator::send_display_layout(tool_display& d) {
      boost::static_pointer_cast < communicator_impl > (impl)->send_display_layout(
                boost::static_pointer_cast< communicator_impl >(impl), d);
    }

    /// \cond INTERNAL
    void communicator_impl::send_display_layout(boost::shared_ptr< communicator_impl > p, tool_display& d) {
      struct trampoline {
        static void receive_data(boost::shared_ptr< const tipi::message >& m, boost::weak_ptr < display > d) {
          boost::shared_ptr< display > g(d.lock());

          if (!d.expired()) {
            try {
              std::vector < tipi::layout::element const* > elements;

              if (g->impl->get_manager()) {
                tipi::visitors::restore(*g, elements, m->to_string());
              }
            }
            catch (...) {
            }
          }
        }

        static void send_display_data(boost::weak_ptr< communicator_impl > impl,
                         tipi::utility::logger& logger,
                         boost::shared_ptr< display > d,
                         void const* e) {

          boost::shared_ptr< communicator_impl > c(impl.lock());

          if (!impl.expired()) {
            try {
              if (dynamic_cast < tipi::layout::element const* > (reinterpret_cast < tipi::layout::element const* > (e))) {
                c->send_message(tipi::message(
                      visitors::store< tipi::layout::element const, const tipi::display::element_identifier >
		           (*reinterpret_cast < tipi::layout::element const* > (e),
                           reinterpret_cast < ::tipi::display::element_identifier > (e)), tipi::message_display_data));

              }
            }
            catch (std::exception& e) {
              logger.log(1, "Failure sending display data (" + std::string(e.what()) + ")\n");
            }
          }
        }
      };

      clear_handlers(tipi::message_display_data);

      current_display.reset(new tipi::tool_display(d));

      // handler for outgoing changes to elements on the display
      current_display->impl->add(boost::bind(trampoline::send_display_data, p, boost::ref(*logger), current_display, _1));

      // handler for incoming data from user interaction
      add_handler(tipi::message_display_data, boost::bind(&trampoline::receive_data, _1, current_display));

      send_message(tipi::message(tipi::visitors::store(d), tipi::message_display_layout));
    }
    /// \endcond

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
      if (dynamic_cast < tipi::layout::element const* > (reinterpret_cast < tipi::layout::element const* > (e))) {
        try {
          boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message(
              visitors::store< tipi::layout::element const, const tipi::display::element_identifier >
                    (*e, reinterpret_cast < ::tipi::display::element_identifier > (e)), tipi::message_display_data));
        }
        catch (std::exception& e) {
          get_logger().log(1, "Failure sending display data (" + std::string(e.what()) + ")\n");
        }
      }
    }

    void communicator::await_configuration() const {
      // wait for configuration offer
      boost::static_pointer_cast < communicator_impl > (impl)->await_message(tipi::message_configuration);
    }
  }
}

