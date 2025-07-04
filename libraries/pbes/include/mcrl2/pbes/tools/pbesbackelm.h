// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesbackelm.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs, 
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables. 

#ifndef MCRL2_PBES_TOOLS_PBESBACKELM_H
#define MCRL2_PBES_TOOLS_PBESBACKELM_H

#include "mcrl2/pbes/algorithms.h"
// #include "mcrl2/pbes/replace_capture_avoiding.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/io.h"

namespace mcrl2::pbes_system
{

struct pbesbackelm_options
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
  using super = Builder<substitute_propositional_variables_builder<Builder>>;
  using super::apply;


  pbes_equation m_eq;
  core::identifier_string name;
  simplify_quantifiers_data_rewriter<data::rewriter> m_pbes_rewriter;
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
        mCRL2log(log::verbose) << "Replaced in PBES equation for " << name << ":\n" << x << " --> " << p << "\n";
        result=p;
        m_stable=false;
        return;
      }
      mCRL2log(log::debug) << "No Replacement in PBES equation for " << name << ":\n" << x << " --> " << p << "\n";
      result=x;
      return;
    }
    result=x;
  }
};

inline
void self_substitute(pbes_equation& equation, substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter)
{
  substituter.set_stable(false);
  while (!substituter.stable())
  {
    substituter.set_equation(equation);
    substituter.set_name(equation.variable().name());
    substituter.set_stable(true);
    pbes_expression p;
    substituter.apply(p, equation.formula());
    equation.formula()=p;
  }
}

inline
void substitute(pbes_equation& into, const pbes_equation& by, substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter)
{
  // substitute_propositional_variables_builder<pbes_system::pbes_expression_builder> substituter(by,r);
  substituter.set_equation(by);
  substituter.set_name(into.variable().name());
  pbes_expression p;
  substituter.apply(p, into.formula());
  into.formula()=p;
}

struct pbesbackelm_pbes_backward_substituter
{
  void run(pbes& p,
           pbesbackelm_options options
         )
  {
    data::rewriter data_rewriter(p.data(),options.rewrite_strategy);
    simplify_quantifiers_data_rewriter<data::rewriter> pbes_rewriter(data_rewriter);
    substitute_propositional_variables_builder<pbes_system::pbes_expression_builder> substituter(pbes_rewriter);
    for(std::vector<pbes_equation>::reverse_iterator i=p.equations().rbegin(); i!=p.equations().rend(); i++)
    {
      // Simplify the equation *i by substituting in itself. 
      mCRL2log(log::verbose) << "Investigating the equation for " <<  i->variable().name() << "\n";
      self_substitute(*i,substituter);
      for(std::vector<pbes_equation>::reverse_iterator j=i+1; j!=p.equations().rend(); j++)
      {
        substitute(*j, *i, substituter);
      }
    }
    pbes_rewrite(p, pbes_rewriter);
  }
};

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_TOOLS_PBESBACKELM_H
