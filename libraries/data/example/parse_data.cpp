#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/data/parser.h"

using namespace mcrl2;

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
    data::data_specification d = data::parse_data_specification(text);
    std::cout << data::pp(d) << std::endl;
  }
  catch (std::runtime_error e)
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
