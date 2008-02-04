// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/enumerator.h
/// \brief The class enumerator.

#ifndef MCRL2_DATA_ENUMERATOR_H
#define MCRL2_DATA_ENUMERATOR_H

#include <vector>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/data/enum.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/utilities/aterm_ext.h"

namespace mcrl2 {

namespace data {

class enumerator
{
  protected:
    Enumerator* m_enumerator;
    Rewriter* m_rewriter;

  public:
    enumerator(const data_specification& data_spec)
    {
      m_rewriter = createRewriter(data_spec);
      m_enumerator = createEnumerator(data_spec, m_rewriter);
    }
    
    ~enumerator()
    {
      delete m_enumerator;
      delete m_rewriter;
    }
    
    /// Returns all possible values of the finite sort s.
    std::vector<data_expression> enumerate_finite_sort(data::sort_expression s)
    {
      std::vector<data_expression> result;
      data_variable dummy(core::identifier_string("dummy"), s);

      // find all elements of sort s by enumerating all valuations of dummy
      // that make the expression "true" true
      EnumeratorSolutions* sols = m_enumerator->findSolutions(atermpp::make_list(dummy), m_rewriter->toRewriteFormat(data_expr::true_()));
      ATermList l; // variable to store a solution
      while (sols->next(&l)) // get next solution
      {
        // l is of the form [subst(x,expr)] where expr is in rewriter format
        atermpp::aterm_appl tmp = utilities::ATAgetFirst(l);
        data_expression d(m_rewriter->fromRewriteFormat(tmp(1)));
        result.push_back(d);
      }     
      return result;
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ENUMERATOR_H
