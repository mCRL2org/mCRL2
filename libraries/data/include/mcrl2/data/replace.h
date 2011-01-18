// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/replace.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_DATA_REPLACE_H
#define MCRL2_DATA_REPLACE_H

#include "mcrl2/data/add_binding.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/data/detail/replace.h"

namespace mcrl2 {

  namespace data {

#ifndef MCRL2_NEW_REPLACE_VARIABLES
/// \brief Recursively traverses the given term, and applies the replace function to
/// each data variable that is encountered during the traversal.
/// \param[in] container a container with expressions (expression, or container of expressions)
/// \param[in] substitution A replace function
/// \note Binders are ignored with replacements
/// \return The replacement result
template < typename Container, typename Substitution >
Container replace_variables(Container const& container, Substitution substitution)
{
  return detail::variable_replace_helper< typename boost::add_reference< Substitution >::type >(substitution)(container);
}

/// \brief Recursively traverses the given expression or expression container,
/// and applies the replace function to each data variable that is not bound it
/// its context.
/// \param[in] container a container with expressions (expression, or container of expressions)
/// \param[in] substitution the function used for replacing variables
/// \pre for all v in find_free_variables(container) for all x in
/// find_free_variables(substitution(v)) v does not occur in a
/// context C[v] = container in which x is bound
/// \return The expression that results after replacement
template <typename Container, typename Substitution >
Container replace_free_variables(Container const& container, Substitution substitution)
{
  return detail::free_variable_replace_helper< typename boost::add_reference< Substitution >::type >(substitution)(container);
}

/// \brief Recursively traverses the given expression or expression container,
/// and applies the replace function to each data variable that is not bound it
/// its context.
/// \param[in] container a container with expressions (expression, or container of expressions)
/// \param[in] substitution the function used for replacing variables
/// \param[in] bound a set of variables that should be considered as bound
/// \pre for all v in find_free_variables(container) for all x in
/// find_free_variables(substitution(v)) v does not occur in a
/// context C[v] = container in which x is bound
/// \return The expression that results after replacement
/// TODO prevent bound from being copied
template <typename Container, typename Substitution , typename VariableSequence >
Container replace_free_variables(Container const& container, Substitution substitution, VariableSequence const& bound)
{
  return detail::free_variable_replace_helper< typename boost::add_reference< Substitution >::type >(bound, substitution)(container);
}

#else // MCRL2_NEW_REPLACE_VARIABLES

namespace detail {

  template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution>
  struct replace_free_variables_builder: public Binder<Builder, replace_free_variables_builder<Builder, Binder, Substitution> >
  {
    typedef Binder<Builder, replace_free_variables_builder<Builder, Binder, Substitution> > super; 
    using super::enter;
    using super::leave;
    using super::operator();
    using super::is_bound;
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

} // namespace detail

#endif // MCRL2_NEW_REPLACE_VARIABLES

//--- start generated data replace code ---//
#ifdef MCRL2_NEW_REPLACE_VARIABLES
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
#endif MCRL2_NEW_REPLACE_VARIABLES
//--- end generated data replace code ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REPLACE_H
