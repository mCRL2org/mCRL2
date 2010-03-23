// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/function_symbol.h
/// \brief The class function symbol.

#ifndef MCRL2_DATA_FUNCTION_SYMBOL_H
#define MCRL2_DATA_FUNCTION_SYMBOL_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {

  namespace data {

    namespace detail {
//--- start generated class ---//
/// \brief A function symbol
class function_symbol_base: public data_expression
{
  public:
    /// \brief Default constructor.
    function_symbol_base()
      : data_expression(core::detail::constructOpId())
    {}

    /// \brief Constructor.
    /// \param term A term
    function_symbol_base(atermpp::aterm_appl term)
      : data_expression(term)
    {
      assert(core::detail::check_term_OpId(m_term));
    }

    /// \brief Constructor.
    function_symbol_base(const core::identifier_string& name, const sort_expression& sort)
      : data_expression(core::detail::gsMakeOpId(name, sort))
    {}

    core::identifier_string name() const
    {
      return atermpp::arg1(*this);
    }

    sort_expression sort() const
    {
      return atermpp::arg2(*this);
    }
};
//--- end generated class ---//

    } // namespace detail

    /// \brief function symbol.
    ///
    class function_symbol: public detail::function_symbol_base
    {
      public:

        /// \brief Constructor.
        ///
        function_symbol()
          : detail::function_symbol_base(core::detail::constructOpId())
        {}

        /// \overload
        function_symbol(atermpp::aterm_appl term)
         : function_symbol_base(term)
        {}

        /// \overload
        function_symbol(const core::identifier_string& name, const sort_expression& sort)
         : function_symbol_base(name, sort)
        {}

        /// \brief Constructor.
        ///
        /// \param[in] name The name of the function.
        /// \param[in] sort The sort of the function.
        function_symbol(const std::string& name,
                        const sort_expression& sort)
          : detail::function_symbol_base(name, sort)
        {
        }

    }; // class function_symbol

    /// \brief list of function symbols
    typedef atermpp::term_list< function_symbol > function_symbol_list;

    /// \brief vector of function symbols
    typedef atermpp::vector< function_symbol >    function_symbol_vector;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FUNCTION_SYMBOL_H

