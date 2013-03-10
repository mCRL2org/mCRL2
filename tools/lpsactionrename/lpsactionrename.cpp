// Author(s): Tom Haenen, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsactionrename.cpp
/// \brief The tool lpsactionrename renames actions in an LPS

#include "boost.hpp" // precompiled headers

#define TOOLNAME "lpsactionrename"
#define AUTHOR "Jan Friso Groote and Tom Haenen"

#include <fstream>
#include <iostream>
#include <algorithm>
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/sumelm.h"
#include "mcrl2/lps/action_rename.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;
using namespace mcrl2::log;

using mcrl2::utilities::tools::input_output_tool;
using mcrl2::utilities::tools::rewriter_tool;

class action_rename_tool: public rewriter_tool<input_output_tool >
{
    //Type definitions
    //----------------
    typedef rewriter_tool<input_output_tool> super;

  protected:
    //t_phase represents the phases at which the program should be able to stop
    typedef enum { PH_NONE, PH_PARSE, PH_TYPE_CHECK, PH_DATA_IMPL} t_phase;

    //t_tool_options represents the options of the tool

    bool            m_pretty;
    bool            m_rewrite;
    bool            m_sumelm;
    t_phase         m_end_phase;
    std::string     m_action_rename_filename;

    std::string synopsis() const
    {
      return "[OPTION]... --renamefile=NAME [INFILE [OUTFILE]]\n";
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("renamefile", make_file_argument("NAME"),
                      "use the rename rules from NAME", 'f');
      desc.add_option("no-rewrite",
                      "do not rewrite data expressions while renaming; use when the rewrite system "
                      "does not terminate", 'o');
      desc.add_option("no-sumelm",
                      "do not apply sum elimination to the final result", 'm');
      desc.add_option("end-phase", make_mandatory_argument("PHASE"),
                      "stop conversion and output the action rename specification after phase PHASE: "
                      "'pa' (parsing), "
                      "'tc' (type checking) "
                      , 'p');
      desc.add_option("pretty",
                      "return a pretty printed version of the output", 'P');

    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      m_rewrite = (parser.options.count("no-rewrite")==0);
      m_sumelm  = (parser.options.count("no-sumelm")==0);
      m_pretty = (parser.options.count("pretty")!=0);

      if (parser.options.count("end-phase")>0)
      {
        std::string phase = parser.option_argument("end-phase");

        if (std::strncmp(phase.c_str(), "pa", 3) == 0)
        {
          m_end_phase = PH_PARSE;
        }
        else if (std::strncmp(phase.c_str(), "tc", 3) == 0)
        {
          m_end_phase = PH_TYPE_CHECK;
        }
        else
        {
          throw parser.error("option -p has illegal argument '" + phase + "'");
        }
      }

      m_action_rename_filename = parser.option_argument("renamefile");
    }

  public:
    action_rename_tool()
      : super(
        TOOLNAME,
        AUTHOR,
        "rename actions of an LPS",
        "Apply the action rename specification in FILE to the LPS in INFILE and save it to OUTFILE. "
        "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is used."
      ),
      m_pretty(false),
      m_rewrite(true),
      m_sumelm(true),
      m_end_phase(PH_NONE)
    {}

    bool run()
    {
      //load LPS
      if (input_filename().empty())
      {
        mCRL2log(verbose) << "reading LPS from stdin..." << std::endl;
      }
      else
      {
        mCRL2log(verbose) << "reading LPS from file '" <<  input_filename() << "'..." << std::endl;
      }
      specification lps_old_spec;
      lps_old_spec.load(input_filename());

      //load action rename file
      mCRL2log(verbose) << "reading input from file '" <<  m_action_rename_filename << "'..." << std::endl;
      std::ifstream rename_stream(m_action_rename_filename.c_str());
      if (!rename_stream.is_open())
      {
        throw mcrl2::runtime_error("cannot open rename file \"" + m_action_rename_filename + "\"");
      }

      // Parse the rename spec in rename_stream.
      // Note that all parsed data and action declarations in rename_stream are
      // added to lps_old_spec.
      action_rename_specification action_rename_spec =
        lps::parse_action_rename_specification(rename_stream,lps_old_spec);
      rename_stream.close();

      //rename all assigned actions
      mCRL2log(verbose) << "renaming actions in LPS..." << std::endl;
      specification lps_new_spec = action_rename(action_rename_spec, lps_old_spec);
      data::rewriter datar;
      if (m_rewrite)
      {
        mCRL2log(verbose) << "rewriting data expressions in LPS..." << std::endl;
        datar = create_rewriter(lps_new_spec.data());
        lps::rewrite(lps_new_spec, datar);
        lps::remove_trivial_summands(lps_new_spec);
      }
      if (m_sumelm)
      {
        mCRL2log(verbose) << "applying sum elimination..." << std::endl;
        sumelm_algorithm(lps_new_spec, mCRL2logEnabled(verbose)||mCRL2logEnabled(debug)).run();
        if (m_rewrite)
        {
          mCRL2log(verbose) << "rewriting data expressions in LPS again..." << std::endl;
          lps::rewrite(lps_new_spec, datar);
          lps::remove_trivial_summands(lps_new_spec);
        }
      }
      //save the result
      if (output_filename().empty())
      {
        mCRL2log(verbose) << "writing LPS to stdout..." << std::endl;
      }
      else
      {
        mCRL2log(verbose) << "writing LPS to file '" <<  output_filename() << "'..." << std::endl;
      }
      lps_new_spec.save(output_filename());

      return true;
    }

};

int main(int argc, char* argv[])
{
  return action_rename_tool().execute(argc, argv);
}
