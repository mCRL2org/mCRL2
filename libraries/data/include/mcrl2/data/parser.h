// Author(s): Wieger Wesselink
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
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include "aterm2.h"
#include "mcrl2/exception.h"
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/data_implementation.h"
#include "mcrl2/core/alpha.h"
#include "mcrl2/core/regfrmtrans.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/utilities/aterm_ext.h"

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
  
  inline
  ATermAppl implement_process_specification(ATermAppl spec)
  {
    ATermAppl result = core::implement_data_proc_spec(spec);
    if (result == NULL)
      throw mcrl2::runtime_error("process data implementation error");
    return result;
  }

  inline
  ATermAppl parse_data_specification(std::istream& from)
  {
    ATermAppl result = core::parse_data_spec(from);
    if (result == NULL)
      throw mcrl2::runtime_error("parse error");
    return result;
  }
  
  inline
  ATermAppl type_check_data_specification(ATermAppl spec)
  {
    ATermAppl result = core::type_check_data_spec(spec);
    if (result == NULL)
      throw mcrl2::runtime_error("type check error");
    return result;
  }

  inline
  ATermAppl implement_data_specification(ATermAppl spec)
  {
    ATermAppl result = core::implement_data_data_spec(spec);
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
    result           = data::detail::implement_process_specification(result);
   
    atermpp::aterm_appl lps(result);
    return data_specification(lps(0));
  }

  /// Parses a single data expression.
  /// \param[in] text The text that is parsed.
  /// \param[in] var_decl An optional declaration of data variables
  /// with their types.<br>
  /// An example of this is:
  /// \code
  ///   m, n: Nat;
  ///   b: Bool;
  /// \endcode
  /// \result the parsed expression
  inline
  data_expression parse_data_expression(std::string text, std::string var_decl = "")
  {
    using namespace utilities;

    data_expression result;

    // make an equation of the form 'x == x'
    std::string s = "eqn (" + text + ") = (" + text + ");";
    if (!boost::trim_copy(var_decl).empty())
    {
      s = "var\n" + var_decl + s;
    }

    try
    {
      std::istringstream in(s);

      ATermAppl e = data::detail::parse_data_specification(in);
      e = data::detail::type_check_data_specification(e);
      e = data::detail::implement_data_specification(e);
      data_specification data_spec(e);

      // extract the left hand side of the equation 'x == x'
      std::vector<data_equation> eqn(data_spec.equations().begin(), data_spec.equations().end());
      result = eqn.back().lhs();
    }
    catch (std::runtime_error e)
    {
      std::cout << "<specification>" << s << std::endl;
      std::cout << e.what() << std::endl;
    }
    return result;
  }

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_PARSER_H
