// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/control_flow.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_CONTROL_FLOW_H
#define MCRL2_PBES_DETAIL_CONTROL_FLOW_H

#include <algorithm>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/substitutions.h"
#include "mcrl2/data/detail/simplify_rewrite_builder.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/pbes/detail/pfnf_pbes.h"
#include "mcrl2/pbes/detail/simplify_quantifier_builder.h"
#include "mcrl2/pbes/detail/control_flow_influence.h"
#include "mcrl2/pbes/detail/control_flow_source_dest.h"
#include "mcrl2/pbes/detail/control_flow_utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// Adds some simplifications to simplify_rewrite_builder.
template <typename Term, typename DataRewriter, typename SubstitutionFunction = no_substitution>
struct control_flow_simplify_quantifier_builder: public pbes_system::detail::simplify_quantifier_builder<Term, DataRewriter, SubstitutionFunction>
{
  typedef pbes_system::detail::simplify_quantifier_builder<Term, DataRewriter, SubstitutionFunction> super;
  typedef SubstitutionFunction                                                                       argument_type;
  typedef typename super::term_type                                                                  term_type;
  typedef typename core::term_traits<term_type>::data_term_type                                      data_term_type;
  typedef typename core::term_traits<term_type>::data_term_sequence_type                             data_term_sequence_type;
  typedef typename core::term_traits<term_type>::variable_sequence_type                              variable_sequence_type;
  typedef typename core::term_traits<term_type>::propositional_variable_type                         propositional_variable_type;
  typedef core::term_traits<Term> tr;

  /// \brief Constructor.
  /// \param rewr A data rewriter
  control_flow_simplify_quantifier_builder(const DataRewriter& rewr)
    : simplify_quantifier_builder<Term, DataRewriter, SubstitutionFunction>(rewr)
  { }

  bool is_data_not(const pbes_expression& x) const
  {
    return data::is_data_expression(x) && data::sort_bool::is_not_application(x);
  }

  // replace !(y || z) by !y && !z
  // replace !(y && z) by !y || !z
  // replace !(y => z) by y || !z
  // replace y => z by !y || z
  term_type post_process(const term_type& x)
  {
    term_type result = x;
    if (tr::is_not(x))
    {
      term_type t = tr::not_arg(x);
      if (tr::is_and(t)) // x = !(y && z)
      {
        term_type y = utilities::optimized_not(tr::left(t));
        term_type z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_and(y, z);
      }
      else if (tr::is_or(t)) // x = !(y || z)
      {
        term_type y = utilities::optimized_not(tr::left(t));
        term_type z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_or(y, z);
      }
      else if (tr::is_imp(t)) // x = !(y => z)
      {
        term_type y = tr::left(t);
        term_type z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_or(y, z);
      }
      else if (is_data_not(t)) // x = !val(!y)
      {
        term_type y = data::application(t).arguments().front();
        result = y;
      }
    }
    else if (tr::is_imp(x)) // x = y => z
    {
      term_type y = utilities::optimized_not(tr::left(x));
      term_type z = tr::right(x);
      result = utilities::optimized_or(y, z);
    }
    return result;
  }

  // replace the data expression y != z by !(y == z)
  term_type visit_data_expression(const term_type& x, const data_term_type& d, SubstitutionFunction& sigma)
  {
    typedef core::term_traits<data::data_expression> tt;
    term_type result = super::visit_data_expression(x, d, sigma);
    data::data_expression t = result;
    if (data::is_not_equal_to_application(t)) // result = y != z
    {
      data::data_expression y = tt::left(t);
      data::data_expression z = tt::right(t);
      result = tr::not_(data::equal_to(y, z));
    }
    return post_process(result);
  }

  term_type visit_true(const term_type& x, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_true(x, sigma));
  }

  term_type visit_false(const term_type& x, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_false(x, sigma));
  }

  term_type visit_not(const term_type& x, const term_type& n, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_not(x, n, sigma));
  }

  term_type visit_and(const term_type& x, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_and(x, left, right, sigma));
  }

  term_type visit_or(const term_type& x, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_or(x, left, right, sigma));
  }

  term_type visit_imp(const term_type& x, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_imp(x, left, right, sigma));
  }

  term_type visit_forall(const term_type& x, const variable_sequence_type&  variables, const term_type&  expression, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_forall(x, variables, expression, sigma));
  }

  term_type visit_exists(const term_type& x, const variable_sequence_type&  variables, const term_type&  expression, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_exists(x, variables, expression, sigma));
  }

  term_type visit_propositional_variable(const term_type& x, const propositional_variable_type&  v, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_propositional_variable(x, v, sigma));
  }
};

template <typename Term, typename DataRewriter>
class control_flow_simplifying_rewriter
{
  protected:
    DataRewriter m_rewriter;

  public:
    typedef typename core::term_traits<Term>::term_type term_type;
    typedef typename core::term_traits<Term>::variable_type variable_type;

    control_flow_simplifying_rewriter(const DataRewriter& rewriter)
      : m_rewriter(rewriter)
    {}

    term_type operator()(const term_type& x) const
    {
      control_flow_simplify_quantifier_builder<Term, DataRewriter> r(m_rewriter);
      return r(x);
    }

    template <typename SubstitutionFunction>
    term_type operator()(const term_type& x, SubstitutionFunction sigma) const
    {
      control_flow_simplify_quantifier_builder<Term, DataRewriter, SubstitutionFunction> r(m_rewriter);
      return r(x, sigma);
    }
};

struct control_flow_vertex;

// edge of the control flow graph
struct control_flow_edge
{
  control_flow_vertex* source;
  control_flow_vertex* target;
  propositional_variable_instantiation label;

  control_flow_edge(control_flow_vertex* source_,
                    control_flow_vertex* target_,
                    const propositional_variable_instantiation& label_
                   )
   : source(source_),
     target(target_),
     label(label_)
   {}

  bool operator<(const control_flow_edge& other) const
  {
    if (source != other.source)
    {
      return source < other.source;
    }
    if (target != other.target)
    {
      return target < other.target;
    }
    return label < other.label;
  }

  std::string print() const;

  void protect() const
  {
    label.protect();
  }

  void unprotect() const
  {
    label.unprotect();
  }

  void mark() const
  {
    label.mark();
  }
};

// vertex of the control flow graph
struct control_flow_vertex
{
  propositional_variable_instantiation X;
  atermpp::set<control_flow_edge> incoming_edges;
  atermpp::set<control_flow_edge> outgoing_edges;
  atermpp::set<pbes_expression> guards;
  std::set<data::variable> marking;
  std::vector<bool> marked_parameters; // will be set after computing the marking

  control_flow_vertex(const propositional_variable_instantiation& X_)
    : X(X_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << pbes_system::pp(X);
    out << " edges:";
    for (atermpp::set<control_flow_edge>::const_iterator i = outgoing_edges.begin(); i != outgoing_edges.end(); ++i)
    {
      out << " " << pbes_system::pp(i->target->X);
    }
    out << " guards: " << print_pbes_expressions(guards);
    return out.str();
  }

  std::set<data::variable> free_guard_variables() const
  {
    std::set<data::variable> result;
    for (atermpp::set<pbes_expression>::const_iterator i = guards.begin(); i != guards.end(); ++i)
    {
      pbes_system::find_free_variables(*i, std::inserter(result, result.end()));
    }
    return result;
  }

  std::set<std::size_t> marking_variable_indices(const pfnf_pbes& p) const
  {
    std::set<std::size_t> result;
    for (std::set<data::variable>::const_iterator i = marking.begin(); i != marking.end(); ++i)
    {
      // TODO: make this code more efficient
      const pfnf_equation& eqn = *find_equation(p, X.name());
      const atermpp::vector<data::variable>& d = eqn.parameters();
      for (atermpp::vector<data::variable>::const_iterator j = d.begin(); j != d.end(); ++j)
      {
        if (*i == *j)
        {
          result.insert(j - d.begin());
          break;
        }
      }
    }
    return result;
  }

  // returns true if the i-th parameter of X is marked
  bool is_marked_parameter(std::size_t i) const
  {
    return marked_parameters[i];
  }

  void protect() const
  {
    X.protect();
  }

  void unprotect() const
  {
    X.unprotect();
  }

  void mark() const
  {
    X.mark();
  }
};

} // namespace detail
} // namespace pbes_system
} // namespace mcrl2
/// \cond INTERNAL_DOCS
namespace atermpp
{
template<>
struct aterm_traits<mcrl2::pbes_system::detail::control_flow_vertex>
{
  static void protect(const mcrl2::pbes_system::detail::control_flow_vertex& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::pbes_system::detail::control_flow_vertex& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::pbes_system::detail::control_flow_vertex& t)
  {
    t.mark();
  }
};

template<>
struct aterm_traits<mcrl2::pbes_system::detail::control_flow_edge>
{
  static void protect(const mcrl2::pbes_system::detail::control_flow_edge& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::pbes_system::detail::control_flow_edge& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::pbes_system::detail::control_flow_edge& t)
  {
    t.mark();
  }
};
} // namespace atermpp
/// \endcond
namespace mcrl2 {
namespace pbes_system {
namespace detail {

inline
std::string control_flow_edge::print() const
{
  std::ostringstream out;
  out << "(" << pbes_system::pp(source->X) << ", " << pbes_system::pp(target->X) << ") label = " << pbes_system::pp(label);
  return out.str();
}

/// \brief Algorithm class for the control_flow algorithm
class pbes_control_flow_algorithm
{
  public:
    // simplify and rewrite the expression x
    pbes_expression simplify(const pbes_expression& x) const
    {
      data::detail::simplify_rewriter r;
      control_flow_simplifying_rewriter<pbes_expression, data::detail::simplify_rewriter> R(r);
      return R(x);
    }

    // simplify and rewrite the guards of the pbes p
    void simplify(pfnf_pbes& p) const
    {
      atermpp::vector<pfnf_equation>& equations = p.equations();
      for (atermpp::vector<pfnf_equation>::iterator k = equations.begin(); k != equations.end(); ++k)
      {
        simplify(k->h());
        atermpp::vector<pfnf_implication>& implications = k->implications();
        for (atermpp::vector<pfnf_implication>::iterator i = implications.begin(); i != implications.end(); ++i)
        {
          simplify(i->g());
        }
      }
    }

    pbes_control_flow_algorithm(const pbes<>& p)
      : m_pbes(p)
    {
      simplify(m_pbes);
    }

  protected:
    typedef atermpp::map<propositional_variable_instantiation, control_flow_vertex>::iterator vertex_iterator;

    // vertices of the control flow graph
    atermpp::map<propositional_variable_instantiation, control_flow_vertex> m_control_vertices;

    // the pbes that is considered
    pfnf_pbes m_pbes;

    // the control flow parameters
    std::map<core::identifier_string, std::vector<bool> > m_is_control_flow;

    propositional_variable find_propvar(const pbes<>& p, const core::identifier_string& X) const
    {
      const atermpp::vector<pbes_equation>& equations = p.equations();
      for (atermpp::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        if (i->variable().name() == X)
        {
          return i->variable();
        }
      }
      throw mcrl2::runtime_error("find_propvar failed!");
      return propositional_variable();
    }

    void print_control_flow_parameters()
    {
      std::cout << "--- control flow parameters ---" << std::endl;
      const atermpp::vector<pfnf_equation>& equations = m_pbes.equations();
      for (atermpp::vector<pfnf_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const atermpp::vector<data::variable>& d_X = k->parameters();
        const std::vector<bool>& cf = m_is_control_flow[X.name()];

        std::cout << core::pp(X.name()) << " ";
        for (std::size_t i = 0; i < cf.size(); ++i)
        {
          if (cf[i])
          {
            std::cout << data::pp(d_X[i]) << " ";
          }
        }
        std::cout << std::endl;
      }
    }

    std::string print_propvar_parameter(const core::identifier_string& X, std::size_t index) const
    {
      return "(" + core::pp(X) + ", " + data::pp(find_equation(m_pbes, X)->parameters()[index]) + ")";
    }

    std::string print_control_flow_assignment(bool control_flow_value,
                                              std::size_t index,
                                              const pbes_system::propositional_variable& X,
                                              const pbes_system::propositional_variable_instantiation& Xij,
                                              const std::string& message,
                                              const data::variable& previous_value = data::variable()
                                             ) const
    {
      std::ostringstream out;
      out << "[cf] " << message << ": " << print_propvar_parameter(Xij.name(), index) << " -> " << std::boolalpha << control_flow_value;
      out << " because of equation " << core::pp(X.name());
      data::variable_list v = X.parameters();
      if (v.size() > 0)
      {
        out << "(";
        for (data::variable_list::iterator i = v.begin(); i != v.end(); ++i)
        {
          if (i != v.begin())
          {
            out << ", ";
          }
          out << core::pp(i->name());
        }
        out << ")";
      }
      out << " = ... " << pbes_system::pp(Xij) << " index = " << index << " " << data::pp(previous_value) << std::endl;
      return out.str();
    }

    void compute_control_flow_parameters()
    {
      const atermpp::vector<pfnf_equation>& equations = m_pbes.equations();
      std::map<core::identifier_string, std::vector<data::variable> > V;

      // initialize all control flow parameters to true
      // initalize V_km to the empty set
      for (atermpp::vector<pfnf_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const atermpp::vector<data::variable>& d_X = k->parameters();
        m_is_control_flow[X.name()] = std::vector<bool>(d_X.size(), true);
        V[X.name()] = std::vector<data::variable>(d_X.size(), data::variable());
      }

      // pass 1
      for (atermpp::vector<pfnf_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        propositional_variable X = k->variable();
        const atermpp::vector<data::variable>& d_X = k->parameters();
        const atermpp::vector<pfnf_implication>& implications = k->implications();
        for (atermpp::vector<pfnf_implication>::const_iterator i = implications.begin(); i != implications.end(); ++i)
        {
          const atermpp::vector<propositional_variable_instantiation>& propvars = i->variables();
          for (atermpp::vector<propositional_variable_instantiation>::const_iterator j = propvars.begin(); j != propvars.end(); ++j)
          {
            const propositional_variable_instantiation& Xij = *j;
            data::data_expression_list d = Xij.parameters();
            std::size_t index = 0;
            for (data::data_expression_list::const_iterator q = d.begin(); q != d.end(); ++q, ++index)
            {
              if (data::is_variable(*q))
              {
                std::vector<data::variable>::const_iterator found = std::find(d_X.begin(), d_X.end(), *q);
                if (found != d_X.end())
                {
                  if (V[Xij.name()][index] == data::variable())
                  {
                    V[Xij.name()][index] = *q;
                    mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(true, index, X, Xij, "pass 1");
                  }
                  else
                  {
                    bool is_same_value = (V[Xij.name()][index] == *q);
                    m_is_control_flow[Xij.name()][index] = is_same_value;
                    mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(is_same_value, index, X, Xij, "pass 1", V[Xij.name()][index]);
                  }
                }
              }
            }
          }
        }
      }

      // pass 2
      std::set<core::identifier_string> todo;
      for (atermpp::vector<pfnf_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        todo.insert(k->variable().name());
      }

      while (!todo.empty())
      {
        core::identifier_string name = *todo.begin();
        todo.erase(todo.begin());
        const pfnf_equation& eqn = *find_equation(m_pbes, name);
        propositional_variable X = eqn.variable();
        const atermpp::vector<data::variable>& d_X = eqn.parameters();
        const atermpp::vector<pfnf_implication>& implications = eqn.implications();
        for (atermpp::vector<pfnf_implication>::const_iterator i = implications.begin(); i != implications.end(); ++i)
        {
          const atermpp::vector<propositional_variable_instantiation>& propvars = i->variables();
          for (atermpp::vector<propositional_variable_instantiation>::const_iterator j = propvars.begin(); j != propvars.end(); ++j)
          {
            const propositional_variable_instantiation& Xij = *j;
            data::data_expression_list d = Xij.parameters();
            std::size_t index = 0;
            for (data::data_expression_list::const_iterator q = d.begin(); q != d.end(); ++q, ++index)
            {
              if (is_constant(*q))
              {
                continue;
              }
              else if (data::is_variable(*q))
              {
                std::vector<data::variable>::const_iterator found = std::find(d_X.begin(), d_X.end(), *q);
                if (found == d_X.end())
                {
                  if (m_is_control_flow[Xij.name()][index] != false)
                  {
                    m_is_control_flow[Xij.name()][index] = false;
                    todo.insert(Xij.name());
                    mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(false, index, X, Xij, "pass 2");
                  }
                }
                else
                {
                  if (X.name() == Xij.name() && (found != d_X.begin() + index))
                  {
                    if (m_is_control_flow[Xij.name()][index] != false)
                    {
                      m_is_control_flow[Xij.name()][index] = false;
                      todo.insert(Xij.name());
                      mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(false, index, X, Xij, "pass 2");
                    }
                  }
                  else
                  {
                    if (!m_is_control_flow[X.name()][found - d_X.begin()])
                    {
                      if (m_is_control_flow[Xij.name()][index] != false)
                      {
                        m_is_control_flow[Xij.name()][index] = false;
                        todo.insert(Xij.name());
                        mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(false, index, X, Xij, "pass 2");
                      }
                    }
                  }
                }
              }
              else
              {
                if (m_is_control_flow[Xij.name()][index] != false)
                {
                  m_is_control_flow[Xij.name()][index] = false;
                  todo.insert(Xij.name());
                  mCRL2log(log::debug, "control_flow") << print_control_flow_assignment(false, index, X, Xij, "pass 2");
                }
              }
            }
          }
        }
      }
    }

    const std::vector<bool>& control_flow_values(const core::identifier_string& X) const
    {
      std::map<core::identifier_string, std::vector<bool> >::const_iterator i = m_is_control_flow.find(X);
      assert (i != m_is_control_flow.end());
      return i->second;
    }

    // returns the control flow parameters of the propositional variable with name X
    std::set<data::variable> control_flow_parameters(const core::identifier_string& X) const
    {
      std::set<data::variable> result;
      const std::vector<bool>& b = control_flow_values(X);
      const pfnf_equation& eqn = *find_equation(m_pbes, X);
      const atermpp::vector<data::variable>& d = eqn.parameters();
      std::size_t index = 0;
      for (atermpp::vector<data::variable>::const_iterator i = d.begin(); i != d.end(); ++i, index++)
      {
        if (b[index])
        {
          result.insert(*i);
        }
      }
      return result;
    }

    // returns true if the i-th parameter of X is a control flow parameter
    bool is_control_flow_parameter(const core::identifier_string& X, std::size_t i) const
    {
      return control_flow_values(X)[i];
    }

    // returns the parameters of the propositional variable with name X
    std::set<data::variable> propvar_parameters(const core::identifier_string& X) const
    {
      const pfnf_equation& eqn = *find_equation(m_pbes, X);
      const atermpp::vector<data::variable>& d = eqn.parameters();
      return std::set<data::variable>(d.begin(), d.end());
    }

    // removes parameter values that do not correspond to a control flow parameter
    propositional_variable_instantiation project(const propositional_variable_instantiation& x) const
    {
      core::identifier_string X = x.name();
      data::data_expression_list d_X = x.parameters();
      const std::vector<bool>& b = control_flow_values(X);
      std::size_t index = 0;
      std::vector<data::data_expression> d;
      for (data::data_expression_list::iterator i = d_X.begin(); i != d_X.end(); ++i, index++)
      {
        if (b[index])
        {
          d.push_back(*i);
        }
      }
      return propositional_variable_instantiation(X, data::data_expression_list(d.begin(), d.end()));
    }

    // removes parameter values that do not correspond to a control flow parameter
    propositional_variable project_variable(const propositional_variable& x) const
    {
      core::identifier_string X = x.name();
      data::variable_list d_X = x.parameters();
      const std::vector<bool>& b = control_flow_values(X);
      std::size_t index = 0;
      std::vector<data::variable> d;
      for (data::variable_list::iterator i = d_X.begin(); i != d_X.end(); ++i, index++)
      {
        if (b[index])
        {
          d.push_back(*i);
        }
      }
      return propositional_variable(X, data::variable_list(d.begin(), d.end()));
    }

    // x is a projected value
    // \pre x is not present in m_control_vertices
    vertex_iterator insert_control_flow_vertex(const propositional_variable_instantiation& X)
    {
      std::pair<vertex_iterator, bool> p = m_control_vertices.insert(std::make_pair(X, control_flow_vertex(X)));
      assert(p.second);
      return p.first;
    }

    template <typename Substitution>
    propositional_variable_instantiation apply_substitution(const propositional_variable_instantiation& X, Substitution sigma) const
    {
      return propositional_variable_instantiation(X.name(), data::replace_free_variables(X.parameters(), sigma));
    }

    void compute_control_flow_graph()
    {
      compute_control_flow_parameters();

      data::rewriter datar(m_pbes.data());
      pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(datar);

      std::set<control_flow_vertex*> todo;

      // handle the initial state
      propositional_variable_instantiation Xinit = project(m_pbes.initial_state());
      vertex_iterator i = insert_control_flow_vertex(Xinit);
      todo.insert(&(i->second));
      mCRL2log(log::debug, "control_flow") << "[cf] Xinit = " << pbes_system::pp(m_pbes.initial_state()) << " -> " << pbes_system::pp(Xinit) << std::endl;

      while (!todo.empty())
      {
        std::set<control_flow_vertex*>::iterator i = todo.begin();
        todo.erase(i);
        control_flow_vertex& u = **i;
        control_flow_vertex* source = &u;
        mCRL2log(log::debug, "control_flow") << "[cf] selected todo element " << pbes_system::pp(u.X) << std::endl;

        const pfnf_equation& eqn = *find_equation(m_pbes, u.X.name());
        propositional_variable X = project_variable(eqn.variable());
        mCRL2log(log::debug, "control_flow") << "[cf] X = " << pbes_system::pp(X) << std::endl;
        mCRL2log(log::debug, "control_flow") << "[cf] u.X = " << pbes_system::pp(u.X) << std::endl;
        data::variable_list d = X.parameters();
        data::data_expression_list e = u.X.parameters();
        data::sequence_sequence_substitution<data::variable_list, data::data_expression_list> sigma(d, e);
        mCRL2log(log::debug, "control_flow") << "[cf] sigma = " << data::print_substitution(sigma) << std::endl;
        const atermpp::vector<pfnf_implication>& implications = eqn.implications();
        if (implications.empty())
        {
          u.guards.insert(eqn.h());
        }
        for (atermpp::vector<pfnf_implication>::const_iterator i = implications.begin(); i != implications.end(); ++i)
        {
          const atermpp::vector<propositional_variable_instantiation>& propvars = i->variables();
          pbes_expression guard = pbesr(and_(eqn.h(), i->g()), sigma);
          if (is_false(guard))
          {
            continue;
          }

          for (atermpp::vector<propositional_variable_instantiation>::const_iterator j = propvars.begin(); j != propvars.end(); ++j)
          {
            mCRL2log(log::debug, "control_flow") << "[cf] Xij = " << pbes_system::pp(*j) << std::endl;
            propositional_variable_instantiation Xij = apply_substitution(*j, sigma);
            mCRL2log(log::debug, "control_flow") << "[cf] Xij_sigma = " << pbes_system::pp(Xij) << std::endl;
            propositional_variable_instantiation Y = project(Xij);
            mCRL2log(log::debug, "control_flow") << "[cf] Xij_sigma_projected = " << pbes_system::pp(Y) << std::endl;
            propositional_variable_instantiation label = Xij;
            vertex_iterator q = m_control_vertices.find(Y);
            if (q == m_control_vertices.end())
            {
              // vertex Y does not yet exist
              mCRL2log(log::debug, "control_flow") << "[cf] discovered " << pbes_system::pp(Y) << std::endl;
              vertex_iterator k = insert_control_flow_vertex(Y);
              control_flow_vertex& v = k->second;
              u.guards.insert(guard);
              todo.insert(&v);
              control_flow_vertex* target = &v;
              control_flow_edge e(source, target, label);
              mCRL2log(log::debug, "control_flow") << "[cf] insert edge " << e.print() << std::endl;
              u.outgoing_edges.insert(e);
              v.incoming_edges.insert(e);
            }
            else
            {
              control_flow_vertex& v = q->second;
              u.guards.insert(guard);
              control_flow_vertex* target = &v;
              control_flow_edge e(source, target, label);
              mCRL2log(log::debug, "control_flow") << "[cf] insert edge " << e.print() << std::endl;
              u.outgoing_edges.insert(e);
              v.incoming_edges.insert(e);
            }
          }
        }
      }
    }

    void print_control_flow_graph() const
    {
      std::cout << "--- control flow graph ---" << std::endl;
      for (atermpp::map<propositional_variable_instantiation, control_flow_vertex>::const_iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        std::cout << "vertex " << i->second.print() << std::endl;
      }
    }

    std::string print_variable_set(const std::set<data::variable>& v) const
    {
      std::ostringstream out;
      out << "{";
      for (std::set<data::variable>::const_iterator j = v.begin(); j != v.end(); ++j)
      {
        if (j != v.begin())
        {
          out << ", ";
        }
        out << data::pp(*j);
      }
      out << "}";
      return out.str();
    }

    std::string print_control_flow_marking(const control_flow_vertex& v) const
    {
      std::ostringstream out;
      out << "vertex " << pbes_system::pp(v.X) << " = " << print_variable_set(v.marking);
      return out.str();
    }

    void print_control_flow_marking() const
    {
      std::cout << "--- control flow marking ---" << std::endl;
      for (atermpp::map<propositional_variable_instantiation, control_flow_vertex>::const_iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        const control_flow_vertex& v = i->second;
        std::cout << print_control_flow_marking(v) << std::endl;
      }
    }

    void compute_control_flow_marking()
    {
      // initialization
      for (atermpp::map<propositional_variable_instantiation, control_flow_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        control_flow_vertex& v = i->second;
        std::set<data::variable> fv = v.free_guard_variables();
        std::set<data::variable> dx = propvar_parameters(v.X.name());
        v.marking = data::detail::set_intersection(fv, dx);
        mCRL2log(log::debug, "control_flow") << "initial marking " << print_control_flow_marking(v) << "\n";
      }

      // backwards reachability algorithm
      std::set<control_flow_vertex*> todo;
      for (atermpp::map<propositional_variable_instantiation, control_flow_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        control_flow_vertex& v = i->second;
        todo.insert(&v);
      }
      while (!todo.empty())
      {
        std::set<control_flow_vertex*>::iterator i = todo.begin();
        todo.erase(i);
        control_flow_vertex& v = **i;
        mCRL2log(log::debug, "control_flow") << "selected marking todo element " << pbes_system::pp(v.X) << std::endl;
        std::set<std::size_t> I = v.marking_variable_indices(m_pbes);

        for (atermpp::set<control_flow_edge>::iterator i = v.incoming_edges.begin(); i != v.incoming_edges.end(); ++i)
        {
          control_flow_vertex& u = *(i->source);
          std::size_t last_size = u.marking.size();
          const propositional_variable_instantiation& Xij = i->label;
          std::set<data::variable> dx = propvar_parameters(Xij.name());
          for (std::set<std::size_t>::const_iterator j = I.begin(); j != I.end(); ++j)
          {
            std::size_t m = *j;
            data::data_expression_list e = Xij.parameters();
            data::data_expression_list::const_iterator k = e.begin();
            for (std::size_t p = 0; p < m; ++p)
            {
              ++k;
            }
            data::data_expression e_m = *k;
            std::set<data::variable> fv = pbes_system::find_free_variables(e_m);
            u.marking = data::detail::set_union(data::detail::set_intersection(fv, dx), u.marking);
          }
          if (u.marking.size() > last_size)
          {
            todo.insert(&u);
            mCRL2log(log::debug, "control_flow") << "updated marking " << print_control_flow_marking(u) << " using edge " << pbes_system::pp(Xij) << "\n";
          }
        }
      }

      // set the marking_parameters attributes
      for (atermpp::map<propositional_variable_instantiation, control_flow_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        control_flow_vertex& v = i->second;
        const pfnf_equation& eqn = *find_equation(m_pbes, v.X.name());
        const atermpp::vector<data::variable>& d = eqn.parameters();
        for (atermpp::vector<data::variable>::const_iterator j = d.begin(); j != d.end(); ++j)
        {
          v.marked_parameters.push_back(v.marking.find(*j) != v.marking.end());
        }
      }
    }

    data::data_expression default_value(const data::sort_expression& x)
    {
      // TODO: make this an attribute
      data::representative_generator f(m_pbes.data());
      return f(x);
    }

    // generates a PBES from the control flow graph and the marking
    pbes<> reset_variables(bool simplify)
    {
      pbes<> result;
      result.initial_state() = m_pbes.initial_state();
      result.data() = m_pbes.data();
      data::rewriter datar(m_pbes.data());
      simplifying_rewriter<pbes_expression, data::rewriter> pbesr(m_pbes.data());

      // create an index for all vertices in the control flow graph with a given name
      std::map<core::identifier_string, std::set<control_flow_vertex*> > propvar_map;
      for (atermpp::map<propositional_variable_instantiation, control_flow_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        control_flow_vertex& v = i->second;
        propvar_map[v.X.name()].insert(&v);
      }

      // expand the equations, and add them to the result
      atermpp::vector<pfnf_equation>& equations = m_pbes.equations();
      for (atermpp::vector<pfnf_equation>::iterator k = equations.begin(); k != equations.end(); ++k)
      {
        mCRL2log(log::debug, "control_flow") << "<reset equation>" << pbes_system::pp(k->convert()) << std::endl;
        atermpp::vector<pfnf_implication>& implications = k->implications();
        atermpp::vector<pbes_expression> new_implications;
        for (atermpp::vector<pfnf_implication>::iterator i = implications.begin(); i != implications.end(); ++i)
        {
          atermpp::vector<propositional_variable_instantiation>& v = i->variables();
          atermpp::vector<pbes_expression> disjuncts;
          for (atermpp::vector<propositional_variable_instantiation>::iterator j = v.begin(); j != v.end(); ++j)
          {
            mCRL2log(log::debug, "control_flow") << "<expand propvar>" << pbes_system::pp(*j) << std::endl;
            atermpp::vector<pbes_expression> Xij_conjuncts;
            core::identifier_string X = j->name();
            std::vector<data::data_expression> d_X = atermpp::convert<std::vector<data::data_expression> >(j->parameters());

            // iterate over the alternatives as defined by the control flow graph
            std::set<control_flow_vertex*>& inst = propvar_map[X];
            for (std::set<control_flow_vertex*>::iterator q = inst.begin(); q != inst.end(); ++q)
            {
              control_flow_vertex& w = **q;
              mCRL2log(log::debug, "control_flow") << "  <vertex>" << pbes_system::pp(w.X) << std::endl;
              atermpp::vector<data::data_expression> e;
              std::size_t N = w.marked_parameters.size();
              data::data_expression_list::const_iterator s = w.X.parameters().begin();
              data::data_expression condition = data::sort_bool::true_();
              for (std::size_t r = 0; r < N; ++r)
              {
                if (is_control_flow_parameter(X, r))
                {
                  data::data_expression v_X_r = *s++;
                  condition = data::lazy::and_(condition, data::equal_to(d_X[r], v_X_r));
                  e.push_back(v_X_r);
                }
                else if (w.is_marked_parameter(r))
                {
                  e.push_back(d_X[r]);
                }
                else
                {
                  e.push_back(default_value(d_X[r].sort()));
                }
              }
              propositional_variable_instantiation Xe(X, atermpp::convert<data::data_expression_list>(e));
              mCRL2log(log::debug, "control_flow") << "  <alternative>" << pbes_system::pp(Xe) << std::endl;
              if (simplify)
              {
                condition = datar(condition);
                if (condition != data::sort_bool::false_())
                {
                  Xij_conjuncts.push_back(imp(condition, Xe));
                }
              }
              else
              {
                Xij_conjuncts.push_back(imp(condition, Xe));
              }
            }
            disjuncts.push_back(pbes_expr::join_and(Xij_conjuncts.begin(), Xij_conjuncts.end()));
          }
          pbes_expression dj = pbes_expr::join_or(disjuncts.begin(), disjuncts.end());
          if (simplify)
          {
            new_implications.push_back(utilities::optimized_imp(pbesr(i->g()), dj));
          }
          else
          {
            new_implications.push_back(imp(i->g(), dj));
          }
        }
        pbes_expression phi = pbes_expr::join_and(new_implications.begin(), new_implications.end());
        pbes_equation eqn = k->apply_implication(phi);
        if (!eqn.is_well_typed())
        {
          std::cerr << "<eqn not well typed>" << pbes_system::pp(eqn) << std::endl;
        }
        result.equations().push_back(eqn);
      }

      return result;
    }

  public:

    /// \brief Runs the control_flow algorithm
    pbes<> run(bool simplify = true, bool print_output = true)
    {
      //control_flow_influence_graph_algorithm ialgo(m_pbes);
      //ialgo.run();

      control_flow_source_dest_algorithm sdalgo(m_pbes);
      sdalgo.compute_source_destination();
      // sdalgo.print_source_destination();
      sdalgo.rewrite_propositional_variables();
      // N.B. This modifies m_pbes. It is needed as a precondition for the
      // function compute_control_flow_parameters().

      compute_control_flow_graph();
      if (print_output)
      {
        print_control_flow_parameters();
        print_control_flow_graph();
      }

      compute_control_flow_marking();
      if (print_output)
      {
        print_control_flow_marking();
      }

      return reset_variables(simplify);
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_CONTROL_FLOW_H
