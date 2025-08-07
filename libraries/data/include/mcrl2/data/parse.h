// Author(s): Wieger Wesselink, Jeroen Keiren, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/parse.h
/// \brief Parser for data specifications.

#ifndef MCRL2_DATA_PARSE_H
#define MCRL2_DATA_PARSE_H

#include "mcrl2/data/typecheck.h"

namespace mcrl2::data
{

namespace detail {

sort_expression parse_sort_expression(const std::string& text);
variable_list parse_variables(const std::string& text);
data_expression parse_data_expression(const std::string& text);
data_specification parse_data_specification_new(const std::string& text);
variable_list parse_variable_declaration_list(const std::string& text);

inline static data_specification const& default_specification()
{
  static data_specification specification;
  return specification;
}

} // namespace detail

std::pair<basic_sort_vector, alias_vector> parse_sort_specification(const std::string& text);

/// \brief Parses a and type checks a data specification.
/// \details This function reads a data specification in
///    input string text. It is assumed that the string contains
///    a single data specification, and nothing else.
///    If a parse or type check error is detected
///    an mcrl2::runtime_error exception is raised with a string that
///    indicates the problem.
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
///  \param[in] in A input stream containing the data specification.
///  \return the data specification corresponding to text.
inline
data_specification parse_data_specification(std::istream& in)
{
  std::string text = utilities::read_text(in);
  data_specification result = detail::parse_data_specification_new(text);
  typecheck_data_specification(result);
  result.translate_user_notation();
  return result;
}

/// \brief Parses a and type checks a data specification.
/// \details This function reads a data specification in
///    input string text.
///    See for an example the function parse_data_expression
///    on a string.
///  \param[in] text A textual description of the data specification.
///  \return the data specification corresponding to the input istream.
inline
data_specification parse_data_specification(const std::string& text)
{
  // handle empty data specification
  if (utilities::trim_copy(text).empty())
  {
    return data_specification();
  }
  std::istringstream spec_stream(text);
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
/// \param[in] in An input stream containing the variable declarations to be parsed.
/// \param[out] o An output interator indicating where the parsed variables must be inserted.
/// \param[in]  begin The start of a variable range against which the variables are checked
///             for double occurrences.
/// \param[in]  end   The end of the variable range against which the parsed variables are checked.
/// \param[in] data_spec The data specification that is used for type checking.

template <typename OutputIterator, typename VariableIterator>
void parse_variables(std::istream& in,
                     OutputIterator o,
                     VariableIterator begin,
                     VariableIterator end,
                     const data_specification& data_spec = detail::default_specification())
{
  std::string text = utilities::read_text(in);
  utilities::trim(text);
  variable_list data_vars;

  if (!text.empty())
  {
    data_vars = detail::parse_variables(text);
    data_type_checker type_checker(data_spec);
    // Check the variable in an empty variable context.
    type_checker(data_vars,detail::variable_context());

    // Undo sort renamings for compatibility with type checker
    // data_vars = data::detail::undo_compatibility_renamings(data_spec, data_vars);
    data_vars = atermpp::reverse(data_vars);
    data_vars = normalize_sorts(data_vars, data_spec);

    // Check that variables do not have equal names.
    for (const variable& v: data_vars)
    {
      for (VariableIterator i = begin; i != end; ++i)
      {
        if (v.name()==i->name())
        {
          throw mcrl2::runtime_error("Name conflict of variables " + data::pp(*i) + " and " + data::pp(v) + ".");
        }
      }
      for (const variable& v1: data_vars)
      {
        if ((v1 != v) && (v1.name() == v.name()))
        {
          throw mcrl2::runtime_error("Name conflict of variables " + data::pp(v1) + " and " + data::pp(v) + ".");
        }
      }
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

template <typename OutputIterator, typename VariableIterator>
void parse_variables(const std::string& text,
                     OutputIterator i,
                     VariableIterator begin,
                     VariableIterator end,
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

template <typename OutputIterator>
void parse_variables(std::istream& text,
                     OutputIterator i,
                     const data_specification& data_spec = detail::default_specification())
{
  variable_list v_list;
  parse_variables(text,i,v_list.begin(),v_list.end(),data_spec);
}

/// \brief Parses and type checks a data variable declaration list.
/// \details See parse_variables on a string for more explanation.
/// \param[in] text A textual description of the variable declarations to be parsed.
/// \param[out] i An input interator indicating where the parsed variables must be inserted.
/// \param[in] data_spec The data specification that is used for type checking.

template <typename OutputIterator>
void parse_variables(const std::string& text,
                     OutputIterator i,
                     const data_specification& data_spec = detail::default_specification())
{
  variable_list v_list;
  parse_variables(text, i, v_list.begin(), v_list.end(), data_spec);
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
  std::vector<variable> v;

  parse_variables(text + ";", std::back_inserter(v),data_spec);

  if (v.empty())
  {
    throw mcrl2::runtime_error("Input does not contain a variable declaration.");
  }
  if (v.size() > 1)
  {
    throw mcrl2::runtime_error("Input contains more than one variable declaration.");
  }

  return v.front();
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
///    A data expression is read from the input where it is assumed that
///    it can contain variables from the range from begin to end. The data
///    expression is type checked using the given data specification data_spec.
///    The default data specification contains all standard sorts and functions.
///    If a parse or type check error occurs this is reported using a mcrl2::runtime_error
///    exception. It is assumed that the input contains exactly one expression, and nothing
///    else.
/// \param[in] in The input stream containing a data expression.
/// \param[in] variables The variables that can occur in the data expression.
/// \param[in] dataspec The data specification that is used for type checking.
/// \param[in] type_check Indication whether the expression is expected to be type checked.
/// \param[in] translate_user_notation Indication whether user notation such a numbers
///                                     must be translated to internal format.
/// \param[in] normalize_sorts Indication whether the sorts must be rewritten to normal form.
/// \returns The parsed data expression.
template <typename VariableContainer>
data_expression parse_data_expression(std::istream& in,
                                      const VariableContainer& variables,
                                      const data_specification& dataspec = detail::default_specification(),
                                      bool type_check = true,
                                      bool translate_user_notation = true,
                                      bool normalize_sorts = true
                                     )
{
  std::string text = utilities::read_text(in);
  data_expression x = detail::parse_data_expression(text);
  if (type_check)
  {
    x = data::typecheck_data_expression(x, variables, dataspec);
  }
  if (translate_user_notation)
  {
    x = data::translate_user_notation(x);
  }
  if (normalize_sorts)
  {
    x = data::normalize_sorts(x, dataspec);
  }
  return x;
}

/// \brief Parses and type checks a data expression.
/// \details
///     See parsing a data expression from a string for details.
/// \param[in] text The input text containing a data expression.
/// \param[in] variables A container with variables that can occur in the data expression.
/// \param[in] data_spec The data specification that is used for type checking.
/// \param[in] type_check Indication whether the expression is expected to be type checked.
/// \param[in] translate_user_notation Indication whether user notation such a numbers
///                                     must be translated to internal format.
/// \param[in] normalize_sorts Indication whether the sorts must be rewritten to normal form.
template <typename VariableContainer>
data_expression parse_data_expression(const std::string& text,
                                      const VariableContainer& variables,
                                      const data_specification& data_spec = detail::default_specification(),
                                      bool type_check = true,
                                      bool translate_user_notation = true,
                                      bool normalize_sorts = true
                                     )
{
  std::istringstream spec_stream(text);
  return parse_data_expression(spec_stream, variables, data_spec, type_check, translate_user_notation, normalize_sorts);
}

/// \brief Parses and type checks a data expression.
/// \details
///     See parsing a data expression from a string for details.
/// \param[in] text The input text containing a data expression.
/// \param[in] data_spec The data specification that is used for type checking.
/// \param[in] type_check Indication whether the expression is expected to be type checked.
/// \param[in] translate_user_notation Indication whether user notation such a numbers
///                                     must be translated to internal format.
/// \param[in] normalize_sorts Indication whether the sorts must be rewritten to normal form.
inline
data_expression parse_data_expression(std::istream& text,
                                      const data_specification& data_spec = detail::default_specification(),
                                      bool type_check = true,
                                      bool translate_user_notation = true,
                                      bool normalize_sorts = true
                                     )
{
  return parse_data_expression(text, variable_list(), data_spec, type_check, translate_user_notation, normalize_sorts);
}

/// \brief Parses and type checks a data expression.
/// \details
///     See parsing a data expression from a string for details.
/// \param[in] text The input text containing a data expression.
/// \param[in] data_spec The data specification that is used for type checking.
/// \param[in] type_check Indication whether the expression is expected to be type checked.
/// \param[in] translate_user_notation Indication whether user notation such a numbers
///                                     must be translated to internal format.
/// \param[in] normalize_sorts Indication whether the sorts must be rewritten to normal form.
inline
data_expression parse_data_expression(const std::string& text,
                                      const data_specification& data_spec = detail::default_specification(),
                                      bool type_check = true,
                                      bool translate_user_notation = true,
                                      bool normalize_sorts = true
                                     )
{
  return parse_data_expression(text, variable_list(), data_spec, type_check, translate_user_notation, normalize_sorts);
}

inline
variable_list parse_variables(const std::string& text)
{
  std::vector<variable> result;
  if (!text.empty())
  {
    parse_variables(text, std::back_inserter(result));
  }
  return variable_list(result.begin(), result.end());
}

/// \brief Parses and type checks a sort expression.
/// \details See parsing a sort expression from a string for details.
/// \param[in] in An input stream containing a sort expression.
/// \param[in] data_spec The data specification that is used for type checking.
inline
sort_expression parse_sort_expression(std::istream& in,
                                      const data_specification& data_spec = detail::default_specification())
{
  std::string text = utilities::read_text(in);
  sort_expression x = detail::parse_sort_expression(text);
  typecheck_sort_expression(x, data_spec);
  x = normalize_sorts(x, data_spec);
  return x;
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
sort_expression parse_sort_expression(const std::string& text,
                                      const data_specification& data_spec = detail::default_specification())
{
  std::istringstream spec_stream(text);
  return parse_sort_expression(spec_stream, data_spec);
}

// parse a string like 'tail: List(D) -> List(D)'
//
// TODO: replace this by a proper parse function once the current parser and type checker have been replaced
inline
data::function_symbol parse_function_symbol(const std::string& text, const std::string& dataspec_text = "")
{
  const std::string prefix = "UNIQUE_FUNCTION_SYMBOL_PREFIX";
  std::string s = utilities::trim_copy(text);
  std::string::size_type pos = s.find_first_of(':');
  std::string name = utilities::trim_copy(s.substr(0, pos));
  std::string type = prefix + s.substr(pos);
  std::string spec_text = dataspec_text + "\nmap " + prefix + type + ";\n";
  data::data_specification dataspec = data::parse_data_specification(spec_text);
  data::function_symbol f = dataspec.user_defined_mappings().back();
  data::function_symbol result = data::function_symbol(name, f.sort());
  return result;
}

/// \brief Parses a variable declaration list.
/// \param[in] text The input text.
/// \param[in] dataspec The data specification used to type normalize the sorts of the variables.
/// \return The parsed variable declaration list.
inline
variable_list parse_variable_declaration_list(const std::string& text, const data_specification& dataspec = detail::default_specification())
{
  variable_list variables = detail::parse_variable_declaration_list(text);
  variables = normalize_sorts(variables, dataspec);
  return variables;
}

/// \cond INTERNAL_DOCS
namespace detail
{

/// \brief Parses a data variable that is applied to arguments.
/// This is typically used for parsing pbes variables or variables in the modal formula context.
/// For example: "X(d:D,e:E)".
/// \param text A string
/// \return The parsed data variable
inline
std::pair<std::string, data_expression_list> parse_variable(std::string const& text)
{
  std::string name;
  data_expression_vector variables;

  std::string::size_type idx = text.find('(');
  if (idx == std::string::npos)
  {
    name = text;
  }
  else
  {
    name = text.substr(0, idx);
    assert(*text.rbegin() == ')');
    std::string w = text.substr(idx + 1, text.size() - idx - 2);
    std::vector<std::string> v = utilities::split(w, ",");
    for (const std::string& s: v)
    {
      variables.push_back(data::parse_variable(s));
    }
  }
  return std::make_pair(name, data_expression_list(variables.begin(), variables.end()));
}

} // namespace detail
/// \endcond

} // namespace mcrl2::data



#endif // MCRL2_DATA_PARSE_H
