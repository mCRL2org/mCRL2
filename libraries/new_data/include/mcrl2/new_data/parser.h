// Author(s): Wieger Wesselink, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/parser.h
/// \brief Parser for new_data specifications.

#ifndef MCRL2_NEW_DATA_PARSER_H
#define MCRL2_NEW_DATA_PARSER_H

#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "aterm2.h"
#include "boost/algorithm/string.hpp"
#include "mcrl2/exception.h"
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/alpha.h"
#include "mcrl2/core/regfrmtrans.h"
#include "mcrl2/new_data/data_specification.h"
#include "mcrl2/new_data/detail/data_specification_compatibility.h"
#include "mcrl2/new_data/detail/data_implementation.h"

namespace mcrl2 {

namespace new_data {

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
      throw mcrl2::runtime_error("new_data implementation error");
    return result;
  }
} // namespace detail
/// \endcond

  /// Parses a new_data specification.
  inline
  data_specification parse_data_specification(const std::string& text)
  {
    // TODO: This is only a temporary solution. A decent standalone parser needs
    // to be made for new_data specifications.

    // make a fake linear process
    std::stringstream lps_stream;
    lps_stream << text;
    lps_stream << "init delta;\n";

    ATermAppl result = new_data::detail::parse_specification(lps_stream);
    result           = new_data::detail::type_check_specification(result);
    result           = new_data::detail::alpha_reduce(result);

    return data_specification(atermpp::arg1(result));
  }

  /// \deprecated This function will be removed after decent testing of
  //              parse_data_specification has been performed.
  /// Parses a new_data specification and implements the new_data types.
  inline
  atermpp::aterm_appl parse_data_specification_and_implement(const std::string& text)
  {
    // TODO: This is only a temporary solution. A decent standalone parser needs
    // to be made for new_data specifications.

    // make a fake linear process
    std::stringstream lps_stream;
    lps_stream << text;
    lps_stream << "init delta;\n";

    ATermAppl result = new_data::detail::parse_specification(lps_stream);
    result           = new_data::detail::type_check_specification(result);
    result           = new_data::detail::alpha_reduce(result);
    result           = new_data::detail::deprecated_implement_data_specification(result);

    return result;
  }

  /// \brief Parses a single data expression.
  /// \param text A string
  /// \param var_decl A string
  /// with their types.<br>
  /// An example of this is:
  /// \code
  ///   m, n: Nat;
  ///   b: Bool;
  /// \endcode
  /// \param data_spec A string
  /// \return The parsed expression
  inline
  data_expression parse_data_expression(std::string text, std::string var_decl = "", std::string data_spec = "")
  {
    data_expression result;

    // make an equation of the form 'x == x'
    std::string s = "eqn (" + text + ") = (" + text + ");";
    if (!boost::trim_copy(var_decl).empty())
    {
      s = "var\n" + var_decl + "\n" + s;
    }
    s = data_spec + (data_spec.empty() ? "" : "\n") + s;

    try
    {
      data_specification data_spec(new_data::parse_data_specification(s));

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

  /// \brief Parses a data variable.
  /// \param var_decl A string
  /// \param data_spec A string
  /// \return The parsed variable
  inline
  variable parse_variable(std::string var_decl, std::string data_spec = "")
  {
    std::istringstream in(var_decl + ";");
    atermpp::term_list< data_expression > v(core::parse_data_vars(in));
    assert(v.size() == 1);
    v = core::type_check_data_vars(v,
           detail::data_specification_to_aterm_data_spec(parse_data_specification(data_spec)));
    variable_list w(v.begin(), v.end());
    return w.front();
  }

  /// \brief Creates a data specification that contains rewrite rules for the standard data types like
  /// Pos, Nat and Int.
  /// \return The created data specification
  inline
  data_specification default_data_specification()
  {
    // Add dummy variables for standard types, to make sure that
    // rewrite rules are created for them.
    return parse_data_specification(
      "map dummy1:Pos;  \n"
      "var dummy2:Bool; \n"
      "    dummy3:Pos;  \n"
      "    dummy4:Nat;  \n"
      "    dummy5:Int;  \n"
      "    dummy6:Real; \n"
      "eqn dummy1 = 1;  \n"
    );
  }

} // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_PARSER_H
