// Author(s): Jan Friso Groote. Based on pbes/remove_parameters.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/remove_parameters.h
/// \brief Functions for removing insignificant parameters from pres types.

#ifndef MCRL2_PRES_REMOVE_PARAMETERS_H
#define MCRL2_PRES_REMOVE_PARAMETERS_H

#include "mcrl2/pres/builder.h"



namespace mcrl2::pres_system
{

/// \cond INTERNAL_DOCS
namespace detail
{

/// \brief Removes elements with indices in a given sequence from the sequence l
/// \param l A sequence of terms
/// \param to_be_removed A sorted sequence of integers
/// \return The removal result
template <typename Term>
atermpp::term_list<Term> remove_elements(const atermpp::term_list<Term>& l, const std::vector<std::size_t>& to_be_removed)
{
  assert(std::is_sorted(to_be_removed.begin(), to_be_removed.end()));
  std::size_t index = 0;
  std::vector<Term> result;
  auto j = to_be_removed.begin();
  for (auto i = l.begin(); i != l.end(); ++i, ++index)
  {
    if (j != to_be_removed.end() && index == *j)
    {
      ++j;
    }
    else
    {
      result.push_back(*i);
    }
  }
  return atermpp::term_list<Term>(result.begin(),result.end());
}

template <typename Derived>
struct remove_parameters_builder: public pres_system::pres_expression_builder<Derived>
{
  using super = pres_system::pres_expression_builder<Derived>;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  const std::vector<std::size_t>& to_be_removed;

  remove_parameters_builder(const std::vector<std::size_t>& to_be_removed_)
    : to_be_removed(to_be_removed_)
  {}

  template <class T>
  void apply(T& result, const propositional_variable& x)
  {
    pres_system::make_propositional_variable(result, x.name(), detail::remove_elements(x.parameters(), to_be_removed));
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    make_propositional_variable_instantiation(result, x.name(), detail::remove_elements(x.parameters(), to_be_removed));
  }

  void update(pres_equation& x)
  {
    propositional_variable variable;
    static_cast<Derived&>(*this).apply(variable, x.variable());
    x.variable() = variable;
    pres_expression formula;
    static_cast<Derived&>(*this).apply(formula, x.formula());
    x.formula() = formula;
  }

  void update(pres& x)
  {
    static_cast<Derived&>(*this).update(x.equations());
    propositional_variable_instantiation initial_state;
    static_cast<Derived&>(*this).apply(initial_state, x.initial_state());
    x.initial_state() = initial_state;
    static_cast<Derived&>(*this).update(x.global_variables());
  }
};


} // namespace detail
/// \endcond

/// \brief Removes parameters from propositional variable instantiations in a pres expression
/// \param x A PRES library object that derives from atermpp::aterm
/// \param to_be_removed The indices of parameters that are to be removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
template <typename T>
T remove_parameters(const T& x,
    const std::vector<std::size_t>& to_be_removed,
    std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_apply_builder_arg1<detail::remove_parameters_builder>(to_be_removed).apply(result, x);
  return result;
}

/// \brief Removes parameters from propositional variable instantiations in a pres expression
/// \param x A PRES library object that does not derive from atermpp::aterm
/// \param to_be_removed The indices of parameters that are to be removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
template <typename T>
void remove_parameters(T& x,
    const std::vector<std::size_t>& to_be_removed,
    std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_apply_builder_arg1<detail::remove_parameters_builder>(to_be_removed).update(x);
}

/// \cond INTERNAL_DOCS
namespace detail
{

template <typename Derived>
struct map_based_remove_parameters_builder: public pres_expression_builder<Derived>
{
  using super = pres_expression_builder<Derived>;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  const std::map<core::identifier_string, std::vector<std::size_t> >& to_be_removed;

  map_based_remove_parameters_builder(const std::map<core::identifier_string, std::vector<std::size_t> >& to_be_removed_)
    : to_be_removed(to_be_removed_)
  {}

  // to prevent default operator() being called
  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = x;
  }

  template <class T>
  void apply(T& result, const propositional_variable& x)
  {
    auto i = to_be_removed.find(x.name());
    if (i == to_be_removed.end())
    {
      result = x;
      return;
    }
    result = remove_parameters(x, i->second);
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    auto i = to_be_removed.find(x.name());
    if (i == to_be_removed.end())
    {
      result = x;
    }
    else
    {
      result = remove_parameters(x, i->second);
    }
  }

  void update(pres_equation& x)
  {
    propositional_variable variable;
    static_cast<Derived&>(*this).apply(variable, x.variable());
    x.variable() = variable;
    pres_expression formula;
    static_cast<Derived&>(*this).apply(formula, x.formula());
    x.formula() = formula;
  }

  void update(pres& x)
  {
    static_cast<Derived&>(*this).update(x.equations());
    propositional_variable_instantiation initial_state;
    static_cast<Derived&>(*this).apply(initial_state, x.initial_state());
    x.initial_state() = initial_state;
  }
};
} // namespace detail
/// \endcond

/// \brief Removes parameters from propositional variable instantiations in a pres expression
/// \param x A PRES library object that derives from atermpp::aterm
/// \param to_be_removed A mapping that maps propositional variable names to indices of parameters that are removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
template <typename T>
T remove_parameters(const T& x,
    const std::map<core::identifier_string, std::vector<std::size_t>>& to_be_removed,
    std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_apply_builder_arg1<detail::map_based_remove_parameters_builder>(to_be_removed).apply(result, x);
  return result;
}

/// \brief Removes parameters from propositional variable instantiations in a pres expression
/// \param x A PRES library object that does not derive from atermpp::aterm
/// \param to_be_removed A mapping that maps propositional variable names to a sorted vector of parameter indices that
/// need to be removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
template <typename T>
void remove_parameters(T& x,
    const std::map<core::identifier_string, std::vector<std::size_t>>& to_be_removed,
    std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  core::make_apply_builder_arg1<detail::map_based_remove_parameters_builder>(to_be_removed).update(x);
}

/// \cond INTERNAL_DOCS
namespace detail
{

template <typename Derived>
struct set_based_remove_parameters_builder: public pres_expression_builder<Derived>
{
  using super = pres_expression_builder<Derived>;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  const std::set<data::variable>& to_be_removed;

  set_based_remove_parameters_builder(const std::set<data::variable>& to_be_removed_)
    : to_be_removed(to_be_removed_)
  {}

  void remove_parameters(std::set<data::variable>& x) const
  {
    for (auto i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
    {
      x.erase(*i);
    }
  }

  void apply_(data::variable_list& result, const data::variable_list& l) const
  {
    using utilities::detail::contains;

    std::vector<data::variable> result_vec;
    for (const data::variable& v: l)
    {
      if (!contains(to_be_removed, v))
      {
        result_vec.push_back(v);
      }
    }
    result = data::variable_list(result_vec.begin(), result_vec.end());
  }

  template <class T>
  void apply(T& result, const data::assignment_list& l) const
  {
    using utilities::detail::contains;
    std::vector<data::assignment> a(l.begin(), l.end());
    a.erase(std::remove_if(a.begin(), a.end(), [&](const data::assignment& y) {	return contains(to_be_removed, y.lhs()); }), a.end());
    result = data::assignment_list(a.begin(), a.end());
  }

  template <class T>
  void apply(T& result, const propositional_variable& x)
  {
    data::variable_list vars;
    static_cast<Derived&>(*this).apply_(vars, x.parameters());  // Underscore is nasty trick to select the correct apply. 
    pres_system::make_propositional_variable(result, x.name(), vars);
  }

  void update(pres_equation& x)
  {
    propositional_variable variable;
    static_cast<Derived&>(*this).apply(variable, x.variable());
    x.variable() = variable;
    pres_expression formula;
    static_cast<Derived&>(*this).apply(formula, x.formula());
    x.formula() = formula;
  }

  void update(pres& x)
  {
    static_cast<Derived&>(*this).update(x.equations());
    propositional_variable_instantiation initial_state;
    static_cast<Derived&>(*this).apply(initial_state, x.initial_state());
    x.initial_state() = initial_state;
    remove_parameters(x.global_variables());
  }
};
} // namespace detail
/// \endcond

/// \brief Removes parameters from propositional variable instantiations in a pres expression
/// \param x A PRES library object that derives from atermpp::aterm
/// \param to_be_removed A set of parameters that are to be removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
template <typename T>
T remove_parameters(const T& x,
    const std::set<data::variable>& to_be_removed,
    std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  T result;
  core::make_apply_builder_arg1<detail::set_based_remove_parameters_builder>(to_be_removed).apply(result, x);
  return result;
}

/// \brief Removes parameters from propositional variable instantiations in a pres expression
/// \param x A PRES library object that does not derive from atermpp::aterm
/// \param to_be_removed A set of parameters that are to be removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
template <typename T>
void remove_parameters(T& x,
    const std::set<data::variable>& to_be_removed,
    std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  core::make_apply_builder_arg1<detail::set_based_remove_parameters_builder>(to_be_removed).update(x);
}


/// \cond INTERNAL_DOCS
// used in pres.h
inline
void remove_pres_parameters(pres& x,
                            const std::set<data::variable>& to_be_removed
                           )
{
  remove_parameters(x, to_be_removed);
}
/// \endcond

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_REMOVE_PARAMETERS_H
