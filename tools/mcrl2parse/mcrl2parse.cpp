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

#define TOOLNAME "mcrl2parse"
#define AUTHOR "Wieger Wesselink"

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include <boost/algorithm/string/trim.hpp>

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/parse.h"
#include "mcrl2/bes/print.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/detail/dparser_functions.h"
#include "mcrl2/core/parser_utility.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/modal_formula/typecheck.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/print.h"
#include "mcrl2/pbes/typecheck.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/print.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/typecheck.h"
#include "mcrl2/utilities/detail/separate_keyword_section.h"
#include "mcrl2/utilities/input_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

// ---------------------------------------------------------------------------------------
//       function                |     additional inputs              | keywords
// -------------------------------------------------------------------|-------------------
// parse_sort_expression         | data_specification                 | sortexpr
// parse_data_expression         | data_specification variable_vector | dataexpr variables
// parse_data_specification      |                                    |
// parse_process_expression      | process_specification              | procexpr
// parse_process_specification   |                                    |
// parse_boolean_expression      |                                    |
// parse_boolean_equation_system |                                    |
// parse_pbes_expression         | pbes_specification                 | pbesexpr
// parse_pbes                    |                                    |
// parse_state_formula           | action_specification               | statefrm
// parse_action_formula          | action_specification               | actfrm
// parse_regular_formula         | action_specification               | regfrm
// parse_multi_action            | action_specification               | multact

inline
std::vector<std::string> data_specification_keywords()
{
  std::vector<std::string> result;
  result.push_back("sort");
  result.push_back("var");
  result.push_back("eqn");
  result.push_back("map");
  result.push_back("cons");
  return result;
}

inline
std::vector<std::string> action_specification_keywords()
{
  std::vector<std::string> result;
  result.push_back("sort");
  result.push_back("var");
  result.push_back("eqn");
  result.push_back("map");
  result.push_back("cons");
  result.push_back("act");
  result.push_back("glob");
  result.push_back("proc");
  result.push_back("init");
  return result;
}

inline
std::vector<std::string> process_specification_keywords()
{
  return action_specification_keywords();
}

inline
std::vector<std::string> pbes_specification_keywords()
{
  std::vector<std::string> result;
  result.push_back("sort");
  result.push_back("var");
  result.push_back("eqn");
  result.push_back("map");
  result.push_back("cons");
  result.push_back("glob");
  result.push_back("pbes");
  result.push_back("init");
  return result;
}

inline
bool has_keyword(const std::string& text, const std::string& keyword)
{
  // inefficient implementation, to avoid the introduction of a regex search function
  std::vector<std::string> words = utilities::regex_split("dummy " + text, "\\b" + keyword + "\\b");
  return words.size() > 1;
}

inline
void separate_data_specification(const std::string& text, const std::string& keyword, data::data_specification& dataspec, std::string& keyword_text)
{
  if (!has_keyword(text, keyword))
  {
    keyword_text = text;
    dataspec = data::data_specification();
  }
  else
  {
    std::pair<std::string, std::string> result = utilities::detail::separate_keyword_section(text, keyword, data_specification_keywords());
    keyword_text = result.first.substr(keyword.size());
    dataspec = data::parse_data_specification(result.second);
  }
}

inline
void separate_data_specification(const std::string& text, const std::string& keyword1, const std::string& keyword2, data::data_specification& dataspec, std::string& keyword1_text, std::string& keyword2_text)
{
  std::string data_spec_text;
  if (!has_keyword(text, keyword1))
  {
    keyword1_text = text;
    keyword2_text = "";
  }
  else
  {
    std::vector<std::string> keywords = data_specification_keywords();
    keywords.push_back(keyword1);

    std::pair<std::string, std::string> result = utilities::detail::separate_keyword_section(text, keyword1, keywords);
    keyword1_text = result.first.substr(keyword1.size());
    data_spec_text = result.second;
    if (has_keyword(result.second, keyword2))
    {
      result = utilities::detail::separate_keyword_section(result.second, keyword2, data_specification_keywords());
      keyword2_text = result.first.substr(keyword2.size());
      data_spec_text = result.second;
    }
    else
    {
      keyword2_text = "";
    }
    dataspec = data::parse_data_specification(data_spec_text);
  }
}

inline
void separate_action_specification(const std::string& text, const std::string& keyword, lps::specification& spec, std::string& keyword_text)
{
  if (!has_keyword(text, keyword))
  {
    keyword_text = text;
    spec = lps::specification();
  }
  else
  {
    std::pair<std::string, std::string> result = utilities::detail::separate_keyword_section(text, keyword, action_specification_keywords());
    if (!has_keyword(result.second, "init"))
    {
      result.second = result.second + "\ninit delta;\n";
    }
    keyword_text = result.first.substr(keyword.size());
    spec=remove_stochastic_operators(lps::linearise(result.second));
  }
}

inline
void separate_process_specification(const std::string& text, const std::string& keyword, process::process_specification& procspec, std::string& keyword_text)
{
  if (!has_keyword(text, keyword))
  {
    keyword_text = text;
    procspec = process::process_specification();
  }
  else
  {
    std::pair<std::string, std::string> result = utilities::detail::separate_keyword_section(text, keyword, process_specification_keywords());
    keyword_text = result.first.substr(keyword.size());
    std::string ptext = result.second;
    if (!has_keyword(ptext, "init"))
    {
      ptext = ptext + "\ninit delta;";
    }
    procspec = process::parse_process_specification(ptext, false);
  }
}

inline
void separate_pbes_specification(const std::string& text, const std::string& keyword, pbes_system::pbes& pbesspec, std::string& keyword_text)
{
  if (!has_keyword(text, keyword))
  {
    keyword_text = text;
    pbesspec = pbes_system::pbes();
  }
  else
  {
    std::pair<std::string, std::string> result = utilities::detail::separate_keyword_section(text, keyword, pbes_specification_keywords());
    keyword_text = result.first.substr(keyword.size());
    pbesspec = pbes_system::parse_pbes(result.second);
  }
}

inline
data::variable_vector parse_data_variables(const std::string& text, const data::data_specification& dataspec)
{
  data::variable_vector result;
  if (boost::algorithm::trim_copy(text).empty())
  {
    return result;
  }
  data::parse_variables(text, std::back_inserter(result), dataspec);
  return result;
}

typedef enum {
  actfrm_e,
  besexpr_e,
  besspec_e,
  dataexpr_e,
  dataspec_e,
  mcrl2spec_e,
  multact_e,
  pbesexpr_e,
  pbesspec_e,
  procexpr_e,
  regfrm_e,
  sortexpr_e,
  statefrm_e
} file_type_t;

inline
file_type_t parse_file_type(const std::string& type)
{
  if      (type == "actfrm"   )   { return actfrm_e   ; }
  else if (type == "besexpr"  )   { return besexpr_e  ; }
  else if (type == "besspec"  )   { return besspec_e  ; }
  else if (type == "dataexpr" )   { return dataexpr_e ; }
  else if (type == "dataspec" )   { return dataspec_e ; }
  else if (type == "mcrl2spec")   { return mcrl2spec_e; }
  else if (type == "multact"  )   { return multact_e  ; }
  else if (type == "pbesexpr" )   { return pbesexpr_e ; }
  else if (type == "pbesspec" )   { return pbesspec_e ; }
  else if (type == "procexpr" )   { return procexpr_e ; }
  else if (type == "regfrm"   )   { return regfrm_e   ; }
  else if (type == "sortexpr" )   { return sortexpr_e ; }
  else if (type == "statefrm" )   { return statefrm_e ; }
  else
  {
    throw mcrl2::runtime_error("unknown file type specified (got `" + type + "')");
  }
}

inline
std::string print_file_type(const file_type_t type)
{
  switch(type)
  {
    case actfrm_e: return "actfrm";
    case besexpr_e: return "besexpr";
    case besspec_e: return "besspec";
    case dataexpr_e: return "dataexpr";
    case dataspec_e: return "dataspec";
    case mcrl2spec_e: return "mcrl2spec";
    case multact_e: return "multact";
    case pbesexpr_e: return "pbesexpr";
    case pbesspec_e: return "pbesspec";
    case procexpr_e: return "procexpr";
    case regfrm_e: return "regfrm";
    case sortexpr_e: return "sortexpr";
    case statefrm_e: return "statefrm";
  }
  throw mcrl2::runtime_error("unknown file type");
}

inline
std::string description(const file_type_t type)
{
  switch(type)
  {
    case actfrm_e: return "for an action formula";
    case besexpr_e: return "for a BES expression";
    case besspec_e: return "for a BES specification";
    case dataexpr_e: return "for a data expression";
    case dataspec_e: return "for a data specification";
    case mcrl2spec_e: return "for an mCRL2 specification";
    case multact_e: return "for a multi action";
    case pbesexpr_e: return "for a PBES expression";
    case pbesspec_e: return "for a PBES specification";
    case procexpr_e: return "for a process expression";
    case regfrm_e: return "for a regular formula";
    case sortexpr_e: return "for a sort expression";
    case statefrm_e: return "for a state formula";
  }
  throw mcrl2::runtime_error("unknown file type");
}

inline
std::istream& operator>>(std::istream& is, file_type_t& type)
{
  try
  {
    std::string s;
    is >> s;
    type = parse_file_type(s);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const file_type_t type)
{
  os << print_file_type(type);
  return os;
}

class mcrl2parse_tool : public input_tool
{
  typedef input_tool super;

  protected:
    std::string text;
    file_type_t file_type;
    bool partial_parses;
    bool print_tree;
    bool check_parser;
    bool check_printer;
    bool aterm_format;
    bool warn;

    bool dot;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("expression",
           make_optional_argument("EXPR", ""),
             "parse EXPR instead of a file",
             'e'
           );
      desc.add_option("filetype",
           make_enum_argument<file_type_t>("NAME")
                      .add_value(actfrm_e)
                      .add_value(besexpr_e)
                      .add_value(besspec_e)
                      .add_value(dataexpr_e)
                      .add_value(dataspec_e)
                      .add_value(mcrl2spec_e, true)
                      .add_value(multact_e)
                      .add_value(pbesexpr_e)
                      .add_value(pbesspec_e)
                      .add_value(procexpr_e)
                      .add_value(regfrm_e)
                      .add_value(sortexpr_e)
                      .add_value(statefrm_e),
             "input has the file type NAME:", 'f');
      desc.add_option("partial-parses", "allow partial parses");
      desc.add_option("print-tree", "print parse tree", 't');
      desc.add_option("check-parser", "compare the results of the old and new parser", 'p');
      desc.add_option("check-printer", "compare the results of the old and new pretty printer", 'P');
      desc.add_option("aterm-format", "compare the results in aterm format", 'a');
      desc.add_option("warn", "generate warnings", 'w');

      desc.add_option("dot", "load a dot file using the old and the new parser and check the result");
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      file_type = parser.option_argument_as<file_type_t>("filetype");
      partial_parses = 0 < parser.options.count("partial-parses");
      print_tree     = 0 < parser.options.count("print-tree");
      check_parser   = 0 < parser.options.count("check-parser");
      check_printer  = 0 < parser.options.count("check-printer");
      aterm_format   = 0 < parser.options.count("aterm-format");
      warn           = 0 < parser.options.count("warn");
      text = parser.option_argument("expression");
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
      if (text.empty())
      {
        if (input_filename().empty())
        {
          text = read_text(std::cin);
        }
        else
        {
          std::ifstream from(input_filename().c_str());
          text = read_text(from);
        }
      }

      data::data_specification dataspec;
      lps::specification lpsspec;
      process::process_specification procspec;
      pbes_system::pbes pbesspec;

      core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
      unsigned int start_symbol_index = 0;
      switch(file_type)
      {
        case actfrm_e   : { start_symbol_index = p.start_symbol_index("ActFrm"); break; }
        case besexpr_e  : { start_symbol_index = p.start_symbol_index("BesExpr"); break; }
        case besspec_e  : { start_symbol_index = p.start_symbol_index("BesSpec"); break; }
        case dataexpr_e : { start_symbol_index = p.start_symbol_index("DataExpr"); break; }
        case dataspec_e : { start_symbol_index = p.start_symbol_index("DataSpec"); break; }
        case mcrl2spec_e: { start_symbol_index = p.start_symbol_index("mCRL2Spec"); break; }
        case multact_e  : { start_symbol_index = p.start_symbol_index("MultAct"); break; }
        case pbesexpr_e : { start_symbol_index = p.start_symbol_index("PbesExpr"); break; }
        case pbesspec_e : { start_symbol_index = p.start_symbol_index("PbesSpec"); break; }
        case procexpr_e : { start_symbol_index = p.start_symbol_index("ProcExpr"); break; }
        case regfrm_e   : { start_symbol_index = p.start_symbol_index("RegFrm"); break; }
        case sortexpr_e : { start_symbol_index = p.start_symbol_index("SortExpr"); break; }
        case statefrm_e : { start_symbol_index = p.start_symbol_index("StateFrm"); break; }
      }

      try
      {
        if (print_tree)
        {
          core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
          p.print_tree(node);
        }

        if (warn)
        {
          core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
          core::warn_and_or(node);
          core::warn_left_merge_merge(node);
        }

        if (check_parser)
        {
          switch(file_type)
          {
            case actfrm_e   :
            {
              separate_action_specification(text, "actfrm", lpsspec, text);
              action_formulas::action_formula x = action_formulas::detail::parse_action_formula_new(text);
              atermpp::aterm_appl a = x;
              if (aterm_format)
              {
                std::cout << a << std::endl;
              }
              else
              {
                std::cout << action_formulas::pp(x) << std::endl;
              }
              break;
            }
            case besexpr_e  :
            {
              bes::boolean_expression x = bes::detail::parse_boolean_expression_new(text);
              atermpp::aterm_appl a = x;
              if (aterm_format)
              {
                std::cout << a << std::endl;
              }
              else
              {
                std::cout << bes::pp(x) << std::endl;
              }
              break;
            }
            case besspec_e  :
            {
              bes::boolean_equation_system x = bes::detail::parse_boolean_equation_system_new(text);
              atermpp::aterm_appl a = bes::boolean_equation_system_to_aterm(x);
              if (aterm_format)
              {
                std::cout << a << std::endl;
              }
              else
              {
                std::cout << bes::pp(x) << std::endl;
              }
              break;
            }
            case dataexpr_e :
            {
              std::string variable_text;
              separate_data_specification(text, "dataexpr", "variables", dataspec, text, variable_text);
              data::variable_vector v = parse_data_variables(variable_text, dataspec);
              data::data_expression x = data::parse_data_expression(text, v.begin(), v.end(), dataspec);
              if (aterm_format)
              {
                std::cout << x << std::endl;
              }
              else
              {
                std::cout << data::pp(x) << std::endl;
              }
              break;
            }
            case dataspec_e :
            {
              data::data_specification x = data::parse_data_specification(text);
              if (aterm_format)
              {
                std::cout << data::detail::data_specification_to_aterm_data_spec(x) << std::endl;
              }
              else
              {
                std::cout << data::pp(x) << std::endl;
              }
              break;
            }
            case mcrl2spec_e:
            {
              process::process_specification x = process::detail::parse_process_specification_new(text);
              process::detail::complete_process_specification(x, false);
              if (aterm_format)
              {
                std::cout << process::process_specification_to_aterm(x) << std::endl;
              }
              else
              {
                std::cout << process::pp(x) << std::endl;
              }
              break;
            }
            case multact_e  :
            {
              separate_action_specification(text, "multact", lpsspec, text);
              process::untyped_multi_action y = lps::detail::parse_multi_action_new(text);
              lps::multi_action x = lps::detail::complete_multi_action(y, lpsspec.action_labels(), lpsspec.data());
              if (aterm_format)
              {
                std::cout << lps::detail::multi_action_to_aterm(x) << std::endl;
              }
              else
              {
                std::cout << lps::pp(x) << std::endl;
              }
              break;
            }
            case pbesexpr_e :
            {
              separate_pbes_specification(text, "pbesexpr", pbesspec, text);
              pbes_system::pbes_expression x = pbes_system::detail::parse_pbes_expression_new(text);
              if (aterm_format)
              {
                std::cout << x << std::endl;
              }
              else
              {
                std::cout << pbes_system::pp(x) << std::endl;
              }
              break;
            }
            case pbesspec_e :
            {
              pbes_system::pbes x = pbes_system::detail::parse_pbes_new(text).construct_pbes();
              pbes_system::detail::complete_pbes(x);
              if (aterm_format)
              {
                std::cout << pbes_system::pbes_to_aterm(x) << std::endl;
              }
              else
              {
                std::cout << pbes_system::pp(x) << std::endl;
              }
              break;
            }
            case procexpr_e :
            {
              separate_process_specification(text, "procexpr", procspec, text);
              process::process_expression x = process::detail::parse_process_expression_new(text);
              if (aterm_format)
              {
                std::cout << x << std::endl;
              }
              else
              {
                std::cout << process::pp(x) << std::endl;
              }
              break;
            }
            case regfrm_e   :
            {
              separate_action_specification(text, "regfrm", lpsspec, text);
              regular_formulas::regular_formula x = regular_formulas::detail::parse_regular_formula_new(text);
              if (aterm_format)
              {
                std::cout << x << std::endl;
              }
              else
              {
                std::cout << regular_formulas::pp(x) << std::endl;
              }
              break;
            }
            case sortexpr_e :
            {
              separate_data_specification(text, "sortexpr", dataspec, text);
              data::sort_expression x = data::parse_sort_expression(text, dataspec);
              if (aterm_format)
              {
                std::cout << x << std::endl;
              }
              else
              {
                std::cout << data::pp(x) << std::endl;
              }
              break;
            }
            case statefrm_e :
            {
              separate_action_specification(text, "statefrm", lpsspec, text);
              bool check_monotonicity = false;
              bool translate_regular = true;
              bool type_check = true;
              bool translate_user_notation = true;
              state_formulas::state_formula x = state_formulas::parse_state_formula(text, lpsspec, check_monotonicity, translate_regular, type_check, translate_user_notation);
              if (aterm_format)
              {
                std::cout << x << std::endl;
              }
              else
              {
                std::cout << state_formulas::pp(x) << std::endl;
              }
              break;
            }
          }
        }

        if (check_printer)
        {
          switch(file_type)
          {
            case actfrm_e   :
            {
              separate_action_specification(text, "actfrm", lpsspec, text);
              action_formulas::action_formula x = action_formulas::detail::parse_action_formula_new(text);
              std::cout << action_formulas::pp(x) << std::endl;
              break;
            }
            case besexpr_e  :
            {
              bes::boolean_expression x = bes::detail::parse_boolean_expression_new(text);
              std::cout << bes::pp(x) << std::endl;
              break;
            }
            case besspec_e  :
            {
              bes::boolean_equation_system x = bes::detail::parse_boolean_equation_system_new(text);
              std::cout << bes::pp(x) << std::endl;
              break;
            }
            case dataexpr_e :
            {
              std::string variable_text;
              separate_data_specification(text, "dataexpr", "variables", dataspec, text, variable_text);
              data::variable_vector v = parse_data_variables(variable_text, dataspec);
              data::data_expression x = data::parse_data_expression(text, v.begin(), v.end(), dataspec);
              std::cout << data::pp(x) << std::endl;
              break;
            }
            case dataspec_e :
            {
              data::data_specification x = data::parse_data_specification(text);
              std::cout << data::pp(x) << std::endl;
              break;
            }
            case mcrl2spec_e:
            {
              process::process_specification x = process::parse_process_specification(text, false);
              std::cout << process::pp(x) << std::endl;
              break;
            }
            case multact_e  :
            {
              separate_action_specification(text, "multact", lpsspec, text);
              lps::multi_action x = lps::parse_multi_action(text, lpsspec.action_labels(), lpsspec.data());
              std::cout << lps::pp(x) << std::endl;
              break;
            }
            case pbesexpr_e :
            {
              std::cout << "Warning: this test may fail due to the absence of a type checker" << std::endl;
              separate_pbes_specification(text, "pbesexpr", pbesspec, text);
              pbes_system::pbes_expression x = pbes_system::detail::parse_pbes_expression_new(text);
              std::cout << pbes_system::pp(x) << std::endl;
              break;
            }
            case pbesspec_e :
            {
              pbes_system::pbes x = pbes_system::parse_pbes(text);
              std::cout << pbes_system::pp(x) << std::endl;
              break;
            }
            case procexpr_e :
            {
              separate_process_specification(text, "procexpr", procspec, text);
              process::process_expression x = process::parse_process_expression(text, process::pp(procspec));
              std::cout << process::pp(x) << std::endl;
              break;
            }
            case regfrm_e   :
            {
              separate_action_specification(text, "regfrm", lpsspec, text);
              regular_formulas::regular_formula x = regular_formulas::detail::parse_regular_formula_new(text);
              std::cout << regular_formulas::pp(x) << std::endl;
              break;
            }
            case sortexpr_e :
            {
              separate_data_specification(text, "sortexpr", dataspec, text);
              data::sort_expression x = data::parse_sort_expression(text, dataspec);
              std::cout << data::pp(x) << std::endl;
              break;
            }
            case statefrm_e :
            {
              separate_action_specification(text, "statefrm", lpsspec, text);
              state_formulas::state_formula x = state_formulas::parse_state_formula(text, lpsspec, false);
              std::cout << state_formulas::pp(x) << std::endl;
              break;
            }
          }
        }
      }
      catch (std::exception& e)
      {
        std::cout << "Exception during parsing: " << e.what() << std::endl;
      }

      return true;
    }

};

int main(int argc, char** argv)
{
  return mcrl2parse_tool().execute(argc, argv);
}
