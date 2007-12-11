// This flag disables the soundness checks in this compilation unit.
// This allows the creation of LPS classes that are (temporarily)
// in a bad state. This can be useful in performance critical sections.
#define MCRL2_NO_SOUNDNESS_CHECKS

// The program below illustrates how this feature can be used to convert
// the data expressions inside a pbes_expressions.

#include <iostream>
#include "pbes_expression_conversion.h"
#include "mcrl2/data/data.h"
#include "mcrl2/lps/mcrl22lps.h"

const std::string SPECIFICATION =
"act a;         \n"
"proc X = a. X; \n"
"init X;        \n"
; 

using namespace atermpp;
using namespace lps;
using namespace lps::detail;

int main()
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  namespace d = lps::data_expr;
  namespace p = lps::pbes_expr;

  specification spec = mcrl22lps(SPECIFICATION);

std::cout << "hiero0" << std::endl;

  optimized_rewriter r(spec.data());

std::cout << "hiero1" << std::endl;

  data_expression x = data_variable("x", sort_expr::int_());
  data_expression y = data_variable("y", sort_expr::int_());

std::cout << "<x>" << x.sort() << std::endl;
std::cout << "<y>" << y.sort() << std::endl;
std::cout << "<x+y>" << d::plus(x, y).sort() << std::endl;
std::cout << "<x-y>" << d::minus(x, y).sort() << std::endl;
std::cout << "<(x+y)*(x-y)>" << d::multiplies(d::plus(x, y), d::minus(x, y)).sort() << std::endl;
std::cout << "<1>" << d::int_(1).sort() << std::endl;

  data_expression xy = d::greater(d::multiplies(d::plus(x, y), d::minus(x, y)), d::int_(1));
  data_expression z  = d::equal_to(x, y);

std::cout << "hiero2" << std::endl;

  pbes_expression z0 = p::and_(p::val(xy), p::val(z));

std::cout << "hiero3" << std::endl;

  pbes_expression z1 = to_rewriter_format(r, z0);

std::cout << "hiero4" << std::endl;

  pbes_expression z2 = to_data_format(r, z1);

std::cout << "hiero5" << std::endl;
  
  std::cout << "z0 = " << z0 << std::endl;
  std::cout << "z1 = " << z1 << std::endl;
  std::cout << "z2 = " << z2 << std::endl;
}