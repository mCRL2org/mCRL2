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
#include "mcrl2/core/aterm_ext.h"

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

  /// \brief Parses a data specification.
  /// \param text A string
  /// \return A data specification
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

  /// \brief Parses a single data expression.
  /// \param[in] text The text that is parsed.
  /// \param[in] var_decl An optional declaration of data variables
  /// with their types.<br>
  /// An example of this is:
  /// \code
  ///   m, n: Nat;
  ///   b: Bool;
  /// \endcode
  /// \param[in] data_spec An optional data specification containing user defined types
  /// \return The parsed expression
  inline
  data_expression parse_data_expression(std::string text, std::string var_decl = "", std::string data_spec = "")
  {
    using namespace utilities;

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

  /// \brief Parses a data variable.
  /// \param[in] var_decl A declaration of a data variable, for example "n: Nat".
  /// \param[in] data_spec A data specification
  /// \return The parsed variable
  inline
  data_variable parse_data_variable(std::string var_decl, std::string data_spec = "")
  {
    std::istringstream in(var_decl + ";");
    atermpp::aterm_list v = core::parse_data_vars(in);
    assert(v.size() == 1);
    data_variable_list w = core::type_check_data_vars(v, parse_data_specification(data_spec));
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

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_PARSER_H
