// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2yices.cpp

#define NAME "pbes2yices"
#define AUTHOR "Ruud Koolen"

#include "pbes2yices.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"

#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/normalize.h"

#include <fstream>


using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::pbes_system::tools;

enum goal_t { disjunctiveWitness, disjunctiveAcyclicUnrolling, conjunctiveWitness, conjunctiveAcyclicUnrolling };

inline std::istream& operator>>(std::istream& is, goal_t& goal)
{
  std::stringbuf buffer;
  is >> &buffer;

  if (buffer.str() == "disjunctiveWitness")
  {
    goal = disjunctiveWitness;
  }
  else if (buffer.str() == "disjunctiveAcyclicUnrolling")
  {
    goal = disjunctiveAcyclicUnrolling;
  }
  else if (buffer.str() == "conjunctiveWitness")
  {
    goal = conjunctiveWitness;
  }
  else if (buffer.str() == "conjunctiveAcyclicUnrolling")
  {
    goal = conjunctiveAcyclicUnrolling;
  }
  else
  {
    is.setstate(std::ios_base::failbit);
  }

  return is;
}

inline std::ostream& operator<<(std::ostream& os, const goal_t goal)
{
  if (goal == disjunctiveWitness)
  {
    os << "disjunctiveWitness";
  }
  else if (goal == disjunctiveAcyclicUnrolling)
  {
    os << "disjunctiveAcyclicUnrolling";
  }
  else if (goal == conjunctiveWitness)
  {
    os << "conjunctiveWitness";
  }
  else if (goal == conjunctiveAcyclicUnrolling)
  {
    os << "conjunctiveAcyclicUnrolling";
  }
  return os;
}


inline std::string description(const goal_t goal)
{
  if (goal == disjunctiveWitness)
  {
    return "a witness for a disjunctive PBES; if satisfiable, shows that the value of the PBES is true";
  }
  if (goal == disjunctiveAcyclicUnrolling)
  {
    return "an acyclic unrolling for a disjunctive PBES; if unsatisfiable, shows that the value of the PBES is false";
  }
  if (goal == conjunctiveWitness)
  {
    return "a witness for a conjunctive PBES; if satisfiable, shows that the value of the PBES is false";
  }
  if (goal == conjunctiveAcyclicUnrolling)
  {
    return "an acyclic unrolling for a disjunctive PBES; if unsatisfiable, shows that the value of the PBES is true";
  }
  return "";
}

class pbes2yices_tool: public pbes_input_tool<input_output_tool>
{
  protected:
    // Tool options.
    unsigned int opt_levels = 10; // Number of unrolling levels
    bool opt_hasgoal = false;
    goal_t opt_goal;

    using super = pbes_input_tool<input_output_tool>;

  public:
    pbes2yices_tool()
      : super(
        NAME,
        AUTHOR,
        "Generate a BES from a PBES and solve it. ",
        "Solves (P)BES from INFILE. "
        "If INFILE is not present, stdin is used. ")
    {}

  protected:
    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);

      if (parser.options.count("levels"))
      {
        opt_levels = parser.option_argument_as< unsigned int > ("levels");
      }

      if (parser.options.count("goal") == 0)
      {
        opt_hasgoal = false;
      }
      else
      {
        opt_hasgoal = true;
        opt_goal = parser.option_argument_as<goal_t>("goal");
      }
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("levels", make_mandatory_argument("NUM"), "unroll NUM levels", 'l');
      desc.add_option("goal", make_enum_argument<goal_t>("GOAL").
          add_value(disjunctiveWitness).
          add_value(disjunctiveAcyclicUnrolling).
          add_value(conjunctiveWitness).
          add_value(conjunctiveAcyclicUnrolling),
          "produce an SMT file for proving GOAL:", 'g');
    }

  public:
    bool run() override
    {
      // load the pbes
      pbes p;
      load_pbes(p, input_filename(), pbes_input_format());

      normalize(p);

      if (!opt_hasgoal)
      {
        parsed_pbes parsed;
        if (parse_pbes(p, true, parsed))
        {
          std::cout << "disjunctive\n";
          return true;
        }
        else if (parse_pbes(p, false, parsed))
        {
          std::cout << "conjunctive\n";
          return true;
        }
        else
        {
          return false;
        }
      }

      parsed_pbes parsed;
      if (opt_goal == disjunctiveWitness || opt_goal == disjunctiveAcyclicUnrolling)
      {
        if (!parse_pbes(p, true, parsed))
        {
          mCRL2log(log::error) << "This is not a disjunctive PBES, giving up.\n";
          return false;
        }
      }
      else
      {
        if (!parse_pbes(p, false, parsed))
        {
          mCRL2log(log::error) << "This is not a conjunctive PBES, giving up.\n";
          return false;
        }
      }

      translated_data_specification specification;
      translate_data_specification(p, specification);

      std::string output;
      if (opt_goal == disjunctiveWitness || opt_goal == conjunctiveWitness)
      {
        output = generate_witness_proposition(parsed, specification, opt_levels);
      }
      else
      {
        output = generate_acyclic_unrolling_proposition(parsed, specification, opt_levels);
      }

      if (output_filename().empty())
      {
        std::cout << output;
        if (std::cout.fail())
        {
          std::cerr << "Could not write SMT problem to standard output\n";
          return false;
        }
      }
      else
      {
        std::ofstream outputfile;
        outputfile.open(output_filename().c_str());
        outputfile << output;
        if (outputfile.fail())
        {
          std::cerr << "Could not write SMT problem to " << output_filename() << "\n";
          return false;
        }
        outputfile.close();
      }

      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbes2yices_tool().execute(argc, argv);
}
