// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rename.h
/// \brief Bisimulation algorithms.

#ifndef MCRL2_PBES_BISIMULATION_H
#define MCRL2_PBES_BISIMULATION_H

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>
#include <sstream>
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/data_operators.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lps/rename.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/multi_action_equality.h"
#include "mcrl2/pbes/detail/pbes_translate_impl.h"
#include "mcrl2/pbes/detail/free_variable_visitor.h"

namespace mcrl2 {

namespace pbes_system {

using namespace data;
using namespace lps;

/// Base class for bisimulation algorithms.
class bisimulation_algorithm
{
  public:
    typedef non_delta_summand_list::iterator my_iterator;

  protected:
    typedef std::map<ATermAppl, std::string> name_map;

    /// \brief Maps summands to strings.
    name_map summand_names;

    /// \brief Store the ATerm of the model.
    ATermAppl model_ptr;

    /// \brief Store a ATerm of the specification.
    ATermAppl spec_ptr;

    /// \brief Returns a name of an action_list.
    std::string action_list_name(action_list l) const
    {
      std::ostringstream out;
      for (action_list::iterator i = l.begin(); i != l.end(); ++i)
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

    /// \brief Returns the name associated with i.
    std::string summand_name(my_iterator i) const
    {
      ATermAppl t = *i;
      name_map::const_iterator j = summand_names.find(t);
      assert(j != summand_names.end());
      return j->second;
    }

    /// \brief Returns true if p is the linear process of the model.
    bool is_from_model(const linear_process& p) const
    {
      return ATermAppl(p) == model_ptr;
    }

    /// \brief Returns a name of a linear process.
    std::string process_name(const linear_process& p) const
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
    void set_summand_names(const linear_process& p)
    {
      set_identifier_generator generator;
      for (my_iterator i = p.non_delta_summands().begin(); i != p.non_delta_summands().end(); ++i)
      {
        std::string name = generator(action_list_name(i->actions()));
        ATermAppl t = *i;
        summand_names[t] = name;
      }
    }

    /// \brief Used for debugging.
    void check_expression(pbes_expression expr, const linear_process& p, const linear_process& q, std::string msg) const
    {
      detail::free_variable_visitor<pbes_expression> visitor;
      visitor.bound_variables = p.process_parameters() + q.process_parameters();
      visitor.visit(expr);
      std::set<data_variable> w = visitor.result;

      std::set<data_variable> free_variables;
      free_variables.insert(p.free_variables().begin(), p.free_variables().end());
      free_variables.insert(q.free_variables().begin(), q.free_variables().end());

      for (std::set<data_variable>::iterator i = w.begin(); i != w.end(); ++i)
      {
        if (free_variables.find(*i) == free_variables.end())
        {
          std::cerr << "error: " << msg << mcrl2::core::pp(*i) << " is free! " << mcrl2::core::pp(expr) << std::endl;
        }
      }
    }

public:
    /// \brief Creates an identifier string.
    core::identifier_string X(const linear_process& p, const linear_process& q) const
    {
      std::string s = "X" + process_name(p) + process_name(q);
      return core::identifier_string(s);
    }

    /// \brief Creates an identifier string.
    core::identifier_string Y(const linear_process& p, const linear_process& q) const
    {
      std::string s = "Y" + process_name(p) + process_name(q);
      return core::identifier_string(s);
    }

    /// \brief Creates an identifier string.
    /// \pre The iterator i must be in p.non_delta_summands().
    core::identifier_string Y(const linear_process& p, const linear_process& q, my_iterator i) const
    {
      std::string s = "Y" + process_name(p) + process_name(q) + "_" + summand_name(i);
      return core::identifier_string(s);
    }

    /// \brief Creates an identifier string.
    /// \pre The iterator i must be in p.non_delta_summands().
    core::identifier_string Y1(const linear_process& p, const linear_process& q, my_iterator i) const
    {
      std::string s = "Y1" + process_name(p) + process_name(q) + "_" + summand_name(i);
      return core::identifier_string(s);
    }

    /// \brief Creates an identifier string.
    /// \pre The iterator i must be in p.non_delta_summands().
    core::identifier_string Y2(const linear_process& p, const linear_process& q, my_iterator i) const
    {
      std::string s = "Y2" + process_name(p) + process_name(q) + "_" + summand_name(i);
      return core::identifier_string(s);
    }

    /// \brief Returns a propositional variable.
    propositional_variable_instantiation var(core::identifier_string name, data_variable_list parameters) const
    {
      return propositional_variable_instantiation(name, make_data_expression_list(parameters));
    }

    /// \brief Returns a pbes expression that expresses equality of the multi actions a and b.
    pbes_expression equals(action_list a, action_list b) const
    {
      return equal_multi_actions(a, b);
    }

    /// \brief Returns mu.
    fixpoint_symbol mu() const
    {
      return fixpoint_symbol::mu();
    }

    /// \brief Returns nu.
    fixpoint_symbol nu() const
    {
      return fixpoint_symbol::nu();
    }

    /// \brief Renames variables in q such that there are no name clashes.
    /// \return The process q after renaming.
    linear_process resolve_name_clashes(const linear_process& p, const linear_process& q)
    {
      std::set<core::identifier_string> used_names;
      used_names.insert(boost::make_transform_iterator(p.process_parameters().begin(), data::detail::data_variable_name()),
                        boost::make_transform_iterator(p.process_parameters().end()  , data::detail::data_variable_name())
                       );
      used_names.insert(boost::make_transform_iterator(p.free_variables().begin(), data::detail::data_variable_name()),
                        boost::make_transform_iterator(p.free_variables().end()  , data::detail::data_variable_name())
                       );
      for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
      {
        used_names.insert(boost::make_transform_iterator(i->summation_variables().begin(), data::detail::data_variable_name()),
                          boost::make_transform_iterator(i->summation_variables().end()  , data::detail::data_variable_name())
                         );
      }
      linear_process result = q;
      result = rename_process_parameters (result, used_names, "_S");
      result = rename_free_variables     (result, used_names, "_S");
      result = rename_summation_variables(result, used_names, "_S");
      return result;
    }

    /// \brief Initializes the name lookup table.
    void init(const linear_process& model, const linear_process& spec)
    {
      summand_names.clear();
      set_summand_names(model);
      set_summand_names(spec);
      model_ptr = ATermAppl(model);
      spec_ptr  = ATermAppl(spec);
      assert(is_from_model(model));
      assert(!is_from_model(spec));
    }

    /// \brief Builds a pbes from the given equations.
    pbes<> build_pbes(const atermpp::vector<pbes_equation>& equations,
                      const specification& M,
                      const specification& S
                     )
    {
      const linear_process& m = M.process();
      const linear_process& s = S.process();

      // TODO: the data of the two specification needs to be merged!
      data_specification data = M.data();
      propositional_variable_instantiation init(X(m, s), M.initial_process().state() + S.initial_process().state());

      pbes<> result(data, equations, init);
      assert(result.is_closed());
      return result;
    }
};

//--------------------------------------------------------------//
//                 branching bisimulation
//--------------------------------------------------------------//

/// Algorithm class for branching bisimulation.
class branching_bisimulation_algorithm : public bisimulation_algorithm
{
  public:
    /// \brief The match function.
    pbes_expression match(const linear_process& p, const linear_process& q) const
    {
      using namespace pbes_expr_optimized;
      std::vector<pbes_expression> result;
      for (my_iterator i = p.non_delta_summands().begin(); i != p.non_delta_summands().end(); ++i)
      {
        const data_expression&    ci = i->condition();
        const data_variable_list& d  = p.process_parameters();
        const data_variable_list& e  = i->summation_variables();
        const data_variable_list& d1 = q.process_parameters();
        pbes_expression expr = forall(e, imp(ci, var(Y(p, q, i), d + d1 + e)));
        result.push_back(expr);
      }
      return join_and(result.begin(), result.end());
    }

    /// \brief The step function.
    pbes_expression step(const linear_process& p, const linear_process& q, my_iterator i) const
    {
      using namespace pbes_expr_optimized;
      const data_variable_list& d1 = q.process_parameters();
      data_variable_list gi = i->next_state(p.process_parameters());
      if (i->is_tau())
      {
        std::vector<pbes_expression> v;
        for (my_iterator j = q.non_delta_summands().begin(); j != q.non_delta_summands().end(); ++j)
        {
          if (!j->is_tau())
          {
            continue;
          }
          const data_expression&    cj = j->condition();
          const data_variable_list& e1 = j->summation_variables();
          data_variable_list        gj = j->next_state(q.process_parameters());
          const action_list         ai = i->actions();
          const action_list         aj = j->actions();
          pbes_expression expr = exists(e1, and_(cj, var(X(p, q), gi + gj)));
          v.push_back(expr);
        }
        return or_(join_or(v.begin(), v.end()), var(X(p, q), gi + d1));
      }
      else
      {
        std::vector<pbes_expression> v;
        for (my_iterator j = q.non_delta_summands().begin(); j != q.non_delta_summands().end(); ++j)
        {
          const data_expression&    cj = j->condition();
          const data_variable_list& e1 = j->summation_variables();
          data_variable_list        gj = j->next_state(q.process_parameters());
          const action_list         ai = i->actions();
          const action_list         aj = j->actions();
          pbes_expression expr = exists(e1, and_(and_(cj, equals(ai, aj)), var(X(p, q), gi + gj)));
          v.push_back(expr);
        }
        return join_or(v.begin(), v.end());
      }
    }

    /// \brief The close function.
    pbes_expression close(const linear_process& p, const linear_process& q, my_iterator i) const
    {
      using namespace pbes_expr_optimized;
      std::vector<pbes_expression> v;
      const data_variable_list& d  = p.process_parameters();
      const data_variable_list& d1 = q.process_parameters();
      const data_variable_list& e  = i->summation_variables();
      for (my_iterator j = q.non_delta_summands().begin(); j != q.non_delta_summands().end(); ++j)
      {
        if (!j->is_tau())
        {
          continue;
        }
        const data_expression&    cj = j->condition();
        const data_variable_list& e1 = j->summation_variables();
        data_variable_list        gj = j->next_state(q.process_parameters());
        pbes_expression expr = exists(e1, and_(cj, var(Y(p, q, i), d + gj + e)));
        v.push_back(expr);
      }
      return or_(join_or(v.begin(), v.end()), and_(var(X(p, q), d + d1), step(p, q, i)));
    }

    /// \brief Returns a pbes that expresses branching bisimulation between
    /// two specifications.
    pbes<> run(const specification& model, const specification& spec)
    {
      using namespace pbes_expr_optimized;
      const linear_process& m = model.process();
      linear_process s = resolve_name_clashes(m, spec.process());
      init(m, s);

      const data_variable_list& d  = m.process_parameters();
      const data_variable_list& d1 = s.process_parameters();
      atermpp::vector<pbes_equation> equations;


      // E1
      equations.push_back(pbes_equation(nu(), propositional_variable(X(m, s), d + d1), and_(match(m, s), match(s, m))));
      equations.push_back(pbes_equation(nu(), propositional_variable(X(s, m), d1 + d), var(X(m, s), d + d1)));

      // E2
      for (my_iterator i = m.non_delta_summands().begin(); i != m.non_delta_summands().end(); ++i)
      {
        const data_variable_list& e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y(m, s, i), d + d1 + e), close(m, s, i));
        equations.push_back(e1);
      }
      for (my_iterator i = s.non_delta_summands().begin(); i != s.non_delta_summands().end(); ++i)
      {
        const data_variable_list& e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y(s, m, i), d1 + d + e), close(s, m, i));
        equations.push_back(e1);
      }

      return build_pbes(equations, model, set_lps(spec, s));
    }
};

/// \brief Returns a pbes that expresses branching bisimulation between two specifications.
pbes<> branching_bisimulation(const specification& model, const specification& spec)
{
  return branching_bisimulation_algorithm().run(model, spec);
}

//--------------------------------------------------------------//
//                 strong bisimulation
//--------------------------------------------------------------//

/// Algorithm class for strong bisimulation.
class strong_bisimulation_algorithm : public bisimulation_algorithm
{
  public:
    /// \brief The match function.
    pbes_expression match(const linear_process& p, const linear_process& q) const
    {
      using namespace pbes_expr_optimized;
      std::vector<pbes_expression> result;
      for (my_iterator i = p.non_delta_summands().begin(); i != p.non_delta_summands().end(); ++i)
      {
        const data_expression&    ci = i->condition();
        const data_variable_list& e  = i->summation_variables();
        pbes_expression expr = forall(e, imp(ci, step(p, q, i)));
        result.push_back(expr);
      }
      return join_and(result.begin(), result.end());
    }

    /// \brief The step function.
    pbes_expression step(const linear_process& p, const linear_process& q, my_iterator i) const
    {
      using namespace pbes_expr_optimized;
      data_variable_list gi = i->next_state(p.process_parameters());

      std::vector<pbes_expression> result;
      for (my_iterator j = q.non_delta_summands().begin(); j != q.non_delta_summands().end(); ++j)
      {
        const data_expression&    cj = j->condition();
        const data_variable_list& e1 = j->summation_variables();
        data_variable_list        gj = j->next_state(q.process_parameters());
        const action_list         ai = i->actions();
        const action_list         aj = j->actions();
        pbes_expression expr = exists(e1, and_(and_(cj, equals(ai, aj)), var(X(p, q), gi + gj)));
        result.push_back(expr);
      }
      return join_or(result.begin(), result.end());
    }

    /// \brief Returns a pbes that expresses strong bisimulation between
    /// two specifications.
    pbes<> run(const specification& model, const specification& spec)
    {
      using namespace pbes_expr_optimized;
      const linear_process& m = model.process();
      linear_process s = resolve_name_clashes(m, spec.process());
      init(m, s);

      const data_variable_list& d  = m.process_parameters();
      const data_variable_list& d1 = s.process_parameters();
      atermpp::vector<pbes_equation> equations;


      // E
      equations.push_back(pbes_equation(nu(), propositional_variable(X(m, s), d + d1), and_(match(m, s), match(s, m))));
      equations.push_back(pbes_equation(nu(), propositional_variable(X(s, m), d1 + d), var(X(m, s), d + d1)));

      return build_pbes(equations, model, set_lps(spec, s));
    }
};

/// \brief Returns a pbes that expresses strong bisimulation between two specifications.
pbes<> strong_bisimulation(const specification& model, const specification& spec)
{
  return strong_bisimulation_algorithm().run(model, spec);
}

//--------------------------------------------------------------//
//                 weak bisimulation
//--------------------------------------------------------------//

/// Algorithm class for weak bisimulation.
class weak_bisimulation_algorithm : public bisimulation_algorithm
{
  public:
    /// \brief The match function.
    pbes_expression match(const linear_process& p, const linear_process& q) const
    {
      using namespace pbes_expr_optimized;
      std::vector<pbes_expression> result;
      for (my_iterator i = p.non_delta_summands().begin(); i != p.non_delta_summands().end(); ++i)
      {
        const data_expression&    ci = i->condition();
        const data_variable_list& d  = p.process_parameters();
        const data_variable_list& e  = i->summation_variables();
        const data_variable_list& d1 = q.process_parameters();
        pbes_expression expr = forall(e, imp(ci, var(Y1(p, q, i), d + d1 + e)));
        result.push_back(expr);
      }
      return join_and(result.begin(), result.end());
    }

    /// \brief The step function.
    pbes_expression step(const linear_process& p, const linear_process& q, my_iterator i) const
    {
      using namespace pbes_expr_optimized;
      const data_variable_list& d1 = q.process_parameters();
      data_variable_list        gi = i->next_state(p.process_parameters());
      const action_list         ai = i->actions();
      if (i->is_tau())
      {
        return close2(p, q, i, gi, d1);
      }
      else
      {
        std::vector<pbes_expression> v;
        for (my_iterator j = q.non_delta_summands().begin(); j != q.non_delta_summands().end(); ++j)
        {
          const data_expression&    cj = j->condition();
          const data_variable_list& e1 = j->summation_variables();
          data_variable_list        gj = j->next_state(q.process_parameters());
          const action_list         aj = j->actions();
          pbes_expression expr = exists(e1, and_(and_(cj, equals(ai, aj)), close2(p, q, i, gi, gj)));
          v.push_back(expr);
        }
        return join_or(v.begin(), v.end());
      }
    }

    /// \brief The close1 function.
    pbes_expression close1(const linear_process& p, const linear_process& q, my_iterator i) const
    {
      using namespace pbes_expr_optimized;
      std::vector<pbes_expression> v;
      const data_variable_list& d1 = q.process_parameters();
      for (my_iterator j = q.non_delta_summands().begin(); j != q.non_delta_summands().end(); ++j)
      {
        if (!j->is_tau())
        {
          continue;
        }
        const data_expression&    cj = j->condition();
        const data_variable_list& e1 = j->summation_variables();
        data_variable_list        gj = j->next_state(q.process_parameters());
        pbes_expression expr = exists(e1, and_(cj, var(Y1(p, q, i), d1 + gj + e1)));
        v.push_back(expr);
      }
      return or_(join_or(v.begin(), v.end()), step(p, q, i));
    }

    /// \brief The close2 function.
    pbes_expression close2(const linear_process& p, const linear_process& q, my_iterator i,
                           data_variable_list d, data_variable_list d1) const
    {
      using namespace pbes_expr_optimized;

      //const data_variable_list& d  = p.process_parameters();
      //const data_variable_list& d1 = q.process_parameters();
      data_variable_list        gi = i->next_state(p.process_parameters());
      const action_list         ai = i->actions();
      std::vector<pbes_expression> v;
      for (my_iterator j = q.non_delta_summands().begin(); j != q.non_delta_summands().end(); ++j)
      {
        if (!j->is_tau())
        {
          continue;
        }
        // d' == q.process_parameters()
        // e' == j->summand_variables()
        data_expression    cj  = j->condition();                        // cj == cj(d',e')
        data_variable_list gj  = j->next_state(q.process_parameters()); // gj == gj(d',e')
        data_variable_list e1  = j->summation_variables();              // e1 == e'

        // replace d' by d1 (if needed)
        if (d1 != q.process_parameters())
        {
          cj = cj.substitute(make_list_substitution(q.process_parameters(), d1));
          gj = gj.substitute(make_list_substitution(q.process_parameters(), d1));
        }

        // replace e' (e1) by fresh variables e'' (e1_new)
        std::set<std::string> used_names = mcrl2::data::detail::find_variable_name_strings(atermpp::make_list(p, q));
        data_variable_list e1_new = fresh_variables(e1, used_names);
        data_expression    cj_new = cj.substitute(make_list_substitution(e1, e1_new));
        data_variable_list gj_new = gj.substitute(make_list_substitution(e1, e1_new));

        pbes_expression expr = exists(e1_new, and_(cj_new, var(Y2(p, q, i), d + gj_new)));
        v.push_back(expr);
      }
      return or_(var(X(p, q), d + d1), join_or(v.begin(), v.end()));
    }

    /// \brief Returns a pbes that expresses weak bisimulation between
    /// two specifications.
    pbes<> run(const specification& model, const specification& spec)
    {
      using namespace pbes_expr_optimized;
      const linear_process& m = model.process();
      linear_process s = resolve_name_clashes(m, spec.process());
      init(m, s);

      const data_variable_list& d  = m.process_parameters();
      const data_variable_list& d1 = s.process_parameters();
      atermpp::vector<pbes_equation> equations;


      // E1
      equations.push_back(pbes_equation(nu(), propositional_variable(X(m, s), d + d1), and_(match(m, s), match(s, m))));
      equations.push_back(pbes_equation(nu(), propositional_variable(X(s, m), d1 + d), var(X(m, s), d + d1)));

      // E2
      for (my_iterator i = m.non_delta_summands().begin(); i != m.non_delta_summands().end(); ++i)
      {
        const data_variable_list& e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y1(m, s, i), d + d1 + e), close1(m, s, i));
        pbes_equation e2(mu(), propositional_variable(Y2(m, s, i), d + d1), close2(m, s, i, d, d1));
        equations.push_back(e1);
        equations.push_back(e2);
      }
      for (my_iterator i = s.non_delta_summands().begin(); i != s.non_delta_summands().end(); ++i)
      {
        const data_variable_list& e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y1(s, m, i), d1 + d + e), close1(s, m, i));
        pbes_equation e2(mu(), propositional_variable(Y2(s, m, i), d1 + d), close2(s, m, i, d1, d));
        equations.push_back(e1);
        equations.push_back(e2);
      }

      return build_pbes(equations, model, set_lps(spec, s));
    }
};

/// \brief Returns a pbes that expresses weak bisimulation between two specifications.
pbes<> weak_bisimulation(const specification& model, const specification& spec)
{
  return weak_bisimulation_algorithm().run(model, spec);
}

//--------------------------------------------------------------//
//                 branching simulation equivalence
//--------------------------------------------------------------//

/// Algorithm class for branching simulation equivalence.
class branching_simulation_equivalence_algorithm : public branching_bisimulation_algorithm
{
  public:
    /// \brief Returns a pbes that expresses branching simulation equivalence between
    /// two specifications.
    pbes<> run(const specification& model, const specification& spec)
    {
      using namespace pbes_expr_optimized;
      const linear_process& m = model.process();
      linear_process s = resolve_name_clashes(m, spec.process());
      init(m, s);

      const data_variable_list& d  = m.process_parameters();
      const data_variable_list& d1 = s.process_parameters();
      atermpp::vector<pbes_equation> equations;


      // E1
      equations.push_back(pbes_equation(nu(), propositional_variable(X(m, s), d + d1), and_(match(m, s), match(s, m))));
      equations.push_back(pbes_equation(nu(), propositional_variable(X(s, m), d1 + d), var(X(m, s), d + d1)));

      // E2
      for (my_iterator i = m.non_delta_summands().begin(); i != m.non_delta_summands().end(); ++i)
      {
        const data_variable_list& e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y(m, s, i), d + d1 + e), close(m, s, i));
        equations.push_back(e1);
      }
      for (my_iterator i = s.non_delta_summands().begin(); i != s.non_delta_summands().end(); ++i)
      {
        const data_variable_list& e  = i->summation_variables();
        pbes_equation e1(mu(), propositional_variable(Y(s, m, i), d1 + d + e), close(s, m, i));
        equations.push_back(e1);
      }

      return build_pbes(equations, model, set_lps(spec, s));
    }
};

/// \brief Returns a pbes that expresses branching simulation equivalence between two specifications.
pbes<> branching_simulation_equivalence(const specification& model, const specification& spec)
{
  return branching_simulation_equivalence_algorithm().run(model, spec);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_BISIMULATION_H
