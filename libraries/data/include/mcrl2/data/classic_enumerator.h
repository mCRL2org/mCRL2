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

#include "boost/assert.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/iterator/iterator_facade.hpp"

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/enum/standard.h"
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
 * A shared context is kept between iterator objects to keep the cost of
 * copying low. As a consequence the iterator can be used in combination
 * with the Boost.Graph library.
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

    const data_specification &                                   m_specification;
    const detail::legacy_rewriter                                m_evaluator;     // Only here for conversion trick
    boost::shared_ptr<detail::EnumeratorStandard>                m_enumerator;    // embedded rewriter should not be part of context

    // for copy constructor, since it is unsafe to copy EnumeratorSolutionsStandard
    detail::EnumeratorSolutionsStandard                          *m_generator;
    bool                                                         m_not_equal_to_false;

  
  public:
    class iterator_internal : 
        public boost::iterator_facade< 
                 iterator_internal,
                 const ATermList,
                 boost::forward_traversal_tag >
    {
      private:

        typedef classic_enumerator < evaluator_type > enclosing_classic_enumerator;
        enclosing_classic_enumerator *m_enclosing_enumerator;
        bool m_enumerator_iterator_valid;
        atermpp::aterm_appl m_condition;
        ATermList m_assignments;

      public:
        
        iterator_internal(enclosing_classic_enumerator *e,
                          const variable_list &variables,
                          const ATermAppl &condition):
          m_enclosing_enumerator(e),
          m_enumerator_iterator_valid(false)
        {
          // Protect variables, condition and assignments?
          m_enclosing_enumerator->m_generator=
                 m_enclosing_enumerator->m_enumerator->findSolutions(
                                     (ATermList)variables,
                                     (ATerm)condition,
                                     m_enclosing_enumerator->m_not_equal_to_false,
                                     m_enclosing_enumerator->m_generator);

          increment();
        }

        iterator_internal():
           m_enumerator_iterator_valid(false)
        { 
        }

      private:
  
        friend class boost::iterator_core_access;
  
        // For past-end iterator: m_impl.get() == 0, for cheap iterator construction and comparison
        // boost::scoped_ptr< implementation_type >  m_impl;
  
        void increment()
        {
          m_enumerator_iterator_valid=m_enclosing_enumerator->m_generator->next(&m_assignments);
        }
    
        bool equal(iterator_internal const& other) const
        {
          /* Only check whether end of enumerator has been reached */
          return m_enumerator_iterator_valid==other.m_enumerator_iterator_valid;
        }
    
        ATermList const& dereference() const
        {
          return m_assignments;
        }
    };

    iterator_internal begin_internal(const variable_list &variables,
                                     const ATermAppl &condition_in_internal_format)
    { 
      return iterator_internal(this, variables, condition_in_internal_format);
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
      private:

        typedef classic_enumerator < evaluator_type > enclosing_classic_enumerator;

        enclosing_classic_enumerator *m_enclosing_enumerator;
        bool m_enumerator_iterator_valid;
        data_expression m_condition;
        substitution_type m_substitution;

      public:
        template < typename Container >
        iterator(enclosing_classic_enumerator *e,
                 const Container &variables,
                 const expression_type &condition,
                 const substitution_type &substitution=substitution_type(),
                 typename atermpp::detail::enable_if_container< Container, variable >::type* = 0):
          m_enclosing_enumerator(e),
          m_enumerator_iterator_valid(false)
        {
          ATermList vars=ATempty;
          for(typename Container::const_iterator i=variables.begin(); i!=variables.end(); ++i)
          { vars=ATinsert(vars,(ATerm)(ATermAppl)*i);
          } 

ATfprintf(stderr,"CONDITION %t\n",(ATermAppl)condition);
          m_enclosing_enumerator->m_generator=
                 m_enclosing_enumerator->m_enumerator->findSolutions(
                          ATreverse(vars),
                          m_enclosing_enumerator->m_evaluator.convert_to(condition),
                          m_enclosing_enumerator->m_not_equal_to_false,
                          m_enclosing_enumerator->m_generator);

          increment();
        }

        iterator():
          m_enumerator_iterator_valid(false)
        {
        }

      private:
  
        friend class boost::iterator_core_access;
  
        // For past-end iterator: m_impl.get() == 0, for cheap iterator construction and comparison
        // boost::scoped_ptr< implementation_type >  m_impl;
  
        void increment()
        {
          ATermList assignment_list;
    
          if (m_enclosing_enumerator->m_generator->next(&assignment_list))
          {
            m_enumerator_iterator_valid=true;
            for (atermpp::term_list_iterator< atermpp::aterm_appl > i(assignment_list);
                 i != atermpp::term_list_iterator< atermpp::aterm_appl >(); ++i)
            {
              assert(static_cast< variable_type >((*i)(0)).sort() == 
                              m_enclosing_enumerator->m_evaluator.convert_from((*i)(1)).sort());
    
              m_substitution[static_cast< variable_type >((*i)(0))] =
                              data_expression(m_enclosing_enumerator->m_evaluator.convert_from((*i)(1)));
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
          return m_substitution;
        }
    };

    template < typename Container >
    iterator begin(const Container &variables,
                   const expression_type &condition,
                   const substitution_type &substitution=substitution_type(),
                   typename atermpp::detail::enable_if_container< Container, variable >::type* = 0)
    { 
      return iterator(this, variables, condition, substitution);
    }

    iterator end() const
    {
      return iterator();
    } 
    
    /** \brief Constructs iterator representing a sequence of expressions
     *
     * \param[in] specification specification containing the definitions of sorts
     * \param[in] variables the set of variables for which to find valuatations
     * \param[in] condition the condition used for filtering generated substitutions
     * \param[in] evaluator component that is used for evaluating conditions
     * \param[in] substitution template for the substitution that is returned (default: empty substitution)
     **/
    classic_enumerator(const data_specification &specification,
                       const evaluator_type &evaluator,
                       const bool not_equal_to_false=true):
      m_specification(specification),
      m_evaluator(evaluator),
      m_enumerator(new detail::EnumeratorStandard(m_specification, &m_evaluator.get_rewriter())),
      m_generator(NULL),
      m_not_equal_to_false(not_equal_to_false)
    {
    } 

    ~classic_enumerator()
    {
      if (m_generator!=NULL)
      { 
        delete m_generator;
      }
    }
    


    /// \brief Copy constructor
    classic_enumerator(classic_enumerator const& other):
      m_specification(other.m_specification),
      m_evaluator(other.m_evaluator),
      m_enumerator(other.m_enumerator),
      m_generator(other.m_generator),
      m_not_equal_to_false(other.m_not_equal_to_false)
    {
    }


    /// \brief Assignment operator constructor
    classic_enumerator& operator=(const classic_enumerator & other)
    {
      m_specification=other.m_specification;
      m_evaluator=other.m_evaluator; 
      m_enumerator=other.m_enumerator;
      m_generator=other.m_generator;
      m_not_equal_to_false=other.m_not_equal_to_false;

      return *this;
    }

};

} // namespace data
} // namespace mcrl2

#endif

