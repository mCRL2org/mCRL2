// Author(s): Willem Rietdijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>

#include "lts_combine.h"

#include "mcrl2/process/action_label.h"
#include "mcrl2/utilities/parallel_tool.h"
#include "mcrl2/utilities/xinput_output_tool.h"

namespace mcrl2
{

using mcrl2::utilities::tools::parallel_tool;
using mcrl2::utilities::tools::xinput_output_tool;

class ltscombine_tool : public parallel_tool<xinput_output_tool>
{
  using super = parallel_tool<xinput_output_tool>;

public:
  ltscombine_tool()
      : super("ltscombine",
          "Willem Rietdijk",
          "Combines LTSs",
          "Combines multiple labelled transition systems (LTS) "
          "in the INFILES and writes the resulting LTS to OUTFILE. "
          "Communication, blocking, allowing and hiding are applied "
          "afterwards in that order.")
  {
    min_input_files = 2;
    max_input_files = 0;
  }

  bool run() override
  {
    // Load LTSs from input
    std::vector<lts::lts_lts_t> lts{};

    for (auto& filename : input_filenames())
    {
      lts::lts_lts_t new_lts;
      new_lts.load(filename);
      lts.push_back(new_lts);
    }

    // Ensure that block and hide sets are sorted
    std::function<bool(const core::identifier_string&, const core::identifier_string&)> action_name_compare = process::action_name_compare();
    blocks = atermpp::sort_list(blocks, action_name_compare);

    hiden = atermpp::sort_list(hiden, action_name_compare);
    // Ensure that each of the multi actions names in the allow set is sorted.
    std::for_each(allow.begin(), allow.end(), [action_name_compare](core::identifier_string_list& multi_action_name)
    {
      multi_action_name = atermpp::sort_list(multi_action_name, action_name_compare);
    });
    // Ensure that the left-hand sides of the synchronisation rules are sorted.
    std::for_each(syncs.begin(), syncs.end(), [action_name_compare](core::identifier_string_list& multi_action_name)
    {
      multi_action_name = atermpp::sort_list(multi_action_name, action_name_compare);
    });

    // Generate and output resulting LTS
    combine_lts(lts, syncs, resulting_actions, blocks, hiden, allow, output_filename(), save_at_end, nr_of_threads);

    return true;
  }

protected:
  void add_options(utilities::interface_description& desc) override
  {
    super::add_options(desc);

    desc.add_option("allow",
        utilities::make_file_argument("FILE"),
        "The file containing the list of "
        "allowed multi-actions.",
        'a');
    desc.add_option("block",
        utilities::make_file_argument("FILE"),
        "The file containing the list of blocked "
        "action labels.",
        'b');
    desc.add_option("hide",
        utilities::make_file_argument("FILE"),
        "The file containing the list action labels "
        "that will be hiden.",
        't');
    desc.add_option("comm",
        utilities::make_file_argument("FILE"),
        "The file containing the list of "
        "synchronising actions.",
        'c');
    desc.add_option("save-at-end",
        "delay saving of the generated LTS until the end. "
        "This option only applies to .aut and .lts files, which are by default saved on the fly.");
  }

  void parse_comm(const utilities::command_line_parser& parser)
  {
    std::string filename = parser.option_argument("comm");
    std::stringstream stringstream(filename);
    mCRL2log(log::debug) << "Reading synchronisation actions from " << filename << std::endl;
    std::ifstream file_input(filename.c_str());
    std::istream* syncs_inputs = &file_input;
    if (!file_input.good())
    {
      // File doesn't exist, read directly from input.
      syncs_inputs = &stringstream;
    }
    std::tie(syncs, resulting_actions) = parse_comm_set(*syncs_inputs);
  }

  void parse_block(const utilities::command_line_parser& parser)
  {
    std::string filename = parser.option_argument("block");
    std::istringstream stringstream(filename);
    std::ifstream file_input(filename.c_str());
    std::istream* block_input = &file_input;
    if (!file_input.good())
    {
      // File doesn't exist
      block_input = &stringstream;
      mCRL2log(log::debug) << "Reading blocked actions from input" << std::endl;
    }
    else
    {
      mCRL2log(log::debug) << "Reading blocked actions from file " << filename << std::endl;
    }

    blocks = parse_action_name_set(*block_input);
  }

  void parse_allow(const utilities::command_line_parser& parser)
  {
    std::string filename = parser.option_argument("allow");
    std::stringstream stringstream(filename);
    mCRL2log(log::debug) << "Reading allowed multi-actions from file " << filename << std::endl;
    std::ifstream file_input(filename.c_str());
    std::istream* allow_input = &file_input;
    if (!file_input.good())
    {
      // File doesn't exist
      allow_input = &stringstream;
    }

    allow = parse_multi_action_name_set(*allow_input);
  }

  void parse_hide(const utilities::command_line_parser& parser)
  {
    std::string filename = parser.option_argument("hide");
    std::stringstream stringstream(filename);
    mCRL2log(log::debug) << "Reading hidden actions from file " << filename << std::endl;
    std::ifstream file_input(filename.c_str());
    std::istream* hide_input = &file_input;
    if (!file_input.good())
    {
      // File doesn't exist
      hide_input = &stringstream;
    }

    hiden = parse_action_name_set(*hide_input);
  }

  void parse_options(const utilities::command_line_parser& parser) override
  {
    super::parse_options(parser);

    save_at_end = parser.has_option("save-at-end");
    nr_of_threads = number_of_threads();

    if (parser.options.count("comm") > 0)
    {
      parse_comm(parser);
    }

    if (parser.options.count("block") > 0)
    {
      parse_block(parser);
    }

    if (parser.options.count("allow") > 0)
    {
      parse_allow(parser);
    }

    if (parser.options.count("hide") > 0)
    {
      parse_hide(parser);
    }
  }

private:
  // trim from start (in place)
  inline void ltrim(std::string& s)
  {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
  }

  // trim from end (in place)
  inline void rtrim(std::string& s)
  {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
  }

  // trim from both ends (in place)
  inline void trim(std::string& s)
  {
    rtrim(s);
    ltrim(s);
  }

  core::identifier_string_list blocks;
  std::unordered_set<core::identifier_string_list> allow_set;
  std::vector<core::identifier_string_list> allow;
  core::identifier_string_list hiden;
  std::vector<core::identifier_string_list> syncs;
  std::vector<core::identifier_string> resulting_actions;

  bool save_at_end = false;
  std::size_t nr_of_threads = 0UL;
};

} // namespace mcrl2

int main(int argc, char** argv)
{
  return mcrl2::ltscombine_tool().execute(argc, argv);
}
