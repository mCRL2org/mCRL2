// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_expression.h
/// \brief The class sort_expression.

#ifndef MCRL2_DATA_SORT_EXPRESSION_H
#define MCRL2_DATA_SORT_EXPRESSION_H

#include "boost/range/iterator_range.hpp"

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct_core.h" // for gsIsSortExpr
#include "mcrl2/data/detail/convert.h"

namespace mcrl2 {

  namespace data {

    /// \brief Returns true if the term t is a sort_expression
    /// \param t A term
    /// \return True if the term is a sort expression.
    inline
    bool is_sort_expression(atermpp::aterm_appl t)
    {
      return core::detail::gsIsSortExpr(t);
    }

    /// \brief Returns true if the term t is a basic sort
    inline bool is_basic_sort(atermpp::aterm_appl p) {
      return core::detail::gsIsSortId(p);
    }

    /// \brief Returns true if the term t is a function sort
    inline bool is_function_sort(atermpp::aterm_appl p) {
      return core::detail::gsIsSortArrow(p);
    }

    /// \brief Returns true if the term t is a container sort
    inline bool is_container_sort(atermpp::aterm_appl p) {
      return core::detail::gsIsSortCons(p);
    }

    /// \brief Returns true if the term t is a structured sort
    inline bool is_structured_sort(atermpp::aterm_appl p) {
      return core::detail::gsIsSortStruct(p);
    }

    /// \brief Returns true if the term t is the unknown sort
    inline bool is_unknown_sort(atermpp::aterm_appl p) {
      return core::detail::gsIsSortUnknown(p);
    }

    /// \brief Returns true if the term t is an expression for multiple possible sorts
    inline bool is_multiple_possible_sorts(atermpp::aterm_appl p) {
      return core::detail::gsIsSortsPossible(p);
    }

    /// \brief sort expression.
    ///
    /// A sort expression can be any of:
    /// - basic sort
    /// - structured sort
    /// - container sort
    /// - function sort
    /// - alias
    /// In the type checker also the following expressions can be used:
    /// - unknown sort
    /// - multiple possible sorts
    class sort_expression: public atermpp::aterm_appl
    {
      public:

        /// \brief Constructor.
        ///
        sort_expression()
          : atermpp::aterm_appl(core::detail::constructSortId())
        {}

        /// \brief Constructor.
        /// \param[in] t A term.
        /// \pre t has the internal structure of a sort expression.
        sort_expression(const atermpp::aterm_appl& t)
          : atermpp::aterm_appl(t)
        {
          assert(is_sort_expression(t));
        }

        /// \brief Returns true iff this expression is a basic sort.
        inline
        bool is_basic_sort() const
        {
          return data::is_basic_sort(*this);
        }

        /// \brief Returns true iff this expression is a structured sort.
        inline
        bool is_structured_sort() const
        {
          return data::is_structured_sort(*this);
        }

        /// \brief Returns true iff this expression is a container sort.
        inline
        bool is_container_sort() const
        {
          return data::is_container_sort(*this);
        }

        /// \brief Returns true iff this expression is a function sort.
        inline
        bool is_function_sort() const
        {
          return data::is_function_sort(*this);
        }

        /// \brief Returns true iff this expression is an unknown sort.
        inline
        bool is_unknown_sort() const
        {
          return data::is_unknown_sort(*this);
        }

        /// \brief Returns true iff this expression is an expression representing multiple possible sorts.
        inline
        bool is_multiple_possible_sorts() const
        {
          return data::is_multiple_possible_sorts(*this);
        }

        /// \brief Returns true iff the expression represents a standard sort.
        bool is_system_defined() const;

        /// \brief Returns the target sort of this expression.
        /// \return For a function sort D->E, return the target sort of E. Otherwise return this sort.
        inline
        sort_expression target_sort() const
        {
          if(is_function_sort())
          {
            return atermpp::arg2(*this);
          }
          else
          {
            return *this;
          }
        }

    }; // class sort_expression

    /// \brief list of sorts
    typedef atermpp::term_list< sort_expression >  sort_expression_list;
    /// \brief vector of sorts
    typedef atermpp::vector< sort_expression >     sort_expression_vector;

    /// \brief Converts an iterator range to sort_expression_list
    /// \param r A range of sort expressions.
    /// \note This function uses implementation details of the iterator type
    /// and hence is sometimes efficient than copying all elements of the list.
    template < typename ForwardTraversalIterator >
    inline sort_expression_list make_sort_expression_list(boost::iterator_range< ForwardTraversalIterator > const& r) {
      return convert< sort_expression_list >(r);
    }

    /// \brief Converts an iterator range to sort_expression_list
    /// \param r A range of sort expressions.
    template < typename ForwardTraversalIterator >
    inline sort_expression_vector make_sort_expression_vector(boost::iterator_range< ForwardTraversalIterator > const& r) {
      return convert< sort_expression_vector >(r);
    }

    /// \brief Converts a vector to a sort_expression_list
    /// \param r A range of sort expressions.
    template < typename Expression >
    inline sort_expression_list make_sort_expresion_list(atermpp::vector< Expression >const& r) {
      return convert< sort_expression_list >(r);
    }

    /// \brief Unknown sort.
    ///
    /// An unknown sort expresses a sort expression that represents the unknown
    /// sort expression.
    class unknown_sort: public sort_expression
    {
      /// \brief Default constructor for the unknown sort expression.
      /// \details This should only be used before and during type checking!
      public:
        unknown_sort()
          : sort_expression(mcrl2::core::detail::gsMakeSortUnknown())
        {}
     };

     /// \brief Multiple possible sorts.
     ///
     /// An expression that expresses that one of multiple sorts is possible.
     /// \details Only for use in the type checker!
     class multiple_possible_sorts: public sort_expression
     {
       public:
         /// \brief Default constructor. Denoting that no sorts are possible.
         multiple_possible_sorts()
          : sort_expression(mcrl2::core::detail::gsMakeSortsPossible(sort_expression_list()))
         {}

         /// \brief Constructor that denotes that the sorts in s are possible.
         /// \param s A container of possible sorts.
         template <typename Container>
         multiple_possible_sorts(Container const& s)
           : sort_expression(mcrl2::core::detail::gsMakeSortsPossible(convert<atermpp::aterm_list>(s)))
         {}
     };

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_EXPRESSION_H

