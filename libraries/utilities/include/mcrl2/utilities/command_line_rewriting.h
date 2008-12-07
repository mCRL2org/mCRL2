
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/data/rewrite.h"

namespace mcrl2 {
  namespace utilities {
    inline std::istream& operator>>(std::istream& is, RewriteStrategy& s) {
      char strategy[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

      is.readsome(strategy, 9);

      s = RewriteStrategyFromString(strategy);

      if (s == GS_REWR_INVALID) {
        is.setstate(std::ios_base::failbit);
      }

      return is;
    }

    namespace detail {
      template <>
      struct initialiser< RewriteStrategy > {
        static void add_options(interface_description& standard) {
          standard.add_option(
            "rewriter", interface_description::mandatory_argument< RewriteStrategy >("NAME", "jitty"),
            "use rewrite strategy NAME:\n"
            "  'jitty' for jitty rewriting (default),\n"
            "  'jittyp' for jitty rewriting with prover,\n"
            "  'jittyc' for compiled jitty rewriting,\n"
            "  'inner' for innermost rewriting,\n"
            "  'innerp' for innermost rewriting with prover, or\n"
            "  'innerc' for compiled innermost rewriting",
            'r'
          );
        }

        static bool register_rewriting_actions() {
          interface_description::register_initialiser<
                                    initialiser< RewriteStrategy > >();

          return true;
        }
      };

      void register_rewriting_options() {
        static bool initialised = initialiser< RewriteStrategy >::register_rewriting_actions();

        static_cast< void >(initialised);
      };
    }
  }
}

