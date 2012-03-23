// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_explorer.cpp
/// \brief
#include "mcrl2/pbes/pbes_explorer.h"
#include "mcrl2/pbes/detail/ppg_visitor.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;
using namespace mcrl2::core;
using namespace mcrl2::data;

namespace mcrl2
{

namespace pbes_system
{




/// lts_type

lts_type::lts_type(int state_length)
{
    this->state_length = state_length;
}


lts_type::~lts_type()
{}


int lts_type::get_state_length() const
{
    return this->state_length;
}


int lts_type::get_number_of_state_types() const
{
    return this->state_type_list.size();
}


int lts_type::get_state_type_no(int part) const
{
    return this->state_type_no.at(part);
}


const std::vector<std::string>& lts_type::get_state_names() const
{
    return state_names;
}


const std::vector<std::string>& lts_type::get_state_types() const
{
    return state_types;
}


size_t lts_type::get_number_of_state_labels() const
{
    return this->state_label_names.size();
}


const std::vector<std::string>& lts_type::get_state_labels() const
{
    return state_label_names;
}


const std::vector<std::string>& lts_type::get_state_label_types() const
{
    return state_label_types;
}


size_t lts_type::get_number_of_edge_labels() const
{
    return this->edge_label_names.size();
}


const std::vector<std::string>& lts_type::get_edge_labels() const
{
    return edge_label_names;
}


const std::vector<std::string>& lts_type::get_edge_label_types() const
{
    return edge_label_types;
}


void lts_type::add_state(const std::string& name, const std::string& type)
{
    //std::clog << "Adding state part " << this->state_names->size() << ": "
    //        << info::get_param_signature(name, type)
    //        << std::endl;
    this->state_names.push_back(name);
    this->state_types.push_back(type);
    int type_index;
    std::map<std::string,int>::iterator type_index_it = this->state_type_index.find(type);
    if (type_index_it != this->state_type_index.end()) {
        type_index = type_index_it->second;
    } else {
        this->state_type_list.push_back(type);
        type_index = this->state_type_list.size() - 1;
        this->state_type_index.insert(std::make_pair(type,type_index));
    }
    this->state_type_no.push_back(type_index);
    //std::clog << "  type_no = " << type_index << ": " << this->state_type_list->at(type_index) << std::endl;
}


void lts_type::add_state_label(const std::string& name,
                                  const std::string& type)
{
    this->state_label_names.push_back(name);
    this->state_label_types.push_back(type);
}


void lts_type::add_edge_label(const std::string& name,
                                 const std::string& type)
{
    this->edge_label_names.push_back(name);
    this->edge_label_types.push_back(type);
}




/// lts_info

lts_info::lts_info(pbes<>& p, pbes_greybox_interface* pgg, bool reset = false):
    p(p),
    pgg(pgg),
    reset_option(reset),
    type(0)
{
    if (!is_ppg(p))
    {
        throw std::runtime_error("PBES is not a PPG! Please rewrite with pbesrewr -pppg.");
    }
    //std::clog << "info: resetOption = " << (this->resetOption ? "true":"false") << ", reset = " << reset << std::endl;
    compute_lts_type();
    compute_transition_groups();
    compute_dependency_matrix();
}


lts_info::~lts_info()
{}


void lts_info::compute_lts_type()
{
    //std::clog << "pbes_type:" << std::endl;
    std::vector<std::string> params;
    std::map<std::string,std::string> paramtypes;
    //this->param_default_values = new atermpp::vector<data_expression>();
    for (atermpp::vector<pbes_equation>::iterator eqn = p.equations().begin(); eqn
            != p.equations().end(); ++eqn) {
        //std::clog << core::pp((*eqn).symbol()) << " " << (*eqn).variable().name()
        //        << std::endl;

        propositional_variable var = (*eqn).variable();
        for (atermpp::term_list<variable>::const_iterator param =
                var.parameters().begin(); param != var.parameters().end(); ++param) {
            variable varparam = (*param);
            std::string signature = get_param_signature(varparam);
            bool new_param = true;
            for (std::vector<std::string>::iterator par = params.begin(); par
                        != params.end(); ++par) {
               if (signature == (*par)) new_param = false;
            }
            //std::clog << "  " << signature << (new_param ? " [NEW] " : "");
            //if (new_param) std::clog << params.size()+1;
            //std::clog << std::endl;
            if (new_param) {
                params.push_back(signature);
                paramtypes[signature] = core::pp(varparam.sort());
                //std::clog << "paramtypes[" << signature << "] = " << paramtypes[signature] << std::endl;
                atermpp::vector< function_symbol > c = p.data().constructors(varparam.sort());
                if (c.size() == 0) {
                    throw(std::runtime_error("Error in info: no constructor for parameter sort " + signature + "."));
                } else {
                    assert(c.size() > 0);
                    function_symbol fs = c[0];
                    pbes_expression e = pgg->rewrite_and_simplify_expression(fs);
                    data_expression v = data_expression(e);
                    //std::clog << "pbes_type: " << signature << "(" << this->param_default_values.size() << ") value = " << v.to_string()
                    //        << " (" << pgg->data_to_string(v) << ")"<< std::endl;
                    this->param_default_values.push_back(v);
                }
            }
        }
        //params.sort();
    }
    //this->param_index = new std::map<std::string, int>();
    this->type = lts_type(1 + params.size());
    this->type.add_state("var", "string"); // Propositional variable name

    int i = 0;
    for (std::vector<std::string>::iterator param = params.begin(); param
            != params.end(); ++param) {
        std::string signature = (*param);
        this->type.add_state(signature, paramtypes[signature]);
        this->param_index.insert(std::make_pair(signature, i));
        i++;
    }

    this->type.add_state_label("priority", "int");
    this->type.add_state_label("type", "int");

    //this->type->add_edge_label("", "");
    //std::clog << "-- end of pbes_type." << std::endl;
}


void lts_info::compute_transition_groups()
{
    int group = 0;
    int priority = 0;
    operation_type type = parity_game_generator::PGAME_AND;
    fixpoint_symbol symbol = fixpoint_symbol::nu();
    ppg_visitor checker;

    std::string name = "true";
    propositional_variable t(name);
    this->variables.insert(std::make_pair(name, t));
    this->variable_type.insert(std::make_pair(name, type));
    this->variable_symbol.insert(std::make_pair(name, symbol));
    this->variable_priority.insert(std::make_pair(name, priority));
    this->variable_parameters.insert(std::make_pair(name, t.parameters()));
    this->variable_parameter_signatures.insert(std::make_pair(name, get_param_sequence(t.parameters())));
    this->variable_parameter_indices.insert(std::make_pair(name, this->get_param_indices(t.parameters())));
    this->variable_parameter_index_positions.insert(std::make_pair(name, this->get_param_index_positions(t.parameters())));
    priority++;

    name = "false";
    symbol = fixpoint_symbol::mu();
    propositional_variable f(name);
    this->variables.insert(std::make_pair(name, f));
    this->variable_type.insert(std::make_pair(name, type));
    this->variable_symbol.insert(std::make_pair(name, symbol));
    this->variable_priority.insert(std::make_pair(name, priority));
    this->variable_parameters.insert(std::make_pair(name, f.parameters()));
    this->variable_parameter_signatures.insert(std::make_pair(name, get_param_sequence(f.parameters())));
    this->variable_parameter_indices.insert(std::make_pair(name, this->get_param_indices(f.parameters())));
    this->variable_parameter_index_positions.insert(std::make_pair(name, this->get_param_index_positions(f.parameters())));
    priority++;

    symbol = fixpoint_symbol::nu();

    for (atermpp::vector<pbes_equation>::iterator eqn = p.equations().begin(); eqn
            != p.equations().end(); ++eqn) {
        pbes_expression expr = pgg->from_rewrite_format(pgg->get_pbes_equation((*eqn).variable().name()).formula());
        std::string variable_name = (*eqn).variable().name();
        this->variables.insert(
                                std::make_pair(variable_name, (*eqn).variable()));
        type = pgg->get_expression_operation(expr);
        this->variable_type.insert(std::make_pair(variable_name, type));
        this->variable_symbol.insert(std::make_pair(variable_name,
                                                     (*eqn).symbol()));
        if ((*eqn).symbol() != symbol) {
            priority++;
        }
        this->variable_priority.insert(std::make_pair(variable_name, priority));
        std::pair<std::string,data::variable_list> var_params_entry = std::make_pair(variable_name, (*eqn).variable().parameters());
        this->variable_parameters.insert(var_params_entry);
        std::pair<std::string,std::vector<std::string> > var_param_signatures_entry = std::make_pair(variable_name, get_param_sequence((*eqn).variable().parameters()));
        this->variable_parameter_signatures.insert(var_param_signatures_entry);
        std::pair<std::string,std::vector<int> > var_param_indices_entry = std::make_pair(variable_name, this->get_param_indices((*eqn).variable().parameters()));
        this->variable_parameter_indices.insert(var_param_indices_entry);
        std::pair<std::string,std::map<int,int> > var_param_index_positions_entry = std::make_pair(variable_name, this->get_param_index_positions((*eqn).variable().parameters()));
        this->variable_parameter_index_positions.insert(var_param_index_positions_entry);
        atermpp::vector<pbes_expression> expression_parts;
        if (tr::is_and(expr)) {
            expression_parts = pbes_expr::split_conjuncts(expr);
        } else if (tr::is_or(expr)) {
            expression_parts = pbes_expr::split_disjuncts(expr);
        } else {
            expression_parts.push_back(expr);
        }
        for (atermpp::vector<pbes_expression>::const_iterator e =
                expression_parts.begin(); e != expression_parts.end(); ++e) {
            this->transition_expression.insert(std::make_pair(group, pgg->rewrite_and_simplify_expression(*e)));
            this->transition_variable_name.insert(
                                                   std::make_pair(group,
                                                                  variable_name));
            this->transition_type.insert(std::make_pair(group, type));
            //std::clog << std::endl << "Add transition group " << group << ": "
            //        << (type==parity_game_generator::PGAME_AND ? "AND" : "OR") << " " << variable_name << " "
            //        << core::pp(*e) << std::endl;
            group++;
        }
    }
    number_of_groups = group;
    //std::clog << "Added " << group << " transition groups." << std::endl;
}


void lts_info::compute_dependency_matrix()
{
    for(int group=0; group < number_of_groups; group++)
    {
        std::vector<bool> dep_row;
        std::vector<bool> read_row;
        std::vector<bool> write_row;
        bool r = is_read_dependent_propvar(group);
        bool w = is_write_dependent_propvar(group);
        bool d = r || w;
        dep_row.push_back(d);
        read_row.push_back(r);
        write_row.push_back(w);
        for (int part = 1; part < type.get_state_length(); part++)
        {
            r = is_read_dependent_parameter(group, part);
            w = is_write_dependent_parameter(group, part);
            d = r || w;
            dep_row.push_back(d);
            read_row.push_back(r);
            write_row.push_back(w);
        }
        matrix.insert(std::make_pair(group, dep_row));
        read_matrix.insert(std::make_pair(group, read_row));
        write_matrix.insert(std::make_pair(group, write_row));
    }
}


bool lts_info::get_reset_option() const
{
    return reset_option;
}


int lts_info::get_number_of_groups() const
{
    return number_of_groups;
}


const atermpp::map<int, pbes_expression>& lts_info::get_transition_expressions() const
{
    return transition_expression;
}


const std::map<int, std::string>& lts_info::get_transition_variable_names() const
{
    return transition_variable_name;
}


const atermpp::map<int, lts_info::operation_type>& lts_info::get_transition_types() const
{
    return transition_type;
}


const atermpp::map<std::string, propositional_variable>& lts_info::get_variables() const
{
    return variables;
}


const atermpp::map<std::string, lts_info::operation_type>& lts_info::get_variable_types() const
{
    return variable_type;
}

const atermpp::map<std::string, fixpoint_symbol>& lts_info::get_variable_symbols() const
{
    return variable_symbol;
}


const std::map<std::string, int>& lts_info::get_variable_priorities() const
{
    return variable_priority;
}


const atermpp::map<std::string, data::variable_list>& lts_info::get_variable_parameters() const
{
    return variable_parameters;
}


const std::map<std::string, std::vector<std::string> >& lts_info::get_variable_parameter_signatures() const
{
    return variable_parameter_signatures;
}


const std::map<std::string, std::vector<int> >& lts_info::get_variable_parameter_indices() const
{
    return variable_parameter_indices;
}


const std::map<std::string, std::map<int,int> >& lts_info::get_variable_parameter_index_positions() const
{
    return variable_parameter_index_positions;
}


const lts_type& lts_info::get_lts_type() const
{
    return type;
}


const std::map<int,std::vector<bool> >& lts_info::get_dependency_matrix() const
{
    return matrix;
}


const std::map<int,std::vector<bool> >& lts_info::get_read_matrix() const
{
    return read_matrix;
}


const std::map<int,std::vector<bool> >& lts_info::get_write_matrix() const
{
    return write_matrix;
}


int lts_info::get_index(const std::string& signature)
{
    return param_index.at(signature);
}


const data_expression& lts_info::get_default_value(int index)
{
    return param_default_values.at(index);
}


bool lts_info::is_read_dependent_propvar(int /* group */)
{
    return true;
}


bool lts_info::is_read_dependent_parameter(int group, int part)
{
    std::string p = type.get_state_names().at(part);
    pbes_expression phi = pgg->from_rewrite_format(transition_expression.at(group));
    std::set<std::string> usedSet = used(phi);
    std::string X = transition_variable_name.at(group);
    if (usedSet.find(p) == usedSet.end())
    {
        return false; // Parameter is not in used(phi).
    }
    std::set<std::string> params = lts_info::get_param_set(variable_parameters.at(X));
    if (params.find(p) != params.end())
    {
        return true; // Parameter is both in used(phi) and in params(X).
    }
    return false; // Parameter is in used(phi), but not in params(X).
}


bool lts_info::is_write_dependent_propvar(int group)
{
    pbes_expression phi = pgg->from_rewrite_format(transition_expression.at(group));
    std::string X = transition_variable_name.at(group);
    if (lts_info::tf(phi))
    {
        return true;
    }
    std::set<std::string> occ = lts_info::occ(phi);
    if (occ.empty())
    {
        return false; // Not dependent if occ(phi) == {}.
    }
    else if (occ.size() == 1)
    {
        bool containsX = occ.find(X) != occ.end();
        return !containsX; // Not dependent if occ(phi) == {X}.
    }
    else
    {
        return true; // Dependent, because occ(phi) contains multiple elements.
    }
}


bool lts_info::is_write_dependent_parameter(int group , int part)
{
    std::string p = type.get_state_names().at(part);
    pbes_expression phi = pgg->from_rewrite_format(transition_expression.at(group));
    std::string X = transition_variable_name.at(group);
    if (this->reset_option) {
        if (lts_info::tf(phi))
        {
            // phi may have boolean result (not only propositional variable instantiations)
            return true;
        }
        std::set<std::string> params = lts_info::get_param_set(variable_parameters.at(X));
        std::set<std::string> resetSet = reset(phi, params);
        if (resetSet.find(p) != resetSet.end())
        {
            return true; // Dependent, because p in reset(phi, params(X)).
        }
    }
    std::set<std::string> emptySet;
    std::set<std::string> changedSet = changed(phi, emptySet);
    bool changedSetContainsP = (changedSet.find(p) != changedSet.end());
    return changedSetContainsP; // Dependent, because p in changed(phi, {}).
}


std::set<std::string> lts_info::changed(const pbes_expression& phi, const std::set<std::string>& L)
{
    std::set<std::string> result;
    if (tr::is_not(phi))
    {
        result = changed(tr::arg(phi), L);
    }
    else if (tr::is_and(phi) || tr::is_or(phi) || tr::is_imp(phi))
    {
        std::set<std::string> l = changed(tr::left(phi), L);
        result.insert(l.begin(), l.end());
        std::set<std::string> r = changed(tr::right(phi), L);
        result.insert(r.begin(), r.end());
    }
    else if (tr::is_forall(phi) || tr::is_exists(phi))
    {
        std::set<std::string> LL;
        LL.insert(L.begin(), L.end());
        data::variable_list vars = tr::var(phi);
        for (data::variable_list::const_iterator var =
                vars.begin(); var != vars.end(); ++var)
        {
            variable variable = *var;
            LL.insert(get_param_signature(variable));
        }
        result = changed(tr::arg(phi), LL);
    }
    else if (tr::is_prop_var(phi))
    {
        std::vector<std::string> var_param_signatures =
                    variable_parameter_signatures.at(tr::name(phi));
        data::data_expression_list values = tr::param(phi);
        assert(var_param_signatures.size() == values.size());
        data::data_expression_list::const_iterator val = values.begin();
        for (std::vector<std::string>::const_iterator param =
                var_param_signatures.begin(); param != var_param_signatures.end(); ++param) {
            std::string param_signature = *param;
            if (tr::is_variable(*val))
            {
                variable value = *val;
                std::string value_signature = get_param_signature(value);
                if (param_signature != value_signature || L.find(value_signature) != L.end())
                {
                    result.insert(param_signature);
                }
            }
            else
            {
                result.insert(param_signature);
            }
            if (val != values.end()) {
                ++val;
            }
        }
    }
    return result;
}


std::set<std::string> lts_info::reset(const pbes_expression& phi, const std::set<std::string>& d)
{
    std::set<std::string> result;
    if (tr::is_not(phi))
    {
        result = reset(tr::arg(phi), d);
    }
    else if (tr::is_and(phi) || tr::is_or(phi) || tr::is_imp(phi))
    {
        std::set<std::string> l = reset(tr::left(phi), d);
        result.insert(l.begin(), l.end());
        std::set<std::string> r = reset(tr::right(phi), d);
        result.insert(r.begin(), r.end());
    }
    else if (tr::is_forall(phi) || tr::is_exists(phi))
    {
        result = reset(tr::arg(phi), d);
    }
    else if (tr::is_prop_var(phi))
    {
        std::set<std::string> params;
        std::vector<std::string> var_params =
                    variable_parameter_signatures.at(tr::name(phi));
        for (std::vector<std::string>::const_iterator param =
                var_params.begin(); param != var_params.end(); ++param) {
            std::string signature = *param;
            params.insert(signature);
        }
        for (std::set<std::string>::const_iterator s = d.begin(); s != d.end(); ++s) {
            std::string signature = *s;
            if (params.find(signature) == params.end())
            {
                result.insert(signature);
            }
        }
    }
    return result;
}


bool lts_info::tf(const pbes_expression& phi)
{
    if (tr::is_not(phi))
    {
        return tf(tr::arg(phi));
    }
    else if (tr::is_and(phi) || tr::is_or(phi) || tr::is_imp(phi))
    {
        return tf(tr::left(phi)) || tf(tr::right(phi));
    }
    else if (tr::is_forall(phi) || tr::is_exists(phi))
    {
        return tf(tr::arg(phi));
    }
    else if (tr::is_prop_var(phi))
    {
        return false;
    }
    return true;
}


std::set<std::string> lts_info::occ(const pbes_expression& expr)
{
    std::set<std::string> result;
    if (tr::is_prop_var(expr))
    {
        result.insert(tr::name(expr));
    }
    else if (tr::is_and(expr) || tr::is_or(expr) ||tr::is_imp(expr))
    {
        std::set<std::string> l = occ(tr::left(expr));
        result.insert(l.begin(), l.end());
        std::set<std::string> r = occ(tr::right(expr));
        result.insert(r.begin(), r.end());
    }
    else if (tr::is_forall(expr) || tr::is_exists(expr) || tr::is_not(expr))
    {
        result = occ(tr::arg(expr));
    }
    return result;
}


std::set<std::string> lts_info::free(const pbes_expression& expr)
{
    std::set<std::string> result;
    data::variable_list free_vars = tr::free_variables(expr);
    for (data::variable_list::iterator v = free_vars.begin(); v != free_vars.end(); ++v)
    {
        variable var = *v;
        result.insert(get_param_signature(var));
    }
    return result;
}


std::set<std::string> lts_info::used(const pbes_expression& expr)
{
    std::set<std::string> emptySet;
    return used(expr, emptySet);
}


std::set<std::string> lts_info::used(const pbes_expression& expr, const std::set<std::string>& L)
{
    //std::clog << "lts_info::used(" << bqnf_visitor<equation_type, term_type>::print_brief(expr) << ", L)" << std::endl;
    std::set<std::string> result;
    if (tr::is_data(expr))
    {
        std::set<std::string> fv = free(expr);
        result.insert(fv.begin(), fv.end());
    }
    if (tr::is_prop_var(expr))
    {
        data::variable_list var_params =
                    variable_parameters.at(tr::name(expr));
        data::data_expression_list values = tr::param(expr);
        assert(var_params.size() == values.size());
        data::data_expression_list::const_iterator val = values.begin();
        for (data::variable_list::const_iterator param =
                var_params.begin(); param != var_params.end(); ++param) {
            variable parameter = *param;
            std::string param_signature = get_param_signature(parameter);
            if (tr::is_variable(*val))
            {
                variable value = *val;
                std::string value_signature = get_param_signature(value);
                if (param_signature != value_signature || L.find(value_signature) != L.end())
                {
                    result.insert(value_signature);
                }
            }
            else
            {
                // add free variables in data expression
                std::set<std::string> l = used(*val, L);
                result.insert(l.begin(), l.end());
            }
            if (val != values.end()) {
                ++val;
            }
        }
    }
    else if (tr::is_and(expr) || tr::is_or(expr) || tr::is_imp(expr))
    {
        std::set<std::string> l = used(tr::left(expr), L);
        result.insert(l.begin(), l.end());
        std::set<std::string> r = used(tr::right(expr), L);
        result.insert(r.begin(), r.end());
    }
    else if (tr::is_not(expr))
    {
        result = used(tr::arg(expr), L);
    }
    else if (tr::is_forall(expr) || tr::is_exists(expr))
    {
        std::set<std::string> LL;
        LL.insert(L.begin(), L.end());
        data::variable_list vars = tr::var(expr);
        for (data::variable_list::const_iterator var =
                vars.begin(); var != vars.end(); ++var)
        {
            variable variable = *var;
            LL.insert(get_param_signature(variable));
        }
        result = used(tr::arg(expr), LL);
    }
    return result;
}




std::string lts_info::to_string(const ltsmin_state& state)
{
    //std::clog << "info::to_string" << std::endl;
    std::string result;
    std::stringstream ss;
    ss << (state.get_type()==parity_game_generator::PGAME_AND ? "AND" : "OR");
    ss << ":" << state.get_variable();
    ss << "(";
    atermpp::vector<data_expression>* param_values = state.get_parameter_values();
    std::vector<std::string> param_signatures =
                this->variable_parameter_signatures.at(state.get_variable());
    std::vector<std::string>::const_iterator param_signature =
            param_signatures.begin();
    for (atermpp::vector<data_expression>::const_iterator param_value =
            param_values->begin(); param_value != param_values->end(); ++param_value) {
        if (param_value != param_values->begin())
            ss << ", ";
        ss << *param_signature << " = ";
        ss << pgg->print(*param_value);
        if (param_signature != param_signatures.end())
        {
            ++param_signature;
        }
    }
    ss << ")";
    result = ss.str();
    return result;
}


std::set<std::string> lts_info::get_param_set(const data::variable_list& params)
{
    std::set<std::string> result;
    for (data::variable_list::const_iterator param =
                params.begin(); param != params.end(); ++param) {
        variable parameter = *param;
        result.insert(get_param_signature(parameter));
    }
    return result;
}


std::vector<std::string> lts_info::get_param_sequence(const data::variable_list& params)
{
    std::vector<std::string> result;
    for (data::variable_list::const_iterator param =
                params.begin(); param != params.end(); ++param) {
        variable parameter = *param;
        result.push_back(get_param_signature(parameter));
    }
    return result;
}


std::vector<int> lts_info::get_param_indices(const data::variable_list& params)
{
    std::vector<int> result;
    for (data::variable_list::const_iterator param =
                params.begin(); param != params.end(); ++param) {
        variable parameter = *param;
        int index = this->get_index(get_param_signature(parameter));
        result.push_back(index);
    }
    return result;
}


std::map<int,int> lts_info::get_param_index_positions(const data::variable_list& params)
{
    std::map<int,int> result;
    int i = 0;
    for (data::variable_list::const_iterator param =
                params.begin(); param != params.end(); ++param) {
        variable parameter = *param;
        int index = this->get_index(get_param_signature(parameter));
        result.insert(std::make_pair(index,i));
        i++;
    }
    return result;
}


std::string lts_info::get_param_signature(const variable& param)
{
    std::string paramname = param.name();
    std::string paramtype = core::pp(param.sort());
    return get_param_signature(paramname, paramtype);
}


std::string lts_info::get_param_signature(const std::string& paramname,
                                         const std::string& paramtype)
{
    return paramname + ":" + paramtype;
}




/// ltsmin_state

ltsmin_state::ltsmin_state(int priority, const std::string& varname,
                       operation_type type)
{
    this->priority = priority;
    this->var = varname;
    this->type = type;
    this->param_values = new atermpp::vector<data_expression>();
}


ltsmin_state::ltsmin_state(int priority, const propositional_variable& v,
                       operation_type type)
{
    ltsmin_state(priority, (std::string)v.name(), type);
}


ltsmin_state::ltsmin_state(int priority, const propositional_variable& v,
                       operation_type type, const pbes_expression& e)
{
    data_expression novalue;
    //std::clog << "ltsmin_state v = " << pp(v) << std::endl;
    this->priority = priority;
    this->var = (std::string)v.name();
    this->type = type;
    this->param_values = new atermpp::vector<data_expression>();
    if (tr::is_prop_var(e)) {
        assert(tr::name(e) == v.name());
        data::data_expression_list values = tr::param(e);
        for (data::data_expression_list::const_iterator val = values.begin(); val != values.end(); ++val)
        {
            if (*val == novalue)
            {
                throw(std::runtime_error("Error in ltsmin_state: state expression contains NoValue: "
                                    + e.to_string()));
            }
            this->add_parameter_value(*val);
        }
    } else {
        throw(std::runtime_error("Not a valid state expression! " + e.to_string()));
    }
}


ltsmin_state::~ltsmin_state()
{
    delete param_values;
}

bool ltsmin_state::operator<( const ltsmin_state& other ) const
{
  if (this->priority < other.priority) return true;
  else if (this->priority == other.priority)
  {
    if (this->type < other.type) return true;
    else if (this->type == other.type)
    {
      if (this->var < other.var) return true;
      else if (this->var == other.var)
      {
        if (this->param_values->size() < other.param_values->size()) return true;
        else if (this->param_values->size() == other.param_values->size())
        {
          if (*(this->param_values) < *(other.param_values)) return true;
        }
      }
    }
  }
  return false;
}


bool ltsmin_state::operator==( const ltsmin_state& other ) const
{
  return this->priority==other.priority
      && this->type==other.type
      && this->var==other.var
      && this->param_values->size()==other.param_values->size()
      && *(this->param_values)==*(other.param_values);
}


int ltsmin_state::get_priority() const
{
    return priority;
}


std::string ltsmin_state::get_variable() const
{
    return var;
}


ltsmin_state::operation_type ltsmin_state::get_type() const
{
    return type;
}


atermpp::vector<data_expression>* ltsmin_state::get_parameter_values() const
{
    return param_values;
}


void ltsmin_state::add_parameter_value(const data_expression& value)
{
    this->param_values->push_back(value);
}


pbes_expression ltsmin_state::to_pbes_expression() const
{
    //std::clog << "to_pbes_expression (this = " << this->to_string() << ")" << std::endl;
    data::data_expression_list parameter_values = data::data_expression_list();
    for (atermpp::vector<data_expression>::const_iterator param_value =
            param_values->begin(); param_value != param_values->end(); ++param_value) {
        parameter_values = parameter_values + *param_value;
    }
    // Create propositional variable instantiation.
    propositional_variable_instantiation expr =
            propositional_variable_instantiation(core::identifier_string(var), parameter_values);
    //std::clog << "to_pbes_expression expr = " << expr.to_string() << std::endl;
    return expr;
}


std::string ltsmin_state::to_string() const
{
    //std::clog << "ltsmin_state::to_string" << std::endl;
    std::string result;
    std::stringstream ss;
    ss << (type==parity_game_generator::PGAME_AND ? "AND" : "OR");
    ss << ":" << var;
    ss << "(";
    for (atermpp::vector<data_expression>::iterator entry =
            this->param_values->begin(); entry != this->param_values->end(); ++entry) {
        if (entry != this->param_values->begin())
            ss << ", ";
        ss << (*entry).to_string();
    }
    ss << ")";
    result = ss.str();
    return result;
}




/// explorer

explorer::explorer(const std::string& filename, data::rewrite_strategy rewrite_strategy = jitty_compiling, bool reset_flag = false)
{
    p.load(filename);
    pbes_system::normalize(p);
    if (!is_ppg(p))
    {
        mCRL2log(log::verbose) << "Rewriting to PPG..." << std::endl;
        p = detail::to_ppg(p);
        mCRL2log(log::verbose) << "Rewriting done." << std::endl;
    }
    this->pgg = new pbes_greybox_interface(p, false, true, rewrite_strategy);
    this->info = new lts_info(p, pgg, reset_flag);
    //std::clog << "explorer" << std::endl;
    for (int i = 0; i < info->get_lts_type().get_number_of_state_types(); i++) {
        atermpp::map<data_expression,int> data2int_map;
        this->localmaps_data2int.push_back(data2int_map);
        atermpp::vector<data_expression> int2data_map;
        this->localmaps_int2data.push_back(int2data_map);
    }
    //std::clog << "-- end of explorer." << std::endl;
}


explorer::explorer(const pbes<>& p_, data::rewrite_strategy rewrite_strategy = jitty_compiling, bool reset_flag = false)
{
    p = p_;
    this->pgg = new pbes_greybox_interface(p, false, true, rewrite_strategy);
    this->info = new lts_info(p, pgg, reset_flag);
    //std::clog << "explorer" << std::endl;
    for (int i = 0; i < info->get_lts_type().get_number_of_state_types(); i++) {
        atermpp::map<data_expression,int> data2int_map;
        this->localmaps_data2int.push_back(data2int_map);
        atermpp::vector<data_expression> int2data_map;
        this->localmaps_int2data.push_back(int2data_map);
    }
    //std::clog << "-- end of explorer." << std::endl;
}


explorer::~explorer()
{
    delete info;
    delete pgg;
}


lts_info* explorer::get_info() const
{
    return info;
}


ltsmin_state* explorer::get_initial_state() const
{
    propositional_variable_instantiation initial_state = pgg->get_initial_state();
    return this->get_state(initial_state);
}

void explorer::initial_state(int* state)
{
    ltsmin_state* initial_state = this->get_initial_state();
    this->to_state_vector(initial_state, state, 0, 0);
}

ltsmin_state* explorer::get_state(const propositional_variable_instantiation& expr) const
{
    //std::clog << "-- get_state --" << std::endl;
    //std::clog << "expr = " << pgg->print(expr) << std::endl;
    propositional_variable_instantiation novalue;
    assert(tr::is_prop_var(expr) && expr != novalue);
    std::string varname = tr::name(expr);
    int priority = info->get_variable_priorities().at(varname);
    propositional_variable var = info->get_variables().at(varname);
    operation_type type = info->get_variable_types().at(varname);
    ltsmin_state* s = new ltsmin_state(priority, var, type, expr);
    return s;
}


ltsmin_state* explorer::true_state()
{
    return new ltsmin_state(1, "true", parity_game_generator::PGAME_AND);
}


ltsmin_state* explorer::false_state()
{
    return new ltsmin_state(0, "false", parity_game_generator::PGAME_OR);
}


std::string explorer::data_to_string(const data::data_expression& e) {
    //std::clog << "data_to_string: e = " << e.to_string() << std::endl;
    return pgg->data_to_string(e);
}


data::data_expression explorer::string_to_data(const std::string& s) {
    ATerm t = atermpp::read_from_string(s);
    data::data_expression value = atermpp::aterm_appl(reinterpret_cast<ATermAppl>(t));
    return pgg->rewrite_and_simplify_expression(value);
}


int explorer::get_index(int type_no, const std::string& s)
{
    if (type_no==0)
    {
        return get_string_index(s);
    }
    else
    {
        data_expression value = this->string_to_data(s);
        return get_value_index(type_no, value);
    }
}


int explorer::get_string_index(const std::string& s)
{
    std::map<std::string,int>::iterator it = this->localmap_string2int.find(s);
    int index;
    if (it != this->localmap_string2int.end()) {
        index = it->second;
    } else {
        this->localmap_int2string.push_back(s);
        index = this->localmap_int2string.size() - 1;
        //std::clog << "[" << getpid() << "] get_string_index DEBUG push_back " << index << ": " << s << std::endl;
        this->localmap_string2int.insert(std::make_pair(s,index));
    }
    //std::clog << "get_string_index result =" << index << " (" << this->localmap_int2string->size() << ")" << std::endl;
    return index;
}


int explorer::get_value_index(int type_no, const data_expression& value)
{
    //std::clog << "get_value_index type_no=" << type_no << " (" << info->get_lts_type().get_number_of_state_types() << ")" << std::endl;
    atermpp::map<data_expression,int>& data2int_map = this->localmaps_data2int.at(type_no);
    atermpp::map<data_expression,int>::iterator it = data2int_map.find(value);
    int index;
    if (it != data2int_map.end()) {
        index = it->second;
    } else {
        this->localmaps_int2data.at(type_no).push_back(value);
        index = this->localmaps_int2data.at(type_no).size() - 1;
        data2int_map.insert(std::make_pair(value,index));
    }
    return index;
}


void explorer::to_state_vector(ltsmin_state* dst_state, int* dst, ltsmin_state* src_state, int* const& src)
{
    data_expression novalue;
    //std::clog << "-- to_state_vector -- " << std::endl;
    int state_length = info->get_lts_type().get_state_length();

    std::string varname = dst_state->get_variable();
    std::string src_varname;
    bool same_var = false;
    if (!(src==0)) {
        src_varname = src_state->get_variable();
        same_var = (varname==src_varname);
    }
    int varindex;
    if (same_var) {
        varindex = src[0];
    } else {
        varindex = this->get_string_index(varname);
    }
    dst[0] = varindex;
    //std::clog << "  to_state_vector: DEBUG: varname = " << varname << " index = " << varindex << (same_var ? " SAME VAR": "") << std::endl;


    // data_expression values[state_length]; N.B. This is not portable C++
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(values, data_expression, state_length);

    if (info->get_reset_option() || src == 0) {
        int type_no;
        for (int i = 1; i < state_length; i++) {
            data_expression default_value = info->get_default_value(i-1);
            values[i] = default_value;
            type_no = info->get_lts_type().get_state_type_no(i);
            dst[i] = this->get_value_index(type_no, values[i]);
        }
    } else {
        for (int i = 1; i < state_length; i++) {
            dst[i] = src[i];
        }
    }
    bool error = false;
    std::vector<int> parameter_indices =
                        info->get_variable_parameter_indices().at(varname);
    std::vector<std::string> parameter_signatures =
                    info->get_variable_parameter_signatures().at(varname);
    std::vector<std::string>::const_iterator param_signature = parameter_signatures.begin();
    for(std::vector<int>::const_iterator param_index = parameter_indices.begin();
            param_index != parameter_indices.end(); ++param_index)
    {
        int i = *param_index + 1;
        int type_no = info->get_lts_type().get_state_type_no(i);
        values[i] = dst_state->get_parameter_values()->at(*param_index);
        if (values[i]==novalue)
        {
            error = true;
            std::clog << "to_state_vector: ERROR varname = " << varname << ", values[" << i << "] = " << this->data_to_string(values[i]) << std::endl;
        } else {
            if (src==0) {
                // no source state available; compute index for value.
                dst[i] = this->get_value_index(type_no, values[i]);
            }
            else
            {
                // lookup src parameter value
                // FIXME: this could be computed statically: a map from src_var, dst_var and part to boolean
                std::map<int,int> src_param_index_positions = info->get_variable_parameter_index_positions().at(src_state->get_variable());
                std::map<int,int>::iterator src_param_index_position_it = src_param_index_positions.find(*param_index);
                if ( src_param_index_position_it != src_param_index_positions.end()
                        && src_state->get_parameter_values()->at(src_param_index_position_it->second) == values[i])
                {
                    // src value exists and is equal to the dst value.
                    // save to copy index from src_state
                    // which has been done earlier
                } else {
                    // parameter value has changed or does not exists in src; compute index for value.
                    dst[i] = this->get_value_index(type_no, values[i]);
                }
            }
            //std::clog << "  to_state_vector: DEBUG: " << i << " index = " << dst[i] << " value = " << this->data_to_string(values[i]) << std::endl;
        }
        if (param_signature != parameter_signatures.end())
        {
            ++param_signature;
        }
    }
    if (error)
    {
        ltsmin_state dst_state_obj = *dst_state;
        throw(std::runtime_error("Error in to_state_vector: NoValue in parameters of dst_state: "
                            + info->to_string(dst_state_obj) + "."));
    }
    //std::clog << "-- to_state_vector: done --" << std::endl;
}


std::string explorer::get_value(int type_no, int index)
{
    //std::clog << "get_value type_no = " << type_no << " index = " << index << std::endl;
    if (type_no==0)
    {
        return this->get_string_value(index);
    }
    else
    {
        data_expression value = get_data_value(type_no, index);
        std::string s = this->data_to_string(value);
        return s;
    }
}


const std::string& explorer::get_string_value(int index)
{
    if (index >= (int)(localmap_int2string.size()))
    {
        throw(std::runtime_error("Error in get_string_value: Value not found for index " + boost::lexical_cast<std::string>(index) + "."));
    }
    return localmap_int2string.at(index);
}


const data_expression& explorer::get_data_value(int type_no, int index)
{
    atermpp::vector<data_expression>& int2data_map = this->localmaps_int2data.at(type_no);
    if (index >= (int)(int2data_map.size()))
    {
        throw(std::runtime_error("Error in get_data_value: Value not found for type_no "
                                            + boost::lexical_cast<std::string>(type_no) + " at index " + boost::lexical_cast<std::string>(index) + "."));
    }
    return int2data_map.at(index);
}


ltsmin_state* explorer::from_state_vector(int* const& src)
{
    //std::clog << "-- from_state_vector(model, src) --" << std::endl;
    data_expression novalue;
    int state_length = info->get_lts_type().get_state_length();

    std::string varname = this->get_string_value(src[0]);
    //std::clog << "from_state_vector: varname = " << varname << std::endl;

    bool error = false;

    // data_expression values[state_length]; N.B. This is not portable C++
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(values, data_expression, state_length);

    int type_no;
    for (int i = 1; i < state_length; i++) {
        //std::clog << "from_state_vector: values: " << i << " (" << src[i] << "): " << std::endl;
        type_no = info->get_lts_type().get_state_type_no(i);
        values[i] = this->get_data_value(type_no, src[i]);
        //std::clog << "from_state_vector:   " << values[i].to_string() << std::endl;
    }
    //std::clog << "from_state_vector: values done." << std::endl;
    data_expression_list parameters;
    std::vector<int> parameter_indices =
            info->get_variable_parameter_indices().at(varname);
    for (std::vector<int>::iterator param_index = parameter_indices.begin(); param_index
            != parameter_indices.end(); ++param_index) {
        if (values[*param_index+1]==novalue)
        {
            error = true;
            //std::clog << "from_state_vector: varname = " << varname << ", values[" << *param_index+1 << "] = " << values[*param_index+1].to_string() << "(" << src[*param_index+1] << ")" << std::endl;
        }
        parameters = push_back(parameters, values[*param_index+1]);
    }
    if (error)
    {
        throw(std::runtime_error("Error in from_state_vector: NoValue in parameters."));
    }
    propositional_variable_instantiation state_expression(varname, parameters);
    //std::clog << "from_state_vector: state_expression = " << state_expression.to_string() << std::endl;
    ltsmin_state* state = this->get_state(state_expression);
    //std::clog << "from_state_vector: state = " << state->to_string() << std::endl;
    return state;
}


atermpp::vector<ltsmin_state*> explorer::get_successors(const ltsmin_state& state)
{
    //std::cout << "get_successors: " << state->to_string() << std::endl;
    atermpp::vector<ltsmin_state*> result;

    pbes_expression e = state.to_pbes_expression();
    assert(core::detail::check_term_PropVarInst(e));
    //pbes_expression e = state.to_pbes_expression(NULL);
    operation_type type = state.get_type();
    if (state.get_variable()=="true")
    {
        //result.push_back(true_state());
    }
    else if (state.get_variable()=="false")
    {
        //result.push_back(false_state());
    }
    else
    {
        atermpp::set<pbes_expression> successors
                = pgg->get_successors(e);
        for (atermpp::set<pbes_expression>::const_iterator expr = successors.begin(); expr
                != successors.end(); ++expr) {
            if (tr::is_prop_var(*expr)) {
                result.push_back(get_state(*expr));
            } else if (pgg->is_true(*expr)) {
                if (type != parity_game_generator::PGAME_AND)
                {
                    result.push_back(true_state());
                }
            } else if (pgg->is_false(*expr)) {
                if (type != parity_game_generator::PGAME_OR)
                {
                    result.push_back(false_state());
                }
            } else {
                throw(std::runtime_error("!! Successor is NOT a propvar: " + pgg->print(*expr)));
            }
        }
    }
    return result;
}


atermpp::vector<ltsmin_state*> explorer::get_successors(const ltsmin_state& state,
                                                     int group)
{
    //std::clog << "get_successors: " << state->to_string() << ", group=" << group << std::endl;
    atermpp::vector<ltsmin_state*> result;

    pbes_expression e = state.to_pbes_expression();
    //std::clog << "* Expression: " << core::pp(e) << std::endl;
    assert(core::detail::check_term_PropVarInst(e));
    //pbes_expression e = state.to_pbes_expression(NULL);
    operation_type type = state.get_type();
    if (state.get_variable()=="true")
    {
        //result.push_back(true_state());
    }
    else if (state.get_variable()=="false")
    {
        //result.push_back(false_state());
    }
    else
    {
        atermpp::set<pbes_expression> successors
                = pgg->get_successors(e, info->get_transition_variable_names().at(group),
                                         info->get_transition_expressions().at(group));
        for (atermpp::set<pbes_expression>::const_iterator expr = successors.begin(); expr
                != successors.end(); ++expr) {
            //std::clog << "* Successor: " << pgg->print(*expr) << std::endl;
            if (tr::is_prop_var(*expr)) {
                result.push_back(get_state(*expr));
            } else if (pgg->is_true(*expr)) {
                if (type != parity_game_generator::PGAME_AND)
                {
                    result.push_back(true_state());
                }
            } else if (pgg->is_false(*expr)) {
                if (type != parity_game_generator::PGAME_OR)
                {
                    result.push_back(false_state());
                }
            } else {
                throw(std::runtime_error("!! Successor is NOT a propvar: " + pgg->print(*expr)));
            }
        }
    }
    return result;
}


} // namespace pbes_system

} // namespace mcrl2
