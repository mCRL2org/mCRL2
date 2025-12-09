// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps_explore_domains.h
/// \brief The lps_explore_domains algorithm.

#ifndef MCRL2_LPS_PARVALUES_H
#define MCRL2_LPS_PARVALUES_H

#include "unordered_set"
#include "unordered_map"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lps/detail/parameter_selection.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"

namespace mcrl2::lps
{

namespace detail
{

template <class VARIABLECONTAINER>
std::string ppsort(const VARIABLECONTAINER& s)
{
  std::string result;
  for(const data::variable& v: s)
  {
    if (!result.empty())
    {
      result = result + ",";
    } 
    result = result + pp(v) + ":" + pp(v.sort());
  }
  return result;
}

struct elements_per_domain
{
  std::unordered_set < data::data_expression > m_elements_stable;
  std::unordered_set < data::data_expression > m_elements_added_in_previous_round;
  std::unordered_set < data::data_expression > m_elements_added_in_current_round;
};

class elements_for_all_domains 
{
  using variable_expression_pair_type=std::unordered_map < data::variable, elements_per_domain >::value_type;
  std::unordered_map < data::variable, elements_per_domain > m_domains;

public:

  /* elements_per_domain& operator[](const data::variable v)
  {
    return m_domains[v];
  } */

  const elements_per_domain& at(const data::variable v) const
  {
    assert(m_domains.contains(v));
    return m_domains.at(v);
  }

  // Insert a new element e in the domain of variable v.
  // Report true if the element was not yet present.
  bool contains(const data::variable& v, const data::data_expression& e) const
  {
    return m_domains.contains(v) &&
           (m_domains.at(v).m_elements_stable.contains(e) ||  
            m_domains.at(v).m_elements_added_in_previous_round.contains(e) || 
            m_domains.at(v).m_elements_added_in_current_round.contains(e));
  }

  // Make a new parameter v with initial value e. . 
  void new_parameter(const data::variable& v, const data::data_expression& e)
  {
    assert(!m_domains.contains(v));
    m_domains[v].m_elements_added_in_current_round.insert(e);
  }

  // Insert a new element e in the domain of variable v.
  // Report true if the element was not yet present.
  bool insert(const data::variable& v, const data::data_expression& e)
  {
    if (contains(v,e))
    {
      return false;
    }
    m_domains.at(v).m_elements_added_in_current_round.insert(e);
    return true;
  }

  // Checks whether there are domains with values from the previous round related to parameters that follow v.
  bool has_available_next_values_from_the_previous_round(const data::variable& v)
  {
    std::unordered_map < data::variable, elements_per_domain > ::const_iterator i=m_domains.find(v);
    if (i!=m_domains.end())
    {
      ++i;
      for( ; i!=m_domains.end(); ++i)
      {
        if (!i->second.m_elements_added_in_previous_round.empty())
        {
          return true;
        }
      }
    }
    return false;
  }

  // If m_domains[v].m_elements_added_in_current_round is not emptye for some variable v, the system is not stable. 
  bool stable() const
  { 
    for(std::unordered_map < data::variable, elements_per_domain >::const_iterator i=m_domains.begin(); i!=m_domains.end(); ++i)
    { 
      if (!i->second.m_elements_added_in_current_round.empty())
      {
        return false;
      }
    }
    return true;
  }

  // Add the elements of the previous round to the stable elements, move the current round to the previous round, 
  // and clear the current round for all parameters.
  void new_round()
  {
    for(std::unordered_map < data::variable, elements_per_domain >::iterator i=m_domains.begin(); i!=m_domains.end(); ++i)
    {
      for(const data::data_expression& e: i->second.m_elements_added_in_previous_round)
      {
        i->second.m_elements_stable.insert(e);
      }
      i->second.m_elements_added_in_previous_round.clear();
      std::swap(i->second.m_elements_added_in_previous_round,i->second.m_elements_added_in_current_round);
    }
  }

  // Provide the product size of the domains for the parameters.
  double product_size()
  {
    double size=1;
    for(const variable_expression_pair_type& elm: m_domains)
    {
      size=size*(elm.second.m_elements_stable.size()+
                 elm.second.m_elements_added_in_previous_round.size()+
                 elm.second.m_elements_added_in_current_round.size());
    }
    return size;
  }

  // List the elements of the domains per variable. If joined=false they are split in stable and unstable elements;
  std::string report(bool joined=true)
  {
    std::stringstream output;
    for(const variable_expression_pair_type& elm: m_domains)
    {
      output << "Parameter " << elm.first << "(" << (elm.second.m_elements_stable.size()+
                 elm.second.m_elements_added_in_previous_round.size()+
                 elm.second.m_elements_added_in_current_round.size()) << ") " << (!joined?"\n  Stable elements: {":": {");
      bool first=true;
      for(const data::data_expression& e: elm.second.m_elements_stable)
      {
        output << (first?" ":", ") << e;
        first=false;
      }
      if (!joined) 
      {
        output << " }\n  Previous round: {";
        first=true;
      }
      for(const data::data_expression& e: elm.second.m_elements_added_in_previous_round)
      {
        output << (first?" ":", ") << e;
        first=false;
      }
      if (!joined) 
      {
        output << " }\n  Current round: {";
        first=true;
      }
      for(const data::data_expression& e: elm.second.m_elements_added_in_current_round)
      {
        output << (first?" ":", ") << e;
        first=false;
      }
      output << " }\n";
    }
    output << "Upperbound on the statespace is " << product_size() << "\n---------------------------------------------------------------------\n";
    return output.str();
  }
};

} // namespace detail

/// \brief Algorithm class that can be used to apply the lps_explore_domains algorithm
///
/// All parameter values of the process parameters are enumerated.
template<typename DataRewriter, typename Specification>
class lps_explore_domains_algorithm: public detail::lps_algorithm<Specification>
{
  using enumerator_element = data::enumerator_list_element_with_substitution<>;
  using super = typename detail::lps_algorithm<Specification>;
  using process_type = typename Specification::process_type;
  using action_summand_type = typename process_type::action_summand_type;
  using super::m_spec;

  protected:
    /// Rewriter
    DataRewriter m_rewriter;
    std::size_t m_qlimit;
    data::enumerator_identifier_generator& m_generator;

    detail::elements_for_all_domains m_parameter_values;

    // Put the value of each parameter in the initial process in the domain of that parameter.
    void process_initial_process()
    {
      data::data_expression_list::const_iterator initial_state_iterator = m_spec.initial_process().expressions().begin();
      for(const data::variable& v: m_spec.process().process_parameters())
      {
        m_parameter_values.new_parameter(v, m_rewriter(*initial_state_iterator));
        ++initial_state_iterator;
      }
    }

    void calculate_additional_parameter_values_set_substitution_non_occurring_vars(
                           data::mutable_indexed_substitution<>& sigma,
                           const data::variable& v,
                           const data::variable_list& sumvars,
                           const data::data_expression& condition,
                           const data::data_expression& new_expression_for_v,
                           const bool used_a_variable_from_the_new_round)
    {
      data::data_expression rewritten_condition = m_rewriter(condition, sigma);
      std::set<data::variable> parameters_in_condition = find_free_variables(rewritten_condition);
      for(const data::variable& v: sumvars)
      {
        parameters_in_condition.erase(v);
      }
      if (parameters_in_condition.empty())
      {
        if (!used_a_variable_from_the_new_round || rewritten_condition==data::sort_bool::false_())
        {
          return;
        }

        // First split the sumvars in those that occur in rewritten_new_expression and those that do not.
        data::variable_list sumvars_in_rewritten;
        data::variable_list sumvars_not_in_rewritten;
        data::variable_list sumvars_in_rewritten_or_condition;
        std::set<data::variable> vars_in_rewritten=data::find_free_variables(new_expression_for_v);
        const bool new_expression_has_no_variables=vars_in_rewritten.empty();
        std::set<data::variable> vars_in_condition=data::find_free_variables(rewritten_condition);
        for(const data::variable& v: sumvars)
        {
          if (vars_in_rewritten.contains(v))
          {
            sumvars_in_rewritten.push_front(v);
            sumvars_in_rewritten_or_condition.push_front(v);
          }
          else if (vars_in_condition.contains(v))
          {
            sumvars_in_rewritten_or_condition.push_front(v);
            sumvars_not_in_rewritten.push_front(v);
            
          }
        }
        sumvars_in_rewritten=reverse(sumvars_in_rewritten);

        data::data_expression quantified_condition=(sumvars_not_in_rewritten.empty()?rewritten_condition:data::exists(sumvars_not_in_rewritten,rewritten_condition));
    
        mCRL2log(log::debug) << "Enumerate " << detail::ppsort(sumvars_in_rewritten) << " in " << quantified_condition << "\n"; 
        using enumerator_type = data::enumerator_algorithm<>;
        using enumerator_element = data::enumerator_list_element_with_substitution<data::data_expression>;

        const std::size_t enumeration_limit = m_qlimit;
        enumerator_type enumerator(m_rewriter, m_spec.data(),
                                   m_rewriter, m_generator, false, enumeration_limit);

        /* Create a list to store solutions */
        std::size_t count=enumerator.enumerate(enumerator_element(sumvars_in_rewritten_or_condition, rewritten_condition),
                             sigma,
                             [&](const enumerator_element& p)
                             {
                               p.add_assignments(sumvars_in_rewritten,sigma,m_rewriter);
                               m_parameter_values.insert(v,m_rewriter(new_expression_for_v,sigma));
                               assert(find_free_variables(m_rewriter(new_expression_for_v,sigma)).empty());
                               p.remove_assignments(sumvars_in_rewritten, sigma);
                               // Stop if the condition is true and no variables occur in the new_expression.
                               return new_expression_has_no_variables && p.expression()==data::sort_bool::true_(); 
                             },
                             // Ignore the following solution. 
                             [&](const data::data_expression& d)->bool 
                             { 
                               if (find_free_variables(d).empty() && d!=data::sort_bool::true_() && d!=data::sort_bool::false_())
                               { 
                                 mCRL2log(log::warning) << "The expression " << d << " does not rewrite to true or false\n";
                               }
                               return d==data::sort_bool::false_(); 
                             },
                             [](const data::data_expression& d)->bool { return false; }
                            );
        if (count>=enumeration_limit)
        {
          if (vars_in_rewritten.empty())
          {
             m_parameter_values.insert(v,new_expression_for_v);
          }
          else 
          {
            throw mcrl2::runtime_error("Cannot enumerate " + detail::ppsort(sumvars_in_rewritten_or_condition) + " in " + data::pp(rewritten_condition) + "\n");
          }
        }

        return;
      }

      data::variable curr_var=*parameters_in_condition.begin();
      {
        for(const data::data_expression& e: m_parameter_values.at(curr_var).m_elements_stable)
        {
          sigma[curr_var]=e;
          calculate_additional_parameter_values_set_substitution_non_occurring_vars(sigma, v, sumvars, rewritten_condition, new_expression_for_v,used_a_variable_from_the_new_round);
        }
      }

      for(const data::data_expression& e: m_parameter_values.at(curr_var).m_elements_added_in_previous_round)
      {
        sigma[curr_var]=e;
        calculate_additional_parameter_values_set_substitution_non_occurring_vars(sigma, v, sumvars, rewritten_condition, new_expression_for_v,true);
      }
      sigma[curr_var]=curr_var;

      return;
    }

    void calculate_additional_parameter_values_set_substitution(
                           data::mutable_indexed_substitution<>& sigma,
                           const data::variable& v, 
                           const data::variable_list& sumvars, 
                           const data::data_expression& condition, 
                           const data::data_expression& new_expression_for_v,
                           const bool used_a_variable_from_the_new_round)
    {
      // We consider the free variables in the new expression relevant as they determine the new value parameter v can get. 
      const data::data_expression rewritten_new_expression=m_rewriter(new_expression_for_v,sigma);
      std::set<data::variable> relevant_parameters = find_free_variables(rewritten_new_expression);
      bool rewritten_new_expression_is_closed = relevant_parameters.empty();
      std::set<data::variable> relevant_sum_variables;
      // Split the relevant variables in those that are parameters and those that occur in the sum variables 
      for(const data::variable& v: sumvars)
      {
        relevant_parameters.erase(v);
        relevant_sum_variables.insert(v);
      }
      
      if (relevant_parameters.empty())
      {
        const data::data_expression rewritten_condition=m_rewriter(condition,sigma);
        if (rewritten_condition==data::sort_bool::false_())
        {
          return;
        }

        if (rewritten_new_expression_is_closed && m_parameter_values.contains(v,rewritten_new_expression))
        {
          return;
        }
        if (rewritten_condition==data::sort_bool::true_() && data::find_free_variables(rewritten_new_expression).empty())
        {
          m_parameter_values.insert(v,rewritten_new_expression); 
          return;
        }
        calculate_additional_parameter_values_set_substitution_non_occurring_vars(
                           sigma, v, sumvars, rewritten_condition, rewritten_new_expression, used_a_variable_from_the_new_round);
        return;
      }

      const data::variable curr_var= *relevant_parameters.begin();
      if (true || m_parameter_values.has_available_next_values_from_the_previous_round(curr_var))  // Zoek uit of dit altijd moet....
      {
        for(const data::data_expression& e: m_parameter_values.at(curr_var).m_elements_stable)
        {
          sigma[curr_var]=e;
          calculate_additional_parameter_values_set_substitution(sigma, v, sumvars, condition, rewritten_new_expression,true);
        }
      }

      for(const data::data_expression& e: m_parameter_values.at(curr_var).m_elements_added_in_previous_round)
      {
        sigma[curr_var]=e;
        calculate_additional_parameter_values_set_substitution(sigma, v, sumvars, condition, rewritten_new_expression,used_a_variable_from_the_new_round);
      }
      sigma[curr_var]=curr_var;

      return;
    }
    
    // Insert new parameter values for parameter v based on the given sum variables, condition and new expression.
    // Yields true if a new value is added to the domain of v. 
    void calculate_additional_parameter_values(
                           const data::variable& v, 
                           const data::variable_list& sumvars, 
                           const data::data_expression& condition, 
                           const data::data_expression& new_expression_for_v)
    {
      if (v==new_expression_for_v) // The value of parameter v does not change. 
      {
        return;
      }

      data::mutable_indexed_substitution<> sigma;
      calculate_additional_parameter_values_set_substitution(sigma, 
                                                             v, 
                                                             sumvars, 
                                                             condition, 
                                                             new_expression_for_v,
                                                             false);
    }

    // Insert parameter values in their respective domains based on the already exising values based on this action summand.
    // Yields true if a new value is added to some domain.
    void process_action_summand(const action_summand& a)
    {
      mCRL2log(log::debug) << "Process action summand " << a << "\n==================================================================================\n";

      for(const data::variable& v: m_spec.process().process_parameters())
      {
        calculate_additional_parameter_values(v, 
                                              a.summation_variables(), 
                                              a.condition(), 
                                              data::find_rhs(v, a.assignments()));
      }
    }

    void explore_domains()
    {
      // Initial process
      mCRL2log(log::debug) << "Process the initial state" << std::endl;
      process_initial_process();

      // Summands
      mCRL2log(log::debug) << "Updating summands" << std::endl;

      std::size_t round=0;
      while (!m_parameter_values.stable())      
      {
        mCRL2log(log::verbose) << "Parameter instantiation round " << round << " (domain upperbound: " << m_parameter_values.product_size() << ").\n";
        round++;
        mCRL2log(log::debug) << m_parameter_values.report(true);
        m_parameter_values.new_round();
        for (const action_summand& a: m_spec.process().action_summands())
        {
          process_action_summand(a);
        }
      }


    }

  public:
    /// \brief Constructor for lps_explore_domains algorithm
    /// \param spec Specification to which the algorithm should be applied
    /// \param r a rewriter for data
    lps_explore_domains_algorithm(Specification& spec,
                     DataRewriter& r,
                     const std::size_t qlimit)
                     
      : detail::lps_algorithm<Specification>(spec),
        m_rewriter(r),
        m_qlimit(qlimit),
        m_generator(const_cast<data::enumerator_identifier_generator&>(r.identifier_generator()))
    {
    }

    /// \brief Apply the algorithm to the specification passed in the
    ///        constructor
    void run()
    {
      mCRL2log(log::debug) << "Start to explore parameter domains" << std::endl;
      detail::instantiate_global_variables(m_spec);
      explore_domains();
      mCRL2log(log::verbose) << m_parameter_values.report(true);
      mCRL2log(log::info) << "This process has at most " << m_parameter_values.product_size() << " states.\n";
    }
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_PARVALUES_H
