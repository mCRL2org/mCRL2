// Author(s): Frank Stappers, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file lpsparunfoldlib.cpp
*
* \brief This file contains the code for the tool lpsparunfold that allows to replace
*        complex data types by simpler ones.
*/

#include <iterator>
#include "mcrl2/lps/lpsparunfoldlib.h"
#include "mcrl2/lps/replace.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::log;
using mcrl2::lps::lpsparunfold;

/* Remarks
- replace on vectors does not work
- vector pretty print does not work
- alias::name() [basic_sort] results in a basic sort, differs form basic_sort::name() [string]
*/

/// \brief Constructor
lpsparunfold::lpsparunfold(lps::stochastic_specification& spec,
                           std::map< data::sort_expression , unfold_cache_element >& cache,
                           bool add_distribution_laws, bool alt_case_placement,
                           bool possibly_inconsistent, bool globvars)
    : lps::detail::lps_algorithm<lps::stochastic_specification>(spec),
      m_run_before(false),
      m_data_equation_argument_generator(m_identifier_generator),
      m_cache(cache),
      m_representative_generator(spec.data()),
      m_add_distribution_laws(add_distribution_laws),
      m_alt_case_placement(alt_case_placement),
      m_possibly_inconsistent(possibly_inconsistent),
      m_globvars(globvars)
{
  m_identifier_generator.add_identifiers(lps::find_identifiers(spec));
  m_identifier_generator.add_identifiers(data::find_identifiers(spec.data()));
  for (const function_symbol& f : spec.data().constructors())
  {
    m_identifier_generator.add_identifier(f.name());
  }
  for (const function_symbol& f : spec.data().mappings())
  {
    m_identifier_generator.add_identifier(f.name());
  }
}

data::basic_sort lpsparunfold::generate_fresh_basic_sort(const data::sort_expression& sort)
{
  //Generate a fresh Basic Sort
  std::string hint("S");
  if(data::is_basic_sort(sort))
  {
    hint = atermpp::down_cast<basic_sort>(sort).name();
  }
  else if(data::is_container_sort(sort))
  {
    hint = filter_illegal_characters(data::pp(sort));
  }

  const data::basic_sort result(m_identifier_generator(hint));
  mCRL2log(log::verbose) << "Generated fresh sort \"" <<  data::pp(result) << "\" for \"" <<  data::pp(sort) << "\"" << std::endl;
  return result;
}

core::identifier_string lpsparunfold::generate_fresh_function_symbol_name(const std::string& str)
{
  //Generate a fresh name for a constructor of mapping
  const core::identifier_string result(m_identifier_generator(filter_illegal_characters(str)));
  mCRL2log(debug) << "Generated a fresh function symbol name: " <<  result << std::endl;
  return result;
}

data::variable lpsparunfold::generate_fresh_variable(std::string str, const sort_expression& sort)
{
  return data::variable(m_identifier_generator(str.append("_pp")), sort);
}

void lpsparunfold::determine_affected_constructors()
{
  assert(m_new_cache_element.affected_constructors.empty());

  m_new_cache_element.affected_constructors = m_spec.data().constructors(m_unfold_parameter.sort());

  mCRL2log(debug) << "constructors of unfolded sort:\t";
  mCRL2log(log::verbose) << "" <<  m_unfold_parameter.name()  << " has " <<  m_new_cache_element.affected_constructors.size() << " constructor function(s)" << std::endl;

  if(log::mCRL2logEnabled(debug))
  {
    for (const function_symbol& f : m_new_cache_element.affected_constructors)
    {
      mCRL2log(debug) << "\t" << f << std::endl;
    }
  }
}

void lpsparunfold::create_new_constructors()
{
  assert(m_new_cache_element.new_constructors.empty());

  for (const function_symbol& func: m_new_cache_element.affected_constructors)
  {
    std::string prefix = "c_";
    prefix.append(func.name());
    const data::function_symbol f(generate_fresh_function_symbol_name(prefix),
                                  m_new_cache_element.fresh_basic_sort);
    m_new_cache_element.new_constructors.push_back(f);
    m_spec.data().add_constructor(f);
    mCRL2log(debug) << "\t" << f << std::endl;
  }
  mCRL2log(debug) << "- Created " <<  m_new_cache_element.new_constructors.size() << " fresh \" c_ \" constructor(s)" << std::endl;
}

data::function_symbol lpsparunfold::create_case_function(const sort_expression& sort)
{
  // Generate new function symbol that is used for all case functions related
  // to unfolding m_new_cache_element.fresh_basic_sort.
  if (m_new_cache_element.case_function_name == core::identifier_string())
  {
    std::string str = "C_";
    str.append(m_new_cache_element.fresh_basic_sort.name());
    m_new_cache_element.case_function_name =
        generate_fresh_function_symbol_name(str);
  }

  // Check if the function symbol was already in the cache; if not, create and add it
  std::map<mcrl2::data::sort_expression, mcrl2::data::function_symbol>::const_iterator
      case_function_it = m_new_cache_element.case_functions.find(sort);
  if(case_function_it == m_new_cache_element.case_functions.end())
  {
    // all except first argument are the sort of the unfolded type.
    data::sort_expression_vector fsl(m_new_cache_element.affected_constructors.size() + 1, sort);
    fsl[0] = m_new_cache_element.fresh_basic_sort ;

    data::function_symbol fs(m_new_cache_element.case_function_name,
                             data::function_sort(fsl, sort));

    mCRL2log(debug) << "- Created C map: " << fs << std::endl;
    m_new_cache_element.case_functions[sort] = fs;
    m_spec.data().add_mapping(fs);

    // generate and add equations.
    generate_case_function_equations(fs);
    return fs;
  }
  else
  {
    return case_function_it->second;
  }
}

void lpsparunfold::create_determine_function()
{
  std::string str = "Det_";
  str.append(std::string(m_new_cache_element.fresh_basic_sort.name()));
  m_new_cache_element.determine_function =
      data::function_symbol(generate_fresh_function_symbol_name(str),
                           data::make_function_sort_(m_unfold_parameter.sort(),
                                     m_new_cache_element.fresh_basic_sort ));
  mCRL2log(debug) << "\t" <<  m_new_cache_element.determine_function << std::endl;
  m_spec.data().add_mapping(m_new_cache_element.determine_function);

  generate_determine_function_equations();
}

void lpsparunfold::create_projection_functions()
{
  std::string str = "pi_";
  str.append(std::string(m_new_cache_element.fresh_basic_sort.name()));

  for (const function_symbol& f: m_new_cache_element.affected_constructors)
  {
    if (is_function_sort(f.sort()))
    {
      const function_sort fs = atermpp::down_cast<function_sort>(f.sort());
      for (const sort_expression& argument_sort: fs.domain())
      {
        const data::function_symbol map(generate_fresh_function_symbol_name(str),
                                        data::make_function_sort_(m_unfold_parameter.sort(),
                                           argument_sort));
        m_spec.data().add_mapping(map);
        m_new_cache_element.projection_functions.push_back(map);
      }
    }
  }

  if(mCRL2logEnabled(debug))
  {
    for (const function_symbol& f : m_new_cache_element.projection_functions)
    {
      mCRL2log(debug) << "\t" << f << std::endl;
    }
  }

  generate_projection_function_equations();
}

void lpsparunfold::generate_projection_function_equations()
{
  // Add projection functions for the arguments of the original constructors.
  function_symbol_vector::const_iterator pi_it = m_new_cache_element.projection_functions.begin();
  for (const function_symbol& f : m_new_cache_element.affected_constructors)
  {
    const variable_vector f_arguments(m_data_equation_argument_generator.arguments(f));
    const variable_list f_arguments_list(f_arguments.begin(), f_arguments.end());

    for(const variable& arg: f_arguments)
    {
      const application lhs(*pi_it, application(f, f_arguments_list));
      m_spec.data().add_equation(data_equation(f_arguments_list, lhs, arg));

      // For the same projection function, generate right hand sides with default
      // values if the projection function is applied to an expression with a
      // constructor that it was not intended for.
      for (const function_symbol& g: m_new_cache_element.affected_constructors)
      {
        if (f != g)
        {
          const variable_vector g_arguments(
              m_data_equation_argument_generator.arguments(g));
          const variable_list g_arguments_list(g_arguments.begin(), g_arguments.end());
          application lhs;
          if (g_arguments.empty())
          {
            lhs = application(*pi_it, g);
          }
          else
          {
            lhs = application(
                *pi_it, application(g, g_arguments_list));
          }
          try
          {
            const data_expression rhs = m_representative_generator(lhs.sort());
            m_spec.data().add_equation(data_equation(g_arguments_list, lhs, rhs));
          }
          catch (runtime_error& e)
          {
            mCRL2log(debug) << "Failed to generate equation " << data::pp(lhs)
                            << "= ... as no default term of sort "
                            << data::pp(lhs.sort()) << " could be generated.\n"
                            << e.what() << "\n";
          }
        }
      }

      // If so desired, add distribution of pi over if and case functions
      // pi(if(b,x,y))=if(b,pi(x),pi(y));
      // pi(C(e,x1,x2,...))=C(e,pi(x1),pi(x2),...);
      if(m_add_distribution_laws)
      {
        create_distribution_law_over_case(*pi_it, data::if_(m_unfold_parameter.sort()));
        create_distribution_law_over_case(*pi_it, m_new_cache_element.case_functions[m_unfold_parameter.sort()]);
      }

      ++pi_it;
    }
  }
}

void lpsparunfold::unfold_summands(lps::stochastic_action_summand_vector& summands)
{
  for (lps::stochastic_action_summand& summand: summands)
  {
    data::assignment_vector new_assignments;
    for (const data::assignment& k: summand.assignments())
    {
      if (k.lhs() == m_unfold_parameter)
      {
        const data::data_expression_vector new_rhs = unfold_constructor(k.rhs());
        const data::assignment_vector injected_assignments = data::make_assignment_vector(m_injected_parameters, new_rhs);
        new_assignments.insert(new_assignments.end(), injected_assignments.begin(), injected_assignments.end());
      }
      else
      {
        new_assignments.push_back(k);
      }
    }
    summand.assignments() = data::assignment_list(new_assignments.begin(), new_assignments.end());
  }
}

lpsparunfold::case_func_replacement lpsparunfold::parameter_case_function()
{
  lpsparunfold::case_func_replacement result;
  data_expression_vector dev;

  auto new_pars_it = m_injected_parameters.cbegin();
  ++new_pars_it;

  for (const data::function_symbol& constr: m_new_cache_element.affected_constructors )
  {
    data::data_expression case_func_arg = constr;

    if (is_function_sort(constr.sort()))
    {
      sort_expression_list dom = function_sort(constr.sort()).domain();
      data_expression_vector arg;

      for (const data::sort_expression& arg_sort: dom)
      {
        if (new_pars_it->sort() != arg_sort)
        {
          throw runtime_error("Unexpected new parameter encountered, maybe they were not sorted well.");
        }
        arg.push_back(*new_pars_it++);
      }
      case_func_arg = data::application(constr, arg);
    }

    dev.push_back(case_func_arg);
  }

  return std::make_tuple(m_unfold_parameter, m_new_cache_element.case_functions, m_injected_parameters[0], dev);

}

void lpsparunfold::update_linear_process(std::size_t parameter_at_index)
{
  /* Get process parameters from lps */
  const data::variable_list& process_parameters =  m_spec.process().process_parameters();

  /* Iterator pointing to the parameter that needs to be unfolded */
  data::variable_list::const_iterator unfold_parameter_it =
      process_parameters.begin();
  std::advance(unfold_parameter_it, parameter_at_index);

  mCRL2log(log::verbose) << "Updating LPS..." << std::endl;

  /* Create new process parameters */
  data::variable_vector new_process_parameters;

  /* Expand unfold_parameter */
  mCRL2log(log::verbose) << "  Unfolding parameter " << unfold_parameter_it->name() << " at index " << parameter_at_index << "..." << std::endl;

  /* First copy the initial part of the parameters */
  new_process_parameters.insert(new_process_parameters.end(),
                                process_parameters.begin(),
                                unfold_parameter_it);

  const data::variable param = generate_fresh_variable(m_unfold_parameter.name(), m_new_cache_element.fresh_basic_sort );
  m_injected_parameters.push_back(param);

  mCRL2log(log::verbose) << "- Created process parameter " <<  data::pp(m_injected_parameters.back()) << " of type " <<  data::pp(m_new_cache_element.fresh_basic_sort ) << "" << std::endl;

  for (const data::function_symbol& constructor: m_new_cache_element.affected_constructors)
  {
    if (is_function_sort(constructor.sort()))
    {
      const sort_expression_list domain = function_sort(constructor.sort()).domain();
      for (const sort_expression& s: domain)
      {
        const data::variable param = generate_fresh_variable(m_unfold_parameter.name(), s);
        m_injected_parameters.push_back(param);
        mCRL2log(log::verbose) << "- Injecting process parameter: " <<  param
                               << "::" <<  pp(s) << std::endl;
      }
    }
    else if (is_basic_sort(constructor.sort())
             || is_structured_sort(constructor.sort())
             || is_container_sort(constructor.sort()))
    {
      mCRL2log(debug) << "- No process parameters are injected for constant: "
                      <<  constructor << std::endl;
    }
    else
    {
      throw mcrl2::runtime_error("Parameter " + pp(constructor) + " has an unsupported type " + pp(constructor.sort()));
    }
  }

  new_process_parameters.insert(new_process_parameters.end(),
                                m_injected_parameters.begin(),
                                m_injected_parameters.end());


  ++unfold_parameter_it;
  /* Copy the remainder of the parameters */
  new_process_parameters.insert(new_process_parameters.end(),
                              unfold_parameter_it, process_parameters.end());

  mCRL2log(debug) << "- New LPS process parameters: " <<  data::pp(new_process_parameters) << std::endl;

  // update the summands in new_lps
  unfold_summands(m_spec.process().action_summands());

  // Replace occurrences of unfolded parameters by the corresponding case function
  // Clear process parameters first, to ensure capture avoiding substitution
  // ignores the process parameters.
  m_spec.process().process_parameters() = data::variable_list();
  if (m_alt_case_placement)
  {
    create_case_function(data::sort_bool::bool_()); // conditions
    create_case_function(data::sort_real::real_()); // time/distributions

    // process parameters
    for (const data::variable& param : new_process_parameters)
    {
      create_case_function(param.sort());
    }

    // parameters of actions
    for (const process::action_label& action_label : m_spec.action_labels())
    {
      for (const sort_expression& s : action_label.sorts())
      {
        create_case_function(s);
      }
    }

    mCRL2log(log::verbose) << "- Inserting case functions into the process using alternative case placement";
    // place the case functions
    insert_case_functions(m_spec.process(), parameter_case_function(), m_identifier_generator);
  }
  else
  {
    mCRL2log(log::verbose) << "- Inserting case functions into the process using default case placement" << std::endl;
    //Prepare parameter substitution
    const mutable_map_substitution< std::map< data::variable , data::data_expression > > s{parameter_substitution()};
    lps::replace_variables_capture_avoiding(m_spec.process(), s);
  }

  // NB: order is important. If we first replace the parameters, they are changed
  // again when performing the capture avoiding substitution, most likely leading
  // to an LPS that is not well-formed.
  m_spec.process().process_parameters() = data::variable_list(new_process_parameters.begin(), new_process_parameters.end());

  mCRL2log(debug) << "\nNew LPS:\n" <<  lps::pp(m_spec.process()) << std::endl;

  assert(check_well_typedness(m_spec.process()));
}

void lpsparunfold::update_linear_process_initialization(
                   std::size_t parameter_at_index)
{
  //
  //update inital process
  //
  mCRL2log(log::verbose) << "Updating initialization...\n" << std::endl;

  //Unfold parameters
  data::data_expression_vector new_init;
  size_t index=0;
  for (const data::data_expression& k: m_spec.initial_process().expressions())
  {
    if (index == parameter_at_index)
    {
      const data::data_expression_vector ins = unfold_constructor(k);
      //Replace unfold parameters in affected assignments
      new_init.insert(new_init.end(), ins.begin(), ins.end());
    }
    else
    {
      new_init.push_back(k);
    }
    ++index;
  }

  m_spec.initial_process() = stochastic_process_initializer(
      data_expression_list(new_init.begin(), new_init.end()),
      m_spec.initial_process().distribution());

  mCRL2log(debug) << "Expressions for the new initial state: " << data::pp(m_spec.initial_process().expressions()) << std::endl;
}

std::map<data::variable, data::data_expression> lpsparunfold::parameter_substitution()
{
  std::map<data::variable, data::data_expression> result;

  data_expression_vector dev;

  auto new_pars_it = m_injected_parameters.cbegin();
  dev.push_back(data_expression(*new_pars_it));
  ++new_pars_it;

  for (const data::function_symbol& constr: m_new_cache_element.affected_constructors)
  {
    data::data_expression case_func_arg = constr;

    if (is_function_sort(constr.sort()))
    {
      sort_expression_list dom = function_sort(constr.sort()).domain();
      data_expression_vector arg;

      for (const data::sort_expression& arg_sort: dom)
      {
        if (new_pars_it->sort() != arg_sort)
        {
          throw runtime_error("Unexpected new parameter encountered, maybe they were not sorted well.");
        }
        arg.push_back(*new_pars_it++);
      }
      case_func_arg = data::application(constr, arg);
    }

    dev.push_back(case_func_arg);
  }
  mCRL2log(log::verbose) << "Parameter substitution:\t" << m_unfold_parameter << "\t->\t" <<  data::application(m_new_cache_element.case_functions[m_unfold_parameter.sort()], dev) << std::endl;
  result.insert(std::make_pair(m_unfold_parameter, data::application(m_new_cache_element.case_functions[m_unfold_parameter.sort()], dev)));
  return result;
}

data::data_expression lpsparunfold::apply_function(const function_symbol& f, const data_expression& de) const
{
  if(m_alt_case_placement && is_if_application(de))
  {
    return data::if_(arg1(de),
                     apply_function(f, arg2(de)),
                     apply_function(f, arg3(de)));
  }
  else
  {
    return application(f, de);
  }
}

data::data_expression_vector lpsparunfold::unfold_constructor(const data_expression& de)
{
  assert(de.sort() == m_unfold_parameter.sort());
  data::data_expression_vector result;

  // Replace global variables with fresh global variables, if the corresponding option is set.
  if(m_globvars &&
      data::is_variable(de) &&
      m_spec.global_variables().find(atermpp::down_cast<variable>(de)) != m_spec.global_variables().end())
  {
    // don't care for det position
    variable v = generate_fresh_variable("dc", m_new_cache_element.determine_function.sort().target_sort());
    result.push_back(v);
    m_spec.global_variables().insert(v);

    // don't cares for each of the arguments
    for (const function_symbol& f: m_new_cache_element.projection_functions)
    {
      v = generate_fresh_variable("dc", f.sort().target_sort());
      result.push_back(v);
      m_spec.global_variables().insert(v);
    }
  }
  else
  {
    /* Det function */
    result.emplace_back(apply_function(m_new_cache_element.determine_function, de));

    for (const function_symbol& f: m_new_cache_element.projection_functions)
    {
      result.emplace_back(apply_function(f, de)) ;
    }
  }

  return result;
}

data::variable lpsparunfold::process_parameter_at(const std::size_t index)
{
  mCRL2log(debug) << "- Number of parameters in LPS: " <<   m_spec.process().process_parameters().size() << "" << std::endl;
  mCRL2log(log::verbose) << "Unfolding process parameter at index: " <<  index << "" << std::endl;
  if ( m_spec.process().process_parameters().size() <= index)
  {
    throw mcrl2::runtime_error("Given index out of bounds. Index value needs to be in the range [0," + std::to_string(m_spec.process().process_parameters().size()) + ").");
  }

  data::variable_list::const_iterator parameter_it = m_spec.process().process_parameters().begin();
  std::advance(parameter_it, index);
  return *parameter_it;
}

// Add equation for f(C(e, d_1, ..., d_n)) = C(e, f(d_1), ..., f(d_n))
// note: the case_function parameter must be copied; passing by reference
// may lead to crashes if the argument is an element of
// m_new_cache_element.case_functions if a new case function is introduced.
void lpsparunfold::create_distribution_law_over_case(
  const data::function_symbol& f,
  const data::function_symbol case_function)
{
  assert(case_function.sort().target_sort() == atermpp::down_cast<function_sort>(f.sort()).domain().front());

  // C(e, d_1, ..., d_n)
  variable_vector lhs_args = m_data_equation_argument_generator.arguments(case_function);
  application lhs(f, application(case_function, lhs_args));

  // Construct rhs arguments e, f(d_1), ..., f(d_n)
  data_expression_vector rhs_args;
  variable_vector::const_iterator args = lhs_args.begin();
  for (variable_vector::const_iterator i = lhs_args.begin(); i != lhs_args.end(); ++i)
  {
    if(i == lhs_args.begin())
    {
      rhs_args.push_back(*args);
    }
    else
    {
      rhs_args.emplace_back(application(f, *i));
    }
  }

  // Determine the new case function or if function symbol.
  data::function_symbol new_case_function;
  if(data::is_if_function_symbol(case_function))
  {
    new_case_function = data::if_(f.sort().target_sort());
  }
  else
  {
    new_case_function = create_case_function(f.sort().target_sort());
  }

  data::application rhs(new_case_function , rhs_args);
  data::data_equation result(lhs_args, lhs, rhs);
  m_spec.data().add_equation(result);

  mCRL2log(log::verbose) << "- Added distribution law for \"" << data::pp(f) << "\" over \"" << data::pp(case_function) << "\": " << data::pp(result) << std::endl;
}

void lpsparunfold::generate_case_function_equations(const data::function_symbol& case_function)
{
  mCRL2log(log::verbose) << "- Generating case function equations for:\t" <<  data::pp(case_function) << ": " <<  data::pp(case_function.sort()) << "" << std::endl;

  assert(atermpp::down_cast<function_sort>(case_function.sort()).domain().size() == m_new_cache_element.new_constructors.size() + 1);
  /* Generate variable identifier string for projection */
  const variable_vector vars = m_data_equation_argument_generator.arguments(case_function);
  const variable_list used_vars(++vars.begin(), vars.end()); // all but the first parameter are used in the data equation.

  // We generate one equation for each of the constructors of the new sort,
  // projecting out the corresponding argument.
  // C(c_i, d_1,...,d_(i-1), d_i, d_(i+1), ..., d_n) = d_i;
  data_expression_vector sub_args(vars.begin(), vars.end());
  // vars_it represents d_i above.
  variable_vector::const_iterator vars_it = vars.begin();
  ++vars_it; // first variable to be skipped.

  for(data::function_symbol new_constructor: m_new_cache_element.new_constructors)
  {
    sub_args[0] = new_constructor; // set c_i
    const application lhs(case_function , sub_args);
    m_spec.data().add_equation(data_equation(used_vars, lhs, *vars_it));
    ++vars_it;
  }

  // Finally add an equation that removes a case function if all (except the first)
  // argument are the same.
  // C(x, d_n, ...., d_n) = d_n
  data_expression_vector eq_args(m_new_cache_element.new_constructors.size() + 1, vars.back());
  eq_args[0] = vars.front();
  const application lhs(case_function , eq_args);
  m_spec.data().add_equation(data_equation(data::variable_list({vars.front(), vars.back()}), lhs, vars.back()));

  // If the case function maps to the Booleans, we can replace it by a disjunction.
  // Note: this may make the data specification inconsistent.
  if(m_possibly_inconsistent && sort_bool::is_bool(case_function.sort().target_sort()))
  {
    // C(x, d_1, ..., d_n) = (d_1 && x == c_1) || (c_2 && x == c_2) || .... (d_n && x == c_n)
    const data::variable_list args(vars.begin(), vars.end());
    const application lhs(case_function, args);

    data_expression_vector disjuncts;
    vars_it = vars.begin();
    const variable det_var = *vars_it++;
    for(const function_symbol& constructor: m_new_cache_element.new_constructors)
    {
      if(vars_it == vars.end())
      {
        throw mcrl2::runtime_error("The number of variables and the number of constructors differs.");
      }
      disjuncts.push_back(
          sort_bool::and_(*vars_it++, equal_to(det_var, constructor)));
    }

    m_spec.data().add_equation(data_equation(args, lhs, lazy::join_or(disjuncts.begin(), disjuncts.end())));

    // We also need to add rules for equality on the new sort.
    for(const function_symbol& left: m_new_cache_element.new_constructors)
    {
      for(const function_symbol& right: m_new_cache_element.new_constructors)
      {
        if (left != right)
        {
          const application lhs = data::equal_to(left, right);
          const data_expression rhs = data::false_();
          m_spec.data().add_equation(data_equation(lhs, rhs));
        }
      }
    }
  }
}

void lpsparunfold::generate_determine_function_equations()
{
  function_symbol_vector::const_iterator constructor_it = m_new_cache_element.new_constructors.begin();
  for (const function_symbol& f: m_new_cache_element.affected_constructors)
  {
    assert(constructor_it != m_new_cache_element.new_constructors.end());
    /* Creating an equation for the detector function */
    const variable_vector function_arguments = m_data_equation_argument_generator.arguments(f);
    if(function_arguments.empty())
    {
      m_spec.data().add_equation(data_equation(
          application(m_new_cache_element.determine_function, f),
                       *constructor_it));
    }
    else
    {
      const variable_list args(function_arguments.begin(), function_arguments.end());
      m_spec.data().add_equation(data_equation(
          args,
          application(m_new_cache_element.determine_function,
                      application(f,args)),
          *constructor_it));
    }
    ++constructor_it;
  }

  if (m_add_distribution_laws)
  {
    /*  Add additional distribution laws for Det over if and case functions
    Det(if(b,x,y))=if(b,Det(x),Det(y));
    Det(C(e,x1,x2,...))=C(e,Det(x1),Det(x2),...);
    */
    create_distribution_law_over_case(m_new_cache_element.determine_function, data::if_(m_unfold_parameter.sort()));
    create_distribution_law_over_case(m_new_cache_element.determine_function, m_new_cache_element.case_functions[m_unfold_parameter.sort()]);
  }
}

void lpsparunfold::algorithm(const std::size_t parameter_at_index)
{
  // Can only be run once as local data structures are not cleared
  assert(!m_run_before);
  m_run_before = true;

  m_unfold_parameter = process_parameter_at(parameter_at_index);

  /* Updating cache*/
  std::map< data::sort_expression, unfold_cache_element >::iterator ce = m_cache.find(m_unfold_parameter.sort());
  if(ce == m_cache.end())
  {
    /* Not using cache */

    /*   Alg */
    /*     1 */
    m_new_cache_element.fresh_basic_sort  = generate_fresh_basic_sort(m_unfold_parameter.sort());
    m_spec.data().add_sort(m_new_cache_element.fresh_basic_sort );

    /*     2 */
    determine_affected_constructors();

    // If there are no constructors, there is nothing to be done.
    if (!m_new_cache_element.affected_constructors.empty())
    {
      /*     4 */
      create_new_constructors();
      /*     6 */
      create_case_function(m_unfold_parameter.sort());
      /*     7 */
      create_determine_function();
      /*  8-12 */
      create_projection_functions();

      /* Updating cache*/
      m_cache[m_unfold_parameter.sort()] = m_new_cache_element;
    }
  }
  else
  {
    /* Using cache */
    mCRL2log(log::verbose) << "Update using cache for sort: \"" <<  data::pp(m_unfold_parameter.sort())  << "\"..." << std::endl;
    m_new_cache_element = ce->second;
  }

  // Perform the actual unfolding (if needed)
  if (m_new_cache_element.affected_constructors.empty())
  {
    mCRL2log(log::verbose) << "The selected process parameter " <<  m_unfold_parameter.name() << " has no constructors." << std::endl;
    mCRL2log(log::verbose) << "No need to unfold." << std::endl;
  }
  else
  {
    update_linear_process(parameter_at_index);
    update_linear_process_initialization(parameter_at_index);
  }

  /* Updating cache*/
  m_cache[m_unfold_parameter.sort()] = m_new_cache_element;

  assert(check_well_typedness(m_spec));
}
