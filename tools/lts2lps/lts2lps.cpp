// Author(s): Frank Stappers, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts2lps.cpp
/// \brief This tool reads a mcrl2 specification of a linear process,
/// and translates it directly into LPS format.

#define TOOLNAME "lts2lps"
#define AUTHOR "Frank Stappers"

#include <fstream>
#include <iostream>
#include <string>
#include "mcrl2/utilities/logger.h"

#include "mcrl2/utilities/input_output_tool.h"

#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts2lps.h"




using namespace mcrl2;
using mcrl2::utilities::tools::input_output_tool;
using namespace mcrl2::utilities;
using namespace mcrl2::lts;
using namespace mcrl2::lps;
using namespace mcrl2::data;
using namespace mcrl2::log;



class lts2lps_tool : public input_output_tool
{
  private:
    typedef input_output_tool super;

  protected:
    data_file_type_t data_file_type;
    std::string data_file;


    void add_options(interface_description& desc)
    {
      super::add_options(desc);

      desc.add_option("data", make_file_argument("FILE"),
                      "use FILE as the data and action specification. "
                      "FILE must be a .mcrl2 file which does not contain an init clause. ", 'D');

      desc.add_option("lps", make_file_argument("FILE"),
                      "use FILE for the data and action specification. "
                      "FILE must be a .lps file. ", 'l');

      desc.add_option("mcrl2", make_file_argument("FILE"),
                      "use FILE as the data and action specification for the LTS. "
                      "FILE must be a .mcrl2 file. ", 'm');
    }

    void parse_options(const command_line_parser& parser)
    {
      using namespace mcrl2::lts;
      super::parse_options(parser);

      if (parser.options.count("data"))
      {
        if (1 < parser.options.count("data"))
        {
          mCRL2log(warning) << "multiple data specification files are specified; can only use one.\n";
        }
        data_file_type=mcrl2::lts::data_file_type_t::data_e;
        data_file = parser.option_argument("data");
      }

      if (parser.options.count("lps"))
      {
        if (1 < parser.options.count("lps") || data_file_type!=mcrl2::lts::data_file_type_t::none_e)
        {
          mCRL2log(warning) << "multiple data specification files are specified; can only use one.\n";
        }

        data_file_type=mcrl2::lts::data_file_type_t::lps_e;
        data_file = parser.option_argument("lps");
      }

      if (parser.options.count("mcrl2"))
      {
        if (1 < parser.options.count("mcrl2") || data_file_type!=mcrl2::lts::data_file_type_t::none_e)
        {
          mCRL2log(warning) << "multiple data specification files are specified; can only use one.\n";
        }

        data_file_type=mcrl2::lts::data_file_type_t::mcrl2_e;
        data_file = parser.option_argument("mcrl2");
      }
    }

  public:

    lts2lps_tool() : super(
        TOOLNAME,
        AUTHOR,
        "translates an LTS into an LPS",
        "Translates an LTS in INFILE and writes the resulting LPS to "
        "OUTFILE. If OUTFILE is not present, standard output is used. If INFILE is not "
        "present, standard input is used."),
      data_file_type(mcrl2::lts::data_file_type_t::none_e)
    {}


    bool run()
    {
      lts_lts_t l;
      const lts_type intype=lts::detail::guess_format(infilename);
      load_lts(l, input_filename(), intype, data_file_type, data_file);
      save_lps(mcrl2::lts::transform_lts2lps(l),output_filename());
      return true;
    }
};

int main(int argc, char** argv)
{
  return lts2lps_tool().execute(argc, argv);
}
