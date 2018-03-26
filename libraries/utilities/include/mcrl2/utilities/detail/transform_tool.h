// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/detail/transform_tool.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_DETAIL_TRANSFORM_TOOL_H
#define MCRL2_UTILITIES_DETAIL_TRANSFORM_TOOL_H

#include "mcrl2/utilities/detail/command.h"
#include "mcrl2/utilities/tool.h"

namespace mcrl2 {

namespace utilities {

namespace detail {

template <typename Tool>
class transform_tool: public Tool
{
  protected:
    typedef Tool super;

    std::string algorithm_and_options;
    int algorithm_number = -1;
    bool print_algorithms = false;
    std::map<std::string, std::shared_ptr<utilities::detail::command>> commands;

    void parse_options(const utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      algorithm_and_options = parser.option_argument("algorithm");
      algorithm_number = parser.option_argument_as<int>("number");
      print_algorithms = parser.options.count("print-algorithms") > 0;
    }

    void add_options(utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("algorithm", utilities::make_optional_argument<std::string>("NAME", ""), "the algorithm that is to be applied", 'a');
      desc.add_option("number", utilities::make_optional_argument<int>("NAME", "-1"), "the number of the algorithm that is to be applied", 'n');
      desc.add_option("print-algorithms", "print the available algorithms", 'p');
    }

    inline
    void add_command(const std::shared_ptr<utilities::detail::command>& command)
    {
      commands[command->name] = command;
    }

    virtual void add_commands(const std::vector<std::string>& options) = 0;

  public:
    transform_tool(const std::string& name,
                   const std::string& author,
                   const std::string& what_is,
                   const std::string& tool_description,
                   std::string known_issues = ""
                  )
      : Tool(name, author, what_is, tool_description, known_issues)
    {}

    bool run()
    {
      std::vector<std::string> options;
      std::set<std::string> algorithms;
      std::string algorithm;

      if (algorithm_number < 0)
      {
        options = utilities::regex_split(algorithm_and_options, "\\s+");
        if (!options.empty())
        {
          algorithm = options[0];
          options.erase(options.begin());
        }
      }

      add_commands(options);

      for (auto i = commands.begin(); i != commands.end(); ++i)
      {
        algorithms.insert(i->first);
      }

      if (algorithm_number >= 0 && !algorithm_and_options.empty())
      {
        throw mcrl2::runtime_error("It is not allowed to set both number and algorithm!");
      }

      // print the algorithms
      if (print_algorithms || (algorithm_number < 0 && algorithm_and_options.empty()))
      {
        int index = 1;
        std::cout << "The following algorithms are available:" << std::endl;
        for (auto const& algorithm: algorithms)
        {
          std::cout << index++ << ") " << algorithm << std::endl;
        }
        return true;
      }

      // if a number was specified, lookup the corresponding algorithm
      if (algorithm_number >= 0)
      {
        int index = 1;
        for (auto const& algo: algorithms)
        {
          if (index++ == algorithm_number)
          {
            algorithm = algo;
          }
        }
      }

      // run the algorithm
      auto i = commands.find(algorithm);
      if (i == commands.end())
      {
        throw std::runtime_error("Unknown algorithm " + algorithm);
      }
      i->second->execute();

      return true;
    }
};

} // namespace detail

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_DETAIL_TRANSFORM_TOOL_H
