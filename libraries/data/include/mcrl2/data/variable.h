// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/variable.h
/// \brief The class variable.

#ifndef MCRL2_DATA_VARIABLE_H
#define MCRL2_DATA_VARIABLE_H

#include "boost/range/iterator_range.hpp"

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/application.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

//--- start generated class ---//
/// \brief A data variable
class variable_base: public data_expression
{
  public:
    /// \brief Constructor.
    /// \param term A term
    variable_base(atermpp::aterm_appl term)
      : data_expression(term)
    {
      assert(core::detail::check_term_DataVarId(m_term));
    }

    /// \brief Constructor.
    variable_base(const core::identifier_string& name, const sort_expression& sort)
      : data_expression(core::detail::gsMakeDataVarId(name, sort))
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

    /// \brief data variable.
    ///
    class variable: public detail::variable_base
    {
      public:

        /// \brief Constructor.
        ///
        variable()
          : detail::variable_base(core::detail::constructDataVarId())
        {}

        ///\overload
        variable(atermpp::aterm_appl term)
          : detail::variable_base(term)
        {}

        ///\overload
        variable(const core::identifier_string& name, const sort_expression& sort)
          : detail::variable_base(name, sort)
        {}

        /// \brief Constructor.
        ///
        /// \param[in] name The name of the variable.
        /// \param[in] sort The sort of the variable.
        variable(const std::string& name, const sort_expression& sort)
          : detail::variable_base(atermpp::aterm_string(name), sort)
        {}

    }; // class variable

    /// \brief list of variables
    typedef atermpp::term_list< variable >                         variable_list;
    /// \brief list of variables
    typedef atermpp::vector< variable >                            variable_vector;

    /// \brief Converts an container with variables to a variable_list
    /// \param r A range of variables.
    /// \note This function uses implementation details of the iterator type
    /// and hence is sometimes efficient than copying all elements of the list.
    template < typename Container >
    inline variable_list make_variable_list(Container const& r, typename detail::enable_if_container< Container, variable >::type* = 0)
    {
      return convert< variable_list >(r);
    }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_VARIABLE_H

