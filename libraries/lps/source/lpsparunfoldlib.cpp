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
                           std::map< data::sort_expression , unfold_cache_element > *cache,
                           bool add_distribution_laws, bool alt_case_placement)
    : lps::detail::lps_algorithm<lps::stochastic_specification>(spec),
      m_run_before(false),
      m_cache(cache),
      m_representative_generator(spec.data()),
      m_add_distribution_laws(add_distribution_laws),
      m_alt_case_placement(alt_case_placement)
{
  m_identifier_generator.add_identifiers(lps::find_identifiers(spec));
  m_identifier_generator.add_identifiers(data::find_identifiers(spec.data()));
}

data::basic_sort lpsparunfold::generate_fresh_basic_sort(const std::string& str)
{
  //Generate a fresh Basic Sort
  const core::identifier_string nstr = m_identifier_generator(str);
  mCRL2log(log::verbose) << "Generated fresh sort \"" <<  std::string(nstr) << "\" for \"" <<  str << "\"" << std::endl;
  return basic_sort(nstr);
}

data::function_symbol lpsparunfold::generate_fresh_function_symbol(std::string str, const sort_expression& sort)
{
  //Generate a fresh name for a constructor of mapping

  str.resize(std::remove_if(str.begin(), str.end(), &char_filter) - str.begin());

  const core::identifier_string nstr = m_identifier_generator(str);
  mCRL2log(debug) << "Generated a fresh function symbol name: " <<  std::string(nstr) << std::endl;
  return data::function_symbol(nstr, sort);
}

data::variable lpsparunfold::generate_fresh_variable(std::string str, const sort_expression& sort)
{
  const core::identifier_string idstr = m_identifier_generator(str.append("_pp"));
  return data::variable(idstr, sort);
}

void lpsparunfold::determine_affected_constructors()
{
  m_affected_constructors = m_spec.data().constructors(m_unfold_process_parameter);

  mCRL2log(debug) << "k:\t";
  mCRL2log(log::verbose) << "" <<  unfold_parameter_name  << " has " <<  m_affected_constructors.size() << " constructor function(s)" << std::endl;

  for (const function_symbol& f: m_affected_constructors)
  {
    mCRL2log(debug) << "\t" <<  f << std::endl;
  }
}


function_symbol_vector lpsparunfold::new_constructors()
{
  using namespace data;

  function_symbol_vector elements_of_new_sorts;

  for (const function_symbol& func: m_affected_constructors)
  {

    std::string prefix = "c_";
    const data::function_symbol f = generate_fresh_function_symbol(prefix.append(func.name()) , fresh_basic_sort);
    elements_of_new_sorts.push_back(f);
    m_spec.data().add_constructor(f);
    mCRL2log(debug) << "\t" << f << std::endl;
  }
  mCRL2log(debug) << "- Created " <<  elements_of_new_sorts.size() << " fresh \" c_ \" constructor(s)" << std::endl;
  return elements_of_new_sorts;
}

data::function_symbol lpsparunfold::create_case_function(std::size_t k)
{
  std::string str = "C_";
  str.append(fresh_basic_sort.name()).append("_");

  data::sort_expression_vector fsl;
  fsl.push_back(fresh_basic_sort);
  for (std::size_t i = 0; i < k; ++i)
  {
    fsl.push_back(m_unfold_process_parameter);
  }

  data::function_symbol fs = generate_fresh_function_symbol(str, data::function_sort(fsl, m_unfold_process_parameter));

  mCRL2log(debug) << "- Created C map: " << fs << std::endl;

  return fs;
}

data::function_symbol lpsparunfold::create_determine_function()
{
  std::string str = "Det_";
  str.append(std::string(fresh_basic_sort.name()).append("_"));
  data::function_symbol fs = generate_fresh_function_symbol(str, data::make_function_sort_(m_unfold_process_parameter , fresh_basic_sort));
  mCRL2log(debug) << "\t" <<  fs << std::endl;

  return fs;
}

data::function_symbol_vector lpsparunfold::create_projection_functions()
{
  data::function_symbol_vector sfs;
  std::string str = "pi_";
  str.append(std::string(fresh_basic_sort.name()).append("_"));

  std::set<data::sort_expression> processed;
  for (const function_symbol& f: m_affected_constructors)
  {
    if (is_function_sort(f.sort()))
    {
      function_sort fs = function_sort(f.sort());
      const sort_expression_list& sel  = fs.domain();
      for (sort_expression_list::const_iterator j = sel.begin(); j != sel.end(); j++)
      {
        data::function_symbol map = generate_fresh_function_symbol(str, data::make_function_sort_(m_unfold_process_parameter , *j));
        m_spec.data().add_mapping(map);
        sfs.push_back(map);
        processed.insert(*j);
      }
    }
  }
  for (const function_symbol& f: sfs)
  {
    mCRL2log(debug) << "\t" << f << std::endl;
  }
  return sfs;
}

void lpsparunfold::add_new_equation(const data_expression& lhs, const data_expression& rhs)
{
  mCRL2log(log::verbose) << "- Added equation " <<  data::pp(data_equation(lhs, rhs)) << std::endl;
  std::set< variable > svars = find_all_variables(lhs);
  std::set< variable > tmp_var = find_all_variables(rhs);
  svars.insert(tmp_var.begin(), tmp_var.end());
  m_spec.data().add_equation(data_equation(variable_list(svars.begin(), svars.end()), lhs, rhs));
}

std::map<function_symbol, data_expression_vector> lpsparunfold::create_arguments_map()
{
  std::string dstr = "d";
  std::map<function_symbol, data_expression_vector> result;

  // The two data structures below are used to allow the reuse of variables names.
  std::map< sort_expression, variable_vector > sort_vars;  /* Mapping for Sort |-> [Variable] */
  std::map< sort_expression, std::size_t > sort_index;          /* Mapping for counting the number of unique Sorts of an equation */

  for (const function_symbol& f: m_affected_constructors)
  {
    data_expression_vector arguments;
    if (is_function_sort(f.sort()))
    {
      const function_sort& fs = atermpp::down_cast<function_sort>(f.sort());
      for (const sort_expression& sort: fs.domain())
      {
        if (sort_vars[sort].size() == sort_index[sort])
        {
          variable v(m_identifier_generator(dstr), sort);
          sort_vars[sort].push_back(v);
        }
        variable y = sort_vars[sort].at(sort_index[sort]);
        sort_index[sort] = sort_index[sort]+1;
        arguments.push_back(y);
      }
    }
    result[f]=arguments;
  }
  return result;
}

void lpsparunfold::create_data_equations(
                const function_symbol_vector& projection_functions,
                const data::function_symbol& case_function,
                function_symbol_vector elements_of_new_sorts,
                const data::function_symbol& determine_function)
{
  variable_vector vars;        /* Equation variables  */
  std::set<core::identifier_string> var_names; /* var_names */

  std::string cstr = "c";

  /* Creating variable for detector function */
  core::identifier_string istr = m_identifier_generator(cstr);
  variable v = variable(istr, fresh_basic_sort);
  vars.push_back(v);

  /* Create Equations */
  if (m_add_distribution_laws)
  {
    mCRL2log(log::verbose) << "Adding equations with additional distribution laws..." << std::endl;
  }
  else
  {
    mCRL2log(log::verbose) << "Adding equations..." << std::endl;
  }

  generate_case_functions(elements_of_new_sorts, case_function);

  const std::map<function_symbol, data_expression_vector> constructors_argument_map=create_arguments_map();

  std::size_t element_of_new_sort_index = 0;
  std::size_t projection_function_index = 0;
  for (const function_symbol& f: m_affected_constructors)
  {
    /* Creating an equation for the detector function */
    data_expression_vector function_arguments=constructors_argument_map.at(f);
    data_expression lhs = (function_arguments.empty()?application(determine_function, f):
                                       application(determine_function , application(f, function_arguments.begin(), function_arguments.end())));
    add_new_equation(lhs, elements_of_new_sorts[element_of_new_sort_index]);

    if (is_function_sort(f.sort()))
    {
      function_sort fs = function_sort(f.sort());

      /* Equations for projection functions */
      for(const data_expression& arg: function_arguments)
      {
        data_expression lhs = application(projection_functions.at(projection_function_index),
                                          application(f, function_arguments.begin(), function_arguments.end()));
        add_new_equation(lhs,arg);

        // Add default values if the projection functions are applied to constructors they are not intended for.
        // The advantage of this is that this simplifies expressions, and it allows to sometimes remove more variables
        // using constant elimination.
        for(const function_symbol& alternative_f: m_affected_constructors)
        {
          if (alternative_f!=f && f.sort().target_sort()==alternative_f.sort().target_sort())
          {
            data_expression_vector arguments_of_alternative_f = constructors_argument_map.at(alternative_f);
            data_expression lhs;
            if (arguments_of_alternative_f.empty())
            {
              lhs = application(projection_functions.at(projection_function_index),alternative_f);
            }
            else
            {
              lhs = application(projection_functions.at(projection_function_index),
                                application(alternative_f,
                                            arguments_of_alternative_f.begin(),
                                            arguments_of_alternative_f.end()));
            }
            try
            {
              data_expression rhs = m_representative_generator(lhs.sort());
              add_new_equation(lhs,rhs);
            }
            catch (runtime_error& e)
            {
              mCRL2log(debug) << "Failed to generate equation " << lhs << "= ... as no default term of sort " << lhs.sort() <<
                                 " could be generated.\n" << e.what() << "\n";
            }
          }
        }

        if (m_add_distribution_laws)
        {
          /* Add additional distribution laws for projection function pi over if

             pi(if(b,x,y))=if(b,pi(x),pi(y));
          */
          sort_expression if_arg_sort(function_sort(projection_functions.at(projection_function_index).sort()).domain().front());
          data::function_symbol if_function_symbol("if", make_function_sort_(sort_bool::bool_(), if_arg_sort, if_arg_sort , if_arg_sort));
          m_spec.data().add_equation(create_distribution_law_over_case(projection_functions.at(projection_function_index), if_function_symbol, false));
          /* Add additional distribution laws for projection function pi over case

             pi(C(projection_function_index,x1,x2,...))=C(projection_function_index,pi(x1),pi(x2),...);
          */
          const data_equation equation=create_distribution_law_over_case(projection_functions.at(projection_function_index), case_function, true);
          m_spec.data().add_equation(equation);

          /* Create additional case function */
          generate_case_functions(elements_of_new_sorts, atermpp::down_cast<function_symbol>(application(equation.rhs()).head()));
        }
        projection_function_index++;
      }
    }
    element_of_new_sort_index++;
  }

  if (m_add_distribution_laws)
  {
    /*  Add additional distribution laws for Det over if

               Det(if(b,x,y))=if(b,Det(x),Det(y));
    */
    sort_expression if_arg_sort(function_sort(determine_function.sort()).domain().front());
    data::function_symbol if_function_symbol("if", make_function_sort_(sort_bool::bool_(), if_arg_sort, if_arg_sort , if_arg_sort));
    m_spec.data().add_equation(create_distribution_law_over_case(determine_function, if_function_symbol, false));

    /*  Add additional distribution laws for Det over case

        Det(C(e,x1,x2,...))=C(e,Det(x1),Det(x2),...);
    */
    m_spec.data().add_equation(create_distribution_law_over_case(determine_function, case_function, true));
  }
}


void lpsparunfold::unfold_summands(lps::stochastic_action_summand_vector& summands, const data::function_symbol& determine_function, const data::function_symbol_vector& projection_functions)
{
  for (lps::stochastic_action_summand& summand: summands)
  {
    data::assignment_list ass = summand.assignments();
    //Create new left-hand assignment_list & right-hand assignment_list
    data::variable_vector new_ass_left;
    data::data_expression_vector new_ass_right;
    for (const data::assignment& k: ass)
    {
      if (proc_par_to_proc_par_inj.find(k.lhs()) != proc_par_to_proc_par_inj.end())
      {
        for (const data::variable& v: proc_par_to_proc_par_inj[ k . lhs() ])
        {
          new_ass_left.push_back(v);
        }

        data::data_expression_vector ins = unfold_constructor(k.rhs(), determine_function, projection_functions);
        //Replace unfold parameters in affected assignments
        new_ass_right.insert(new_ass_right.end(), ins.begin(), ins.end());
      }
      else
      {
        new_ass_left.push_back(k.lhs());
        new_ass_right.push_back(k.rhs());
      }
    }

    assert(new_ass_left.size() == new_ass_right.size());
    data::assignment_vector new_ass;
    while (!new_ass_left.empty())
    {
      new_ass.push_back(data::assignment(new_ass_left.front(), new_ass_right.front()));
      new_ass_left.erase(new_ass_left.begin());
      new_ass_right.erase(new_ass_right.begin());
    }
    summand.assignments() = data::assignment_list(new_ass.begin(), new_ass.end());
  }
}

lpsparunfold::case_func_vector lpsparunfold::parameter_case_function(const std::map<data::variable, data::variable_vector >& proc_par_to_proc_par_inj, const data::function_symbol& case_function)
{
  lpsparunfold::case_func_vector result;

  for (auto& [old_par, new_pars]: proc_par_to_proc_par_inj)
  {
    data_expression_vector dev;

    auto new_pars_it = new_pars.cbegin();
    ++new_pars_it;

    for (const data::function_symbol& constr: m_affected_constructors)
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

    result.push_back(std::make_tuple(old_par, case_function, new_pars[0], dev));
  }
  return result;
}

// TODO: Modify such that update happens in-place
lps::stochastic_linear_process lpsparunfold::update_linear_process(const function_symbol& case_function , const function_symbol& determine_function, std::size_t parameter_at_index, const function_symbol_vector& projection_functions)
{
  /* Get process parameters from lps */
  const data::variable_list& lps_proc_pars =  m_spec.process().process_parameters();

  /* Iterator pointing to the parameter that needs to be unfolded */
  data::variable_list::const_iterator unfold_parameter_it = lps_proc_pars.begin();
  std::advance(unfold_parameter_it, parameter_at_index);

  mCRL2log(log::verbose) << "Updating LPS..." << std::endl;
  /* Create new process parameters */
  data::variable_vector new_process_parameters;

  /* First copy the initial part of the parameters */
  new_process_parameters.insert(new_process_parameters.end(),
                                lps_proc_pars.begin(),
                                unfold_parameter_it);

  /* Expand unfold_parameter */
  mCRL2log(log::verbose) << "Unfolding parameter " << unfold_parameter_it->name() << " at index " << parameter_at_index << "..." << std::endl;

  /* Generate fresh process parameter for new Sort */
  data::variable_vector injected_process_parameters;

  const data::variable param = generate_fresh_variable(unfold_parameter_name, fresh_basic_sort);
  injected_process_parameters.push_back(param);

  mCRL2log(log::verbose) << "- Created process parameter " <<  data::pp(injected_process_parameters.back()) << " of type " <<  data::pp(fresh_basic_sort) << "" << std::endl;

  for (data::function_symbol_vector::iterator j = m_affected_constructors.begin()
           ; j != m_affected_constructors.end()
           ; ++j)
  {
    if (is_function_sort(j -> sort()))
    {
      const sort_expression_list dom = function_sort(j -> sort()).domain();
      for (sort_expression_list::const_iterator k = dom.begin(); k != dom.end(); ++k)
      {
        const data::variable param = generate_fresh_variable(unfold_parameter_name, *k);
        injected_process_parameters.push_back(param);
        mCRL2log(log::verbose) << "- Injecting process parameter: " <<  param << "::" <<  *k << std::endl;
      }
    }
    else if (is_basic_sort(j -> sort()) || is_structured_sort(j->sort()) || is_container_sort(j -> sort()))
    {
      mCRL2log(debug) << "- No process parameters are injected for constant: " <<  *j << std::endl;
    }
    else
    {
      throw mcrl2::runtime_error("Parameter " + pp(*j) + " has an unsupported type " + pp(j->sort()));
    }
  }

  /* store mapping: process parameter -> process parameter injection:
     Required for process parameter replacement in summands
  */
  proc_par_to_proc_par_inj[*unfold_parameter_it] = injected_process_parameters;
  new_process_parameters.insert(new_process_parameters.end(),
                                injected_process_parameters.begin(),
                                injected_process_parameters.end());


  ++unfold_parameter_it;
  /* Copy the remainder of the parameters */
  new_process_parameters.insert(new_process_parameters.end(),
                              unfold_parameter_it,
                              lps_proc_pars.end());

  mCRL2log(debug) << "- New LPS process parameters: " <<  data::pp(new_process_parameters) << std::endl;


  // TODO: avoid unnecessary copies of the LPS
  lps::stochastic_linear_process new_lps;
  new_lps.action_summands() = m_spec.process().action_summands();
  new_lps.deadlock_summands() = m_spec.process().deadlock_summands();

  // update the summands in new_lps
  unfold_summands(new_lps.action_summands(), determine_function, projection_functions);

  // Replace occurrences of unfolded parameters by the corresponding case function
  if (m_alt_case_placement)
  {
    insert_case_functions(new_lps, parameter_case_function(proc_par_to_proc_par_inj, case_function));
  }
  else
  {
    //Prepare parameter substitution
    std::map<data::variable, data::data_expression> parsub = parameter_substitution(proc_par_to_proc_par_inj, case_function);
    mutable_map_substitution< std::map< data::variable , data::data_expression > > s{parsub};
    lps::replace_variables_capture_avoiding( new_lps, s );
  }

  // NB: order is important. If we first replace the parameters, they are changed
  // again when performing the capture avoiding substitution, most likely leading
  // to an LPS that is not well-formed.
  new_lps.process_parameters() = data::variable_list(new_process_parameters.begin(), new_process_parameters.end());

  mCRL2log(debug) << "\nNew LPS:\n" <<  lps::pp(new_lps) << std::endl;

  assert(check_well_typedness(new_lps));

  return new_lps;
}

// TODO: Modify such that update happens in-place
lps::stochastic_process_initializer lpsparunfold::update_linear_process_initialization(
                   const data::function_symbol& determine_function,
                   std::size_t parameter_at_index,
                   const function_symbol_vector& projection_functions)
{
  //
  //update inital process
  //
  mCRL2log(log::verbose) << "Updating initialization...\n" << std::endl;

  const data::data_expression_list ass = m_spec.initial_process().expressions();
  //Unfold parameters
  data::data_expression_vector new_ass_right;
  size_t index=0;
  for (const data::data_expression& k: ass)
  {
    if (index == parameter_at_index)
    {
      data::data_expression_vector ins = unfold_constructor(k, determine_function, projection_functions);
      //Replace unfold parameters in affected assignments
      new_ass_right.insert(new_ass_right.end(), ins.begin(), ins.end());
    }
    else
    {
      new_ass_right.push_back(k);
    }
    index++;
  }

  const lps::stochastic_process_initializer new_init(data::data_expression_list(new_ass_right.begin(), new_ass_right.end()),
                                                            m_spec.initial_process().distribution());
  mCRL2log(debug) << "Expressions for the new initial state: " << lps::pp(new_init) << std::endl;

  return new_init;
}

std::map<data::variable, data::data_expression> lpsparunfold::parameter_substitution(std::map<data::variable, data::variable_vector > proc_par_to_proc_par_inj, const data::function_symbol& case_function)
{
  std::map<data::variable, data::data_expression> result;

  for (auto& [old_par, new_pars]: proc_par_to_proc_par_inj)
  {
    data_expression_vector dev;

    auto new_pars_it = new_pars.cbegin();
    dev.push_back(data_expression(*new_pars_it));
    ++new_pars_it;

    for (const data::function_symbol& constr: m_affected_constructors)
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

    mCRL2log(log::verbose) << "Parameter substitution:\t" << old_par << "\t->\t" <<  data::application(case_function, dev) << std::endl;
    result.insert(std::make_pair(old_par, data::application(case_function, dev)));
  }
  return result;
}

data::data_expression_vector lpsparunfold::unfold_constructor(const data_expression& de, const data::function_symbol& determine_function, function_symbol_vector projection_functions)
{
  data::data_expression_vector result;
  {
    /* Unfold parameter if function symbol occurs  */
    /* size of unfold parameter must be equal to 1 */
    data_expression_vector new_ass;

    /* Det function */
    new_ass.push_back(application(determine_function, de)) ;

    for (const function_symbol& f: projection_functions)
    {
      new_ass.push_back(application(f, de)) ;
    }

    result = new_ass;
  }
  return result;
}

data::sort_expression lpsparunfold::sort_at_process_parameter_index(std::size_t parameter_at_index)
{
  data::variable_list lps_proc_pars_list =  m_spec.process().process_parameters();
  data::variable_vector lps_proc_pars = data::variable_vector(lps_proc_pars_list.begin(), lps_proc_pars_list.end());
  mCRL2log(debug) << "- Number of parameters in LPS: " <<  lps_proc_pars.size() << "" << std::endl;
  mCRL2log(log::verbose) << "Unfolding process parameter at index: " <<  parameter_at_index << "" << std::endl;
  if (lps_proc_pars.size() <= parameter_at_index)
  {
    mCRL2log(log::error) << "Given index out of bounds. Index value needs to be in the range [0," << lps_proc_pars.size() <<")." << std::endl;
    abort();
  }

  if (is_basic_sort(lps_proc_pars[parameter_at_index].sort()))
  {
    unfold_parameter_name = basic_sort(lps_proc_pars[parameter_at_index].sort()).name();
  }

  if (is_structured_sort(lps_proc_pars[parameter_at_index].sort()))
  {
    core::identifier_string nstr;
    nstr = m_identifier_generator("S");
    unfold_parameter_name = nstr;
  }

  if (is_container_sort(lps_proc_pars[parameter_at_index].sort()))
  {
    core::identifier_string nstr;
    nstr = m_identifier_generator("S");
    unfold_parameter_name = nstr;
  }

  return lps_proc_pars[parameter_at_index].sort();
}

data::data_equation lpsparunfold::create_distribution_law_over_case(
  const data::function_symbol& function_for_distribution,
  const data::function_symbol& case_function,
  const bool add_case_function_to_data_type)
{
  assert(function_sort(case_function.sort()).codomain() == function_sort(function_for_distribution.sort()).domain().front());

  variable_vector variables_used;
  core::identifier_string istr;

  sort_expression_list case_function_sort_arguments = function_sort(case_function.sort()).domain();
  for (sort_expression_list::iterator i = case_function_sort_arguments.begin();
       i != case_function_sort_arguments.end();
       ++i)
  {
    if (i == case_function_sort_arguments.begin())
    {
      istr = m_identifier_generator("b");
    }
    else
    {
      istr = m_identifier_generator("d");
    }
    variable v(istr, *i);
    variables_used.push_back(v);
  }

  data_expression lhs(application(function_for_distribution, application(case_function, variables_used)));
  data_expression_vector rw_data_expressions;
  sort_expression_vector rw_sort_expressions;
  for (variable_vector::iterator i = variables_used.begin();
       i != variables_used.end();
       ++i)
  {
    if (i == variables_used.begin())
    {
      rw_data_expressions.push_back(*i);
      rw_sort_expressions.push_back(i->sort());
    }
    else
    {
      rw_data_expressions.push_back(application(function_for_distribution, *i));
      rw_sort_expressions.push_back(function_sort(function_for_distribution.sort()).codomain());
    }
  }

  data::function_symbol new_case_function = data::function_symbol(case_function.name(),
                         function_sort(rw_sort_expressions,function_sort(function_for_distribution.sort()).codomain()));
  if (add_case_function_to_data_type)
  {
    m_spec.data().add_mapping(new_case_function);
    generate_case_functions(m_spec.data().constructors(function_sort(new_case_function.sort()).domain().front()),new_case_function);   //add equations
  }

  /* Generate new case functions for laws */
  application rhs(new_case_function , rw_data_expressions);

  mCRL2log(log::verbose) << "- Added distribution law for \"" << data::pp(function_for_distribution) << "\" over \"" << data::pp(case_function) << "\": " << data::pp(data_equation(lhs,  rhs)) << std::endl;

  return data_equation(variables_used, lhs, rhs);
}

void lpsparunfold::generate_case_functions(function_symbol_vector elements_of_new_sorts, const data::function_symbol& case_function)
{
  mCRL2log(log::verbose) << "- Generating case function for:\t" <<  data::pp(case_function) << ": " <<  data::pp(case_function.sort()) << "" << std::endl;

  /* Generate variable identifier string for projection */
  std::string fstr = "y";
  core::identifier_string istr = m_identifier_generator(fstr);

  variable_vector vars;
  sort_expression_list dom = function_sort(case_function.sort()).domain();
  for (const sort_expression& s: dom)
  {
    istr = m_identifier_generator(fstr);
    variable v(istr, s);
    vars.push_back(v);
  }

  std::size_t e = 1;
  data_expression_vector sub_args(vars.begin(), vars.end());
  for (std::size_t i = 1 ; i < function_sort(case_function.sort()).domain().size() ; ++i)
  {
    sub_args[0] = data_expression(elements_of_new_sorts[i-1]);
    data_expression lhs = application(case_function , sub_args);
    add_new_equation(lhs,vars[e]);
    ++e;
  }

  {
    data_expression_vector eq_args;

    eq_args = data_expression_vector(function_sort(case_function.sort()).domain().size() -1 , vars.back());
    eq_args.insert(eq_args.begin(), vars.front());

    data_expression lhs = application(case_function , eq_args);
    add_new_equation(lhs, vars.back());
  }
}

void lpsparunfold::algorithm(std::size_t parameter_at_index)
{
  // Can only be run once as local data structures are not cleared
  assert(!m_run_before);
  m_run_before = true;

  m_unfold_process_parameter = sort_at_process_parameter_index(parameter_at_index);

  /* Var Dec */
  lps::stochastic_linear_process new_lps;
  lps::stochastic_process_initializer new_init;
  function_symbol_vector elements_of_new_sorts;
  function_symbol_vector projection_functions;
  data::function_symbol case_function;
  data::function_symbol determine_function;
  // data_equation_vector data_equations;

  /* Updating cache*/
  if( m_cache->find(m_unfold_process_parameter) == m_cache->end() )
  {
    /* Not using cache */

    /*   Alg */
    /*     1 */
    fresh_basic_sort = generate_fresh_basic_sort(unfold_parameter_name);
    /*     2 */
    determine_affected_constructors();
    if (m_affected_constructors.empty())
    {
      mCRL2log(log::verbose) << "The selected process parameter " <<  unfold_parameter_name << " has no constructors." << std::endl;
      mCRL2log(log::verbose) << "No need to unfold." << std::endl;
    }
    else
    {
      /*     4 */
      elements_of_new_sorts = new_constructors();
      /*     6 */
      case_function = create_case_function(m_affected_constructors.size());
      /*     7 */
      determine_function = create_determine_function();
      /*  8-12 */
      projection_functions = create_projection_functions();
      /* 13-xx */
      create_data_equations(projection_functions, case_function, elements_of_new_sorts, determine_function);

      //Reconstruct data specification, where already quite a number of mappings, constructors and functions have been added.
      m_spec.data().add_sort(fresh_basic_sort);
      m_spec.data().add_mapping(determine_function);
      m_spec.data().add_mapping(case_function);

      /*----------------*/
      m_spec.process() = update_linear_process(case_function, determine_function, parameter_at_index, projection_functions);
      m_spec.initial_process() = update_linear_process_initialization(determine_function, parameter_at_index, projection_functions);

      /* Updating cache*/
      unfold_cache_element e;
      e.cached_case_function = case_function;
      e.cached_k = m_affected_constructors;
      e.cached_determine_function = determine_function;
      e.cached_projection_functions = projection_functions;
      e.cached_fresh_basic_sort = fresh_basic_sort;

      m_cache->insert( std::pair<data::sort_expression , unfold_cache_element>( m_unfold_process_parameter , e ));
    }
  }
  else
  {
    /* Using cache */
    mCRL2log(log::verbose) << "Update using cache for sort: \"" <<  data::pp(m_unfold_process_parameter)  << "\"..." << std::endl;

    std::map< data::sort_expression , unfold_cache_element >::iterator ce = m_cache->find(m_unfold_process_parameter);

    fresh_basic_sort = ce->second.cached_fresh_basic_sort;
    m_affected_constructors = ce->second.cached_k;
    if (m_affected_constructors.empty())
    {
      mCRL2log(log::verbose) << "The selected process parameter " <<  unfold_parameter_name << " has no constructors." << std::endl;
      mCRL2log(log::verbose) << "No need to unfold." << std::endl;
    }

    case_function = ce->second.cached_case_function;
    determine_function = ce->second.cached_determine_function;
    projection_functions = ce->second.cached_projection_functions;

    m_spec.process() = update_linear_process(case_function, determine_function, parameter_at_index, projection_functions);
    m_spec.initial_process() = update_linear_process_initialization(determine_function, parameter_at_index, projection_functions);
  }

  assert(check_well_typedness(m_spec));
}
