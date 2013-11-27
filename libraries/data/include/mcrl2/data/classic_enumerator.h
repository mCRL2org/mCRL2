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
#include "mcrl2/utilities/workarounds.h" // for nullptr on older compilers

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
///        By default the solutions are represented as substitutions.
///        The classic enumerator can also work with expressions
///        in internal rewrite format. In that case solutions are
///        aterm_list < aterm_appl >'s of the same length as the
///        list of variables.
template < typename Evaluator = rewriter >
class classic_enumerator
{

  public:
    /// \brief The type of objects that represent substitutions
    typedef typename Evaluator::substitution_type  substitution_type;
    typedef typename Evaluator::internal_substitution_type  internal_substitution_type;

    /// \brief The type of objects that represent variables
    typedef typename substitution_type::variable_type                     variable_type;
    /// \brief The type of objects that represent expressions
    typedef typename substitution_type::expression_type                   expression_type;
    /// \brief The type of objects that represent evaluator components
    typedef Evaluator                                                     evaluator_type;

  protected:
    const detail::legacy_rewriter                                m_evaluator;     // Only here for conversion trick
    detail::EnumeratorStandard                                   m_enumerator;    // The real enumeration is done in an EnumeratorStandard
                                                                                  // class.

  public:

    /// \brief A class to enumerate solutions for terms in internal format.
    /// \details Solutions are presented as aterm lists in internal format of the same length as
    ///          the list of variables for which a solution is sought.
    class iterator_internal :
        public boost::iterator_facade<
                 iterator_internal,
                 const data_expression_list,
                 boost::forward_traversal_tag >
    {
      protected:

        typedef classic_enumerator < evaluator_type > enclosing_classic_enumerator;
        enclosing_classic_enumerator *m_enclosing_enumerator;
        data_expression_list m_assignments; // m_assignments are only protected if it does contain something else than the empty list.
        bool m_enumerator_iterator_valid;
        bool m_solution_is_exact;
        bool m_solution_possible;
        typedef boost::shared_ptr < detail::EnumeratorSolutionsStandard> m_generator_type;
        m_generator_type m_generator;

      public:

        /// \brief Constructor. Use it via the begin_internal function of the classic enumerator class.
        iterator_internal(enclosing_classic_enumerator *e,
                          const variable_list &variables,
                          const data_expression &condition,
                          internal_substitution_type &sigma,
                          const bool not_equal_to_false=true,
                          const size_t max_internal_variables=0,
                          const bool do_not_throw_exceptions=false):
          m_enclosing_enumerator(e),
          m_enumerator_iterator_valid(false),
          m_solution_possible(do_not_throw_exceptions)
        {
          const data_expression rewritten_condition=e->m_evaluator.get_rewriter().rewrite(condition,sigma);
          if ((not_equal_to_false && rewritten_condition==e->m_evaluator.get_rewriter().internal_false) ||
              (!not_equal_to_false && rewritten_condition==e->m_evaluator.get_rewriter().internal_true))
          {
            // no solutions are found.
            m_solution_possible=true;
          }
          else if (variables.empty())
          {
            // in this case we generate exactly one solution.
            m_enumerator_iterator_valid=true;
            m_solution_possible=true;
            m_solution_is_exact=((not_equal_to_false && rewritten_condition==e->m_evaluator.get_rewriter().internal_true) ||
                                 (!not_equal_to_false && rewritten_condition==e->m_evaluator.get_rewriter().internal_false));
          }
          else
          {
            // we must calculate the solutions.
            m_generator=m_generator_type(new detail::EnumeratorSolutionsStandard(variables,
                                                              condition,
                                                              sigma,
                                                              not_equal_to_false,
                                                              &(m_enclosing_enumerator->m_enumerator),
                                                              max_internal_variables));
            increment();
          }
        }

        /// \brief Constructor representing the end of an internal_iterator.
        //  \details It is advisable to avoid its use, and use end_internal instead.
        iterator_internal():
           m_enumerator_iterator_valid(false),
           m_solution_possible(false)
        {
        }

        /// \brief Destructor.
        ~iterator_internal()
        {
          if (m_generator!=nullptr)
          {
            m_generator.reset();
          }
        }

        /// \brief Standard assignment operator.
        iterator_internal& operator=(const iterator_internal &other)
        {
          m_enclosing_enumerator=other.m_enclosing_enumerator;
          m_assignments=other.m_assignments;
          m_enumerator_iterator_valid=other.m_enumerator_iterator_valid;
          m_solution_is_exact=other.m_solution_is_exact;
          m_solution_possible=other.m_solution_possible;
          /* Code below appears to be without effect.
          if (m_generator==NULL && other.m_generator!=NULL)
          {
          }
          if (m_generator!=NULL && other.m_generator==NULL)
          {
          } */
          m_generator=other.m_generator;
          return *this;
        }

        /// \brief Standard copy constructor
        iterator_internal(const iterator_internal &other)
        {
          m_enclosing_enumerator=other.m_enclosing_enumerator;
          m_assignments=other.m_assignments;
          m_enumerator_iterator_valid=other.m_enumerator_iterator_valid;
          m_solution_is_exact=other.m_solution_is_exact;
          m_solution_possible=other.m_solution_possible;
          /* Code below appears to be without effect.
          if (m_generator==NULL && other.m_generator!=NULL)
          {
          }
          if (m_generator!=NULL && other.m_generator==NULL)
          {
          }
          */
          m_generator=other.m_generator;
        }

        /// \brief Indicate whether this enumerated value for the variables makes the
        //         condition exactly true if not_equal_to_false is true, or exactly false if not_equal_to_false
        //         is false.
        bool solution_is_exact() const
        {
          assert(m_enumerator_iterator_valid);
          return m_solution_is_exact;
        }

        /// \brief Indicate whether an solution was attempted to be generated (true) or whether
        //         an error occurred (false).
        //  \details This indicator only works if the variable do_not_throw_exceptions was true.
        //           Otherwise an exception is thrown. If do_not_throw_exceptions was true, and
        //           this function returns false, then the iterator is equal to end().
        bool solution_is_possible() const
        {
          assert(!m_enumerator_iterator_valid);
          return m_solution_possible;
        }

      protected:

        friend class boost::iterator_core_access;

        void increment()
        {
          if (m_generator==nullptr)
          {
            m_enumerator_iterator_valid=false; // There was only one solution.
          }
          else
          {
            data_expression instantiated_solution;
            m_enumerator_iterator_valid=m_generator->next(instantiated_solution,m_assignments,m_solution_possible);
            if (m_enumerator_iterator_valid)
            {
              m_solution_is_exact=instantiated_solution==m_enclosing_enumerator->m_evaluator.get_rewriter().internal_true;
            }
          }
        }

        bool equal(iterator_internal const& other) const
        {
          /* Only check whether end of enumerator has been reached */
          return m_enumerator_iterator_valid==other.m_enumerator_iterator_valid;
        }

        const data_expression_list& dereference() const
        {
          assert(m_enumerator_iterator_valid);
          return m_assignments;
        }
    };

    /// \brief An iterator that delivers values for variables to make a condition true.
    /// \details The condition and the solutions are in internal rewrite format.
    ///          Dereferencing a valid iterator yields a list of solutions of the
    ///          same length as variables.
    /// \param[in] variables The variables for which a solution is sought.
    /// \param[in] condition_in_internal_format The condition in internal format.
    /// \param[in] sigma A substitution in the internal format.
    /// \param[in] max_internal_variables The maximal number of internally generatable variables.
    ///            If zero, then there is no bound. If the bound is reached, generating
    ///            new solutions stops with or without an exception as desired.
    /// \param[in] not_equal_to_false Indicates whether solutions are generated that make
    ///            the condition not equal to false, or not equal to true. This last option is
    ///            for instance useful to generate the solutions for the universal quantifier.
    /// \param[in] do_not_throw_exceptions A boolean indicating whether an exception can be thrown.
    ///            if not, the function solution_is_possible can be used to indicate whether
    ///            valid solutions are being generated.
    iterator_internal begin_internal(const variable_list variables,
                                     const data_expression condition_in_internal_format,
                                     internal_substitution_type &sigma,
                                     const size_t max_internal_variables=0,
                                     const bool not_equal_to_false=true,
                                     const bool do_not_throw_exceptions=false)

    {
      return iterator_internal(this, variables, condition_in_internal_format,sigma,not_equal_to_false,max_internal_variables,do_not_throw_exceptions);
    }

    /// \brief The standard end iterator to indicate the end of an iteration.
    iterator_internal end_internal() const
    {
      return iterator_internal();
    }

  public:
    /// \brief An iterator class to iterate over values of variables satisfying a condition.
    /// \details The dereference operator of this iterator class yields a substitution that
    ///          contains a mapping from variables to values representing a solution.
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
        internal_substitution_type internal_sigma;
        detail::EnumeratorSolutionsStandard m_generator;

      public:

        /// \brief Constructor. Use it via the begin function of the classic enumerator class.
        template < typename Container >
        iterator(enclosing_classic_enumerator *e,
                 const Container &variables,
                 const expression_type &condition,
                 const substitution_type &sigma=substitution_type(),
                 const bool not_equal_to_false=true,
                 const size_t max_internal_variables=0,
                 const bool do_not_throw_exceptions=false,
                 typename atermpp::detail::enable_if_container< Container, variable >::type* = 0):
          m_enclosing_enumerator(e),
          m_enumerator_iterator_valid(false),
          m_vars(atermpp::convert<variable_list,Container>(variables)),
          m_solution_possible(do_not_throw_exceptions),
          internal_sigma(sigma),
          m_generator(m_vars,
                      condition,
                      internal_sigma,
                      not_equal_to_false,
                      &(m_enclosing_enumerator->m_enumerator),
                      max_internal_variables)
        {
          increment();
        }

        /// \brief Constructor representing the end of an internal_iterator.
        //  \details It is advisable to avoid its use, and use method end of the classic enumerator instead.
        iterator():
          m_enumerator_iterator_valid(false),
          m_solution_possible(false)
        {
        }

        /// \brief Indicate whether this enumerated value for the variables makes the
        //         condition exactly true if not_equal_to_false is true, or exactly false if not_equal_to_false
        //         is false.
        bool solution_is_exact() const
        {
          assert(m_enumerator_iterator_valid);
          return m_solution_is_exact;
        }

        /// \brief Indicate whether an solution was attempted to be generated (true) or whether
        //         an error occurred (false).
        //  \details This indicator only works if the variable do_not_throw_exceptions was true.
        //           Otherwise an exception is thrown. If do_not_throw_exceptions was true, and
        //           this function returns false, then the iterator is equal to end().
        bool solution_is_possible() const
        {
          assert(!m_enumerator_iterator_valid);
          return m_solution_possible;
        }

      protected:

        friend class boost::iterator_core_access;

        void increment()
        {
          data_expression_list assignment_list;

          const bool b=m_solution_possible;
          data_expression instantiated_solution;
          if (m_generator.next(instantiated_solution,assignment_list,m_solution_possible) && b==m_solution_possible)
          {
            if (m_solution_possible)
            {
              m_solution_is_exact=instantiated_solution==m_enclosing_enumerator->m_evaluator.get_rewriter().internal_true;
            }
            m_enumerator_iterator_valid=true;
            variable_list::const_iterator j=m_vars.begin();
            for (data_expression_list::const_iterator i=assignment_list.begin();
                 i != assignment_list.end(); ++i,++j)
            {
              assert(static_cast< variable_type >(*j).sort() == i->sort());

              m_substitution[static_cast< variable_type >(*j)] = *i;
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

    /** \brief The standard begin function of an enumerating iterator
        \details The iterator enumerates solutions for the variables that make the
                 condition not false (by default) or if not_equal_to_false is false, solutions
                 are enumerated that does not make the condition true.
        \param[in] variables The variables for which solutions are sought
        \param[in] condition The condition which must or must not be satisfied
        \param[in] max_internal_variables The maximum number of internal variables to be generated.
                     If set to 0, there is no limit and enumerator may not terminate. Otherwise
                     the enumerator will terminate, provided the evaluator that is used terminates
                     always.
        \param[in] not_equal_to_false This variable indicates whether it is attempted to make
                     the condition not equal to false, or -- if false -- not equal to true. This
                     last option is useful to expand the universal quantifier. The default option
                     is useful for the sum operator or the existential quantifier.
        \param[in] do_not_throw_exceptions If set, exceptions are surpressed, but the function
                        solution_is_possible of the iterator can be used to find out whether
                        valid solutions are being generated, or whether a problem has occurred.
                        Basically, there are two possible problems, namely, there are no constructor
                        functions or the number of internally generated variables reached the limit.
        \param[in] substitution A substitution.

    **/
    template < typename Container >
    iterator begin(const Container &variables,
                   const expression_type &condition,
                   const size_t max_internal_variables=0,
                   const bool not_equal_to_false=true,
                   const bool do_not_throw_exceptions=false,
                   const substitution_type &substitution=substitution_type(),
                   typename atermpp::detail::enable_if_container< Container, variable >::type* = 0)
    {
      return iterator(this, variables, condition, substitution, not_equal_to_false, max_internal_variables, do_not_throw_exceptions);
    }

    /** \brief The standard end function of an enumerating iterator
    **/
    iterator end() const
    {
      return iterator();
    }

  public:
    /** \brief Constructs a classic enumerator to
     * \param[in] specification A data specification containing the definitions of sorts
     * \param[in] evaluator A component that is used for evaluating conditions, generally an ordinary rewriter
     **/
    classic_enumerator(const data_specification &specification,
                       const evaluator_type &evaluator):
      m_evaluator(evaluator),
      m_enumerator(detail::EnumeratorStandard(specification, &m_evaluator.get_rewriter()))
    {
    }

    /// \brief Standard destructor
    ~classic_enumerator()
    {
    }

    /// \brief Copy constructor
    classic_enumerator(classic_enumerator const& other):
      m_evaluator(other.m_evaluator),
      m_enumerator(other.m_enumerator)
    {
    }


    /// \brief Assignment operator
    classic_enumerator& operator=(const classic_enumerator & other)
    {
      m_evaluator=other.m_evaluator;
      m_enumerator=other.m_enumerator;
      return *this;
    }
};

} // namespace data
} // namespace mcrl2

#endif

