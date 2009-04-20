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
#include "mcrl2/new_data/utility.h"
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

  /// \brief Parses a new_data specification.
  /// \param[in] text a textual description of the data specification
  /// \return the data specification corresponding to text.
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
  /// \brief Parses a new_data specification and implements the new_data types.
  /// \param[in] text a textual description of the data specification.
  /// \return the implemented data specfication corresponding to text.
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

    return atermpp::arg1(result);
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
  variable parse_variable(std::string const& var_decl, std::string const& data_spec = "")
  {
    std::istringstream in(var_decl + ";");
    atermpp::term_list< data_expression > v(core::parse_data_vars(in));
    assert(v.size() == 1);
    v = core::type_check_data_vars(v,
           detail::data_specification_to_aterm_data_spec(remove_all_system_defined(parse_data_specification(data_spec))));
    return atermpp::front(v);
  }

  /// \cond INTERNAL_DOCS
  namespace detail {
    /// \brief Parses a data variable
    /// For example: "X(d:D,e:E)".
    /// \param s A string
    /// \return The parsed data variable
    inline
    std::pair<std::string, data_expression_list> parse_variable(std::string const& s)
    {
      using boost::algorithm::split;
      using boost::algorithm::is_any_of;

      std::string name;
      data_expression_vector variables;

      std::string::size_type idx = s.find('(');
      if (idx == std::string::npos)
      {
        name = s;
      }
      else
      {
        name = s.substr(0, idx);
        assert(*s.rbegin() == ')');
        std::vector<std::string> v;
        std::string w = s.substr(idx + 1, s.size() - idx - 2);
        split(v, w, is_any_of(","));
        // This doesn't compile in combination with 'using namespace std::rel_ops'
        // for Visual C++ 8.0 (looks like a compiler bug)
        // for (std::vector<std::string>::reverse_iterator i = v.rbegin(); i != v.rend(); ++i)
        // {
        //   data_expression d = variable(*i);
        //   variables = push_front(variables, d);
        // }
        for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
        {
          variables.push_back(new_data::parse_variable(*i));
        }
      }
      return std::make_pair(name, make_variable_list(variables));
    }
  } // namespace detail
  /// \endcond

} // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_PARSER_H
