// Author(s): Willem Rietdijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <fstream>
#include <string>

#include "lts_combine.h"

#include "mcrl2/utilities/xinput_output_tool.h"

namespace mcrl2
{

using log::log_level_t;
using mcrl2::utilities::tools::xinput_output_tool;

class ltscombine_tool : public xinput_output_tool
{
  typedef xinput_output_tool super;

public:
  ltscombine_tool() : super(
      "ltscombine",
      "Willem Rietdijk",
      "Combines LTSs",
      "Combines multiple labelled transition systems (LTS)"
      "in the INFILES and writes the resulting LTS to OUTFILE."
      "Communication, blocking, allowing and hiding are applied"
      "afterwards in that order.",
      2, 0)
  {}

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
    if (output_filename().empty())
    {
      combine_lts(lts, syncs, blocks, hiden, allow, std::cout);
    }
    else
    {
      std::ofstream file(output_filename(), std::ios_base::binary);
      combine_lts(lts, syncs, blocks, hiden, allow, file);
    }

    return true;
  }

protected:
  void add_options(utilities::interface_description& desc) override
  {
    super::add_options(desc);

    desc.add_option("allow",
        utilities::make_file_argument("FILE"),
        "The file containing the list of"
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
        "The file containing the list of"
        "synchronising actions.",
        'c');
  }

  void parse_options(const utilities::command_line_parser& parser) override
  {
    super::parse_options(parser);

    if (parser.options.count("comm") > 0)
    {
      std::string filename = parser.option_argument("comm");
      mCRL2log(log::debug) << "Reading synchronisation actions from " << filename << std::endl;
      std::ifstream syncs_inputs(filename.c_str());
      std::string line;

      // Read synchronisation statements seperated by commas
      while (std::getline(syncs_inputs, line, ','))
      {
        std::istringstream iss(line);
        std::vector<std::string> labels{};
        std::string label;
        std::size_t delim = line.find('|');
        std::size_t prev_delim = 0;

        // Read multi-action consisting of actions seperated by bars
        while (delim != std::string::npos)
        {
          label = line.substr(prev_delim, delim - prev_delim);
          trim(label);
          labels.push_back(label);

          prev_delim = delim + 1;
          delim = line.find('|', prev_delim);
        }

        // Find ->, which signals the resulting action
        delim = line.find('-', prev_delim+1);
        if (delim != std::string::npos && line[delim + 1] == '>')
        {
          // Add last action of multi-action to list
          label = line.substr(prev_delim, delim - prev_delim);
          trim(label);
          labels.push_back(label);

          // Add multi-action and resulting action to list of sychronisations
          label = line.substr(delim + 2);
          trim(label);
          syncs.push_back(std::make_pair(label, labels));
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
      mCRL2log(log::debug) << "Reading blocked actions from file " << filename << std::endl;
      std::ifstream blocks_input(filename.c_str());
      std::string action_label;
      
      // Read list of multi-actions seperated by commas
      while (std::getline(blocks_input, action_label, ','))
      {
        std::istringstream iss(action_label);
        std::vector<std::string> labels{};
        std::string label;
        std::size_t delim = action_label.find('|');
        std::size_t prev_delim = 0;

        // Read all actions seperated by bars
        while (delim != std::string::npos)
        {
          label = action_label.substr(prev_delim, delim - prev_delim);
          trim(label);
          labels.push_back(label);

          prev_delim = delim + 1;
          delim = action_label.find('|', delim + 1);
        }

        // Read last action of multi-action
        label = action_label.substr(prev_delim);
        trim(label);
        labels.push_back(label);

        // Add multi-action to the list of blocked actions
        blocks.push_back(labels);
      }
    }

    if (parser.options.count("allow") > 0)
    {
      std::string filename = parser.option_argument("allow");
      mCRL2log(log::debug) << "Reading allowed multi-actions from file " << filename << std::endl;
      std::ifstream allow_input(filename.c_str());
      std::string action_label;

      // Read list of allowed multi-actions seperated by commas
      while (std::getline(allow_input, action_label, ','))
      {
        std::istringstream iss(action_label);
        std::vector<std::string> labels{};
        std::string label;
        std::size_t delim = action_label.find('|');
        std::size_t prev_delim = 0;

        // Read all actions seperated by bars
        while (delim != std::string::npos)
        {
          label = action_label.substr(prev_delim, delim - prev_delim);
          trim(label);
          labels.push_back(label);

          prev_delim = delim + 1;
          delim = action_label.find('|', delim + 1);
        }

        // Read last action of multi-action
        label = action_label.substr(prev_delim);
        trim(label);
        labels.push_back(label);

        // Add multi-action to the list of allowed actions
        allow.push_back(labels);
      }
    }

    if (parser.options.count("hide") > 0)
    {
      std::string filename = parser.option_argument("hide");
      mCRL2log(log::debug) << "Reading hiden actions from file " << filename << std::endl;
      std::ifstream hide_input(filename.c_str());
      std::string action_label;

      // Read list of hiden actions seperated by commas
      while (std::getline(hide_input, action_label, ','))
      {
        if (action_label.find('|') != std::string::npos)
        {
          // Syntax error, multi-actions cannot be hiden
          mCRL2log(log::error) << "List of hide action contains multi-action." << std::endl;
          throw mcrl2::runtime_error("Could not parse file " + filename + ".");
        }
        
        // Trim action and add to list of hiden actions
        trim(action_label);
        hiden.push_back(action_label);
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

  std::vector<std::vector<std::string>> blocks;
  std::vector<std::vector<std::string>> allow;
  std::vector<std::string> hiden;
  std::vector<std::pair<std::string, std::vector<std::string>>> syncs{};

};

} // namespace mcrl2

int main(int argc, char** argv)
{
  return mcrl2::ltscombine_tool().execute(argc, argv);
}
