// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef PBES2YICES_H
#define PBES2YICES_H

#include "mcrl2/data/data.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/pbes/pbes.h"

static inline std::string itoa(int number)
{
	char buffer[40];
	snprintf(buffer, sizeof(buffer), "%d", number);
	return std::string(buffer);
}

struct translated_data_specification
{
	std::string definition;
	std::map<mcrl2::data::sort_expression, std::string> sort_names;
	std::map<mcrl2::data::function_symbol, std::vector<std::string> > constructor_field_names;
	std::map<mcrl2::data::function_symbol, std::string> function_names;
};

struct clause
{
	mcrl2::data::variable_vector quantification_domain;
	mcrl2::data::data_expression predicate;
	mcrl2::pbes_system::propositional_variable_instantiation instantiation;
};

struct equation
{
	mcrl2::pbes_system::fixpoint_symbol symbol;
	mcrl2::pbes_system::propositional_variable variable;
	std::vector<clause> clauses;
};

struct parsed_pbes
{
	std::set<mcrl2::data::variable> global_variables;
	std::vector<equation> equations;
	mcrl2::pbes_system::propositional_variable_instantiation initial_state;
        bool disjunctive = false;
};

void translate_data_specification(const mcrl2::pbes_system::pbes &pbes, translated_data_specification &translation);

std::string translate_expression(mcrl2::data::data_expression expression, const std::map<mcrl2::data::variable, std::string> &bound_variables, const translated_data_specification &translation);

bool parse_pbes(const mcrl2::pbes_system::pbes &pbes, bool disjunctive, parsed_pbes &output);

std::string generate_witness_proposition(const parsed_pbes &pbes, const translated_data_specification &translation, size_t levels);

std::string generate_acyclic_unrolling_proposition(const parsed_pbes &pbes, const translated_data_specification &translation, size_t levels);

#endif
