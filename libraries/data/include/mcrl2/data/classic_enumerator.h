// Author(s): Jeroen van der Wulp, Jan Friso Groote
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

#include "boost/iterator/iterator_facade.hpp"

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/enum/standard.h"

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

template < typename REWRITER = rewriter, typename TERM = data_expression >
class classic_enumerator
{
  public:
    /// \brief The type of objects that represent substitutions
    typedef typename REWRITER::substitution_type substitution_type;
    /// \brief The type of objects that represent expressions
    typedef TERM expression_type;

    /// \brief A class to enumerate solutions for terms.
    /// \details Solutions are presented as data_expression_lists of the same length as
    ///          the list of variables for which a solution is sought. 
    class iterator :
        public boost::iterator_facade<
                 iterator,
                 const data_expression_list,
                 boost::forward_traversal_tag >
    {
      protected:

        typedef classic_enumerator < REWRITER > enclosing_classic_enumerator;
        enclosing_classic_enumerator *m_enclosing_enumerator;
        data_expression_list m_assignments; 
        bool m_enumerator_iterator_valid;
        TERM m_resulting_condition;
        bool m_exception_occurred;
        bool m_not_equal_to_false;
        variable_list enum_vars;                    // The variables over which a solution is searched.
        TERM enum_expr;                             // Condition to be satisfied.
        substitution_type* enum_sigma;
    
        std::deque < detail::fs_expr<TERM>> fs_stack;
    
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
                          const variable_list& variables,
                          const TERM& condition,
                          substitution_type& sigma,
                          const bool not_equal_to_false=true):
          m_enclosing_enumerator(e),
          m_not_equal_to_false(not_equal_to_false),
          enum_sigma(&sigma)
        {
          m_resulting_condition= (e->m_evaluator)(condition,sigma);
          if ((m_not_equal_to_false && m_resulting_condition==sort_bool::false_()) ||
              (!m_not_equal_to_false && m_resulting_condition==sort_bool::true_()))
          {
            // no solutions are found.
            m_exception_occurred=false;
            m_enumerator_iterator_valid=false;
          }
          else if (variables.empty())
          {
            // in this case we generate exactly one solution.
            m_enumerator_iterator_valid=true;
            m_exception_occurred=false;
          }
          else
          {
            // we must calculate the solutions.
            enum_vars=variables;
            enum_expr=condition;
            used_vars=0;
            max_vars=MAX_VARS_INIT;
            push_on_fs_stack_and_split_or(fs_stack,
                                enum_vars,
                                variable_list(),
                                data_expression_list(),
                                enum_expr,
                                data_expression_list(),
                                !m_not_equal_to_false);
            increment();
          }
        }

        /// \brief Constructor representing the end of an iterator.
        //  \details It is advisable to avoid its use, and use end instead.
        iterator():
           m_enumerator_iterator_valid(false),
           enum_sigma(&default_sigma())
        {
        }

        /// \brief Provides the last found solution, but only if a valid solution was found.
        ///        This is the initial condition to which the solution of the variables
        ///        has been applied, and which is rewritten by the rewriter.
        const TERM &resulting_condition() const
        {
          assert(m_enumerator_iterator_valid);
          return m_resulting_condition;
        } 

        /// \brief Indicate whether a problem occurred when enumerating solutions. 
        //  \details This indicator only works if the parameter throw_exceptions was not set.
        //           Otherwise an exception is thrown. If throw_exceptions was false, and
        //           this function returns false, then the iterator is equal to end(), but
        //           in this case not all possible solutions have been generated.
        bool exception_occurred() const
        {
          assert(!(m_exception_occurred&&m_enumerator_iterator_valid));
          return m_exception_occurred;
        }

      protected:

        friend class boost::iterator_core_access;

        void increment();

        bool equal(iterator const& other) const
        {
          /* Only check whether end of enumerator has been reached */
          return m_enumerator_iterator_valid==other.m_enumerator_iterator_valid;
        }

        const data_expression_list& dereference() const
        {
          assert(m_enumerator_iterator_valid);
          return m_assignments;
        }

        void reset();

        bool find_equality(const data_expression& T,
                           const mcrl2::data::variable_list& vars,
                           mcrl2::data::variable& v,
                           data_expression& e);

        void EliminateVars(detail::fs_expr<TERM>& e);

        data_expression_list build_solution(
                     const variable_list& vars,
                     const variable_list& substituted_vars,
                     const data_expression_list& exprs) const;

        data_expression_list build_solution2(
                     const variable_list& vars,
                     const variable_list& substituted_vars,
                     const data_expression_list& exprs) const;
        TERM add_negations(
                     const TERM& condition,
                     const data_expression_list& negation_term_list,
                     const bool negated) const;
        void push_on_fs_stack_and_split_or(
                     std::deque < detail::fs_expr<TERM> >& fs_stack,
                     const variable_list& var_list,
                     const variable_list& substituted_vars,
                     const data_expression_list& substitution_terms,
                     const TERM& condition,
                     const data_expression_list& negated_term_list,
                     const bool negated) const;
        void push_on_fs_stack_and_split_or_without_rewriting(
                     std::deque < detail::fs_expr<TERM> >& fs_stack,
                     const variable_list& var_list,
                     const variable_list& substituted_vars,
                     const data_expression_list& substitution_terms,
                     const TERM& condition,
                     const data_expression_list& negated_term_list,
                     const bool negated) const;
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
    iterator begin(const variable_list& variables,
                   const TERM& condition,
                   substitution_type& sigma,
                   const bool not_equal_to_false=true)
    {
      return iterator(this, variables, condition,sigma,not_equal_to_false);
    }


    /// \brief The standard end iterator to indicate the end of an iteration.
    const iterator& end() const
    {
      return default_end_iterator();
    }

  public:
    /** \brief Constructs a classic enumerator to
     * \param[in] specification A data specification containing the definitions of sorts
     * \param[in] evaluator A component that is used for evaluating conditions, generally an ordinary rewriter
     * \param[in]  max_internal_variables The maximum number of variables to be 
     *             used internally when generating solutions. If set to 0 an unbounded number
     *             of variables are allowed, and warnings are printed to warn for potentially
     *             unbounded loops.
     * \param[in] throw_exceptions A boolean indicating whether an exception can be thrown.
     *            if false, the function exception_occurred of the iterator can be used to indicate whether
     *            valid solutions are being generated. 
     **/
    classic_enumerator(const data_specification &specification,
                       const REWRITER& evaluator,
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

