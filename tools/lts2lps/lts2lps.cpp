// Author(s): Frank Stappers
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

#include "boost.hpp" // precompiled headers

#define TOOLNAME "lts2lps"
#define AUTHOR "Frank Stappers"

#include <fstream>
#include <iostream>
#include <string>
#include "mcrl2/core/messaging.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lts/lts.h"

using namespace mcrl2;
using mcrl2::utilities::tools::input_output_tool;
using namespace mcrl2::utilities;
using namespace mcrl2::core;

struct t_lts2lps_options {
  std::string datafile;
  std::string infilename;
  std::string outfilename;
};

class lts2lps_tool : public input_output_tool
{

  typedef input_output_tool super;
  mcrl2::lts::lts_type        intype;

  private:

    t_lts2lps_options m_lps2lts_options;

    void add_options(interface_description &desc)
    {
      super::add_options(desc);

      desc.add_option("data", make_mandatory_argument("FILE"),
          "use FILE as the data specification from which the input LTS was generated.", 'D');
    }

    void parse_options(const command_line_parser& parser)
    {
      using namespace mcrl2::lts;
      super::parse_options(parser);

      if (parser.options.count("data")) {
        if (1 < parser.options.count("data")) {
          std::cerr << "warning: multiple data specification files are specified; can only use one\n";
        }

        m_lps2lts_options.datafile = parser.option_argument("data");
      }  
 
      if (parser.options.count("in")) {
        if (1 < parser.options.count("in")) {
          parser.error("multiple input formats specified; can only use one");
        }

        intype = mcrl2::lts::lts::parse_format(parser.option_argument("in"));
        if (intype == lts_none || intype == lts_dot)  {
          parser.error("option -i/--in has illegal argument '" +
            parser.option_argument("in") + "'");
        }
      }


      m_lps2lts_options.infilename       = input_filename();
      m_lps2lts_options.outfilename      = output_filename();
    } 

  public:

    lts2lps_tool() : super(
             TOOLNAME,
             AUTHOR,
             "translates an LTS into an LPS",
             "Translates an LTS in INFILE and writes the resulting LPS to "
             "OUTFILE. If OUTFILE is not present, standard output is used. If INFILE is not "
             "present, standard input is used."),
      intype(mcrl2::lts::lts_none)
    {}


    bool run()
    {   
      using namespace mcrl2::lts;

      std::stringstream lps;

      /* Read data specification (if any) */ 
      if (m_lps2lts_options.datafile.empty())
      {
         std::cerr << "No data specification provided" << std::endl;
      }
      else
      {
        std::ifstream dfile( m_lps2lts_options.datafile.c_str() );

        if( !dfile )
        {
          std::cerr << "Cannot read data file" << std::endl;
        }

        char ch;
        while(dfile)
        {
          dfile.get(ch);
          if(dfile) lps << ch;
        }
        dfile.close();
        lps << std::endl;
      }

      /* Read LTS */
      mcrl2::lts::lts l;

      if (m_lps2lts_options.infilename.empty()) {
        gsVerboseMsg("reading LTS from stdin...\n");

        if ( !l.read_from(std::cin, intype) ) {
          throw mcrl2::runtime_error("cannot read LTS from stdin\nretry with -v/--verbose for more information");
        }
      }
      else {
        gsVerboseMsg("reading LTS from '%s'...\n",m_lps2lts_options.infilename.c_str());

        if (!l.read_from(m_lps2lts_options.infilename,intype)) {
          throw mcrl2::runtime_error("cannot read LTS from file '" + m_lps2lts_options.infilename +
                                             "'\nretry with -v/--verbose for more information");
        }
      }

      lps <<"proc P(n: Int)=";
      bool fst_el = true;
      for(transition_iterator ti = l.get_transitions(); ti.more(); ++ti)
      {
        unsigned int idFrom, idTo;
        std::string label = l.label_value_str(
                              ti.label());
        idFrom = ti.from();
        idTo = ti.to();

        if  ( fst_el )
        {
          fst_el = false;
        } else {
          lps << "\t+";
        }

        lps << "(n==" << ti.from() << ")->" << label << ".P("<< ti.to() << ")" << std::endl; 
      }

      lps << "\t;" << std::endl << std::endl <<"init P(" << l.initial_state() << ");" << std::endl; 

      gsVerboseMsg("%s", lps.str().c_str() );

      lps::specification spec = lps::parse_linear_process_specification(lps.str());
      spec.save(output_filename());
      return true;
    }
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
 
  return lts2lps_tool().execute(argc, argv);
}
