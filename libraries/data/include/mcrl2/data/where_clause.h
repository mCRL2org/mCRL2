// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/where_clause.h
/// \brief The class where_clause.

#ifndef MCRL2_DATA_WHERE_CLAUSE_H
#define MCRL2_DATA_WHERE_CLAUSE_H

#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/assignment.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

//--- start generated class ---//
/// \brief A where expression
class where_clause_base: public data_expression
{
  public:
    /// \brief Constructor.
    /// \param term A term
    where_clause_base(atermpp::aterm_appl term)
      : data_expression(term)
    {
      assert(core::detail::check_term_Whr(m_term));
    }

    /// \brief Constructor.
    where_clause_base(const data_expression& body, const assignment_expression_list& declarations)
      : data_expression(core::detail::gsMakeWhr(body, declarations))
    {}

    data_expression body() const
    {
      return atermpp::arg1(*this);
    }

    assignment_expression_list declarations() const
    {
      return atermpp::list_arg2(*this);
    }
};
//--- end generated class ---//

    } // namespace detail

    /// \brief function symbol.
    ///
    class where_clause: public detail::where_clause_base
    {
      public:

        /// \brief Iterator range over list of declarations
        typedef atermpp::term_list< assignment >  declarations_range;

        /// \brief Iterator range over constant list of declarations
        typedef atermpp::term_list< assignment >  declarations_const_range;

      public:

        /// Constructor.
        ///
        where_clause()
          : detail::where_clause_base(core::detail::constructWhr())
        {}

        /// \overload
        where_clause(atermpp::aterm_appl term)
          : where_clause_base(term)
        {}

        /// \overload
        where_clause(const data_expression& body, const assignment_expression_list& declarations)
          : where_clause_base(body, declarations)
        {}

        /// Constructor.
        ///
        /// \param[in] body The body of the where_clause.
        /// \param[in] declarations The variable declarations of the where
        ///            clause (objects of type assignment_expression).
        template < typename Container >
        where_clause(const data_expression& body,
                     const Container& declarations,
                     typename detail::enable_if_container< Container, assignment_expression >::type* = 0)
          : detail::where_clause_base(body, convert< assignment_expression_list >(declarations))
        {}

    }; // class where_clause

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_WHERE_CLAUSE_H

