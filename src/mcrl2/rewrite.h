#ifndef MCRL2_REWRITE_H
#define MCRL2_REWRITE_H

#include "atermpp/aterm.h"
#include "mcrl2/data.h"
#include "libgsrewrite.h"
#include "gsfunc.h"

namespace mcrl2 {

  using atermpp::aterm_appl;

  /// This class can be used to rewrite terms.
  ///
  class rewriter
  {
    protected:
      data_equation_list m_equations;

    public:
      rewriter(data_equation_list equations)
        : m_equations(equations)
      {
        gsEnableConstructorFunctions();
        gsRewriteInit(gsMakeDataEqnSpec(equations), GS_REWR_INNER3); 
      }
      
      ~rewriter()
      {
        gsRewriteFinalise();
      }
      
      aterm_appl rewrite(aterm_appl t)
      {
        return gsRewriteTerm(t);
      }
  };

} // namespace mcrl

#endif // MCRL2_REWRITE_H
