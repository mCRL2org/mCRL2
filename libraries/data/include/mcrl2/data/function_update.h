
// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/function_update.h
/// \brief The expressions for function updates.

#ifndef MCRL2_DATA_FUNCTION_UPDATE_H
#define MCRL2_DATA_FUNCTION_UPDATE_H

#include "mcrl2/data/detail/construction_utility.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/function_symbol.h"

namespace mcrl2 {

  namespace data {

    /// \brief Generate identifier function_update
    /// \return Identifier function_update
    core::identifier_string const& function_update_name()
    {
      static core::identifier_string function_update_name = data::detail::initialise_static_expression(function_update_name, core::identifier_string("@func_update"));
      return function_update_name;
    }

    /// \brief Constructor for function update
    /// \param s a function sort (S -> T)
    /// \pre |s.domain()| == 1
    /// \ret The function symbol representing function updates of functions of sort S -> T
    inline
    function_symbol function_update(const function_sort& s)
    {
      assert(s.domain().size() == 1);
      return data::function_symbol(function_update_name(),
                             make_function_sort(s, *(s.domain().begin()), s.codomain(), s));
    }

    /// \brief Recogniser for the function function_update
    /// \param e a data expression
    /// \ret true iff e is a function_update function symbol.
    inline
    bool is_function_update_function_symbol(const data_expression& e)
    {
      if (is_function_symbol(e))
      {
        return data::function_symbol(e).name() == function_update_name();
      }
      return false;
    }

    /// \brief Function update e[arg0 -> arg1], i.e. the function represented by e,
    ///        where e(arg0) becomes right
    /// \param e a data expression with function sort
    /// \param arg0 a data expression
    /// \param arg1 a data expression
    /// \pre |e.sort().domain()| == 1, arg0.sort() == e.sort().domain()[0], arg1.sort() == e.sort().codomain()
    /// \ret The expression e[arg0 -> arg1]
    inline
    application function_update(const data_expression& e,
                                const data_expression& arg0,
                                const data_expression& arg1)
    {
      assert(is_function_sort(e.sort()));
      assert(arg0.sort() == *(function_sort(e.sort()).domain().begin()));
      assert(arg1.sort() == function_sort(e.sort()).codomain());
      return function_update(e.sort())(e, arg0, arg1);
    }

    /// \brief Recogniser for the application of a function update
    /// \param e a data expression
    /// \ret true iff e is an applied function update
    inline
    bool is_function_update_application(const data_expression& e)
    {
      if (is_application(e))
      {
        is_function_update_function_symbol(application(e).head());
      }
      return false;
    }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FUNCTION_UPDATE_H

