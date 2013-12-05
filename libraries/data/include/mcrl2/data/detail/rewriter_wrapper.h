// Author(s): Jeroen van der Wulp, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/enumeration.h
/// \brief Template class for substitution



#ifndef _MCRL2_DATA_DETAIL_REWRITER_WRAPPER_H
#define _MCRL2_DATA_DETAIL_REWRITER_WRAPPER_H

#include <boost/bind.hpp>
#include "mcrl2/data/rewriter.h"

namespace mcrl2
{
namespace data
{

/// \cond INTERNAL
namespace detail
{

struct legacy_rewriter : public mcrl2::data::rewriter
{
  public:

    typedef mcrl2::data::rewriter::substitution_type substitution_type;

    template < typename EquationSelector >
    legacy_rewriter(mcrl2::data::data_specification const& d, EquationSelector const& selector, strategy s = jitty) :
        mcrl2::data::rewriter(d, selector, s)
    { 
    } 
  
    legacy_rewriter(const mcrl2::data::rewriter &other) :
      mcrl2::data::rewriter(other)
    { 
    }

    legacy_rewriter(const legacy_rewriter &other) :
      mcrl2::data::rewriter(other)
    { 
    }

    ~legacy_rewriter()
    {
    }

    mcrl2::data::detail::Rewriter& get_rewriter() const
    {
      return *const_cast< mcrl2::data::detail::Rewriter* >(m_rewriter.get());
    }
}; 


} // namespace detail
} // namespace data
} // namespace mcrl2

#endif // _MCRL2_DATA_DETAIL_REWRITER_WRAPPER_H
