// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bestranslate.cpp

#include <string>
#include <iostream>
#include <fstream>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/pbes/file_formats.h"
#include "mcrl2/bes/pg_parse.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::pbes_system;
using namespace mcrl2::bes;
using namespace mcrl2;

/// \brief Guess output file format based on filename
pbes_file_format guess_file_format(std::string const& filename)
{
  std::string extension = *(utilities::split(filename, ".").rbegin());

  pbes_file_format result;
  if (extension == "bes")
  {
    result = pbes_file_bes;
  }
  else if (extension == "cwi")
  {
    result = pbes_file_cwi;
  }
  else if (extension == "gm")
  {
    result = pbes_file_pgsolver;
  }
  else
  {
    throw mcrl2::runtime_error("Unknown extension `." + extension + "' occurred.");
  }

  return result;
}

/// \brief Load BES from input_filename. This guesses the file type of
///        input_filename based on the extension.
inline
void load_bes(boolean_equation_system& bes, std::string const& input_filename)
{
  pbes_file_format format = guess_file_format(input_filename);
  std::ifstream input; // Cannot declare in switch

  switch (format)
  {
    case pbes_file_bes:
      bes.load(input_filename);
      break;
    case pbes_file_cwi:
      throw mcrl2::runtime_error("Reading BES from cwi format is not supported");
      break;
    case pbes_file_pgsolver:
      input.open(input_filename.c_str());
      parse_pgsolver(input, bes);
      break;
    default:
      throw mcrl2::runtime_error("Trying to read BES from unsupported format");
  }
}

/// \brief Save BES to output_filename. The type is guessed based upon the
///        extension of output_filename
inline
void save_bes(boolean_equation_system const& bes, std::string const& output_filename)
{
  pbes_file_format format = guess_file_format(output_filename);
  save_bes(bes, output_filename, format);
}

class bestranslate_tool: public input_output_tool
{
  private:
    typedef input_output_tool super;

  public:
    bestranslate_tool()
      : super("bestranslate", "Jeroen Keiren",
              "translate a BES between various formats",
              "Translate BES in INFILE to OUTFILE converting between the formats"
              "specified in the filename. If OUTFILE is not present, stdout is"
              "used. If INFILE is not present, stdin is used."
             )
    {}

    bool run()
    {
      using namespace mcrl2::bes;
      boolean_equation_system bes;
      ::load_bes(bes, input_filename());
      ::save_bes(bes, output_filename());
      return true;
    }
};


int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: bestranslate INFILE OUTFILE" << std::endl;
    return 0;
  }


  return bestranslate_tool().execute(argc, argv);
}

