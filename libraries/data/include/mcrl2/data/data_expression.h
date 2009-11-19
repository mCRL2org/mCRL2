// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_expression.h
/// \brief The class data_expression.

#ifndef MCRL2_DATA_DATA_EXPRESSION_H
#define MCRL2_DATA_DATA_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct.h" // for gsIsDataExpr
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/function_sort.h"

namespace mcrl2 {

  namespace data {

    /// \brief Returns true if the term t is a data expression
    /// \param t A term
    /// \return True if the term is a data expression.
    inline
    bool is_data_expression(atermpp::aterm_appl t)
    {
      return core::detail::gsIsDataExpr(t);
    }

    /// \brief Returns true if the term t is a function symbol
    inline bool is_abstraction(atermpp::aterm_appl p) {
      return core::detail::gsIsBinder(p);
    }

    /// \brief Returns true if the term t is a function symbol
    inline bool is_function_symbol(atermpp::aterm_appl p) {
      return core::detail::gsIsOpId(p);
    }

    /// \brief Returns true if the term t is a variable
    inline bool is_variable(atermpp::aterm_appl p) {
      return core::detail::gsIsDataVarId(p);
    }

    /// \brief Returns true if the term t is an application
    inline bool is_application(atermpp::aterm_appl p) {
      return core::detail::gsIsDataAppl(p);
    }

    /// \brief Returns true if the term t is a where clause
    inline bool is_where_clause(atermpp::aterm_appl p) {
      return core::detail::gsIsWhr(p);
    }

    /// \brief data expression.
    ///
    /// A data expression can be any of:
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

        /// \brief Default constructor.
        ///
        data_expression()
          : atermpp::aterm_appl(core::detail::constructOpId())
        {}

        /// \brief Constructor.
        ///
        /// \param[in] t a term adhering to the internal format.
        data_expression(const atermpp::aterm_appl& t)
          : atermpp::aterm_appl(t)
        { // As Nil is used to indicate a non existing time value
          // in a linear process, we allow the occurrence of a Nil
          // term as a data_expression. This is a workaround which
          // should be removed.
          assert(is_data_expression(t) || core::detail::gsIsNil(t));
        }

        /// \brief Returns the sort of the data expression
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
          else 
          { std::cerr << "Failing term " << *this << "\n";
            assert(false);
          }

          return result;
        }

        /// \brief Returns true iff the expression is variable
        inline
        bool is_variable() const
        {
          return data::is_variable(*this);
        }

        /// \brief Returns true iff the expression is a function symbol
        inline
        bool is_function_symbol() const
        {
          return data::is_function_symbol(*this);
        }

        /// \brief Returns true iff the expression is an abstraction
        inline
        bool is_abstraction() const
        {
          return data::is_abstraction(*this);
        }

        /// \brief Returns true iff the expression is an application
        inline
        bool is_application() const
        {
          return data::is_application(*this);
        }

        /// \brief Returns true iff the expression is a where clause
        inline
        bool is_where_clause() const
        {
          return data::is_where_clause(*this);
        }

    }; // class data_expression

    /// \brief list of data expressions
    ///
    typedef atermpp::term_list<data_expression> data_expression_list;

    /// \brief vector of data expressions
    ///
    typedef atermpp::vector<data_expression> data_expression_vector;

    /// \brief Converts an container with data expressions to data_expression_list
    /// \param r A range of data expressions.
    /// \note This function uses implementation details of the iterator type
    /// and hence is sometimes efficient than copying all elements of the list.
    template < typename Container >
    inline data_expression_list make_data_expression_list(Container const& r, typename detail::enable_if_container< Container, data_expression >::type* = 0)
    {
      return convert< data_expression_list >(r);
    }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DATA_EXPRESSION_H

