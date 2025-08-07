// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsfununfold.cpp

// C++
#include <climits>

//LPS framework

//DATA
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/utilities/input_output_tool.h"
// #include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/replace.h"


using namespace mcrl2::utilities;
using namespace mcrl2::data;

using namespace mcrl2;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::log;

using mcrl2::data::tools::rewriter_tool;


struct replaced_function_parameter
{
  // This is the list of variables that can act as the function argument for this function parameter.
  data::variable_list function_arguments;
  // For each element of the domain of the list of variables below contains one variable. 
  std::vector<data::variable> new_parameters;
  // The vector below enumerates all expressions in the domain of the replaced function.
  std::vector<data::data_expression_list> domain_expressions;

  replaced_function_parameter(const data::variable_list& fa,
                              const std::vector<data::variable>& vl,
                              const std::vector<data::data_expression_list>& dl)
     : function_arguments(fa),
       new_parameters(vl),
       domain_expressions(dl)
  {}
};

data_expression equal(const variable_list& vl, data::data_expression_list dl)
{
  assert(vl.size()==dl.size());
  data::data_expression result;
  bool result_defined=false;
  for(const data::data_expression& d: vl)
  {
    if (result_defined)
    {
      result=data::sort_bool::and_(data::equal_to(d, dl.front()), result);
    }
    else
    {
      result=data::equal_to(d, dl.front());
      result_defined=true;
    }
    dl.pop_front();
  }
  return result;
}

void unfold_assignments_in_summands(
           lps::stochastic_action_summand_vector& summands, 
           std::unordered_map<variable, replaced_function_parameter>& replacement,
           const rewriter& R)
{                               
  for (lps::stochastic_action_summand& summand: summands)
  {
    data::assignment_vector new_assignments;
    for (const data::assignment& k: summand.assignments())
    {
      const std::unordered_map<variable, replaced_function_parameter>::const_iterator i=replacement.find(k.lhs());
      if (i!=replacement.end())
      {
        const std::vector<data_expression_list>& dl=i->second.domain_expressions;
        std::size_t count=0;
        for(const variable& v: i->second.new_parameters)
        {
          if (is_function_sort(k.lhs().sort()))
          {
            data_expression new_rhs=R(application(k.rhs(),dl[count]));
            if (new_rhs!=v)
            {
              new_assignments.emplace_back(v,new_rhs);
            }
            count++;
          }
          else if (is_container_sort(k.lhs().sort()))
          {
            const container_sort& s=atermpp::down_cast<container_sort>(k.lhs().sort());
            const container_type& t=s.container_name();
            if (is_fset_container(t) || is_set_container(t) || is_fbag_container(t) || is_bag_container(t))
            {
              data_expression new_rhs=
                 (is_fset_container(t)?     R(sort_fset::in(s.element_sort(),dl[count].front(),k.rhs())):
                  is_set_container(t)?      R(sort_set::in(s.element_sort(),dl[count].front(),k.rhs())):
                  is_fbag_container(t)?     R(sort_fbag::count(s.element_sort(),dl[count].front(),k.rhs())):
                  /* is_bag_container(t) */ R(sort_bag::count(s.element_sort(),dl[count].front(),k.rhs())));
              if (new_rhs!=v)
              {
                new_assignments.emplace_back(v, new_rhs);
              }
              count++;
            }
            else // container is a list, which is not expanded.
            {
              new_assignments.push_back(k);
            }
          }
        }
      }
      else
      {
        new_assignments.push_back(k);
      }
    }
    summand.assignments() = data::assignment_list(new_assignments.begin(), new_assignments.end());
  }
}   

void unfold_initializer(
           lps::stochastic_process_initializer& initializer,
           const std::unordered_map<variable, replaced_function_parameter>& replacement,
           const variable_list& old_parameters,
           const rewriter& R)
{                               
  data_expression_vector new_initializer;
  variable_list::const_iterator parameter=old_parameters.begin();
  for (const data_expression& d: initializer.expressions())
  {       
    const std::unordered_map<variable, replaced_function_parameter>::const_iterator i=replacement.find(*parameter);
    if (i!=replacement.end())
    {     
      const std::vector<data_expression_list>& dl=i->second.domain_expressions;
      for(std::size_t count=0; count<i->second.new_parameters.size(); count++)
      {   
        if (is_function_sort(d.sort()))
        {
          new_initializer.push_back(R(application(d,dl[count])));
        }
        else if (is_container_sort(d.sort()))
        {
          sort_expression sort = d.sort();
          const container_sort& s=atermpp::down_cast<container_sort>(sort);
          const container_type& t=s.container_name();
          if (is_fset_container(t))
          {
            new_initializer.push_back(R(sort_fset::in(s.element_sort(),dl[count].front(),d)));
          }
          else if (is_set_container(t))
          {
            new_initializer.push_back(R(sort_set::in(s.element_sort(),dl[count].front(),d)));
          }
          else if (is_fbag_container(t))
          {
            new_initializer.push_back(R(sort_fbag::count(s.element_sort(),dl[count].front(),d)));
          }
          else if (is_bag_container(t))
          {
            new_initializer.push_back(R(sort_bag::count(s.element_sort(),dl[count].front(),d)));
          }
          else // container is a list, which is not expanded.
          {
            new_initializer.push_back(d);
          }
        }
      }  
      
    } 
    else 
    {   
      new_initializer.push_back(d);
    }
    parameter++;
  }
  initializer=lps::stochastic_process_initializer(data_expression_list(new_initializer.begin(),new_initializer.end()),
                                                  initializer.distribution());
    
}     



class lpsfununfold_tool: public  rewriter_tool<input_output_tool>
{
  protected:

    using super = rewriter_tool<input_output_tool>;

    std::set< std::size_t > m_set_index; ///< Options of the algorithm
    std::string m_unfoldsort;
    std::size_t m_repeat_unfold = 0UL;
    bool m_alt_case_placement = false;
    bool m_possibly_inconsistent = false;
    bool m_disable_pattern_unfolding = false;

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      /* desc.add_option("index", make_mandatory_argument("[NUM]"),
                      "unfolds process parameters for comma separated indices", 'i');
      desc.add_option("sort", make_mandatory_argument("NAME"),
                      "unfolds all process parameters of sort NAME", 's');
      desc.add_option("repeat", make_mandatory_argument("NUM"),
                      "repeat unfold NUM times", 'n');
      desc.add_option("alt-case",
                      "use an alternative placement method for case functions", 'a');
      desc.add_option("possibly-inconsistent",
                      "add rewrite rules that can make a data specification inconsistent", 'p');
      desc.add_option("no-pattern",
                      "do not unfold pattern matching functions in state updates", 'x'); */
    }

    void parse_options(const command_line_parser& parser) override { super::parse_options(parser); }

    bool add_rewrite_rules_for_functions(const function_sort& s, data_specification& data_spec)
    {                         
      static std::unordered_set<function_sort> function_sorts_for_which_extra_rewrite_rules_have_been_generated;
                                                                
      if (function_sorts_for_which_extra_rewrite_rules_have_been_generated.insert(s).second)
      {   
        variable vb("b",sort_bool::bool_());
        variable vf("f",s);
        variable vg("g",s);
        variable_list vt;
        std::size_t i=0;
        for(const sort_expression& s1: s.domain()) 
        { 
          vt=push_back(vt,variable("t"+std::to_string(i),s1)); 
          ++i;
        } 
        // Equation: if(b, f, g)(t0, t1,....) = if(b, f(t0,t1,...), g(t0,t1,...)).
        data_spec.add_equation(data_equation(variable_list({vb, vf, vg}) + vt, 
                                             application(if_(vb, vf, vg),vt),
                                             if_(vb,application(vf,vt),application(vg,vt))));
        return true;
      }
      return false;
   } 


    bool add_rewrite_rules(const container_type& t, const sort_expression& s, data_specification& data_spec)
    {
      static std::unordered_set<std::pair<container_type, sort_expression>> sorts_for_which_extra_rewrite_rules_have_been_generated;
      bool no_equations_added_yet=sorts_for_which_extra_rewrite_rules_have_been_generated.empty();

      if (sorts_for_which_extra_rewrite_rules_have_been_generated.count(std::pair(t,s))==0)
      {
        if (is_set_container(t))
        {
          sorts_for_which_extra_rewrite_rules_have_been_generated.insert(std::pair(t,s));
          variable vb("b",sort_bool::bool_());
          variable vd("d",s);
          variable vs("s",sort_set::set_(s));
          variable vt("t",sort_set::set_(s));
          // Equation: d in s+t = d in s || d in t.
          data_spec.add_equation(data_equation(variable_list({vd, vs, vt}), 
                                 sort_set::in(s, vd, sort_set::union_(s, vs, vt)), 
                                 sort_bool::or_(sort_set::in(s, vd, vs), sort_set::in(s, vd, vt))));
          // Equation: d in s-t = d in s && !(d in t).
          data_spec.add_equation(data_equation(variable_list({vd, vs, vt}), 
                                 sort_set::in(s, vd, sort_set::difference(s, vs, vt)), 
                                 sort_bool::and_(sort_set::in(s, vd, vs), sort_bool::not_(sort_set::in(s, vd, vt)))));
          // Equation: d in if(b,s,t) = if(b, d in s, d in t).
          data_spec.add_equation(data_equation(variable_list({vb, vd, vs, vt}), 
                                 sort_set::in(s, vd, if_(vb, vs, vt)), 
                                 if_(vb, sort_set::in(s, vd, vs), sort_set::in(s, vd, vt))));
        }
        if (is_fset_container(t)||is_set_container(t))
        {
          if (sorts_for_which_extra_rewrite_rules_have_been_generated.insert(std::pair(fset_container(),s)).second)
          {
            variable vb("b",sort_bool::bool_());
            variable vd("d",s);
            variable vs("s",sort_fset::fset(s));
            variable vt("t",sort_fset::fset(s));
            // Equation: d in s+t = d in s || d in t.
            data_spec.add_equation(data_equation(variable_list({vd, vs, vt}), 
                                   sort_fset::in(s, vd, sort_fset::union_(s, vs, vt)), 
                                   sort_bool::or_(sort_fset::in(s, vd, vs), sort_fset::in(s, vd, vt))));
            // Equation: d in s-t = d in s && !(d in t).
            data_spec.add_equation(data_equation(variable_list({vd, vs, vt}), 
                                   sort_fset::in(s, vd, sort_fset::difference(s, vs, vt)), 
                                   sort_bool::and_(sort_fset::in(s, vd, vs), sort_bool::not_(sort_fset::in(s, vd, vt)))));
            // Equation: d in if(b,s,t) = if(b, d in s, d in t).
            data_spec.add_equation(data_equation(variable_list({vb, vd, vs, vt}), 
                                   sort_fset::in(s, vd, if_(vb, vs, vt)), 
                                   if_(vb, sort_fset::in(s, vd, vs), sort_fset::in(s, vd, vt))));
          }
        }
        if (is_bag_container(t))
        {       
          sorts_for_which_extra_rewrite_rules_have_been_generated.insert(std::pair(t,s));
          variable vb("b",sort_bool::bool_());
          variable vd("d",s);
          variable vs("s",sort_bag::bag(s));
          variable vt("t",sort_bag::bag(s));
          // Equation: count(d,s+t) = count(d,s) + count(d,t).
          data_spec.add_equation(data_equation(variable_list({vd, vs, vt}), 
                                 sort_bag::count(s, vd, sort_bag::union_(s, vs, vt)),
                                 sort_nat::plus(sort_bag::count(s, vd, vs), sort_bag::count(s, vd, vt))));
          // Equation: count(d,s-t) = monus(count(d,s), count(d,t)).
          data_spec.add_equation(data_equation(variable_list({vd, vs, vt}), 
                                 sort_bag::count(s, vd, sort_bag::difference(s, vs, vt)),
                                 sort_nat::monus(sort_bag::count(s, vd, vs), sort_bag::count(s, vd, vt))));
          // Equation: count(d,if(b,s,t)) = if(count(b, d),count(d,t)).
          data_spec.add_equation(data_equation(variable_list({vb, vd, vs, vt}), 
                                 sort_bag::count(s, vd, if_(vb, vs, vt)), 
                                 if_(vb, sort_bag::count(s, vd, vs), sort_bag::count(s, vd, vt))));
        }                                                           
        if (is_fbag_container(t)||is_bag_container(t))
        {       
          if (sorts_for_which_extra_rewrite_rules_have_been_generated.insert(std::pair(fbag_container(),s)).second)
          {                                                         
            variable vb("b",sort_bool::bool_());                    
            variable vd("d",s);                
            variable vs("s",sort_fbag::fbag(s));
            variable vt("t",sort_fbag::fbag(s));
            // Equation: count(d,s+t) = count(d,s) + count(d,t).
            data_spec.add_equation(data_equation(variable_list({vd, vs, vt}),
                                   sort_fbag::count(s, vd, sort_fbag::union_(s, vs, vt)),
                                   sort_nat::plus(sort_fbag::count(s, vd, vs), sort_fbag::count(s, vd, vt))));
            // Equation: count(d,s-t) = monus(count(d,s), count(d,t)).
            data_spec.add_equation(data_equation(variable_list({vd, vs, vt}),
                                   sort_fbag::count(s, vd, sort_fbag::difference(s, vs, vt)),
                                   sort_nat::monus(sort_fbag::count(s, vd, vs), sort_fbag::count(s, vd, vt))));
            // Equation: count(d,if(b,s,t)) = if(count(b, d),count(d,t)).
            data_spec.add_equation(data_equation(variable_list({vb, vd, vs, vt}),
                                   sort_fbag::count(s, vd, if_(vb, vs, vt)),
                                   if_(vb, sort_fbag::count(s, vd, vs), sort_fbag::count(s, vd, vt))));
          }
        }

        if (no_equations_added_yet)
        {
          // equation: if(b,true,false)=b.
          variable vb("b",sort_bool::bool_());
          data_spec.add_equation(data_equation(variable_list({vb}),if_(vb,sort_bool::true_(),sort_bool::false_()), vb));
        }
        return true;
      }
      return false;
    }

  public:

    lpsfununfold_tool()
      : super(
        "lpsfununfold",
        "Jan Friso Groote",
        "Unfolds process parameters of type function sort, fset, set, fbag and bag with a finite domain",
        "Unfolds each parameter with a function type D1#D2#...#Dn -> D into a sequence "
        "of parameters of sort D in the lps, each parameter of type FSet/Set(D) into a sequence of boolean "
        "variables, and each parameter of type FBag/BagD) into a sequence of variables of type Nat. "
        "The number of variables is equal to the number of elements in D1#...#Dn, resp. D. "
        "The input is taken from INFILE and writes the result to OUTFILE. If INFILE is not present, stdin is "
        "used. If OUTFILE is not present, stdout is used."
      )
    {}

    ///Reads a specification from input_file,
    ///applies instantiation of sums to it and writes the result to output_file.
    bool run() override
    {
      lps::stochastic_specification spec;
      load_lps(spec, input_filename());
      set_identifier_generator fresh_name_generator;
      fresh_name_generator.add_identifiers(lps::find_identifiers(spec));
      std::unordered_map<variable, replaced_function_parameter> representation_for_the_new_parameters;
      rewriter R = create_rewriter(spec.data());

      std::vector<data::variable> resulting_parameters;
      bool new_rewrite_rules_added=false;
      for (const data::variable& v: spec.process().process_parameters())
      {
        if (is_function_sort(v.sort()))
        {
          // Handle variable v which has a function sort. Check whether it has a finite domain. 
          const function_sort& s=atermpp::down_cast<function_sort>(v.sort());

          if (!spec.data().is_certainly_finite(s.domain()))
          {
            mCRL2log(verbose) << "The process parameter " << v << ":" << v.sort() << " is not replaced as its domain does not seem finite.\n";
            resulting_parameters.push_back(v);
          }
          else
          {
            new_rewrite_rules_added=add_rewrite_rules_for_functions(s, spec.data())||new_rewrite_rules_added;

            std::vector<data::variable> new_parameters;
            std::vector<data::data_expression_list> new_enumerated_domain_elements(1);
            variable_vector new_arguments;
            for(const sort_expression& se: s.domain())
            {
              new_arguments.emplace_back(fresh_name_generator("x"), se);
              data_expression_vector new_elements=enumerate_expressions(se, spec.data(), R);
              std::vector<data::data_expression_list> old_enumerated_domain_elements=new_enumerated_domain_elements;
              new_enumerated_domain_elements.clear();
              for (const data_expression& d: new_elements)
              {
                for (const data_expression_list& l: old_enumerated_domain_elements)
                {
                  new_enumerated_domain_elements.push_back(l);
                  new_enumerated_domain_elements.back().push_front(d); 
                }
              }
            }
            for(data_expression_list& l: new_enumerated_domain_elements)
            // for(std::size_t i=0; i<new_enumerated_domain_elements.size(); ++i)
            {
              new_parameters.emplace_back(fresh_name_generator(v.name()), s.codomain());
              l=reverse(l); // Then new_enumerated_domain_elements were stored in the wrong order. 
            }
            mCRL2log(verbose) << "The process parameter " << v << ": " << v.sort() << " is replaced by " 
                              << new_enumerated_domain_elements.size() << " new parameters.\n";
            representation_for_the_new_parameters.insert({v, replaced_function_parameter(
                                                                variable_list(new_arguments.begin(), new_arguments.end()), 
                                                                new_parameters, 
                                                                new_enumerated_domain_elements)});
          }
        }
        else if (is_container_sort(v.sort()))
        {
          const container_sort& s=atermpp::down_cast<container_sort>(v.sort());
          const container_type& t=s.container_name();
          
          if (is_list_container(t))
          {
            resulting_parameters.push_back(v);  // A list container is not replaced.
          }
          else
          { // This is an fset, set, fbag or bag container.
            assert(is_fset_container(t) || is_set_container(t) || is_fbag_container(t) || is_bag_container(t));
            if (!spec.data().is_certainly_finite(s.element_sort()))
            { 
              mCRL2log(verbose) << "The process parameter " << v << ":" << v.sort() << " is not replaced as its element sort does not seem finite.\n";
              resulting_parameters.push_back(v);
            }
            else
            { 
              if (is_fset_container(t) || is_set_container(t) || is_fbag_container(t) || is_bag_container(t)) 
              { 
                new_rewrite_rules_added= add_rewrite_rules(t, s.element_sort(), spec.data())||new_rewrite_rules_added;
              
                std::vector<data::variable> new_parameters;
                variable_vector new_arguments; 
                
                new_arguments.emplace_back(fresh_name_generator("x"), s.element_sort());
                std::vector<data::data_expression_list> new_enumerated_domain_elements;
                data_expression_vector new_elements=enumerate_expressions(s.element_sort(), spec.data(), R);
                for (const data_expression& d: new_elements)
                {
                  data_expression_list l;
                  l.push_front(d);
                  new_enumerated_domain_elements.push_back(l);
                }
                const sort_expression target_sort=(is_fset_container(t) || is_set_container(t)? sort_bool::bool_(): sort_nat::nat());
                
                for(std::size_t i=0; i<new_enumerated_domain_elements.size(); ++i)
                { 
                  new_parameters.emplace_back(fresh_name_generator(v.name()), target_sort);
                }
                mCRL2log(verbose) << "The process parameter " << v << ": " << v.sort() << " is replaced by "
                                  << new_enumerated_domain_elements.size() << " new parameters.\n";
                representation_for_the_new_parameters.insert({v, replaced_function_parameter(
                                                                    variable_list(new_arguments.begin(), new_arguments.end()),
                                                                    new_parameters, 
                                                                    new_enumerated_domain_elements)});
              }
              /* else
              {
                mCRL2log(verbose) << "The process parameter " << v << ":" << v.sort() << " is of type set, bag or fbag, which are not yet handled..\n";
                resulting_parameters.push_back(v);
              } */
            }
          }

        }
        else
        {
          resulting_parameters.push_back(v);
        }
      }
      if (new_rewrite_rules_added)
      {
        R = create_rewriter(spec.data());
      }
      // Replace the function parameters by the newly generated parameters.

      data::mutable_indexed_substitution sigma;
      std::vector<data::variable> new_parameters;
      // data::variable_list old_parameters=spec.process().process_parameters();
      // for(const replaced_function_parameter& rfp: representation_for_the_new_parameters)
      for(const variable& v: spec.process().process_parameters())
      {
        const std::unordered_map<variable, replaced_function_parameter>::const_iterator i=representation_for_the_new_parameters.find(v);
        if (i!=representation_for_the_new_parameters.end())
        {
          // Here par==rfp.variable. 
          new_parameters.insert(new_parameters.end(), i->second.new_parameters.begin(), i->second.new_parameters.end());

          data::variable_list new_parameters(i->second.new_parameters.begin(), i->second.new_parameters.end());
          data::data_expression body;
          bool body_defined=false;
          std::vector<data::variable>::const_iterator new_parameters_it = i->second.new_parameters.begin();
          for(const data::data_expression_list& dl: i->second.domain_expressions)
          {
            if (!body_defined)
            {
              if (is_function_sort(v.sort()))
              {
                body = *new_parameters_it;
                body_defined=true;
              }
              else if (is_container_sort(v.sort()))
              {
                const container_sort& s=atermpp::down_cast<container_sort>(v.sort());
                const container_type& t=s.container_name();
                if (is_fset_container(t) || is_set_container(t))
                {
                  body = if_(*new_parameters_it,
                                  sort_fset::insert(s.element_sort(), dl.front(),sort_fset::empty(s.element_sort())),
                                  sort_fset::empty(s.element_sort()));
                  body_defined=true;
                }
                else if (is_fbag_container(t) || is_bag_container(t))
                { 
                  body = if_(equal_to(*new_parameters_it,data::sort_nat::c0()),
                                  sort_fbag::empty(s.element_sort()),
                                  sort_fbag::insert(s.element_sort(), 
                                                    dl.front(),
                                                    sort_nat::nat2pos(*new_parameters_it), 
                                                    sort_fbag::empty(s.element_sort())));
                  body_defined=true;
                }
              }
            }
            else
            {
              if (is_function_sort(v.sort()))
              {
                body=if_(equal(i->second.function_arguments,dl),*new_parameters_it,body);
              }
              else if (is_container_sort(v.sort()))
              {
                const container_sort& s=atermpp::down_cast<container_sort>(v.sort());
                const container_type& t=s.container_name();
                if (is_fset_container(t) || is_set_container(t))
                {
                  body = sort_fset::union_(s.element_sort(),
                                           if_(*new_parameters_it, 
                                               sort_fset::insert(s.element_sort(), dl.front(), sort_fset::empty(s.element_sort())),
                                               sort_fset::empty(s.element_sort())),
                                           body);
                }
                else if (is_fbag_container(t) || is_bag_container(t))
                {
                  body = sort_fbag::union_(s.element_sort(),
                                           if_(equal_to(*new_parameters_it, data::sort_nat::c0()),
                                               sort_fbag::empty(s.element_sort()),
                                               sort_fbag::insert(s.element_sort(), 
                                                                 dl.front(), 
                                                                 sort_nat::nat2pos(*new_parameters_it), 
                                                                 sort_fbag::empty(s.element_sort()))),
                                           body);
                }
              }
            }
            new_parameters_it++;
          }
          if (is_function_sort(v.sort()))
          {
            sigma[v]=lambda(i->second.function_arguments,body);
          }
          else if (is_container_sort(v.sort()))
          {
            const container_sort& s=atermpp::down_cast<container_sort>(v.sort());
            const container_type& t=s.container_name();
            if (is_set_container(t)) 
            {
              sigma[v]=data::sort_set::constructor(s.element_sort(),data::sort_set::false_function(s.element_sort()), body);
            }
            else if (is_bag_container(t)) 
            {
              sigma[v]=data::sort_bag::constructor(s.element_sort(),data::sort_bag::zero_function(s.element_sort()), body);
            }
            else if (is_fset_container(t) || is_fbag_container(t)) 
            {
              sigma[v]=body;
            }
          }
        }
        else
        {
          new_parameters.push_back(v);
        }
      }

      variable_list old_process_parameters=spec.process().process_parameters();
      lps::replace_variables(spec.process(),sigma);
      unfold_assignments_in_summands(spec.process().action_summands(), representation_for_the_new_parameters, R);
      unfold_initializer(spec.initial_process(), representation_for_the_new_parameters, old_process_parameters, R);
      assert(spec.initial_process().expressions().size()==new_parameters.size());
      spec.process().process_parameters()=variable_list(new_parameters.begin(), new_parameters.end());
      assert(check_well_typedness(spec));      
      save_lps(spec, output_filename());

      return true;
    }

};

int main(int argc, char** argv)
{
  return lpsfununfold_tool().execute(argc, argv);
}
