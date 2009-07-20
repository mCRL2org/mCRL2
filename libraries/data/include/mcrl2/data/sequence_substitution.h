// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sequence_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SEQUENCE_SUBSTITUTION_H__
#define MCRL2_DATA_SEQUENCE_SUBSTITUTION_H__

#include "boost/type_traits/remove_reference.hpp"
#include "boost/type_traits/add_reference.hpp"
#include "boost/type_traits/add_const.hpp"
#include "boost/iterator/iterator_facade.hpp"
#include "mcrl2/data/substitution.h"

namespace mcrl2 {

namespace data {

  template < typename VariableSequence,
               template < typename Substitution > class SubstitutionProcedure = structural_substitution >
  class sequence_substitution;

  template < typename VariableSequence, typename ExpressionSequence,
               template < typename Substitution > class SubstitutionProcedure = structural_substitution >
  class double_sequence_substitution;

  /// \cond INTERNAL_DOCS
  namespace detail {
    template < typename Assignment >
    struct assignment_helper;

    template < typename Variable, typename Expression>
    struct assignment_helper< std::pair< Variable, Expression > >
    {
      typedef Variable   left_type;
      typedef Expression right_type;

      static Variable const& left(std::pair< Variable, Expression > const& a)
      {
        return a.first;
      }

      static Expression const& right(std::pair< Variable, Expression > const& a)
      {
        return a.second;
      }
    };
  }
  /// \endcond

  /**
   * \brief Substitution that uses a single sequences of assignments
   *
   * Variables and expressions are associated by their position in each of the sequences.
   *  \arg[in] Sequence a sequence pairs of expressions and variables
   */
  template < typename Sequence, template < typename Substitution > class SubstitutionProcedure >
  class sequence_substitution : public substitution< sequence_substitution< Sequence, SubstitutionProcedure >,
               typename detail::assignment_helper< typename boost::remove_reference< Sequence >::type::value_type >::left_type,
               typename detail::assignment_helper< typename boost::remove_reference< Sequence >::type::value_type >::right_type,
	       SubstitutionProcedure >
  {
     template < typename D, typename V, typename E, template < typename Substitution > class P >
     friend class substitution;

     protected:

       Sequence m_assignments;

       typedef typename boost::remove_const< typename boost::remove_reference< Sequence >::type >::type sequence_type;

       typedef detail::assignment_helper< typename sequence_type::value_type >                          assignment_helper;

       /// \brief Applies the assignments to a variable v and returns the result.
       /// \param v A term
       /// \return The application of the assignments to the term.
       data_expression apply(variable const& v) const
       {
         for (typename sequence_type::const_iterator i = m_assignments.begin(); i != m_assignments.end(); ++i)
         {
           if (assignment_helper::left(*i) == v)
           {
             return assignment_helper::right(*i);
           }
         }
  
         return v;
       }

     public:

       sequence_substitution(Sequence assignments) : m_assignments(assignments)
       { }
  };

  /// \brief Factory function uses references to sequences
  template < typename Sequence >
  sequence_substitution< Sequence const& >
  make_sequence_substitution_adaptor(Sequence const& s)
  {
    return sequence_substitution< Sequence const& >(s);
  }

  /**
   * \brief Substitution that uses two sequences to represent replacements storing assignments
   *
   * Variables and expressions are associated by their position in each of the sequences.
   *  \arg[in] VariableSequence a sequence with variables
   *  \arg[in] ExpressionSequence a sequence with expressions
   *
   * Types must be such that substitution< double_sequence_substitution, V, E >
   * is a model of Substitution, where V = VariableSequence::value_type and
   * E = ExpressionSequence::value_type.
   */
  template < typename VariableSequence, typename ExpressionSequence, template < typename Substitution > class SubstitutionProcedure >
  class double_sequence_substitution :
      public substitution< double_sequence_substitution< VariableSequence, ExpressionSequence, SubstitutionProcedure >,
        typename boost::remove_reference< VariableSequence >::type::value_type,
        typename boost::remove_reference< ExpressionSequence >::type::value_type, SubstitutionProcedure >
  {
     template < typename D, typename V, typename E, template < typename Substitution > class P >
     friend class substitution;

      typedef typename boost::remove_reference< VariableSequence >::type   variable_sequence;
      typedef typename boost::remove_reference< ExpressionSequence >::type expression_sequence;

    public:

      /// \brief type used to represent variables
      typedef typename variable_sequence::value_type    variable_type;

      /// \brief type used to represent expressions
      typedef typename expression_sequence::value_type  expression_type;

      /// \brief type of assignments that are stored by this container
      typedef std::pair< typename variable_sequence::value_type,
                         typename expression_sequence::value_type > value_type;

      /// \brief Iterator type for constant element access
      class iterator :
             public boost::iterator_facade< iterator, value_type, boost::forward_traversal_tag, value_type >
      {
        friend class boost::iterator_core_access;

        private:

          typename variable_sequence::const_iterator   m_variable_iterator;
          typename expression_sequence::const_iterator m_expression_iterator;

          value_type dereference() const
          {
            return std::make_pair(*m_variable_iterator, *m_expression_iterator);
          }

          void increment()
          {
            ++m_variable_iterator;
            ++m_expression_iterator;
          }

          bool equal(iterator const& other) const
          {
            return m_variable_iterator == other.m_variable_iterator;
          }

        public:

          iterator(typename variable_sequence::const_iterator const& v,
		   typename expression_sequence::const_iterator const& e) : m_variable_iterator(v), m_expression_iterator(e)
          { }
      };

      /// \brief Iterator type for non-constant element access
      typedef iterator                                           const_iterator;

    protected:

      friend class substitution< double_sequence_substitution, variable_type, expression_type, SubstitutionProcedure >;

      /// \brief The left-hand side of the i-th assignment is represented by m_variables[i]
      VariableSequence   m_variables;

      /// \brief The right-hand side of the i-th assignment is represented by m_expressions[i]
      ExpressionSequence m_expressions;

    protected:

      //\brief Apply on single single variable expression
      /// \param[in] v the variable for which to give the associated expression
      /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
      expression_type apply(variable_type const& v) const
      {

        for (const_iterator i(begin()); i != end(); ++i)
        {
          if (i->first == v) {
            return i->second;
          }
        }
      
        return static_cast< expression_type const& >(v);
      }

      double_sequence_substitution()
      { }

    public:

      /// \brief Constructor
      //
      /// \param[in] variables a sequence of variables
      /// \param[in] expressions a sequence of expressions
      /// \pre variables.size() == expressions.size()
      double_sequence_substitution(typename boost::add_reference< VariableSequence >::type v,
                                   typename boost::add_reference< ExpressionSequence >::type e) : m_variables(v), m_expressions(e) {
        assert(v.size() == e.size());
      }

      /// \brief Returns an iterator pointing to the beginning of the sequence of assignments
      const_iterator begin() const {
        return const_iterator(m_variables.begin(), m_expressions.begin());
      }

      /// \brief Returns an iterator pointing past the end of the sequence of assignments
      const_iterator end() const {
        return const_iterator(m_variables.end(), m_expressions.end());
      }

      /// \brief Returns true if the sequence of assignments is empty
      bool empty() const {
        return m_variables.begin() == m_variables.end();
      }
  };

  /// \brief Factory function copies the sequences, useful when the input goes out of scope
  template < typename VariableSequence, typename ExpressionSequence >
  double_sequence_substitution< const VariableSequence, const ExpressionSequence >
  make_double_sequence_substitution(VariableSequence const& vc, ExpressionSequence const& ec)
  {
    return double_sequence_substitution< const VariableSequence, const ExpressionSequence >(vc, ec);
  }

  /// \brief Factory function uses references to sequences
  template < typename VariableSequence, typename ExpressionSequence >
  double_sequence_substitution< VariableSequence const&, ExpressionSequence const& >
  make_double_sequence_substitution_adaptor(VariableSequence const& vc, ExpressionSequence const& ec)
  {
    return double_sequence_substitution< VariableSequence const&, ExpressionSequence const& >(vc, ec);
  }

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_MAP_SUBSTITUTION_ADAPTER_H
