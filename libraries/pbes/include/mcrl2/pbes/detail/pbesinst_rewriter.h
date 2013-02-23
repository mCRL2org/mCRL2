// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbesinst_rewriter.h
/// \brief Rewriter for the pbesinst algorithm.

#ifndef MCRL2_PBES_DETAIL_PBESINST_REWRITER_H
#define MCRL2_PBES_DETAIL_PBESINST_REWRITER_H

#include <iostream>
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/pbes/pbes_expression_with_propositional_variables.h"
#include "mcrl2/pbes/detail/enumerate_quantifiers_builder.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// \brief The substitution function used by the pbesinst rewriter.
typedef data::mutable_map_substitution<std::map<data::variable, data::data_expression_with_variables> > pbesinst_substitution_function;

/// \brief Simplifying PBES rewriter that eliminates quantifiers using enumeration.
/// As a side effect propositional variable instantiations are being renamed
/// using a rename function.
template <typename DataRewriter, typename DataEnumerator>
struct pbesinst_rewrite_builder: public enumerate_quantifiers_builder<pbes_expression_with_propositional_variables, DataRewriter, DataEnumerator, pbesinst_substitution_function>
{
  typedef enumerate_quantifiers_builder<pbes_expression_with_propositional_variables, DataRewriter, DataEnumerator, pbesinst_substitution_function> super;
  typedef typename super::term_type term_type;
  typedef typename super::data_term_type data_term_type;
  typedef typename super::variable_sequence_type variable_sequence_type;
  typedef typename core::term_traits<term_type>::propositional_variable_type propositional_variable_type;
  typedef core::term_traits<term_type> tr;

  pbesinst_rewrite_builder(DataRewriter& datar, DataEnumerator& datae)
    : super(datar, datae)
  {}

  /// \brief Creates a unique name for a propositional variable instantiation. The
  /// propositional variable instantiation must be closed.
  /// Implemented by Alexander van Dam.
  /// \param v A term
  /// \return A name that uniquely corresponds to the propositional variable.
  term_type rename(const term_type& v)
  {
    assert(tr::is_prop_var(v));
    if (!tr::is_constant(v))
    {
      return v;
    }
    const data::data_expression_list del = tr::param(v);
    std::string propvar_name_current = tr::name(v);
    if (!del.empty())
    {
      for (data::data_expression_list::iterator del_i = del.begin(); del_i != del.end(); del_i++)
      {
        if (is_function_symbol(*del_i))
        {
          propvar_name_current += "@";
          propvar_name_current += mcrl2::data::pp(*del_i);
        }
        else if (is_application(*del_i))
        {
          propvar_name_current += "@";
          propvar_name_current += mcrl2::data::pp(*del_i);
        }
        else if (is_abstraction(*del_i)) // case added by Wieger, 24-05-2011
        {
          propvar_name_current += "@";
          propvar_name_current += mcrl2::data::pp(*del_i);
        }
        // else if (data::is_variable(*del_i))
        // {
        //   throw mcrl2::runtime_error(std::string("Could not rename the variable ") + data::pp(v));
        // }
        else
        {
          throw mcrl2::runtime_error(std::string("pbesinst_rewrite_builder: could not rename the variable ") + pbes_system::pp(v) + " " + data::pp(*del_i) + " " + to_string(*del_i));
        }
      }
    }
    return propositional_variable_type(propvar_name_current, data::data_expression_list());
  }

  /// \brief Visit propositional_variable node
  /// Visit propositional variable node.
  /// \param x A term
  /// \param v A propositional variable
  /// \param sigma A substitution function
  /// \return The result of visiting the node
  term_type visit_propositional_variable(const term_type& x, const propositional_variable_type& v, pbesinst_substitution_function& sigma)
  {
    term_type y = super::visit_propositional_variable(x, v, sigma);
    term_type result = term_type(rename(y), y.variables(), 
                                 atermpp::aterm_cast<propositional_variable_instantiation_list>(atermpp::make_list(atermpp::aterm_appl(y))));
    return result;
  }
};

/// A rewriter that simplifies expressions and eliminates quantifiers using enumeration.
class pbesinst_rewriter
{
  public:
    typedef pbes_expression_with_propositional_variables term_type;
    typedef data::data_expression_with_variables data_term_type;
    typedef data::variable variable_type;

    /// \brief Constructor.
    /// \param data_spec A data specification
    /// \param rewriter_strategy A rewriter strategy
    /// \param print_rewriter_output If true, rewriter output is printed to standard error
    pbesinst_rewriter(data::data_specification const& data_spec, data::rewriter::strategy rewriter_strategy = data::jitty, bool print_rewriter_output = false)
      :
      datar(data_spec, rewriter_strategy),
      datarv(data_spec),
      datae(data_spec, datar),
      m_print_rewriter_output(print_rewriter_output)
    {}

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    term_type operator()(const term_type& x)
    {
      pbesinst_substitution_function sigma;
      pbesinst_rewrite_builder<data::rewriter_with_variables, data::data_enumerator> r(datarv, datae);
      term_type result = r(x, sigma);
      if (m_print_rewriter_output)
      {
        mCRL2log(log::info) << pbes_system::pp(x) << " [default]-> " << pbes_system::pp(result) << std::endl;
      }
      return result;
    }

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result.
    term_type operator()(const term_type& x, pbesinst_substitution_function& sigma)
    {
      pbesinst_rewrite_builder<data::rewriter_with_variables, data::data_enumerator> r(datarv, datae);
      term_type result = r(x, sigma);
      if (m_print_rewriter_output)
      {
        mCRL2log(log::info) << pbes_system::pp(x) << "   " << data::print_substitution(sigma) << " [subst]-> " << pbes_system::pp(result) << std::endl;
      }
      return result;
    }

    /// \brief Returns the flag for debug information.
    /// If this flag is set, rewriter output is printed to standard out.
    /// \return The flag for debug information.
    bool& print_rewriter_output()
    {
      return m_print_rewriter_output;
    }

    /// \brief Creates a unique name for a propositional variable instantiation. The
    /// propositional variable instantiation must be closed.
    /// Implemented by Alexander van Dam.
    /// \param v A term
    /// \return A name that uniquely corresponds to the propositional variable.
    term_type rename(const term_type& v)
    {
      pbesinst_rewrite_builder<data::rewriter_with_variables, data::data_enumerator> r(datarv, datae);
      return r.rename(v);
    }

  protected:
    data::rewriter datar;
    data::rewriter_with_variables datarv;
    data::data_enumerator datae;
    bool m_print_rewriter_output;
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBESINST_REWRITER_H
