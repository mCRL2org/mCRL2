// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/enumerator.h
/// \brief The class enumerator.

#ifndef MCRL2_DATA_ENUMERATOR_H
#define MCRL2_DATA_ENUMERATOR_H

#include <utility>
#include <boost/shared_ptr.hpp>
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/sequence.h"
#include "mcrl2/data/enum.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/utilities/aterm_ext.h"

namespace mcrl2 {

namespace data {

/// \cond INTERNAL_DOCS

namespace detail {

  /// Helper class for enumerate_data_variable_substitutions function. Every time operator()
  /// is called, the result of applying the given sequence of substitutions to the single
  /// term rewriter is appended to the result.
  struct enumerate_substitutions_helper
  {
    const single_term_rewriter& m_rewriter;
    const atermpp::vector<rewriter::substitution>& m_substitutions;
    atermpp::set<data_expression>& m_result;
  
    enumerate_substitutions_helper(const single_term_rewriter& rewriter,
                                   const atermpp::vector<rewriter::substitution>& substitutions,
                                   atermpp::set<data_expression>& result
                                  )
     : m_rewriter(rewriter),
       m_substitutions(substitutions),
       m_result(result)
    {}
    
    void operator()()
    {
      m_result.insert(m_rewriter(m_substitutions.begin(), m_substitutions.end()));
    }
  };
  
  /// This function generates all possible sequences of substitutions
  /// [d1 := d1_, ..., dn := dn_] where [first, last[ = [d1 ... dn].
  /// Each sequence of substitutions is stored in the output range [i, i+n[,
  /// and for each sequence the function f is called.
  template <typename Iter, typename Function>
  void enumerate_substitutions(atermpp::vector<rewriter::substitution>::iterator i, Iter first, Iter last, Function f)
  {
    if (first == last)
    {
      f();
    }
    else
    {
      for (atermpp::vector<rewriter::substitution>::iterator j = first->begin(); j != first->end(); ++j)
      {
        *i = *j;
        enumerate_substitutions(i+1, first + 1, last, f);
      }
    }
  }

} // namespace detail

/// \endcond

class enumerator
{
  protected:
    rewriter& m_rewriter;
    boost::shared_ptr<Enumerator> m_enumerator;

    typedef std::map<data_variable, atermpp::vector<rewriter::substitution> > substition_map;

    /// Caches the ranges of values of data variables of finite sort.
    substition_map m_finite_sort_substitutions;

    /// Returns all possible values of the data variable d, by means of a sequence of
    /// substitutions [d := d1, d := d2, ..., d := dn). For efficiency, such sequences
    /// are cached.
    atermpp::vector<rewriter::substitution>& enumerate_variable(data_variable d)
    {
      substition_map::iterator i = m_finite_sort_substitutions.find(d);
      if (i != m_finite_sort_substitutions.end())
      {
        return i->second;
      }
      atermpp::vector<data_expression> v = enumerate_finite_sort(d.sort());
      atermpp::vector<rewriter::substitution> w;
      for (std::vector<data_expression>::iterator i = v.begin(); i != v.end(); ++i)
      {
        w.push_back(rewriter::substitution(m_rewriter, d, *i));
      }
      m_finite_sort_substitutions[d] = w;
      return m_finite_sort_substitutions[d];
    }

  public:
    enumerator(rewriter& rewr, const data_specification& data_spec)
      : m_rewriter(rewr),
        m_enumerator(createEnumerator(data_spec, m_rewriter.m_rewriter.get()))
    {
    }
    
    /// Returns all possible values of the finite sort s.
    atermpp::vector<data_expression> enumerate_finite_sort(data::sort_expression s)
    {
      atermpp::vector<data_expression> result;
      data_variable dummy(core::identifier_string("dummy"), s);

      // find all elements of sort s by enumerating all valuations of dummy
      // that make the expression "true" true
      EnumeratorSolutions* sols = m_enumerator.get()->findSolutions(atermpp::make_list(dummy), m_rewriter.m_rewriter.get()->toRewriteFormat(data_expr::true_()));
      ATermList l; // variable to store a solution
      while (sols->next(&l)) // get next solution
      {
        // l is of the form [subst(x,expr)] where expr is in rewriter format
        atermpp::aterm_appl tmp = utilities::ATAgetFirst(l);
        data_expression d(m_rewriter.m_rewriter.get()->fromRewriteFormat(tmp(1)));
        result.push_back(d);
      }     
      return result;
    }

    /// Returns all expressions of the form rewrite(phi([d1 := d1_, ..., dn := dn_])),
    /// where d1 ... dn are data variables of the sequence [first, last[, and where
    /// d1_ ... dn_ are data expressions that vary over all possible values of the
    /// variables d1 ... dn. The variables d1 ... dn must have finite sorts.
    ///
    template <typename Iter>
    atermpp::set<data_expression> enumerate_expression_values(const data_expression& phi, Iter first, Iter last)
    {
      single_term_rewriter r(m_rewriter, phi);
//      boost::ptr_vector<atermpp::vector<rewriter::substitution> > substitution_sequences; // this fails miserably...
      std::vector<atermpp::vector<rewriter::substitution> > substitution_sequences;
      for (Iter i = first; i != last; ++i)
      {
//        substitution_sequences.push_back(&(enumerate_variable(*i)));
        substitution_sequences.push_back(enumerate_variable(*i));
      }
      atermpp::vector<rewriter::substitution> substitutions(std::distance(first, last));
      atermpp::set<data_expression> result;
      detail::enumerate_substitutions(substitutions.begin(),
                                      substitution_sequences.begin(),
                                      substitution_sequences.end(),
                                      detail::enumerate_substitutions_helper(r, substitutions, result)
                                     );
      return result;
    }
};

/// A data expression that contains additional information for the enumerator.
class enumerator_expression
{
  protected:
    data_expression m_expression;
    data_variable_list m_variables;
  
  public:
    /// Constructor.
    enumerator_expression()
    {}
    
    /// Constructor.
    enumerator_expression(data_expression expression, data_variable_list variables)
      : m_expression(expression),  m_variables(variables)
    {}
    
    /// The contained data expression.
    data_expression expression() const
    {
      return m_expression;
    }
    
    /// The unbound variables of the contained expression.
    data_variable_list variables() const
    {
      return m_variables;
    }
    
    bool is_constant() const
    {
      return m_variables.empty();
    }
};

namespace detail {

  template <typename VariableContainer, typename EnumeratorExpressionContainer>
  struct data_enumerator_replace_helper
  {
    const VariableContainer& variables_;
    const EnumeratorExpressionContainer& replacements_;
    
    data_enumerator_replace_helper(const VariableContainer& variables,
                                   const EnumeratorExpressionContainer& replacements
                                  )
      : variables_(variables), replacements_(replacements)
    {
      assert(variables.size() == replacements.size());
    }
    
    data_expression operator()(data_variable t) const
    {
      typename VariableContainer::const_iterator i = variables_.begin();
      typename EnumeratorExpressionContainer::const_iterator j = replacements_.begin();
      for (; i != variables_.end(); ++i, ++j)
      {
        if (*i == t)
        {
          return j->expression();
        }
      }
      return t;
    }
  };

  struct data_enumerator_helper
  {
    const enumerator_expression& e_;
    const atermpp::vector<enumerator_expression>& values_;
    atermpp::vector<enumerator_expression>& result_;

    data_enumerator_helper(const enumerator_expression& e,
                           const atermpp::vector<enumerator_expression>& values,
                           atermpp::vector<enumerator_expression>& result
                          )
     : e_(e), values_(values), result_(result)
    {}
    
    void operator()()
    {
      data_expression d = replace_data_variables(e_.expression(), data_enumerator_replace_helper<data_variable_list, atermpp::vector<enumerator_expression> >(e_.variables(), values_));
      std::vector<data_variable> v;
      for (atermpp::vector<enumerator_expression>::const_iterator i = values_.begin(); i != values_.end(); ++i)
      {
        v.insert(v.end(), i->variables().begin(), i->variables().end());
      }       
      result_.push_back(enumerator_expression(d, data_variable_list(v.begin(), v.end())));
    }
  };

} // namespace detail

/// A class that enumerates data expressions.
template <typename DataRewriter, typename IdentifierGenerator>
class data_enumerator
{
  protected:
    typedef std::map<sort_expression, std::vector<data_operation> > constructor_map;

    const data_specification* m_data;
    DataRewriter* m_rewriter;
    IdentifierGenerator* m_generator;
    constructor_map m_constructors;

    /// Returns the constructors with target s.
    const std::vector<data_operation>& constructors(sort_expression s)
    {
      constructor_map::const_iterator i = m_constructors.find(s);
      if (i != m_constructors.end())
      {
        return i->second;
      }
      data_operation_list d = m_data->constructors(s);
      std::vector<data_operation> v(d.begin(), d.end());
      m_constructors[s] = v;
      return m_constructors[s];
    }

  public:
    /// Constructor.
    data_enumerator(const data_specification& data_spec,
                    DataRewriter& rewriter,
                    IdentifierGenerator& generator)
     : m_data(&data_spec), m_rewriter(&rewriter), m_generator(&generator)
    {}

    /// Enumerates a data variable.
    atermpp::vector<enumerator_expression> enumerate(const data_variable& v)
    {
      atermpp::vector<enumerator_expression> result;
      const std::vector<data_operation>& c = constructors(v.sort());

      for (std::vector<data_operation>::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        sort_expression_list dsorts = domain_sorts(i->sort());
        std::vector<data_variable> variables;
        for (sort_expression_list::iterator j = dsorts.begin(); j != dsorts.end(); ++j)
        {
          variables.push_back(data_variable((*m_generator)(), *j));
        }
        data_variable_list w(variables.begin(), variables.end());
        data_expression_list w1 = make_data_expression_list(w);
        result.push_back(enumerator_expression((*m_rewriter)((*i)(w1)), w));
      }

      return result;
    }

    /// Enumerates a data expression. Only the variables of the enumerator
    /// expression are expanded. Fresh variables are created using the
    /// identifier generator that was passed in the constructor.
    atermpp::vector<enumerator_expression> enumerate(const enumerator_expression& e)
    {
      atermpp::vector<enumerator_expression> result;

      // Compute the instantiations for each variable of e.
      std::vector<atermpp::vector<enumerator_expression> > enumerated_values;
      for (data_variable_list::iterator i = e.variables().begin(); i != e.variables().end(); ++i)
      {
        enumerated_values.push_back(enumerate(*i));     
      }
      
      atermpp::vector<enumerator_expression> values(enumerated_values.size());
      core::foreach_sequence(enumerated_values, values.begin(), detail::data_enumerator_helper(e, values, result));
      return result;
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ENUMERATOR_H
