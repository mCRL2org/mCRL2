#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/data/parser.h"

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  if (argc < 1)
  {
    std::cout << "Usage: parse_data FILENAME" << std::endl;
    return 1;
  }

  std::string text = mcrl2::core::read_text(argv[1]);
  try
  {
    mcrl2::data::data_specification d = mcrl2::data::parse_data_specification(text);
    std::cout << core::pp(d) << std::endl;
  }
  catch (std::runtime_error e)
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
