// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/data/data_expression.h"
#include "pbes2yices.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;

static data_expression and_or(data_expression left, data_expression right, bool disjunction)
{
	if (disjunction) {
		if (left == sort_bool::true_() || right == sort_bool::true_()) {
			return sort_bool::true_();
		} else if (left == sort_bool::false_()) {
			return right;
		} else if (right == sort_bool::false_()) {
			return left;
		} else {
			return sort_bool::or_(left, right);
		}
	} else {
		if (left == sort_bool::false_() || right == sort_bool::false_()) {
			return sort_bool::false_();
		} else if (left == sort_bool::true_()) {
			return right;
		} else if (right == sort_bool::true_()) {
			return left;
		} else {
			return sort_bool::and_(left, right);
		}
	}
}

static bool parse_condition(pbes_expression expression, bool disjunctive, bool negate, data_expression &condition, variable_vector &quantification_domain)
{
	assert(find_propositional_variable_instantiations(expression).empty());
	assert(quantification_domain.empty());
	
	while (is_pbes_not(expression)) {
		negate = !negate;
		expression = accessors::arg(expression);
	}
	if (is_pbes_imp(expression)) {
		expression = or_(accessors::left(expression), not_(accessors::right(expression)));
	}
	
	if (is_pbes_forall(expression) || is_pbes_exists(expression)) {
		if (is_pbes_exists(expression) ^ disjunctive ^ negate) {
			std::cerr << "Wat lastig, een verkeerde quantificatie\n";
			return false;
		}
		
		variable_list variables = accessors::var(expression);
		bool result = parse_condition(accessors::arg(expression), disjunctive, negate, condition, quantification_domain);
		quantification_domain.insert(quantification_domain.end(), variables.begin(), variables.end());
		return result;
	} else if (is_pbes_and(expression) || is_pbes_or(expression)) {
          data_expression left_condition;
          data_expression right_condition;
          variable_vector left_domain;
          variable_vector right_domain;

          if (!parse_condition(accessors::left(expression), disjunctive, negate, left_condition, left_domain))
          {
            return false;
		}
		if (!parse_condition(accessors::right(expression), disjunctive, negate, right_condition, right_domain)) {
			return false;
		}
		
		quantification_domain.insert(quantification_domain.end(), left_domain.begin(), left_domain.end());
		quantification_domain.insert(quantification_domain.end(), right_domain.begin(), right_domain.end());
		condition = and_or(left_condition, right_condition, is_pbes_or(expression) ^ negate);
		return true;
	} else {
		assert(is_data(expression));
		condition = negate ? sort_bool::not_(data_expression(expression)) : data_expression(expression);
		return true;
	}
}

bool parse_clauses(pbes_expression expression, bool disjunctive, propositional_variable_instantiation constant_instantiation, variable_vector quantification_domain, data_expression condition, bool negate, std::vector<clause> &clauses)
{
	assert(clauses.empty());
	
	while (is_pbes_not(expression)) {
		negate = !negate;
		expression = accessors::arg(expression);
	}
	if (is_pbes_imp(expression)) {
		expression = or_(accessors::left(expression), not_(accessors::right(expression)));
	}
	
	if (is_data(expression)) {
		data_expression new_condition = data_expression(expression);
		if (negate) {
			new_condition = sort_bool::not_(new_condition);
		}
		data_expression predicate = and_or(condition, new_condition, !disjunctive ^ negate);
		
		clause c;
		c.quantification_domain = quantification_domain;
		c.predicate = disjunctive ? predicate : sort_bool::not_(predicate);
		c.instantiation = constant_instantiation;
		clauses.push_back(c);
		return true;
	} else if (is_pbes_forall(expression) || is_pbes_exists(expression)) {
		if (is_pbes_exists(expression) ^ disjunctive ^ negate) {
			std::cerr << "Wat lastig, een verkeerde quantificatie\n";
			return false;
		}
		
		variable_list variables = accessors::var(expression);
		quantification_domain.insert(quantification_domain.end(), variables.begin(), variables.end());
		return parse_clauses(accessors::arg(expression), disjunctive, constant_instantiation, quantification_domain, condition, negate, clauses);
	} else if (is_pbes_and(expression) || is_pbes_or(expression)) {
		pbes_expression left = accessors::left(expression);
		pbes_expression right = accessors::right(expression);
		
		if (is_pbes_and(expression) ^ disjunctive ^ negate) {
                  std::vector<clause> left_clauses;
                  std::vector<clause> right_clauses;
                  if (!parse_clauses(left,
                          disjunctive,
                          constant_instantiation,
                          quantification_domain,
                          condition,
                          negate,
                          left_clauses))
                  {
                    return false;
			}
			if (!parse_clauses(right, disjunctive, constant_instantiation, quantification_domain, condition, negate, right_clauses)) {
				return false;
			}
			clauses.insert(clauses.end(), left_clauses.begin(), left_clauses.end());
			clauses.insert(clauses.end(), right_clauses.begin(), right_clauses.end());
			return true;
		}
		
		pbes_expression condition_part;
		pbes_expression recurse_part;
		
		if (find_propositional_variable_instantiations(left).empty()) {
			condition_part = left;
			recurse_part = right;
		} else if (find_propositional_variable_instantiations(right).empty()) {
			condition_part = right;
			recurse_part = left;
		} else {
			return false;
		}
		
		data_expression new_condition;
		variable_vector domain;
		if (!parse_condition(condition_part, disjunctive, negate, new_condition, domain)) {
			return false;
		}
		
		quantification_domain.insert(quantification_domain.end(), domain.begin(), domain.end());
		condition = and_or(condition, new_condition, !disjunctive ^ negate);
		return parse_clauses(recurse_part, disjunctive, constant_instantiation, quantification_domain, condition, negate, clauses);
	} else {
		assert(is_propositional_variable_instantiation(expression));
		propositional_variable_instantiation instantiation(expression);
		
		if (negate) {
			mCRL2log(log::error) << "Found negatively occurring propositional variable instantiation " << pbes_system::pp(instantiation) << ", giving up.\n";
			return false;
		}
		
		clause c;
		c.quantification_domain = quantification_domain;
		c.predicate = disjunctive ? condition : sort_bool::not_(condition);
		c.instantiation = instantiation;
		clauses.push_back(c);
		return true;
	}
}

bool parse_pbes(const pbes &pbes, bool disjunctive, parsed_pbes &output)
{
	assert(output.equations.empty());
	
	// Create a TRUE variable or a FALSE variable.
	core::identifier_string constant_variable_name(disjunctive ? "X_true" : "X_false");
	
	clause constant_clause;
	constant_clause.predicate = sort_bool::true_();
	constant_clause.instantiation = propositional_variable_instantiation(constant_variable_name, data_expression_list());
	
	equation constant_equation;
	constant_equation.symbol = disjunctive ? fixpoint_symbol::nu() : fixpoint_symbol::mu();
	constant_equation.variable = propositional_variable(constant_variable_name, variable_list());
	constant_equation.clauses.push_back(constant_clause);
	
	for (std::vector<pbes_equation>::const_iterator i = pbes.equations().begin(); i != pbes.equations().end(); ++i) {
		equation e;
		e.symbol = i->symbol();
		e.variable = i->variable();
		if (!parse_clauses(i->formula(), disjunctive, constant_clause.instantiation, variable_vector(), disjunctive ? sort_bool::true_() : sort_bool::false_(), false, e.clauses)) {
			output.equations.clear();
			return false;
		}
		output.equations.push_back(e);
	}
	output.equations.push_back(constant_equation);
	output.initial_state = pbes.initial_state();
	output.disjunctive = disjunctive;
	output.global_variables = pbes.global_variables();
	
/*	
	std::cerr << "Parsed form:\n====================\n";
	for (std::vector<equation>::iterator i = output.equations.begin(); i != output.equations.end(); ++i) {
		std::cerr << pbes_system::pp(i->symbol) << " " << pbes_system::pp(i->variable) << " = \n";
		for (std::vector<clause>::iterator j = i->clauses.begin(); j != i->clauses.end(); ++j) {
			std::cerr << "    (";
			if (!j->quantification_domain.empty()) {
				std::cerr << "exists ";
				for (variable_vector::iterator k = j->quantification_domain.begin(); k != j->quantification_domain.end(); ++k) {
					std::cerr << data::pp(*k);
					if (k != j->quantification_domain.end() - 1) {
						std::cerr << ", ";
					}
				}
				std::cerr << " . ";
			}
			std::cerr << data::pp(j->predicate);
			std::cerr << (output.disjunctive ? " && " : " || ");
			std::cerr << pbes_system::pp(j->instantiation);
			std::cerr << ")";
			if (j == i->clauses.end() - 1) {
				std::cerr << ";";
			} else {
				std::cerr << (output.disjunctive ? " ||" : " &&");
			}
			std::cerr << "\n\n";
		}
	}
*/	
	return true;
}

struct unrolling_variables
{
	// Contains the name of PBES global variables.
	std::map<data::variable, std::string> global_variables;
	// Contains, for each propositional variable name, its index.
	std::map<core::identifier_string, size_t> variable_indices;
	// Contains, for each round i, the name of the variable T_i.
	std::vector<std::string> equation_number_variables;
	// Contains, for each round i, equation j, and data parameter k of equation j, the name of the variable d_i_j_k.
	std::vector<std::vector<std::vector<std::string> > > parameter_variables;
	// Contains, for each round i, equation j, clause k, and quantification variable E, the name of the variable E.
	std::vector<std::vector<std::vector<std::vector<std::string> > > > quantification_variables;
	
	// The yices definition of the above variables.
	std::string definition;
};

static void build_unrolling_variables(const parsed_pbes &pbes, const translated_data_specification &translation, size_t rounds, unrolling_variables &variables)
{
	assert(rounds > 0);
	
	std::string global_variable_base = "global";
	std::string equation_number_base = "equation";
	std::string parameter_base = "parameter";
	std::string quantification_base = "quantification";
	
	for (std::set<data::variable>::const_iterator i = pbes.global_variables.begin(); i != pbes.global_variables.end(); ++i) {
		std::string global_variable = global_variable_base + "-" + std::string(i->name());
		variables.global_variables[*i] = global_variable;
		variables.definition += "(define " + global_variable + "::" + translation.sort_names.at(i->sort()) + ")\n";
	}
	
	for (size_t i = 0; i < pbes.equations.size(); ++i) {
		variables.variable_indices[pbes.equations[i].variable.name()] = i;
	}
	
	for (size_t round = 0; round < rounds; ++round) {
		std::string equation_number_variable = equation_number_base + "-" + itoa(round);
		variables.equation_number_variables.push_back(equation_number_variable);
		variables.definition += "(define " + equation_number_variable + "::nat)\n";
		
		std::vector<std::vector<std::string> > round_parameter_variables;
		std::vector<std::vector<std::vector<std::string> > > round_quantification_variables;
		for (std::vector<equation>::const_iterator i = pbes.equations.begin(); i != pbes.equations.end(); ++i) {
			std::vector<std::string> equation_parameter_variables;
			for (variable_list::const_iterator j = i->variable.parameters().begin(); j != i->variable.parameters().end(); ++j) {
				std::string parameter_variable =
					parameter_base + "-" +
					std::string(i->variable.name()) + "-" +
					std::string(j->name()) + "-" +
					itoa(round);
				equation_parameter_variables.push_back(parameter_variable);
				variables.definition += "(define " + parameter_variable + "::" + translation.sort_names.at(j->sort()) + ")\n";
			}
			round_parameter_variables.push_back(equation_parameter_variables);
			
			std::vector<std::vector<std::string> > equation_quantification_variables;
			for (std::vector<clause>::const_iterator j = i->clauses.begin(); j != i->clauses.end(); ++j) {
				std::vector<std::string> clause_quantification_variables;
				for (variable_vector::const_iterator k = j->quantification_domain.begin(); k != j->quantification_domain.end(); ++k) {
					std::string quantification_variable =
						quantification_base + "-" +
						std::string(i->variable.name()) + "-" +
						itoa(equation_quantification_variables.size()) + "-" +
						std::string(k->name()) + "-" +
						itoa(round);
					clause_quantification_variables.push_back(quantification_variable);
					variables.definition += "(define " + quantification_variable + "::" + translation.sort_names.at(k->sort()) + ")\n";
				}
				equation_quantification_variables.push_back(clause_quantification_variables);
			}
			round_quantification_variables.push_back(equation_quantification_variables);
		}
		variables.parameter_variables.push_back(round_parameter_variables);
		variables.quantification_variables.push_back(round_quantification_variables);
	}
}

static std::string assert_initial_state(const parsed_pbes &pbes, const translated_data_specification &translation, const unrolling_variables &variables)
{
	std::string output;
	size_t variable_index = variables.variable_indices.at(pbes.initial_state.name());
	output += "(assert (= " + variables.equation_number_variables[0] + " " + itoa(variable_index) + "))\n";
	
	const std::vector<std::string> &equation_variables = variables.parameter_variables[0][variable_index];
	data_expression_list values = pbes.initial_state.parameters();
	size_t index = 0;
        for (const data_expression& value : values)
        {
          output += "(assert (= " + equation_variables[index++] + " "
                    + translate_expression(value, variables.global_variables, translation) + "))\n";
        }

        return output;
}

static std::string join(const std::vector<std::string> &clauses, const std::string &op, bool simplify = true)
{
	assert(clauses.size() > 0);
	if (clauses.size() == 1 && simplify) {
		return clauses[0];
	}
	std::string output = "(" + op;
	for (std::vector<std::string>::const_iterator i = clauses.begin(); i != clauses.end(); ++i) {
		output += " " + *i;
	}
	output += ")";
	return output;
}

static std::string assert_occurs(const parsed_pbes &pbes, const translated_data_specification &translation, const unrolling_variables &variables, size_t from_round, size_t to_round)
{
	std::vector<std::string> possibilities;
	for (std::vector<equation>::const_iterator i = pbes.equations.begin(); i != pbes.equations.end(); ++i) {
		size_t from_variable_index = variables.variable_indices.at(i->variable.name());
		for (std::vector<clause>::const_iterator j = i->clauses.begin(); j != i->clauses.end(); ++j) {
			size_t to_variable_index = variables.variable_indices.at(j->instantiation.name());
			std::vector<std::string> clauses;
			
			clauses.push_back("(= " + variables.equation_number_variables.at(from_round) + " " + itoa(from_variable_index) + ")");
			clauses.push_back("(= " + variables.equation_number_variables.at(to_round) + " " + itoa(to_variable_index) + ")");
			
			std::map<mcrl2::data::variable, std::string> bound_variables = variables.global_variables;
			size_t index = 0;
			for (const mcrl2::data::variable& k: i->variable.parameters())
			{
				bound_variables[k] = variables.parameter_variables[from_round][from_variable_index][index++];
			}

			for (variable_vector::const_iterator k = j->quantification_domain.begin(); k != j->quantification_domain.end(); ++k) {
				bound_variables[*k] = variables.quantification_variables[from_round][from_variable_index][j - i->clauses.begin()][k - j->quantification_domain.begin()];
			}
			
			if (j->predicate != sort_bool::true_()) {
				clauses.push_back(translate_expression(j->predicate, bound_variables, translation));
			}
			
			index = 0;
			for (const mcrl2::data::data_expression& k: j->instantiation.parameters())
			{
				std::string rhs = translate_expression(k, bound_variables, translation);
				std::string lhs = variables.parameter_variables[to_round][to_variable_index][index++];
				clauses.push_back("(= " + lhs + " " + rhs + ")");
			}

			possibilities.push_back("\n  " + join(clauses, "and"));
		}
	}
	
	return "(assert " + join(possibilities, "or") + ")\n";
}

static std::string define_assert_witness(const parsed_pbes &pbes, const translated_data_specification &translation, const unrolling_variables &variables, size_t levels)
{
	std::string output;
	
	std::string base = "witness";
	std::string start_round_variable = base + "-start";
	std::string end_round_variable = base + "-end";
	std::string equation_variable = base + "-equation";
	
	output += "(define " + start_round_variable + "::nat)\n";
	output += "(define " + end_round_variable + "::nat)\n";
	output += "(define " + equation_variable + "::nat)\n";
	
	std::vector<std::vector<std::string> > parameter_variables;
	for (std::vector<equation>::const_iterator i = pbes.equations.begin(); i != pbes.equations.end(); ++i) {
		std::vector<std::string> equation_parameter_variables;
		for (variable_list::const_iterator j = i->variable.parameters().begin(); j != i->variable.parameters().end(); ++j) {
			std::string parameter_variable =
				base + "-parameter-" +
				std::string(i->variable.name()) + "-" +
				std::string(j->name());
			equation_parameter_variables.push_back(parameter_variable);
			output += "(define " + parameter_variable + "::" + translation.sort_names.at(j->sort()) + ")\n";
		}
		parameter_variables.push_back(equation_parameter_variables);
	}
	
	output += "(assert (< " + start_round_variable + " " + end_round_variable + "))\n";
	
	std::vector<std::string> valid_equation_options;
	for (std::vector<equation>::const_iterator i = pbes.equations.begin(); i != pbes.equations.end(); ++i) {
		if (pbes.disjunctive ? i->symbol.is_nu() : i->symbol.is_mu()) {
			std::string option = "(= " + equation_variable + " " + itoa(variables.variable_indices.at(i->variable.name())) + ")";
			valid_equation_options.push_back(option);
		}
	}
	output += "(assert " + join(valid_equation_options, "or") + ")\n";
	
	std::vector<std::string> start_round_options;
	std::vector<std::string> end_round_options;
	for (size_t round = 0; round < levels; ++round) {
		output += "(assert (=> (and (<= " + start_round_variable + " " + itoa(round) + ") (<= " + itoa(round) + " " + end_round_variable + ")) (<= " + equation_variable + " " + variables.equation_number_variables[round] + ")))\n";
		
		std::vector<std::string> requirements;
		requirements.push_back("(= " + equation_variable + " " + variables.equation_number_variables[round] + ")");
		for (size_t i = 0; i < pbes.equations.size(); ++i) {
			for (size_t j = 0; j < pbes.equations[i].variable.parameters().size(); ++j) {
				requirements.push_back("(= " + parameter_variables[i][j] + " " + variables.parameter_variables[round][i][j] + ")");
			}
		}
		
		requirements.push_back("(= " + start_round_variable + " " + itoa(round) + ")");
		start_round_options.push_back("\n  " + join(requirements, "and"));
		
		requirements.pop_back();
		requirements.push_back("(= " + end_round_variable + " " + itoa(round) + ")");
		end_round_options.push_back("\n  " + join(requirements, "and"));
	}
	
	output += "(assert " + join(start_round_options, "or") + ")\n";
	output += "(assert " + join(end_round_options, "or") + ")\n";
	
	return output;
}

std::string generate_witness_proposition(const parsed_pbes &pbes, const translated_data_specification &translation, size_t levels)
{
	std::string output;
	output += translation.definition;
	
	unrolling_variables unrolling;
	build_unrolling_variables(pbes, translation, levels, unrolling);
	output += unrolling.definition;
	
	output += assert_initial_state(pbes, translation, unrolling);
	
	for (size_t i = 0; i < levels - 1; ++i) {
		output += assert_occurs(pbes, translation, unrolling, i, i + 1);
	}
	
	output += define_assert_witness(pbes, translation, unrolling, levels);
	
	output += "(check)\n";
	return output;
}

static std::string assert_distinct(const parsed_pbes &pbes, const translated_data_specification &translation, const unrolling_variables &unrolling, size_t levels)
{
	std::string output;
	
	output += "(define-type pbes-state (datatype";
	for (size_t i = 0; i < pbes.equations.size(); ++i) {
		if (pbes.equations[i].variable.parameters().size() == 0) {
			output += " pbes-state-" + itoa(i);
		} else {
			output += " (pbes-state-" + itoa(i);
			size_t index = 0;
			for (data::variable_list::const_iterator j = pbes.equations[i].variable.parameters().begin(); j != pbes.equations[i].variable.parameters().end(); ++j) {
				output += " pbes-state-var-" + itoa(i) + "-" + itoa(index) + "::" + translation.sort_names.at(j->sort());
				index++;
			}
			output += ")";
		}
	}
	output += "))\n";
	
	output += "(define make-pbes-state::(-> nat";
        for (const equation& equation: pbes.equations)
        {
          for (const mcrl2::data::variable& j: equation.variable.parameters())
          {
            output += " " + translation.sort_names.at(j.sort());
          }
        }
        output += " pbes-state) (lambda (equation::nat";
	for (size_t i = 0; i < pbes.equations.size(); ++i) {
		size_t index = 0;
		for (data::variable_list::const_iterator j = pbes.equations[i].variable.parameters().begin(); j != pbes.equations[i].variable.parameters().end(); ++j) {
			output += " var-" + itoa(i) + "-" + itoa(index) + "::" + translation.sort_names.at(j->sort());
			index++;
		}
	}
	output += ") ";
	for (size_t i = 0; i < pbes.equations.size(); ++i) {
		std::string term;
		if (pbes.equations[i].variable.parameters().size() == 0) {
			term = "pbes-state-" + itoa(i);
		} else {
			term = "(pbes-state-" + itoa(i);
			for (size_t j = 0; j < pbes.equations[i].variable.parameters().size(); ++j) {
				term += " var-" + itoa(i) + "-" + itoa(j);
			}
			term += ")";
		}
		
		if (i == pbes.equations.size() - 1) {
			output += " " + term;
		} else {
			output += "(if (= equation " + itoa(i) + ") " + term;
		}
	}
	for (size_t i = 0; i < pbes.equations.size() - 1; ++i) {
		output += ")";
	}
	output += "))\n";
	
	output += "(define pbes-state-number::(-> pbes-state nat))\n";
	for (size_t i = 0; i < levels; ++i) {
		output += "(assert (= " + itoa(i) + " (pbes-state-number (make-pbes-state " + unrolling.equation_number_variables[i];
		for (const std::vector<std::basic_string<char>>& j: unrolling.parameter_variables[i])
		{
			for (const std::basic_string<char>& k: j)
			{
				output += " " + k;
			}
		}
		output += "))))\n";
	}
	
	return output;
}

std::string generate_acyclic_unrolling_proposition(const parsed_pbes &pbes, const translated_data_specification &translation, size_t levels)
{
	std::string output;
	output += translation.definition;
	
	unrolling_variables unrolling;
	build_unrolling_variables(pbes, translation, levels, unrolling);
	output += unrolling.definition;
	
	output += assert_initial_state(pbes, translation, unrolling);
	
	for (size_t i = 0; i < levels - 1; ++i) {
		output += assert_occurs(pbes, translation, unrolling, i, i + 1);
	}
	
	output += assert_distinct(pbes, translation, unrolling, levels);
	
	output += "(check)\n";
	return output;
}
