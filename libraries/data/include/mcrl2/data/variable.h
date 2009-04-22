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
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/application.h"

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
        /// \param[in] d A term expression.
        /// \pre d is a variable.
        variable(const atermpp::aterm_appl& d)
          : data_expression(d)
        {
          assert(data_expression(d).is_variable());
        }

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

        /// \brief Returns the application of this variable to an argument.
        /// \pre this->sort() is a function sort.
        /// \param[in] e The data expression to which the variable is applied
        application operator()(const data_expression& e)
        {
          assert(this->sort().is_function_sort());
          return application(*this, e);
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
    typedef atermpp::term_list< variable >                         variable_list;
    /// \brief list of variables
    typedef atermpp::vector< variable >                            variable_vector;

    /// \brief Converts an iterator range to data_expression_list
    /// \param r A range of variables.
    /// \note This function uses implementation details of the iterator type
    /// and hence is sometimes efficient than copying all elements of the list.
    template < typename ForwardTraversalIterator >
    inline variable_list make_variable_list(boost::iterator_range< ForwardTraversalIterator > const& r) {
      return convert< variable_list >(r);
    }

    /// \brief Converts a vector to a variable_list 
    /// \param r A range of variables.
    template < typename Expression >
    inline variable_list make_variable_list(atermpp::vector< Expression >const& r) {
      return convert< variable_list >(r);
    }

    /// \brief Converts an iterator range to variable_list
    /// \param r A range of variables.
    template < typename ForwardTraversalIterator >
    inline variable_vector make_variable_vector(boost::iterator_range< ForwardTraversalIterator > const& r) {
      return convert< variable_vector >(r);
    }

    /// \brief Converts an iterator range to variable_list
    /// \param r A range of variables.
    template < typename Expression >
    inline variable_vector make_variable_vector(atermpp::term_list< Expression > const& r) {
      return convert< variable_vector >(boost::make_iterator_range(r));
    }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_VARIABLE_H

