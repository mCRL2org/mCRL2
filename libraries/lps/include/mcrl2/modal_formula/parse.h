// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/parse.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_PARSE_H
#define MCRL2_MODAL_FORMULA_PARSE_H

#include <iostream>
#include "mcrl2/core/parse.h"
#include "mcrl2/core/regfrmtrans.h"
#include "mcrl2/data/detail/internal_format_conversion.h"
#include "mcrl2/modal_formula/typecheck.h"
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2 {

namespace state_formulas {

  /// \brief Translates regular formulas appearing in f into action formulas.
  /// \param f A state formula
  inline
  void translate_regular_formula(state_formula& f)
  {
    ATermAppl result = core::translate_reg_frms(f);
    if (result == NULL)
    {
      throw mcrl2::runtime_error("formula translation error");
    }
    f = state_formula(result);
  }

  /// \brief Parses a state formula from an input stream
  // spec may be updated as the data implementation of the state formula
  // may cause internal names to change.
  /// \param formula_stream A stream from which can be read
  /// \param spec A linear process specification
  /// \return The converted modal formula
  inline
  state_formula parse_state_formula(std::istream& from, lps::specification& spec)
  {
    ATermAppl result = core::parse_state_frm(from);
    if (result == NULL)
      throw mcrl2::runtime_error("parse error in parse_state_frm()");
    state_formula f = result;   
    type_check(f, spec);
    translate_regular_formula(f);

    // TODO: make find functions for state formulas
    spec.data().add_context_sorts(data::find_sort_expressions(atermpp::aterm_appl(f)));
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
  state_formula parse_state_formula(const std::string& formula_text, lps::specification& spec)
  {
    std::stringstream formula_stream(formula_text);
    return parse_state_formula(formula_stream, spec);
  }

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_PARSE_H
