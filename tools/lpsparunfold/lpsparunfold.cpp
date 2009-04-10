// Author(s): F.P.M. (Frank) Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsparunfold.cpp

#define NAME "lpsparunfold"
#define AUTHOR "Frank Stappers"

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>
#endif

//C++
#include <exception>
#include <cstdio>
#include <set>

//Boost
#include <boost/lexical_cast.hpp>

//mCRL2
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
//#include "mcrl2/utilities/command_line_interface.h"
//#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/core/print.h"


//LPS framework
#include "mcrl2/lps/specification.h"

//DATA
#include "mcrl2/new_data/data_specification.h"

//LPSPARUNFOLDLIB
#include "lpsparunfoldlib.h"
#include "lpsparunfold.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"


using namespace std;
using namespace atermpp;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::new_data;

using namespace mcrl2;
using namespace mcrl2::utilities::tools;

//class parunfold_tool: public squadt_tool< rewriter_tool<input_output_tool> >
class parunfold_tool: public  rewriter_tool<input_output_tool>
{
  protected:

    //typedef squadt_tool< rewriter_tool<input_output_tool> > super;
    typedef rewriter_tool<input_output_tool> super;

    lps::t_parunfold_options m_parunfold_opts; ///< Options of the algorithm

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("index", make_mandatory_argument("NUM"), "unfolds process parameter at given index", 'i');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      if (0 == parser.options.count("index"))
      {
        cerr << "Index of process parameter is not specified." << endl;
        abort(); 
      }
      m_parunfold_opts.index = parser.option_argument_as< int >("index");
      
    }

  public:

    parunfold_tool()
      : super(
          "lpsparunfold",
          "Frank Stappers",
          "unfolds process parameter of an LPS",
          "Unfolds the given process paramters of the linear process specification (LPS) "
          "in INFILE and writes the result to OUTFILE. If INFILE is not present, stdin is "
          "used. If OUTFILE is not present, stdout is used."
        )
    {}

    ///Reads a specification from input_file,
    ///applies instantiation of sums to it and writes the result to output_file.
    bool run()
    {
      lps::specification lps_specification;

       lps_specification.load(m_input_filename);

       linear_process lps = lps_specification.process();

       data_specification data_spec = lps_specification.data();

       Sorts sorts( data_spec, lps );

       //lps::specification result = sorts.algorithm();
       sorts.algorithm( m_parunfold_opts.index );

       //result.save(m_output_filename);
       cerr<< "The following assertion will be removed upon tool finalization" <<endl;
       assert(false);

       return true;
    }

};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return parunfold_tool().execute(argc, argv);
}
