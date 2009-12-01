// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes2txt.cpp
/// Convert a BES to a textual format

#include <algorithm>
#include <iostream>
#include <fstream>
#include <boost/bind.hpp>

#include "bes_common.h"

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/pbes/bes.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/utilities/input_output_tool.h"

namespace mcrl2 {

  namespace bes {

    inline
    unsigned int size(boolean_expression e)
    {
      typedef core::term_traits<boolean_expression> tr;

      if (tr::is_variable(e))
      {
        return 1;
      }
      else if (tr::is_true(e))
      {
        return 1;
      }
      else if (tr::is_false(e))
      {
        return 1;
      }
      else if (tr::is_and(e))
      {
        return size(tr::left(e)) + size(tr::right(e));
      }
      else if (tr::is_or(e))
      {
        return size(tr::left(e)) + size(tr::right(e));
      }
      else if (tr::is_imp(e))
      {
        return size(tr::left(e)) + size(tr::right(e));
      }
      throw mcrl2::runtime_error("error in mcrl2::bes::size: encountered unknown boolean expression " + e.to_string());
      return 0;
    }

    inline
    unsigned int size(const boolean_equation& eq)
    {
      return size(eq.formula());
    }

    template <typename Container>
    int size(const boolean_equation_system<Container>& p)
    {
      unsigned int result = 0;
      BOOST_FOREACH(const boolean_equation& eq, p.equations())
      {
        result += size(eq);
      }
      return result;
    }

    inline
    std::string pp_cwi(const boolean_expression& e, bool add_parens = false)
    {
      typedef core::term_traits<boolean_expression> tr;
  
      if (tr::is_variable(e))
      {
        return pp(tr::term2variable(e));
      }
      else if (tr::is_true(e))
      {
        return "T";
      }
      else if (tr::is_false(e))
      {
        return "F";
      }
      else if (tr::is_not(e))
      {
        return std::string("!") + (add_parens ? "(" : "") + pp_cwi(tr::arg(e), true) + (add_parens ? ")" : "");
      }
      else if (tr::is_and(e))
      {
        return (add_parens ? "(" : "") + pp_cwi(tr::left(e), true) + "&" + pp_cwi(tr::right(e), true) + (add_parens ? ")" : "");
      }
      else if (tr::is_or(e))
      {
        return (add_parens ? "(" : "") + pp_cwi(tr::left(e), true) + "|" + pp_cwi(tr::right(e), true) + (add_parens ? ")" : "");
      }
      else if (tr::is_imp(e))
      {
        // Make sure implication is not printed
        return pp_cwi(tr::or_(tr::not_(tr::left(e)), tr::right(e)), add_parens);
      }
      throw mcrl2::runtime_error("error in mcrl2::bes::pp: encountered unknown boolean expression " + e.to_string());
      return "";
    }

    /// \brief Print fixpoint symbol in CWI format
    /// \param symbol a fixpoint symbol
    /// \ret the CWI format for symbol
    inline
    std::string pp_cwi(const fixpoint_symbol& symbol)
    {
      if(symbol.is_nu())
      {
        return "max";
      }
      else
      {
        return "min";
      }
    }

    /// \brief Pretty print function for cwi format
    /// \param eq A boolean equation
    /// \return A pretty printed representation of the boolean equation, in cwi format
    std::string pp_cwi(const boolean_equation& eq)
    {
      return pp_cwi(eq.symbol()) + " " + pp(eq.variable()) + " = " + pp_cwi(eq.formula());
    }
   
    /// \brief Print as CWI file
    /// \param p A boolean equation system
    /// \return The CWI file representation of p
    template <typename Container>
    std::string pp_cwi(const boolean_equation_system<Container>& p)
    {
      std::ostringstream out;
      BOOST_FOREACH(const boolean_equation& eq, p.equations())
      {
        out << pp_cwi(eq) << std::endl;
      }
      return out.str();
    }

    // prototype
    inline
    std::string pp_pgsolver(const boolean_expression& e);

    inline
    std::string pp_pgsolver_set(const atermpp::set<boolean_expression>& v)
    {
      std::ostringstream out;

      for(atermpp::set<boolean_expression>::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        out << pp_pgsolver(*i);

        atermpp::set<boolean_expression>::const_iterator j = i;
        ++j;
        if(j != v.end())
        {
          out << ", ";
        }
      }

      return out.str();
    }
   
    inline
    std::string pp_pgsolver(const boolean_expression& e)
    {
      typedef core::term_traits<boolean_expression> tr;
  
      if (tr::is_variable(e))
      {
        return pp(tr::term2variable(e)).erase(0,1); // Not so nice solution to remove X from string
      }
      else if (tr::is_and(e))
      {
        atermpp::set<boolean_expression> clauses = split_and(e);
        return pp_pgsolver_set(clauses);
      }
      else if (tr::is_or(e))
      {
        atermpp::set<boolean_expression> clauses = split_or(e);
        return pp_pgsolver_set(clauses);
      }
      throw mcrl2::runtime_error("error in mcrl2::bes::pp: encountered unknown boolean expression " + e.to_string());
      return "";
    }


    std::string pp_pgsolver(const boolean_equation& eq, unsigned int rank)
    {
      typedef core::term_traits<boolean_expression> tr;
      std::ostringstream out;
      out << pp_pgsolver(eq.variable()) << " " << rank << " " << (tr::is_and(eq.formula())?"1":"0") << " " << pp_pgsolver(eq.formula()) << ";";
      return out.str();
    }

    template <typename Container>
    std::string pp_pgsolver(const boolean_equation_system<Container>& p)
    {
      std::ostringstream out;

      // Compute maximal rank
      unsigned int max_rank = 0;
      fixpoint_symbol symbol = fixpoint_symbol::nu();
      for(typename Container::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        if(i->symbol() != symbol)
        {
          ++max_rank;
          symbol = i->symbol();
        }
      }

      // Max rank needs to be even, and >= 2
      if(max_rank % 2 == 1)
      {
        ++max_rank;
      }

      if(max_rank == 0)
      {
        max_rank = 2;
      }

      // Start outputting
      symbol = fixpoint_symbol::nu();
      unsigned int rank = 0;
      BOOST_FOREACH(const boolean_equation& eq, p.equations())
      {
        if(eq.symbol() != symbol)
        {
          ++rank;
          symbol = eq.symbol();
        }
        out << pp_pgsolver(eq, rank) << std::endl;
      }
      return out.str();
    }
  }
}
 
using namespace mcrl2::utilities;

/// \brief Simple input/output tool to perform strong as well as oblivious bisimulation
///        reduction on a boolean equation system.
class bes2txt_tool: public mcrl2::utilities::tools::input_output_tool
{

  protected:

    typedef tools::input_output_tool super;

    std::string m_output_format;

    void add_options(mcrl2::utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("output",
        make_mandatory_argument("FORMAT"),
        "use output format FORMAT:\n"
        " 'txt' (default), \n"
        " 'cwi', \n"
        " 'cwi-des', \n"
        " 'pgsolver'",
        'o');
    }

    void parse_options(const mcrl2::utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      if (parser.options.count("output"))
      {
        std::string format = parser.option_argument("output");
        if(!(format == "txt" || format == "cwi" || format == "cwi-des" || format == "pgsolver"))
        {
          parser.error("unknown output format specified (got `" + format + "')");
        }

        m_output_format = format;
      }
    }      

  public:

    bes2txt_tool()
      : super(
          "bes2txt",
          "Jeroen Keiren",
          "Convert the BES in INFILE to a textual format",
          "Convert the BES in INFILE to text and write the result to OUTFILE. If INFILE is not "
          "present, stdin is used. If OUTFILE is not present, stdout is used."),
        m_output_format("txt")
    {}

    bool run()
    {
      using namespace mcrl2::bes;
      using namespace mcrl2;

      boolean_equation_system<> bes;
      bes.load(m_input_filename);

      std::fstream output;
      output.open(m_output_filename.c_str(), std::ios::out);
      if(!output.is_open())
      {
        throw mcrl2::runtime_error("could not open output file " + m_output_filename);
      }

      if(m_output_format == "txt")
      {
        output << pp(bes);
      }
      else if(m_output_format == "cwi")
      {
        output << pp_cwi(bes);
      }
      else if(m_output_format == "cwi-des")
      {
        output << "des(0," << size(bes) << "," << bes.equations().size() << ")" << std::endl;
        output << pp_cwi(bes);
      }
      else if(m_output_format == "pgsolver")
      {
        if(!is_standard_form(bes) || has_true(bes) || has_false(bes))
        {
          throw mcrl2::runtime_error("Storing BES in pgsolver format requires the BES to be in standard recursive form");
        }
        output << pp_pgsolver(bes);
      }
      else
      {
        throw mcrl2::runtime_error("Unexpected output format");
      }

      output.close();

      return true;
    }
};


int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return bes2txt_tool().execute(argc, argv);
}

