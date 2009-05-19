#include "boost.hpp" // precompiled headers

#include "mcrl2/core/messaging.h" //workaround
#include "data_specification.cpp"
#include "standard.cpp"
#include "detail/enumerate/enum_standard.cpp"

#undef ATisList
#undef ATisAppl
#undef ATisInt

#undef ATAgetFirst
#undef ATLgetFirst
#undef ATAgetArgument
#undef ATLgetArgument

#undef MAX_VARS_INIT
#undef MAX_VARS_FACTOR

#undef fs_bottom
#undef fs_top

#undef fs_filled
#undef ss_filled

#include "detail/prover/bdd2dot.cpp"
#include "detail/prover/bdd_path_eliminator.cpp"
#include "detail/prover/bdd_prover.cpp"
#include "detail/prover/bdd_simplifier.cpp"
#include "detail/prover/formula_checker.cpp"
#include "detail/prover/induction.cpp"
#include "detail/prover/info.cpp"
#include "detail/prover/manipulator.cpp"
#include "detail/prover/prover.cpp"
#include "detail/prover/smt_lib_solver.cpp"
