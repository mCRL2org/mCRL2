// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriter.h
/// \brief Rewriters for pbes expressions.

#ifndef MCRL2_PBES_REWRITER_H
#define MCRL2_PBES_REWRITER_H

#include <set>
#include <utility>
#include <vector>
#include "mcrl2/core/print.h"
#include "mcrl2/core/data_implementation.h"
#include "mcrl2/old_data/find.h"
#include "mcrl2/old_data/rewriter.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/pbes/gauss.h"
#include "mcrl2/pbes/detail/simplify_rewrite_builder.h"
#include "mcrl2/pbes/detail/enumerate_quantifiers_builder.h"

namespace mcrl2 {

namespace pbes_system {

  /// A rewriter that simplifies expressions.
  ///
  template <typename DataRewriter>
  class simplify_rewriter
  {
    protected:
      DataRewriter& m_rewriter;
    
    public:
      simplify_rewriter(DataRewriter& rewriter)
        : m_rewriter(rewriter)
      {}
      
      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \return The rewrite result.
      ///
      pbes_expression operator()(const pbes_expression& x)
      {
        detail::simplify_rewrite_builder<DataRewriter, int> visitor(m_rewriter);
        std::pair<int, bool> dummy(0, false);
        return visitor.visit(x, dummy);
      }

      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \param[in] sigma A sequence of rewriter substitutions that is applied during rewriting.
      /// \return A pair (r, b), where r is the rewrite result, and b denotes whether or not r depends
      /// on sigma.
      ///
      template <typename DataSubstitutionRange>
      std::pair<pbes_expression, bool> operator()(const pbes_expression& x, const DataSubstitutionRange& sigma)
      {
        detail::simplify_rewrite_builder<DataRewriter, DataSubstitutionRange> visitor(m_rewriter);
        bool b = false;
        pbes_expression result = visitor.visit(x, std::make_pair(sigma, b));
        return std::make_pair(result, b);
      }
  };

  /// A rewriter that simplifies expressions and eliminates quantifiers using enumeration.
  ///
  template <typename DataRewriter, typename DataEnumerator>
  class enumerate_quantifiers_rewriter
  {
    protected:
      DataRewriter& m_rewriter;
      DataEnumerator& m_enumerator;

    public:
      enumerate_quantifiers_rewriter(DataRewriter& r, DataEnumerator& e)
        : m_rewriter(r), m_enumerator(e)
      {}
      
      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \return The rewrite result.
      ///
      pbes_expression operator()(const pbes_expression& x)
      {
        detail::enumerate_quantifiers_builder<DataRewriter, DataEnumerator> visitor(m_rewriter, m_enumerator);     
        typename detail::enumerate_quantifiers_builder<DataRewriter, DataEnumerator>::argument_type arg;
        return visitor.visit(x, arg);
      }
      
      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \param[in] sigma A sequence of rewriter substitutions that is applied during rewriting.
      /// \return A pair (r, b), where r is the rewrite result, and b denotes whether or not r depends
      /// on sigma.
      ///
      template <typename SubstitutionSequence>
      std::pair<pbes_expression, bool> operator()(const pbes_expression& x, SubstitutionSequence& sigma)
      {
        detail::enumerate_quantifiers_builder<DataRewriter, DataEnumerator> visitor(m_rewriter, m_enumerator);
        bool b = false;
        pbes_expression result = visitor.visit(x, std::make_pair(sigma, b));
        return std::make_pair(result, b);
      }
  };
 
  /// The simplifying pbes rewriter used in pbes2bool.
  class simplify_rewriter_jfg
  {
    old_data::rewriter datar;
    
    public:
      simplify_rewriter_jfg(const old_data::data_specification& data)
        : datar(data)
      { }
      
      /// Rewrites a pbes expression.
      pbes_expression operator()(pbes_expression p)
      {
        return pbes_expression_rewrite_and_simplify(p, datar.get_rewriter());
      }
  };
  
  /// The substituting pbes rewriter used in pbes2bool.
  class substitute_rewriter_jfg
  {
    old_data::rewriter& datar_;
    const old_data::data_specification& data_spec;
   
    public:
      substitute_rewriter_jfg(old_data::rewriter& datar, const old_data::data_specification& data)
        : datar_(datar), data_spec(data)
      { }
      
      /// Rewrites a pbes expression.
      pbes_expression operator()(pbes_expression p)
      {
        return pbes_expression_substitute_and_rewrite(p, data_spec, datar_.get_rewriter(), false);
      }   
  };
  
  /// A pbes rewriter that uses a bdd based prover internally.
  class pbessolve_rewriter
  {
    old_data::rewriter& datar_;
    const old_data::data_specification& data_spec;
    int n;
    data_variable_list fv;
    boost::shared_ptr<BDD_Prover> prover;
   
    public:
      pbessolve_rewriter(old_data::rewriter& datar, const old_data::data_specification& data, RewriteStrategy rewrite_strategy, SMT_Solver_Type solver_type)
        : datar_(datar),
          data_spec(data),
          n(0),
          prover(new BDD_Prover(data_spec, rewrite_strategy, 0, false, solver_type, false))
      { }
  
      /// Rewrites a pbes expression.
      pbes_expression operator()(pbes_expression p)
      {
        return pbes_expression_simplify(p, &n, &fv, prover.get());
      }   
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITER_H
