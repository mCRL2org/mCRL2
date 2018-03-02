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

#define TOOLNAME "lpsactionrename"
#define AUTHOR "Jan Friso Groote and Tom Haenen"

#include <fstream>
#include <iostream>
#include <algorithm>
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/sumelm.h"
#include "mcrl2/lps/action_rename.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;
using namespace mcrl2::log;

using mcrl2::utilities::tools::input_output_tool;
using mcrl2::data::tools::rewriter_tool;

class action_rename_tool: public rewriter_tool<input_output_tool >
{
    //Type definitions
    //----------------
    typedef rewriter_tool<input_output_tool> super;

  protected:

    bool            m_rewrite;
    bool            m_sumelm;
    bool            m_typecheck;
    bool            m_use_renamefile;
    std::string     m_action_rename_filename;
    std::string     m_regex;

    std::string synopsis() const
    {
      return "[OPTION]... (--renamefile=NAME | --regex=EXPR) [INFILE [OUTFILE]]\n";
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("renamefile", make_file_argument("NAME"),
                      "use the rename rules from NAME", 'f');
      desc.add_option("regex", make_mandatory_argument("EXPR"),
                      "use the provided regular expression to rename action labels. "
                      "Argument should be of the shape 'matching pattern/replacement'. "
                      "Matched groups can be substituted in the result with $n, where n is the index of the group. "
                      "It is generally good to surround the matching expression with ^$. Example: '^(.*)_send$/$1_receive' ", 'e');
      desc.add_option("no-rewrite",
                      "do not rewrite data expressions while renaming; use when the rewrite system "
                      "does not terminate", 'o');
      desc.add_option("no-sumelm",
                      "do not apply sum elimination to the final result", 'm');
      desc.add_option("no-typecheck",
                      "do not typecheck the resulting specfication", 't');

    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      m_rewrite = (parser.options.count("no-rewrite")==0);
      m_sumelm  = (parser.options.count("no-sumelm")==0);
      m_typecheck = (parser.options.count("no-typecheck")==0);

      bool renamefile_argument_present = (parser.options.count("renamefile")!=0);
      bool regex_argument_present = (parser.options.count("regex")!=0);
      if(renamefile_argument_present && regex_argument_present)
      {
        throw mcrl2::runtime_error("More than one of the arguments --renamefile and --regex was provided.");
      }
      if(!renamefile_argument_present && !regex_argument_present)
      {
        throw mcrl2::runtime_error("Neither of the arguments --renamefile and --regex was provided.");
      }
      m_use_renamefile = renamefile_argument_present;
      m_action_rename_filename = parser.option_argument("renamefile");
      m_regex = parser.option_argument("regex");
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
      m_rewrite(true),
      m_sumelm(true),
      m_typecheck(true)
    {}

    bool run()
    {
      //load LPS
      if (input_filename().empty())
      {
        mCRL2log(verbose) << "Reading LPS from stdin..." << std::endl;
      }
      else
      {
        mCRL2log(verbose) << "Reading LPS from file '" <<  input_filename() << "'..." << std::endl;
      }
      stochastic_specification old_spec;
      load_lps(old_spec, input_filename());

      stochastic_specification new_spec;
      if(m_use_renamefile)
      {
        //load action rename file
        mCRL2log(verbose) << "Reading input from file '" <<  m_action_rename_filename << "'..." << std::endl;
        std::ifstream rename_stream(m_action_rename_filename.c_str());
        if (!rename_stream.is_open())
        {
          throw mcrl2::runtime_error("Cannot open rename file \"" + m_action_rename_filename + "\".");
        }

        // Parse the rename spec in rename_stream.
        // Note that all parsed data and action declarations in rename_stream are
        // added to lps_old_spec.
        action_rename_specification action_rename_spec =
          lps::parse_action_rename_specification(rename_stream,old_spec);
        rename_stream.close();

        //rename all assigned actions
        mCRL2log(verbose) << "Renaming actions in LPS..." << std::endl;
        new_spec = action_rename(action_rename_spec, old_spec);
      }
      else
      {
        std::size_t slash_pos = m_regex.find('/');
        if(slash_pos == std::string::npos)
        {
          throw mcrl2::runtime_error("The provided regular expression did not include a slash.");
        }
        std::regex matching_regex(m_regex.substr(0, slash_pos));
        std::string replacing_fmt(m_regex.substr(slash_pos+1));

        mCRL2log(verbose) << "Renaming actions in LPS..." << std::endl;
        new_spec = action_rename(matching_regex, replacing_fmt, old_spec);
      }
      if(m_typecheck)
      {
        mCRL2log(verbose) << "Type checking resulting LPS..." << std::endl;
        if(!check_well_typedness(new_spec))
        {
          throw mcrl2::runtime_error("Type checking the specification obtained after renaming was unsuccesful.");
        }
      }
      data::rewriter datar;
      if (m_rewrite)
      {
        mCRL2log(verbose) << "Rewriting data expressions in LPS..." << std::endl;
        datar = create_rewriter(new_spec.data());
        lps::rewrite(new_spec, datar);
        lps::remove_trivial_summands(new_spec);
      }
      if (m_sumelm)
      {
        mCRL2log(verbose) << "Applying sum elimination..." << std::endl;
        sumelm_algorithm<lps::stochastic_specification>(new_spec, mCRL2logEnabled(verbose)||mCRL2logEnabled(debug)).run();
        if (m_rewrite)
        {
          mCRL2log(verbose) << "Rewriting data expressions in LPS again..." << std::endl;
          lps::rewrite(new_spec, datar);
          lps::remove_trivial_summands(new_spec);
        }
      }
      //save the result
      if (output_filename().empty())
      {
        mCRL2log(verbose) << "Writing LPS to stdout..." << std::endl;
      }
      else
      {
        mCRL2log(verbose) << "Writing LPS to file '" <<  output_filename() << "'..." << std::endl;
      }
      save_lps(new_spec, output_filename());

      return true;
    }

};

int main(int argc, char* argv[])
{
  return action_rename_tool().execute(argc, argv);
}
