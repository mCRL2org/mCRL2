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
    // atermpp::aterm_appl internal_true;
    // atermpp::aterm_appl internal_false;

    template < typename EquationSelector >
    legacy_rewriter(mcrl2::data::data_specification const& d, EquationSelector const& selector, strategy s = jitty) :
        mcrl2::data::rewriter(d, selector, s)
    { 
      // internal_true.protect();
      // internal_true=(atermpp::aterm_appl)m_rewriter->toRewriteFormat(sort_bool::true_());
      // internal_false.protect();
      // internal_false=(atermpp::aterm_appl)m_rewriter->toRewriteFormat(sort_bool::false_());
    } 
  
    legacy_rewriter(const mcrl2::data::rewriter &other) :
      mcrl2::data::rewriter(other)
    { 
      // internal_true.protect();
      // internal_true=(atermpp::aterm_appl)m_rewriter->toRewriteFormat(sort_bool::true_());
      // internal_false.protect();
      // internal_false=(atermpp::aterm_appl)m_rewriter->toRewriteFormat(sort_bool::false_());
    }

    legacy_rewriter(const legacy_rewriter &other) :
      mcrl2::data::rewriter(other)
    { 
      // internal_true.protect();
      // internal_true=(atermpp::aterm_appl)m_rewriter->toRewriteFormat(sort_bool::true_());
      // internal_false.protect();
      // internal_false=(atermpp::aterm_appl)m_rewriter->toRewriteFormat(sort_bool::false_());
    }

    ~legacy_rewriter()
    {
      // internal_true.unprotect();
      // internal_false.unprotect();
    }

  
    atermpp::aterm_appl convert_to(const data_expression &t) const
    {
      return m_rewriter->toRewriteFormat(t);
    }
  
    mutable_map_substitution < atermpp::map < variable,atermpp::aterm_appl> > convert_to(const mutable_map_substitution <> &sigma) const
    {
      mutable_map_substitution < atermpp::map < variable,atermpp::aterm_appl> > sigma_in_internal_format;
      for(mutable_map_substitution <>::const_iterator i=sigma.begin(); i!=sigma.end(); ++i)
      {
        sigma_in_internal_format[i->first]=convert_to(i->second);
      }
      return sigma_in_internal_format;
    }
  
    data_expression convert_from(const atermpp::aterm_appl t) const
    {
      return m_rewriter->fromRewriteFormat(t);
    }
  
    atermpp::aterm_appl rewrite_internal(atermpp::aterm_appl const& t, mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma) const
    {
      return m_rewriter->rewrite_internal(t,sigma);
    } 
  
    atermpp::term_list <atermpp::aterm_appl> rewrite_internal_list(
         const atermpp::term_list<atermpp::aterm_appl> & t,
         mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma) const
    {
      return m_rewriter->rewrite_internal_list(t,sigma);
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
