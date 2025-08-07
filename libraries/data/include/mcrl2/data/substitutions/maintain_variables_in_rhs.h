// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/maintain_variables_in_rhs.h
/// \brief This class extends a substitution to recall the variables that occur in its right hand side. 
///        It assumes the presence of an operation sigma[x]=v to set values of a variable. 
///        If sigma[x]=x, this is seen as clearing the variable, and x is not recalled. 
///        The variables recalled are an overapproximation. By regular garbage collection the 


#ifndef MCRL2_DATA_SUBSTITUTIONS_MAINTAIN_VARIABLES_IN_RHS_H
#define MCRL2_DATA_SUBSTITUTIONS_MAINTAIN_VARIABLES_IN_RHS_H

#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"

namespace mcrl2::data
{

/// \brief Wrapper that extends any substitution to a substitution maintaining the vars in its rhs. 
/// \details This substitution assumes a function variable -> std::size_t, that, for
///          each variable gives a unique index. The substitutions are stored
///          internally as a vector, mapping std::size_t to expression.
///          Provided that, given a variable, its index can be computed in O(1)
///          time, insertion is O(1) amortized, and lookup is O(1).
///          Memory required is O(n) where n is the largest index used.
template <typename Substitution>
class maintain_variables_in_rhs: public Substitution
{
  public:
    using super = Substitution;
    using variable_type = typename super::variable_type;
    using expression_type = typename super::expression_type;

  protected:
    std::multiset<variable_type> m_variables_in_rhs;
    std::set<variable_type> m_scratch_set;
  
  public:
    /// \brief Default constructor
    maintain_variables_in_rhs() = default;

    /// \brief Wrapper class for internal storage and substitution updates using operator()
    class assignment
    {
      protected: 
        const variable_type& m_variable;
        Substitution& m_sigma; 
        std::multiset<variable_type>& m_variables_in_rhs;
        std::set<variable_type>& m_scratch_set;
  
      public:
        /// \brief Constructor.
        /// \param[in] v A reference to the variable.
        /// \param[in] sigma A reference to the substitution.
        /// \param[in] variables_in_rhs Variables in the rhs of the assignments.
        /// \param[in] scratch_set A set used for locally for computations.
        assignment(const variable_type& v,
                   Substitution& sigma,
                   std::multiset<variable_type>& variables_in_rhs,
                   std::set<variable_type>& scratch_set) :
          m_variable(v),
          m_sigma(sigma),
          m_variables_in_rhs(variables_in_rhs),
          m_scratch_set(scratch_set)
        { }
  
        /// \brief Actual assignment
        void operator=(const expression_type& e)
        {
          assert(e.defined());

          const expression_type& e_old=m_sigma(m_variable);
          if (e_old!=m_variable)
          {
            // Remove the free variables in the old rhs.
            m_scratch_set=find_free_variables(e_old);
            for(const variable_type& v: m_scratch_set) 
            {
              m_variables_in_rhs.erase(v);
            }
            m_scratch_set.clear();
          }
          if (e != m_variable)
          {
            // Add the free variables in the new rhs.
            m_scratch_set=find_free_variables(e);
            m_variables_in_rhs.insert(m_scratch_set.begin(),m_scratch_set.end());
            m_scratch_set.clear();
          }
          // Set the new variable;
          m_sigma.operator[](m_variable)=e; 
        }
    };
  
    /// \brief Assigment operator.
    /// \param v The variable to which the assignment is carried out. 
    assignment operator[](variable_type const& v)
    {
      return assignment(v, *this, m_variables_in_rhs, m_scratch_set);
    }
  
    /// \brief Clear substitutions.
    void clear()
    {
      m_variables_in_rhs().clear();
      super::clear();
    }
  
    /// \brief Provides a set of variables that occur in the right hand sides of the assignments.
    const std::multiset<variable>& variables_in_rhs()
    {
      return m_variables_in_rhs;
    }

    /// \brief Indicates whether a variable occurs in some rhs of this substitution. 
    /// \param v The variable for which occurrence in a rhs is checked.
    bool variable_occurs_in_a_rhs(const variable_type& v)
    {
      return m_variables_in_rhs.find(v)!=m_variables_in_rhs.end();
    }

};

} // namespace mcrl2::data

#endif // MCRL2_DATA_SUBSTITUTIONS_MAINTAIN_VARIABLES_IN_RHS_H
