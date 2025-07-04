// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/mutable_substitution_composer.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_MUTABLE_SUBSTITUTION_COMPOSER_H
#define MCRL2_DATA_SUBSTITUTIONS_MUTABLE_SUBSTITUTION_COMPOSER_H

#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

namespace mcrl2::data
{

/// \brief An adapter that makes an arbitrary substitution function mutable.
template <typename Substitution>
class mutable_substitution_composer
{
  public:
    /// \brief type used to represent variables.
    using variable_type = typename Substitution::variable_type ;

    /// \brief type used to represent expressions.
    using expression_type = typename Substitution::expression_type;

    /// \brief Wrapper class for internal storage and substitution updates using operator().
    using assignment = typename mutable_map_substitution<std::map<variable_type, expression_type> >::assignment ;

    /// \brief The type of the wrapped substitution.
    using substitution_type = Substitution;

    using argument_type = variable_type;
    using result_type = expression_type;

  protected:
    /// \brief The wrapped substitution.
    const Substitution& f_;

    /// \brief An additional mutable substitution.
    mutable_map_substitution<std::map<variable_type, expression_type> > g_;

  public:
    /// \brief Constructor
    mutable_substitution_composer(const Substitution& f)
      : f_(f)
    {}

    /// \brief Apply on single single variable expression.
    /// \param[in] v the variable for which to give the associated expression.
    /// \return expression equivalent to s(e), or a reference to such an expression.
    expression_type operator()(variable_type const& v) const
    {
      return data::replace_free_variables(f_(v), g_);
    }

    assignment operator[](variable_type const& v)
    {
      return g_[v];
    }

    /// \brief Returns the wrapped substitution.
    /// \return The wrapped substitution.
    const substitution_type& substitution() const
    {
      return f_;
    }
};

/// \brief Specialization for mutable_map_substitution.
template <typename AssociativeContainer>
class mutable_substitution_composer<mutable_map_substitution<AssociativeContainer>>
{
  public:
    /// \brief The type of the wrapped substitution.
    using substitution_type = mutable_map_substitution<AssociativeContainer>;

    /// \brief type used to represent variables.
    using variable_type = typename substitution_type::variable_type;

    /// \brief type used to represent expressions.
    using expression_type = typename substitution_type::expression_type;

    /// \brief Wrapper class for internal storage and substitution updates using operator().
    using assignment = typename substitution_type::assignment;
    
    using argument_type = variable_type;
    using result_type = expression_type;

  protected:
    /// \brief object on which substitution manipulations are performed.
    substitution_type& g_;

  public:

    /// \brief Constructor with mutable substitution object.
    /// \param[in,out] g underlying substitution object.
    mutable_substitution_composer(mutable_map_substitution<AssociativeContainer>& g)
      : g_(g)
    {}

    /// \brief Apply on single single variable expression.
    /// \param[in] v the variable for which to give the associated expression.
    /// \return expression equivalent to s(e), or a reference to such an expression.
    const expression_type operator()(variable_type const& v) const
    {
      return g_(v);
    }

    assignment operator[](variable_type const& v)
    {
      return g_[v];
    }

    /// \brief Returns the wrapped substitution
    /// \return The wrapped substitution
    const substitution_type& substitution() const
    {
      return g_;
    }
};

/// \brief Specialization for mutable_indexed_substitution.
template <typename VariableType, typename ExpressionSequence>
class mutable_substitution_composer<mutable_indexed_substitution<VariableType, ExpressionSequence>>
{
  public:
    /// \brief The type of the wrapped substitution.
    using substitution_type = mutable_indexed_substitution<VariableType, ExpressionSequence>;

    /// \brief type used to represent variables.
    using variable_type = typename substitution_type::variable_type;

    /// \brief type used to represent expressions.
    using expression_type = typename substitution_type::expression_type;

    /// \brief Wrapper class for internal storage and substitution updates using operator().
    using assignment = typename substitution_type::assignment;

    using argument_type = variable_type;
    using result_type = expression_type;

  protected:
    /// \brief object on which substitution manipulations are performed.
    substitution_type& g_;

  public:

    /// \brief Constructor with mutable substitution object.
    /// \param[in,out] g underlying substitution object.
    mutable_substitution_composer(substitution_type& g)
      : g_(g)
    {}

    /// \brief Apply on single single variable expression.
    /// \param[in] v the variable for which to give the associated expression.
    /// \return expression equivalent to s(e), or a reference to such an expression.
    const expression_type operator()(variable_type const& v) const
    {
      return g_(v);
    }

    assignment operator[](variable_type const& v)
    {
      return g_[v];
    }

    /// \brief Returns the wrapped substitution.
    /// \return The wrapped substitution.
    const substitution_type& substitution() const
    {
      return g_;
    }
};

} // namespace mcrl2::data

#endif // MCRL2_DATA_SUBSTITUTIONS_MUTABLE_SUBSTITUTION_COMPOSER_H
