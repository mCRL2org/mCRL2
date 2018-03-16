// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2typecheck.cpp

#include <iostream>
#include <string>

#include "mcrl2/data/parse.h"
#include "mcrl2/data/traverser.h"
#include "mcrl2/utilities/detail/separate_keyword_section.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2;

struct print_sub_expressions_traverser: public data::data_expression_traverser<print_sub_expressions_traverser>
{
  typedef data::data_expression_traverser<print_sub_expressions_traverser> super;

  template <typename T>
  void leave(const T& x,
             typename std::enable_if<std::is_base_of<data::data_expression, T>::value>::type* = nullptr
            )
  {
    std::cout << "    " << x << " : " << x.sort() << std::endl;
  }

  template <typename T>
  void leave(const T& /* x */,
             typename std::enable_if<!std::is_base_of<data::data_expression, T>::value>::type* = nullptr
            )
  {}
};

inline
data::variable_vector parse_data_variables(const std::string& text, const data::data_specification& dataspec)
{
  data::variable_vector result;
  if (utilities::trim_copy(text).empty())
  {
    return result;
  }
  data::parse_variables(text, std::back_inserter(result), dataspec);
  return result;
}

inline
std::vector<std::string> mcrl2_keywords()
{
  return { "sort", "var", "eqn", "map", "cons" };
}

inline
std::vector<std::string> section_keywords()
{
  return { "data_expressions", "data_variables", "data_specification" };
}

inline
std::string keyword_section(std::string& text, const std::string& keyword, const std::vector<std::string>& keywords)
{
  auto q = utilities::detail::separate_keyword_section(text, keyword, keywords);
  text = q.second;
  std::string result = q.first;
  boost::algorithm::trim(result);
  result = utilities::regex_replace("^\\s*" + keyword + "\\s*", std::string(""), result);
  return result;
}

inline
std::vector<std::string> split_lines(const std::string& text)
{
  auto lines = utilities::regex_split(text, "\\n");
  for (std::string& line: lines)
  {
    utilities::trim(line);
  }
  lines.erase(std::remove_if(lines.begin(), lines.end(), [](const std::string& s) { return s.empty(); }), lines.end());
  return lines;
}

inline
void rewrite_expressions(std::string text, bool print_subterms)
{
  text = utilities::regex_replace("^%[^\\n]*\n", "", text);
  auto keywords = section_keywords();
  auto dataspec_text = keyword_section(text, "data_specification", keywords);
  auto data_variables_text = split_lines(keyword_section(text, "data_variables", keywords));
  auto data_expressions_text = split_lines(keyword_section(text, "data_expressions", keywords));

  data::data_specification dataspec;
  if (!dataspec_text.empty())
  {
    dataspec = data::parse_data_specification(dataspec_text);
  }

  std::vector<data::variable> data_variables;
  for (const std::string& expr: data_variables_text)
  {
    data_variables.push_back(data::parse_variable(expr, dataspec));
  }

  for (const std::string& expr: data_expressions_text)
  {
    data::data_expression x = data::parse_data_expression(expr, data_variables, dataspec);
    std::cout << expr << " : " << x.sort() << std::endl;
    if (print_subterms)
    {
      print_sub_expressions_traverser f;
      f.apply(x);
    }
  }
}

class mcrl2typecheck_tool: public utilities::tools::input_tool
{
  protected:
    typedef utilities::tools::input_tool super;
    bool print_subterms = false;

    void parse_options(const utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      print_subterms = parser.options.count("print-subterms") > 0;
    }

    void add_options(utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("print-subterms", "print the sorts of subterms", 's');
    }

  public:
    mcrl2typecheck_tool()
      : super("mcrl2typecheck",
              "Wieger Wesselink",
              "applies the mcrl2 type checker to data expressions",
              "Takes a data specification and a file with data expressions as input."
              "Applies the type checker to the expressions."
             )
    {}

    bool run()
    {
      std::string text = utilities::read_text(input_filename());
      rewrite_expressions(text, print_subterms);
      return true;
    }
};

int main(int argc, char* argv[])
{
  return mcrl2typecheck_tool().execute(argc, argv);
}
