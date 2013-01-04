// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/replace.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_REPLACE_H
#define MCRL2_LPS_REPLACE_H

#include "mcrl2/data/replace.h"
#include "mcrl2/lps/add_binding.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2
{

namespace lps
{

//--- start generated lps replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<lps::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_sort_expressions_builder<lps::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<lps::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_data_expressions_builder<lps::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<lps::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{   
  return core::make_update_apply_builder<lps::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<lps::data_expression_builder, lps::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<lps::data_expression_builder, lps::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<lps::data_expression_builder, lps::add_data_variable_binding>(sigma)(x, bound_variables);
}

template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<lps::data_expression_builder, lps::add_data_variable_binding>(sigma)(x, bound_variables);
}
//--- end generated lps replace code ---//

namespace detail {

/// \cond INTERNAL_DOCS
template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
struct replace_process_parameter_builder: public Binder<Builder, replace_process_parameter_builder<Builder, Binder, Substitution> >
{
  typedef Binder<Builder, replace_process_parameter_builder<Builder, Binder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::is_bound;
  using super::bind_count;
  using super::increase_bind_count;

  Substitution sigma;
  std::size_t count; // the bind count

  replace_process_parameter_builder(Substitution sigma_)
    : sigma(sigma_), count(1)
  {}

  template <typename VariableContainer>
  replace_process_parameter_builder(Substitution sigma_, const VariableContainer& bound_variables)
    : sigma(sigma_), count(1)
  {
    increase_bind_count(bound_variables);
  }

  data::data_expression operator()(const data::variable& x)
  {
    if (bind_count(x) == count)
    {
      return sigma(x);
    }
    return x;
  }

  data::assignment_expression operator()(const data::assignment& x)
  {
    data::variable lhs = (*this)(x.lhs());
    data::data_expression rhs = (*this)(x.rhs());
    return data::assignment_expression(lhs, rhs);
  }

  void operator()(lps::deadlock_summand& x)
  {
    count = 1;
    super::operator()(x);
  }

  void operator()(lps::action_summand& x)
  {
    count = 1;
    super::operator()(x);
    x.assignments() = super::operator()(x.assignments());
  }

  lps::process_initializer operator()(const lps::process_initializer& x)
  {
    count = 0;
    lps::process_initializer result = super::operator()(x);
    result.assignments() = super::operator()(result.assignments());
    return result;
  }

  void operator()(lps::linear_process& x)
  {
    super::operator()(x);
    count = 0;
    x.process_parameters() = super::operator()(x.process_parameters());
  }

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
replace_process_parameter_builder<Builder, Binder, Substitution>
make_replace_process_parameters_builder(Substitution sigma)
{
  return replace_process_parameter_builder<Builder, Binder, Substitution>(sigma);
}
/// \endcond

} // namespace detail

/// \brief Applies a substitution to the process parameters of the specification spec.
template <typename Substitution>
void replace_process_parameters(specification& spec, Substitution sigma)
{
  lps::detail::make_replace_process_parameters_builder<lps::data_expression_builder, lps::add_data_variable_binding>(sigma)(spec);

  // This does not work!
  //linear_process& p = spec.process();
  //p.process_parameters() = data::replace_free_variables(p.process_parameters(), sigma);
  //lps::replace_free_variables(p.action_summands(), sigma);
  //lps::replace_free_variables(p.deadlock_summands(), sigma);
}

/// \brief Applies a substitution to the process parameters of the specification spec.
template <typename Substitution>
void replace_summand_variables(specification& spec, Substitution sigma)
{
  action_summand_vector& a = spec.process().action_summands();
  for (action_summand_vector::iterator i = a.begin(); i != a.end(); ++i)
  {
    i->summation_variables() = data::replace_variables(i->summation_variables(), sigma);
    i->condition() = data::replace_free_variables(i->condition(), sigma);
    lps::replace_free_variables(i->multi_action(), sigma);
    i->assignments() = lps::replace_free_variables(i->assignments(), sigma);
  }
  deadlock_summand_vector& d = spec.process().deadlock_summands();
  for (deadlock_summand_vector::iterator i = d.begin(); i != d.end(); ++i)
  {
    i->summation_variables() = data::replace_variables(i->summation_variables(), sigma);
    i->condition() = data::replace_free_variables(i->condition(), sigma);
    lps::replace_free_variables(i->deadlock(), sigma);
  }
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_REPLACE_H
