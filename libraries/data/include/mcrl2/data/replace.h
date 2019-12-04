// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/replace.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_REPLACE_H
#define MCRL2_DATA_REPLACE_H

#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/replace_capture_avoiding.h"

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

  explicit replace_free_variables_builder(const Substitution& sigma_)
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

/// \endcond

} // namespace detail

/// \brief Returns the variables appearing in the right hand sides of the substitution.
template <typename Substitution>
std::set<data::variable> substitution_variables(const Substitution& /* sigma */)
{
  throw mcrl2::runtime_error("substitution_variables is undefined!");
}

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
  data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_builder_binding>(sigma).update(x);
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
  return data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_builder_binding>(sigma).apply(x);
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
  data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_builder_binding>(sigma).update(x, bound_variables);
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
  return data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_builder_binding>(sigma).apply(x, bound_variables);
}
//--- end generated data replace code ---//

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
