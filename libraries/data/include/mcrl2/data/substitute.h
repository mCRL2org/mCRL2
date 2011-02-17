// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitute.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTE_H
#define MCRL2_DATA_SUBSTITUTE_H

#include "mcrl2/atermpp/map.h"
#include "mcrl2/data/add_binding.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/data/print.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

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
make_substitute_sort_expressions_builder(Substitution sigma, bool innermost)
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
make_substitute_data_expressions_builder(Substitution sigma, bool innermost)
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
make_substitute_free_variables_builder(Substitution sigma)
{
  return substitute_free_variables_builder<Builder, Binder, Substitution>(sigma);
}

template <template <class> class Builder, template <template <class> class, class> class Binder, class Substitution, class VariableContainer>
substitute_free_variables_builder<Builder, Binder, Substitution>
make_substitute_free_variables_builder(Substitution sigma, const VariableContainer& bound_variables)
{
  return substitute_free_variables_builder<Builder, Binder, Substitution>(sigma, bound_variables);
}

} // namespace detail

//--- start generated data substitute code ---//
template <typename T, typename Substitution>
void substitute_sort_expressions(T& x,
                                 Substitution sigma,
                                 bool innermost,
                                 typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                                )
{
  data::detail::make_substitute_sort_expressions_builder<data::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T substitute_sort_expressions(const T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
{
  return data::detail::make_substitute_sort_expressions_builder<data::sort_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void substitute_data_expressions(T& x,
                                 Substitution sigma,
                                 bool innermost,
                                 typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                                )
{
  data::detail::make_substitute_data_expressions_builder<data::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
T substitute_data_expressions(const T& x,
                              Substitution sigma,
                              bool innermost,
                              typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                             )
{
  return data::detail::make_substitute_data_expressions_builder<data::data_expression_builder>(sigma, innermost)(x);
}

template <typename T, typename Substitution>
void substitute_variables(T& x,
                          Substitution sigma,
                          typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                         )
{
  core::make_update_apply_builder<data::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
T substitute_variables(const T& x,
                       Substitution sigma,
                       typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                      )
{   
  return core::make_update_apply_builder<data::data_expression_builder>(sigma)(x);
}

template <typename T, typename Substitution>
void substitute_free_variables(T& x,
                               Substitution sigma,
                               typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                              )
{
  data::detail::make_substitute_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution>
T substitute_free_variables(const T& x,
                            Substitution sigma,
                            typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                           )
{
  return data::detail::make_substitute_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x);
}

template <typename T, typename Substitution, typename VariableContainer>
void substitute_free_variables(T& x,
                               Substitution sigma,
                               const VariableContainer& bound_variables,
                               typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                              )
{
  data::detail::make_substitute_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x, bound_variables);
}

template <typename T, typename Substitution, typename VariableContainer>
T substitute_free_variables(const T& x,
                            Substitution sigma,
                            const VariableContainer& bound_variables,
                            typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                           )
{
  return data::detail::make_substitute_free_variables_builder<data::data_expression_builder, data::add_data_variable_binding>(sigma)(x, bound_variables);
}
//--- end generated data substitute code ---//

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

struct sort_assignment: public std::unary_function<basic_sort, sort_expression>
{
  typedef basic_sort variable_type;
  typedef sort_expression expression_type;
  
  basic_sort lhs;
  sort_expression rhs;

  sort_assignment(const basic_sort& lhs_, const sort_expression& rhs_)
  : lhs(lhs_),
    rhs(rhs_)
  {}

  sort_expression operator()(const basic_sort& x)
  {
    if (x == lhs) {
      return rhs;
    }
    return x;
  }
};

struct sort_expression_assignment: public std::unary_function<sort_expression, sort_expression>
{
  typedef sort_expression variable_type;
  typedef sort_expression expression_type;
  
  sort_expression lhs;
  sort_expression rhs;

  sort_expression_assignment(const sort_expression& lhs_, const sort_expression& rhs_)
  : lhs(lhs_),
    rhs(rhs_)
  {}

  sort_expression operator()(const sort_expression& x)
  {
    if (x == lhs) {
      return rhs;
    }
    return x;
  }
};

struct data_expression_assignment: public std::unary_function<data_expression, data_expression>
{
  typedef data_expression variable_type;
  typedef data_expression expression_type;
  
  data_expression lhs;
  data_expression rhs;

  data_expression_assignment(const data_expression& lhs_, const data_expression& rhs_)
  : lhs(lhs_),
    rhs(rhs_)
  {}

  data_expression operator()(const data_expression& x)
  {
    if (x == lhs) {
      return rhs;
    }
    return x;
  }
};

struct assignment_sequence_substitution : public std::unary_function<variable, data_expression>
{
  typedef variable variable_type;
  typedef data_expression expression_type;

  assignment_list assignments;

  assignment_sequence_substitution(assignment_list assignments_)
    : assignments(assignments_)
  {}

  data_expression operator()(const variable& v) const
  {
    for (assignment_list::const_iterator i = assignments.begin(); i != assignments.end(); ++i)
    {
      if (i->lhs() == v)
      {
        return i->rhs();
      }
    }
    return v;
  }
};

template <typename VariableContainer, typename ExpressionContainer>
struct sequence_sequence_substitution: public std::unary_function<typename VariableContainer::value_type, typename ExpressionContainer::value_type>
{
  /// \brief type used to represent variables
  typedef typename VariableContainer::value_type variable_type;

  /// \brief type used to represent expressions
  typedef typename ExpressionContainer::value_type  expression_type;

  const VariableContainer& variables;
  const ExpressionContainer& expressions;

  sequence_sequence_substitution(const VariableContainer& variables_, const ExpressionContainer& expressions_)
    : variables(variables_),
      expressions(expressions_)
  {
    assert(variables.size() == expressions.size());
  }

  expression_type operator()(const variable_type& v) const
  {
    typename VariableContainer::const_iterator i = variables.begin();
    typename ExpressionContainer::const_iterator j = expressions.begin();

    for (; i != variables.end(); ++i, ++j)
    {
      if (*i == v)
      {
        return *j;
      }
    }
    return v;
  }

  template <typename Expression>
  expression_type operator()(const Expression& x) const
  {
    throw std::runtime_error("data::sequence_sequence_substitution::operator(const Expression&) is a deprecated interface!");
    return data_expression();
  }
};

template <typename VariableContainer, typename ExpressionContainer>
sequence_sequence_substitution<VariableContainer, ExpressionContainer>
make_sequence_sequence_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
{
  return sequence_sequence_substitution<VariableContainer, ExpressionContainer>(vc, ec);
}

template <typename Container>
struct pair_sequence_substitution: public std::unary_function<typename Container::value_type::first_type, typename Container::value_type::second_type>
{
  /// \brief type used to represent variables
  typedef typename Container::value_type::first_type variable_type;

  /// \brief type used to represent expressions
  typedef typename Container::value_type::second_type expression_type;

  const Container& container;

  pair_sequence_substitution(const Container& container_)
    : container(container_)
  {}

  expression_type operator()(const variable_type& v) const
  {
    for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    {
      if (i->first == v)
      {
        return i->second;
      }
    }
    return v;
  }

  template <typename Expression>
  expression_type operator()(const Expression& x) const
  {
    throw std::runtime_error("data::pair_sequence_substitution::operator(const Expression&) is a deprecated interface!");
    return data_expression();
  }
};

template <typename Container>
pair_sequence_substitution<Container>
make_pair_sequence_substitution(const Container& c)
{
  return pair_sequence_substitution<Container>(c);
}

template <typename AssociativeContainer>
struct associative_container_substitution : public std::unary_function<typename AssociativeContainer::key_type, typename AssociativeContainer::mapped_type>
{
  typedef typename AssociativeContainer::key_type variable_type;
  typedef typename AssociativeContainer::mapped_type expression_type;

  const AssociativeContainer& m_map;

  associative_container_substitution(const AssociativeContainer& m)
    : m_map(m)
  {}

  expression_type operator()(const variable_type& v) const
  {
    typename AssociativeContainer::const_iterator i = m_map.find(v);
    if (i == m_map.end())
    {
      return v;
    }
    else
    {
      return i->second;
    }
    // N.B. This does not work!
    // return i == m_map.end() ? v : i->second;
  }

  template <typename Expression>
  expression_type operator()(const Expression& x) const
  {
    throw std::runtime_error("data::associative_container_substitution::operator(const Expression&) is a deprecated interface!");
    return data_expression();
  }
};

template <typename AssociativeContainer>
associative_container_substitution<AssociativeContainer>
make_associative_container_substitution(const AssociativeContainer& m)
{
  return associative_container_substitution<AssociativeContainer>(m);
}

template <typename AssociativeContainer = atermpp::map<variable,data_expression> >
struct mutable_associative_container_substitution : public std::unary_function<typename AssociativeContainer::key_type, typename AssociativeContainer::mapped_type>
{
  typedef typename AssociativeContainer::key_type variable_type;
  typedef typename AssociativeContainer::mapped_type expression_type;
  typedef typename AssociativeContainer::const_iterator const_iterator;
  typedef typename AssociativeContainer::iterator iterator;

  AssociativeContainer m_map;

  /// \brief Wrapper class for internal storage and substitution updates using operator()
  struct assignment
  {
    typename AssociativeContainer::key_type m_variable;
    AssociativeContainer&                   m_map;

    /// \brief Constructor.
    ///
    /// \param[in] v a variable.
    /// \param[in] m a mapping of variables to expressions.
    assignment(typename AssociativeContainer::key_type v, AssociativeContainer& m) :
      m_variable(v), m_map(m)
    { }

    template <typename AssignableToExpression>
    void operator=(AssignableToExpression const& e)
    {
      if (e != m_variable)
      {
        m_map[m_variable] = e;
      }
      else
      {
        m_map.erase(m_variable);
      }
    }
  };

  mutable_associative_container_substitution()
  {}

  mutable_associative_container_substitution(const AssociativeContainer& m)
    : m_map(m)
  {}

  template <typename VariableContainer, typename ExpressionContainer>
  mutable_associative_container_substitution(VariableContainer const& vc, ExpressionContainer const& ec)
  {
    BOOST_ASSERT(vc.size() == ec.size());

    typename ExpressionContainer::const_iterator j = ec.begin();
    for (typename VariableContainer::const_iterator i = vc.begin(); i != vc.end(); ++i, ++j)
    {
      m_map[*i] = *j;
    }
  }

  expression_type operator()(const variable_type& v) const
  {
    typename AssociativeContainer::const_iterator i = m_map.find(v);
    if (i == m_map.end())
    {
      return v;
    }
    else
    {
      return i->second;
    }
    // N.B. This does not work!
    // return i == m_map.end() ? v : i->second;
  }

  template <typename Expression>
  expression_type operator()(const Expression& x) const
  {
    throw std::runtime_error("data::mutable_associative_container_substitution::operator(const Expression&) is a deprecated interface!");
    return data_expression();
  }

  assignment operator[](variable_type const& v)
  {
    return assignment(v, this->m_map);
  }

  /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
  const_iterator begin() const
  {
    return m_map.begin();
  }

  /// \brief Returns an iterator pointing past the end of the sequence of assignments
  const_iterator end() const
  {
    return m_map.end();
  }

  /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
  iterator begin()
  {
    return this->m_map.begin();
  }

  /// \brief Returns an iterator pointing past the end of the sequence of assignments
  iterator end()
  {
    return this->m_map.end();
  }

  /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
  iterator find(variable_type const& v)
  {
    return this->m_map.find(v);
  }

  /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
  const_iterator find(variable_type const& v) const
  {
    return m_map.find(v);
  }

  /// \brief Returns true if the substitution is empty
  bool empty()
  {
    return m_map.empty();
  }

  template <typename Substitution>
  bool operator==(const Substitution& other) const
  {
    return false;
  }

  // Needed by classic_enumerator
  bool operator==(const mutable_associative_container_substitution<AssociativeContainer>& other) const
  {
    return m_map == other.m_map;
  }
};

/// \brief Utility function for creating a associative_container_substitution_adapter.
template <typename VariableContainer, typename ExpressionContainer, typename MapContainer>
mutable_associative_container_substitution<MapContainer>
make_mutable_associative_container_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
{
  return mutable_associative_container_substitution<MapContainer>(vc, ec);
}

template <typename VariableContainer, typename ExpressionContainer>
mutable_associative_container_substitution<std::map<typename VariableContainer::value_type, typename ExpressionContainer::value_type> >
make_mutable_associative_container_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
{
  return mutable_associative_container_substitution<std::map<typename VariableContainer::value_type, typename ExpressionContainer::value_type> >(vc, ec);
}

/// \brief An adapter that makes an arbitrary substitution function mutable.
template <typename Substitution>
class mutable_substitution_composer: public std::unary_function<typename Substitution::variable_type, typename Substitution::expression_type>
{
  public:
    /// \brief type used to represent variables
    typedef typename Substitution::variable_type variable_type;

    /// \brief type used to represent expressions
    typedef typename Substitution::expression_type expression_type;

    /// \brief Wrapper class for internal storage and substitution updates using operator()
    typedef typename mutable_associative_container_substitution<atermpp::map<variable_type, expression_type> >::assignment assignment;

    /// \brief The type of the wrapped substitution
    typedef Substitution substitution_type;

  protected:
    /// \brief The wrapped substitution
    const Substitution& f_;

    /// \brief An additional mutable substitution
    mutable_associative_container_substitution<atermpp::map<variable_type, expression_type> > g_;

  public:
    /// \brief Constructor
    mutable_substitution_composer(const Substitution& f)
      : f_(f)
    {}

    /// \brief Apply on single single variable expression
    /// \param[in] v the variable for which to give the associated expression
    /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
    expression_type operator()(variable_type const& v) const
    {
      return data::substitute_free_variables(f_(v), g_);
    }

    template <typename Expression>
    expression_type operator()(const Expression& x) const
    {
      throw std::runtime_error("data::mutable_substitution_composer::operator(const Expression&) is a deprecated interface!");
      return data_expression();
    }

    assignment operator[](variable_type const& v)
    {
      return g_[v];
    }

    /// \brief Returns the wrapped substitution
    /// \return The wrapped substitution
    const substitution_type& substitution() const
    {
      return f_;
    }
};

/// \brief Specialization for mutable_associative_container_substitution.
template <typename AssociativeContainer>
class mutable_substitution_composer<mutable_associative_container_substitution<AssociativeContainer> >: public std::unary_function<typename AssociativeContainer::key_type, typename AssociativeContainer::mapped_type>
{
  public:
    /// \brief The type of the wrapped substitution
    typedef mutable_associative_container_substitution<AssociativeContainer> substitution_type;

    /// \brief type used to represent variables
    typedef typename substitution_type::variable_type variable_type;

    /// \brief type used to represent expressions
    typedef typename substitution_type::expression_type expression_type;

    /// \brief Wrapper class for internal storage and substitution updates using operator()
    typedef typename substitution_type::assignment assignment;

  protected:
    /// \brief object on which substitution manipulations are performed
    mutable_associative_container_substitution<AssociativeContainer>& g_;

  public:

    /// \brief Constructor with mutable substitution object
    /// \param[in,out] g underlying substitution object
    mutable_substitution_composer(mutable_associative_container_substitution<AssociativeContainer>& g)
      : g_(g)
    {}

    /// \brief Apply on single single variable expression
    /// \param[in] v the variable for which to give the associated expression
    /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
    expression_type operator()(variable_type const& v) const
    {
      return g_(v);
    }

    template <typename Expression>
    expression_type operator()(const Expression& x) const
    {
      throw std::runtime_error("data::mutable_substitution_composer<mutable_associative_container_substitution<AssociativeContainer> >::operator(const Expression&) is a deprecated interface!");
      return data_expression();
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

/// \brief Returns a string representation of the map, for example [a := 3, b := true].
/// \param[in] sigma a constant reference to an object of a mutable_substitution_composer instance
/// \return A string representation of the map.
template <typename Substitution>
std::string to_string(const mutable_substitution_composer<Substitution>& sigma)
{
  return to_string(sigma.substitution());
}

/// \brief Returns a string representation of the map, for example [a := 3, b := true].
/// \param sigma a substitution.
/// \return A string representation of the map.
template <typename Substitution>
std::string to_string(const Substitution& sigma)
{
  std::stringstream result;
  result << "[";
  for (typename Substitution::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
  {
    result << (i == sigma.begin() ? "" : "; ") << data::pp(i->first) << ":" << data::pp(i->first.sort()) << " := " << data::pp(i->second);
  }
  result << "]";
  return result.str();
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SUBSTITUTE_H
