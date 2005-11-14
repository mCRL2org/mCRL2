#ifndef MCRL2_REWRITE_H
#define MCRL2_REWRITE_H

#include "mcrl2/data.h"
#include "librewrite_c.h"
#include "libstruct.h"

namespace mcrl2 {

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
      
      ATermAppl rewrite(ATermAppl t)
      {
        return gsRewriteTerm(t);
      }
  };

} // namespace mcrl

#endif // MCRL2_REWRITE_H
