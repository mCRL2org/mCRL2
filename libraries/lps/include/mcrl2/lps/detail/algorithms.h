// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithms.h
/// \brief Add your file description here.

#ifndef LPS_DETAIL_LPS2PBES
#define LPS_DETAIL_LPS2PBES

#include <stdexcept>
#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "aterm2.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/alpha.h"
#include "mcrl2/core/regfrmtrans.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/lps/lin_types.h"
#include "mcrl2/lps/lin_std.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Reads a specification from an input stream
  /// \param from An input stream
  /// \return A term in an undocumented format
  inline
  ATermAppl parse_specification(std::istream& from)
  {
    ATermAppl result = core::parse_proc_spec(from);
    if (result == NULL)
      throw mcrl2::runtime_error("parse error");
    return result;
  }

  /// \brief Type checks a specification
  /// \param spec A term
  /// \return A term in an undocumented format
  inline
  ATermAppl type_check_specification(ATermAppl spec)
  {
    ATermAppl result = core::type_check_proc_spec(spec);
    if (result == NULL)
      throw mcrl2::runtime_error("type check error");
    return result;
  }

  /// \brief Applies alpha reduction to a specification
  /// \param spec A term
  /// \return A term in an undocumented format
  inline
  ATermAppl alpha_reduce(ATermAppl spec)
  {
    ATermAppl result = core::gsAlpha(spec);
    if (result == NULL)
      throw mcrl2::runtime_error("alpha reduction error");
    return result;
  }

  /// \brief Applies linearization to a specification
  /// \param spec A term
  /// \param options Options for the algorithm
  /// \return The linearized specification
  inline
  specification linearise(ATermAppl spec, t_lin_options options)
  {
    return linearise_std(spec, options);
  }

  /// \brief Reads a state formula from an input stream
  /// \param from An input stream
  /// \return A term in an undocumented format
  inline
  ATermAppl parse_state_formula(std::istream& from)
  {
    ATermAppl result = core::parse_state_frm(from);
    if (result == NULL)
      throw mcrl2::runtime_error("parse error in parse_state_frm()");
    return result;
  }

  /// \brief Type checks state formula
  /// \pre spec is a {lps specification, pbes specification, data specification}
  /// \param formula A term
  /// \param spec A term
  /// \return A term in an undocumented format
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
  /// \return The converted regular formula
  inline
  modal::state_formula translate_regular_formula(ATermAppl formula)
  {
    ATermAppl result = core::translate_reg_frms(formula);
    if (result == NULL)
      throw mcrl2::runtime_error("formula translation error");
    return result;
  }

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // LPS_DETAIL_LPS2PBES
