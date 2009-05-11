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

# include "mcrl2/utilities/mcrl2_squadt_interface.h"
# include "mcrl2/utilities/input_output_tool.h"
# include "mcrl2/utilities/tipi_ext.h"

namespace mcrl2 {
  namespace utilities {
    namespace tools {

      namespace detail {
        struct dummy_tool {
        };
      }

      /// \brief Base class for tools that use a squadt interactor
      template < typename Tool >
      class squadt_tool: public Tool, public mcrl2::utilities::squadt::mcrl2_tool_interface {

        public:

          void update_configuration(tipi::configuration& c);

          void synchronise_with_configuration(tipi::configuration& c);

        public:

          /// \brief Constructor.
          squadt_tool(const std::string& name,
                        const std::string& author,
                        const std::string& what_is,
                        const std::string& tool_description,
                        std::string known_issues = ""
                       )
            : Tool(name, author, what_is, tool_description)
          {}

          int execute(int argc, char** argv)
          {
            try {
              if (mcrl2::utilities::squadt::free_activation(*this, argc, argv)) {
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

      template <>
      void squadt_tool< input_tool >::update_configuration(tipi::configuration&) {
      }

      template <>
      void squadt_tool< input_output_tool >::update_configuration(tipi::configuration& c) {
        input_output_tool::m_output_filename = c.get_output("main-output").location();
      }

      template <>
      void squadt_tool< input_tool >::synchronise_with_configuration(tipi::configuration& c) {
        input_tool::m_input_filename = c.get_input("main-input").location();
      }

      template <>
      void squadt_tool< input_output_tool >::synchronise_with_configuration(tipi::configuration& c) {
        if (!c.output_exists("main-output")) {
          c.add_output("main-output",
              tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
        }
        else {
          input_output_tool::m_output_filename = c.get_output("main-output").location();
        }

        input_output_tool::m_input_filename = c.get_input("main-input").location();
      }

    } // namespace tools
  } // namespace utilities
} // namespace mcrl2
#endif

#endif // MCRL2_UTILITIES_REWRITER_TOOL_H
