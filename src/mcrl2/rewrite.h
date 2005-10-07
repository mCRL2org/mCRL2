#ifndef MCRL2_REWRITE_H
#define MCRL2_REWRITE_H

#include "atermpp/aterm.h"
#include "libgsrewrite.h"
#include "gsfunc.h"

namespace mcrl2 {

  /// Rewrite the term t using the given 'rewrite terms'. A possible
  /// choice for rewrite terms is the result of specification::equations().
  ///
  inline
  ATermAppl rewrite(ATermAppl t, ATermAppl rewrite_terms)
  {
    gsEnableConstructorFunctions();
    gsRewriteInit(rewrite_terms, GS_REWR_INNER3); 
    ATermAppl result = gsRewriteTerm(t);
    gsRewriteFinalise();
    return result;
  }

} // namespace mcrl

#endif // MCRL2_REWRITE_H


