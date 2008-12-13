#ifndef __MCRL2_COMMAND_LINE_REWRITING_HPP_
#define __MCRL2_COMMAND_LINE_REWRITING_HPP_

#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/core/messaging.h"

namespace mcrl2 {
  namespace utilities {
    namespace detail {
      template < >
      struct initialiser< mcrl2::core::messageType > {
        static bool actions(command_line_parser& parser) {
          if (parser.options.count("quiet")) {
            if (parser.options.count("debug")) {
              parser.error("options -q/--quiet and -d/--debug cannot be used together\n");
            }
            if (parser.options.count("verbose")) {
              parser.error("options -q/--quiet and -v/--verbose cannot be used together\n");
            }

            mcrl2::core::gsSetQuietMsg();
          }
          if (parser.options.count("verbose")) {
            mcrl2::core::gsSetVerboseMsg();
          }
          if (parser.options.count("debug")) {
            mcrl2::core::gsSetDebugMsg();
          }

          return true;
        }

        static bool register_messaging_actions() {
          command_line_parser::register_action(actions);

          return true;
        }

        static bool initialised;
      };

      bool initialiser< mcrl2::core::messageType >::initialised = register_messaging_actions();
    }
  }
}

#endif

