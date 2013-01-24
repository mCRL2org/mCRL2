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

#include "mcrl2/atermpp/map.h"
#include "mcrl2/data/add_binding.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

/// \cond INTERNAL_DOCS
template <template <class> class Builder, class Substitution>
struct substitute_sort_expressions_builder: public Builder<substitute_sort_expressions_builder<Builder, Substitution> >
{
  typedef Builder<substitute_sort_expressions_builder<Builder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  Substitution sigma;
  bool innermost;

  substitute_sort_expressions_builder(Substitution sigma_, bool innermost_)
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
substitute_sort_expressions_builder<Builder, Substitution>
make_replace_sort_expressions_builder(Substitution sigma, bool innermost)
{
  return substitute_sort_expressions_builder<Builder, Substitution>(sigma, innermost);
}

template <template <class> class Builder, class Substitution>
struct substitute_data_expressions_builder: public Builder<substitute_data_expressions_builder<Builder, Substitution> >
{
  typedef Builder<substitute_data_expressions_builder<Builder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  Substitution sigma;
  bool innermost;

  substitute_data_expressions_builder(Substitution sigma_, bool innermost_)
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
substitute_data_expressions_builder<Builder, Substitution>
make_replace_data_expressions_builder(Substitution sigma, bool innermost)
{
  return substitute_data_expressions_builder<Builder, Substitution>(sigma, innermost);
}

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
struct substitute_free_variables_builder: public Binder<Builder, substitute_free_variables_builder<Builder, Binder, Substitution> >
{
  typedef Binder<Builder, substitute_free_variables_builder<Builder, Binder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::is_bound;
  using super::bind_count;
  using super::increase_bind_count;

  Substitution sigma;

  substitute_free_variables_builder(Substitution sigma_)
    : sigma(sigma_)
  {}

  template <typename VariableContainer>
  substitute_free_variables_builder(Substitution sigma_, const VariableContainer& bound_variables)
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
substitute_free_variables_builder<Builder, Binder, Substitution>
make_replace_free_variables_builder(Substitution sigma)
{
  return substitute_free_variables_builder<Builder, Binder, Substitution>(sigma);
}

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution, class VariableContainer>
substitute_free_variables_builder<Builder, Binder, Substitution>
make_replace_free_variables_builder(Substitution sigma, const VariableContainer& bound_variables)
{
  return substitute_free_variables_builder<Builder, Binder, Substitution>(sigma, bound_variables);
}
/// \endcond

} // namespace detail

//--- start generated data replace code ---//
template <typename T, typename Substitution>
void replace_sort_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_sort_expressions_builder<data::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_sort_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_sort_expressions_builder<data::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_data_expressions(T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
{
  data::detail::make_replace_data_expressions_builder<data::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T replace_data_expressions(const T& x,
                           Substitution sigma,
                           bool innermost,
                           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                          )
{
  return data::detail::make_replace_data_expressions_builder<data::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void replace_variables(T& x,
                       Substitution sigma,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                      )
{
  core::make_update_apply_builder<data::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_variables(const T& x,
                    Substitution sigma,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                   )
{
  return core::make_update_apply_builder<data::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void replace_free_variables(T& x,
                            Substitution sigma,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution, typename VariableContainer>
void replace_free_variables(T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                           )
{
  data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x, bound_variables);
}

template <typename T, typename Substitution, typename VariableContainer>
T replace_free_variables(const T& x,
                         Substitution sigma,
                         const VariableContainer& bound_variables,
                         typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                        )
{
  return data::detail::make_replace_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x, bound_variables);
}
//--- end generated data replace code ---//

template <typename T, typename Substitution>
void substitute_sorts(T& x,
                      Substitution sigma,
                      typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                     )
{
  core::make_update_apply_builder<data::sort_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T substitute_sorts(const T& x,
                   Substitution sigma,
                   typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                  )
{
  return core::make_update_apply_builder<data::sort_expression_builder>(sigma)(x);
}

namespace detail {

template <typename Substitution, typename IdentifierGenerator>
data::variable_list update_substitution(Substitution& sigma, const data::variable_list& v, const std::set<data::variable>& V, IdentifierGenerator& id_generator)
{
	atermpp::vector<data::variable> result;
  for (data::variable_list::const_iterator i = v.begin(); i != v.end(); ++i)
  {
  	if (V.find(*i) == V.end() && sigma(*i) == *i)
    {
    	result.push_back(*i);
    }
    else
    {
    	id_generator.add_identifier(i->name());
    	data::variable w(id_generator(i->name()), i->sort());

      while (sigma(w) != w || V.find(w) != V.end())
      {
      	w = data::variable(id_generator(i->name()), i->sort());
      }

      sigma[*i] = w;
      result.push_back(w);
    }
  }
  return data::variable_list(result.begin(), result.end());
}

template <template <class> class Builder, class Substitution>
struct replace_variables_capture_avoiding_builder: public Builder<replace_variables_capture_avoiding_builder<Builder, Substitution> >
{
  typedef Builder<replace_variables_capture_avoiding_builder<Builder, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  Substitution& sigma;
  std::set<data::variable>& V;
  data::set_identifier_generator id_generator;

  replace_variables_capture_avoiding_builder(Substitution& sigma_, std::set<data::variable>& V_)
    : sigma(sigma_), V(V_)
  { }

  data_expression operator()(const variable& v)
  {
    return sigma(v);
  }

  data_expression operator()(const data::where_clause& x)
  {
  	// TODO
  	throw mcrl2::runtime_error("not implemented yet");
  	return data_expression();
  }

  data_expression operator()(const data::forall& x)
  {
    data::variable_list v = update_substitution(sigma, x.variables(), V, id_generator);
    V.insert(v.begin(), v.end());
    return data::forall(v, (*this)(x.body()));
  }

  data_expression operator()(const data::exists& x)
  {
    data::variable_list v = update_substitution(sigma, x.variables(), V, id_generator);
    V.insert(v.begin(), v.end());
    return data::forall(v, (*this)(x.body()));
  }

  data_expression operator()(const data::lambda& x)
  {
    data::variable_list v = update_substitution(sigma, x.variables(), V, id_generator);
    V.insert(v.begin(), v.end());
    return data::forall(v, (*this)(x.body()));
  }

  data_expression operator()(const data::assignment& x)
  {
  	// TODO
  	throw mcrl2::runtime_error("not implemented yet");
  	return data_expression();
  }

  void operator()(data::data_equation& x)
  {
  	// TODO
  	throw mcrl2::runtime_error("not implemented yet");
  }

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, class Substitution>
replace_variables_capture_avoiding_builder<Builder, Substitution>
make_replace_variables_capture_avoiding_builder(Substitution& sigma, std::set<data::variable>& V)
{
  return replace_variables_capture_avoiding_builder<Builder, Substitution>(sigma, V);
}

} // namespace detail

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma_variables contains the free variables appearing in the right hand side of sigma
template <typename T, typename Substitution, typename VariableContainer>
void replace_variables_capture_avoiding(T& x,
                       Substitution& sigma,
                       const VariableContainer& sigma_variables,
                       typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                      )
{
	std::set<data::variable> V = data::find_free_variables(x);
  V.insert(sigma_variables.begin(), sigma_variables.end());
  data::detail::make_replace_variables_capture_avoiding_builder<data::data_expression_builder>(sigma, V)(x);
}

/// \brief Applies sigma as a capture avoiding substitution to x
/// \param sigma_variables contains the free variables appearing in the right hand side of sigma
template <typename T, typename Substitution, typename VariableContainer>
T replace_variables_capture_avoiding(const T& x,
                    Substitution& sigma,
                    const VariableContainer& sigma_variables,
                    typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                   )
{
	std::set<data::variable> V = data::find_free_variables(x);
  V.insert(sigma_variables.begin(), sigma_variables.end());
  return data::detail::make_replace_variables_capture_avoiding_builder<data::data_expression_builder>(sigma, V)(x);
}

} // namespace data

} // namespace mcrl2

#ifndef MCRL2_DATA_SUBSTITUTIONS_H
#include "mcrl2/data/substitutions.h"
#endif

#endif // MCRL2_DATA_REPLACE_H
