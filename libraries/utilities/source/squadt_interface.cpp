// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/squadt_interface.cpp
/// \brief Add your file description here.

#define NO_MCRL2_TOOL_FACILITIES
#include <tipi/tool.hpp>

#include "mcrl2/utilities/squadt_interface.h"
#include "tipi/utility/logger.hpp"

#if defined(__APPLE__)
# include <Carbon/Carbon.h>
#endif

using namespace mcrl2::utilities;

namespace mcrl2 {
  namespace utilities {
    namespace squadt {
      /// \cond INTERNAL_DOCS
      boost::function< void (const tipi::report::type, std::string const&) > mcrl2_tool_interface::do_send_report;
      /// \endcond

      tool_interface::tool_interface() : m_communicator(new tipi::tool::communicator) {
      }
  
      tool_interface::~tool_interface() {
        delete m_communicator;
      }

      bool tool_interface::try_run() {
        if (active) {
          bool valid_configuration_present = false;
          bool termination_requested       = false;
  
          initialise();
  
          try {
            while (!termination_requested) {
              switch (m_communicator->await_message(tipi::message_any)->get_type()) {
                case tipi::message_configuration: {
                    tipi::configuration& configuration = m_communicator->get_configuration();
           
                    /* Insert configuration in tool communicator object */
                    valid_configuration_present = check_configuration(configuration);
           
                    if (configuration.is_fresh()) {
                      do {
                        user_interactive_configuration(configuration);
           
                        /* Insert configuration in tool communicator object */
                        valid_configuration_present = check_configuration(configuration);
                   
                      } while (!valid_configuration_present);
                    }
           
                    /* Signal that the configuration is acceptable */
                    m_communicator->send_configuration(configuration);
                  }
                  break;
                case tipi::message_task_start:
                  if (valid_configuration_present) {
                    tipi::configuration& configuration = m_communicator->get_configuration();

                    /* Signal that the job is finished */
                    bool result = perform_task(configuration);
  
                    if (!result) {
                      send_error("Operation failed; tool may have crashed!");
                    }
                    else {
                      m_communicator->send_configuration(configuration);
                    }
  
                    m_communicator->send_signal_done(result);
                  }
                  else {
                    send_error("Start signal received without valid configuration!");
                  }
                  break;
                case tipi::message_termination:
           
                  termination_requested = true;
           
                  break;
                default:
                  /* Messages with a type that do not need to be handled */
                  break;
              }
            }
          }
          catch (std::exception& e) {
            /* Handle standard exceptions */
            send_error(std::string("Caught exception: ") + e.what());
          }
  
          finalise();

          m_communicator->send_signal_termination();

          m_communicator->disconnect();

          active = false;

          return true;
        }
  
        return false;
      }
  
      /**
       * \param[in] av command line arguments (Windows specific)
       **/
      bool tool_interface::try_interaction(char* av) {
        set_capabilities(m_communicator->get_capabilities());

        active = m_communicator->activate(av);

        return try_run();
      }
  
      /**
       * The connection is built using information such as a socket identifier when
       * found among the command line arguments. The command line arguments that
       * are SQuADT specific are filtered out.
       *
       * \param[in] ac the number of command line arguments
       * \param[in] av a pointer to an array of the actual command line arguments
       *
       * \return whether or not SQuADT interaction was successful
       **/
      bool tool_interface::try_interaction(int& ac, char** const av) {
  
        set_capabilities(m_communicator->get_capabilities());
  
        active = m_communicator->activate(ac,av);

#if defined (__APPLE__)
        // Apple Event handler for arguments
        if (!active) {
          static char* argv = 0;

          struct local {
            static OSErr get_arguments(const AppleEvent* message, AppleEvent*, long)  {
              AEDesc descriptor = { typeChar, 0 };

              if (AEGetParamDesc(message, keyDirectObject, typeChar, &descriptor) == noErr) {
                argv = new char[AEGetDescDataSize(&descriptor)];

                AEGetDescData(&descriptor, argv, AEGetDescDataSize(&descriptor));
              }

              AEDisposeDesc(&descriptor);

              return noErr;
            }
          };

          if (AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, &local::get_arguments, 0, FALSE) == noErr) {
            EventTypeSpec type_mask = { kEventClassAppleEvent, kEventAppleEvent };
            EventRef      initial_apple_event;

            if ((ReceiveNextEvent(1, &type_mask, 1.0, true, &initial_apple_event) == noErr)) {
              EventRecord initial_apple_event_record;

              if (ConvertEventRefToEventRecord(initial_apple_event, &initial_apple_event_record)) {
                if (AEProcessAppleEvent(&initial_apple_event_record) == noErr) {
                  if (argv) {
                    if (active = m_communicator->activate(argv)) {
                      return try_run();
                    }
                  }
                }
              }
            }

            AERemoveEventHandler(kCoreEventClass, kAEOpenApplication, 0, FALSE);
          }
        }
#endif
  
        return try_run();
      }
  
      bool tool_interface::is_active() const {
        return (active);
      }
  
      /**
       * \param[in] m descriptive message
       **/
      void tool_interface::send_notification(std::string const& m) const {
        m_communicator->send_status_report(tipi::report::notice, m);
      }
  
      /**
       * \param[in] m descriptive message
       **/
      void tool_interface::send_warning(std::string const& m) const {
        m_communicator->send_status_report(tipi::report::warning, m);
      }
  
      /**
       * \param[in] m descriptive message
       **/
      void tool_interface::send_error(std::string const& m) const {
        m_communicator->send_status_report(tipi::report::error, m);
      }

      /**
       * \param[in] m the type of the report
       * \param[in] d the data
       **/
      void tool_interface::send_report(const tipi::report::type m, std::string const& d) {
        m_communicator->send_status_report(m, d);
      }

      /// \cond INTERNAL_DOCS
      class trivial_deleter {

        public:

          void operator() (::tipi::layout::tool_display*) {
          }
      };
      /// \endcond
  
      /**
       * \param[in] d the display object that contains the data to communicate
       **/
      void tool_interface::send_display_layout(::tipi::layout::tool_display& d) {
        boost::shared_ptr < ::tipi::layout::tool_display > dsp(&d, trivial_deleter());

        m_communicator->send_display_layout(dsp);
      }
  
      void tool_interface::send_clear_display() {
        m_communicator->send_clear_display();
      }
  
      void tool_interface::send_hide_display() {
        boost::shared_ptr < tipi::layout::tool_display > p(new tipi::layout::tool_display());
  
        p->show(false);
  
        m_communicator->send_display_layout(p);
      }
  
      /// \cond INTERNAL_DOCS
      boost::shared_ptr < tipi::datatype::enumeration > rewrite_strategy_enumeration;
  
      static bool initialise() {
        rewrite_strategy_enumeration.reset(new tipi::datatype::enumeration("inner"));
        *rewrite_strategy_enumeration % "innerc" % "jitty" % "jittyc";

        return true;
      }
  
      bool initialised = initialise();
      /// \endcond
    }
  }
}
