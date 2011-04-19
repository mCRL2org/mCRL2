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

// #include <memory>
// #include <boost/utility.hpp>
#include "mcrl2/data/rewriter.h"

namespace mcrl2
{
namespace data
{

/// \cond INTERNAL
namespace detail
{


/// \cond INTERNAL_DOCS
// inherits from rewriter only for data implementation/reconstruction
//
// To minimize changes to the existing implementation, data
// implementation/reconstruction is performed manually.


struct legacy_rewriter : public mcrl2::data::rewriter
{
  public:
    template < typename EquationSelector >
    legacy_rewriter(mcrl2::data::data_specification const& d, EquationSelector const& selector, strategy s = jitty) :
        mcrl2::data::rewriter(d, selector, s)
    { 
    } 
  
    legacy_rewriter(const mcrl2::data::rewriter &other) :
      mcrl2::data::rewriter(other)
    { 
    }
  
    ATerm convert_to(const data_expression &t) const
    {
      return m_rewriter->toRewriteFormat(mcrl2::data::rewriter::implement(t));
    }
  
    data_expression convert_from(const ATerm t) const
    {
      return this->reconstruct(atermpp::aterm_appl(m_rewriter->fromRewriteFormat(t)));
    }
  
    ATerm rewrite_internal(atermpp::aterm const& t) const
    {
      return m_rewriter->rewriteInternal(t);
    }
  
    ATerm rewrite_internal(ATerm const& t) const
    {
      return m_rewriter->rewriteInternal(t);
    }
  
    ATermList rewrite_internal(ATermList const& t) const
    {
      return m_rewriter->rewriteInternalList(t);
    }
  
    ATerm internally_associated_value(const variable &v) const
    {
      return m_rewriter->getSubstitutionInternal(v);
    }
  
    /// Note the internal state of the rewriter changes so constness is violated
    void set_internally_associated_value(const variable &v, const ATerm &e) const
    {
      m_rewriter->setSubstitutionInternal(v, e);
    }
  
    /// Note the internal state of the rewriter changes so constness is violated
    void set_internally_associated_value(const variable &v, const data_expression &e) const
    {
      m_rewriter->setSubstitution(v, e);
    }
  
    /// Note the internal state of the rewriter changes so constness is violated
    void clear_internally_associated_value(const variable &v) const
    {
      m_rewriter->clearSubstitution(v);
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
