// Author(s): Wieger Wesselink, Jeroen Keiren, Jan Friso Groote
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
#include "boost/algorithm/string.hpp"
#include "mcrl2/exception.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/alpha.h"
#include "mcrl2/core/regfrmtrans.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/internal_format_conversion.h"

namespace mcrl2 {

namespace data {

  /// \cond INTERNAL_DOCS
  namespace detail {
    inline static data_specification const& default_specification()
    {
      static data_specification specification;

      return specification;
    }
  } // namespace detail
  /// \endcond  

  /// \brief Parses a and type checks a data specification.
  /// \details This function reads a data specification in 
  ///    input string text. It is assumed that the string contains
  ///    a single data specification, and nothing else. 
  ///    If a parse or type check error is detected
  ///    an mcrl2::runtime_error exception is raised with a string that
  ///    indicates the problem. <br>
  ///    A typical example of a specification is:
  ///    \code
  ///     sort D=struct d1 | d2;
  ///          F=D->Set(D);
  ///          Natural;
  ///     cons zero:Natural; plus:Natural->Natural;
  ///     map  plus:Natural#Natural->Natural;
  ///     var  x,y:Natural;
  ///     eqn  y==zero -> plus(x,y)=x;
  ///          plus(x,succ(y))=succ(plus(x,y));
  ///    \endcode
  ///  \param[in] text A textual description of the data specification.
  ///  \return the data specification corresponding to text.
  inline
  data_specification parse_data_specification(
                                 std::istream& text)
  { atermpp::aterm_appl spec = core::parse_data_spec(text);
    if (spec == 0)
      throw mcrl2::runtime_error("Error while parsing data specification");
    spec = core::type_check_data_spec(spec);
    if (spec == 0)
      throw mcrl2::runtime_error("Error while type checking data specification");
    data_specification d(spec);
    std::cerr << "READ SPEC " << spec << "\n";
    detail::internal_format_conversion(d); // Translate bag/set enumerations and numbers to internal format.
    return d;
  }

  /// \brief Parses a and type checks a data specification.
  /// \details This function reads a data specification in 
  ///    input string text. 
  ///    See for an example the function parse_data_expression
  ///    on a string.
  ///  \param[in] text A textual description of the data specification.
  ///  \return the data specification corresponding to the input istream.
  inline
  data_specification parse_data_specification(
                                 const std::string& text)
  { std::istringstream spec_stream(text);
    return parse_data_specification(spec_stream);
  }

  /// \brief Parses and type checks a data variable declaration list checking for double occurrences
  ///        of variables in an existing variable range.
  /// \details The shape of the variables are x_11,...,x_1n:S_1; ... x_m1,...,x_mk:S_m where
  ///      x_ij are variable strings and S_i are sort  expressions. It is checked that the
  ///      sort expressions are properly typed regarding the data specification and that
  ///      the variable names do not clash with the names of mappings and constructors. It
  ///      is also not allowed to use a variable name twice. If an optional range of variables
  ///      is given, then it is also checked that there are no conflicts with 
  ///      variable names in this range. An mcrl2::runtime_error exception is raised when an
  ///      error occurs. In this case no names added using the input iterator. The
  ///      default data specification contains all standard data types.<br>
  ///      The output iterator can be used as follows, on standard variable lists.
  ///      \code
  ///         variable_list l;
  ///         parse_variables("x:Nat; y:Pos", std::front_inserter(l));
  ///      \endcode
  /// \param[in] text A textual description of the variable declarations to be parsed.
  /// \param[out] i An input interator indicating where the parsed variables must be inserted.
  /// \param[in]  begin The start of a variable range against which the variables are checked
  ///             for double occurrences.
  /// \param[in]  end   The end of the variable range against which the parsed variables are checked.
  /// \param[in] data_spec The data specification that is used for type checking. 

  template < typename Output_iterator, typename Variable_iterator >
  void parse_variables(std::istream &text,
                                   Output_iterator o,
                                   const Variable_iterator begin,
                                   const Variable_iterator end,
                                   const data_specification& data_spec = detail::default_specification())
  { // Parse the variables list.
    variable_list data_vars = core::parse_data_vars(text);
    if (data_vars == 0)
      throw mcrl2::runtime_error("Error while parsing data variable declarations.");

    // Type check the variable list.
    /* atermpp::aterm_appl d=mcrl2::data::detail::data_specification_to_aterm_data_spec(
                                        mcrl2::data::remove_all_system_defined(data_spec), true); */
    atermpp::aterm_appl d=mcrl2::data::detail::data_specification_to_aterm_data_spec(data_spec, true);

    data_vars = core::type_check_data_vars(data_vars, d);

    if (data_vars == 0)
      throw mcrl2::runtime_error("Error while type checking data variable declarations.");
    // Undo sort renamings for compatibility with type checker
    data_vars = data::detail::undo_compatibility_renamings(data_spec, data_vars);
    data_vars = atermpp::reverse(data_vars);

    // Check that variables do not have equal names.
    for(variable_list::const_iterator v=data_vars.begin(); v!=data_vars.end(); ++v)
    { for(Variable_iterator i=begin; i!=end; ++i)
      { if (v->name()==i->name())
        throw mcrl2::runtime_error("Name conflict of variables " + pp(*i) + " and " + pp(*v) + ".");
      }
      for(variable_list::const_iterator v1=data_vars.begin(); v1!=data_vars.end(); ++v1)
      { if (((*v1)!=(*v)) && (v1->name()==v->name()))
        throw mcrl2::runtime_error("Name conflict of variables " + pp(*v1) + " and " + pp(*v) + ".");
      }
    }

    // Output the variables read via the Output iterator.
    std::copy(data_vars.begin(), data_vars.end(), o);
  }

  /// \brief Parses and type checks a data variable declaration list checking for double occurrences
  ///        of variables in an existing variable range.
  /// \details See parse_variables on a string for more explanation.
  /// \param[in] text A textual description of the variable declarations to be parsed.
  /// \param[out] i An input interator indicating where the parsed variables must be inserted.
  /// \param[in]  begin The start of a variable range against which the variables are checked
  ///             for double occurrences.
  /// \param[in]  end   The end of the variable range against which the parsed variables are checked.
  /// \param[in] data_spec The data specification that is used for type checking. 

  template <typename Output_iterator, typename Variable_iterator>
  void parse_variables(const std::string &text,
                                   Output_iterator i,
                                   Variable_iterator begin,
                                   Variable_iterator end,
                                   const data_specification& data_spec = detail::default_specification())
  {
    std::istringstream spec_stream(text);
    parse_variables(spec_stream, i, begin, end, data_spec);
  }

  /// \brief Parses and type checks a data variable declaration list.
  /// \details See parse_variables on a string for more explanation.
  /// \param[in] text A textual description of the variable declarations to be parsed.
  /// \param[out] i An input interator indicating where the parsed variables must be inserted.
  /// \param[in] data_spec The data specification that is used for type checking. 

  template <typename Output_iterator>
  void parse_variables(std::istream &text,
                                   Output_iterator i,
                                   const data_specification& data_spec = detail::default_specification())
  { variable_list v_list;
    parse_variables(text,i,v_list.begin(),v_list.end(),data_spec);
  }

  /// \brief Parses and type checks a data variable declaration list.
  /// \details See parse_variables on a string for more explanation.
  /// \param[in] text A textual description of the variable declarations to be parsed.
  /// \param[out] i An input interator indicating where the parsed variables must be inserted.
  /// \param[in] data_spec The data specification that is used for type checking. 

  template <typename Output_iterator>
  void parse_variables(const std::string &text,
                                   Output_iterator i,
                                   const data_specification& data_spec = detail::default_specification())
  { variable_list v_list;
    parse_variables(text,i,v_list.begin(),v_list.end(),data_spec);
  }

  /// \brief Parses and type checks a data variable declaration.
  /// \details
  ///    See the information for reading a variable declaration from a string.
  ///  \param[in] text A textual description of the variable declaration.
  ///  \param[in] data_spec The data specification that is used for type checking.
  ///  \return the variable corresponding to the input istream.
  inline
  variable parse_variable(const std::string& text,
                                      const data_specification& data_spec = detail::default_specification())
  {
    atermpp::vector < variable > variable_store;

    parse_variables(text + ";", std::back_inserter(variable_store),data_spec);

    if (variable_store.size()==0)
       throw mcrl2::runtime_error("Input does not contain a variable declaration.");
    if (variable_store.size()>1)
       throw mcrl2::runtime_error("Input contains more than one variable declaration.");

    return variable_store.front();
  }

  /// \brief Parses and type checks a data variable declaration.
  /// \details 
  ///    A variable declaration has the form x:S where x is a string and S is a
  ///    sort expression. No trailing information after the declaration of the
  ///    variable is allowed. The declaration is checked using the data specification
  ///    that is provided. The default data specification contains all standard 
  ///    data types.
  ///    If a parse or typecheck error occurs an mcrl2::runtime_error exception
  ///    is raised.
  ///  \param[in] text A textual description of the variable declaration.
  ///  \param[in] data_spec The data specification that is used for type checking.
  ///  \return the variable corresponding to the string text.
  inline
  variable parse_variable(std::istream& text,
                                      const data_specification& data_spec = detail::default_specification())
  {
    std::ostringstream input;
    input << text.rdbuf();
    return parse_variable(input.str(),data_spec);
  }

  /// \brief Parses and type checks a data expression.
  /// \details 
  ///     A data expression is read from the input where it is assumed that
  ///     it can contain variables from the range from begin to end. The data     
  ///     expression is type checked using the given data specification data_spec.
  ///     The default data specification contains all standard sorts and functions.
  ///     If a parse or type check error occurs this is reported using a mcrl2::runtime_error
  ///     exception. It is assumed that the input contains exactly one expression, and nothing
  ///     else.
  /// \param[in] text The input text containing a data expression.
  /// \param[in]  begin The start of a variables that can occur in the data expression.
  /// \param[in]  end   The end of the potentially free variables in the expression.
  /// \param[in] data_spec The data specification that is used for type checking. 

  template <typename Variable_iterator>
  data_expression parse_data_expression(std::istream &text,
                                        const Variable_iterator begin,
                                        const Variable_iterator end,
                                        const data_specification& data_spec = detail::default_specification())
  {
    atermpp::aterm_appl data_expr = core::parse_data_expr(text);
    if (data_expr == 0)
      throw mcrl2::runtime_error("error parsing data expression");
    //create ATerm table from begin and end
    atermpp::table variables;
    for(Variable_iterator v=begin; v!=end; ++v)
    { // The application of atermpp::aterm_string is necessary to take care that
      // the name of the variable is quoted, which is what the typechecker expects.
      variables.put(atermpp::aterm_string(v->name()),v->sort());
    }
    std::cerr << "Parsed data expression " << data_expr << "\n";
    /* data_expr = core::type_check_data_expr(data_expr, 0,
                 mcrl2::data::detail::data_specification_to_aterm_data_spec(
                        mcrl2::data::remove_all_system_defined(data_spec), true), variables); */
    data_expr = core::type_check_data_expr(data_expr, 0,
                 mcrl2::data::detail::data_specification_to_aterm_data_spec(data_spec, true), variables);
    if (data_expr == 0)
      throw mcrl2::runtime_error("error type checking data expression");
    std::cerr << "Typed data expression " << data_expr << "\n";
    // Undo sort renamings for compatibility with type checker
    data_expr = data::detail::undo_compatibility_renamings(data_spec, data_expr);
    detail::internal_format_conversion_helper converter(data_spec);
    return converter(data_expression(data_expr));
  }

  /// \brief Parses and type checks a data expression.
  /// \details
  ///     See parsing a data expression from a string for details.
  /// \param[in] text The input text containing a data expression.
  /// \param[in]  begin The start of a variables that can occur in the data expression.
  /// \param[in]  end   The end of the potentially free variables in the expression.
  /// \param[in] data_spec The data specification that is used for type checking. 

  template <typename Variable_iterator>
  data_expression parse_data_expression(const std::string &text,
                                        const Variable_iterator begin,
                                        const Variable_iterator end,
                                        const data_specification& data_spec = detail::default_specification())
  {
    std::istringstream spec_stream(text);
    return parse_data_expression(spec_stream, begin,end, data_spec);
  }

  /// \brief Parses and type checks a data expression.
  /// \details
  ///     See parsing a data expression from a string for details.
  /// \param[in] text The input text containing a data expression.
  /// \param[in] data_spec The data specification that is used for type checking. 
  inline
  data_expression parse_data_expression(std::istream &text,
                                        const data_specification& data_spec = detail::default_specification())
  { variable_list v_list;
    return parse_data_expression(text,v_list.begin(),v_list.end(),data_spec);
  }

  /// \brief Parses and type checks a data expression.
  /// \details
  ///     See parsing a data expression from a string for details.
  /// \param[in] text The input text containing a data expression.
  /// \param[in] data_spec The data specification that is used for type checking. 
  inline
  data_expression parse_data_expression(const std::string &text,
                                        const data_specification& data_spec = detail::default_specification())
  { variable_list v_list;
    return parse_data_expression(text,v_list.begin(),v_list.end(),data_spec);
  }

  /// \brief Parses and type checks a data expression.
  /// \details
  ///     See parsing a data expression from a string for details.
  /// \param[in] text The input text containing a data expression.
  /// \param[in] var_decl a list of variable declarations
  /// \param[in] data_spec The data specification that is used for type checking. 
  inline
  data_expression parse_data_expression(const std::string& text,
                                        const std::string& var_decl,
                                        const data_specification& data_spec = detail::default_specification())
  { atermpp::vector < variable > variable_store;
    parse_variables(var_decl,std::back_inserter(variable_store),data_spec);
    return parse_data_expression(text,variable_store.begin(),variable_store.end(),data_spec);
  }

  /// \brief Parses and type checks a sort expression.
  /// \details See parsing a sort expression from a string for details.
  /// \param[in] text The input text containing a sort expression.
  /// \param[in] data_spec The data specification that is used for type checking. 
  inline
  sort_expression parse_sort_expression(std::istream &text,
                                        const data_specification& data_spec = detail::default_specification())
  {
    atermpp::aterm_appl sort_expr = core::parse_sort_expr(text);
    if (sort_expr == 0)
      throw mcrl2::runtime_error("error parsing sort expression");
    /* atermpp::aterm_appl aterm_data_spec=mcrl2::data::detail::data_specification_to_aterm_data_spec(
                                                mcrl2::data::remove_all_system_defined(data_spec), true); */
    atermpp::aterm_appl aterm_data_spec=mcrl2::data::detail::data_specification_to_aterm_data_spec(data_spec, true);
    sort_expr = core::type_check_sort_expr(sort_expr, aterm_data_spec);
    if (sort_expr == 0)
      throw mcrl2::runtime_error("error type checking sort expression");
    // Undo sort renamings for compatibility with type checker
    sort_expr = data::detail::undo_compatibility_renamings(data_spec, sort_expr);
    return sort_expression(sort_expr);
  }

  /// \brief Parses and type checks a sort expression.
  /// \details
  ///     Parses and type checks the sort expression. An error is signalled using
  ///     the mcrl2::runtime_error exception. This routine expects exactly one sort
  ///     expression on the input. The default data specification contains all standard
  ///     sorts.
  /// \param[in] text The input text containing a sort expression.
  /// \param[in] data_spec The data specification that is used for type checking. 
  inline
  sort_expression parse_sort_expression(const std::string &text,
                                        const data_specification& data_spec = detail::default_specification())
  {
    std::istringstream spec_stream(text);
    return parse_sort_expression(spec_stream, data_spec);
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
  data_expression parse_data_expression(std::string text, std::string var_decl, std::string data_spec)
  { 
    return parse_data_expression(text,var_decl,data::parse_data_specification(data_spec));
  }

  /// \cond INTERNAL_DOCS
  namespace detail {
    /// \brief Parses a data variable that is applied to arguments. 
    /// This is typically used for parsing pbes variables or variables in the modal formula context.
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
          variables.push_back(data::parse_variable(*i));
        }
      }
      return std::make_pair(name, make_variable_list(variables));
    }
  } // namespace detail
  /// \endcond  

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_PARSER_H
