// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/parity_game_generator_deprecated.h
/// \brief A class for generating a parity game from a pbes. This version is
///        more efficient that the one in mcrl2/pbes/parity_game_generator.h

#ifndef MCRL2_PBES_PARITY_GAME_GENERATOR_DEPRECATED_H
#define MCRL2_PBES_PARITY_GAME_GENERATOR_DEPRECATED_H

#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <utility>
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/data/detail/rewriter_wrapper.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/parity_game_generator.h"

#include "mcrl2/bes/bes_deprecated.h" // We use some implementation tricks from this

namespace mcrl2
{

namespace pbes_system
{

/// \brief Parity game generator which uses the internal rewrite format of the
/// legacy rewriters. This is more efficient than parity_game_generator
class parity_game_generator_deprecated: public parity_game_generator
{
  protected:
    /// \brief Type used internally for storing equations
    typedef atermpp::aterm_appl internal_equation_t;

    /// \brief Whether to translation pbes_equations to internal format or not.
    /// Note that this does not work in debug mode.
    bool m_precompile_pbes;

    /// \brief Rewriter used internally to achieve better performance
    data::detail::legacy_rewriter datar_internal;

    /// \brief Maps propositional variables to corresponding PBES equations.
    std::map<core::identifier_string, std::vector<internal_equation_t>::const_iterator > m_pbes_equation_index;

    /// \brief Stores an internal representation of equations
    std::vector<internal_equation_t> m_internal_equations;
public:
    pbes_expression from_rewrite_format(const pbes_expression& e)
    {
      pbes_expression result;
      if(!m_precompile_pbes)
      {
        result = e;
      }
      else if(is_pbes_true(e) || is_pbes_false(e))
      {
        result = e;
      }
      else if(tr::is_and(e))
      {
        result = tr::and_(from_rewrite_format(tr::left(e)), from_rewrite_format(tr::right(e)));
      }
      else if(tr::is_or(e))
      {
        result = tr::or_(from_rewrite_format(tr::left(e)), from_rewrite_format(tr::right(e)));
      }
      else if(tr::is_prop_var(e))
      {
        tr::data_term_sequence_type args = tr::param(e);
        data::data_expression_vector pretty_args;
        for(tr::data_term_sequence_type::const_iterator i = args.begin(); i != args.end(); ++i)
        {
          pretty_args.push_back(datar_internal.convert_from((atermpp::aterm_appl)*i));
        }
        result = tr::prop_var(tr::name(e), pretty_args.begin(), pretty_args.end());
      }
      else if(tr::is_forall(e))
      {
        tr::variable_sequence_type params = tr::var(e);
        data::variable_vector pretty_args;
        for(auto i = params.begin(); i != params.end(); ++i)
        {
          data::data_expression d = datar_internal.convert_from((atermpp::aterm_appl)*i);
          const data::variable& vd = core::static_down_cast<const data::variable&>(d);
          pretty_args.push_back(vd);
        }
        pbes_expression arg = from_rewrite_format(tr::arg(e));
        result = tr::forall(tr::variable_sequence_type(pretty_args.begin(), pretty_args.end()), arg);
      }
      else if(tr::is_exists(e))
      {
        tr::variable_sequence_type params = tr::var(e);
        data::variable_vector pretty_args;
        for(auto i = params.begin(); i != params.end(); ++i)
        {
          data::data_expression d = datar_internal.convert_from((atermpp::aterm_appl)*i);
          const data::variable& vd = core::static_down_cast<const data::variable&>(d);
          pretty_args.push_back(vd);
        }
        pbes_expression arg = from_rewrite_format(tr::arg(e));
        result = tr::exists(tr::variable_sequence_type(pretty_args.begin(), pretty_args.end()), arg);
      }
      else
      {
        result = datar_internal.convert_from((atermpp::aterm_appl)e);
      }
      return result;
    }
public:
    virtual
    std::string print(const pbes_expression& e)
    {
      if (m_precompile_pbes)
      {
        return to_string(e) + " (" + data::pp(from_rewrite_format(e)) + ")";
      }
      else
      {
        return pbes_system::pp(e);
      }
    }
public:
    virtual
    std::string data_to_string(const data::data_expression& e)
    {
      if (m_precompile_pbes)
      {
        return data::pp(from_rewrite_format(e));
      }
      else
      {
        return data::pp(e);
      }
    }
public:
    /// \brief Check whether e corresponds to true
    virtual
    bool is_true(const pbes_expression& e) const
    {
      bool result = tr::is_true(e);
      if (m_precompile_pbes)
      {
        result = result || e == datar_internal.get_rewriter().internal_true;
      }
      return result;
    }

    /// \brief Check whether e corresponds to false
    virtual
    bool is_false(const pbes_expression& e) const
    {
      bool result = tr::is_false(e);
      if (m_precompile_pbes)
      {
        result = result || e == datar_internal.get_rewriter().internal_false;
      }
      return result;
    }
protected:
    /// \brief Add mappings and equations to datar_internal
    /// Declare constructors to the rewriter to prevent unnecessary compilation for bound variables.
    // This can be removed if the jittyc compilers are not in use anymore.
    void initialize_internal_rewriter()
    {
      std::set < mcrl2::data::variable > vset=mcrl2::pbes_system::find_all_variables(m_pbes);
      std::set < mcrl2::data::variable > vfset=mcrl2::pbes_system::find_free_variables(m_pbes);
      std::set < mcrl2::data::variable > diff_set;
      std::set_difference(vfset.begin(),vfset.end(),vset.begin(),vset.end(),std::inserter(diff_set,diff_set.begin()));

      std::set < mcrl2::data::sort_expression > bounded_sorts;
      for(std::set < mcrl2::data::variable > :: const_iterator i=diff_set.begin(); i!=diff_set.end(); ++i)
      {
        bounded_sorts.insert(i->sort());
      }
      for(std::set < mcrl2::data::sort_expression > :: const_iterator i=bounded_sorts.begin(); i!=bounded_sorts.end(); ++i)
      {
        const mcrl2::data::function_symbol_vector constructors(m_pbes.data().constructors(*i));
        for (mcrl2::data::function_symbol_vector::const_iterator j = constructors.begin(); j != constructors.end(); ++j)
        {
          datar_internal.convert_to(*i);
        }
      }
    }


    /// \brief Translate equations to internal format, and store them in m_internal_equations
    void populate_internal_equations()
    {
      data::detail::legacy_rewriter::substitution_type sigma;
      data::detail::legacy_rewriter::internal_substitution_type sigma_internal;
      for (std::vector<pbes_equation>::const_iterator i = m_pbes.equations().begin(); i != m_pbes.equations().end(); ++i)
      {
        m_internal_equations.push_back(
          pbes_equation_to_aterm(
            pbes_equation(
              i->symbol(),
              i->variable(),
              rewrite_and_simplify(i->formula(),sigma,sigma_internal)
            )
           )
          );
      }
    }
public:
    /// \brief Simplify expression e.
    pbes_expression rewrite_and_simplify(
           const pbes_expression& e,
           data::detail::legacy_rewriter::substitution_type &sigma,
           data::detail::legacy_rewriter::internal_substitution_type &sigma_internal,
           const bool convert_data_to_pbes = true)
    {
      return ::bes::pbes_expression_rewrite_and_simplify(e, m_precompile_pbes, datar_internal,sigma,sigma_internal, convert_data_to_pbes);
    }

protected:
    /// \brief Substitute and rewrite e.
    pbes_expression substitute_and_rewrite(
           const pbes_expression& e,
           data::detail::legacy_rewriter::substitution_type &sigma,
           data::detail::legacy_rewriter::internal_substitution_type &sigma_internal)
    {
      pbes_expression result =  detail::pbes_expression_substitute_and_rewrite
          (e,
           m_pbes.data(),
           datar_internal,
           m_precompile_pbes,
           sigma,
           sigma_internal
          );
      return result;
    }

    /// \brief Compute equation index map.
    virtual
    void compute_equation_index_map()
    {
      for (std::vector<internal_equation_t>::const_iterator i = m_internal_equations.begin(); i != m_internal_equations.end(); ++i)
      {
        m_pbes_equation_index[pbes_equation(*i).variable().name()] = i;
      }
    }

    /// \brief Make a substitution in the internal rewriter
    void make_substitution_internal(
           const data::variable_list& v,
           const data::data_expression_list& e,
           data::detail::legacy_rewriter::substitution_type &sigma,
           data::detail::legacy_rewriter::internal_substitution_type &sigma_internal)
    {
      data::variable_list::const_iterator i = v.begin();
      for(data::data_expression_list::const_iterator j = e.begin();
          i != v.end() && j != e.end(); ++i, ++j)
      {
        if (m_precompile_pbes)
        {
          // datar_internal.set_internally_associated_value(*i,(atermpp::aterm)(*j));
          sigma_internal[*i]=*j;
        }
        else
        {
          // datar_internal.set_internally_associated_value(*i,*j);
          sigma[*i]=*j;
        }
      }
    }

    virtual
    pbes_expression expand_rhs(const pbes_expression& psi)
    {
      // expand the right hand side if needed
      if (tr::is_prop_var(psi))
      {
        const pbes_equation& pbes_eqn = *m_pbes_equation_index[tr::name(psi)];

        mCRL2log(log::debug, "parity_game_generator") << "Expanding right hand side of formula " << print(psi) << std::endl << "  rhs: " << print(pbes_eqn.formula()) << " into " << std::endl;

        pbes_expression result;

        data::detail::legacy_rewriter::substitution_type sigma;
        data::detail::legacy_rewriter::internal_substitution_type sigma_internal;
        make_substitution_internal(pbes_eqn.variable().parameters(), tr::param(psi),sigma,sigma_internal);
        result = substitute_and_rewrite(pbes_eqn.formula(),sigma,sigma_internal);

        mCRL2log(log::debug, "parity_game_generator") << print(result) << std::endl;

        return result;
      }
      return psi;
    }

    virtual
    void initialize_generation()
    {
      if (m_initialized)
      {
        return;
      }
      else
      {
                // Nothing to be done for an empty PBES.
        if (m_pbes.equations().empty())
        {
          return;
        }

        // Normalize the pbes, since the parity game generator currently doesn't handle negation and implication.

        pbes_system::algorithms::normalize(m_pbes);
        initialize_internal_rewriter();
        populate_internal_equations();
        compute_equation_index_map();
        compute_priorities(m_pbes.equations());

        // Add a BES equation for the initial state.
        data::detail::legacy_rewriter::substitution_type sigma;
        data::detail::legacy_rewriter::internal_substitution_type sigma_internal;
        propositional_variable_instantiation phi = core::static_down_cast<const propositional_variable_instantiation&>(rewrite_and_simplify(m_pbes.initial_state(),sigma,sigma_internal));
        add_bes_equation(phi, m_priorities[phi.name()]);

        m_initialized = true;
      }
    }

  public:
    using parity_game_generator::PGAME_OR;
    using parity_game_generator::PGAME_AND;

    /// \brief Constructor.
    /// \param p A PBES
    /// \param true_false_dependencies If true, nodes are generated for the values <tt>true</tt> and <tt>false</tt>.
    /// \param is_min_parity If true a min-parity game is produced, otherwise a max-parity game
    /// \param rewrite_strategy Strategy to use for the data rewriter
    parity_game_generator_deprecated(pbes& p, bool true_false_dependencies = false, bool is_min_parity = true, data::rewriter::strategy rewrite_strategy = data::jitty)
      :
      parity_game_generator(p, true_false_dependencies, is_min_parity, rewrite_strategy),
      datar_internal(datar)
    {
#ifdef NDEBUG
      m_precompile_pbes = true;
      mCRL2log(log::verbose) << "Using precompiled PBES" << std::endl;
#else
      m_precompile_pbes = false;
#endif
    }

    virtual ~parity_game_generator_deprecated() {}

};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PARITY_GAME_GENERATOR_H
