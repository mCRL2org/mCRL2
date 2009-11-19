// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/state_formulas/detail/algorithms.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_FORMULA_DETAIL_ALGORITHMS_H
#define MCRL2_MODAL_FORMULA_DETAIL_ALGORITHMS_H

#include <stdexcept>
#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "aterm2.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/regfrmtrans.h"
#include "mcrl2/data/detail/internal_format_conversion.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace state_formulas {

namespace detail {

  /// \brief Parse a state formula
  /// \param from An input stream
  /// \return A state formula in an undocumented format
  inline
  ATermAppl parse_state_formula(std::istream& from)
  {
    ATermAppl result = core::parse_state_frm(from);
    if (result == NULL)
      throw mcrl2::runtime_error("parse error in parse_state_frm()");
    return result;
  }

  /// \brief Type checks a state formula against spec
  /// \param formula A term
  /// \param spec A term
  /// a pbes specification before data implementation, or
  /// a data specification before data implementation.
  /// \return The type checked formula
  inline
  ATermAppl type_check_state_formula(ATermAppl formula, ATermAppl spec)
  {
    ATermAppl result = core::type_check_state_frm(formula, spec);
    if (result == NULL)
      throw mcrl2::runtime_error("type check error");
    return result;
  }

  /// \brief Converts a regular formula to a state formula
  /// \param formula A term
  /// \return The converted formula
  inline
  state_formula translate_regular_formula(ATermAppl formula)
  {
    ATermAppl result = core::translate_reg_frms(formula);
    if (result == NULL)
      throw mcrl2::runtime_error("formula translation error");
    return result;
  }

  /// \brief Parses a state formula from an input stream
  // spec may be updated as the data implementation of the state formula
  // may cause internal names to change.
  /// \param formula_stream A stream from which can be read
  /// \param spec A linear process specification
  /// \return The converted modal formula
  inline
  state_formula mcf2statefrm(std::istream& formula_stream, lps::specification& spec)
  { std::cerr << "SPEC ACTS " << spec.action_labels() << "\n";
    atermpp::aterm_appl f = parse_state_formula(formula_stream);
    lps::specification copy_spec = spec;
    /* copy_spec.data() = remove_all_system_defined(spec.data()); */
    atermpp::aterm_appl reconstructed_spec = specification_to_aterm(copy_spec,false);
    f = type_check_state_formula(f, reconstructed_spec);
    f = translate_regular_formula(f);
    f = data::detail::undo_compatibility_renamings(spec.data(), f);
    spec.data().make_complete(data::find_sort_expressions(f)); // Make complete with respect to f
    f = data::detail::internal_format_conversion(spec.data(), f);
    return f;
  }

  /// \brief Parses a state formula from text
  // spec may be updated as the data implementation of the state formula
  // may cause internal names to change.
  /// \param formula_text A string
  /// \param spec A linear process specification
  /// \return The converted modal formula
  inline
  state_formula mcf2statefrm(const std::string& formula_text, lps::specification& spec)
  {
    std::stringstream formula_stream(formula_text);
    return mcf2statefrm(formula_stream, spec);
  }

} // namespace detail

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_ALGORITHMS_H
