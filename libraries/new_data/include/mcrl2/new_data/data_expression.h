// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/data_expression.h
/// \brief The class data_expression.

#ifndef MCRL2_NEW_DATA_DATA_EXPRESSION_H
#define MCRL2_NEW_DATA_DATA_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct.h" // for gsIsDataExpr
#include "mcrl2/new_data/sort_expression.h"
#include "mcrl2/new_data/function_sort.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief new_data expression.
    ///
    /// A new_data expression can be any of:
    /// - variable
    /// - function symbol
    /// - application
    /// - abstraction
    /// - where clause
    /// - set enumeration
    /// - bag enumeration
    class data_expression: public atermpp::aterm_appl
    {
      public:

        /// Constructor.
        ///
        data_expression()
          : atermpp::aterm_appl(core::detail::constructOpId())
        {}

        /// \internal
        data_expression(const atermpp::aterm_appl& t)
          : atermpp::aterm_appl(t)
        {}

        /// \brief Returns the sort of the new_data expression
        inline
        sort_expression sort() const
        {
          sort_expression result;

          // This implementation is currently done in this class, because there
          // is no elegant solution of distributing the implementation of the
          // derived classes (as we need to support requesting the sort of a
          // data_expression we do need to provide an implementation here).
          if (is_variable())
          {
            result = atermpp::arg2(*this);
          }
          else if (is_function_symbol())
          {
            result = atermpp::arg2(*this);
          }
          else if (is_abstraction())
          {
            atermpp::term_list<data_expression> v_variables = atermpp::list_arg2(*this);
            sort_expression_vector s;
            for(atermpp::term_list<data_expression>::const_iterator i = v_variables.begin() ; i != v_variables.end(); ++i)
            {
              s.push_back(i->sort());
            }
            result = function_sort(boost::make_iterator_range(s), data_expression(atermpp::arg3(*this)).sort());
          }
          else if (is_application())
          {
            sort_expression s(data_expression(atermpp::arg1(*this)).sort());
            assert(s.is_function_sort());
            result = atermpp::arg2(s);
          }
          else if (is_where_clause())
          {
            result = data_expression(atermpp::arg1(*this)).sort();
          }
          else {
            assert(false);
          }

          return result;
        }

        /// \brief Returns true iff the expression is variable
        inline
        bool is_variable() const
        {
          return core::detail::gsIsDataVarId(*this);
        }

        /// \brief Returns true iff the expression is a function symbol
        inline
        bool is_function_symbol() const
        {
          return core::detail::gsIsOpId(*this);
        }

        /// \brief Returns true iff the expression is an abstraction
        inline
        bool is_abstraction() const
        {
          return core::detail::gsIsBinder(*this);
        }

        /// \brief Returns true iff the expression is an application
        inline
        bool is_application() const
        {
          return core::detail::gsIsDataAppl(*this);
        }

        /// \brief Returns true iff the expression is a where clause
        inline
        bool is_where_clause() const
        {
          return core::detail::gsIsWhr(*this);
        }

    }; // class data_expression

    /// \brief list of new_data expressions
    ///
    typedef atermpp::term_list<data_expression> data_expression_list;

    /// \brief vector of new_data expressions
    ///
    typedef atermpp::vector<data_expression> data_expression_vector;

    /// \brief Returns true if the term t is a data expression
    /// \param t A term
    /// \return True if the term is a data expression.
    inline
    bool is_data_expression(atermpp::aterm_appl t)
    {
      return core::detail::gsIsDataExpr(t);
    }

    /// \brief Converts an iterator range to data_expression_list
    /// \note This function uses implementation details of the iterator type
    /// and hence is sometimes efficient than copying all elements of the list.
    template < typename ForwardTraversalIterator >
    inline data_expression_list make_data_expression_list(boost::iterator_range< ForwardTraversalIterator > const& r) {
      return convert< data_expression_list >(r);
    }

    /// \brief Converts an iterator range to data_expression_list
    template < typename ForwardTraversalIterator >
    inline data_expression_vector  make_data_expression_vector(boost::iterator_range< ForwardTraversalIterator > const& r) {
      return convert< data_expression_vector >(r);
    }

    /// \brief Converts a vector to a data_expression_list
    template < typename Expression >
    inline data_expression_list  make_data_expression_list(atermpp::vector< Expression >const& r) {
      return convert< data_expression_list >(r);
    }

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_DATA_EXPRESSION_H

