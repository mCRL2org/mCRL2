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
#include "mcrl2/data/is_simple_substitution.h"
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
  using super::apply;
  using super::update;

  const Substitution& sigma;
  bool innermost;

  replace_sort_expressions_builder(const Substitution& sigma_, bool innermost_)
    : sigma(sigma_),
      innermost(innermost_)
  {}

  sort_expression apply(const sort_expression& x)
  {
    if (innermost)
    {
      sort_expression y = super::apply(x);
      return sigma(y);
    }
    return sigma(x);
  }
};

template <template <class> class Builder, class Substitution>
replace_sort_expressions_builder<Builder, Substitution>
make_replace_sort_expressions_builder(const Substitution& sigma, bool innermost)
{
  return replace_sort_expressions_builder<Builder, Substitution>(sigma, innermost);
}

template <template <class> class Builder, class Substitution>
struct replace_data_expressions_builder: public Builder<replace_data_expressions_builder<Builder, Substitution> >
{
  typedef Builder<replace_data_expressions_builder<Builder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  Substitution sigma;
  bool innermost;

  replace_data_expressions_builder(Substitution sigma_, bool innermost_)
    : sigma(sigma_),
      innermost(innermost_)
  {}

  data_expression apply(const data_expression& x)
  {
    if (innermost)
    {
      data_expression y = super::apply(x);
      return sigma(y);
    }
    return sigma(x);
  }
};

template <template <class> class Builder, class Substitution>
replace_data_expressions_builder<Builder, Substitution>
make_replace_data_expressions_builder(const Substitution& sigma, bool innermost)
{
  return replace_data_expressions_builder<Builder, Substitution>(sigma, innermost);
}

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
struct replace_free_variables_builder: public Binder<Builder, replace_free_variables_builder<Builder, Binder, Substitution> >
{
  typedef Binder<Builder, replace_free_variables_builder<Builder, Binder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;
  using super::is_bound;
  using super::bound_variables;
  using super::increase_bind_count;

  const Substitution& sigma;

  replace_free_variables_builder(const Substitution& sigma_)
    : sigma(sigma_)
  {}

  template <typename VariableContainer>
  replace_free_variables_builder(const Substitution& sigma_, const VariableContainer& bound_variables)
    : sigma(sigma_)
  {
    increase_bind_count(bound_variables);
  }

  data_expression apply(const variable& v)
  {
    if (is_bound(v))
    {
      return v;
    }
    return sigma(v);
  }
};

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
replace_free_variables_builder<Builder, Binder, Substitution>
make_replace_free_variables_builder(const Substitution& sigma)
{
  return replace_free_variables_builder<Builder, Binder, Substitution>(sigma);
}

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution, class VariableContainer>
replace_free_variables_builder<Builder, Binder, Substitution>
make_replace_free_variables_builder(const Substitution& sigma, const VariableContainer& bound_variables)
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
    using utilities::detail::contains;
    if (!contains(V, v) && sigma(v) == v)
    {
      return v;
    }
    else
    {
      id_generator.add_identifier(v.name());
      data::variable w(id_generator(v.name()), v.sort());
      while (sigma(w) != w || contains(V, w))
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
  using utilities::detail::contains;
  if (!contains(V, v) && sigma(v) == v)
  {
    return v;
  }
  else
  {
    id_generator.add_identifier(v.name());
    data::variable w(id_generator(v.name()), v.sort());

    while (sigma(w) != w || contains(V, w))
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
  using super::apply;
  using super::update;

  replace_capture_avoiding_variables_builder(Substitution& sigma, std::multiset<data::variable>& V)
    : super(sigma, V)
  { }
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
  using super::apply;
  using super::update;

  Substitution& sigma;
  std::multiset<data::variable>& V;
  data::set_identifier_generator id_generator;
  substitution_updater<Substitution> update_sigma;

  add_capture_avoiding_replacement(Substitution& sigma_, std::multiset<data::variable>& V_)
    : sigma(sigma_), V(V_), update_sigma(sigma_, V_)
  { }

  data_expression apply(const variable& v)
  {
    return sigma(v);
  }

  data_expression apply(const data::where_clause& x)
  {
    data::assignment_list assignments = atermpp::container_cast<data::assignment_list>(x.declarations());
    std::vector<data::variable> tmp;
    for (const data::assignment& a: assignments)
    {
      tmp.push_back(a.lhs());
    }
    std::vector<data::variable> v = update_sigma.push(tmp);

    // The updated substitution should be applied to the body.
    const data::data_expression new_body = apply(x.body());
    update_sigma.pop(v);

    // The original substitution should be applied to the right hand sides of the assignments.
    std::vector<data::assignment> a;
    std::vector<data::variable>::const_iterator j = v.begin();
    for (data::assignment_list::const_iterator i = assignments.begin(); i != assignments.end(); ++i, ++j)
    {
      a.push_back(data::assignment(*j, apply(i->rhs())));
    }
    data_expression result = data::where_clause(new_body, assignment_list(a.begin(), a.end()));
    return result;
  }

  data_expression apply(const data::forall& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    data_expression result = data::forall(v, apply(x.body()));
    update_sigma.pop(v);
    return result;
  }

  data_expression apply(const data::exists& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    data_expression result = data::exists(v, apply(x.body()));
    update_sigma.pop(v);
    return result;
  }

  data_expression apply(const data::lambda& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    data_expression result = data::lambda(v, apply(x.body()));
    update_sigma.pop(v);
    return result;
  }

  data_equation apply(data_equation& /* x */)
  {
    throw mcrl2::runtime_error("not implemented yet");
  }
};
/// \endcond

} // namespace detail

//--- start generated data replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost,
                              typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                             )
{
  data::detail::make_replace_sort_expressions_builder<data::sort_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost,
                           typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                          )
{
  return data::detail::make_replace_sort_expressions_builder<data::sort_expression_builder>(sigma, innermost).apply(x);
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              const Substitution& sigma,
                              bool innermost,
                              typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                             )
{
  data::detail::make_replace_data_expressions_builder<data::data_expression_builder>(sigma, innermost).update(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           const Substitution& sigma,
                           bool innermost,
                           typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                          )
{
  return data::detail::make_replace_data_expressions_builder<data::data_expression_builder>(sigma, innermost).apply(x);
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       const Substitution& sigma,
                       typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                      )
{
  core::make_update_apply_builder<data::data_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    const Substitution& sigma,
                    typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                   )
{
  return core::make_update_apply_builder<data::data_expression_builder>(sigma).apply(x);
}

template <typename T, typename Substitution>
void replace_all_variables(T& x,
                           const Substitution& sigma,
                           typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                          )
{
  core::make_update_apply_builder<data::variable_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T replace_all_variables(const T& x,
                        const Substitution& sigma,
                        typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                       )
{
  return core::make_update_apply_builder<data::variable_builder>(sigma).apply(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            const Substitution& sigma,
                            typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma).update(x);
}

/// \brief Applies the substitution sigma to x.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         const Substitution& sigma,
                         typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                        )
{
  assert(data::is_simple_substitution(sigma));
  return data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma).apply(x);
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            const Substitution& sigma,
                            const VariableContainer& bound_variables,
                            typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                           )
{
  assert(data::is_simple_substitution(sigma));
  data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma).update(x, bound_variables);
}

/// \brief Applies the substitution sigma to x, where the elements of bound_variables are treated as bound variables.
/// \pre { The substitution sigma must have the property that FV(sigma(x)) is included in {x} for all variables x. }
template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         const Substitution& sigma,
                         const VariableContainer& bound_variables,
                         typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                        )
{
  assert(data::is_simple_substitution(sigma));
  return data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma).apply(x, bound_variables);
}
//--- end generated data replace code ---//

//--- start generated data replace_capture_avoiding code ---//
/// \brief Applies sigma as a capture avoiding substitution to x.
/// \param x The object to which the subsitution is applied.
/// \param sigma A mutable substitution.
/// \param sigma_variables a container of variables.
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }.
template <typename T, typename Substitution, typename VariableContainer>
void replace_variables_capture_avoiding(T& x,
                       Substitution& sigma,
                       const VariableContainer& sigma_variables,
                       typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                      )
{
  std::multiset<data::variable> V;
  data::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  data::detail::apply_replace_capture_avoiding_variables_builder<data::data_expression_builder, data::detail::add_capture_avoiding_replacement>(sigma, V).update(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x.
/// \param x The object to which the substiution is applied.
/// \param sigma A mutable substitution.
/// \param sigma_variables a container of variables.
/// \pre { sigma_variables must contain the free variables appearing in the right hand side of sigma }.
template <typename T, typename Substitution, typename VariableContainer>
T replace_variables_capture_avoiding(const T& x,
                    Substitution& sigma,
                    const VariableContainer& sigma_variables,
                    typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                   )
{
  std::multiset<data::variable> V;
  data::find_free_variables(x, std::inserter(V, V.end()));
  V.insert(sigma_variables.begin(), sigma_variables.end());
  return data::detail::apply_replace_capture_avoiding_variables_builder<data::data_expression_builder, data::detail::add_capture_avoiding_replacement>(sigma, V).apply(x);
}
//--- end generated data replace_capture_avoiding code ---//

template <typename T, typename Substitution>
void substitute_sorts(T& x,
                      const Substitution& sigma,
                      typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value >::type* = 0
                     )
{
  core::make_update_apply_builder<data::sort_expression_builder>(sigma).update(x);
}

template <typename T, typename Substitution>
T substitute_sorts(const T& x,
                   const Substitution& sigma,
                   typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value >::type* = 0
                  )
{
  return core::make_update_apply_builder<data::sort_expression_builder>(sigma).apply(x);
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REPLACE_H
