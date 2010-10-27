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

#include "mcrl2/utilities/input_output_tool.h"

#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/bes_parse.h"
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/bes/normal_forms.h"
#include "mcrl2/bes/find.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/detail/liblts_bisim.h"

using namespace mcrl2::bes;
using namespace mcrl2::utilities::tools;

namespace mcrl2 {
  namespace bes {

    template<typename Container = atermpp::vector<boolean_equation> >
    class bes_reduction_algorithm
    {
      public:
        enum equivalence_t
        {
          eq_none, /**< Unknown or no equivalence */
          eq_bisim, /**< Strong bisimulation equivalence */
          eq_oblivious_bisim, /**< Oblivious bisimulation equivalence */
          eq_stut /**< Stuttering equivalence */
        };

      protected:
        boolean_equation_system<Container>& m_bes;
        equivalence_t m_equivalence;
        lts::lts_lts_t m_lts;

        std::set<equivalence_t> m_allowed_equivalences;
        std::map<equivalence_t, std::string> m_equivalence_strings;

        enum boolean_operand_t { BOOL_AND, BOOL_OR, BOOL_VAR };

      public:
        equivalence_t parse_equivalence(std::string const& s)
        {
          if (s == m_equivalence_strings[eq_bisim])
          {
            return eq_bisim;
          }
          else if (s == m_equivalence_strings[eq_oblivious_bisim])
          {
            return eq_oblivious_bisim;
          }
          else if (s == m_equivalence_strings[eq_stut])
          {
            return eq_stut;
          }
          else
          {
            return eq_none;
          }
        }

        std::string string_for_equivalence(const equivalence_t& eq)
        {
          return m_equivalence_strings[eq];
        }

        const std::set<equivalence_t> &allowed_eqs() const
        {
          return m_allowed_equivalences;
        }

        std::string allowed_equivalences()
        {
          std::set<std::string> tmp;
          for(typename std::set<equivalence_t>::const_iterator i = allowed_eqs().begin(); i != allowed_eqs().end(); ++i)
          {
            tmp.insert(string_for_equivalence(*i));
          }
          return core::string_join(tmp, ", ");
        }

      protected:
        void initialise_allowed_eqs()
        {
          m_allowed_equivalences.insert(eq_bisim);
          m_allowed_equivalences.insert(eq_stut);
          m_equivalence_strings[eq_bisim] = "bisim";
          m_equivalence_strings[eq_stut] = "stuttering";
          m_equivalence_strings[eq_oblivious_bisim] = "oblivious";
          m_equivalence_strings[eq_none] = "none";
        }

        boolean_operand_t get_operand(boolean_expression const& e)
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

        boolean_operand_t string_to_operand(std::string const& s)
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

        std::string print_operand(const boolean_operand_t o)
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

        /// \brief Translate a BES to an LTS.
        /// \pre m_bes is in SRF.
        ///
        /// Tranlation of m_bes to an LTS in m_lts, according to the following rules.
        /// m_lts consists of one vertex for each equation, as well as an additional
        /// vertex d, which has no outgoing transitions.
        /// The transition relation is as follows.
        /// X -tau-> Y if Y in rhs(X) and block(X) = block(Y) and op(X) = op(Y)
        /// X -"block(Y),op(Y)"-> Y if Y in rhs(X) and block(X) != block(Y) or op(X) != op(Y)
        /// X -"block(X),op(X)"-> d.
        /// Initial state of the lts is the state representing the initial equation of the BES.
        void bes_to_lts()
        {
          if(core::gsDebug)
          {
            std::cerr << "Tranforming BES to LTS" << std::endl;
          }

          if(m_bes.initial_state() != m_bes.equations().begin()->variable())
          {
            throw mcrl2::runtime_error("The first equation is not the variable designated as initial. This situation is not handled by the tool.");
          }


          // Collect block indices and operands of all equations
          std::map<boolean_variable, std::pair<unsigned int, boolean_operand_t> > statistics;
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

            std::set<boolean_variable> occurring_variables = bes::find_variables(i->formula());
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
            std::pair<unsigned int, boolean_operand_t> info = statistics[i->variable()];
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
                m_lts.add_label(mcrl2::lts::detail::action_label_mcrl2((ATerm)t),label.str()=="tau");
              }
              m_lts.add_transition(lts::transition(from,label_index,to));
            }

            // Edges to successors
            std::set<boolean_variable> occurring_variables = bes::find_variables(i->formula());
            for(std::set<boolean_variable>::const_iterator j = occurring_variables.begin(); j != occurring_variables.end(); ++j)
            {
              std::stringstream label;
              std::pair<unsigned int, boolean_operand_t> info_target = statistics[*j];
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
                m_lts.add_label(mcrl2::lts::detail::action_label_mcrl2(t),label.str()=="tau");
              }
              m_lts.add_transition(lts::transition(from,label_index,to));
            }
          }

          // m_lts.set_type(lts::lts_aut);
        }

        void reduce_lts()
        {
          if(core::gsDebug)
          {
            std::cerr << "Reduce LTS" << std::endl;
          }
          switch(m_equivalence)
          {
            case eq_bisim:
              lts::detail::bisimulation_reduce(m_lts, false, false);
              break;
            case eq_stut:
              lts::detail::bisimulation_reduce(m_lts, true, true);
              break;
            default:
              throw mcrl2::runtime_error("Reduction using " + string_for_equivalence(m_equivalence) + " not supported.");
          }
        }

        /// \brief Transform LTS back to BES.
        /// Reverse of tranlation LTS to BES. Unicity of labels was guaranteed by
        /// the edges to the deadlock state.
        void lts_to_bes()
        {
          if(core::gsDebug)
          {
            std::cerr << "Transforming reduced LTS to BES." << std::endl;
          }
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
          assert(deadlock_found); // Deadlock state must be present, otherwise
                                  // something has severely gone wrong in reduction.

          // Build formulas
          unsigned int cur_state = 0;
          lts::transition_const_range::const_iterator i = transitions.begin();

          atermpp::map<unsigned int, atermpp::vector<boolean_equation> > blocks;

          while(i != transitions.end())
          {
            atermpp::vector<boolean_variable> variables;
            unsigned int block = 0;
            boolean_operand_t op = BOOL_VAR;
            cur_state = i->from();

            while(i->from() == cur_state && i != transitions.end())
            {
              if(i->to() == deadlock_state)
              {
                std::string label = pp(m_lts.label_value(i->label()));
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

      public:
        bes_reduction_algorithm(boolean_equation_system<>& v_bes, const equivalence_t equivalence=eq_stut)
          : m_bes(v_bes),
            m_equivalence(equivalence)
        {
          initialise_allowed_eqs();
          if(core::gsDebug)
          {
            std::cerr << "Converting BES to standard form" << std::endl;
          }
          make_standard_form(m_bes, true);
          if(core::gsDebug)
          {
            std::cerr << "BES Reduction algorithm initialised" << std::endl;
          }
        }

        void run()
        {
          bes_to_lts();
          reduce_lts();
          lts_to_bes();
        }

    };
  } // namespace bes
} // namespace mcrl2


/// \brief Simple input/output tool to perform strong as well as oblivious bisimulation
///        reduction on a boolean equation system.
typedef mcrl2::utilities::tools::input_output_tool super;
class bes_bisimulation_tool: public super
{
  protected:
    bes_reduction_algorithm<>::equivalence_t equivalence;

    void add_options(mcrl2::utilities::interface_description& desc)
    {
      using namespace mcrl2::utilities::tools;
      using namespace mcrl2::utilities;
      super::add_options(desc);

      desc.add_option("equivalence", make_mandatory_argument("NAME"),
        "generate an equivalent BES, preserving equivalence NAME:\n"
        "supported equivalences: bisim, stuttering (default stuttering)", 'e');
    }

    void parse_options(const mcrl2::utilities::command_line_parser& parser)
    {
      super::parse_options(parser);

      if (parser.options.count("equivalence")) {
        boolean_equation_system<> b; // TODO: build proper solution.
        mcrl2::bes::bes_reduction_algorithm<> a(b);
        std::cerr << "Allowed eqs: " << a.allowed_equivalences() << std::endl;
        equivalence = a.parse_equivalence(parser.option_argument("equivalence"));
        if( a.allowed_eqs().count(equivalence) == 0 )
        {
          parser.error("option -e/--equivalence has illegal argument '" +
            parser.option_argument("equivalence") + "'");
        }
      }
    }      

  public:

    bes_bisimulation_tool()
      : super(
          "bes_bisimulation",
          "Jeroen Keiren",
          "reduce a BES (or PBES) using (variations of) behavioural equivalences",
          "reduce the (P)BES in INFILE modulo write the result to OUTFILE (as PBES)."
          "If INFILE is not "
          "present, stdin is used. If OUTFILE is not present, stdout is used."),
        equivalence(bes_reduction_algorithm<>::eq_stut)
    {}

    bool run()
    {
      using namespace mcrl2::bes;
      using namespace mcrl2;

      boolean_equation_system<> b;

      core::gsVerboseMsg("Loading BES from input file... ");
      b.load(m_input_filename);
      core::gsVerboseMsg("done\n");
      bes_reduction_algorithm<atermpp::vector<boolean_equation> >(b).run();
      b.save(m_output_filename);
      
      return true;
    }
};


int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return bes_bisimulation_tool().execute(argc, argv);
}
