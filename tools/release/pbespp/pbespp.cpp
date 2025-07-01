// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbespp.cpp

#define NAME "pbespp"
#define AUTHOR "Aad Mathijssen and Jeroen Keiren"

#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/detail/pfnf_print.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2;
using pbes_system::tools::pbes_input_tool;

//local declarations

class pbespp_tool: public pbes_input_tool<input_output_tool>
{
  private:
    typedef pbes_input_tool<input_output_tool> super;

  public:
    pbespp_tool()
      : super(NAME, AUTHOR,
              "pretty print a PBES",
              "Print the PBES in INFILE to OUTFILE in a human readable format. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             ),
      format(core::print_default)
    {}

    bool run() override
    {
      pbes_system::pbes p;
      load_pbes(p, input_filename(), pbes_input_format());

      mCRL2log(log::verbose) << "printing PBES from "
                            << (input_filename().empty()?"standard input":input_filename())
                            << " to " << (output_filename().empty()?"standard output":output_filename())
                            << " in the " << core::pp_format_to_string(format) << " format" << std::endl;

      if (output_filename().empty())
      {
        if (format == core::print_internal)
        {
          std::cout << pbes_to_aterm(p);
        }
        else if(use_pfnf_printer && mcrl2::pbes_system::detail::is_pfnf(p))
        {
          std::cout << pfnf_pp(p);
        }
        else
        {
          std::cout << pp(p);
        }
      }
      else
      {
        std::ofstream out(output_filename().c_str());
        if (out)
        {
          if (format == core::print_internal)
          {
            out << pbes_to_aterm(p);
          }
          else if(use_pfnf_printer && mcrl2::pbes_system::detail::is_pfnf(p))
          {
            out << pfnf_pp(p);
          }
          else
          {
            out << pp(p);
          }
          out.close();
        }
        else
        {
          throw mcrl2::runtime_error("could not open output file " + output_filename() + " for writing");
        }
      }
      return true;
    }

  protected:
    core::print_format_type  format;
    bool use_pfnf_printer = false;

    void add_options(interface_description& desc) override
    {
      input_output_tool::add_options(desc);
      desc.add_option("format", make_enum_argument<core::print_format_type>("FORMAT")
                      .add_value_desc(core::print_default, "a PBES specification", true)
                      .add_value_desc(core::print_internal, "a textual ATerm representation of the internal format"),
                      "print the PBES in the specified FORMAT:", 'f');
      desc.add_option("pfnf-printer",
                      "format the output according to the structure of PFNF (only has an effect when printing a PBES in PFNF to text)", 
                      'p');
    }

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      format = parser.option_argument_as<core::print_format_type>("format");
      if (parser.options.count("pfnf-printer")>0)
      {
        use_pfnf_printer = true;
      }
    }
};

int main(int argc, char* argv[])
{
  return pbespp_tool().execute(argc, argv);
}

