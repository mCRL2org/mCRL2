// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesnucheck.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs, 
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables. 

#ifndef MCRL2_PBES_TOOLS_PBESNUCHECK_H
#define MCRL2_PBES_TOOLS_PBESNUCHECK_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/rewriters/pbes2data_rewriter.h"
#include "mcrl2/data/detail/prover/bdd_prover.h"

namespace mcrl2 {

namespace pbes_system {

struct pbesnucheck_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
};


/* template <template <class> class Builder, class Substitution>
struct substitute_propositional_variables_builder: public Builder<substitute_propositional_variables_builder<Builder, Substitution> >
{
  typedef Builder<substitute_propositional_variables_builder<Builder, Substitution> > super;
  using super::apply; */
template <template <class> class Builder>
struct substitute_propositional_variables_builder: public Builder<substitute_propositional_variables_builder<Builder> >
{
  typedef Builder<substitute_propositional_variables_builder<Builder> > super;
  using super::apply;


  pbes_equation m_eq;
  core::identifier_string name;
  simplify_quantifiers_data_rewriter<data::rewriter> m_pbes_rewriter;
  data::data_specification data_spec;
  bool m_stable=false;

  explicit substitute_propositional_variables_builder(simplify_quantifiers_data_rewriter<data::rewriter>& r)
    : m_pbes_rewriter(r)
  {}

  void set_stable(bool b)
  {
    m_stable=b;
  }

  bool stable() const
  {
    return m_stable;
  }

  void set_equation(const pbes_equation& eq)
  {
    m_eq=eq;
  }
 
  void set_name(const core::identifier_string& s)
  {
    name=s;
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (x.name()==m_eq.variable().name())
    {
      data::mutable_indexed_substitution sigma;
      data::data_expression_list pars=x.parameters();
      for(const data::variable& v: m_eq.variable().parameters())
      {
        data::data_expression par=pars.front();
        pars.pop_front();
        if (par.sort()!=v.sort())
        {
          // Parameters do not match with variables. Ignore this substitution.
          result=x;
          return;
        }
        sigma[v]=par;
      }
      pbes_expression p=pbes_rewrite(m_eq.formula(),m_pbes_rewriter,sigma);
      std::set<propositional_variable_instantiation> set=find_propositional_variable_instantiations(p);
      if (std::all_of(set.begin(),
                      set.end(),
                      [this](const propositional_variable_instantiation& v){ return v.name()!=m_eq.variable().name(); }))
      {
        // The result does not contain the variable m_eq.variable().name() and is therefore considered simpler. 
//        mCRL2log(log::verbose) << "Replaced in PBES equation for " << name << ":\n" << x << " --> " << p << "\n";
        result=p;
        m_stable=false;
        return;
      }
//      mCRL2log(log::debug) << "No Replacement in PBES equation for " << name << ":\n" << x << " --> " << p << "\n";
      result=x;
      return;
    }
    result=x;
  }
};
//
// bool is_true(data::data_expression expr)
//    {
//      if(m_solver != nullptr)
//      {
//        bool negate = false;
//        if(data::is_forall(expr))
//        {
//          negate = true;
//          const data::forall& f = atermpp::down_cast<data::forall>(expr);
//          expr = data::make_exists_(f.variables(), data::sort_bool::not_(f.body()));
//        }
//        // data::data_expression result = data::one_point_rule_rewrite(m_rewr(expr));
//        switch(m_solver->solve(data::variable_list(), expr, m_options.smt_timeout))
//        {
//          case smt::answer::SAT: return negate ^ true;
//          case smt::answer::UNSAT: return negate ^ false;
//          case smt::answer::UNKNOWN: return false;
//        }
//      }
//      else
//      {
//        data::data_expression result = m_rewr(data::one_point_rule_rewrite((m_rewr(expr))));
//        if (result != data::sort_bool::true_() && result != data::sort_bool::false_())
//        {
//          mCRL2log(log::verbose) << "Cannot rewrite " << result << " any further" << std::endl;
//        }
//        return result == data::sort_bool::true_();
//      }
//      // This code is unreachable, but necessary to silence a compiler warning
//      return false;
//    }

void nu_iteration(pbes_equation& equation, substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter, data::data_specification data_spec)
{
    mcrl2::data::detail::BDD_Prover f_bdd_prover(data_spec, data::used_data_equation_selector(data_spec));
//  A pbes expression expressing the value true
pbes_equation eq;
eq.formula()=true_();
eq.variable()=equation.variable();
bool stable=false;
int i = 0;
while (!stable && i < 20) {
//    pbes_expression sigma;
//equation.formula()=sigma;
    substituter.set_equation(eq);
    substituter.set_name(equation.variable().name());
    pbes_expression p;
    substituter.apply(p, equation.formula());
  data::data_expression eq_data =atermpp::down_cast<data::data_expression>( detail::pbes2data(eq.formula()));
  data::data_expression p_data =atermpp::down_cast<data::data_expression>( detail::pbes2data(p));
//      mCRL2log(log::debug) << "Equivalent? " << (eq.formula() == p) << "\n";
//      mCRL2log(log::debug) << " --- " << (eq.formula()) << "\n";
//      mCRL2log(log::debug) << " --- " << (p) << "\n";
      f_bdd_prover.set_formula(data::and_(data::imp(eq_data,p_data),data::imp(p_data,eq_data)));
        data::detail::Answer v_is_tautology = f_bdd_prover.is_tautology();
        data::detail::Answer v_is_contradiction = f_bdd_prover.is_contradiction();
        if (v_is_tautology == data::detail::answer_yes)
        {
          mCRL2log(log::info) << "Tautology" << std::endl;
          stable=true;
          return;
        }
        else if (v_is_contradiction == data::detail::answer_yes)
        {
          mCRL2log(log::info) << "Contradiction" << std::endl;
        }
        else
        {
          mCRL2log(log::info) << "Undeterminable" << std::endl;
//          print_counter_example();
//          print_witness();
//          save_dot_file(v_formula_number);
        }
    eq.formula()=p;
    i++;
    }
    equation.formula()=eq.formula();
}

struct pbesnucheck_pbes_backward_substituter
{
  void run(pbes& p,
           pbesnucheck_options options
         )
  {
    data::rewriter data_rewriter(p.data(),options.rewrite_strategy);
    simplify_quantifiers_data_rewriter<data::rewriter> pbes_rewriter(data_rewriter);
    substitute_propositional_variables_builder<pbes_system::pbes_expression_builder> substituter(pbes_rewriter);
    for(std::vector<pbes_equation>::reverse_iterator i=p.equations().rbegin(); i!=p.equations().rend(); i++)
    {
      // Simplify the equation *i by substituting in itself. 
      mCRL2log(log::verbose) << "Investigating the equation for " <<  i->variable().name() << "\n";
      nu_iteration(*i,substituter,p.data());
//      for(std::vector<pbes_equation>::reverse_iterator j=i+1; j!=p.equations().rend(); j++)
//      {
//        substitute(*j, *i, substituter);
//      }
    }
    pbes_rewrite(p, pbes_rewriter);
  }
};

/// \brief Load formula from the file mentioned in infilename, if infilename
    ///        is empty, use std::cin.
    /// \param infilename The name of the input file
    /// \return The formula stored in infilename
//    data::data_expression load_formula(const std::string& infilename, data::data_specification& specification)
//    {
//      if (infilename.empty())
//      {
//        return parse_data_expression(std::cin, specification);
//      }
//      else
//      {
//        std::ifstream instream(infilename.c_str());
//        if (!instream.is_open())
//        {
//          throw mcrl2::runtime_error("Cannot open input file");
//        }
//
////  mCRL2log(log::debug) << "is expression" << mcrl2::pbes_system::detail::parse_pbes_expression("forall col_P1: Colour,col_P2: Colour. (val(false) => col_P1 == col_P2)");
//        data::data_expression formula = parse_data_expression("forall col_P1: Colour,col_P2: Colour. (val(false) => col_P1 == col_P2)", specification);
//        instream.close();
//
//        return formula;
//      }
//    }

void pbesnucheck(const std::string& input_filename,
             const std::string& output_filename,
             const utilities::file_format& input_format,
             const utilities::file_format& output_format,
             pbesnucheck_options options
            )
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  complete_data_specification(p);
  algorithms::normalize(p);
  pbesnucheck_pbes_backward_substituter backward_substituter;
//  data::data_expression formula = load_formula(input_filename, p.data());
//  mCRL2log(log::debug) << "nu_iteration: formula=" << formula;
  backward_substituter.run(p, options);
  save_pbes(p, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESnucheck_H
