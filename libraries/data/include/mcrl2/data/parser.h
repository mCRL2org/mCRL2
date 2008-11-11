// Author(s): Wieger Wesselink, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/parser.h
/// \brief Parser for data specifications.

#ifndef MCRL2_DATA_PARSER_H
#define MCRL2_DATA_PARSER_H

#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "aterm2.h"
#include "mcrl2/exception.h"
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/alpha.h"
#include "mcrl2/core/regfrmtrans.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/data_implementation.h"

namespace mcrl2 {

namespace data {

/// \cond INTERNAL_DOCS
namespace detail {
  inline
  ATermAppl parse_specification(std::istream& from)
  {
    ATermAppl result = core::parse_proc_spec(from);
    if (result == NULL)
      throw mcrl2::runtime_error("parse error");
    return result;
  }
  
  inline
  ATermAppl type_check_specification(ATermAppl spec)
  {
    ATermAppl result = core::type_check_proc_spec(spec);
    if (result == NULL)
      throw mcrl2::runtime_error("type check error");
    return result;
  }
  
  inline
  ATermAppl alpha_reduce(ATermAppl spec)
  {
    ATermAppl result = core::gsAlpha(spec);
    if (result == NULL)
      throw mcrl2::runtime_error("alpha reduction error");
    return result;
  }

  // \deprecated
  inline
  ATermAppl deprecated_implement_data_specification(ATermAppl spec)
  {
    ATermAppl result = implement_data_proc_spec(spec);
    if (result == NULL)
      throw mcrl2::runtime_error("data implementation error");
    return result;
  }
} // namespace detail
/// \endcond

  /// Parses a data specification.
  inline
  data_specification parse_data_specification(const std::string& text)
  {
    // TODO: This is only a temporary solution. A decent standalone parser needs
    // to be made for data specifications.

    // make a fake linear process
    std::stringstream lps_stream;
    lps_stream << text;
    lps_stream << "init delta;\n";

    ATermAppl result = data::detail::parse_specification(lps_stream);
    result           = data::detail::type_check_specification(result);
    result           = data::detail::alpha_reduce(result);
   
    return data_specification(atermpp::arg1(result));
  }

  /// \deprecated This function will be removed after decent testing of
  //              parse_data_specification has been performed.
  /// Parses a data specification and implements the data types.
  inline
  atermpp::aterm_appl parse_data_specification_and_implement(const std::string& text)
  {
    // TODO: This is only a temporary solution. A decent standalone parser needs
    // to be made for data specifications.

    // make a fake linear process
    std::stringstream lps_stream;
    lps_stream << text;
    lps_stream << "init delta;\n";

    ATermAppl result = data::detail::parse_specification(lps_stream);
    result           = data::detail::type_check_specification(result);
    result           = data::detail::alpha_reduce(result);
    result           = data::detail::deprecated_implement_data_specification(result);
   
    return atermpp::arg1(result);
  }

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_PARSER_H
