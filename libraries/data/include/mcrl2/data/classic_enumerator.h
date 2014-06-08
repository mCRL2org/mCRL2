// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/classic_enumerator.h
/// \brief Template class for createing enumerator components

#ifndef _MCRL2_DATA_CLASSIC_ENUMERATOR__HPP_
#define _MCRL2_DATA_CLASSIC_ENUMERATOR__HPP_

// Constant used for printing progress messages
const size_t MAX_VARS_INIT=1000;  
const size_t MAX_VARS_FACTOR=5;

#include "boost/iterator/iterator_facade.hpp"

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/detail/enumerator_variable_limit.h"

namespace mcrl2
{
namespace data
{

/// \brief The classic enumerator can enumerate solutions for boolean expressions.
/// \details The classic enumerator enumerates solutions for variables that
///        make an expression not equal to false, or
///        not equal to true. For instance in x<2 with x:Nat, it will yield the
///        solutions 0 and 1 for x as these values make x<2 not equal
///        to false. In an expression forall x:Nat.cond(x) it is interesting to
///        find all solutions for x that do not make cond true. If a single
///        solution is found that makes cond false, then forall x:Nat.cond(x) can
///        be replaced by false. If a finite number of expressions t1,...,tn can be found
///        that neither makes cond(t_i) false nor true, forall x:Nat.cond(x) can
///        be replaced by cond(t1) && cond(t2) && ... && cond(tn).
///        By default the solutions are represented as data_expression_lists of the same
///        length as a variable list. For a variable list [v1,...,vn] and data_expression_list
///        [t1,...,tn] the data_expression ti is the solution for variable vi.

template < typename REWRITER = rewriter, 
           typename MutableIndexedSubstitution = data::mutable_indexed_substitution<>, 
           typename EnumeratorListElement = enumerator_list_element_with_substitution<typename REWRITER::term_type> >
class classic_enumerator
{
  public:
    /// \brief The type of objects that represent substitutions
    typedef MutableIndexedSubstitution substitution_type;
    
    /// \brief The type of objects that represent expressions
    typedef typename REWRITER::term_type expression_type;
    
    /// \brief A class to enumerate solutions for terms.
    /// \details Solutions are presented as data_expression_lists of the same length as
    ///          the list of variables for which a solution is sought.
    class iterator :
        public boost::iterator_facade<
                 iterator,
                 const EnumeratorListElement&,
                 boost::forward_traversal_tag >
    {
      protected:

        typedef classic_enumerator < REWRITER, MutableIndexedSubstitution, EnumeratorListElement > enclosing_classic_enumerator;
        enclosing_classic_enumerator *m_enclosing_enumerator;
        bool m_not_equal_to_false;
        variable_list enum_vars;                    // The variables over which a solution is searched, used for debug en exception messages.
        expression_type enum_expr;                  // Initial condition to be satisfied, used for debug and exception messages.
        substitution_type* enum_sigma;

        std::deque < EnumeratorListElement > fs_stack;

        size_t used_vars;
        size_t max_vars;

        substitution_type& default_sigma()
        {
          static substitution_type default_sigma;
          return default_sigma;
        }

        data_specification& default_data_spec()
        {
          static data_specification default_data_spec;
          return default_data_spec;
        }

      public:

        /// \brief Constructor.
        //  \details Use it via begin() of the classic enumerator class. See the
        //           explanation at this function for the meaning of the parameters.
        iterator(enclosing_classic_enumerator *e,
                          const EnumeratorListElement& partial_solution,
                          substitution_type& sigma,
                          const bool not_equal_to_false=true):
          m_enclosing_enumerator(e),
          m_not_equal_to_false(not_equal_to_false),
          enum_vars(partial_solution.variables()),
          enum_expr(partial_solution.expression()),
          enum_sigma(&sigma)
        {
          const data_expression condition= (e->m_evaluator)(partial_solution.expression(),sigma);
          if ((m_not_equal_to_false && condition==sort_bool::false_()) ||
              (!m_not_equal_to_false && condition==sort_bool::true_()))
          {
            // no solutions are found.
          }
          else if (partial_solution.variables().empty())
          {
            // in this case we generate exactly one solution.
            fs_stack.emplace_back(partial_solution.variables(),condition);
          }
          else
          {
            used_vars=0;
            max_vars=MAX_VARS_INIT;
            push_on_fs_stack_and_split_or_without_rewriting(
                                          EnumeratorListElement(partial_solution.variables(),condition),
                                          data_expression_list(),
                                          !m_not_equal_to_false);
            find_next_solution(false);
          }
        }

        /// \brief Constructor representing the end of an iterator.
        //  \details It is advisable to avoid its use, and use end instead.
        iterator():
           enum_sigma(&default_sigma())
        {
        }

      protected:

        friend class boost::iterator_core_access;

        void increment()
        {
          find_next_solution(true);
        } 

        bool equal(iterator const& other) const
        {
          /* Only check whether end of enumerator has been reached */
          return fs_stack.size()==other.fs_stack.size();
        }

        const EnumeratorListElement& dereference() const
        {
          assert(fs_stack.size()>0);
          assert(fs_stack.front().is_valid());
          return fs_stack.front();
        }

        void find_next_solution(const bool pop_front_of_stack);

        bool find_equality(const data_expression& T,
                           const mcrl2::data::variable_list& vars,
                           mcrl2::data::variable& v,
                           data_expression& e);

        void EliminateVars(EnumeratorListElement& e);

        expression_type add_negations(
                     const expression_type& condition,
                     const data_expression_list& negation_term_list,
                     const bool negated) const;
        void push_on_fs_stack_and_split_or(
                     const EnumeratorListElement& partial_solution,
                     const data_expression_list& negated_term_list,
                     const bool negated);
        void push_on_fs_stack_and_split_or_without_rewriting(
                     const EnumeratorListElement& partial_solution,
                     const data_expression_list& negated_term_list,
                     const bool negated);
        data_expression_list negate(
                     const data_expression_list& l) const;

    };

    /// \brief An iterator that delivers solutions for variables to satisfy a condition.
    /// \details The solutions that are sought are those that do not make the condition false, or
    ///           if not_equal_to_false is set to false, make the solution not equal to true.
    ///           The method solution_is_exact can be used to find out whether the solution is
    ///           exactly true or false.
    ///           Dereferencing a valid iterator yields a list of data_expressions solutions of the
    ///          same length as variables.
    /// \param[in] variables The variables for which a solution is sought.
    /// \param[in] condition The condition.
    /// \param[in] sigma A substitution.
    /// \param[in] not_equal_to_false Indicates whether solutions are generated that make
    ///            the condition not equal to false, or not equal to true. This last option is
    ///            for instance useful to generate the solutions for the universal quantifier.
    /// \param[in] max_internal_variables The maximal number of internally generatable variables.
    ///            If zero, then there is no bound. If the bound is reached, generating
    ///            new solutions stops with or without an exception as desired.
    iterator begin(substitution_type& sigma,
                   const EnumeratorListElement& p,
                   const bool not_equal_to_false=true)
    {
      return iterator(this, p, sigma, not_equal_to_false);
    }


    /// \brief The standard end iterator to indicate the end of an iteration.
    const iterator& end() const
    {
      return default_end_iterator();
    }

    /** \brief Constructs a classic enumerator to
     * \param[in] evaluator A component that is used for evaluating conditions, generally an ordinary rewriter
     * \param[in] specification A data specification containing the definitions of sorts
     * \param[in]  max_internal_variables The maximum number of variables to be
     *             used internally when generating solutions. If set to 0 an unbounded number
     *             of variables are allowed, and warnings are printed to warn for potentially
     *             unbounded loops.
     * \param[in] throw_exceptions A boolean indicating whether an exception can be thrown.
     *            if false, the function exception_occurred of the iterator can be used to indicate whether
     *            valid solutions are being generated.
     **/
    classic_enumerator(const REWRITER& evaluator,
                       const data_specification& specification,
                       const size_t max_internal_variables=0,
                       const bool throw_exceptions=true):
      m_evaluator(evaluator),
      m_data_spec(specification),
      m_max_internal_variables(max_internal_variables),
      m_throw_exceptions(throw_exceptions)
    {
    }

  protected:
    REWRITER m_evaluator;
    const mcrl2::data::data_specification& m_data_spec;
    const size_t m_max_internal_variables;
    const bool m_throw_exceptions;

    /// \brief A static end iterator, such that it does not need to be reconstructed each time end() is used.
    iterator& default_end_iterator() const
    {
      static iterator default_end_iterator;
      return default_end_iterator;
    }

};


} // namespace data
} // namespace mcrl2

#include "mcrl2/data/detail/enum/enumerator_implementation.h"

#endif

