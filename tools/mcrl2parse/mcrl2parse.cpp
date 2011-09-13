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

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "mcrl2/lps/specification.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "parser.h"

#ifdef MCRL2_PARSER_ACTIONS
#include "data_actions.h"
#endif

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

extern "C" {
  extern D_ParserTables parser_tables_mcrl2;
}

class mcrl2parse_tool : public input_tool
{
  typedef input_tool super;

  protected:
    typedef enum {
      mcrl2spec_e,
      besspec_e,
      pbesspec_e,
      dataspec_e,
      besexpr_e,
      dataexpr_e,
      pbesexpr_e,
      procexpr_e,
      actfrm_e,
      regfrm_e,
      statefrm_e
    } file_type_t;

    file_type_t file_type;
    bool partial_parses;
    bool print_tree;

    void set_file_type(const std::string& type)
    {
           if (type == "mcrl2spec")   { file_type = mcrl2spec_e; }
      else if (type == "besspec"  )   { file_type = besspec_e  ; }
      else if (type == "pbesspec" )   { file_type = pbesspec_e ; }
      else if (type == "dataspec" )   { file_type = dataspec_e ; }
      else if (type == "besexpr"  )   { file_type = besexpr_e  ; }
      else if (type == "dataexpr" )   { file_type = dataexpr_e ; }
      else if (type == "pbesexpr" )   { file_type = pbesexpr_e ; }
      else if (type == "procexpr" )   { file_type = procexpr_e ; }
      else if (type == "actfrm"   )   { file_type = actfrm_e   ; }
      else if (type == "regfrm"   )   { file_type = regfrm_e   ; }
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
             "  'mcrl2spec' for an mCRL2 specification (default)\n"
             "  'besspec'   for a BES specification\n"
             "  'pbesspec'  for a PBES specification\n"
             "  'dataspec'  for a data specification\n"
             "  'besexpr'   for a BES expression\n"
             "  'dataexpr'  for a data expression\n"
             "  'pbesexpr'  for a PBES expression\n"
             "  'procexpr'  for a process expression\n"
             "  'actfrm'    for an action formula\n"
             "  'regfrm'    for a regular formula\n"
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

      dparser::parser p(parser_tables_mcrl2);
      unsigned int start_symbol_index = 0;
      switch(file_type)
      {
        case mcrl2spec_e: { start_symbol_index = p.start_symbol_index("mCRL2Spec"); break; }
        case besspec_e  : { start_symbol_index = p.start_symbol_index("BesSpec"); break; }
        case pbesspec_e : { start_symbol_index = p.start_symbol_index("PbesSpec"); break; }
        case dataspec_e : { start_symbol_index = p.start_symbol_index("DataSpec"); break; }
        case besexpr_e  : { start_symbol_index = p.start_symbol_index("BesExpr"); break; }
        case dataexpr_e : { start_symbol_index = p.start_symbol_index("DataExpr"); break; }
        case pbesexpr_e : { start_symbol_index = p.start_symbol_index("PbesExpr"); break; }
        case procexpr_e : { start_symbol_index = p.start_symbol_index("ProcExpr"); break; }
        case actfrm_e   : { start_symbol_index = p.start_symbol_index("ActFrm"); break; }
        case regfrm_e   : { start_symbol_index = p.start_symbol_index("RegFrm"); break; }
        case statefrm_e : { start_symbol_index = p.start_symbol_index("StateFrm"); break; }
      }

      try
      {
        dparser::parse_node node = p.parse(text, start_symbol_index, partial_parses);

#ifdef MCRL2_PARSER_ACTIONS
        if (file_type == dataexpr_e)
        {
          data::data_actions actions(p.symbol_table());
          data::data_expression d = actions.parse_DataExpr(node);
        }
#endif

        std::cout << "Parsing successful." << std::endl;
        if (print_tree)
        {
          p.print_tree(node);
          //p.print_symbol_table();
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
