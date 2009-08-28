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
#include "mcrl2/core/detail/struct.h" // for gsIsSortExpr
#include "mcrl2/data/detail/convert.h"

namespace mcrl2 {

  namespace data {

    /// \brief Returns true if the term t is a sort_expression
    /// \param t A term
    /// \return True if the term is a sort expression.
    inline
    bool is_sort_expression(atermpp::aterm_appl t)
    {
      return core::detail::gsIsSortId(t) || core::detail::gsIsSortArrow(t) ||
             core::detail::gsIsSortStruct(t) || core::detail::gsIsSortCons(t);
    }

    /// \brief sort expression.
    ///
    /// A sort expression can be any of:
    /// - basic sort
    /// - structured sort
    /// - container sort
    /// - function sort
    /// - alias
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
          return core::detail::gsIsSortId(*this);
        }

        /// \brief Returns true iff this expression is a structured sort.
        inline
        bool is_structured_sort() const
        {
          return core::detail::gsIsSortStruct(*this);
        }

        /// \brief Returns true iff this expression is a container sort.
        inline
        bool is_container_sort() const
        {
          return core::detail::gsIsSortCons(*this);
        }

        /// \brief Returns true iff this expression is a function sort.
        inline
        bool is_function_sort() const
        {
          return core::detail::gsIsSortArrow(*this);
        }

        /// \brief Returns true iff the expression represents a standard sort.
        inline
        bool is_system_defined() const
        {
          using namespace core::detail;

          return gsIsSortExprBool(*this) || gsIsSortExprReal(*this) ||
                 gsIsSortExprInt(*this) || gsIsSortExprNat(*this) ||
                 gsIsSortExprPos(*this) || is_container_sort() || is_structured_sort();
        }

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

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_EXPRESSION_H

