
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/data/prover/bdd_path_eliminator.h"

namespace mcrl2 {
  namespace utilities {
    /// \cond INTERNAL_DOCS
    inline std::istream& operator>>(std::istream& is, SMT_Solver_Type& s) {
      char solver_type[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

      /// no standard conversion available function, so implement on-the-spot
      is.readsome(solver_type, 10);

      s = solver_type_cvc;

      if (std::strncmp(solver_type, "ario", 5) == 0) {
        s = solver_type_ario;
      }
      else if (std::strncmp(solver_type, "cvc", 3) == 0) {
        if (solver_type[3] != '\0') {
          is.setstate(std::ios_base::failbit);
        }
      }
      else {
        is.setstate(std::ios_base::failbit);
      }

      return is;
    }

    namespace detail {
      template <>
      struct initialiser< SMT_Solver_Type > {
        static void add_options(interface_description& standard) {
          standard.add_option("smt-solver", interface_description::mandatory_argument< SMT_Solver_Type >("SOLVER"),
            "use SOLVER to remove inconsistent paths from the internally used "
            "BDDs (by default, no path elimination is applied):\n"
            "  'ario' for the SMT solver Ario, or\n"
            "  'cvc' for the SMT solver CVC3",
            'z');
        }
      };

      void register_proving_options(interface_description& d) {
        initialiser< SMT_Solver_Type >::add_options(d);
      }
    }
    /// \endcond
  }
}

