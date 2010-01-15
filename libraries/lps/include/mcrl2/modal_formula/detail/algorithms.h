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
#include "mcrl2/modal_formula/typecheck.h"
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

  /// \brief Converts a regular formula to a state formula
  /// \param formula A term
  /// \return The converted formula
  inline
  state_formula translate_regular_formula(const state_formula& formula)
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
  { 
    state_formula f = parse_state_formula(formula_stream);
    type_check(f, spec);
    f = translate_regular_formula(f);
    spec.data().add_context_sorts(data::find_sort_expressions(f)); // Make complete with respect to f
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
