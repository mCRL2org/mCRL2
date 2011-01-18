#~ Copyright 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import re
from path import *
from mcrl2_utility import *

REPLACE_FUNCTION_TEXT = '''#ifdef MCRL2_NEW_REPLACE_VARIABLES
  template <typename T, typename Substitution>
  void replace_variables(T& x,
                         Substitution sigma,
                         typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
  {
    core::make_update_apply_builder<NAMESPACE::data_expression_builder>(sigma)(x);
  }

  template <typename T, typename Substitution>
  T replace_variables(const T& x,
                      Substitution sigma,
                      typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                     )
  {   
    return core::make_update_apply_builder<NAMESPACE::data_expression_builder>(sigma)(x);
  }

  template <typename T, typename Substitution>
  void replace_free_variables(T& x,
                              Substitution sigma,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
  {
    data::detail::make_replace_free_variables_builder<NAMESPACE::data_expression_builder, NAMESPACE::add_data_variable_binding>(sigma)(x);
  }

  template <typename T, typename Substitution>
  T replace_free_variables(const T& x,
                           Substitution sigma,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
  {
    return data::detail::make_replace_free_variables_builder<NAMESPACE::data_expression_builder, NAMESPACE::add_data_variable_binding>(sigma)(x);
  }

  template <typename T, typename Substitution, typename VariableContainer>
  void replace_free_variables(T& x,
                              Substitution sigma,
                              const VariableContainer& bound_variables,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
  {
    data::detail::make_replace_free_variables_builder<NAMESPACE::data_expression_builder, NAMESPACE::add_data_variable_binding>(sigma)(x, bound_variables);
  }

  template <typename T, typename Substitution, typename VariableContainer>
  T replace_free_variables(const T& x,
                           Substitution sigma,
                           const VariableContainer& bound_variables,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
  {
    return data::detail::make_replace_free_variables_builder<NAMESPACE::data_expression_builder, NAMESPACE::add_data_variable_binding>(sigma)(x, bound_variables);
  }
#else
  template < typename Container, typename Substitution >
  Container replace_variables(Container const& container, Substitution substitution)
  {
    return data::replace_variables(container, substitution);
  }
  
  template <typename Container, typename Substitution >
  Container replace_free_variables(Container const& container, Substitution substitution)
  {
    return data::replace_free_variables(container, substitution);
  }

  template <typename Container, typename Substitution , typename VariableSequence >
  Container replace_free_variables(Container const& container, Substitution substitution, VariableSequence const& bound)
  {
    return data::replace_free_variables(container, substitution, bound);
  }
#endif MCRL2_NEW_REPLACE_VARIABLES
'''

def generate_code(filename, namespace, label, text):
    text = re.sub('NAMESPACE', namespace, text)
    insert_text_in_file(filename, text, 'generated %s %s code' % (namespace, label))   

if __name__ == "__main__":
    generate_code('../../data/include/mcrl2/data/replace.h'        , 'data'            , 'replace', REPLACE_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/lps/replace.h'          , 'lps'             , 'replace', REPLACE_FUNCTION_TEXT)
    generate_code('../../process/include/mcrl2/process/replace.h'  , 'process'         , 'replace', REPLACE_FUNCTION_TEXT)
    generate_code('../../pbes/include/mcrl2/pbes/replace.h'        , 'pbes_system'     , 'replace', REPLACE_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/replace.h', 'action_formulas' , 'replace', REPLACE_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/replace.h', 'regular_formulas', 'replace', REPLACE_FUNCTION_TEXT)
    generate_code('../../lps/include/mcrl2/modal_formula/replace.h', 'state_formulas'  , 'replace', REPLACE_FUNCTION_TEXT)
