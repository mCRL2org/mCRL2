// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/replace.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_REPLACE_H
#define MCRL2_PROCESS_REPLACE_H

#include "mcrl2/data/replace.h"
#include "mcrl2/process/add_binding.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/find.h"

namespace mcrl2
{

namespace process
{

namespace detail {

/// \cond INTERNAL_DOCS
template <template <class> class Builder, class Derived, class Substitution>
struct add_capture_avoiding_replacement: public data::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution>
{
  typedef data::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::sigma;
  using super::update_sigma;

  data::assignment_list::const_iterator find_variable(const data::assignment_list& a, const data::variable& v) const
  {
    for (auto i = a.begin(); i != a.end(); ++i)
    {
      if (i->lhs() == v)
      {
        return i;
      }
    }
    return a.end();
  }

  add_capture_avoiding_replacement(Substitution& sigma, std::multiset<data::variable>& V)
    : super(sigma, V)
  { }

  process::process_expression operator()(const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment_list a = x.assignments();
    std::vector<data::assignment> v;

    auto const& variables = x.identifier().variables();
    for (auto j = variables.begin(); j != variables.end(); ++j)
    {
      auto k = find_variable(a, *j);
      if (k == a.end())
      {
        data::data_expression e = (*this)(*j);
        if (e != *j)
        {
          v.push_back(data::assignment(*j, e));
        }
      }
      else
      {
        v.push_back(data::assignment(k->lhs(), (*this)(k->rhs())));
      }
    }
    process::process_expression result = process::process_instance_assignment(x.identifier(), data::assignment_list(v.begin(), v.end()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  process_expression operator()(const sum& x)
  {
    data::variable_list v = update_sigma.push(x.bound_variables());
    process_expression result = sum(v, (*this)(x.operand()));
    update_sigma.pop(v);
    return result;
  }
};

template <template <class> class Builder, template <template <class> class, class, class> class Binder, class Substitution>
struct replace_capture_avoiding_variables_builder: public Binder<Builder, replace_capture_avoiding_variables_builder<Builder, Binder, Substitution>, Substitution>
{
  typedef Binder<Builder, replace_capture_avoiding_variables_builder<Builder, Binder, Substitution>, Substitution> super;
  using super::enter;
  using super::leave;
  using super::operator();

  replace_capture_avoiding_variables_builder(Substitution& sigma, std::multiset<data::variable>& V)
    : super(sigma, V)
  { }

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, template <template <class> class, class, class> class Binder, class Substitution>
replace_capture_avoiding_variables_builder<Builder, Binder, Substitution>
apply_replace_capture_avoiding_variables_builder(Substitution& sigma, std::multiset<data::variable>& V)
{
  return replace_capture_avoiding_variables_builder<Builder, Binder, Substitution>(sigma, V);
}
/// \endcond

} // namespace detail

//--- start generated process replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost,
                              typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<process::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost,
                           typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                          )
{
  return data::detail::make_replace_sort_expressions_builder<process::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost,
                              typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<process::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost,
                           typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                          )
{
  return data::detail::make_replace_data_expressions_builder<process::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       const Substitution& sigma,
                       typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                      )
{
  core::make_update_apply_builder<process::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    const Substitution& sigma,
                    typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                   )
{
  return core::make_update_apply_builder<process::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           const Substitution& sigma,
                           typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                          )
{
  core::make_update_apply_builder<process::variable_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        const Substitution& sigma,
                        typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                       )
{
  return core::make_update_apply_builder<process::variable_builder>(sigma)(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            const Substitution& sigma,
                            typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_binding>(sigma)(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         const Substitution& sigma,
                         typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                        )
{
  assert(data::is_simple_substitution(sigma));
  return data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_binding>(sigma)(x);
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            const Substitution& sigma,
                            const VariableContainer& bound_variables,
                            typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_binding>(sigma)(x, bound_variables);
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         const Substitution& sigma,
                         const VariableContainer& bound_variables,
                         typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                        )
{
  assert(data::is_simple_substitution(sigma));
  return data::detail::make_replace_free_variables_builder<process::data_expression_builder, process::add_data_variable_binding>(sigma)(x, bound_variables);
}
//--- end generated process replace code ---//

//--- start generated process replace_capture_avoiding code ---//
/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma A mutable substitution
/// \param sigma_variables a container of variables
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }
template <typename T, typename Substitution, typename VariableContainer>
void replace_variables_capture_avoiding(T& x,
                       Substitution& sigma,
                       const VariableContainer& sigma_variables,
                       typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                      )
{
  std::multiset<data::variable> V;
  process::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  data::detail::apply_replace_capture_avoiding_variables_builder<process::data_expression_builder, process::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma A mutable substitution
/// \param sigma_variables a container of variables
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }
template <typename T, typename Substitution, typename VariableContainer>
T replace_variables_capture_avoiding(const T& x,
                    Substitution& sigma,
                    const VariableContainer& sigma_variables,
                    typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = 0
                   )
{
  std::multiset<data::variable> V;
  process::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  return data::detail::apply_replace_capture_avoiding_variables_builder<process::data_expression_builder, process::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}
//--- end generated process replace_capture_avoiding code ---//

struct process_identifier_assignment
{
  process_identifier lhs;
  process_identifier rhs;

  typedef process_identifier result_type;
  typedef process_identifier argument_type;

  process_identifier_assignment(const process_identifier& lhs_, const process_identifier& rhs_)
    : lhs(lhs_), rhs(rhs_)
  {}

  process_identifier operator()(const process_identifier& x)
  {
    if (x == lhs)
    {
      return rhs;
    }
    return x;
  }
};

template <typename T, typename Substitution>
void replace_process_identifiers(T& x,
                       Substitution sigma,
                       typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value >::type* = 0
                      )
{
  core::make_update_apply_builder<process::process_identifier_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_process_identifiers(const T& x,
                    Substitution sigma,
                    typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value >::type* = 0
                   )
{
  return core::make_update_apply_builder<process::process_identifier_builder>(sigma)(x);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_REPLACE_H
