#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/data/parse.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

using namespace mcrl2;

int main(int argc, char* argv[])
{
  if (argc < 2 || std::string(argv[1]) == "-h")
  {
    std::cout << "Usage: parse_data FILENAME" << std::endl;
    return 0;
  }

  std::string text = utilities::read_text(argv[1]);
  try
  {
    data::data_specification d = data::parse_data_specification(text);
  }
  catch (std::runtime_error e)
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
