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

#include <boost/ptr_container/ptr_vector.hpp>
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/data/enum.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/data_specification.h"
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
    Enumerator* m_enumerator;
    rewriter m_rewriter;

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
        w.push_back(m_rewriter.make_substitution(d, *i));
      }
      m_finite_sort_substitutions[d] = w;
      return m_finite_sort_substitutions[d];
    }

  public:
    enumerator(const data_specification& data_spec)
      : m_rewriter(data_spec)
    {
      m_enumerator = createEnumerator(data_spec, m_rewriter.m_rewriter.get());
    }
    
    ~enumerator()
    {
      delete m_enumerator;
    }
    
    /// Returns all possible values of the finite sort s.
    atermpp::vector<data_expression> enumerate_finite_sort(data::sort_expression s)
    {
      atermpp::vector<data_expression> result;
      data_variable dummy(core::identifier_string("dummy"), s);

      // find all elements of sort s by enumerating all valuations of dummy
      // that make the expression "true" true
      EnumeratorSolutions* sols = m_enumerator->findSolutions(atermpp::make_list(dummy), m_rewriter.m_rewriter.get()->toRewriteFormat(data_expr::true_()));
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
//      boost::ptr_vector<atermpp::vector<rewriter::substitution> > substitution_sequences;
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

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ENUMERATOR_H
