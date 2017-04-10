// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_bisim.h


#ifndef MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H
#define MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H

#include <string>

#include "mcrl2/data/bool.h"
#include "mcrl2/data/detail/linear_inequalities_utilities.h"
#include "mcrl2/data/detail/prover/bdd_path_eliminator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/data/substitutions/data_expression_assignment.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/utilities/logger.h"

#include "simplifier.h"
// #include "simplifier_dbm.h"
#include "simplifier_fourier_motzkin.h"

namespace mcrl2
{
namespace data
{

using namespace mcrl2::log;

template <template <class> class Traverser, class OutputIterator>
struct find_linear_inequality_traverser: public Traverser<find_linear_inequality_traverser<Traverser, OutputIterator> >
{
  typedef Traverser<find_linear_inequality_traverser<Traverser, OutputIterator> > super;
  using super::enter;
  using super::leave;
  using super::apply;

  OutputIterator out;
  std::set< variable > real_free_vars;
  bool search_free_var;
  bool found_free_var;

  find_linear_inequality_traverser(OutputIterator out_, std::set< variable > real_free_vars_)
    : out(out_)
    , real_free_vars(real_free_vars_)
  {}

  void apply(const data::variable& v)
  {
    if(search_free_var && !found_free_var && real_free_vars.find(v) != real_free_vars.end())
    {
      found_free_var = true;
    }
  }

  void apply(const data::application& x)
  {
    if (is_equal_to_application(x) ||
        is_not_equal_to_application(x) ||
        is_less_application(x) ||
        is_less_equal_application(x) ||
        is_greater_application(x) ||
        is_greater_equal_application(x))
    {
      // Switch to 'searching for free variables of type Real' mode
      search_free_var = true;
      found_free_var = false;
      super::apply(x);
      if(found_free_var)
      {
        // This expression contains a free variable of type Real, so add it to output
        *out = x;
      }
      search_free_var = false;
    }
    else if(!search_free_var || !found_free_var)
    {
      // traverse sub-expressions
      super::apply(x);
    }
  }
};

template <template <class> class Traverser, class OutputIterator>
find_linear_inequality_traverser<Traverser, OutputIterator>
make_find_linear_inequality_traverser(OutputIterator out, std::set< variable > real_free_vars)
{
  return find_linear_inequality_traverser<Traverser, OutputIterator>(out, real_free_vars);
}

template <typename T, typename OutputIterator>
void find_linear_inequalities(const T& x, OutputIterator o, std::set< variable > real_free_vars)
{
  make_find_linear_inequality_traverser<data::data_expression_traverser>(o, real_free_vars).apply(x);
}

template <typename T>
std::set<data::data_expression> find_linear_inequalities(const T& x, std::set< variable > real_free_vars)
{
  std::set<data::data_expression> result;
  find_linear_inequalities(x, std::inserter(result, result.end()), real_free_vars);
  return result;
}

struct eliminate_existential_quantifier_sigma: public std::unary_function<data_expression, data_expression>
{

  rewriter rewr;

  eliminate_existential_quantifier_sigma(rewriter rewr_)
  :  rewr(rewr_)
  {}

  const data_expression operator()(const data_expression& d) const
  {
    if(is_exists(d))
    {
      const variable_list variables = static_cast<exists>(d).variables();
      const data_expression body = static_cast<exists>(d).body();

      std::cout << "Replacing " << d << std::flush;

      variable_list new_variables;
      data_expression new_body;
      fourier_motzkin(body, variables, new_body, new_variables, rewr);

      std::cout << " with " << exists(new_variables, new_body) << std::endl;
      return rewr(exists(new_variables, new_body));
    }
    return d;
  }
};

template <typename Specification>
class symbolic_bisim_algorithm: public mcrl2::lps::detail::lps_algorithm<Specification>
{
  typedef typename mcrl2::lps::detail::lps_algorithm<Specification> super;
  typedef typename Specification::process_type process_type;
  typedef typename process_type::action_summand_type action_summand_type;
  typedef rewriter::substitution_type substitution_t;
  using super::m_spec;

protected:
  const rewrite_strategy strat;
  rewriter rewr;
  rewriter proving_rewr;

  variable_list                  process_parameters;
  data_expression                invariant;
  std::set<data_expression>      partition;
  data_specification             ad_hoc_data;
  bool                           m_use_path_eliminator;
  detail::BDD_Path_Eliminator    m_path_eliminator;
  // simplifier_dbm                 simpl;
  simplifier_fourier_motzkin                 simpl;

  template <typename Container>
  std::string pp_container(Container& expressions, typename atermpp::enable_if_container<Container, data_expression>::type* = nullptr)
  {

    std::ostringstream out;
    out << "[";
    for(typename Container::const_iterator it = expressions.begin(); it != expressions.end(); it++)
    {
      if(it != expressions.begin())
      {
        out << ", ";
      }
      out << pp(*it);
    }
    out << "]";
    return out.str();
  }

  core::identifier_string iff_name()
  {
    static core::identifier_string iff_name = core::identifier_string("<=>");
    return iff_name;
  }

  function_symbol iff()
  {
    static function_symbol iff(iff_name(), make_function_sort(sort_bool::bool_(), sort_bool::bool_(), sort_bool::bool_()));
    return iff;
  }

  inline application iff(const data_expression& d1, const data_expression& d2)
  {
    return iff()(d1, d2);
  }

  void add_ad_hoc_rules()
  {
    ad_hoc_data = merge_data_specifications(m_spec.data(),parse_data_specification(
      "var "
        "a,b,c:Bool;"
        "r1,r2,r3:Real;"
      "eqn "
        "!a || a = true;"
        "a || !a = true;"
        "!a && a = false;"
        "a && !a = false;"

        "r2 > r3 -> !(r1 < r2) && r1 < r3 = false;"
        "r2 > r3 -> r1 < r3 && !(r1 < r2) = false;"
        "r2 < r3 -> !(r1 < r2) || r1 < r3 = true;"
        "r2 < r3 -> r1 < r3 || !(r1 < r2) = true;"
      ));

    variable vb1("b1", sort_bool::bool_());
    variable vb2("b2", sort_bool::bool_());
    variable vb3("b3", sort_bool::bool_());
    variable vp1("p1", sort_pos::pos());
    variable vp2("p2", sort_pos::pos());
    variable vr1("r1", sort_real::real_());
    variable vr2("r2", sort_real::real_());
    variable vr3("r3", sort_real::real_());

    //  a && a = a;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), sort_bool::and_(vb1, vb1), vb1));
    //  a && (a && b) = a && b;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), sort_bool::and_(vb1, sort_bool::and_(vb1, vb2)), sort_bool::and_(vb1, vb2)));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), sort_bool::and_(vb1, sort_bool::and_(vb2, vb1)), sort_bool::and_(vb1, vb2)));
    //  a || a = a;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), sort_bool::or_(vb1, vb1), vb1));
    //  a => b = !a || b;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::implies(vb1, vb2), sort_bool::or_(sort_bool::not_(vb1), vb2)));
    // a && (!a || b) = a && b
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), sort_bool::and_(vb1, sort_bool::or_(sort_bool::not_(vb1), vb2)), sort_bool::and_(vb1, vb2)));
    // !a && (a || b) == !a && b
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), sort_bool::and_(sort_bool::not_(vb1), sort_bool::or_(vb1, vb2)), sort_bool::and_(sort_bool::not_(vb1), vb2)));
    // Pushing not inside
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::not_(sort_bool::and_(vb1, vb2)), sort_bool::or_(sort_bool::not_(vb1), sort_bool::not_(vb2))));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::not_(sort_bool::or_(vb1, vb2)), sort_bool::and_(sort_bool::not_(vb1), sort_bool::not_(vb2))));
    // Formulate all linear equalities with positive rhs: -1 * x_P <= -5   !(1 * x_P < 5)
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1, vp1, vp2),
      less_equal(vr1, sort_real::creal(sort_int::cneg(vp1), vp2)),
      sort_bool::not_(less(sort_real::times(real_minus_one(), vr1), sort_real::creal(sort_int::cint(sort_nat::cnat(vp1)), vp2)))));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1, vp1, vp2),
      less(vr1, sort_real::creal(sort_int::cneg(vp1), vp2)),
      sort_bool::not_(less_equal(sort_real::times(real_minus_one(), vr1), sort_real::creal(sort_int::cint(sort_nat::cnat(vp1)), vp2)))));
    // Eliminate nested -1 * -1
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1), sort_real::times(real_minus_one(), sort_real::times(real_minus_one(), vr1)), vr1));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1,vr2,vr3),
      sort_real::times(vr1, sort_real::plus(vr2,vr3)), sort_real::plus(sort_real::times(vr1,vr2), sort_real::times(vr1,vr3))));
    // Eliminate 1 * r
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1), sort_real::times(real_one(), vr1), vr1));

    // Rules for bidirectional implication (iff)
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), iff(sort_bool::true_(), vb1), vb1));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), iff(vb1, sort_bool::true_()), vb1));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), iff(sort_bool::false_(), vb1), sort_bool::not_(vb1)));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), iff(vb1, sort_bool::false_()), sort_bool::not_(vb1)));

    // if(a,b,c) = (a && b) || (!a && c);
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2,vb3), if_(vb1, vb2, vb3),
      sort_bool::or_(sort_bool::and_(vb1, vb2), sort_bool::and_(sort_bool::not_(vb1), vb3))));
    // (a && b) || !a = b || !a
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2),
      sort_bool::or_(sort_bool::and_(vb1,vb2), sort_bool::not_(vb1)), sort_bool::or_(vb2,sort_bool::not_(vb1))));
    // !a || (a && b) = !a || b
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2),
      sort_bool::or_(sort_bool::not_(vb1), sort_bool::and_(vb1,vb2)), sort_bool::or_(sort_bool::not_(vb1), vb2)));
    // a || (!a && b) = a || b
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2),
      sort_bool::or_(vb1, sort_bool::and_(sort_bool::not_(vb1), vb2)), sort_bool::or_(vb1,vb2)));

    rewr = rewriter(ad_hoc_data, strat);
  }

  variable_list replace_free_reals(data_expression& split_block,
    std::set< data_expression > linear_inequalities, data_expression& enumeration_condition, data_expression& lli)
  {
    set_identifier_generator var_gen;
    var_gen.add_identifiers(find_identifiers(split_block));
    variable_vector replacements;
    for(data_expression d: linear_inequalities)
    {
      variable rep_bool(var_gen("b"), sort_bool::bool_());
      replacements.push_back(rep_bool);
      std::cout << rep_bool << " will replace " << d << std::endl;
    }
    data_expression replaced_relation = sort_bool::true_();

    uint32_t i = 0;
    for(data_expression d: linear_inequalities)
    {
      split_block = replace_data_expressions(split_block, data_expression_assignment(d, replacements[i]), true);
      enumeration_condition = replace_data_expressions(enumeration_condition, data_expression_assignment(d, replacements[i]), true);
      if(linear_inequalities.size() > 1)
      {
        replaced_relation = lazy::and_(replaced_relation, iff(replacements[i], d));
        if(i == linear_inequalities.size() - 1)
        {
          lli = d;
        }
      }
      i++;
    }
    enumeration_condition = lazy::and_(enumeration_condition, replaced_relation);
    return variable_list(replacements.begin(), replacements.end());
  }

  struct enumerate_filter_print
  {
    data_expression last_linear_inequality;
    rewriter rewr;

    explicit enumerate_filter_print(const data_expression& lli, const rewriter& rewr_)
      : last_linear_inequality(lli)
      , rewr(rewr_)
    {}

    bool operator()(const data_expression& d)
    {
      data_expression conjunct = rewr(d);
      if(sort_bool::is_false_function_symbol(conjunct))
      {
        return false;
      }
      else if(sort_bool::right(conjunct) == last_linear_inequality || sort_bool::right(conjunct) == sort_bool::not_(last_linear_inequality))
      {
        // We assume now all variables have been filled in so we can
        // just walk over a conjuction to gather all the linear
        // inequalities
        std::vector< linear_inequality > lis;
        while(sort_bool::is_and_application(conjunct))
        {
          data_expression li_expr = sort_bool::right(conjunct);
          bool invert = false;
          if(sort_bool::is_not_application(li_expr))
          {
            li_expr = sort_bool::arg(li_expr);
            invert = true;
          }
          linear_inequality li = invert ? linear_inequality(li_expr, rewr).invert(rewr) : linear_inequality(li_expr, rewr);
          lis.push_back(li);
          conjunct = sort_bool::left(conjunct);
        }
        data_expression li_expr = conjunct;
        bool invert = false;
        if(sort_bool::is_not_application(li_expr))
        {
          li_expr = sort_bool::arg(li_expr);
          invert = true;
        }
        linear_inequality li = invert ? linear_inequality(li_expr, rewr).invert(rewr) : linear_inequality(li_expr, rewr);
        lis.push_back(li);
        return !is_inconsistent(lis, rewr, true);
      }
      return true;
    }
  };

  // For all evaluations of variables in vars for which block holds,
  // add split_block to the partition
  // vars is the set of free variables in split_block
  void enumerate(data_expression block, variable_list vars, data_expression split_block, const data_expression& lli)
  {

    typedef enumerator_algorithm_with_iterator<rewriter, enumerator_list_element_with_substitution<>, data::enumerator_identifier_generator, enumerate_filter_print> enumerator_type;
    data::enumerator_identifier_generator id_generator;
    enumerator_type enumerator(rewr, m_spec.data(), rewr, id_generator, 10000);

    std::cout << "Enumerating variables " << pp_container(vars) << " for which " << pp(block) << " holds in expression " << split_block << "." << std::endl;

    data::mutable_indexed_substitution<> sigma;
    std::deque<enumerator_list_element_with_substitution<> >
         enumerator_deque(1, enumerator_list_element_with_substitution<>(vars, block));
    enumerate_filter_print filter(lli, rewr);
    for (typename enumerator_type::iterator i = enumerator.begin(sigma, enumerator_deque, filter); i != enumerator.end(filter); ++i)
    {
      i->add_assignments(vars,sigma,rewr);

      std::cout << "enum: [";
      for (variable_list::const_iterator v=vars.begin(); v!=vars.end() ; ++v)
      {
        if (v!=vars.begin())
        {
          std::cout << ", ";
        }

        std::cout << data::pp(*v) << " := " << data::pp(sigma(*v));
      }
      std::cout << "] leads to " << std::endl;
      data_expression new_block(simpl.apply(split_block, sigma));
      std::cout << new_block << std::endl;
      partition.insert(new_block);
    }
  }

  void refine()
  {
    std::cout << "=====================================================" << std::endl;
    int i = 0;
    for(typename std::vector< action_summand_type >::const_iterator it = m_spec.process().action_summands().begin();
        it != m_spec.process().action_summands().end(); it++)
    {
      std::cout << "Summand " << i << " " << pp(*it) << std::endl;
      i++;
    }
    std::cout << "Partition:" << std::endl;
    i = 0;
    data_expression_vector indexed_blocks;
    for(std::set< data_expression >::const_iterator it = partition.cbegin(); it != partition.cend(); it++)
    {
      std::cout << "  block " << i << "  " << pp(*it) << std::endl;
      i++;
      indexed_blocks.push_back(*it);
    }
    std::cout << "Specify phi_k, phi_l, and summand index (separate with spaces): ";
    std::string s;
    std::getline(std::cin, s);

    int k = std::stoi(s.substr(0,s.find(' ')));
    s = s.substr(s.find(' ') + 1);
    int l = std::stoi(s.substr(0,s.find(' ')));
    s = s.substr(s.find(' ') + 1);
    int summ_i = std::stoi(s);
    data_expression phi_k = indexed_blocks[k];
    data_expression phi_l = indexed_blocks[l];
    std::cout <<
      "phi_k   " << phi_k << std::endl <<
      "phi_l   " << phi_l << std::endl <<
      "summand " << m_spec.process().action_summands()[summ_i] << std::endl;

    const lps::action_summand as = m_spec.process().action_summands()[summ_i];
    const lps::multi_action action = as.multi_action();
    substitution_t sub_primed;
    variable_list primed_summation_variables;
    for(const variable& v: as.summation_variables())
    {
      variable var(static_cast<std::string>(v.name()) + std::string("'"), v.sort());
      primed_summation_variables.push_front(var);
      sub_primed[v] = var;
    }
    primed_summation_variables = reverse(primed_summation_variables);
    variable_list primed_process_parameters;
    for(const variable& v: process_parameters)
    {
      variable var(static_cast<std::string>(v.name()) + std::string("'"), v.sort());
      primed_process_parameters.push_front(var);
      sub_primed[v] = var;
    }
    primed_process_parameters = reverse(primed_process_parameters);

    data_expression_list updates;
    for(variable_list::const_iterator it = process_parameters.begin(); it != process_parameters.end(); it++)
    {
      assignment_list::const_iterator assign = std::find_if(as.assignments().begin(), as.assignments().end(), [&it](const assignment& arg) {return arg.lhs() == *it;});
      if(assign != as.assignments().end())
      {
        updates.push_front(assign->rhs());
      }
      else
      {
        updates.push_front(*it);
      }
    }
    updates = reverse(updates);

    data_expression arguments_equal = sort_bool::true_();
    for(const data_expression& expr: action.arguments())
    {
      arguments_equal = lazy::and_(arguments_equal, equal_to(expr, rewr(expr, sub_primed)));
    }
    std::cout << "Constructing block expression ..." << std::endl;
    data_expression split_block =
      lambda(process_parameters,
        lazy::and_(
          application(phi_k,process_parameters),
          lazy::and_(
            sort_bool::not_(exists(primed_summation_variables,
              sort_bool::not_(lazy::implies(
                rewr(lazy::and_(
                  replace_variables(as.condition(), sub_primed),
                  application(phi_l, replace_variables(updates, sub_primed))
                )),
                exists(as.summation_variables(),
                  rewr(lazy::and_(
                    lazy::and_(
                      as.condition(),
                      rewr(application(phi_l, updates))),
                    arguments_equal
                  ))
                )
              ))
            )),
            sort_bool::not_(exists(as.summation_variables(),
              sort_bool::not_(lazy::implies(
                rewr(lazy::and_(
                  as.condition(),
                  rewr(application(phi_l, updates))
                )),
                exists(primed_summation_variables,
                  rewr(lazy::and_(
                    lazy::and_(
                      replace_variables(as.condition(), sub_primed),
                      application(phi_l, replace_variables(updates, sub_primed))
                    ),
                    arguments_equal
                  ))
                )
              ))
            ))
          )
        )
      );

    std::cout << "Starting simplification of " << std::endl << pp(split_block) << std::endl;
    split_block = one_point_rule_rewrite(quantifiers_inside_rewrite(split_block));
    std::cout << "Quantifiers pushed inside " << std::endl << pp(split_block) << std::endl;

    data_expression result = replace_data_expressions(split_block, eliminate_existential_quantifier_sigma(rewr), true);
    std::cout << "Block after replacements:" << std::endl << result << std::endl;
    split_block = rewr(result);
    std::cout << "Simplified block: " << std::endl << split_block << std::endl;
    std::set<variable> free_vars = find_free_variables(split_block);
    variable_list free_vars_list;
    std::cout << "Free variables: {";
    std::set< variable > real_free_vars;
    for(const variable& v: free_vars)
    {
      std::cout << v << ", ";
      if(v.sort() == sort_real::real_())
      {
        real_free_vars.insert(v);
      }
      else
      {
        free_vars_list.push_front(v);
      }
    }
    std::cout << "}" << std::endl;

    data_expression enumeration_condition(rewr(application(phi_k,primed_process_parameters)));
    data_expression lli(sort_bool::true_());
    if(real_free_vars.size() > 0)
    {
      std::set< data_expression > li = find_linear_inequalities(split_block, real_free_vars);
      std::cout << "Linear inequalities with free variables: " << pp_container(li) << std::endl;
      free_vars_list = free_vars_list + replace_free_reals(split_block, li, enumeration_condition, lli);
    }

    partition.erase(partition.find(phi_k));
    std::cout << "Starting enumeration..." << std::endl;
    enumerate(enumeration_condition, free_vars_list, split_block, lli);
  }

  data_expression find_initial_block()
  {
    for(std::set< data_expression >::const_iterator it = partition.begin(); it != partition.end(); it++)
    {
      if(rewr(application(*it, m_spec.initial_process().state(process_parameters))) == sort_bool::true_())
      {
        std::cout << "Found initial block " << *it << std::endl;
        return *it;
      }
    }
    throw mcrl2::runtime_error("Initial block not found");
  }

public:
  symbolic_bisim_algorithm(Specification& spec, data_expression inv,
    bool use_path_eliminator, detail::smt_solver_type solver_type, const rewrite_strategy st = jitty)
    : mcrl2::lps::detail::lps_algorithm<Specification>(spec)
    , strat(st)
    , rewr(spec.data(),strat)
    , proving_rewr(spec.data(), jitty_prover)
    , invariant(inv)
    , m_use_path_eliminator(use_path_eliminator)
    , m_path_eliminator(solver_type)
    // , simpl(rewr, proving_rewr, process_parameters)
    , simpl(rewr, proving_rewr)
  {

  }

  void run()
  {
    mCRL2log(mcrl2::log::verbose) << "Running symbolic bisimulation..." << std::endl;
    process_parameters = m_spec.process().process_parameters();
    partition.insert( lambda(process_parameters, invariant));
    add_ad_hoc_rules();
    // simpl = simplifier_dbm(rewr, proving_rewr, process_parameters);
    simpl = simplifier_fourier_motzkin(rewr, proving_rewr);

    while(true)
    {
      refine();
      find_initial_block();
    }
  }
};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H
