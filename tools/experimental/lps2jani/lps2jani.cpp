// Author(s): Wieger Wesselink; Threads are added by Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2lts_parallel.cpp
/// \brief This tool transforms an .lps file into a file in Jani format such
///        that tools like Modest or Storm can be used to analyse probabilistic
///        aspects. As Jani hardly support data types, the translation only
///        works for .lps specs using simple data. 

#include <fstream>
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/is_stochastic.h"
#include "mcrl2/lps/untime.h" // For real_zero. Should be moved to another file. TODO.

using namespace mcrl2;
using utilities::tools::input_output_tool;
using data::tools::rewriter_tool;
using enumerator_element = data::enumerator_list_element_with_substitution<>;

class data_structures
{
  data::rewriter m_rewr;
  data::enumerator_identifier_generator m_id_generator;
  data::enumerator_algorithm<> m_enumerator;

  public:
    data_structures(const lps::stochastic_specification& stochastic_lpsspec)
      : m_rewr(stochastic_lpsspec.data()),
        m_enumerator(m_rewr, stochastic_lpsspec.data(), m_rewr, m_id_generator, false)
    {
    }

    const data::data_expression rewriter(const data::data_expression& e, data::mutable_indexed_substitution<>& sigma) const
    {
      return m_rewr(e, sigma);;
    }

    data::enumerator_algorithm<>& enumerator()
    {
      return m_enumerator;
    }
};

class lps2jani_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;
       
  public:
    lps2jani_tool()
      : super("lps2jani",
              "Jan Friso Groote",
              "generates a Jani file from a (stochastic) LPS",
              "Reads the LPS in INFILE and writes a corresponding file in Jani format "
              "to OUTFILE. The LPS must only contain simple data types (Numbers, Booleans) that the "
              "Jani format supports. See http://jani-spec.org for more information about the Jani format. "
              "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
    }

    void export_action_declarations_to_jani(const process::action_label_list& act_decls, 
                                            std::ostream& out)
    {
      if (!act_decls.empty())
      {
        out << "  \"actions\": [ ";
        std::string separator="";
        for(const process::action_label& d: act_decls)
        {
          out << separator << "{ \"name\": \"" << d.name() << "\"}";
          separator = ", ";
        }
        out << "],\n";
      }
    }

    void export_data_expression_to_jani(const data::data_expression& e_in,
                                        std::ostream& out,
                                        data::mutable_indexed_substitution<>& sigma,
                                        data_structures& d)
    {
      const data::data_expression e = d.rewriter(e_in, sigma);
      if (is_variable(e))
      {
        out << "\"" << atermpp::down_cast<data::variable>(e).name() << "\"";
      }
      else if (is_equal_to_application(e))
      {
        const data::application& appl = atermpp::down_cast<data::application>(e);
        out << " \"exp\": { \"left\": ";
        export_data_expression_to_jani(appl[0], out, sigma, d);
        out << ", \"op\": \"=\", \"right\": ";
        export_data_expression_to_jani(appl[1], out, sigma, d);
        out << " } ";
      }
      else if (data::sort_real::is_creal_application(e))
      {
        // out << pp(e);  
        const data::application& appl = atermpp::down_cast<data::application>(e);
        out << " \"exp\": { \"left\": ";
        export_data_expression_to_jani(appl[0], out, sigma, d);
        out << ", \"op\": \"/\", \"right\": ";
        export_data_expression_to_jani(appl[1], out, sigma, d);
        out << " } ";
      }
      else if (data::sort_real::is_divides_application(e))
      {
        const data::application& appl = atermpp::down_cast<data::application>(e);
        out << " \"exp\": { \"left\": ";
        export_data_expression_to_jani(appl[0], out, sigma, d);
        out << ", \"op\": \"/\", \"right\": ";
        export_data_expression_to_jani(appl[1], out, sigma, d);
        out << " } ";
      }
      else if (data::sort_int::is_int(e.sort()) ||
          data::sort_nat::is_nat(e.sort()) ||
          data::sort_pos::is_pos(e.sort()) ||
          e == mcrl2::data::sort_bool::true_() ||
          e == mcrl2::data::sort_bool::false_())
      {
        out << pp(e);
      }
      else 
      {
        throw mcrl2::runtime_error("Jani only supports expressions true, false and numbers. "
                                   "It does not support expression " + pp(e) +".");
      }
    }

    void export_initial_value_to_jani(const data::data_expression& init,
                                      std::ostream& out,
                                      const std::string& indent,
                                      data::mutable_indexed_substitution<>& sigma,
                                      data_structures& d)
    {
      out << indent << "  \"initial-value\": ";
      export_data_expression_to_jani(init, out, sigma, d);
      out << indent << "\n";
    }

    void export_sort_to_jani(const data::sort_expression& sort,
                             std::ostream& out,
                             const std::string& indent)
    {
      if (data::sort_bool::is_bool(sort))
      {
        out << indent << "\"bool\"";
      }
      else if (data::sort_int::is_int(sort))
      {
        out << indent << "\"int\"";
      }
      else if (data::sort_nat::is_nat(sort))
      {
        out << indent << "{" 
            << indent << "  \"base\": \"nat\",\n"
            << indent << "  \"kind\": \"bounded\",\n"
            << indent << "  \"lower-bound\": 0\n"
            << indent << "}\n";
      }
      else if (data::sort_pos::is_pos(sort))
      {
        out << indent << "{" 
            << indent << "  \"base\": \"int\",\n"
            << indent << "  \"kind\": \"bounded\",\n"
            << indent << "  \"lower-bound\": 1\n"
            << indent << "}\n";
      }
      else 
      {
        throw mcrl2::runtime_error("Jani only supports sorts bool, int, nat and pos. "
                                   "It does not support sort " + pp(sort) +".");
      }
    }

    void export_assignments_to_jani(const data::assignment_list& assignments,
                                    const data::variable_list& parameters,
                                    std::ostream& out,
                                    const std::string& indent,
                                    data::mutable_indexed_substitution<>& sigma,
                                    data_structures& d)
    {
      if (!parameters.empty())
      {
        out << indent << "\"assignments\":\n"
            << indent << "[ \n";
        std::string separator="";
        for(const data::assignment& ass: assignments)
        {
          out << separator << "\n" << indent << "  {\n" 
              << indent << "    \"ref\": \"" << ass.lhs().name() << "\",\n"
              << indent << "    \"value\": ";
          
          export_data_expression_to_jani(ass.rhs(), out, sigma, d);
          out << indent << "  }";
          separator = ",";
        }
        out << indent << "]\n";
      }
    }

    void export_distribution_instance_to_jani(const enumerator_element& p,
                                              const data::assignment_list& assignments,
                                              const data::variable_list& parameters,
                                              data::mutable_indexed_substitution<>& sigma,
                                              std::ostream& out,
                                              const std::string& indent,
                                              std::string& separator,
                                              data_structures& d)
    {
      out << separator 
          << indent << "{\n"
          << indent << "  \"location\": \"l\",\n"
          << indent << "  \"probability\":\n"
          << indent << "  { ";
      export_data_expression_to_jani(p.expression(), out, sigma, d);
      out << "},\n";
      export_assignments_to_jani(assignments, parameters, out, indent + "  ", sigma, d);
      //               "location": "l",
      //               "probability": { "exp": 0.5 },
      //               "assignments": [
      //                  {
      //                     "ref": "s",
      //                     "value": 1
      //                  }
      //               ]
      out << indent << "}";
      separator = ",\n";
    }

    void export_distribution_to_jani(const lps::stochastic_distribution& dist,
                                     const data::assignment_list& assignments,
                                     const data::variable_list& parameters,
                                     std::ostream& out,
                                     data::mutable_indexed_substitution<>& sigma,
                                     const std::string& indent,
                                     data_structures& d)
    {
      if (dist.is_defined())
      {
        std::string separator="";
        d.enumerator().enumerate<enumerator_element>(
                    dist.variables(),
                    dist.distribution(),
                    sigma,
                    [&](const enumerator_element& p) 
                    {
                      export_distribution_instance_to_jani(p, assignments, parameters, sigma, out, indent, separator, d);
                      return false;
                    },
                    [](const data::data_expression& e) 
                    { 
                      return e == data::real_zero(); 
                    }
        );
        out << "\n";
      }
      else
      {
        out << indent << "{\n"
            << indent << "  \"location\": \"l\",\n"
            << indent << "  \"probability\": { \"exp\": 1 },\n";
            export_assignments_to_jani(assignments, parameters, out, indent + "  ", sigma, d);
        out << indent << "}\n";
      }
    }

    void export_action_summands_to_jani(const lps::stochastic_action_summand_vector& action_summands,
                                        const data::variable_list& parameters,
                                        std::ostream& out,
                                        const std::string& indent,
                                        data::mutable_indexed_substitution<>& sigma,
                                        data_structures& d)
    {
      std::string separator;
      for(const lps::stochastic_action_summand& s: action_summands)
      {
        if (!s.summation_variables().empty())
        {
          throw mcrl2::runtime_error("There is a summand with a non-empty sum operator. Jani does not support this. Remove the sum operator using the command lpssuminst. The problemantic summand is " + pp(s) + ".");
        }
        out << separator 
            << indent << "{"
            // << indent << "\"action\": \"" << s.multi_action() << "\",\n" TO BE INSERTED MAYBE, NOW TAU. 
            << indent << "\"location\": \"l\",\n"
            << indent << "\"guard\": {";
        export_data_expression_to_jani(s.condition(), out, sigma, d);
        out << " },\n"
            << indent << "\"destinations\":\n"
            << indent << "[\n";
        export_distribution_to_jani(s.distribution(), s.assignments(), parameters, out, sigma, indent + "  ", d);
        out << indent << "]\n"
            << indent << "}";;
        separator = ","; // TODO
      }
    }

    void export_parameter_declarations_to_jani(const data::variable_list& parameters, 
                                               const lps::stochastic_process_initializer& initial_values,
                                               std::ostream& out,
                                               data::mutable_indexed_substitution<>& sigma,
                                               data_structures& d)
    {
      if (!parameters.empty())
      {
        const lps::stochastic_distribution& dist=initial_values.distribution();
        if (!dist.variables().empty())
        {
          throw mcrl2::runtime_error("The initial state has a distribution. This is not allowed in Jani. "
                                     "A solution is to add an extra action at the start. The problematic disttribution is " + pp(dist) + ".");
        }
        out << "  \"variables\": [ ";
        std::string separator="";
        data::data_expression_list::const_iterator initial_value_iterator = initial_values.expressions().begin();
        for(const data::variable& v: parameters)
        {
          out << separator 
              << "{ \"name\": \"" << v.name() << "\",\n"
              << "                   \"type\": ";
          export_sort_to_jani(v.sort(), out, "                             ");
          out << ",\n";
          export_initial_value_to_jani(*initial_value_iterator, out, "                 ", sigma, d);

          out << "                 }";
          separator = ",\n                 ";
          initial_value_iterator++;
        }
        out << "\n               ],\n";
      }
    }

    void export_specification_to_jani(const lps::stochastic_specification& spec, 
                                      std::ostream& out,
                                      const std::string& input_file_name,
                                      data_structures& d)
    {
      data::mutable_indexed_substitution<> sigma;
      out << "{ \"jani-version\": 1,\n"
          << "  \"name\": \"" << input_file_name << ".jani\",\n"
          << "  \"type\": \"dtmc\",\n";
      export_action_declarations_to_jani(spec.action_labels(), out);
      out << "  \"features\": [ \"derived-operators\" ],\n";
      export_parameter_declarations_to_jani(spec.process().process_parameters(), spec.initial_process(), out, sigma, d);

      out << "  \"properties\": [],\n"
          << "  \"automata\":\n"
          << "  [ {\n"
          << "      \"name\": \"" << input_file_name << "\",\n"
          << "      \"locations\": [ { \"name\": \"l\" }],\n"
          << "      \"initial-locations\": [\"l\"],\n"
          << "      \"edges\":\n"
          << "      [\n";
      export_action_summands_to_jani(spec.process().action_summands(),
                                     spec.process().process_parameters(),
                                     out,
                                     "       ",
                                     sigma,
                                     d);
      out << "      ]\n"
          << "    }\n"
          << "  ],\n"
          << "  \"system\":\n"
          << "  {\n"
          << "    \"elements\": [ { \"automaton\": \"" << input_file_name << "\" } ]\n"
          << "  }\n" 
          << "}\n";
    }

    bool run() override
    {
      // mCRL2log(log::verbose) << options << std::endl;

      lps::stochastic_specification stochastic_lpsspec;
      lps::load_lps(stochastic_lpsspec, input_filename());
      data_structures d(stochastic_lpsspec);
      // m_rewr=data::rewriter(stochastic_lpsspec.data(), false);
      // m_enumerator=std::move(data::enumerator_algorithm<>(m_rewr, stochastic_lpsspec.data(), m_rewr, m_id_generator, false));

      if (output_filename().empty())
      {
        export_specification_to_jani(stochastic_lpsspec, std::cout, input_filename(),d);
      }
      else
      {
        std::ofstream output_file(output_filename(), std::ofstream::out);
        export_specification_to_jani(stochastic_lpsspec, output_file, input_filename(), d);
      }
      return true;
    }

};

std::unique_ptr<lps2jani_tool> tool_instance;

int main(int argc, char** argv)
{
  tool_instance = std::make_unique<lps2jani_tool>();
  return tool_instance->execute(argc, argv);
}
