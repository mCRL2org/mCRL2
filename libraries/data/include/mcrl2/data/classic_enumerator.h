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

#include <set>

#include "boost/shared_ptr.hpp"
#include "boost/iterator/iterator_facade.hpp"

#include "mcrl2/atermpp/convert.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/rewriter_wrapper.h"
#include "mcrl2/data/detail/enum/standard.h"

namespace mcrl2
{
namespace data
{

template < typename Evaluator = rewriter >
class classic_enumerator;

/** \brief Specialised template class for generating data enumerator components
 *
 * A data enumerator represents a sequence of valuations (specified as a
 * substitution) for a given set of variables that satisfy a given
 * condition. The classic enumerator when constructed represents a
 * sequence of <em>all</em> possible evaluations that satisfy a condition.
 *
 * An classic object is an iterator over a possibly unbounded
 * sequence of valuations. Think of iteration as picking elements from a
 * stream. The iterator itself points to an element in the sequence and
 * represents the computation of the remainder of that sequence.
 *
 * As an example of what problems an enumerator can solve consider finding
 * all lists of even length smaller than 100, containing Natural numbers
 * smaller than 100 that are prime. Provided that this criterion can be
 * captured as an open boolean expression, and that the condition
 * evaluation process is complete (enough) and terminates.
 *
 * This type models concept Data Enumerator when the template arguments satisfy:
 *  \arg MutableSubstitution a model of the MutableSubstitution concept
 *  \arg Evaluator a model of Evaluator concept
 *
 * The following example shows a function that takes a data specification,
 * a data expression that represents a condition and a set of variables
 * that occur free in the condition.
 *
 * \code
 * void enumerate(data_specification const& d,
 *          std::set< variable > variables const& v,
 *                  data_expression const& c) {
 *
 *   using namespace mcrl2::data;
 *
 *   for (classic_enumerator< > i(d, variables, c); i!=enumerator_type(); ++i)
 *   {
 *     std::cerr << mcrl2::core::pp((*i)(c)) << std::endl;
 *   }
 * }
 * \endcode
 *
 * Besides the set of variables passed as argument the condition is allowed
 * to contain other free variables. The condition evaluation component
 * selects all expressions that cannot be reduced to false. This is
 * especially useful when an enumerator must be utilised from a context
 * where bound variables occur that cannot be eliminated from the
 * condition.
 *
 * In addition to <a href="http://tinyurl.com/9xtcmg">Single Pass Iterator</a> which is
 * required by the Enumerator concept, this type also models the
 * <a href="http://tinyurl.com/ayp9xb">Forward Traversal Iterator</a>
 * concept.  As a result the computation can be branched at any point as
 * illustrated by the following example (using the types from the previous
 * example).
 *
 * \code
 * void enumerate(data_specification const& d,
 *          std::set< variable > variables const& v,
 *                  data_expression const& c) {
 *
 *   enumerator_type i(d, variables, c);
 *
 *   for (enumerator_type j = i; i!=enumerator_type(); ++i, ++j)
 *   {
 *     assert(*i == *j);
 *   }
 * }
 * \endcode
 *
 **/
template < typename Evaluator >
class classic_enumerator 
{

  public:
    /// \brief The type of objects that represent substitutions
    typedef mcrl2::data::mutable_map_substitution< atermpp::map< data::variable, data_expression > >  substitution_type;
    /// \brief The type of objects that represent variables
    typedef typename substitution_type::variable_type                     variable_type;
    /// \brief The type of objects that represent expressions
    typedef typename substitution_type::expression_type                   expression_type;
    /// \brief The type of objects that represent evaluator components
    typedef Evaluator                                                     evaluator_type;

  private:

    const detail::legacy_rewriter                                m_evaluator;     // Only here for conversion trick
    detail::EnumeratorStandard*                                  m_enumerator;    // embedded rewriter should not be part of context

  public:
    class iterator_internal : 
        public boost::iterator_facade< 
                 iterator_internal,
                 const atermpp::term_list<atermpp::aterm_appl>,
                 boost::forward_traversal_tag >
    {
      protected:

        typedef classic_enumerator < evaluator_type > enclosing_classic_enumerator;
        enclosing_classic_enumerator *m_enclosing_enumerator;
        atermpp::term_list<atermpp::aterm_appl> m_assignments; // m_assignments are only protected if it does contain something else than the empty list.
        bool m_enumerator_iterator_valid;
        bool m_solution_is_exact;
        bool m_solution_possible;
        typedef boost::shared_ptr < detail::EnumeratorSolutionsStandard> m_generator_type;
        m_generator_type m_generator;

      public:
        
        iterator_internal(enclosing_classic_enumerator *e,
                          const variable_list &variables,
                          const atermpp::aterm_appl &condition,
                          const bool not_equal_to_false=true,
                          const size_t max_internal_variables=0,
                          const bool do_not_throw_exceptions=false):
          m_enclosing_enumerator(e),
          m_enumerator_iterator_valid(false),
          m_solution_possible(do_not_throw_exceptions)
        {
          const atermpp::aterm_appl rewritten_condition=e->m_evaluator.rewrite_internal(condition);
          if ((not_equal_to_false && rewritten_condition==e->m_evaluator.internal_false) ||
              (!not_equal_to_false && rewritten_condition==e->m_evaluator.internal_true))
          { 
            // no solutions are found.
            m_solution_possible=true;
          }
          else if (variables.empty())
          { 
            // in this case we generate exactly one solution.
            m_enumerator_iterator_valid=true;
            m_solution_possible=true;
            m_solution_is_exact=((not_equal_to_false && rewritten_condition==e->m_evaluator.internal_true) ||
                                 (!not_equal_to_false && rewritten_condition==e->m_evaluator.internal_false));
          }
          else 
          {
            // we must calculate the solutions.
            m_generator=m_generator_type(new detail::EnumeratorSolutionsStandard(variables,
                                                              condition,
                                                              not_equal_to_false,
                                                              m_enclosing_enumerator->m_enumerator,
                                                              max_internal_variables));
            m_assignments.protect();
            increment();
          }
        }

        iterator_internal():
           m_enumerator_iterator_valid(false),
           m_solution_possible(false)
        { 
        }

        ~iterator_internal()
        {
          if (m_generator!=NULL)
          {
            m_assignments.unprotect();
            m_generator.reset();
          }
        }

        iterator_internal& operator=(const iterator_internal &other)
        {
          m_enclosing_enumerator=other.m_enclosing_enumerator;
          m_assignments=other.m_assignments; 
          m_enumerator_iterator_valid=other.m_enumerator_iterator_valid;
          m_solution_is_exact=other.m_solution_is_exact;
          m_solution_possible=other.m_solution_possible;
          if (m_generator==NULL && other.m_generator!=NULL)
          { 
            m_assignments.protect();
          }
          if (m_generator!=NULL && other.m_generator==NULL)
          { 
            m_assignments.unprotect();
          }
          m_generator=other.m_generator;
          return *this;
        }

        iterator_internal(const iterator_internal &other)
        {
          m_enclosing_enumerator=other.m_enclosing_enumerator;
          m_assignments=other.m_assignments; 
          m_enumerator_iterator_valid=other.m_enumerator_iterator_valid;
          m_solution_is_exact=other.m_solution_is_exact;
          m_solution_possible=other.m_solution_possible;
          if (m_generator==NULL && other.m_generator!=NULL)
          { 
            m_assignments.protect();
          }
          if (m_generator!=NULL && other.m_generator==NULL)
          { 
            m_assignments.unprotect();
          }
          m_generator=other.m_generator;
        }

        bool solution_is_exact() const
        { 
          assert(m_enumerator_iterator_valid);
          return m_solution_is_exact;
        }

        bool solution_is_possible() const
        { 
          assert(!m_enumerator_iterator_valid);
          return m_solution_possible;
        }

      protected:
  
        friend class boost::iterator_core_access;
  
        void increment()
        {
          if (m_generator==NULL)
          { 
            m_enumerator_iterator_valid=false; // There was only one solution.
          }
          else 
          {
            m_enumerator_iterator_valid=m_generator->next(m_solution_is_exact,m_assignments,m_solution_possible);
          }
        }
    
        bool equal(iterator_internal const& other) const
        {
          /* Only check whether end of enumerator has been reached */
          return m_enumerator_iterator_valid==other.m_enumerator_iterator_valid;
        }
    
        const atermpp::term_list<atermpp::aterm_appl> & dereference() const
        {
          assert(m_enumerator_iterator_valid);
          return m_assignments;
        }
    };

    iterator_internal begin_internal(const variable_list &variables,
                                     const atermpp::aterm_appl &condition_in_internal_format,
                                     const size_t max_internal_variables=0,
                                     const bool not_equal_to_false=true,
                                     const bool do_not_throw_exceptions=false)

    { 
      return iterator_internal(this, variables, condition_in_internal_format,not_equal_to_false,max_internal_variables,do_not_throw_exceptions);
    }

    iterator_internal end_internal() const
    {
      return iterator_internal();
    }

  public:
    class iterator : 
        public boost::iterator_facade< 
                 iterator,
                 const substitution_type, 
                 boost::forward_traversal_tag >
    {
      protected:

        typedef classic_enumerator < evaluator_type > enclosing_classic_enumerator;

        enclosing_classic_enumerator *m_enclosing_enumerator;
        bool m_enumerator_iterator_valid;
        substitution_type m_substitution;
        variable_list m_vars;
        bool m_solution_is_exact;
        bool m_solution_possible;
        detail::EnumeratorSolutionsStandard m_generator;

      public:
        template < typename Container >
        iterator(enclosing_classic_enumerator *e,
                 const Container &variables,
                 const expression_type &condition,
                 const substitution_type &substitution=substitution_type(),
                 const bool not_equal_to_false=true,
                 const size_t max_internal_variables=0,
                 const bool do_not_throw_exceptions=false,
                 typename atermpp::detail::enable_if_container< Container, variable >::type* = 0):
          m_enclosing_enumerator(e),
          m_enumerator_iterator_valid(false),
          m_vars(atermpp::convert<variable_list,Container>(variables)),
          m_solution_possible(do_not_throw_exceptions),
          m_generator(m_vars,
                      m_enclosing_enumerator->m_evaluator.convert_to(condition),
                      not_equal_to_false,
                      m_enclosing_enumerator->m_enumerator,
                      max_internal_variables)
        { increment();
        }

        iterator():
          m_enumerator_iterator_valid(false),
          m_solution_possible(false)
          // m_generator(NULL)
        {
        }

        bool solution_is_exact() const
        { 
          assert(m_enumerator_iterator_valid);
          return m_solution_is_exact;
        }

        bool solution_is_possible() const
        { 
          assert(!m_enumerator_iterator_valid);
          return m_solution_possible;
        }

      protected:
  
        friend class boost::iterator_core_access;
  
        void increment()
        {
          atermpp::term_list <atermpp::aterm_appl> assignment_list;
    
          if (m_generator.next(m_solution_is_exact,assignment_list,m_solution_possible))
          {
            m_enumerator_iterator_valid=true;
            variable_list::const_iterator j=m_vars.begin();
            for (atermpp::term_list_iterator< atermpp::aterm_appl > i(assignment_list);
                 i != atermpp::term_list_iterator< atermpp::aterm_appl >(); ++i,++j)
            {
              assert(static_cast< variable_type >(*j).sort() == 
                              m_enclosing_enumerator->m_evaluator.convert_from(*i).sort());
    
              m_substitution[static_cast< variable_type >(*j)] =
                              data_expression(m_enclosing_enumerator->m_evaluator.convert_from(*i));
            }
          
          }
          else
          { 
            m_enumerator_iterator_valid=false;
          }
        }
    
        bool equal(iterator const& other) const
        {
          // Only check whether end of enumerator has been reached 
          return m_enumerator_iterator_valid==other.m_enumerator_iterator_valid;
        }
    
        substitution_type const& dereference() const
        {
          assert(m_enumerator_iterator_valid);
          return m_substitution;
        }
    };

    template < typename Container >
    iterator begin(const Container &variables,
                   const expression_type &condition,
                   const size_t max_internal_variables=0,
                   const bool not_equal_to_false=true,
                   const bool do_not_throw_exceptions=false,
                   const substitution_type &substitution=substitution_type(),
                   typename atermpp::detail::enable_if_container< Container, variable >::type* = 0)
    { 
      return iterator(this, variables, condition, substitution,not_equal_to_false,max_internal_variables,do_not_throw_exceptions);
    }

    iterator end() const
    {
      return iterator();
    }

  public: 
    /** \brief Constructs iterator representing a sequence of expressions
     *
     * \param[in] specification specification containing the definitions of sorts
     * \param[in] variables the set of variables for which to find valuatations
     * \param[in] condition the condition used for filtering generated substitutions
     * \param[in] evaluator component that is used for evaluating conditions
     * \param[in] substitution template for the substitution that is returned (default: empty substitution)
     **/
    classic_enumerator(const data_specification &specification,
                       const evaluator_type &evaluator):
      m_evaluator(evaluator),
      m_enumerator(new detail::EnumeratorStandard(specification, &m_evaluator.get_rewriter()))
    {
      // m_assignments.protect();
    } 

    ~classic_enumerator()
    {
      // m_assignments.unprotect();
    }
    


    /// \brief Copy constructor
    classic_enumerator(classic_enumerator const& other):
      m_evaluator(other.m_evaluator),
      m_enumerator(other.m_enumerator)
      // m_assignments(other.m_assignments)
    {
      // m_assignments.protect();
    }


    /// \brief Assignment operator constructor
    classic_enumerator& operator=(const classic_enumerator & other)
    {
      m_evaluator=other.m_evaluator; 
      m_enumerator=other.m_enumerator;
      // m_assignments=other.m_assignments;
      // m_assignments.unprotect();

      return *this;
    }
};

} // namespace data
} // namespace mcrl2

#endif

