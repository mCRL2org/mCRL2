// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/selective_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SELECTIVE_TRAVERSER_H
#define MCRL2_DATA_SELECTIVE_TRAVERSER_H

#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_base_of.hpp"

#include "mcrl2/core/selective_traverser.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2 {

namespace data {

  /// \brief Selective traversal class for data library data types
  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser: public core::selective_traverser<Derived, AdaptablePredicate, data::traverser>
  {
    typedef core::selective_traverser<Derived, AdaptablePredicate, data::traverser> super;

    protected:     

      template <typename Expression>
      void forward_call(Expression const& e, typename boost::disable_if<typename boost::is_base_of<data_expression, Expression>::type>::type* = 0,
                                             typename boost::disable_if<typename boost::is_base_of<sort_expression, Expression>::type>::type* = 0)
      {
        static_cast<super&>(*this)(e);
      }

      void forward_call(data_expression const& e)
      {
        static_cast<super&>(*this)(e);
      }

      void forward_call(sort_expression const& e)
      {
        static_cast<super&>(*this)(e);
      }

      template <typename Expression>
      void forward_call(Expression const& e, typename boost::enable_if<typename boost::is_base_of<data_expression, Expression>::type>::type* = 0)
      {
        if (m_traverse_condition(e))
        {
          static_cast<super&>(*this)(e);
        }
      }

      template <typename Expression>
      void forward_call(Expression const& e, typename boost::enable_if<typename boost::is_base_of<sort_expression, Expression>::type>::type* = 0) {
        if (m_traverse_condition(e))
        {
          static_cast<super&>(*this)(e);
        }
      }

    public:
      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SELECTIVE_TRAVERSER_H
