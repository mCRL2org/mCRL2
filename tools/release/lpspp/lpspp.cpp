// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpspp.cpp

#include "mcrl2/lps/io.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;

class lpspp_tool: public input_output_tool
{
  private:
    using super = input_output_tool;

  public:
    lpspp_tool()
      : super("lpspp", "Aad Mathijssen and Jeroen Keiren",
              "pretty print an LPS",
              "Print the mCRL2 LPS in INFILE to OUTFILE in a human readable format. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    bool run() override
    {
      lps::stochastic_specification spec;
      load_lps(spec, input_filename());

      mCRL2log(log::verbose) << "printing LPS from "
                        << (input_filename().empty()?"standard input":input_filename())
                        << " to " << (output_filename().empty()?"standard output":output_filename())
                        << " in the " << core::pp_format_to_string(m_format) << " format" << std::endl;

      std::string text;
      if (m_format == core::print_internal)
      {
        text = pp(specification_to_aterm(spec));
      }
      else
      {
        text = lps::pp_extended(spec, m_process_name, m_precedence_aware, m_print_summand_numbers);
      }
      if (output_filename().empty())
      {
        std::cout << text;
      }
      else
      {
        std::ofstream output_stream(output_filename().c_str());
        if (output_stream)
        {
          output_stream << text;
          output_stream.close();
        }
        else
        {
          throw mcrl2::runtime_error("could not open output file " + output_filename() + " for writing");
        }
      }
      return true;
    }

  protected:
    print_format_type m_format = print_default;
    bool m_print_summand_numbers = false;
    bool m_precedence_aware = true;
    std::string m_process_name = "P";

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("format", make_enum_argument<print_format_type>("FORMAT")
                      .add_value_desc(print_default, "for a process specification", true)
                      .add_value_desc(print_internal, "for a textual ATerm representation of the internal format"),
                      "print the LPS in the specified FORMAT:", 'f');
      desc.add_option("print-summand_numbers", "print numbers in front of summands", 'n');
      desc.add_option("no-precedence-aware", "disable printing with precedence aware enabled", 'x');
      desc.add_option("process-name", make_mandatory_argument("NAME"), "the name used for the pretty printed process name (default='P')", 'm');
    }

    void parse_options(const command_line_parser& parser) override
    {
      input_output_tool::parse_options(parser);
      m_format = parser.option_argument_as<print_format_type>("format");
      m_print_summand_numbers = parser.options.count("print-summand_numbers") > 0;
      m_precedence_aware = parser.options.count("no-precedence-aware") == 0;

      if (parser.has_option("process-name"))
      {
        m_process_name = parser.option_argument("process-name");
      }
    }
};

int main(int argc, char* argv[])
{
  return lpspp_tool().execute(argc, argv);
}
