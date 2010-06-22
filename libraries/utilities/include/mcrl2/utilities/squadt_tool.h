// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/squadt_tool.h
/// \brief Base class for tools that use a squadt interactor.

#ifndef MCRL2_UTILITIES_SQUADT_TOOL_H
#define MCRL2_UTILITIES_SQUADT_TOOL_H

# ifndef ENABLE_SQUADT_CONNECTIVITY
namespace mcrl2 {
  namespace utilities {
    namespace tools {

      /// \brief Dummy class for tools that can use a squadt interactor but squadt connectivity is not enabled.
      template < typename Tool >
      struct squadt_tool : public Tool {
        /// \brief Constructor.
        squadt_tool()
        {}

        /// \brief Constructor.
        squadt_tool(const std::string& name,
                      const std::string& author,
                      const std::string& what_is,
                      const std::string& tool_description,
                      std::string known_issues = ""
                     )
          : Tool(name, author, what_is, tool_description, known_issues)
        {}
      };
    } // namespace tools
  } // namespace utilities
} // namespace mcrl2
# else

#include "boost/bind.hpp"

// On OS X, a macro called `check' may have been imported
#undef check

# include "mcrl2/utilities/squadt_interface.h"
# include "mcrl2/utilities/input_output_tool.h"
# include "mcrl2/utilities/tipi_ext.h"
#ifdef __LIBLTS_H
#include "mcrl2/lts/lts_algorithm.h"
#endif

namespace mcrl2 {
  namespace utilities {
    namespace tools {

      namespace detail {
        struct dummy_tool {
        };
      }

      /// \cond INTERNAL_DOCS
      template < class Tool >
      void relay_message(const ::mcrl2::core::messageType, const char*);
      /// \endcond

      /**
       * \brief Base class for mCRL2 tools that work in a squadt context
       *
       * In addition to the functionality provided by the tool_interface class,
       * logging system used by most tools in the mCRL2 toolset is initialised
       * such that messages are relayed to sqaudt.
       *
       * \note only works for one object at a time due to the global nature of
       * the logging system
       **/
      template < typename Tool >
      class squadt_tool: public Tool,
                         public squadt::tool_interface
      {
        typedef squadt::tool_interface super;

        /// \brief relays messages from the mCRL2 messaging system
        template < class T >
        friend void relay_message(const ::mcrl2::core::messageType, const char* data);

        private:

          /* send status message, used only to relay messages */
          static squadt_tool*& get_reporter() {
            static squadt_tool* reporter;

            return reporter;
          }

          using super::send_report;

          void send_report(const tipi::report::type t, std::string const& d) {
            super::send_report(t, d);
          }

          static bool initialise_enumerated_type_conversions() {
#ifdef __LIBLTS_H
            using namespace mcrl2::lts;

            tipi::datatype::enumeration< mcrl2::lts::lts_equivalence > transformation_methods;

            std::set< mcrl2::lts::lts_equivalence > const& equivalences(mcrl2::lts::supported_lts_equivalences());

            for (std::set< mcrl2::lts::lts_equivalence >::const_iterator i = equivalences.begin(); i != equivalences.end(); ++i) {
              transformation_methods.add(*i, mcrl2::lts::string_for_equivalence(*i));
            }

            tipi::datatype::enumeration< mcrl2::lts::lts_type > storage_types;

            std::set< mcrl2::lts::lts_type > const& formats(mcrl2::lts::detail::supported_lts_formats());

            for (std::set< mcrl2::lts::lts_type >::const_iterator i = formats.begin(); i != formats.end(); ++i) {
              storage_types.add(*i, mcrl2::lts::detail::string_for_type(*i));
            }
#endif

            return true;
          }

          void initialise() {
            mcrl2::core::gsSetCustomMessageHandler(relay_message< squadt_tool >);

            static bool initialised = initialise_enumerated_type_conversions();

            static_cast< void >(initialised);

            get_reporter() = this;

            tipi::utility::logger::log_level l = tipi::utility::logger::get_default_filter_level();

            mcrl2::core::gsSetNormalMsg();

            if (2 < l) {
              mcrl2::core::gsSetVerboseMsg();

              if (3 < l) {
                mcrl2::core::gsSetDebugMsg();
              }
            }
          }

          /** \brief finalisation after termination signal has been received */
          void finalise() {
            /* Unregister message relay */
            mcrl2::core::gsSetCustomMessageHandler(0);
          }

        protected:

          template < typename Char >
          int initialise(int& argc, Char* argv[])
          {
            if (try_interaction(argc, argv))
            {
              return EXIT_SUCCESS;
            }

            return Tool::execute(argc, argv);
          }

        public:

          void update_configuration(tipi::configuration& c);

          void synchronise_with_configuration(tipi::configuration& c);

        public:

          /// \brief Constructor.
          squadt_tool()
          {}

          /// \brief Constructor.
          squadt_tool(const std::string& name,
                        const std::string& author,
                        const std::string& what_is,
                        const std::string& tool_description,
                        std::string known_issues = ""
                       )
            : Tool(name, author, what_is, tool_description, known_issues)
          {}

          template < typename Char >
          int execute(int& argc, Char* argv[])
          {
            try {
              if (try_interaction(argc, argv) && try_run()) {
                return EXIT_SUCCESS;
              }

              return Tool::execute(argc, argv);
            }
            catch (std::exception&)
            {
              return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
          }
      };

      /// \cond INTERNAL_DOCS
      /** \brief Used to relay messages generated using core::print */
      template < class Tool >
      void relay_message(const ::mcrl2::core::messageType t, const char* data) {
        tipi::report::type report_type;

        switch (t) {
          case mcrl2::core::gs_notice:
            report_type = tipi::report::notice;
            break;
          case mcrl2::core::gs_warning:
            report_type = tipi::report::warning;
            break;
          case mcrl2::core::gs_error:
          default:
            report_type = tipi::report::error;
            break;
        }

        Tool::get_reporter()->send_report(report_type, std::string(data));
      }
      /// \endcond

      template <>
      inline void squadt_tool< input_tool >::update_configuration(tipi::configuration&) {
      }

      template <>
      inline void squadt_tool< input_output_tool >::update_configuration(tipi::configuration& c) {
        input_output_tool::m_output_filename = c.get_output("main-output").location();
      }

      template <>
      inline void squadt_tool< input_tool >::synchronise_with_configuration(tipi::configuration& c) {
        input_tool::m_input_filename = c.get_input("main-input").location();
      }

      template <>
      inline void squadt_tool< input_output_tool >::synchronise_with_configuration(tipi::configuration& c) {
        if (c.output_exists("main-output")) {
          input_output_tool::m_output_filename = c.get_output("main-output").location();
        }

        input_output_tool::m_input_filename = c.get_input("main-input").location();
      }

    } // namespace tools
  } // namespace utilities
} // namespace mcrl2
#endif

#endif // MCRL2_UTILITIES_REWRITER_TOOL_H
