// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/transform_iterator.h
/// \brief Provides a transform iterator for containers.

#ifndef MCRL2_ATERMPP_TRANSFORM_ITERATOR_H
#define MCRL2_ATERMPP_TRANSFORM_ITERATOR_H

#include "boost/call_traits.hpp"
#include "boost/iterator/iterator_adaptor.hpp"

namespace atermpp {

    /// \cond INTERNAL_DOCS
    namespace detail {

      // \note the dereference operation returns (re-)evaluates the function
      template < typename AdaptableUnaryFunction, typename Iterator, typename Value = typename AdaptableUnaryFunction::result_type >
      class transform_iterator : public boost::iterator_adaptor<
                 atermpp::detail::transform_iterator< AdaptableUnaryFunction, Iterator, Value >,
                                                        Iterator, Value, boost::use_default, Value > {

        friend class boost::iterator_core_access;

        private:

          AdaptableUnaryFunction m_transformer;

          Value dereference() const
          {
            return m_transformer(*(this->base_reference()));
          }

        public:

          transform_iterator(Iterator const& iterator) : transform_iterator::iterator_adaptor_(iterator)
          {}

          transform_iterator(Iterator const& iterator, typename boost::call_traits< AdaptableUnaryFunction >::param_type transformer) :
                                                         transform_iterator::iterator_adaptor_(iterator), m_transformer(transformer)
          {}
      };

    } // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_TRANSFORM_ITERATOR_H
