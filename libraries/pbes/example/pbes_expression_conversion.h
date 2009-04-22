/// WARNING: This header file can only be used when the soundness checks are disabled
/// using the MCRL2_NO_SOUNDNESS_CHECKS flag!

#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/data/detail/optimized_rewriter.h"

namespace lps {

namespace detail {

struct to_rewriter_format_builder: public pbes_expression_builder
{
  optimized_rewriter& r;

  to_rewriter_format_builder(optimized_rewriter& r0)
    : r(r0)
  {}

  pbes_expression visit_data_expression(const pbes_expression& /* e */, const data_expression& d)
  {
std::cout << "<data>" << d << std::endl;
    return pbes_expression(r.to_rewriter_format(d));
  }
};

struct to_data_format_builder: public pbes_expression_builder
{
  optimized_rewriter& r;

  to_data_format_builder(optimized_rewriter& r0)
    : r(r0)
  {}

  pbes_expression visit_unknown(const pbes_expression& e)
  {
    return r.to_data_format(aterm_traits<pbes_expression>::term(e));
  }
};

/// Transforms all data expressions that are a subterm of x into
/// rewriter format.
///
inline
pbes_expression to_rewriter_format(optimized_rewriter& r, pbes_expression x)
{
  return to_rewriter_format_builder(r).visit(x);
}

/// Transforms all rewriter format expressions that are a subterm of x from
/// the internal rewriter format to external data format.
///
inline
pbes_expression to_data_format(optimized_rewriter& r, pbes_expression x)
{
  return to_data_format_builder(r).visit(x);
}

} // namespace detail

} // namespace lps
