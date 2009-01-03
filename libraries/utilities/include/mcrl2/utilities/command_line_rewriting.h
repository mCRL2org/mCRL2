
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

    /// \cond INTERNAL_DOCS
    namespace detail {
      template <>
      struct initialiser< RewriteStrategy > {
        static void add_options(interface_description& standard) {
          standard.add_option(
            "rewriter", interface_description::mandatory_argument< RewriteStrategy >("NAME", "jitty"),
            "use rewrite strategy NAME:\n"
            "  'jitty' for jitty rewriting (default),\n"
            "  'jittyp' for jitty rewriting with prover,\n"
#ifdef MCRL2_JITTYC_AVAILABLE
            "  'jittyc' for compiled jitty rewriting,\n"
#endif
            "  'inner' for innermost rewriting,\n"
            "  'innerp' for innermost rewriting with prover, or\n"
#ifdef MCRL2_JITTYC_AVAILABLE
            "  'innerc' for compiled innermost rewriting"
#endif
            ,'r'
          );
        }
      };

      void register_rewriting_options(interface_description& d) {
        initialiser< RewriteStrategy >::add_options(d);
      }
    }
  /// \endcond
  }
}

