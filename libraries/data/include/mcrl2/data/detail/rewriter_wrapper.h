// Author(s): Jeroen van der Wulp, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

namespace detail
{

struct rewriter_wrapper
{
  public:
    typedef Rewriter::substitution_type substitution_type;

    typedef data_expression term_type;

    rewriter_wrapper(Rewriter* r):
      m_rewriter(r)
    {}

    data_expression operator()(const data_expression& t, Rewriter::substitution_type& sigma) const
    {
      return m_rewriter->rewrite(t,sigma);
    }

    void operator()(data_expression& result, const data_expression& t, Rewriter::substitution_type& sigma) const
    {
      return m_rewriter->rewrite(result, t, sigma);
    }

    data_expression operator()(const data_expression& t) const
    {
      Rewriter::substitution_type sigma;
      return m_rewriter->rewrite(t,sigma);
    }

    void operator()(data_expression& result, const data_expression& t) const
    {
      Rewriter::substitution_type sigma;
      m_rewriter->rewrite(result, t, sigma);
    }

  protected:
    Rewriter* m_rewriter;
};


} // namespace detail
} // namespace data
} // namespace mcrl2

#endif // _MCRL2_DATA_DETAIL_REWRITER_WRAPPER_H
