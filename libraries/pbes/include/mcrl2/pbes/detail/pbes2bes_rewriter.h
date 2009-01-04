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

#include <iostream>
#include "mcrl2/atermpp/map.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/pbes/pbes_expression_with_propositional_variables.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/enumerate_quantifiers_builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// \brief The substitution function used by the pbes2bes rewriter.
  typedef data::rewriter_map<atermpp::map<data::data_variable, data::data_expression_with_variables> > pbes2bes_substitution_function;

  /// \brief Simplifying PBES rewriter that eliminates quantifiers using enumeration.
  /// As a side effect propositional variable instantiations are being renamed
  /// using a rename function. 
  template <typename DataRewriter, typename DataEnumerator>
  struct pbes2bes_rewrite_builder: public enumerate_quantifiers_builder<pbes_expression_with_propositional_variables, DataRewriter, DataEnumerator, pbes2bes_substitution_function>
  {
    typedef enumerate_quantifiers_builder<pbes_expression_with_propositional_variables, DataRewriter, DataEnumerator, pbes2bes_substitution_function> super;
    typedef typename super::term_type term_type;
    typedef typename super::data_term_type data_term_type;
    typedef typename super::variable_sequence_type variable_sequence_type;
    typedef typename core::term_traits<term_type>::propositional_variable_type propositional_variable_type;
    typedef core::term_traits<term_type> tr;

    pbes2bes_rewrite_builder(DataRewriter& datar, DataEnumerator& datae)
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
          if (data::is_data_operation(*del_i))
          {
            propvar_name_current += "@";
            propvar_name_current += mcrl2::core::pp(*del_i);
          }
          else if (data::is_data_application(*del_i))
          {
            propvar_name_current += "@";
            propvar_name_current += mcrl2::core::pp(*del_i);
          }
          // else if (data::is_data_variable(*del_i))
          // {
          //   throw mcrl2::runtime_error(std::string("Could not rename the variable ") + core::pp(v));
          // }
          else
          {
            throw mcrl2::runtime_error(std::string("pbes2bes_rewrite_builder: could not rename the variable ") + core::pp(v));
          }
        }
      }   
      return propositional_variable_type(propvar_name_current, data::data_expression_list());
    }

    /// \brief Check if the intermediate result is constant.
    /// \param x A term
    /// \param result A term
    /// \param sigma A substitution function
    void check_result(term_type x, term_type result, pbes2bes_substitution_function& sigma)
    {
      if (result == term_type())
      {
        return;
      }
      std::set<data::data_variable> v = find_free_variables(result);
      for (std::set<data::data_variable>::iterator j = v.begin(); j != v.end(); ++j)
      {
        if (sigma.find(*j) != sigma.end())
        {
          std::cerr << "OFFENDING VARIABLE: " << core::pp(*j) << std::endl;
          std::cerr << "x = " << core::pp(x) << std::endl;
          std::cerr << "sigma = [";
          for (pbes2bes_substitution_function::iterator i = sigma.begin(); i != sigma.end(); ++i)
          {
            std::cerr << (i == sigma.begin() ? "" : ", ") << core::pp(i->first) << " := " << core::pp(i->second);
          }
          std::cerr << "]" << std::endl;
          std::cerr << "result = " << core::pp(result) << std::endl;
          break;
        }
      }
    }
  
    /// \brief Visit propositional_variable node
    /// Visit propositional variable node.
    /// \param x A term
    /// \param v A propositional variable
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_propositional_variable(const term_type& x, const propositional_variable_type& v, pbes2bes_substitution_function& sigma)
    {
      term_type y = super::visit_propositional_variable(x, v, sigma);
      term_type result = term_type(rename(y), y.variables(), atermpp::make_list(y));
      // check_result(x, result, sigma);
      return result;
    }
/*
    /// \brief Visit data_expression node
    /// Visit data expression node.
    /// \param x A term
    /// \param d A data term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_data_expression(const term_type& x, const data_term_type& d, pbes2bes_substitution_function& sigma)
    {
      term_type result = super::visit_data_expression(x, d, sigma);
      check_result(x, result, sigma);
      return result;
    }
  
    /// \brief Visit true node
    /// Visit true node.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_true(const term_type& x, pbes2bes_substitution_function& sigma)
    {
      term_type result = super::visit_true(x, sigma);
      check_result(x, result, sigma);
      return result;
    }
  
    /// \brief Visit false node
    /// Visit false node.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_false(const term_type& x, pbes2bes_substitution_function& sigma)
    {
      term_type result = super::visit_false(x, sigma);
      check_result(x, result, sigma);
      return result;
    }
  
    /// \brief Visit not node
    /// Visit not node.
    /// \param x A term
    /// \param n A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_not(const term_type& x, const term_type& n, pbes2bes_substitution_function& sigma)
    {
      term_type result = super::visit_not(x, n, sigma);
      check_result(x, result, sigma);
      return result;
    }
  
    /// \brief Visit and node
    /// Visit and node.
    /// \param x A term
    /// \param left A term
    /// \param right A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_and(const term_type& x, const term_type& left, const term_type& right, pbes2bes_substitution_function& sigma)
    {
      term_type result = super::visit_and(x, left, right, sigma);
      check_result(x, result, sigma);
      return result;
    }
  
    /// \brief Visit or node
    /// Visit or node.
    /// \param x A term
    /// \param left A term
    /// \param right A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_or(const term_type& x, const term_type& left, const term_type& right, pbes2bes_substitution_function& sigma)
    {
      term_type result = super::visit_or(x, left, right, sigma);
      check_result(x, result, sigma);
      return result;
    }
  
    /// \brief Visit imp node
    /// Visit imp node.
    /// \param x A term
    /// \param left A term
    /// \param right A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_imp(const term_type& x, const term_type& left, const term_type& right, pbes2bes_substitution_function& sigma)
    {
      term_type result = super::visit_imp(x, left, right, sigma);
      check_result(x, result, sigma);
      return result;
    }
  
    /// \brief Visit forall node
    /// Visit forall node.
    /// \param x A term
    /// \param variables A sequence of variables
    /// \param expression A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_forall(const term_type& x, const variable_sequence_type& variables, const term_type& expression, pbes2bes_substitution_function& sigma)
    {
      term_type result = super::visit_forall(x, variables, expression, sigma);
      check_result(x, result, sigma);
      return result;
    }
  
    /// \brief Visit exists node
    /// Visit exists node.
    /// \param x A term
    /// \param variables A sequence of variables
    /// \param expression A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_exists(const term_type& x, const variable_sequence_type& variables, const term_type& expression, pbes2bes_substitution_function& sigma)
    {
      term_type result = super::visit_exists(x, variables, expression, sigma);
      check_result(x, result, sigma);
      return result;
    }
  
    /// Visit propositional variable node.
    // term_type visit_propositional_variable(const term_type& x, const propositional_variable_type& v, pbes2bes_substitution_function& sigma)
    // {
    //   term_type result = super::visit_data_expression(x, v, sigma);
    //   check_result(x, result, sigma);
    //   return result;
    // }
*/    
  };

  /// A rewriter that simplifies expressions and eliminates quantifiers using enumeration.
  class pbes2bes_rewriter
  {
    public:
      typedef pbes_expression_with_propositional_variables term_type;
      typedef data::data_enumerator<data::number_postfix_generator> pbes2bes_enumerator;
      typedef data::data_expression_with_variables data_term_type;
      typedef data::data_variable variable_type;

      /// \brief Constructor.
      /// \param data_spec A data specification
      /// \param rewriter_strategy A rewriter strategy
      /// \param print_rewriter_output If true, rewriter output is printed to standard error
      pbes2bes_rewriter(data::data_specification& data_spec, data::rewriter::strategy rewriter_strategy = data::rewriter::jitty, bool print_rewriter_output = false)
       :
         datar(data_spec, rewriter_strategy),
         datarv(data_spec),
         name_generator("UNIQUE_PREFIX"),
         datae(data_spec, datar, name_generator),
         m_print_rewriter_output(print_rewriter_output)
      {}     
      
      /// \brief Rewrites a pbes expression.
      /// \param x A term
      /// \return The rewrite result.
      term_type operator()(const term_type& x)
      {
        pbes2bes_substitution_function sigma;
        pbes2bes_rewrite_builder<data::rewriter_with_variables, pbes2bes_enumerator> r(datarv, datae);
        term_type result = r(x, sigma);
        if (m_print_rewriter_output)
        {
          std::cerr << core::pp(x) << " -> " << core::pp(result) << std::endl;
        }
        return result;
      }

      /// \brief Rewrites a pbes expression.
      /// \param x A term
      /// \param sigma A substitution function
      /// \return The rewrite result.
      term_type operator()(const term_type& x, pbes2bes_substitution_function& sigma)
      {
        pbes2bes_rewrite_builder<data::rewriter_with_variables, pbes2bes_enumerator> r(datarv, datae);
        term_type result = r(x, sigma);
        if (m_print_rewriter_output)
        {
          std::cerr << core::pp(x) << "   " << sigma.to_string() << " -> " << core::pp(result) << std::endl;
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
        pbes2bes_rewrite_builder<data::rewriter_with_variables, pbes2bes_enumerator> r(datarv, datae);
        return r.rename(v);
      }

    protected:      
      data::rewriter datar;
      data::rewriter_with_variables datarv;
      data::number_postfix_generator name_generator;
      pbes2bes_enumerator datae;
      bool m_print_rewriter_output;
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES2BES_REWRITER_H
