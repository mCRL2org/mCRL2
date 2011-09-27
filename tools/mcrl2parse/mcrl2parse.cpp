// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2parse.cpp
/// \brief tool for testing the new parser

#include "boost.hpp" // precompiled headers

#define TOOLNAME "mcrl2parse"
#define AUTHOR "Wieger Wesselink"
#define MCRL2_USE_NEW_PARSER

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "mcrl2/lps/specification.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/print.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/print.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/bes/parse.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/pbes/typecheck.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/modal_formula/typecheck.h"
#include "mcrl2/process/typecheck.h"
#include "mcrl2/data/typecheck.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class mcrl2parse_tool : public input_tool
{
  typedef input_tool super;

  protected:
    typedef enum {
      actfrm_e,
      besexpr_e,
      besspec_e,
      dataexpr_e,
      dataspec_e,
      mcrl2spec_e,
      pbesexpr_e,
      pbesspec_e,
      procexpr_e,
      regfrm_e,
      sortexpr_e,
      statefrm_e
    } file_type_t;

    file_type_t file_type;
    bool partial_parses;
    bool print_tree;
    bool use_new_parser;

    void set_file_type(const std::string& type)
    {
      if      (type == "actfrm"   )   { file_type = actfrm_e   ; }
      else if (type == "besexpr"  )   { file_type = besexpr_e  ; }
      else if (type == "besspec"  )   { file_type = besspec_e  ; }
      else if (type == "dataexpr" )   { file_type = dataexpr_e ; }
      else if (type == "dataspec" )   { file_type = dataspec_e ; }
      else if (type == "mcrl2spec")   { file_type = mcrl2spec_e; }
      else if (type == "pbesexpr" )   { file_type = pbesexpr_e ; }
      else if (type == "pbesspec" )   { file_type = pbesspec_e ; }
      else if (type == "procexpr" )   { file_type = procexpr_e ; }
      else if (type == "regfrm"   )   { file_type = regfrm_e   ; }
      else if (type == "sortexpr" )   { file_type = sortexpr_e ; }
      else if (type == "statefrm" )   { file_type = statefrm_e ; }
      else
      {
        throw std::runtime_error("unknown file type specified (got `" + type + "')");
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc
        .add_option("filetype",
           make_optional_argument("NAME", "mcrl2spec"),
             "input has the file type NAME:\n"
             "  'actfrm'    for an action formula\n"
             "  'besexpr'   for a BES expression\n"
             "  'besspec'   for a BES specification\n"
             "  'dataexpr'  for a data expression\n"
             "  'dataspec'  for a data specification\n"
             "  'mcrl2spec' for an mCRL2 specification (default)\n"
             "  'pbesexpr'  for a PBES expression\n"
             "  'pbesspec'  for a PBES specification\n"
             "  'procexpr'  for a process expression\n"
             "  'regfrm'    for a regular formula\n"
             "  'sortexpr'  for a sort expression\n"
             "  'statefrm'  for a state formula\n"
             ,
             'f'
           )
        .add_option("partial-parses",
           make_optional_argument("NAME", "0"),
             "allow partial parses (default: false)",
             'p'
           )
        .add_option("print-tree",
           make_optional_argument("NAME", "0"),
             "print parse tree (default: false)",
             't'
           )
        .add_option("use-new-parser",
           make_optional_argument("NAME", "1"),
             "use new parser (default: true)",
             'n'
           )
        ;
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      try
      {
        set_file_type(parser.option_argument("filetype"));
      }
      catch (std::logic_error)
      {
        set_file_type("mcrl2");
      }
      partial_parses = parser.option_argument_as<bool>("partial-parses");
      print_tree = parser.option_argument_as<bool>("print-tree");
      use_new_parser = parser.option_argument_as<bool>("use-new-parser");
    }

    std::string read_text(std::istream& from)
    {
      std::ostringstream out;
      std::string s;
      while (std::getline(from, s))
      {
        out << s << std::endl;
      }
      return out.str();
    }

  public:

    mcrl2parse_tool() : super(
        TOOLNAME,
        AUTHOR,
        "parses a string containing an mCRL2 data structure",
        "Parses the text in the file INFILE. If INFILE is not present, standard input is used."
       )
    { }

    void print1(const atermpp::aterm_appl& x)
    {
      if (x)
      {
        std::cout << "new: " << x << std::endl;
      }
    }

    void print2(const atermpp::aterm_appl& x)
    {
      if (x)
      {
        std::cout << "old: " << x << std::endl;
      }
    }

    void print1(const std::string& x)
    {
      std::cout << "new: " << x << std::endl;
    }

    void print2(const std::string& x)
    {
      std::cout << "old: " << x << std::endl;
    }

    template <typename T>
    void compare(const T& x1, const T& x2)
    {
      if (x1 != x2)
      {
        print1(x1);
        print2(x2);
        std::cout << "ERROR: NOT EQUAL!";
      }
      else
      {
        print1(x1);
      }
    }

    bool run()
    {
      std::string text;
      if (input_filename().empty())
      {
        text = read_text(std::cin);
      }
      else
      {
        std::ifstream from(input_filename().c_str());
        text = read_text(from);
      }

      core::parser p(parser_tables_mcrl2);
      unsigned int start_symbol_index = 0;
      switch(file_type)
      {
        case actfrm_e   : { start_symbol_index = p.start_symbol_index("ActFrm"); break; }
        case besexpr_e  : { start_symbol_index = p.start_symbol_index("BesExpr"); break; }
        case besspec_e  : { start_symbol_index = p.start_symbol_index("BesSpec"); break; }
        case dataexpr_e : { start_symbol_index = p.start_symbol_index("DataExpr"); break; }
        case dataspec_e : { start_symbol_index = p.start_symbol_index("DataSpec"); break; }
        case mcrl2spec_e: { start_symbol_index = p.start_symbol_index("mCRL2Spec"); break; }
        case pbesexpr_e : { start_symbol_index = p.start_symbol_index("PbesExpr"); break; }
        case pbesspec_e : { start_symbol_index = p.start_symbol_index("PbesSpec"); break; }
        case procexpr_e : { start_symbol_index = p.start_symbol_index("ProcExpr"); break; }
        case regfrm_e   : { start_symbol_index = p.start_symbol_index("RegFrm"); break; }
        case sortexpr_e : { start_symbol_index = p.start_symbol_index("SortExpr"); break; }
        case statefrm_e : { start_symbol_index = p.start_symbol_index("StateFrm"); break; }
      }

      try
      {
        atermpp::aterm_appl x1;
        atermpp::aterm_appl x2;

        if (print_tree)
        {
          core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
          p.print_tree(node);
        }
        if (file_type == sortexpr_e)
        {
          if (use_new_parser) x1 = data::parse_sort_expression_new(text);
          x2 = data::parse_sort_expression(text);
          compare (x1, x2);
        }
        else if (file_type == dataexpr_e)
        {
          if (use_new_parser) x1 = data::parse_data_expression_new(text);
          x2 = data::parse_data_expression(text);
          compare (x1, x2);
        }
        else if (file_type == dataspec_e)
        {
          if (use_new_parser) x1 = data::detail::data_specification_to_aterm_data_spec(data::parse_data_specification_new(text));
          x2 = data::detail::data_specification_to_aterm_data_spec(data::parse_data_specification(text));
          compare (x1, x2);
        }
        else if (file_type == procexpr_e)
        {
          if (use_new_parser) x1 = process::parse_process_expression_new(text);
          print1(x1);
          x2 = process::parse_process_expression(text, "", "");
          compare (x1, x2);
        }
        else if (file_type == mcrl2spec_e)
        {
          if (use_new_parser) x1 = process::process_specification_to_aterm(process::parse_process_specification_new(text));
          x2 = process::process_specification_to_aterm(process::parse_process_specification(text));
          compare (x1, x2);
          if (use_new_parser)
          {
            process::process_specification spec1(x1, false);
            process::type_check(spec1);
            process::translate_user_notation(spec1);
            process::normalize_sorts(spec1, spec1.data());
            process::process_specification spec2(x2, false);
            compare(process::pp(spec1), process::pp(spec2));
          }
        }
        else if (file_type == besexpr_e)
        {
          bes::boolean_expression x1 = bes::parse_boolean_expression_new(text);
          std::cout << bes::pp(x1) << std::endl;
        }
        else if (file_type == besspec_e)
        {
          bes::boolean_equation_system<> x1 = bes::parse_boolean_equation_system_new(text);
          std::cout << bes::pp(x1) << std::endl;
        }
        else if (file_type == pbesexpr_e)
        {
          if (use_new_parser) x1 = pbes_system::parse_pbes_expression_new(text);
          x2 = pbes_system::parse_pbes_expression(text, "", "");
          compare (x1, x2);
        }
        else if (file_type == pbesspec_e)
        {
          x1 = pbes_system::pbes_to_aterm(pbes_system::parse_pbes_new(text));
          x2 = pbes_system::pbes_to_aterm(pbes_system::parse_pbes(text));
          compare (x1, x2);
        }
        else if (file_type == actfrm_e)
        {
          action_formulas::action_formula x1 = action_formulas::parse_action_formula_new(text);
          std::cout << x1 << std::endl;
        }
        else if (file_type == regfrm_e)
        {
          regular_formulas::regular_formula x1 = regular_formulas::parse_regular_formula_new(text);
          std::cout << x1 << std::endl;
        }
        else if (file_type == statefrm_e)
        {
          if (use_new_parser) x1 = state_formulas::parse_state_formula_new(text);
          x2 = state_formulas::parse_state_formula_old(text);
          compare (x1, x2);
        }
      }
      catch (std::exception& e)
      {
        std::cout << "Exception during parsing: " << e.what() << std::endl;
      }

      return true;
    }

};

class mcrl2parse_gui_tool: public mcrl2_gui_tool<mcrl2parse_tool>
{
  public:
    mcrl2parse_gui_tool() {}
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return mcrl2parse_gui_tool().execute(argc, argv);
}
