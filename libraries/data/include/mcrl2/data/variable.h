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

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2 {
  
  namespace data {

    /// \brief data variable.
    ///
    class variable: public data_expression
    {
      public:

        /// \brief Constructor.
        ///
        variable()
          : data_expression(core::detail::constructDataVarId())
        {}

        /// \brief Constructor.
        ///
        /// \param[in] d A data expression.
        /// \pre d is a variable.
        variable(const data_expression& d)
          : data_expression(d)
        {
          assert(d.is_variable());
        }

        /// \brief Constructor.
        ///
        /// \param[in] name The name of the variable.
        /// \param[in] sort The sort of the variable.
        variable(const std::string& name, const sort_expression& sort)
          : data_expression(core::detail::gsMakeDataVarId(atermpp::aterm_string(name), sort))
        {}

        /// \brief Constructor.
        ///
        /// \param[in] name The name of the variable.
        /// \param[in] sort The sort of the variable.
        variable(const core::identifier_string& name, const sort_expression& sort)
          : data_expression(core::detail::gsMakeDataVarId(name, sort))
        {}

        /// \brief Returns the name of the variable.
        inline
        std::string name() const
        {
          return atermpp::aterm_string(atermpp::arg1(*this));
        }

        /* Should be enabled when the implementation in data_expression is
         * removed
        /// \overload
        inline
        sort_expression sort() const
        {
          return atermpp::arg2(*this);
        }
        */

    }; // class variable

    /// \brief list of variables
    ///
    typedef atermpp::vector<variable> variable_list;

  } // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::variable)
/// \endcond

#endif // MCRL2_DATA_VARIABLE_H

