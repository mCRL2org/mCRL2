// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/bisimulation.h
/// \brief Bisimulation algorithms.

#ifndef MCRL2_PBES_BISIMULATION_H
#define MCRL2_PBES_BISIMULATION_H

#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/atermpp/detail/aterm_list_utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/replace.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/detail/lps2pbes_utility.h"
#include "mcrl2/pbes/join.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/number_postfix_generator.h"
#include <algorithm>
#include <set>
#include <sstream>
#include <vector>

namespace mcrl2
{

namespace pbes_system
{

using atermpp::detail::operator+;

/// \brief Base class for bisimulation algorithms.
class bisimulation_algorithm
{
  public:
    /// \brief The iterator type for non-delta summands
    typedef lps::action_summand_vector::const_iterator my_iterator;

  protected:
    /// \brief A map type for mapping summands to strings.
    typedef std::map<const lps::action_summand*, std::string> name_map;

    /// \brief Maps summands to strings.
    name_map summand_names;

    /// \brief Store the address of the model.
    const lps::linear_process* model_ptr;

    /// \brief Store the address of the specification.
    const lps::linear_process* spec_ptr;

    /// \brief Generates a name for an action_list.
    /// \param l A sequence of actions
    /// \return A string representation of the list \p l
    std::string action_list_name(const process::action_list& l) const
    {
      std::ostringstream out;
      for (auto i = l.begin(); i != l.end(); ++i)
      {
        out << (i != l.begin() ? "-" : "") << std::string(i->label().name());
      }
      std::string result = out.str();
      if (result == "")
      {
        result = "tau";
      }
      return result;
    }

    /// \brief Returns the name of a summand
    /// \param i A summand iterator
    /// \return The name of the summand referred to by \p i
    std::string summand_name(my_iterator i) const
    {
      const lps::action_summand* t = &(*i);
      name_map::const_iterator j = summand_names.find(t);
      assert(j != summand_names.end());
      return j->second;
    }

    /// \brief Returns true if p is the linear process of the model.
    /// \param p A linear process
    /// \return True if p is the linear process of the model.
    bool is_from_model(const lps::linear_process& p) const
    {
      return &p == model_ptr;
    }

    /// \brief Returns a name of a linear process.
    /// \param p A linear process
    /// \return The name of the linear process.
    std::string process_name(const lps::linear_process& p) const
    {
      if (is_from_model(p))
      {
        return "m";
      }
      else
      {
        return "s";
      }
    }

    /// \brief Used for initializing summand names.
    /// \param p A linear process
    void set_summand_names(const lps::linear_process& p)
    {
      data::set_identifier_generator generator;
      for (const lps::action_summand& s: p.action_summands())
      {
        std::string name = generator(action_list_name(s.multi_action().actions()));
        summand_names[&s] = name;
      }
    }

    // creates the substitution v[i] := e[i]
    // pre: v.size() == e.size()
    void make_substitution(const data::variable_list& v, const data::data_expression_list& e, data::mutable_map_substitution<>& result) const
    {
      assert(v.size() == e.size());
      auto vi = v.begin();
      auto ei = e.begin();
      for (; vi != v.end(); ++vi, ++ei)
      {
        result[*vi] = *ei;
      }
    }

  public:
    /// \brief Creates a name for the propositional variable Xpq
    /// \param p A linear process
    /// \param q A linear process
    /// \return The name for the propositional variable Xpq
    core::identifier_string X(const lps::linear_process& p, const lps::linear_process& q) const
    {
      std::string s = "X" + process_name(p) + process_name(q);
      return core::identifier_string(s);
    }

    /// \brief Creates a name for the propositional variable Ypq
    /// \param p A linear process
    /// \param q A linear process
    /// \return The name for the propositional variable Ypq
    core::identifier_string Y(const lps::linear_process& p, const lps::linear_process& q) const
    {
      std::string s = "Y" + process_name(p) + process_name(q);
      return core::identifier_string(s);
    }

    /// \brief Creates a name for the propositional variable Ypqi
    /// \pre The iterator i must be in p.action_summands().
    /// \param p A linear process
    /// \param q A linear process
    /// \param i A summand iterator
    /// \return The name for the propositional variable Ypqi
    core::identifier_string Y(const lps::linear_process& p, const lps::linear_process& q, my_iterator i) const
    {
      std::string s = "Y" + process_name(p) + process_name(q) + "_" + summand_name(i);
      return core::identifier_string(s);
    }

    /// \brief Creates a name for the propositional variable Y1pqi
    /// \pre The iterator i must be in p.action_summands().
    /// \param p A linear process
    /// \param q A linear process
    /// \param i A summand iterator
    /// \return The name for the propositional variable Y1pqi
    core::identifier_string Y1(const lps::linear_process& p, const lps::linear_process& q, my_iterator i) const
    {
      std::string s = "Y1" + process_name(p) + process_name(q) + "_" + summand_name(i);
      return core::identifier_string(s);
    }

    /// \brief Creates a name for the propositional variable Y2pqi
    /// \pre The iterator i must be in p.action_summands().
    /// \param p A linear process
    /// \param q A linear process
    /// \param i A summand iterator
    /// \return The name for the propositional variable Y2pqi
    core::identifier_string Y2(const lps::linear_process& p, const lps::linear_process& q, my_iterator i) const
    {
      std::string s = "Y2" + process_name(p) + process_name(q) + "_" + summand_name(i);
      return core::identifier_string(s);
    }

    /// \brief Creates a propositional variable.
    /// \param name A
    /// \param parameters A sequence of data variables
    /// \return The created propositional variable
    propositional_variable_instantiation var(const core::identifier_string& name, data::variable_list const& parameters) const
    {
      return propositional_variable_instantiation(name, data::data_expression_list(parameters));
    }

    /// \brief Creates a propositional variable.
    /// \param name A
    /// \param parameters A sequence of data expressions
    /// \return The created propositional variable
    propositional_variable_instantiation var(const core::identifier_string& name, data::data_expression_list const& parameters) const
    {
      return propositional_variable_instantiation(name, parameters);
    }

    /// \brief Returns a pbes expression that expresses equality of the multi-actions a and b.
    /// \param a A sequence of actions
    /// \param b A sequence of actions
    /// \return Necessary conditions for the equality of a and b
    pbes_expression equals(const lps::multi_action& a, const lps::multi_action& b) const
    {
      return lps::equal_multi_actions(a, b);
    }

    /// \brief Returns the fixpoint symbol mu.
    /// \return The fixpoint symbol mu.
    fixpoint_symbol mu() const
    {
      return fixpoint_symbol::mu();
    }

    /// \brief Returns the fixpoint symbol nu.
    /// \return The fixpoint symbol nu.
    fixpoint_symbol nu() const
    {
      return fixpoint_symbol::nu();
    }

    /// \brief Returns a substitution of variables in q such that there are no name clashes
    /// between p and q.
    /// \param p A linear process specification
    /// \param q A linear process specification
    /// \return A substitution that should be applied to q to remove name clashes between p and q.
    /// \details After this substitution the following holds:
    /// \f[ ((param(p)\cup glob(p))\cap ((param(q)\cup glob(q))=\emptyset \f]
    /// where param(p) denotes p.process().process_parameters() and glob(p) denotes p.global_variables().
    data::mutable_map_substitution<> compute_process_parameter_name_clashes(const lps::specification& p, const lps::specification& q) const
    {
       data::mutable_map_substitution<> result;

      // put the names of variables appearing in p and q in an identifier generator
      std::set<data::variable> context = lps::find_all_variables(p);
      std::set<data::variable> vars = lps::find_all_variables(q);
      context.insert(vars.begin(), vars.end());
      data::set_identifier_generator generator;
      for (const data::variable& v: context)
      {
        generator.add_identifier(v.name());
      }

      // generate renamings for variables appearing in qvars
      for (const data::variable& w: q.process().process_parameters())
      {
        data::variable v(generator(w.name()), w.sort());
        if (v != w)
        {
          result[w] = v;
        }
      }
      return result;
    }

    /// \brief Returns a substitution of variables in q such that there are no name clashes
    /// between the summation variables of p and q.
    /// \param p A linear process specification
    /// \param q A linear process specification
    /// \return A substitution that should be applied to q to remove name clashes between p and q.
    data::mutable_map_substitution<> compute_summand_variable_name_clashes(const lps::specification& p, const lps::specification& q) const
    {
      data::mutable_map_substitution<> result;

      // put the names of variables appearing in p and q in an identifier generator
      std::set<data::variable> context = lps::find_all_variables(p);
      std::set<data::variable> vars = lps::find_all_variables(q);
      context.insert(vars.begin(), vars.end());
      data::set_identifier_generator generator;
      for (const data::variable& v: context)
      {
        generator.add_identifier(v.name());
      }

      // put the summation variables of q in qvars
      std::set<data::variable> qvars;
      for (const lps::action_summand& s: q.process().action_summands())
      {
        const data::variable_list& v = s.summation_variables();
        qvars.insert(v.begin(), v.end());
      }
      for (const lps::deadlock_summand& s: q.process().deadlock_summands())
      {
        const data::variable_list& v = s.summation_variables();
        qvars.insert(v.begin(), v.end());
      }

      // generate renamings for variables appearing in qvars
      for (const data::variable& w: qvars)
      {
        data::variable v(generator(w.name()), w.sort());
        if (v != w)
        {
          result[w] = v;
        }
      }
      return result;
    }

    /// \brief Resolves name clashes between model and spec.
    void resolve_name_clashes(const lps::specification& model, lps::specification& spec, bool include_summand_variables = false)
    {
      data::mutable_map_substitution<> sigma = compute_process_parameter_name_clashes(model, spec);
      lps::replace_process_parameters(spec, sigma);
      if (include_summand_variables)
      {
        sigma = compute_summand_variable_name_clashes(model, spec);
        lps::replace_summand_variables(spec, sigma);
      }
      mCRL2log(log::debug) << "bisimulation spec after resolving name clashes:\n" << lps::pp(spec) << std::endl;
    }

    /// \brief Initializes the name lookup table.
    /// \param model A linear process
    /// \param spec A linear process
    /// \pre model and spec must have the same data specification
    void init(const lps::linear_process& model, const lps::linear_process& spec)
    {
      summand_names.clear();
      set_summand_names(model);
      set_summand_names(spec);
      model_ptr = &model;
      spec_ptr  = &spec;
      assert(is_from_model(model));
      assert(!is_from_model(spec));
    }

    /// \brief Builds a pbes from the given equations.
    /// \param equations A sequence of pbes equations
    /// \param M A specification
    /// \param S A specification
    /// \return The constructed pbes
    pbes build_pbes(const std::vector<pbes_equation>& equations,
                    const lps::specification& M,
                    const lps::specification& S
                   )
    {
      const lps::linear_process& m = M.process();
      const lps::linear_process& s = S.process();

      propositional_variable_instantiation init(X(m, s), M.initial_process().state(M.process().process_parameters()) + S.initial_process().state(S.process().process_parameters()));

      pbes result(M.data(), equations, init);
      assert(result.is_closed());
      return result;
    }
};

//--------------------------------------------------------------//
//                 branching bisimulation
//--------------------------------------------------------------//

/// \brief Algorithm class for branching bisimulation.
class branching_bisimulation_algorithm : public bisimulation_algorithm
{
  public:
    /// \brief The match function.
    /// \param p A linear process
    /// \param q A linear process
    /// \return The function result
    pbes_expression match(const lps::linear_process& p, const lps::linear_process& q) const
    {
      std::vector<pbes_expression> result;
      for (auto i = p.action_summands().begin(); i != p.action_summands().end(); ++i)
      {
        data::data_expression ci = i->condition();
        const data::variable_list& d  = p.process_parameters();
        data::variable_list e  = i->summation_variables();
        const data::variable_list& d1 = q.process_parameters();
        pbes_expression expr = make_forall(e, optimized_imp(ci, var(Y(p, q, i), d + d1 + e)));
        result.push_back(expr);
      }
      return optimized_join_and(result.begin(), result.end());
    }

    /// \brief The step function.
    /// \param p A linear process
    /// \param q A linear process
    /// \param i A summand iterator
    /// \return The function result
    pbes_expression step(const lps::linear_process& p, const lps::linear_process& q, my_iterator i) const
    {
      const data::variable_list& d1 = q.process_parameters();
      data::data_expression_list gi = i->next_state(p.process_parameters());
      if (i->is_tau())
      {
        std::vector<pbes_expression> v;
        for (auto j = q.action_summands().begin(); j != q.action_summands().end(); ++j)
        {
          if (!j->is_tau())
          {
            continue;
          }
          data::data_expression cj = j->condition();
          data::variable_list e1 = j->summation_variables();
          data::data_expression_list gj = j->next_state(q.process_parameters());
          pbes_expression expr = make_exists(e1, optimized_and(cj, var(X(p, q), gi + gj)));
          v.push_back(expr);
        }
        return optimized_or(optimized_join_or(v.begin(), v.end()), var(X(p, q), gi + d1));
      }
      else
      {
        std::vector<pbes_expression> v;
        for (auto j = q.action_summands().begin(); j != q.action_summands().end(); ++j)
        {
          data::data_expression cj = j->condition();
          data::variable_list e1 = j->summation_variables();
          data::data_expression_list gj = j->next_state(q.process_parameters());
          lps::multi_action ai = i->multi_action();
          lps::multi_action aj = j->multi_action();
          pbes_expression expr = make_exists(e1, optimized_and(optimized_and(cj, equals(ai, aj)), var(X(p, q), gi + gj)));
          v.push_back(expr);
        }
        return optimized_join_or(v.begin(), v.end());
      }
    }

    /// \brief The close function.
    /// \param p A linear process
    /// \param q A linear process
    /// \param i A summand iterator
    /// \return The function result
    pbes_expression close(const lps::linear_process& p, const lps::linear_process& q, my_iterator i) const
    {
      std::vector<pbes_expression> v;
      const data::variable_list& d  = p.process_parameters();
      const data::variable_list& d1 = q.process_parameters();
      data::variable_list e  = i->summation_variables();
      for (auto j = q.action_summands().begin(); j != q.action_summands().end(); ++j)
      {
        if (!j->is_tau())
        {
          continue;
        }
        data::data_expression cj = j->condition();
        data::variable_list e1 = j->summation_variables();
        data::data_expression_list  gj = j->next_state(q.process_parameters());
        pbes_expression expr = make_exists(e1, optimized_and(cj, var(Y(p, q, i), data::data_expression_list(d) + gj + data::data_expression_list(e))));
        v.push_back(expr);
      }
      return optimized_or(optimized_join_or(v.begin(), v.end()), optimized_and(var(X(p, q), d + d1), step(p, q, i)));
    }

    /// \brief Returns a pbes that expresses branching bisimulation between
    /// two specifications.
    /// \param model A linear process specification
    /// \param spec A linear process specification
    /// \return A pbes that expresses branching bisimulation between the
    /// two specifications.
    pbes run(const lps::specification& model, const lps::specification& spec)
    {
      // resolve name clashes, and merge the data specifications of model and spec
      data::data_specification dataspec = data::merge_data_specifications(model.data(), spec.data());
      lps::specification spec1 = spec;
      lps::specification model1 = model;
      resolve_name_clashes(model1, spec1, true);
      model1.data() = dataspec;
      spec1.data() = dataspec;
      lps::normalize_sorts(model1, model1.data());
      lps::normalize_sorts(spec1, spec1.data());

      const lps::linear_process& m = model1.process();
      const lps::linear_process& s = spec1.process();
      init(m, s);

      const data::variable_list& d  = m.process_parameters();
      const data::variable_list& d1 = s.process_parameters();
      std::vector<pbes_equation> equations;


      // E1
      equations.push_back(pbes_equation(nu(), propositional_variable(X(m, s), d + d1), optimized_and(match(m, s), match(s, m))));
      equations.push_back(pbes_equation(nu(), propositional_variable(X(s, m), d1 + d), var(X(m, s), d + d1)));

      // E2
      for (auto i = m.action_summands().begin(); i != m.action_summands().end(); ++i)
      {
        data::variable_list e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y(m, s, i), d + d1 + e), close(m, s, i));
        equations.push_back(e1);
      }
      for (auto i = s.action_summands().begin(); i != s.action_summands().end(); ++i)
      {
        data::variable_list e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y(s, m, i), d1 + d + e), close(s, m, i));
        equations.push_back(e1);
      }

      return build_pbes(equations, model1, spec1);
    }
};

/// \brief Returns a pbes that expresses branching bisimulation between two specifications.
/// \param model A linear process specification
/// \param spec A linear process specification
/// \return A pbes that expresses branching bisimulation between the two specifications.
inline
pbes branching_bisimulation(const lps::specification& model, const lps::specification& spec)
{
  return branching_bisimulation_algorithm().run(model, spec);
}

//--------------------------------------------------------------//
//                 strong bisimulation
//--------------------------------------------------------------//

/// \brief Algorithm class for strong bisimulation.
class strong_bisimulation_algorithm : public bisimulation_algorithm
{
  public:
    /// \brief The match function.
    /// \param p A linear process
    /// \param q A linear process
    /// \return The function result
    pbes_expression match(const lps::linear_process& p, const lps::linear_process& q) const
    {
      std::vector<pbes_expression> result;
      for (auto i = p.action_summands().begin(); i != p.action_summands().end(); ++i)
      {
        data::data_expression ci = i->condition();
        data::variable_list e = i->summation_variables();
        pbes_expression  expr = make_forall(e, optimized_imp(ci, step(p, q, i)));
        result.push_back(expr);
      }
      return optimized_join_and(result.begin(), result.end());
    }

    /// \brief The step function.
    /// \param p A linear process
    /// \param q A linear process
    /// \param i A summand iterator
    /// \return The function result
    pbes_expression step(const lps::linear_process& p, const lps::linear_process& q, my_iterator i) const
    {
      data::data_expression_list gi = i->next_state(p.process_parameters());

      std::vector<pbes_expression> result;
      for (auto j = q.action_summands().begin(); j != q.action_summands().end(); ++j)
      {
        data::data_expression cj = j->condition();
        data::variable_list e1 = j->summation_variables();
        data::data_expression_list gj = j->next_state(q.process_parameters());
        lps::multi_action ai = i->multi_action();
        lps::multi_action aj = j->multi_action();
        pbes_expression expr = make_exists(e1, optimized_and(optimized_and(cj, equals(ai, aj)), var(X(p, q), gi + gj)));
        result.push_back(expr);
      }
      return optimized_join_or(result.begin(), result.end());
    }

    /// \brief Runs the algorithm
    /// \param model A linear process specification
    /// \param spec A linear process specification
    /// \return A pbes that expresses strong bisimulation between stwo specifications.
    pbes run(const lps::specification& model, const lps::specification& spec)
    {
      lps::specification spec1 = spec;
      resolve_name_clashes(model, spec1, true);
      const lps::linear_process& m = model.process();
      const lps::linear_process& s = spec1.process();
      init(m, s);

      const data::variable_list& d  = m.process_parameters();
      const data::variable_list& d1 = s.process_parameters();
      std::vector<pbes_equation> equations;


      // E
      equations.push_back(pbes_equation(nu(), propositional_variable(X(m, s), d + d1), optimized_and(match(m, s), match(s, m))));
      equations.push_back(pbes_equation(nu(), propositional_variable(X(s, m), d1 + d), var(X(m, s), d + d1)));

      return build_pbes(equations, model, spec1);
    }
};

/// \brief Returns a pbes that expresses strong bisimulation between two specifications.
/// \param model A linear process specification
/// \param spec A linear process specification
/// \return A pbes that expresses strong bisimulation between the two specifications.
inline
pbes strong_bisimulation(const lps::specification& model, const lps::specification& spec)
{
  return strong_bisimulation_algorithm().run(model, spec);
}

//--------------------------------------------------------------//
//                 weak bisimulation
//--------------------------------------------------------------//

/// \brief Algorithm class for weak bisimulation.
class weak_bisimulation_algorithm : public bisimulation_algorithm
{
  protected:
    mutable data::set_identifier_generator m_generator;

  public:
    /// \brief The match function.
    /// \param p A linear process
    /// \param q A linear process
    /// \return The function result
    pbes_expression match(const lps::linear_process& p, const lps::linear_process& q) const
    {
      std::vector<pbes_expression> result;
      for (auto i = p.action_summands().begin(); i != p.action_summands().end(); ++i)
      {
        data::data_expression ci = i->condition();
        const data::variable_list& d  = p.process_parameters();
        data::variable_list e  = i->summation_variables();
        const data::variable_list& d1 = q.process_parameters();
        pbes_expression expr = make_forall(e, optimized_imp(ci, var(Y1(p, q, i), d + d1 + e)));
        result.push_back(expr);
      }
      return optimized_join_and(result.begin(), result.end());
    }

    /// \brief The step function.
    /// \param p A linear process
    /// \param q A linear process
    /// \param i A summand iterator
    /// \return The function result
    pbes_expression step(const lps::linear_process& p, const lps::linear_process& q, my_iterator i) const
    {
      const data::variable_list& d1 = q.process_parameters();
      data::data_expression_list gi = i->next_state(p.process_parameters());
      lps::multi_action ai(i->multi_action().actions());
      if (i->is_tau())
      {
        return close2(p, q, i, gi, data::data_expression_list(d1.begin(), d1.end()));
      }
      else
      {
        std::vector<pbes_expression> v;
        for (auto j = q.action_summands().begin(); j != q.action_summands().end(); ++j)
        {
          data::data_expression cj = j->condition();
          data::variable_list e1 = j->summation_variables();
          data::data_expression_list gj = j->next_state(q.process_parameters());
          lps::multi_action aj(j->multi_action().actions());
          pbes_expression expr = make_exists(e1, optimized_and(optimized_and(cj, equals(ai, aj)), close2(p, q, i, gi, gj)));
          v.push_back(expr);
        }
        return optimized_join_or(v.begin(), v.end());
      }
    }

    /// \brief The close1 function.
    /// \param p A linear process
    /// \param q A linear process
    /// \param i A summand iterator
    /// \return The function result
    pbes_expression close1(const lps::linear_process& p, const lps::linear_process& q, my_iterator i) const
    {
      std::vector<pbes_expression> v;
      data::variable_list e = i->summation_variables();
      const data::variable_list& d = p.process_parameters();
      const data::variable_list& d1 = q.process_parameters();
      for (auto j = q.action_summands().begin(); j != q.action_summands().end(); ++j)
      {
        if (!j->is_tau())
        {
          continue;
        }
        data::data_expression cj = j->condition();
        data::variable_list e1 = j->summation_variables();
        data::data_expression_list gj = j->next_state(d1);
        pbes_expression expr = make_exists(e1, optimized_and(cj, var(Y1(p, q, i), data::data_expression_list(d) + gj + data::data_expression_list(e))));
        v.push_back(expr);
      }
      return optimized_or(optimized_join_or(v.begin(), v.end()), step(p, q, i));
    }

    /// \brief The close function.
    /// \param p A linear process
    /// \param q A linear process
    /// \param i A summand iterator
    /// \param d A sequence of data expressions
    /// \param d1 A sequence of data expressions
    /// \return The function result
    pbes_expression close2(const lps::linear_process& p, const lps::linear_process& q, my_iterator i, const data::data_expression_list& d, const data::data_expression_list& d1) const
    {
      const data::variable_list& parameters = q.process_parameters();

      data::mutable_map_substitution<> sigma; // q.process_parameters() := d1
      make_substitution(parameters, d1, sigma);
      std::set<data::variable> sigma_variables = data::find_free_variables(d1);

      std::vector<pbes_expression> v;
      for (auto j = q.action_summands().begin(); j != q.action_summands().end(); ++j)
      {
        if (!j->is_tau())
        {
          continue;
        }
        // d' == q.process_parameters()
        // e' == j->summand_variables()
        data::data_expression cj = j->condition();                        // cj == cj(d',e')
        data::data_expression_list gj = j->next_state(q.process_parameters()); // gj == gj(d',e')
        data::variable_list e1 = j->summation_variables();              // e1 == e'

        // replace d' by d1 (if needed)
        if (d1 != data::data_expression_list(parameters.begin(), parameters.end()))
        {
          cj = data::replace_variables_capture_avoiding(cj, sigma, sigma_variables);
          gj = data::replace_variables_capture_avoiding(gj, sigma, sigma_variables);
        }

        // replace e' (e1) by fresh variables e'' (e11)
        std::vector<data::variable> tmp;
        for (const data::variable& v: e1)
        {
          tmp.push_back(data::variable(m_generator(std::string(v.name())), v.sort()));
        }
        data::variable_list e11(tmp.begin(), tmp.end());

        data::mutable_map_substitution<> sigma1;
        make_substitution(e1, atermpp::container_cast<data::data_expression_list>(e11), sigma1);
        std::set<data::variable> sigma1_variables(e11.begin(), e11.end());
        data::data_expression cj_new = data::replace_variables_capture_avoiding(cj, sigma1, sigma1_variables);
        data::data_expression_list gj_new = data::replace_variables_capture_avoiding(gj, sigma1, sigma1_variables);

        pbes_expression expr = make_exists(e11, optimized_and(cj_new, var(Y2(p, q, i), d + gj_new)));
        v.push_back(expr);
      }
      return optimized_or(var(X(p, q), d + d1), optimized_join_or(v.begin(), v.end()));
    }

    /// \brief Runs the algorithm
    /// \param model A linear process specification
    /// \param spec A linear process specification
    /// \return A pbes that expresses weak bisimulation between two specifications.
    pbes run(const lps::specification& model, const lps::specification& spec)
    {
      lps::specification spec1 = spec;
      resolve_name_clashes(model, spec1, true);
      const lps::linear_process& m = model.process();
      const lps::linear_process& s = spec1.process();
      init(m, s);

      m_generator.clear_context();
      m_generator.add_identifiers(data::function_and_mapping_identifiers(model.data()));
      m_generator.add_identifiers(data::function_and_mapping_identifiers(spec.data()));
      m_generator.add_identifiers(lps::find_identifiers(model));
      m_generator.add_identifiers(lps::find_identifiers(spec));

      data::variable_list const& d  = m.process_parameters();
      data::variable_list const& d1 = s.process_parameters();
      std::vector<pbes_equation> equations;

      // E1
      equations.push_back(pbes_equation(nu(), propositional_variable(X(m, s), d + d1), optimized_and(match(m, s), match(s, m))));
      equations.push_back(pbes_equation(nu(), propositional_variable(X(s, m), d1 + d), var(X(m, s), d + d1)));

      // E2
      for (auto i = m.action_summands().begin(); i != m.action_summands().end(); ++i)
      {
        data::variable_list e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y1(m, s, i), d + d1 + e), close1(m, s, i));
        pbes_equation e2(mu(), propositional_variable(Y2(m, s, i), d + d1), close2(m, s, i, data::data_expression_list(d.begin(), d.end()), data::data_expression_list(d1.begin(), d1.end())));
        equations.push_back(e1);
        equations.push_back(e2);
      }
      for (auto i = s.action_summands().begin(); i != s.action_summands().end(); ++i)
      {
        data::variable_list e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y1(s, m, i), d1 + d + e), close1(s, m, i));
        pbes_equation e2(mu(), propositional_variable(Y2(s, m, i), d1 + d), close2(s, m, i, data::data_expression_list(d1.begin(), d1.end()), data::data_expression_list(d.begin(), d.end())));
        equations.push_back(e1);
        equations.push_back(e2);
      }

      return build_pbes(equations, model, spec1);
    }
};

/// \brief Returns a pbes that expresses weak bisimulation between two specifications.
/// \param model A linear process specification
/// \param spec A linear process specification
/// \return A pbes that expresses weak bisimulation between the two specifications.
inline
pbes weak_bisimulation(const lps::specification& model, const lps::specification& spec)
{
  return weak_bisimulation_algorithm().run(model, spec);
}

//--------------------------------------------------------------//
//                 branching simulation equivalence
//--------------------------------------------------------------//

/// \brief Algorithm class for branching simulation equivalence.
class branching_simulation_equivalence_algorithm : public branching_bisimulation_algorithm
{
  public:
    /// \brief Runs the algorithm
    /// \param model A linear process specification
    /// \param spec A linear process specification
    /// \return A pbes that expresses branching simulation equivalence between two specifications.
    pbes run(const lps::specification& model, const lps::specification& spec)
    {
      lps::specification spec1 = spec;
      resolve_name_clashes(model, spec1, true);
      const lps::linear_process& m = model.process();
      const lps::linear_process& s = spec1.process();
      init(m, s);

      data::variable_list const& d  = m.process_parameters();
      data::variable_list const& d1 = s.process_parameters();
      std::vector<pbes_equation> equations;


      // E1
      equations.push_back(pbes_equation(nu(), propositional_variable(X(m, s), d + d1), optimized_and(match(m, s), match(s, m))));
      equations.push_back(pbes_equation(nu(), propositional_variable(X(s, m), d1 + d), var(X(m, s), d + d1)));

      // E2
      for (auto i = m.action_summands().begin(); i != m.action_summands().end(); ++i)
      {
        data::variable_list e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y(m, s, i), d + d1 + e), close(m, s, i));
        equations.push_back(e1);
      }
      for (auto i = s.action_summands().begin(); i != s.action_summands().end(); ++i)
      {
        data::variable_list e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y(s, m, i), d1 + d + e), close(s, m, i));
        equations.push_back(e1);
      }

      return build_pbes(equations, model, spec1);
    }
};

/// \brief Returns a pbes that expresses branching simulation equivalence between two specifications.
/// \param model A linear process specification
/// \param spec A linear process specification
/// \return A pbes that expresses branching simulation equivalence between the two specifications.
inline
pbes branching_simulation_equivalence(const lps::specification& model, const lps::specification& spec)
{
  return branching_simulation_equivalence_algorithm().run(model, spec);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_BISIMULATION_H
