// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes2bes_rewriter.h
/// \brief Rewriter for the pbes2bes algorithm.

#ifndef MCRL2_PBES_DETAIL_PBES2BES_REWRITER_H
#define MCRL2_PBES_DETAIL_PBES2BES_REWRITER_H

#include "mcrl2/atermpp/map.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/pbes/pbes_expression_with_propositional_variables.h"
#include "mcrl2/pbes/detail/enumerate_quantifiers_builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// The substitution function used by the pbes2bes rewriter.
  typedef data::rewriter_map<atermpp::map<data::data_variable, data::data_expression_with_variables> > pbes2bes_substitution_function;

  /// Simplifying PBES rewriter that eliminates quantifiers using enumeration.
  /// As a side effect propositional variable instantiations are being renamed
  /// using a rename function.
  
  template <typename DataRewriter, typename DataEnumerator>
  struct pbes2bes_rewrite_builder: public enumerate_quantifiers_builder<pbes_expression_with_propositional_variables, DataRewriter, DataEnumerator, pbes2bes_substitution_function>
  {
    typedef enumerate_quantifiers_builder<pbes_expression_with_propositional_variables, DataRewriter, DataEnumerator, pbes2bes_substitution_function> super;
    typedef typename super::term_type term_type;
    typedef typename core::term_traits<term_type>::propositional_variable_type propositional_variable_type;

    pbes2bes_rewrite_builder(DataRewriter& datar, DataEnumerator& datae)
      : super(datar, datae)
    {}

    /// Creates a unique name for a propositional variable instantiation. The
    /// propositional variable instantiation must be closed.
    /// Implemented by Alexander van Dam.
    /// \param v A propositional variable.
    /// \return A name that uniquely corresponds to the propositional variable.
    propositional_variable_type rename(const propositional_variable_type& v)
    {
      const data::data_expression_list del = v.parameters();
      std::string propvar_name_current = v.name();
      if (!del.empty())
      {
        for (data::data_expression_list::iterator del_i = del.begin(); del_i != del.end(); del_i++)
        {
          if (data::is_data_operation(*del_i))
          {
            propvar_name_current += "@";
            propvar_name_current += mcrl2::core::pp(*del_i);
          }
          else if (data::is_data_variable(*del_i))
          {
            core::gsErrorMsg("The propositional varaible contains a variable of finite sort.\n");
            core::gsErrorMsg("Can not handle variables of finite sort when creating a propositional variable name.\n");
            core::gsErrorMsg("Computation aborted.\n");
            std::cout << "Problematic Term: " << core::pp(*del_i) << std::endl;
            throw mcrl2::runtime_error("exit!");
          }
          else if (data::is_data_application(*del_i))
          {
            propvar_name_current += "@";
            propvar_name_current += mcrl2::core::pp(*del_i);
          }
          else
          {
            core::gsErrorMsg("Can not rewrite the name of the propositional_variable\n");
            std::cout << "Problematic Term: " << core::pp(*del_i) << std::endl;
            throw mcrl2::runtime_error("exit!");
          }
        }
      }   
      return propositional_variable_type(propvar_name_current, data::data_expression_list());
    }

    /// Visit propositional variable node.
    ///
    term_type visit_propositional_variable(const term_type& x, const propositional_variable_type& v, pbes2bes_substitution_function& sigma)
    {
      term_type y = super::visit_propositional_variable(x, v, sigma);
      return term_type(rename(y), y.variables(), atermpp::make_list(y));
    }
  };

  /// A rewriter that simplifies expressions and eliminates quantifiers using enumeration.
  ///
  class pbes2bes_rewriter
  {
    public:
      typedef pbes_expression_with_propositional_variables term_type;
      typedef data::data_enumerator<data::rewriter, data::number_postfix_generator> pbes2bes_enumerator;
      typedef data::data_expression_with_variables data_term_type;
      typedef data::data_variable variable_type;

      /// Constructor.
      pbes2bes_rewriter(const data_specification& data_spec)
       :
         datar(data_spec),
         name_generator("UNIQUE_PREFIX"),
         datae(data_spec, datar, name_generator)
      {}     
      
      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \return The rewrite result.
      ///
      term_type operator()(const term_type& x)
      {
        pbes2bes_substitution_function sigma;
        pbes2bes_rewrite_builder<data::rewriter, pbes2bes_enumerator> r(datar, datae);
        return r(x, sigma);
      }
      
      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \param[in] sigma A substitution function that is applied to data variables during rewriting.
      /// \return The rewrite result.
      ///
      term_type operator()(const term_type& x, pbes2bes_substitution_function& sigma)
      {
        pbes2bes_rewrite_builder<data::rewriter, pbes2bes_enumerator> r(datar, datae);
        return r(x, sigma);
      }

    protected:      
      data::rewriter datar;
      data::number_postfix_generator name_generator;
      pbes2bes_enumerator datae;     
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES2BES_REWRITER_H
