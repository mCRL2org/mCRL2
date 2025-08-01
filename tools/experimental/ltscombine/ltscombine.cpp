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

  void parse_options(const utilities::command_line_parser& parser) override
  {
    super::parse_options(parser);

    save_at_end = parser.has_option("save-at-end");
    nr_of_threads = number_of_threads();

    if (parser.options.count("comm") > 0)
    {
      std::string filename = parser.option_argument("comm");
      std::stringstream stringstream(filename);
      mCRL2log(log::debug) << "Reading synchronisation actions from " << filename << std::endl;
      std::ifstream file_input(filename.c_str());
      std::istream* syncs_inputs = &file_input;
      if (!file_input.good())
      {
        // File doesn't exist
        syncs_inputs = &stringstream;
      }
      std::string line;

      // Read synchronisation statements seperated by commas
      while (std::getline(*syncs_inputs, line, ','))
      {
        std::istringstream iss(line);
        std::multiset<core::identifier_string> labels{};
        std::string label;
        std::size_t delim = line.find('|');
        std::size_t prev_delim = 0;

        // Read multi-action consisting of actions seperated by bars
        while (delim != std::string::npos)
        {
          label = line.substr(prev_delim, delim - prev_delim);
          trim(label);
          labels.insert(label);

          prev_delim = delim + 1;
          delim = line.find('|', prev_delim);
        }

        // Find ->, which signals the resulting action
        delim = line.find('-', prev_delim + 1);
        if (delim != std::string::npos && line[delim + 1] == '>')
        {
          // Add last action of multi-action to list
          label = line.substr(prev_delim, delim - prev_delim);
          trim(label);
          labels.insert(label);

          if (labels.size() < 2)
          {
            // Syntax error, communication must contain two or more action labels
            mCRL2log(log::error)
                << "Syntax error in communications, left hand size must contain two or more action labels: '" << line
                << "'." << std::endl;
            throw mcrl2::runtime_error("Could not parse file " + filename + ".");
          }

          syncs.emplace_back(labels.begin(), labels.end());

          // Add multi-action and resulting action to list of sychronisations
          label = line.substr(delim + 2);
          trim(label);
          resulting_actions.emplace_back(label);
        }
        else
        {
          // Syntax error, -> not found
          mCRL2log(log::error) << "Syntax error in communications, -> not found for '" << line << "'." << std::endl;
          throw mcrl2::runtime_error("Could not parse file " + filename + ".");
        }
      }
    }

    if (parser.options.count("block") > 0)
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
      std::string action_label;

      // Read list of blocked actions seperated by commas
      while (std::getline(*block_input, action_label, ','))
      {
        if (action_label.find('|') != std::string::npos)
        {
          // Syntax error, multi-actions cannot be blocked
          mCRL2log(log::error) << "List of blocked action contains multi-action '" << action_label << "'." << std::endl;
          throw mcrl2::runtime_error("Could not parse file " + filename + ".");
        }

        // Trim action and add to list of blocked actions
        trim(action_label);
        blocks.emplace_back(action_label);
      }
    }

    if (parser.options.count("allow") > 0)
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
      std::string action_label;

      // Read list of allowed multi-actions seperated by commas
      while (std::getline(*allow_input, action_label, ','))
      {
        std::istringstream iss(action_label);
        std::multiset<core::identifier_string> labels;
        std::string label;
        std::size_t delim = action_label.find('|');
        std::size_t prev_delim = 0;

        // Read all actions seperated by bars
        while (delim != std::string::npos)
        {
          label = action_label.substr(prev_delim, delim - prev_delim);
          trim(label);
          labels.insert(label);

          prev_delim = delim + 1;
          delim = action_label.find('|', delim + 1);
        }

        // Read last action of multi-action
        label = action_label.substr(prev_delim);
        trim(label);
        labels.insert(label);

        // Add multi-action to the list of allowed actions
        allow.emplace_back(labels.begin(), labels.end());
      }
    }

    if (parser.options.count("hide") > 0)
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
      std::string action_label;

      // Read list of hiden actions seperated by commas
      while (std::getline(*hide_input, action_label, ','))
      {
        if (action_label.find('|') != std::string::npos)
        {
          // Syntax error, multi-actions cannot be hiden
          mCRL2log(log::error) << "List of hide actions contains multi-action '" << action_label << "'." << std::endl;
          throw mcrl2::runtime_error("Could not parse file " + filename + ".");
        }

        // Trim action and add to list of hiden actions
        trim(action_label);
        hiden.emplace_back(action_label);
      }
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

  std::vector<core::identifier_string> blocks;
  std::unordered_set<core::identifier_string_list> allow_set;
  std::vector<core::identifier_string_list> allow;
  std::vector<core::identifier_string> hiden;
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
