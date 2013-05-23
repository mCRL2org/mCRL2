// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/replace.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_REPLACE_H
#define MCRL2_DATA_REPLACE_H

#include "mcrl2/data/add_binding.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

/// \cond INTERNAL_DOCS
template <template <class> class Builder, class Substitution>
struct replace_sort_expressions_builder: public Builder<replace_sort_expressions_builder<Builder, Substitution> >
{
  typedef Builder<replace_sort_expressions_builder<Builder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  Substitution sigma;
  bool innermost;

  replace_sort_expressions_builder(Substitution sigma_, bool innermost_)
    : sigma(sigma_),
      innermost(innermost_)
  {}

  sort_expression operator()(const sort_expression& x)
  {
    if (innermost)
    {
      sort_expression y = super::operator()(x);
      return sigma(y);
    }
    return sigma(x);
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, class Substitution>
replace_sort_expressions_builder<Builder, Substitution>
make_replace_sort_expressions_builder(Substitution sigma, bool innermost)
{
  return replace_sort_expressions_builder<Builder, Substitution>(sigma, innermost);
}

template <template <class> class Builder, class Substitution>
struct replace_data_expressions_builder: public Builder<replace_data_expressions_builder<Builder, Substitution> >
{
  typedef Builder<replace_data_expressions_builder<Builder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  Substitution sigma;
  bool innermost;

  replace_data_expressions_builder(Substitution sigma_, bool innermost_)
    : sigma(sigma_),
      innermost(innermost_)
  {}

  data_expression operator()(const data_expression& x)
  {
    if (innermost)
    {
      data_expression y = super::operator()(x);
      return sigma(y);
    }
    return sigma(x);
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, class Substitution>
replace_data_expressions_builder<Builder, Substitution>
make_replace_data_expressions_builder(Substitution sigma, bool innermost)
{
  return replace_data_expressions_builder<Builder, Substitution>(sigma, innermost);
}

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
struct replace_free_variables_builder: public Binder<Builder, replace_free_variables_builder<Builder, Binder, Substitution> >
{
  typedef Binder<Builder, replace_free_variables_builder<Builder, Binder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::is_bound;
  using super::bound_variables;
  using super::increase_bind_count;

  Substitution sigma;

  replace_free_variables_builder(Substitution sigma_)
    : sigma(sigma_)
  {}

  template <typename VariableContainer>
  replace_free_variables_builder(Substitution sigma_, const VariableContainer& bound_variables)
    : sigma(sigma_)
  {
    increase_bind_count(bound_variables);
  }

  data_expression operator()(const variable& v)
  {
    if (is_bound(v))
    {
      return v;
    }
    return sigma(v);
  }

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
replace_free_variables_builder<Builder, Binder, Substitution>
make_replace_free_variables_builder(Substitution sigma)
{
  return replace_free_variables_builder<Builder, Binder, Substitution>(sigma);
}

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution, class VariableContainer>
replace_free_variables_builder<Builder, Binder, Substitution>
make_replace_free_variables_builder(Substitution sigma, const VariableContainer& bound_variables)
{
  return replace_free_variables_builder<Builder, Binder, Substitution>(sigma, bound_variables);
}

template <typename Substitution>
struct substitution_updater
{
  Substitution& sigma;
  std::multiset<data::variable>& V;
  data::set_identifier_generator id_generator;
  std::vector<data::assignment> undo;
  std::vector<std::size_t> undo_sizes;

  substitution_updater(Substitution& sigma_, std::multiset<data::variable>& V_)
    : sigma(sigma_), V(V_)
  {}

  data::variable bind(const data::variable& v)
  {
    if (V.find(v) == V.end() && sigma(v) == v)
    {
      return v;
    }
    else
    {
      id_generator.add_identifier(v.name());
      data::variable w(id_generator(v.name()), v.sort());
      while (sigma(w) != w || V.find(w) != V.end())
      {
        w = data::variable(id_generator(v.name()), v.sort());
      }
      undo.push_back(data::assignment(v, sigma(v)));
      sigma[v] = w;
      return w;
    }
  }

  data::variable push(const data::variable& v)
  {
    undo_sizes.push_back(undo.size());
    data::variable result = bind(v);
    V.insert(result);
    return result;
  }

  void pop(const data::variable& v)
  {
    V.erase(V.find(v));
    std::size_t n = undo.size() - undo_sizes.back();
    undo_sizes.pop_back();
    if (n > 0)
    {
      const data::assignment& a = undo.back();
      sigma[a.lhs()] = a.rhs();
      undo.pop_back();
    }
  }

  template <typename VariableContainer>
  VariableContainer push(const VariableContainer& v)
  {
    undo_sizes.push_back(undo.size());
    std::vector<data::variable> result; for (typename VariableContainer::const_iterator i = v.begin(); i != v.end(); ++i) {
      data::variable w = bind(*i);
      V.insert(w);
      result.push_back(w);
    }
    return VariableContainer(result.begin(), result.end());
  }

  template <typename VariableContainer>
  void pop(const VariableContainer& v)
  {
    for (typename VariableContainer::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      V.erase(V.find(*i));
    }
    std::size_t n = undo.size() - undo_sizes.back();
    undo_sizes.pop_back();
    for (std::size_t i = 0; i < n; i++)
    {
      const data::assignment& a = undo.back();
      sigma[a.lhs()] = a.rhs();
      undo.pop_back();
    }
  }
};

template <typename Substitution, typename IdentifierGenerator>
data::variable update_substitution(Substitution& sigma, const data::variable& v, const std::multiset<data::variable>& V, IdentifierGenerator& id_generator)
{
  if (V.find(v) == V.end() && sigma(v) == v)
  {
    return v;
  }
  else
  {
    id_generator.add_identifier(v.name());
    data::variable w(id_generator(v.name()), v.sort());

    while (sigma(w) != w || V.find(w) != V.end())
    {
      w = data::variable(id_generator(v.name()), v.sort());
    }
    sigma[v] = w;
    return w;
  }
}

template <typename Substitution, typename IdentifierGenerator, typename VariableContainer>
VariableContainer update_substitution(Substitution& sigma, const VariableContainer& v, const std::multiset<data::variable>& V, IdentifierGenerator& id_generator)
{
  std::vector<data::variable> result;
  for (typename VariableContainer::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    result.push_back(update_substitution(sigma, *i, V, id_generator));
  }
  return VariableContainer(result.begin(), result.end());
}

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

template <template <class> class Builder, class Derived, class Substitution>
struct add_capture_avoiding_replacement: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  Substitution& sigma;
  std::multiset<data::variable>& V;
  data::set_identifier_generator id_generator;
  substitution_updater<Substitution> update_sigma;

  add_capture_avoiding_replacement(Substitution& sigma_, std::multiset<data::variable>& V_)
    : sigma(sigma_), V(V_), update_sigma(sigma_, V_)
  { }

  data_expression operator()(const variable& v)
  {
    return sigma(v);
  }

  data_expression operator()(const data::where_clause& x)
  {
    data::assignment_list assignments = atermpp::convert<data::assignment_list>(x.declarations());
    std::vector<data::variable> tmp;
    for (data::assignment_list::const_iterator i = assignments.begin(); i != assignments.end(); ++i)
    {
      tmp.push_back(i->lhs());
    }
    std::vector<data::variable> v = update_sigma.push(tmp);

    std::vector<data::assignment> a;
    std::vector<data::variable>::const_iterator j = v.begin();
    for (data::assignment_list::const_iterator i = assignments.begin(); i != assignments.end(); ++i, ++j)
    {
      a.push_back(data::assignment(*j, (*this)(i->rhs())));
    }
    data_expression result = data::where_clause((*this)(x.body()), assignment_list(a.begin(), a.end()));
    update_sigma.pop(v);
    return result;
  }

  data_expression operator()(const data::forall& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    data_expression result = data::forall(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  data_expression operator()(const data::exists& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    data_expression result = data::exists(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  data_expression operator()(const data::lambda& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    data_expression result = data::lambda(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  void operator()(data::data_equation& x)
  {
    throw mcrl2::runtime_error("not implemented yet");
  }

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};
/// \endcond

} // namespace detail

//--- start generated data replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<data::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_sort_expressions_builder<data::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<data::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_data_expressions_builder<data::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<data::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{
  return core::make_update_apply_builder<data::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           Substitution sigma,
                           typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                          )
{
  core::make_update_apply_builder<data::variable_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        Substitution sigma,
                        typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                       )
{
  return core::make_update_apply_builder<data::variable_builder>(sigma)(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  assert(data::is_simple_substitution(sigma));
  return data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x);
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x, bound_variables);
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                        )
{
  assert(data::is_simple_substitution(sigma));
  return data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x, bound_variables);
}
//--- end generated data replace code ---//

//--- start generated data replace_capture_avoiding code ---//
/// \brief Applies sigma as a capture avoiding substitution to x
/// \param x The term to which the substitution is applied
/// \param sigma A mutable substitution
/// \param sigma_variables a container of variables
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }
template <typename T, typename Substitution, typename VariableContainer>
void replace_variables_capture_avoiding(T& x,
                       Substitution& sigma,
                       const VariableContainer& sigma_variables,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                      )
{
  std::multiset<data::variable> V;
  data::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  data::detail::apply_replace_capture_avoiding_variables_builder<data::data_expression_builder, data::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param x The term to which the substitution is applied
/// \param sigma A mutable substitution
/// \param sigma_variables a container of variables
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }
template <typename T, typename Substitution, typename VariableContainer>
T replace_variables_capture_avoiding(const T& x,
                    Substitution& sigma,
                    const VariableContainer& sigma_variables,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                   )
{
  std::multiset<data::variable> V;
  data::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  return data::detail::apply_replace_capture_avoiding_variables_builder<data::data_expression_builder, data::detail::add_capture_avoiding_replacement>(sigma, V)(x);
}
//--- end generated data replace_capture_avoiding code ---//

template <typename T, typename Substitution>
void substitute_sorts(T& x,
                      Substitution sigma,
                      typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                     )
{
  core::make_update_apply_builder<data::sort_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T substitute_sorts(const T& x,
                   Substitution sigma,
                   typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                  )
{
  return core::make_update_apply_builder<data::sort_expression_builder>(sigma)(x);
}

} // namespace data

} // namespace mcrl2

#ifndef MCRL2_DATA_SUBSTITUTIONS_H
#include "mcrl2/data/substitutions.h"
#endif

#endif // MCRL2_DATA_REPLACE_H
