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

namespace mcrl2 {

namespace data {

namespace detail {

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

  template <typename VariableSequence, typename ExpressionSequence>
  struct sequence_sequence_substitution: public std::unary_function<typename VariableSequence::value_type, typename ExpressionSequence::value_type>
  {
    /// \brief type used to represent variables
    typedef typename VariableSequence::value_type variable_type;

    /// \brief type used to represent expressions
    typedef typename ExpressionSequence::value_type  expression_type;
 
    const VariableSequence& variables;
    const ExpressionSequence& expressions;
 
    sequence_sequence_substitution(const VariableSequence& variables_, const ExpressionSequence& expressions_)
      : variables(variables_),
        expressions(expressions_)
    {
      assert(variables.size() == expressions.size());
    }
    
    expression_type operator()(const variable_type& v) const
    {
      typename VariableSequence::const_iterator i = variables.begin();
      typename ExpressionSequence::const_iterator j = expressions.begin();
      
      for ( ; i != variables.end(); ++i, ++j)
      {
        if (*i == v)
        {
          return *j;
        }
      }
      return v;
    }
  };

  template <typename VariableSequence, typename ExpressionSequence>
  sequence_sequence_substitution<VariableSequence, ExpressionSequence>
  make_sequence_sequence_substitution(const VariableSequence& vc, const ExpressionSequence& ec)
  {
    return sequence_sequence_substitution<VariableSequence, ExpressionSequence>(vc, ec);
  }

  template <typename AssociativeContainer>
  struct associative_container_substitution : public std::unary_function<typename AssociativeContainer::key_type, typename AssociativeContainer::value_type>
  {
    typedef typename AssociativeContainer::key_type variable_type;
    typedef typename AssociativeContainer::value_type expression_type;
  
    const AssociativeContainer& m_map;

    associative_container_substitution(const AssociativeContainer& m)
      : m_map(m)
    {}

    expression_type operator()(const variable_type& v) const
    {
      typename AssociativeContainer::const_iterator i = m_map.find(v);
      return i == m_map.end() ? v : i->second;
    }
  };

  template <typename AssociativeContainer>
  associative_container_substitution<AssociativeContainer>
  make_associative_container_substitution(const AssociativeContainer& m)
  {
    return associative_container_substitution<AssociativeContainer>(m);
  }

  template <typename AssociativeContainer = atermpp::map<variable,data_expression> >
  struct mutable_associative_container_substitution : public std::unary_function<variable, data_expression>
  {
    typedef variable variable_type;
    typedef data_expression expression_type;
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
        if (e != m_variable) {
          m_map[m_variable] = e;
        }
        else {
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
    
    data_expression operator()(const variable& v) const
    {
      typename AssociativeContainer::const_iterator i = m_map.find(v);
      return i == m_map.end() ? v : i->second;
    }

    assignment operator[](variable_type const& v) {
      return assignment(v, this->m_map);
    }

    /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
    const_iterator begin() const {
      return m_map.begin();
    }

    /// \brief Returns an iterator pointing past the end of the sequence of assignments
    const_iterator end() const {
      return m_map.end();
    }

    /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
    iterator begin() {
      return this->m_map.begin();
    }

    /// \brief Returns an iterator pointing past the end of the sequence of assignments
    iterator end() {
      return this->m_map.end();
    }

    /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
    iterator find(variable_type const& v) {
      return this->m_map.find(v);
    }

    /// \brief Returns an iterator that references the expression associated with v or is equal to m_map.end()
    const_iterator find(variable_type const& v) const {
      return m_map.find(v);
    }
  };

  /// \brief Utility function for creating a associative_container_substitution_adapter.
  template <typename VariableContainer, typename ExpressionContainer, typename MapContainer>
  associative_container_substitution<MapContainer>
  make_associative_container_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
  {
    return associative_container_substitution<MapContainer>(vc, ec);
  }

  template <typename VariableContainer, typename ExpressionContainer>
  associative_container_substitution<std::map<typename VariableContainer::value_type, typename ExpressionContainer::value_type> >
  make_associative_container_substitution(const VariableContainer& vc, const ExpressionContainer& ec)
  {
    return associative_container_substitution<std::map<typename VariableContainer::value_type, typename ExpressionContainer::value_type> >(vc, ec);
  }

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SUBSTITUTE_H
