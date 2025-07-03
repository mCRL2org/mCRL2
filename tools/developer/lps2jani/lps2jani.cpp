// Author(s): Jan Friso Groote; usage of boost::json was implementen by P.H.M. van Spaendonck
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file  lps2jani.cpp
/// \brief This tool transforms an .lps file into a file in Jani format such
///        that tools like Modest or Storm can be used to analyse probabilistic
///        aspects. As Jani hardly supports data types, the translation only
///        works for .lps specs using simple data. 
///

#include <fstream>
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"

#include "mcrl2/data/enumerator.h"
#ifdef MCRL2_ENABLE_MACHINENUMBERS
#include "mcrl2/data/pos64.h"
#include "mcrl2/data/nat64.h"
#include "mcrl2/data/int64.h"
#include "mcrl2/data/real64.h"
#else
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#endif
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/is_stochastic.h"
#include "mcrl2/lps/untime.h" // For real_zero. Should be moved to another file. TODO.

#include <boost/json/src.hpp>

using namespace mcrl2;
using namespace boost::json;

using enumerator_element = data::enumerator_list_element_with_substitution<>;
using super = mcrl2::data::tools::rewriter_tool<mcrl2::utilities::tools::input_output_tool>;

class data_structures
{
  data::rewriter m_rewr;
  data::enumerator_identifier_generator m_id_generator;
  data::enumerator_algorithm<> m_enumerator;
  data::mutable_indexed_substitution<> m_sigma;

public:
  explicit data_structures(const lps::stochastic_specification& stochastic_lpsspec)
    : m_rewr(stochastic_lpsspec.data()),
    m_enumerator(m_rewr, stochastic_lpsspec.data(), m_rewr, m_id_generator, false)
  {
  }

  data::data_expression rewriter(const data::data_expression& e) const
  {
    return m_rewr(e, m_sigma);
  }

  data::enumerator_algorithm<>& enumerator()
  {
    return m_enumerator;
  }

  data::mutable_indexed_substitution<>& sigma()
  {
    return m_sigma;
  }
};

class lps2jani_tool : public super
{
  std::unique_ptr<data_structures> m_data_structures;
public:
  lps2jani_tool()
    : super("lps2jani",
      "Jan Friso Groote, PHM van Spaendonck",
      "generates a Jani file from a (stochastic) LPS",
      "Reads the LPS in INFILE and writes a corresponding file in Jani format "
      "to OUTFILE. The LPS must only contain simple data types (Numbers, Booleans) that the "
      "Jani format supports. See http://jani-spec.org/#Model for more information about the Jani format. "
      "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is used."
    )
  {}

private:
  /// \brief Converts the given action label list to a JSON array of actions.
  /// Actions use the schema:
  ///  {"name": Identifier}
  ///
  /// \param act_decls the to-be converted action declaration list.
  /// \return A JSON array corresponding to \ref act_decls.
  array convert_action_declarations(const process::action_label_list& act_decls) const
  {
    array actions;
    for (const process::action_label& d : act_decls)
    {
      actions.push_back(object{ {"name", (std::string)d.name()} });
    }
    return actions;
  }

  /// \brief Converts the given OpId into the JSON string that corresponds to it according to the JANI schema.
  ///
  /// \param opid the to-be converted OpId.
  /// \return A JSON string corresponding to the operator.
  string convert_operator_to_jani(const data::data_expression& opid) const {
    if (std::string op = mcrl2::data::pp(opid);
      "!=" == op) 
    {
      return reinterpret_cast<const char*>(u8"≠");
    }
    else if ("==" == op) 
    {
      return "=";
    }
    else if ("<=" == op) 
    {
      return reinterpret_cast<const char*>(u8"≤");
    }
    else if ("&&" == op) 
    {
      return reinterpret_cast<const char*>(u8"∧");
    }
    else if ("||" == op) 
    {
      return reinterpret_cast<const char*>(u8"∨");
    }
    else if ("@cReal" == op) 
    { 
      return "/"; // threat real numbers as a division operator
    }
    else 
    {
      return op.c_str();
    }
  }

  /// \brief Converts the given data expression into a corresponding JSON value in line with the JANI Expression schema.
  ///
  /// \param e_in the to-be converted expression.
  /// \return A JSON value corresponding to the expression.
  value convert_data_expression(const data::data_expression& e_in) const
  {
    const data::data_expression e = m_data_structures->rewriter(e_in);
    if (is_variable(e))
    {
      return static_cast<std::string>(atermpp::down_cast<data::variable>(e).name()).c_str();
    }
    else if (data::sort_pos::is_positive_constant(e) ||
      data::sort_nat::is_natural_constant(e) ||
      data::sort_int::is_integer_constant(e)) {
      return std::stoi(pp(e));
    }
    else if (data::sort_bool::is_true_function_symbol(e)) {
      return true;
    }
    else if (data::sort_bool::is_false_function_symbol(e)) {
      return false;
    }
    else if (data::sort_bool::is_not_application(e))
    {
      const data::application& appl = atermpp::down_cast<data::application>(e);
      return object{
        {"op", reinterpret_cast<const char*>(u8"¬")},
        {"exp", convert_data_expression(appl[0])}
      };
    }
    else if (is_greater_application(e)) // > is not supported within jani, and thus should be flipped
    {
      const data::application& appl = atermpp::down_cast<data::application>(e);
      return object{
        {"left", convert_data_expression(appl[1])},
        {"op", "<"},
        {"right", convert_data_expression(appl[0])}
      };
    }
    else if (is_greater_equal_application(e)) // >= is not supported within jani, and thus should be flipped
    {
      const data::application& appl = atermpp::down_cast<data::application>(e);
      return object{
        {"left", convert_data_expression(appl[1])},
        {"op", reinterpret_cast<const char*>(u8"≤")},
        {"right", convert_data_expression(appl[0])}
      };
    }
    else if (data::sort_bool::is_implies_application(e)) {
      const data::application& appl = atermpp::down_cast<data::application>(e);
      return object{
        {"op", "ite"},
        {"if", convert_data_expression(appl[0])},
        {"then", convert_data_expression(appl[1])},
        {"else", "true"}
      };
    }
    else if (data::sort_real::is_floor_application(e) ||
      data::sort_real::is_ceil_application(e))
    {
      const data::application& appl = atermpp::down_cast<data::application>(e);
      return object{
        {"op", pp(appl.head())},
        {"exp", convert_data_expression(appl[0])}
      };
    }
    else if (data::is_application(e) && e.size() == 3) {
      const data::application& appl = atermpp::down_cast<data::application>(e);
      return object{
        {"left", convert_data_expression(appl[0])},
        {"op", convert_operator_to_jani(appl.head())},
        {"right", convert_data_expression(appl[1])}
      };
    }
    else
    {
      throw mcrl2::runtime_error("Jani only supports expressions true, false and numbers. "
        "It does not support the main operator in the expression " + pp(e) + ".");
    }
  }

  /// \brief Converts the given sort expression into a corresponding JSON value in line with the JANI Type schema.
  ///
  /// \param sort to-be converted sort.
  /// \return A JSON value corresponding to the sort.
  value convert_sort_expression(const data::sort_expression& sort) const
  {
    if (data::sort_bool::is_bool(sort))
    {
      return "bool";
    }
    else if (data::sort_int::is_int(sort))
    {
      return "int";
    }
    else if (data::sort_nat::is_nat(sort))
    {
      return object{
        { "base", "int" },
        {"kind", "bounded"},
        {"lower-bound", 0}
      };
    }
    else if (data::sort_pos::is_pos(sort))
    {
      return object{
        {"base", "int"},
        {"kind", "bounded"},
        {"lower-bound", 1}
      };
    }
    else
    {
      throw mcrl2::runtime_error("Jani only supports sorts bool, int, nat and pos. "
        "It does not support sort " + pp(sort) + ".");
    }
  }

  /// \brief Converts the given assignment list into a sorresponding JSON array of assignments.
  /// Assignments use the following schema:
  ///  {"ref": LValue,
  ///   "value": Expression
  ///  }
  ///
  /// \param assignments the to-be converted assignments list.
  /// \return A json array corresponding to \ref assignements.
  array convert_assignments(const data::assignment_list& assignments) const
  {
    array assignments_as_array;
    for (const data::assignment& ass : assignments)
    {
      assignments_as_array.push_back(object{
        {"ref", (std::string)ass.lhs().name()},
        {"value", convert_data_expression(ass.rhs())}
        });
    }
    return assignments_as_array;
  }

  /// \brief Converts the given enumerator element and combines it with the given assingnments into a corresponding JSON object.
  ///
  /// \param p the to-be converted enumerator element.
  /// \param assignments a converted list of assignments.
  /// \return The corresponding JSON object.
  object export_distribution_instance_to_jani(const enumerator_element& p,
    const data::assignment_list& assignments) const
  {
    return object{
      {"location", "l"},
      {"probability", object{{"exp", convert_data_expression(p.expression())}} },
      {"assignments", convert_assignments(assignments)}
    };
  }

  /// \brief Converts the given stochastic distribution and assignments into a corresponding JSON array of destinations.
  /// The array contains only a single elements if the distribution is not defined.
  ///
  /// \param dist the to-be converted distribution.
  /// \param assignments the to-be converted assignments.
  /// \return The corresponding JSON array.
  array export_distribution_to_jani(const lps::stochastic_distribution& dist,
    const data::assignment_list& assignments) const
  {
    if (dist.is_defined())
    {
      array destinations;
      m_data_structures->enumerator().enumerate<enumerator_element>(
        dist.variables(),
        dist.distribution(),
        m_data_structures->sigma(),
        [&](const enumerator_element& p)
        {
          destinations.push_back(export_distribution_instance_to_jani(p, assignments));
      return false;
        },
        [](const data::data_expression& e)
        {
          return e == data::real_zero();
        });
      return destinations;
    }
    else
    {
      return array{ object{
        {"location", "l"},
        {"probability", object{ {"exp", 1} }},
        {"assignments", convert_assignments(assignments)}
        } };
    }
  }

  /// \brief Converts the given action summands to a JSON array of corresponding edges.
  /// Edges use the following schema:
  /// {"location": "l",
  ///  "guard": {"exp": Expression},
  ///  "destinations": Destination (see \ref export_distribution_to_jani)
  /// }
  ///
  /// \param action_summands the to-be converted action summands.
  /// \return A JSON array corresponding to \ref action_summands.
  array convert_action_summands(const lps::stochastic_action_summand_vector& action_summands) const
  {
    array edges;
    for (const lps::stochastic_action_summand& s : action_summands)
    {
      if (!s.summation_variables().empty())
      {
        throw mcrl2::runtime_error("There is a summand with a non-empty sum operator. Jani does not support this. Remove the sum operator using the command lpssuminst. The problemantic summand is " + pp(s) + ".");
      }
          if (s.multi_action().actions().empty())
          {
            edges.push_back(object{
              {"location", "l"},
              {"guard", object({{"exp", convert_data_expression(s.condition())}})},
              {"destinations", export_distribution_to_jani(s.distribution(), s.assignments())}
              });
          }
          else
          {
            edges.push_back(object{
              {"location", "l"},
              {"action", pp(s.multi_action().actions().front().label()) },     // Only the label of the first action is exported. 
              {"guard", object({{"exp", convert_data_expression(s.condition())}})},
              {"destinations", export_distribution_to_jani(s.distribution(), s.assignments())}
              });
          }
    }
    return edges;
  }

  /// \brief Converts the given process parameters to a JSON array of variable declaration in line with the JANI VariableDeclaration schema.
  ///
  /// \param parameters the to-be converted list of process parameters.
  /// \param initial_values the initial values of the process parameters.
  /// \return A JSON array corresponding to \ref parameters and \ref initial_values.
  ///
  /// \throw mcrl2::runtime_error if the initial state has a distribution, as this is not allowed in JANI.
  array convert_parameters(const data::variable_list& parameters,
    const lps::stochastic_process_initializer& initial_values) const
  {
    const lps::stochastic_distribution& dist = initial_values.distribution();
    if (!dist.variables().empty())
    {
      throw mcrl2::runtime_error("The initial state has a distribution. This is not allowed in Jani. "
        "A solution is to add an extra action at the start. The problematic distribution is " + pp(dist) + ".");
    }
    array variables;
    auto initial_value_iterator = initial_values.expressions().begin();
    for (const data::variable& v : parameters)
    {
      variables.push_back(object{
        {"name", (std::string)v.name()},
        {"type", convert_sort_expression(v.sort())},
        {"initial-value", convert_data_expression(*initial_value_iterator) }
        });
      initial_value_iterator++;
    }
    return variables;
  }

  /// \brief Converts the given stochastic specification into a JSON object corresponding to the JANI schema.
  ///
  /// \param spec the to-be converted stochastic specification.
  /// \param input_file_name the file name of the specification.
  /// \return A JSON object corresponding to \ref spec.
  object export_specification_to_jani(const lps::stochastic_specification& spec,
    const std::string& input_file_name) const
  {
    return object({
      {"jani-version", 1},
      {"name", input_file_name + ".jani"},
      {"type", "dtmc"},
      {"actions", convert_action_declarations(spec.action_labels())},
      {"features", array{ "derived-operators" }},
      {"variables", convert_parameters(spec.process().process_parameters(), spec.initial_process())},
      {"properties", array{}},
      {"automata", array{object{
        {"name", input_file_name},
        {"locations", array{ object{{"name", "l"}} } },
        {"initial-locations", array{"l"}},
        {"edges", convert_action_summands(spec.process().action_summands()) }
        }}},
      {"system", object{
        {"elements", array{
          object{ {"automaton", input_file_name} }
          }}
        }}
      });
  }

public:
  bool run() override
  {
    lps::stochastic_specification stochastic_lpsspec;
    lps::load_lps(stochastic_lpsspec, input_filename());
    m_data_structures = std::make_unique<data_structures>(stochastic_lpsspec);
    object jani = export_specification_to_jani(stochastic_lpsspec, input_filename());
    if (output_filename().empty())
    {
      std::cout << jani;
    }
    else
    {
      std::ofstream output_file(output_filename(), std::ofstream::out);
      output_file << jani;
    }
    return true;
  }

};


int main(int argc, char** argv)
{
  return lps2jani_tool().execute(argc, argv);
}
