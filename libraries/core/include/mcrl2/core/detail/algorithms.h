// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/algorithms.h
/// \brief Add your file description here.

#ifndef MCRL2_CORE_DETAIL_ALGORITHMS_H
#define MCRL2_CORE_DETAIL_ALGORITHMS_H

#include <stdexcept>
#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "aterm2.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/regfrmtrans.h"
#include "mcrl2/exception.h"

namespace mcrl2 {

namespace core {

namespace detail {

  /// \brief     Parses a process specification.
  /// \param[in] ps_stream An input stream from which can be read.
  /// \post      The content of ps_stream is parsed as an mCRL2 process
  ///            specification.
  /// \return    The parsed mCRL2 process specification in the internal ATerm
  ///            format after parsing (before type checking).
  /// \exception mcrl2::runtime_error Parsing failed.
  inline
  ATermAppl parse_process_specification(std::istream& ps_stream)
  {
    ATermAppl result = core::parse_proc_spec(ps_stream);
    if (result == NULL)
      throw mcrl2::runtime_error("parse error");
    return result;
  }

  /// \brief     Type checks a process specification.
  /// \param[in] spec An mCRL2 process specification in the internal ATerm
  ///            format after parsing (before type checking).
  /// \post      proc_spec is type checked.
  /// \return    The type checked mCRL2 process specification in the
  ///            internal ATerm format after type checking.
  /// \exception mcrl2::runtime_error Type checking failed.
  inline
  ATermAppl type_check_process_specification(ATermAppl proc_spec)
  {
    ATermAppl result = core::type_check_proc_spec(proc_spec);
    if (result == NULL)
      throw mcrl2::runtime_error("type check error when checking process specification");
    return result;
  }

  /// \brief     Parses a state formula.
  /// \param[in] sf_stream An input stream from which can be read.
  /// \post      The content of sf_stream is parsed as an mCRL2 state formula.
  /// \return    The parsed mCRL2 state formula in the internal ATerm format
  ///            after parsing (before type checking).
  /// \exception mcrl2::runtime_error Parsing failed.
  inline
  ATermAppl parse_state_formula(std::istream& sf_stream)
  {
    ATermAppl result = core::parse_state_frm(sf_stream);
    if (result == NULL)
      throw mcrl2::runtime_error("parse error in parse_state_frm()");
    return result;
  }

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_ALGORITHMS_H
