#~ Copyright 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import re
from path import *
from mcrl2_utility import *

REWRITE_TEXT = '''/// \\\\brief Rewrites all embedded expressions in an object x
/// \param x an object containing expressions
/// \param R a rewriter
template <typename T, typename Rewriter>
void rewrite(T& x,
             Rewriter R,
             typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
            )
{
  data::detail::make_rewrite_data_expressions_builder<NAMESPACE::data_expression_builder>(R)(x);
}

/// \\\\brief Rewrites all embedded expressions in an object x
/// \param x an object containing expressions
/// \param R a rewriter
/// \\\\return the rewrite result
template <typename T, typename Rewriter>
T rewrite(const T& x,
          Rewriter R,
          typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
         )
{
  return data::detail::make_rewrite_data_expressions_builder<NAMESPACE::data_expression_builder>(R)(x);
}

/// \\\\brief Rewrites all embedded expressions in an object x, and applies a substitution to variables on the fly
/// \param x an object containing expressions
/// \param R a rewriter
/// \param sigma a substitution
template <typename T, typename Rewriter, typename Substitution>
void rewrite(T& x,
             Rewriter R,
             Substitution sigma,
             typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
            )
{
  data::detail::make_rewrite_data_expressions_with_substitution_builder<NAMESPACE::data_expression_builder>(R, sigma)(x);
}

/// \\\\brief Rewrites all embedded expressions in an object x, and applies a substitution to variables on the fly
/// \param x an object containing expressions
/// \param R a rewriter
/// \param sigma a substitution
/// \\\\return the rewrite result
template <typename T, typename Rewriter, typename Substitution>
T rewrite(const T& x,
          Rewriter R,
          Substitution sigma,
          typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
         )
{
  return data::detail::make_rewrite_data_expressions_with_substitution_builder<NAMESPACE::data_expression_builder>(R, sigma)(x);
}
'''

SUBSTITUTE_FUNCTION_TEXT = '''template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<NAMESPACE::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_sort_expressions_builder<NAMESPACE::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<NAMESPACE::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_data_expressions_builder<NAMESPACE::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<NAMESPACE::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{   
  return core::make_update_apply_builder<NAMESPACE::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<NAMESPACE::data_expression_builder, NAMESPACE::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<NAMESPACE::data_expression_builder, NAMESPACE::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<NAMESPACE::data_expression_builder, NAMESPACE::add_data_variable_binding>(sigma)(x, bound_variables);
}

template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<NAMESPACE::data_expression_builder, NAMESPACE::add_data_variable_binding>(sigma)(x, bound_variables);
}
'''

FIND_VARIABLES_FUNCTION_TEXT = '''/// \\\\brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \\\\return All variables that occur in the term x
template <typename T, typename OutputIterator>
void find_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_variables_traverser<NAMESPACE::variable_traverser>(o)(x);
}

/// \\\\brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \\\\return All variables that occur in the object x
template <typename T>
std::set<data::variable> find_variables(const T& x)
{
  std::set<data::variable> result;
  NAMESPACE::find_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \\\\brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are added.
/// \\\\return All free variables that occur in the object x
template <typename T, typename OutputIterator>
void find_free_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_free_variables_traverser<NAMESPACE::variable_traverser, NAMESPACE::add_data_variable_binding>(o)(x);
}

/// \\\\brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \param[in] bound a container of variables
/// \\\\return All free variables that occur in the object x
template <typename T, typename OutputIterator, typename VariableContainer>
void find_free_variables_with_bound(const T& x, OutputIterator o, const VariableContainer& bound)
{
  data::detail::make_find_free_variables_traverser<NAMESPACE::variable_traverser, NAMESPACE::add_data_variable_binding>(o, bound)(x);
}

/// \\\\brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \\\\return All free variables that occur in the object x
template <typename T>
std::set<data::variable> find_free_variables(const T& x)
{
  std::set<data::variable> result;
  NAMESPACE::find_free_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \\\\brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in] bound a bound a container of variables
/// \\\\return All free variables that occur in the object x
template <typename T, typename VariableContainer>
std::set<data::variable> find_free_variables_with_bound(const T& x, VariableContainer const& bound)
{
  std::set<data::variable> result;
  NAMESPACE::find_free_variables_with_bound(x, std::inserter(result, result.end()), bound);
  return result;
}

/// \\\\brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \param[in,out] o an output iterator to which all identifiers occurring in x are written.
/// \\\\return All identifiers that occur in the term x
template <typename T, typename OutputIterator>
void find_identifiers(const T& x, OutputIterator o)
{
  data::detail::make_find_identifiers_traverser<NAMESPACE::identifier_string_traverser>(o)(x);
}

/// \\\\brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \\\\return All identifiers that occur in the object x
template <typename T>
std::set<core::identifier_string> find_identifiers(const T& x)
{
  std::set<core::identifier_string> result;
  NAMESPACE::find_identifiers(x, std::inserter(result, result.end()));
  return result;
}

/// \\\\brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \param[in,out] o an output iterator to which all sort expressions occurring in x are written.
/// \\\\return All sort expressions that occur in the term x
template <typename T, typename OutputIterator>
void find_sort_expressions(const T& x, OutputIterator o)
{
  data::detail::make_find_sort_expressions_traverser<NAMESPACE::sort_expression_traverser>(o)(x);
}

/// \\\\brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \\\\return All sort expressions that occur in the object x
template <typename T>
std::set<data::sort_expression> find_sort_expressions(const T& x)
{
  std::set<data::sort_expression> result;
  NAMESPACE::find_sort_expressions(x, std::inserter(result, result.end()));
  return result;
}

/// \\\\brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \param[in,out] o an output iterator to which all function symbols occurring in x are written.
/// \\\\return All function symbols that occur in the term x
template <typename T, typename OutputIterator>
void find_function_symbols(const T& x, OutputIterator o)
{
  data::detail::make_find_function_symbols_traverser<NAMESPACE::data_expression_traverser>(o)(x);
}

/// \\\\brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \\\\return All function symbols that occur in the object x
template <typename T>
std::set<data::function_symbol> find_function_symbols(const T& x)
{
  std::set<data::function_symbol> result;
  NAMESPACE::find_function_symbols(x, std::inserter(result, result.end()));
  return result;
}
'''

def generate_code(filename, namespace, label, text):
    text = re.sub('NAMESPACE', namespace, text)
    insert_text_in_file(filename, text, 'generated %s %s code' % (namespace, label))
    print_labels(namespace, label)   

def print_labels(namespace, label):
    print '//--- start generated %s %s code ---//' % (namespace, label)
    print '//--- end generated %s %s code ---//' % (namespace, label)

def generate_rewrite_functions():
    generate_code('../../data/include/mcrl2/data/rewrite.h'        , 'data'            , 'rewrite', REWRITE_TEXT)
    generate_code('../../lps/include/mcrl2/lps/rewrite.h'          , 'lps'             , 'rewrite', REWRITE_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/rewrite.h', 'action_formulas' , 'rewrite', REWRITE_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/rewrite.h', 'regular_formulas', 'rewrite', REWRITE_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/rewrite.h', 'state_formulas'  , 'rewrite', REWRITE_TEXT)
    generate_code('../../pbes/include/mcrl2/pbes/rewrite.h'        , 'pbes_system'     , 'rewrite', REWRITE_TEXT)
    generate_code('../../process/include/mcrl2/process/rewrite.h'  , 'process'         , 'rewrite', REWRITE_TEXT)

def generate_replace_functions():
    generate_code('../../data/include/mcrl2/data/replace.h'        , 'data'            , 'replace', SUBSTITUTE_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/lps/replace.h'          , 'lps'             , 'replace', SUBSTITUTE_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/replace.h', 'action_formulas' , 'replace', SUBSTITUTE_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/replace.h', 'regular_formulas', 'replace', SUBSTITUTE_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/replace.h', 'state_formulas'  , 'replace', SUBSTITUTE_FUNCTION_TEXT)
    generate_code('../../pbes/include/mcrl2/pbes/replace.h'        , 'pbes_system'     , 'replace', SUBSTITUTE_FUNCTION_TEXT)
    generate_code('../../process/include/mcrl2/process/replace.h'  , 'process'         , 'replace', SUBSTITUTE_FUNCTION_TEXT)

def generate_find_functions():
    generate_code('../../data/include/mcrl2/data/find.h'        , 'data'            , 'find', FIND_VARIABLES_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/lps/find.h'          , 'lps'             , 'find', FIND_VARIABLES_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/find.h', 'action_formulas' , 'find', FIND_VARIABLES_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/find.h', 'regular_formulas', 'find', FIND_VARIABLES_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/find.h', 'state_formulas'  , 'find', FIND_VARIABLES_FUNCTION_TEXT)
    generate_code('../../pbes/include/mcrl2/pbes/find.h'        , 'pbes_system'     , 'find', FIND_VARIABLES_FUNCTION_TEXT)
    generate_code('../../process/include/mcrl2/process/find.h'  , 'process'         , 'find', FIND_VARIABLES_FUNCTION_TEXT)

if __name__ == "__main__":
    generate_rewrite_functions()
    generate_replace_functions()
    generate_find_functions()
