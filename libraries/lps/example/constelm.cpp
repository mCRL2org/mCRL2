#include <string>
#include "mcrl2/lps/constelm.h"
#include "mcrl2/data/rewrite.h"

using namespace lps;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// rewriter
/// \brief rewriter.
class rewriter
{
  private:
    Rewriter* m_rewriter;

  public:
    /// Constructs a rewriter from data specification d.
    ///
    rewriter(data_specification d)
    {
      m_rewriter = createRewriter(d);
    }

    ~rewriter()
    {
      delete m_rewriter;
    }
  
		/// \brief Rewrites a data expression.
		/// \param d The term to be rewritten.
		/// \return The normal form of d.
		///
		data_expression operator()(const data_expression& d) const
		{
		  ATerm t = m_rewriter->toRewriteFormat(d);
		  return m_rewriter->rewrite((ATermAppl) t);
		}
};

using namespace lps;

int main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  string infile(argv[1]);
  specification spec1;
  spec1.load(argv[1]);
  rewriter r(spec1.data());
  specification spec2 = constelm(spec1, r);
  spec2.save(argv[2]);

  return 0;
}
