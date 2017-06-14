// Author(s): Frank Stappers, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "mcrl2/utilities/logger.h"

#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

#include "mcrl2/lps/lpsparunfoldlib.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/replace.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::log;

/* Remarks
- replace on vectors does not work
- vector pretty print does not work
- alias::name() [basic_sort] results in a basic sort, differs form basic_sort::name() [string]
*/

lpsparunfold::lpsparunfold(mcrl2::lps::stochastic_specification spec,
    std::map< mcrl2::data::sort_expression , lspparunfold::unfold_cache_element > *cache,
    bool add_distribution_laws
)
  : 
    m_cache(cache),
    m_data_specification(spec.data()),
    m_representative_generator(m_data_specification),
    m_lps(spec.process()),
    m_glob_vars(spec.global_variables()),
    m_init_process(spec.initial_process()),
    m_action_label_list(spec.action_labels()),
    m_add_distribution_laws(add_distribution_laws)
{
  mCRL2log(debug) << "Processing" << std::endl;

  m_identifier_generator.add_identifiers(mcrl2::lps::find_identifiers(spec));

  for (const sort_expression& s:  m_data_specification.sorts())
  {
    if (is_basic_sort(s))
    {
      sort_names.insert((basic_sort(s)).name());
    }
  };

  m_identifier_generator.add_identifiers(sort_names);

  {
    std::size_t size = mapping_and_constructor_names.size();
    for (const function_symbol& f: m_data_specification.constructors()) 
    {
      mapping_and_constructor_names.insert(f.name());
    };
    mCRL2log(debug) << "- Specification has " <<   mapping_and_constructor_names.size() - size << " constructors" << std::endl;
  }

  {
    std::size_t size = mapping_and_constructor_names.size();
    for (const function_symbol& f: m_data_specification.mappings())
    {
      mapping_and_constructor_names.insert(f.name());
    };
    mCRL2log(debug) << "- Specification has " <<  mapping_and_constructor_names.size() - size << " mappings " << std::endl;
  }

  m_identifier_generator.add_identifiers(mapping_and_constructor_names);
}

mcrl2::data::basic_sort lpsparunfold::generate_fresh_basic_sort(const std::string& str)
{
  //Generate a fresh Basic Sort
  mcrl2::core::identifier_string nstr = m_identifier_generator(str);
  mCRL2log(verbose) << "Generated fresh sort \"" <<  string(nstr) << "\" for \"" <<  str << "\"" << std::endl;
  sort_names.insert(nstr);
  return basic_sort(std::string(nstr));
}

mcrl2::core::identifier_string lpsparunfold::generate_fresh_constructor_and_mapping_name(std::string str)
{
  //Generate a fresh name for a constructor of mapping

  str.resize(std::remove_if(str.begin(), str.end(), &char_filter) - str.begin());

  mcrl2::core::identifier_string nstr = m_identifier_generator(str);
  mCRL2log(debug) << "Generated a fresh mapping: " <<  string(nstr) << std::endl;
  mapping_and_constructor_names.insert(nstr);
  return nstr;
}

function_symbol_vector lpsparunfold::determine_affected_constructors()
{
  function_symbol_vector t = m_data_specification.constructors(m_unfold_process_parameter);
  function_symbol_vector k = function_symbol_vector(t.begin(), t.end());

  mCRL2log(debug) << "k:\t";
  mCRL2log(verbose) << "" <<  unfold_parameter_name  << " has " <<  k.size() << " constructor function(s)" << std::endl;

  for (const function_symbol& f: k)
  {
    mCRL2log(debug) << "\t" <<  f << std::endl;
  }
  return k;
}


function_symbol_vector lpsparunfold::new_constructors(mcrl2::data::function_symbol_vector affected_constructors)
{
  using namespace mcrl2::data;

  function_symbol_vector elements_of_new_sorts;

  for (const function_symbol& func: affected_constructors)
  {

    std::string prefix = "c_";
    mcrl2::core::identifier_string fresh_name = generate_fresh_constructor_and_mapping_name(prefix.append(func.name()));
    const data::function_symbol f(fresh_name , fresh_basic_sort);
    elements_of_new_sorts.push_back(f);
    m_data_specification.add_constructor(f);
    mCRL2log(debug) << "\t" << data::function_symbol(fresh_name , fresh_basic_sort) << std::endl;
    mapping_and_constructor_names.insert(fresh_name);

  }
  mCRL2log(debug) << "- Created " <<  elements_of_new_sorts.size() << " fresh \" c_ \" constructor(s)" << std::endl;
  return elements_of_new_sorts;
}

mcrl2::data::function_symbol lpsparunfold::create_case_function(std::size_t k)
{
  mcrl2::data::function_symbol fs;
  std::string str = "C_";
  str.append(fresh_basic_sort.name()).append("_");
  mcrl2::core::identifier_string idstr = generate_fresh_constructor_and_mapping_name(str);
  mcrl2::data::sort_expression_vector fsl;
  fsl.push_back(fresh_basic_sort);
  for (std::size_t i = 0; i < k; ++i)
  {
    fsl.push_back(m_unfold_process_parameter);
  }

  fs = data::function_symbol(idstr , mcrl2::data::function_sort(fsl, m_unfold_process_parameter));

  mCRL2log(debug) << "- Created C map: " << fs << std::endl;

  return fs;
}

mcrl2::data::function_symbol lpsparunfold::create_determine_function()
{
  mcrl2::data::function_symbol fs;
  std::string str = "Det_";
  str.append(string(fresh_basic_sort.name()).append("_"));
  mcrl2::core::identifier_string idstr = generate_fresh_constructor_and_mapping_name(str);
  mcrl2::data::sort_expression_list fsl;
  fs = data::function_symbol(idstr , mcrl2::data::make_function_sort(m_unfold_process_parameter , fresh_basic_sort));
  mCRL2log(debug) << "\t" <<  fs << std::endl;

  return fs;
}

mcrl2::data::function_symbol_vector lpsparunfold::create_projection_functions(function_symbol_vector affected_constructors)
{
  mcrl2::data::function_symbol_vector sfs;
  std::string str = "pi_";
  str.append(string(fresh_basic_sort.name()).append("_"));

  std::set<mcrl2::data::sort_expression> processed;
  for (const function_symbol& f: affected_constructors)
  {
    if (is_function_sort(f.sort()))
    {
      function_sort fs = function_sort(f.sort());
      const sort_expression_list& sel  = fs.domain();
      for (sort_expression_list::const_iterator j = sel.begin(); j != sel.end(); j++)
      {
        mcrl2::core::identifier_string idstr = generate_fresh_constructor_and_mapping_name(str);
        data::function_symbol map(idstr , mcrl2::data::make_function_sort(m_unfold_process_parameter , *j));
        m_data_specification.add_mapping(map);
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
  mCRL2log(verbose) << "- Added equation " <<  data::pp(data_equation(lhs, rhs)) << std::endl;
  set< variable > svars = find_all_variables(lhs);
  set< variable > tmp_var = find_all_variables(rhs);
  svars.insert(tmp_var.begin(), tmp_var.end());
  m_data_specification.add_equation(data_equation(variable_list(svars.begin(), svars.end()), lhs, rhs));
}

std::map<function_symbol, data_expression_vector> lpsparunfold::create_arguments_map(const function_symbol_vector& affected_constructors)
{
  std::string dstr = "d";
  std::map<function_symbol, data_expression_vector> result;

  // The two data structures below are used to allow the reuse of variables names.
  std::map< sort_expression, variable_vector > sort_vars;  /* Mapping for Sort |-> [Variable] */
  std::map< sort_expression, std::size_t > sort_index;          /* Mapping for counting the number of unique Sorts of an equation */

  for (const function_symbol& f: affected_constructors)
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
                const function_symbol_vector& affected_constructors,
                const data::function_symbol& determine_function)
{
  assert(projection_functions.size()==affected_constructors.size());
  variable_vector vars;        /* Equation variables  */
  std::set<mcrl2::core::identifier_string> var_names; /* var_names */

  std::string cstr = "c";

  /* Creating variable for detector function */
  mcrl2::core::identifier_string istr = m_identifier_generator(cstr);
  variable v = variable(istr, fresh_basic_sort);
  vars.push_back(v);

  /* Create Equations */
  if (m_add_distribution_laws)
  {
    mCRL2log(verbose) << "Adding equations with additional distribution laws..." << std::endl;
  }
  else
  {
    mCRL2log(verbose) << "Adding equations..." << std::endl;
  }

  generate_case_functions(elements_of_new_sorts, case_function);

  const std::map<function_symbol, data_expression_vector> constructors_argument_map=create_arguments_map(affected_constructors);

  std::size_t element_of_new_sort_index = 0;
  std::size_t projection_function_index = 0;
  for (const function_symbol& f: affected_constructors)
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
        for(const function_symbol& alternative_f: affected_constructors)
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
            catch (mcrl2::runtime_error& e)
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
          data::function_symbol if_function_symbol("if", make_function_sort(sort_bool::bool_(), if_arg_sort, if_arg_sort , if_arg_sort));
          m_data_specification.add_equation(create_distribution_law_over_case(projection_functions.at(projection_function_index), if_function_symbol, false));
          /* Add additional distribution laws for projection function pi over case

             pi(C(projection_function_index,x1,x2,...))=C(projection_function_index,pi(x1),pi(x2),...);
          */
          const data_equation equation=create_distribution_law_over_case(projection_functions.at(projection_function_index), case_function, true);
          m_data_specification.add_equation(equation);

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
    data::function_symbol if_function_symbol("if", make_function_sort(sort_bool::bool_(), if_arg_sort, if_arg_sort , if_arg_sort));
    m_data_specification.add_equation(create_distribution_law_over_case(determine_function, if_function_symbol, false));

    /*  Add additional distribution laws for Det over case

        Det(C(e,x1,x2,...))=C(e,Det(x1),Det(x2),...);
    */
    m_data_specification.add_equation(create_distribution_law_over_case(determine_function, case_function, true));
  }
}

mcrl2::core::identifier_string lpsparunfold::generate_fresh_process_parameter_name(std::string str, std::set<mcrl2::core::identifier_string>& process_parameter_names)
{
  mcrl2::core::identifier_string idstr = m_identifier_generator(str.append("_pp"));
  process_parameter_names.insert(idstr);
  return idstr;
}

void lpsparunfold::unfold_summands(mcrl2::lps::stochastic_action_summand_vector& summands, const mcrl2::data::function_symbol& determine_function, const mcrl2::data::function_symbol_vector& projection_functions)
{
  for (mcrl2::lps::stochastic_action_summand& summand: summands)
  {
    mcrl2::data::assignment_list ass = summand.assignments();
    //Create new left-hand assignment_list & right-hand assignment_list
    mcrl2::data::variable_vector new_ass_left;
    mcrl2::data::data_expression_vector new_ass_right;
    for (const mcrl2::data::assignment& k: ass)
    {
      if (proc_par_to_proc_par_inj.find(k.lhs()) != proc_par_to_proc_par_inj.end())
      {
        for (const mcrl2::data::variable& v: proc_par_to_proc_par_inj[ k . lhs() ])
        {
          new_ass_left.push_back(v);
        }

        mcrl2::data::data_expression_vector ins = unfold_constructor(k.rhs(), determine_function, projection_functions);
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
    mcrl2::data::assignment_vector new_ass;
    while (!new_ass_left.empty())
    {
      new_ass.push_back(mcrl2::data::assignment(new_ass_left.front(), new_ass_right.front()));
      new_ass_left.erase(new_ass_left.begin());
      new_ass_right.erase(new_ass_right.begin());
    }
    summand.assignments() = mcrl2::data::assignment_list(new_ass.begin(), new_ass.end());
  }
}

mcrl2::lps::stochastic_linear_process lpsparunfold::update_linear_process(const function_symbol& case_function , function_symbol_vector affected_constructors, const function_symbol& determine_function, std::size_t parameter_at_index, const function_symbol_vector& projection_functions)
{
  /* Get process parameters from lps */
  mcrl2::data::variable_list lps_proc_pars =  m_lps.process_parameters();

  /* Get process_parameters names from lps */
  std::set<mcrl2::core::identifier_string> process_parameter_names;
  for (const mcrl2::data::variable& v: lps_proc_pars)
  {
    process_parameter_names.insert(v.name());
  }

  mCRL2log(verbose) << "Updating LPS..." << std::endl;
  /* Create new process parameters */
  mcrl2::data::variable_vector new_process_parameters;
  for (mcrl2::data::variable_list::iterator i = lps_proc_pars.begin();
       i != lps_proc_pars.end();
       ++i)
  {
    if (static_cast<std::size_t>(std::distance(lps_proc_pars.begin(), i)) == parameter_at_index)
    {
      mCRL2log(verbose) << "Unfolding parameter " << i->name() << " at index " << std::distance(lps_proc_pars.begin(), i) << "..." << std::endl;
      mcrl2::data::variable_vector process_parameters_injection;

      /* Generate fresh process parameter for new Sort */
      mcrl2::core::identifier_string idstr = generate_fresh_process_parameter_name(unfold_parameter_name, process_parameter_names);
      process_parameters_injection.push_back(mcrl2::data::variable(idstr , fresh_basic_sort));

      mCRL2log(verbose) << "- Created process parameter " <<  data::pp(process_parameters_injection.back()) << " of type " <<  data::pp(fresh_basic_sort) << "" << std::endl;

      for (mcrl2::data::function_symbol_vector::iterator j = affected_constructors.begin()
           ; j != affected_constructors.end()
           ; ++j)
      {
        bool processed = false;
        if (is_function_sort(j -> sort()))
        {
          sort_expression_list dom = function_sort(j -> sort()).domain();
          for (sort_expression_list::iterator k = dom.begin(); k != dom.end(); ++k)
          {
            mcrl2::core::identifier_string idstr = generate_fresh_process_parameter_name(unfold_parameter_name, process_parameter_names);
            process_parameters_injection.push_back(mcrl2::data::variable(idstr ,  *k));
            mCRL2log(verbose) << "- Injecting process parameter: " <<  idstr << "::" <<  *k << std::endl;
          }
          processed = true;
        }

        if (is_basic_sort(j -> sort()))
        {
          mCRL2log(debug) << "- No processed parameter are injected for basic sort: " <<  *j << std::endl;
          processed = true;
        }

        if (is_structured_sort(j -> sort()))
        {
          processed = true;
        }

        if (is_container_sort(j -> sort()))
        {
          processed = true;
        }
        if (!processed)
        {
          mCRL2log(mcrl2::log::debug) << *j << " is not processed" << endl;
          mCRL2log(mcrl2::log::debug) << *j << endl;
          abort();
        }
      }
      new_process_parameters.insert(new_process_parameters.end(), process_parameters_injection.begin(), process_parameters_injection.end());

      /* store mapping: process parameter -> process parameter injection:
         Required for process parameter replacement in summands
      */
      proc_par_to_proc_par_inj[*i] = process_parameters_injection;

    }
    else
    {
      new_process_parameters.push_back(*i);
    }
  }
  mCRL2log(debug) << "- New LPS process parameters: " <<  mcrl2::data::pp(new_process_parameters) << std::endl;

  //Prepare parameter substitution
  std::map<mcrl2::data::variable, mcrl2::data::data_expression> parsub = parameter_substitution(proc_par_to_proc_par_inj, affected_constructors, case_function);

  // TODO: avoid unnecessary copies of the LPS
  mcrl2::lps::stochastic_linear_process new_lps;
  new_lps.action_summands() = m_lps.action_summands();
  new_lps.deadlock_summands() = m_lps.deadlock_summands();

  // update the summands in new_lps
  unfold_summands(new_lps.action_summands(), determine_function, projection_functions);

  new_lps.process_parameters() = mcrl2::data::variable_list(new_process_parameters.begin(), new_process_parameters.end());

  for (auto i = parsub.begin()
       ; i != parsub.end()
       ; ++i)
  {
    mutable_map_substitution< std::map< mcrl2::data::variable , mcrl2::data::data_expression > > s;
    s[ i->first ] = i->second;
    mcrl2::lps::replace_variables( new_lps, s );
  }

  mCRL2log(debug) << "\nNew LPS:\n" <<  lps::pp(new_lps) << std::endl;

  assert(check_well_typedness(new_lps));

  return new_lps;
}

mcrl2::lps::stochastic_process_initializer lpsparunfold::update_linear_process_initialization(const data::function_symbol& determine_function, std::size_t parameter_at_index, const function_symbol_vector& projection_functions)
{
  //
  //update inital process
  //
  mCRL2log(verbose) << "Updating initialization...\n" << std::endl;

  data::assignment_list ass = m_init_process.assignments();
  //Create new left-hand assignment_list
  mcrl2::data::variable_vector new_ass_left;
  for (mcrl2::data::assignment_list::iterator k = ass.begin()
       ; k != ass.end()
       ; ++k)
  {
    if (proc_par_to_proc_par_inj.find(k-> lhs()) != proc_par_to_proc_par_inj.end())
    {
      for (mcrl2::data::variable_vector::iterator l =  proc_par_to_proc_par_inj[ k -> lhs() ].begin()
           ; l != proc_par_to_proc_par_inj[ k -> lhs() ].end()
           ; ++l)
      {
        new_ass_left.push_back(*l);
      }
    }
    else
    {
      new_ass_left.push_back(k-> lhs());
    }
  }
  //Create new right-hand assignment_list
  //Unfold parameters
  mcrl2::data::data_expression_vector new_ass_right;
  for (mcrl2::data::assignment_list::iterator k = ass.begin()
       ; k != ass.end()
       ; ++k)
  {
    if (static_cast<std::size_t>(std::distance(ass.begin(), k)) == parameter_at_index)
    {

      mcrl2::data::data_expression_vector ins = unfold_constructor(k -> rhs(), determine_function, projection_functions);
      //Replace unfold parameters in affected assignments
      new_ass_right.insert(new_ass_right.end(), ins.begin(), ins.end());
    }
    else
    {
      new_ass_right.push_back(k-> rhs());
    }
  }

  assert(new_ass_left.size() == new_ass_right.size());
  mcrl2::data::assignment_vector new_ass;
  while (!new_ass_left.empty())
  {
    new_ass.push_back(mcrl2::data::assignment(new_ass_left.front(), new_ass_right.front()));
    new_ass_left.erase(new_ass_left.begin());
    new_ass_right.erase(new_ass_right.begin());
  }

  const mcrl2::lps::stochastic_process_initializer new_init(mcrl2::data::assignment_list(new_ass.begin(), new_ass.end()), m_init_process.distribution());
  mCRL2log(debug) << lps::pp(new_init) << std::endl;

  return new_init;
}

std::map<mcrl2::data::variable, mcrl2::data::data_expression> lpsparunfold::parameter_substitution(std::map<mcrl2::data::variable, mcrl2::data::variable_vector > proc_par_to_proc_par_inj, mcrl2::data::function_symbol_vector affected_constructors, const mcrl2::data::function_symbol& case_function)
{
  std::map<mcrl2::data::variable, mcrl2::data::data_expression> result;
  data_expression_vector dev;

  set<mcrl2::data::variable_vector::iterator> used_iters;

  mcrl2::data::variable prev;
  for (std::map<mcrl2::data::variable, mcrl2::data::variable_vector >::iterator i = proc_par_to_proc_par_inj.begin()
       ; i != proc_par_to_proc_par_inj.end()
       ; ++i)
  {
    if (prev != i->first)
    {
      dev.clear();
    }

    dev.push_back(data_expression(i->second.front()));

    for (mcrl2::data::function_symbol_vector::iterator m = affected_constructors.begin()
         ; m != affected_constructors.end()
         ; ++m)
    {
      if (is_basic_sort(m -> sort()))
      {
        dev.push_back(*m);
      }

      if (is_structured_sort(m -> sort()))
      {
        dev.push_back(*m);
      }

      if (is_function_sort(m -> sort()))
      {
        sort_expression_list dom = function_sort(m -> sort()). domain();
        data_expression_vector arg;

        for (sort_expression_list::iterator n = dom.begin(); n != dom.end(); ++n)
        {
          for (mcrl2::data::variable_vector::iterator o = i->second.begin()
               ; o != i->second.end()
               ; ++o)
          {
            if (o -> sort() == *n && used_iters.find(o) == used_iters.end())
            {
              used_iters.insert(o);
              arg.push_back(*o);
              break;
            }
          }
        }
        dev.push_back(mcrl2::data::application(*m, arg));
      }

      if (is_container_sort(m -> sort()))
      {
        dev.push_back(*m);
      }

    }

    mCRL2log(verbose) << "Parameter substitution:\t" << data::pp(i->first) << "\t->\t" <<  data::pp(mcrl2::data::application(case_function, dev)) << std::endl;
    result.insert(std::pair<mcrl2::data::variable, mcrl2::data::data_expression>(i -> first,  mcrl2::data::application(case_function, dev)));
  }
  return result ;
}

mcrl2::data::data_expression_vector lpsparunfold::unfold_constructor(const data_expression& de, const data::function_symbol& determine_function, function_symbol_vector projection_functions)
{
  mcrl2::data::data_expression_vector result;
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

mcrl2::data::sort_expression lpsparunfold::sort_at_process_parameter_index(std::size_t parameter_at_index)
{
  mcrl2::data::variable_list lps_proc_pars_list =  m_lps.process_parameters();
  mcrl2::data::variable_vector lps_proc_pars = mcrl2::data::variable_vector(lps_proc_pars_list.begin(), lps_proc_pars_list.end());
  mCRL2log(debug) << "- Number of parameters in LPS: " <<  lps_proc_pars.size() << "" << std::endl;
  mCRL2log(verbose) << "Unfolding process parameter at index: " <<  parameter_at_index << "" << std::endl;
  if (lps_proc_pars.size() <= parameter_at_index)
  {
    mCRL2log(mcrl2::log::error) << "Given index out of bounds. Index value needs to be in the range [0," << lps_proc_pars.size() <<")." << endl;
    abort();
  }

  if (is_basic_sort(lps_proc_pars[parameter_at_index].sort()))
  {
    unfold_parameter_name = basic_sort(lps_proc_pars[parameter_at_index].sort()).name();
  }

  if (is_structured_sort(lps_proc_pars[parameter_at_index].sort()))
  {
    mcrl2::core::identifier_string nstr;
    nstr = m_identifier_generator("S");
    sort_names.insert(nstr);
    unfold_parameter_name = nstr;
  }

  if (is_container_sort(lps_proc_pars[parameter_at_index].sort()))
  {
    mcrl2::core::identifier_string nstr;
    nstr = m_identifier_generator("S");
    sort_names.insert(nstr);
    unfold_parameter_name = nstr;
  }

  return lps_proc_pars[parameter_at_index].sort();
}

mcrl2::data::data_equation lpsparunfold::create_distribution_law_over_case(
  const mcrl2::data::function_symbol& function_for_distribution,
  const mcrl2::data::function_symbol& case_function,
  const bool add_case_function_to_data_type)
{
  assert(function_sort(case_function.sort()).codomain() == function_sort(function_for_distribution.sort()).domain().front());

  variable_vector variables_used;
  mcrl2::core::identifier_string istr;

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

  mcrl2::data::function_symbol new_case_function = data::function_symbol(case_function.name(),
                         function_sort(rw_sort_expressions,function_sort(function_for_distribution.sort()).codomain()));
  if (add_case_function_to_data_type)
  {
    m_data_specification.add_mapping(new_case_function);
    generate_case_functions(m_data_specification.constructors(function_sort(new_case_function.sort()).domain().front()),new_case_function);   //add equations
  }

  /* Generate new case functions for laws */
  application rhs(new_case_function , rw_data_expressions);

  mCRL2log(verbose) << "- Added distribution law for \"" << data::pp(function_for_distribution) << "\" over \"" << data::pp(case_function) << "\": " << data::pp(data_equation(lhs,  rhs)) << std::endl;

  return data_equation(variables_used, lhs, rhs);
}

void lpsparunfold::generate_case_functions(function_symbol_vector elements_of_new_sorts, const data::function_symbol& case_function)
{
  mCRL2log(verbose) << "- Generating case function for:\t" <<  mcrl2::data::pp(case_function) << ": " <<  mcrl2::data::pp(case_function.sort()) << "" << std::endl;

  /* Generate variable identifier string for projection */
  std::string fstr = "y";
  mcrl2::core::identifier_string istr = m_identifier_generator(fstr);

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

mcrl2::lps::stochastic_specification lpsparunfold::algorithm(std::size_t parameter_at_index)
{
  m_unfold_process_parameter = sort_at_process_parameter_index(parameter_at_index);

  /* Var Dec */
  mcrl2::lps::stochastic_linear_process new_lps;
  mcrl2::lps::stochastic_process_initializer new_init;
  function_symbol_vector affected_constructors;
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
    affected_constructors = determine_affected_constructors();
    if (affected_constructors.empty())
    {
      mCRL2log(verbose) << "The selected process parameter " <<  unfold_parameter_name << " has no constructors." << std::endl;
      mCRL2log(verbose) << "No need to unfold." << std::endl;
      new_lps = m_lps;
      new_init = m_init_process;
    }
    else
    {
      /*     4 */
      elements_of_new_sorts = new_constructors(affected_constructors);
      /*     6 */
      case_function = create_case_function(affected_constructors.size());
      /*     7 */
      determine_function = create_determine_function();
      /*  8-12 */
      projection_functions = create_projection_functions(affected_constructors);
      /* 13-xx */
      create_data_equations(projection_functions, case_function, elements_of_new_sorts, affected_constructors, determine_function);

      //Reconstruct data specification, where already quite a number of mappings, constructors and functions have been added.
      m_data_specification.add_sort(fresh_basic_sort);
      m_data_specification.add_mapping(determine_function);
      m_data_specification.add_mapping(case_function);

      /*----------------*/
      new_lps = update_linear_process(case_function, affected_constructors, determine_function, parameter_at_index, projection_functions);
      new_init = update_linear_process_initialization(determine_function, parameter_at_index, projection_functions);

      /* Updating cache*/
      lspparunfold::unfold_cache_element e;
      e.cached_case_function = case_function;
      e.cached_k = affected_constructors;
      e.cached_determine_function = determine_function;
      e.cached_projection_functions = projection_functions;
      e.cached_fresh_basic_sort = fresh_basic_sort;

      m_cache->insert( pair<mcrl2::data::sort_expression , lspparunfold::unfold_cache_element>( m_unfold_process_parameter , e ));
    }
  }
  else
  {
    /* Using cache */
    mCRL2log(verbose) << "Update using cache for sort: \"" <<  mcrl2::data::pp(m_unfold_process_parameter)  << "\"..." << std::endl;

    std::map< mcrl2::data::sort_expression , lspparunfold::unfold_cache_element >::iterator ce = m_cache->find(m_unfold_process_parameter);

    fresh_basic_sort = ce->second.cached_fresh_basic_sort;
    affected_constructors = ce->second.cached_k;
    if (affected_constructors.empty())
    {
      mCRL2log(verbose) << "The selected process parameter " <<  unfold_parameter_name << " has no constructors." << std::endl;
      mCRL2log(verbose) << "No need to unfold." << std::endl;
      new_lps = m_lps;
      new_init = m_init_process;
    }

    case_function = ce->second.cached_case_function;
    determine_function = ce->second.cached_determine_function;
    projection_functions = ce->second.cached_projection_functions;

    new_lps = update_linear_process(case_function, affected_constructors, determine_function, parameter_at_index, projection_functions);
    new_init = update_linear_process_initialization(determine_function, parameter_at_index, projection_functions);
  }

  mcrl2::lps::stochastic_specification new_spec(m_data_specification, m_action_label_list, m_glob_vars, new_lps, new_init);

  assert(check_well_typedness(new_spec));

  return new_spec;
}
