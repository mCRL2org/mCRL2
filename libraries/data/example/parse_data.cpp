#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/data_implementation.h"
#include "mcrl2/core/alpha.h"
#include "mcrl2/core/regfrmtrans.h"
#include "mcrl2/new_data/parser.h"
#include "mcrl2/new_data/rewriter.h"

using namespace mcrl2;
using namespace mcrl2::data;

/// Parses a data specification.
inline
data_specification parse_data(const std::string& text)
{
  // TODO: This is only a temporary solution. A decent standalone parser needs
  // to be made for data specifications.

  // make a fake linear process
  std::stringstream lps_stream;
  lps_stream << text;
  lps_stream << "init delta;\n";

  ATermAppl result = data::detail::parse_specification(lps_stream);
  result           = data::detail::type_check_specification(result);
  result           = data::detail::alpha_reduce(result);

  atermpp::aterm_appl lps(result);
  return data_specification(lps(0));
}

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  if (argc < 1)
  {
    std::cout << "Usage: parse_data FILENAME" << std::endl;
    return 1;
  }

  std::string text = core::read_text(argv[1]);
  try
  {
    data_specification d = parse_data_specification(text);
    std::cout << core::pp(d) << std::endl;
  }
  catch (std::runtime_error e)
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
