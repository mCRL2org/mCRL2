// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes_bisimulation.cpp
/// Implements strong bisimulation and oblivious bisimulation reduction on
/// boolean equation systems.

#include <algorithm>
#include <boost/bind.hpp>

#include "bes_common.h"
#include "mcrl2/utilities/input_output_tool.h"

#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/bes/bes.h"
#include "mcrl2/bes/bes_parse.h"
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/detail/liblts_bisim.h"

namespace mcrl2 {
  namespace bes {

    boolean_equation_system<> pbes_to_bes(pbes_system::pbes<> const& p)
    {
      using namespace pbes_system;
      // Convert pbes to BES. Should be standard algorithm
      if (!p.is_bes())
      {
        throw mcrl2::runtime_error("PBES in input is not a BES");
      }

      atermpp::vector<boolean_equation> equations;
      for(atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        equations.push_back(boolean_equation(i->symbol(), boolean_variable(i->variable().name()), pbes_expression2boolean_expression(i->formula())));
      }

      return boolean_equation_system<>(equations, boolean_variable(accessors::name(p.initial_state())));
    }

    enum bes_equivalence
    {
      bes_eq_none, /**< Unknown or no equivalence */
      bes_eq_bisim, /**< Strong bisimulation equivalence */
      bes_eq_oblivious_bisim, /**< Oblivious bisimulation equivalence */
      bes_eq_stut /**< Stuttering equivalence */
    };

    static std::string equivalence_strings[] =
    {
      "unknown",
      "bisim",
      "oblivious",
      "stuttering"
    };

    inline bes_equivalence parse_equivalence(std::string const& s)
    {
      if (s == equivalence_strings[bes_eq_bisim])
      {
        return bes_eq_bisim;
      }
      else if (s == equivalence_strings[bes_eq_oblivious_bisim])
      {
        return bes_eq_oblivious_bisim;
      }
      else if (s == equivalence_strings[bes_eq_stut])
      {
        return bes_eq_stut;
      }
      else
      {
        return bes_eq_none;
      }
    }

    inline std::string string_for_equivalence(const bes_equivalence& eq)
    {
      return equivalence_strings[eq];
    }

    static const std::set<bes_equivalence> &initialise_allowed_eqs()
    {
      static std::set<bes_equivalence> s;
      s.insert(bes_eq_bisim);
      s.insert(bes_eq_stut);
      return s;
    }

    static const std::set<bes_equivalence> &allowed_eqs()
    {
      static const std::set<bes_equivalence> &s = initialise_allowed_eqs();
      return s;
    }

    template<typename Container>
    class bes_reduction_algorithm
    {
      protected:
        boolean_equation_system<Container>& m_bes;
        bes_equivalence m_equivalence;
        lts::lts m_lts;

        enum boolean_operand {BOOL_AND, BOOL_OR, BOOL_VAR};

      public:
        bes_reduction_algorithm(boolean_equation_system<>& v_bes, bes_equivalence eq=bes_eq_stut)
          : m_bes(v_bes),
            m_equivalence(eq)
        {}

        void run()
        {
          bes_to_lts();
          reduce_lts();
          lts_to_bes();
        }

      protected:
        boolean_operand get_operand(boolean_expression const& e)
        {
          typedef core::term_traits<boolean_expression> tr;

          if(tr::is_and(e))
          {
            return BOOL_AND;
          }
          else if(tr::is_or(e))
          {
            return BOOL_OR;
          }
          else if(tr::is_variable(e))
          {
            return BOOL_VAR;
          }
          else
          {
            throw mcrl2::runtime_error("Unexpected operand occurred as top-level symbol in " + tr::pp(e) + ", expected only &&, || or a variable");
          }
        }

        boolean_operand string_to_operand(std::string const& s)
        {
          int i = atoi(s.c_str());
          if(i == 0)
          {
            return BOOL_AND;
          }
          else if(i == 1)
          {
            return BOOL_OR;
          }
          else if(i == 2)
          {
            return BOOL_VAR;
          }
          else
          {
            throw mcrl2::runtime_error("Unknown operand string " + s);
          }
        }

        std::string print_operand(const boolean_operand o)
        {
          switch(o)
          {
            case BOOL_AND:
              return "&&";
            case BOOL_OR:
              return "||";
            case BOOL_VAR:
              return "var";
            default:
              throw mcrl2::runtime_error("Unexpected case when printing operand.");
          }
        }

        void bes_to_lts()
        {
          //assert(m_bes.is_closed());
          assert(is_standard_recursive_form(m_bes));
          assert(m_bes.initial_state() == m_bes.equations().begin()->variable());

          // Collect block indices and operands of all equations
          std::map<boolean_variable, std::pair<unsigned int, boolean_operand> > statistics;
          std::map<boolean_variable, unsigned int> indices;

          unsigned int occurring_variable_count = 0; // count total number of occurring variables in right hand sides.

          unsigned int current_block = 0;
          unsigned int index = 0;
          fixpoint_symbol sigma = fixpoint_symbol::nu();
          for(typename Container::const_iterator i = m_bes.equations().begin(); i != m_bes.equations().end(); ++i)
          {
            if(i->symbol() != sigma)
            {
              sigma = i->symbol();
              ++current_block;
            }

            std::set<boolean_variable> occurring_variables;
            atermpp::find_all_if(i->formula(), &is_variable, std::inserter(occurring_variables, occurring_variables.end()));
            occurring_variable_count += occurring_variables.size();

            statistics[i->variable()] = std::make_pair(current_block, get_operand(i->formula()));
            indices[i->variable()] = index++;
          }

          unsigned int transitioncount = occurring_variable_count + m_bes.equations().size();
          unsigned int statecount = m_bes.equations().size() + 1;
          unsigned int deadlock = m_bes.equations().size(); // deadlock state
          unsigned int initial_state = 0;
          std::stringstream aut;
          aut << "des(0," << transitioncount << "," << statecount << ")" << std::endl;

          m_lts.set_num_states(statecount, false);
          m_lts.set_initial_state(initial_state);

          atermpp::indexed_set labs(100,50);

          for(typename Container::const_iterator i = m_bes.equations().begin(); i != m_bes.equations().end(); ++i)
          {
            std::pair<unsigned int, boolean_operand> info = statistics[i->variable()];
            unsigned int from = indices[i->variable()];

            // Edge to deadlock state
            {
              std::stringstream label;
              label << "block(" << info.first << "),op(" << info.second << ")";
              unsigned int to = deadlock;
              atermpp::aterm t = atermpp::aterm_appl(atermpp::function_symbol(label.str(),0,true));
              int label_index = labs.index(t);
              if ( label_index < 0 )
              {
                std::pair<int, bool> put_result = labs.put(t);
                label_index = put_result.first;
                m_lts.add_label(t,label.str()=="tau");
              }
              m_lts.add_transition(lts::transition(from,label_index,to));
            }

            // Edges to successors
            std::set<boolean_variable> occurring_variables;
            atermpp::find_all_if(i->formula(), &is_variable, std::inserter(occurring_variables, occurring_variables.end()));
            for(std::set<boolean_variable>::const_iterator j = occurring_variables.begin(); j != occurring_variables.end(); ++j)
            {
              std::stringstream label;
              std::pair<unsigned int, boolean_operand> info_target = statistics[*j];
              if(info == info_target)
              {
                label << "tau";
              }
              else
              {
                label << "block(" << info_target.first << "),op(" << info_target.second << ")";
              }
              unsigned int to = indices[*j];
              atermpp::aterm t = atermpp::aterm_appl(atermpp::function_symbol(label.str(),0,true));
              int label_index = labs.index(t);
              if ( label_index < 0 )
              {
                std::pair<int, bool> put_result = labs.put(t);
                label_index = put_result.first;
                m_lts.add_label(t,label.str()=="tau");
              }
              m_lts.add_transition(lts::transition(from,label_index,to));
            }
          }

          m_lts.set_type(lts::lts_aut);
        }

        void reduce_lts()
        {
          switch(m_equivalence)
          {
            case bes_eq_bisim:
              lts::detail::bisimulation_reduce(m_lts, false, false);
              break;
            case bes_eq_stut:
              lts::detail::bisimulation_reduce(m_lts, true, true);
              break;
            default:
              throw mcrl2::runtime_error("Reduction using " + string_for_equivalence(m_equivalence) + " not supported.");
          }
        }

        void lts_to_bes()
        {
          // Find deadlock state
          unsigned int state_count = m_lts.num_states();
          std::map<unsigned int, bool> has_outgoing_transition;
          for(unsigned int i = 0; i < state_count; ++i)
          {
            has_outgoing_transition[i] = false;
          }

          m_lts.sort_transitions(lts::src_lbl_tgt);
          lts::transition_const_range transitions(m_lts.get_transitions());
          for(lts::transition_const_range::const_iterator i = transitions.begin(); i != transitions.end(); ++i)
          {
            has_outgoing_transition[i->from()] = true;
          }

          unsigned int deadlock_state = 0;
          bool deadlock_found = false;
          do
          {  
            assert(has_outgoing_transition.find(deadlock_state) != has_outgoing_transition.end());
            deadlock_found = !has_outgoing_transition[deadlock_state];
            if(!deadlock_found)
            {
              ++deadlock_state;
            }
          }
          while (!deadlock_found && deadlock_state < state_count);
          assert(deadlock_found);

          // Build formulas
          unsigned int cur_state = 0;
          lts::transition_const_range::const_iterator i = transitions.begin();

          atermpp::map<unsigned int, atermpp::vector<boolean_equation> > blocks;

          while(i != transitions.end())
          {
            atermpp::vector<boolean_variable> variables;
            unsigned int block = 0;
            boolean_operand op = BOOL_VAR;
            cur_state = i->from();

            while(i->from() == cur_state && i != transitions.end())
            {
              if(i->to() == deadlock_state)
              {
                std::string label = m_lts.pretty_print_label_value(m_lts.label_value(i->label()));
                size_t comma_pos = label.find(",");

                std::string block_str = label.substr(0,comma_pos);
                block_str.replace(0,block_str.find("(")+1,"");
                block_str.replace(block_str.find(")"),1,"");

                std::string op_str = label.substr(comma_pos);
                op_str.replace(0,op_str.find("(")+1,"");
                op_str.replace(op_str.find(")"),1,"");

                block = atoi(block_str.c_str());
                op = string_to_operand(op_str);
              }
              else
              {
                std::stringstream name;
                name << "X" << i->to();
                variables.push_back(boolean_variable(name.str()));
              }
              ++i;
            }

            fixpoint_symbol fp;
            if(block % 2 == 0)
            {
              fp = fixpoint_symbol::nu();
            }
            else
            {
              fp = fixpoint_symbol::mu();
            }

            boolean_expression expr;
            switch(op)
            {
              case BOOL_AND:
                expr = join_and(variables.begin(), variables.end());
                break;
              case BOOL_OR:
                expr = join_or(variables.begin(), variables.end());
                break;
              case BOOL_VAR:
                assert(variables.size() == 1);
                expr = *(variables.begin());
                break;
              default:
                throw mcrl2::runtime_error("Unexpected operand");
            }
            std::stringstream name;
            name << "X" << cur_state;
            boolean_equation eq(fp, boolean_variable(name.str()), expr);
            blocks[block].push_back(eq);
          }

          atermpp::vector<boolean_equation> eqns;
          for(unsigned int i = 0; i <= blocks.size(); ++i)
          {
            atermpp::map<unsigned int, atermpp::vector<boolean_equation> >::const_iterator j = blocks.find(i);
            if(j != blocks.end())
            {
              eqns.insert(eqns.end(), j->second.begin(), j->second.end());
            }
          }
          
          m_bes.equations().swap(eqns);
          std::stringstream name;
          name << "X" << m_lts.initial_state();
          m_bes.initial_state() = boolean_variable(name.str());
        }

    };
  } // namespace bes
} // namespace mcrl2


/// \brief Simple input/output tool to perform strong as well as oblivious bisimulation
///        reduction on a boolean equation system.
class bes_bisimulation_tool: public mcrl2::utilities::tools::input_output_tool
{
  protected:

    typedef mcrl2::utilities::tools::input_output_tool super;

    mcrl2::bes::bes_equivalence equivalence;

    void add_options(mcrl2::utilities::interface_description& desc)
    {
      super::add_options(desc);
/*
      desc.add_option("equivalence", make_mandatory_argument("NAME"),
        "generate an equivalent BES, preserving equivalence NAME:\n"
        "supported equivalences: bisim, stuttering (default bisim)", 'e');
        */
    }

    void parse_options(const mcrl2::utilities::command_line_parser& parser)
    {
      super::parse_options(parser);

      /*
      if (parser.options.count("equivalence")) {
        equivalence = mcrl2::bes::parse_equivalence(parser.option_argument("equivalence"));
        if( allowed_eqs().count(equivalence) == 0 )
        {
          parser.error("option -e/--equivalence has illegal argument '" +
            parser.option_argument("equivalence") + "'");
        }
      }
      */
    }      

  public:

    bes_bisimulation_tool()
      : super(
          "bes_bisimulation",
          "Jeroen Keiren",
          "reduce a BES (or PBES) using (variations of) behavioural equivalences",
          "reduce the (P)BES in INFILE modulo write the result to OUTFILE (as PBES)."
          "If INFILE is not "
          "present, stdin is used. If OUTFILE is not present, stdout is used.")
    {}

    bool run()
    {
      using namespace mcrl2::bes;
      using namespace mcrl2;

      boolean_equation_system<> b;

      try
      {
        b.load(m_input_filename);
      }
      catch(mcrl2::runtime_error&)
      {
        using namespace pbes_system;
        pbes_system::pbes<> pbes;
        pbes.load(m_input_filename);

        b = pbes_to_bes(pbes);
      }

      bes_reduction_algorithm<atermpp::vector<boolean_equation> >(b).run();
      
      pbes_system::pbes<> pbes(bes2pbes(b));
      pbes.save(m_output_filename);

      return true;
    }
};


int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return bes_bisimulation_tool().execute(argc, argv);
}
