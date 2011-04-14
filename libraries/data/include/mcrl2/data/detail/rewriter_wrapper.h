// Author(s): Jeroen van der Wulp
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

#include <memory>

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
  typedef atermpp::aterm_appl term_type;

  template < typename ExpressionOrEquation >
  ATermAppl implement(ExpressionOrEquation const& expression) const
  {
    return expression;
  }

  template < typename EquationSelector >
  legacy_rewriter(mcrl2::data::data_specification const& d, EquationSelector const& selector, strategy s = jitty) :
    mcrl2::data::rewriter(d, selector, s)
  { 
  }

  legacy_rewriter(const mcrl2::data::rewriter &other) :
    mcrl2::data::rewriter(other)
  { 
  }

  /* legacy_rewriter()
  {
    assert(false);
  } */

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

  /// \brief Evaluate a substitution applied to an expression
  ///
  /// \param[in] e expression to evaluate
  /// \param[in] s substitution to apply to expression
  /// \return an expression equivalent to m_rewriter(s(e))

  template < typename Substitution >
  atermpp::aterm rewrite_internal(atermpp::aterm const& e, Substitution const& s) const
  {
    mcrl2::data::detail::Rewriter& local_rewriter(*m_rewriter);

    for (typename Substitution::const_iterator i(s.begin()); i != s.end(); ++i)
    {
      local_rewriter.setSubstitutionInternal(static_cast< ATermAppl >(i->first),
                                             static_cast< ATerm >(i->second));
    }

    ATerm result = local_rewriter.rewriteInternal(static_cast< ATerm >(e));

    // Subtle removal as NextStateGenerator requires substitutions for other variables
    for (typename Substitution::const_iterator i(s.begin()); i != s.end(); ++i)
    {
      local_rewriter.clearSubstitution(static_cast< ATermAppl >(i->first));
    }

    return result;
  }

  

  ATerm internally_associated_value(ATermAppl t) const
  {
    return m_rewriter->getSubstitutionInternal(t);
  }

  /// Note the internal state of the rewriter changes so constness is violated
  void set_internally_associated_value(ATermAppl t, ATerm e) const
  {
    m_rewriter->setSubstitutionInternal(t, e);
  }

  /// Note the internal state of the rewriter changes so constness is violated
  void set_internally_associated_value(ATermAppl t, data_expression e) const
  {
    m_rewriter->setSubstitution(t, e);
  }

  /// Note the internal state of the rewriter changes so constness is violated
  void clear_internally_associated_value(ATermAppl t) const
  {
    m_rewriter->clearSubstitution(t);
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
