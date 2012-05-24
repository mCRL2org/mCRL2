// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file besconvert.cpp
/// Implements strong bisimulation and oblivious bisimulation reduction on
/// boolean equation systems.

#include <algorithm>
#include <boost/bind.hpp>

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/pbes_input_output_tool.h"
#include "mcrl2/utilities/execution_timer.h"

#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/bes/detail/bes_algorithm.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/parse.h"
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/bes/normal_forms.h"
#include "mcrl2/bes/find.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/detail/liblts_bisim.h"

using namespace mcrl2::bes;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::log;

namespace mcrl2
{
namespace bes
{

template <typename Container = std::vector<boolean_equation> >
class bes_reduction_algorithm: public detail::bes_algorithm<Container>
{
  public:
    enum equivalence_t
    {
      eq_none, /**< Unknown or no equivalence */
      eq_bisim, /**< Strong bisimulation equivalence */
      eq_oblivious_bisim, /**< Oblivious bisimulation equivalence */
      eq_stut /**< Stuttering equivalence */
    };

    enum to_lts_translation_t
    {
      to_lts_deadlock,
      to_lts_selfloop,
      to_lts_outgoing_transition // for bisimulation only
    };

  protected:
    typedef detail::bes_algorithm<Container> super;
    using super::m_bes; // Why doesn't the compiler see this by itself?

    equivalence_t m_equivalence;
    lts::lts_aut_t m_lts;
    to_lts_translation_t m_translation;
    std::string m_lts_filename;
    bool m_translate_to_lts_only;

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
      for (typename std::set<equivalence_t>::const_iterator i = allowed_eqs().begin(); i != allowed_eqs().end(); ++i)
      {
        tmp.insert(string_for_equivalence(*i));
      }
      return utilities::string_join(tmp, ", ");
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

      if (tr::is_and(e))
      {
        return BOOL_AND;
      }
      else if (tr::is_or(e))
      {
        return BOOL_OR;
      }
      else if (tr::is_variable(e))
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
      if (i == 0)
      {
        return BOOL_AND;
      }
      else if (i == 1)
      {
        return BOOL_OR;
      }
      else if (i == 2)
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
      switch (o)
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
      mCRL2log(debug) << "Tranforming BES to LTS" << std::endl;

      // Collect block indices and operands of all equations
      std::map<boolean_variable, std::pair<size_t, boolean_operand_t> > statistics;
      std::map<boolean_variable, size_t> indices;
      std::map<unsigned int, boolean_operand_t> block_to_operand; // Stores operand assigned to equations without boolean operand.

      size_t occurring_variable_count = 0; // count total number of occurring variables in right hand sides.

      size_t current_block = 0;
      size_t index = 0;
      fixpoint_symbol sigma = fixpoint_symbol::nu();
      bool and_in_block = false;
      for (typename Container::const_iterator i = m_bes.equations().begin(); i != m_bes.equations().end(); ++i)
      {
        if (i->symbol() != sigma)
        {
          block_to_operand[current_block] = and_in_block?BOOL_AND:BOOL_OR;
          and_in_block = false;
          sigma = i->symbol();
          ++current_block;
        }

        if (get_operand(i->formula()) == BOOL_AND)
        {
          and_in_block = true;
        }

        std::set<boolean_variable> occurring_variables = bes::find_boolean_variables(i->formula());
        occurring_variable_count += occurring_variables.size();

        statistics[i->variable()] = std::make_pair(current_block, get_operand(i->formula()));
        indices[i->variable()] = index++;
      }

      // Collect block indices and operands of all equations
      unsigned int transitioncount = occurring_variable_count;
      unsigned int statecount = m_bes.equations().size();
      unsigned int deadlock = m_bes.equations().size(); // only used in deadlock translation
      if (m_translation == to_lts_deadlock)
      {
        ++statecount;
        transitioncount += m_bes.equations().size();
      }
      unsigned int initial_state = indices[static_cast<boolean_variable>(m_bes.initial_state())];

      m_lts.set_num_states(statecount, false);
      m_lts.set_initial_state(initial_state);

      atermpp::indexed_set labs(100,50);

      for (typename Container::const_iterator i = m_bes.equations().begin(); i != m_bes.equations().end(); ++i)
      {
        std::pair<unsigned int, boolean_operand_t> info = statistics[i->variable()];
        // If variable, map to operand that was precomputed for variables.
        if (info.second == BOOL_VAR)
        {
          info.second = block_to_operand[info.first];
        }

        unsigned int from = indices[i->variable()];

        // Create selfloop self:block(...),op(...)
        // recording block and operand.
        {
          std::stringstream label;
          label << "self:block(" << info.first << "),op(" << info.second << ")";
          lps::action t(lps::action_label(core::identifier_string(label.str()), data::sort_expression_list()), data::data_expression_list());
          size_t label_index = labs.index(t);
          if (label_index == atermpp::ATERM_NON_EXISTING_POSITION)
          {
            std::pair<int, bool> put_result = labs.put(t);
            label_index = put_result.first;
            m_lts.add_action(mcrl2::lts::detail::action_label_string(t.label().name()),false);
          }

          switch (m_translation)
          {
            case to_lts_deadlock:
            {
              m_lts.add_transition(lts::transition(from,label_index,deadlock));
              break;
            }
            case to_lts_selfloop:
            {
              m_lts.add_transition(lts::transition(from,label_index,from));
              break;
            }
            case to_lts_outgoing_transition:
            {
              break; // No special transition needed.
            }
            default:
            {
              throw mcrl2::runtime_error("Unsupported translation");
            }
          }
        }

        // Edges to successors
        std::set<boolean_variable> occurring_variables = bes::find_boolean_variables(i->formula());
        for (std::set<boolean_variable>::const_iterator j = occurring_variables.begin(); j != occurring_variables.end(); ++j)
        {
          std::stringstream label;
          std::pair<unsigned int, boolean_operand_t> info_target = statistics[*j];

          // If variable, map to operand that was precomputed for variables.
          if (info_target.second == BOOL_VAR)
          {
            info_target.second = block_to_operand[info_target.first];
          }

          if (m_translation == to_lts_outgoing_transition)
          {
            label << "block(" << info.first << "),op(" << info.second << ")";
          }
          else if (info == info_target && (m_translation != to_lts_outgoing_transition))
          {
            label << "tau";
          }
          else
          {
            label << "block(" << info_target.first << "),op(" << info_target.second << ")";
          }
          size_t to = indices[*j];
          lps::action t(lps::action_label(core::identifier_string(label.str()), data::sort_expression_list()), data::data_expression_list());
          size_t label_index = labs.index(t);
          if (label_index == atermpp::ATERM_NON_EXISTING_POSITION)
          {
            std::pair<int, bool> put_result = labs.put(t);
            label_index = put_result.first;
            m_lts.add_action(mcrl2::lts::detail::action_label_string(t.label().name()),label.str()=="tau");
          }
          m_lts.add_transition(lts::transition(from,label_index,to));
        }
      }

      if (!m_lts_filename.empty())
      {
        m_lts.save(m_lts_filename);
      }

    }

    void reduce_lts()
    {
      mCRL2log(debug) << "Reduce LTS" << std::endl;

      switch (m_equivalence)
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
    /// the self-loops.
    void lts_to_bes()
    {
      mCRL2log(debug) << "Transforming reduced LTS to BES." << std::endl;

      // Find deadlock state
      // Only used if m_translation == to_lts_deadlock
      unsigned int state_count = m_lts.num_states();
      std::map<unsigned int, bool> has_outgoing_transition;
      for (unsigned int i = 0; i < state_count; ++i)
      {
        has_outgoing_transition[i] = false;
      }

      m_lts.sort_transitions(lts::src_lbl_tgt);
      const std::vector<lts::transition> &transitions=m_lts.get_transitions();

      for (std::vector<lts::transition>::const_iterator i = transitions.begin(); i != transitions.end(); ++i)
      {
        has_outgoing_transition[i->from()] = true;
      }

      unsigned int deadlock_state = 0;
      bool deadlock_found = false;
      do
      {
        assert(has_outgoing_transition.find(deadlock_state) != has_outgoing_transition.end());
        deadlock_found = !has_outgoing_transition[deadlock_state];
        if (!deadlock_found)
        {
          ++deadlock_state;
        }
      }
      while (!deadlock_found && deadlock_state < state_count);

      if (m_translation == to_lts_deadlock && !deadlock_found)
      {
        throw mcrl2::runtime_error("Used deadlock translation, but no deadlock found in reduced BES. Cannot proceed.");
      }

      // Build formulas
      size_t cur_state = 0;
      std::vector<lts::transition>::const_iterator i = transitions.begin();
      std::map<size_t, std::vector<boolean_equation> > blocks;

      while (i != transitions.end())
      {
        std::vector<boolean_variable> variables;
        size_t block = 0;
        boolean_operand_t op = BOOL_VAR;
        cur_state = i->from();

        while (i != transitions.end() && i->from() == cur_state)
        {
          std::string label = pp(m_lts.action_label(i->label()));
          size_t index = label.find(":");

          if ((m_translation == to_lts_deadlock && i->to() == deadlock_state)
              ||(m_translation == to_lts_outgoing_transition && index == std::string::npos)
              ||(m_translation == to_lts_selfloop && index != std::string::npos))
          {
            // transition recording block/operand info.
            // strip self: from label...
            if (index != std::string::npos)
            {
              label = label.substr(index+1, label.size());
            }

            size_t comma_pos = label.find(",");
            std::string block_str = label.substr(0,comma_pos);
            block_str.replace(0,block_str.find("(")+1,"");
            block_str.replace(block_str.find(")"),1,"");

            std::string op_str = label.substr(comma_pos);
            op_str.replace(0,op_str.find("(")+1,"");
            op_str.replace(op_str.find(")"),1,"");

            block = atoi(block_str.c_str());
            op = string_to_operand(op_str);

            if (m_translation == to_lts_outgoing_transition)
            {
              // Construct part of formula
              std::stringstream name;
              name << "X" << i->to();
              variables.push_back(boolean_variable(name.str()));
            }
          }
          else
          {
            // Construct part of formula
            std::stringstream name;
            name << "X" << i->to();
            variables.push_back(boolean_variable(name.str()));
          }
          ++i;
        }

        fixpoint_symbol fp;
        if (block % 2 == 0)
        {
          fp = fixpoint_symbol::nu();
        }
        else
        {
          fp = fixpoint_symbol::mu();
        }

        boolean_expression expr;
        switch (op)
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

      std::vector<boolean_equation> eqns;
      for (size_t i = 0; i <= blocks.size(); ++i)
      {
        std::map<size_t, std::vector<boolean_equation> >::const_iterator j = blocks.find(i);
        if (j != blocks.end())
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
    bes_reduction_algorithm(boolean_equation_system<Container>& v_bes, const equivalence_t equivalence=eq_stut, const to_lts_translation_t translation = to_lts_selfloop, const std::string& lts_filename = "", const bool to_lts_only = false)
      : detail::bes_algorithm<Container>(v_bes),
        m_equivalence(equivalence),
        m_translation(translation),
        m_lts_filename(lts_filename),
        m_translate_to_lts_only(to_lts_only)

    {
      initialise_allowed_eqs();
    }

    void run(utilities::execution_timer& timing)
    {
      mCRL2log(log::verbose) << "Reducing BES modulo " << m_equivalence_strings[m_equivalence] << std::endl;
      mCRL2log(debug) << "Converting BES to standard form" << std::endl;

      timing.start("standard form conversion");
      make_standard_form(m_bes, true);
      timing.finish("standard form conversion");

      mCRL2log(debug) << "BES Reduction algorithm initialised" << std::endl;

      timing.start("conversion to LTS");
      bes_to_lts();
      timing.finish("conversion to LTS");

      if(m_translate_to_lts_only)
      {
        return;
      }

      timing.start("reduction");
      reduce_lts();
      timing.finish("reduction");

      timing.start("conversion to BES");
      lts_to_bes();
      timing.finish("conversion to BES");

      mCRL2log(log::verbose) << "Removing unreachable equations" << std::endl;
      super::remove_unreachable_equations();
    }

};
} // namespace bes
} // namespace mcrl2


/// \brief Simple input/output tool to perform strong as well as oblivious bisimulation
///        reduction on a boolean equation system.
typedef bes_input_output_tool<input_output_tool> super;
class besconvert_tool: public super
{
  protected:
    bes_reduction_algorithm<>::equivalence_t equivalence;
    std::string m_lts_filename;
    bes_reduction_algorithm<>::to_lts_translation_t m_translation;
    bool m_no_reduction;

    void add_options(mcrl2::utilities::interface_description& desc)
    {
      using namespace mcrl2::utilities::tools;
      using namespace mcrl2::utilities;
      super::add_options(desc);

      desc.add_option("equivalence", make_mandatory_argument("NAME"),
                      "generate an equivalent BES, preserving equivalence NAME:"
                      "  'bisim' for strong bisimulation,\n"
                      "  'stuttering' for stuttering equivalence (default)", 'e');
      desc.add_option("intermediate", make_file_argument("FILE"),
                      "save the intermediate LTS to FILE", 'l');
      desc.add_option("translation", make_mandatory_argument("TRANSLATION"),
                      "translate to intermediate LTS using TRANSLATION:\n"
                      "  'deadlock' for an additional deadlock state recording labels,\n"
                      "  'selfloop' for a self-loop recording the information in each state,\n"
                      "  'successor' for an edge with the label of the current state to each successor state"
                      "              (may only be used with --equivalence=bisim)", 't');
      desc.add_option("noreduction",
                      "do not perform the reduction, only store the intermediate LTS", 'n');
    }

    void parse_options(const mcrl2::utilities::command_line_parser& parser)
    {
      super::parse_options(parser);

      if (parser.options.count("equivalence"))
      {
        boolean_equation_system<> b; // TODO: build proper solution.
        mcrl2::bes::bes_reduction_algorithm<> a(b);
        equivalence = a.parse_equivalence(parser.option_argument("equivalence"));
        if (a.allowed_eqs().count(equivalence) == 0)
        {
          parser.error("option -e/--equivalence has illegal argument '" +
                       parser.option_argument("equivalence") + "'");
        }
      }

      if (parser.options.count("intermediate"))
      {
        m_lts_filename = parser.option_argument("intermediate");
      }

      if (parser.options.count("translation"))
      {
        std::string str_translation(parser.option_argument("translation"));
        if (str_translation == "deadlock")
        {
          m_translation = bes_reduction_algorithm<>::to_lts_deadlock;
        }
        else if (str_translation == "selfloop")
        {
          m_translation = bes_reduction_algorithm<>::to_lts_selfloop;
        }
        else if (str_translation == "successor")
        {
          m_translation = bes_reduction_algorithm<>::to_lts_outgoing_transition;
        }
        else
        {
          parser.error("option -t/--translation has illegal argument `" + str_translation + "'");
        }
      }

      if (equivalence != bes_reduction_algorithm<>::eq_bisim && m_translation == bes_reduction_algorithm<>::to_lts_outgoing_transition)
      {
        parser.error("option --translation=successor can only be used with --equivalence=bisim");
      }

      m_no_reduction = parser.options.count("noreduction") > 0;
    }

  public:

    besconvert_tool()
      : super(
        "besconvert",
        "Jeroen Keiren",
        "reduce a BES (or PBES) using (variations of) behavioural equivalences",
        "reduce the (P)BES in INFILE modulo write the result to OUTFILE (as PBES)."
        "If INFILE is not "
        "present, stdin is used. If OUTFILE is not present, stdout is used."),
      equivalence(bes_reduction_algorithm<>::eq_stut),
      m_translation(bes_reduction_algorithm<>::to_lts_selfloop),
      m_no_reduction(false)
    {}

    bool run()
    {
      using namespace mcrl2::bes;
      using namespace mcrl2;

      boolean_equation_system<> b;

      mCRL2log(verbose) << "Loading BES from input file...";
      load_bes(b, input_filename(), bes_input_format());

      bool reach = detail::bes_algorithm<>(b).remove_unreachable_equations();
      if(!reach)
      {
        throw mcrl2::runtime_error("expect all equations to be reachable");
      }

      mCRL2log(verbose) << "done" << std::endl;
      bes_reduction_algorithm<std::vector<boolean_equation> >(b, equivalence, m_translation, m_lts_filename, m_no_reduction).run(timer());
      save_bes(b, output_filename(), bes_output_format());

      return true;
    }
};


int main(int argc, char* argv[])
{
  return besconvert_tool().execute(argc, argv);
}
