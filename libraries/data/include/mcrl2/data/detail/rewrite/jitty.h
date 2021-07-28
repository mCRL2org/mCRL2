// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_DATA_DETAIL_REWRITE_JITTY_H
#define MCRL2_DATA_DETAIL_REWRITE_JITTY_H

#include "mcrl2/atermpp/standard_containers/vector.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/strategy_rule.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

class RewriterJitty: public Rewriter
{
  public:
    typedef Rewriter::substitution_type substitution_type;

    RewriterJitty(const data_specification& data_spec, const used_data_equation_selector &);
    virtual ~RewriterJitty();

    rewrite_strategy getStrategy();

    data_expression rewrite(const data_expression &term, substitution_type &sigma);

    RewriterJitty& operator=(const RewriterJitty& other)=delete;

    void increase_rewrite_stack(std::size_t distance) 
    {
      if (m_rewrite_stack.capacity()<1000)
      {
        m_rewrite_stack.reserve(1000);
      }
      m_rewrite_stack.resize(m_rewrite_stack.size()+distance);
    }

    void decrease_rewrite_stack(std::size_t distance)
    {
      m_rewrite_stack.resize(m_rewrite_stack.size()-distance);
    }

    void set_element_in_rewrite_stack(std::size_t pos, std::size_t frame_size, const data_expression& d)
    {
      assert(m_top_of_the_stack>pos);
      m_rewrite_stack[m_rewrite_stack.size()-frame_size+pos]=d;
    }

    const data_expression& get_element_from_rewrite_stack(std::size_t pos, std::size_t frame_size) const
    {
      assert(m_top_of_the_stack>pos);
      return m_rewrite_stack.at(m_rewrite_stack.size()-frame_size+pos);
    }

    atermpp::vector<data_expression>& rewrite_stack()
    {
      return m_rewrite_stack;
    } 

  private:
    atermpp::vector<data_expression> m_rewrite_stack;     // Stack for intermediate rewrite results.

    std::map< function_symbol, data_equation_list > jitty_eqns;
    std::vector<strategy> jitty_strat;

    template <class ITERATOR>
    data_expression apply_cpp_code_to_higher_order_term(
                  const application& t,
                  const std::function<data_expression(const data_expression&)> rewrite_cpp_code,
                  ITERATOR begin,
                  ITERATOR end,
                  substitution_type& sigma);


    data_expression rewrite_aux(const data_expression& term, substitution_type& sigma);

    data_expression rewrite_aux_function_symbol(
                      const function_symbol& op,
                      const application& term,
                      substitution_type& sigma);

    data_expression rewrite_aux_const_function_symbol(
                      const function_symbol& op,
                      substitution_type& sigma);

    /// \brief Auxiliary function to take care that the array jitty_strat is sufficiently large
    ///        to access element i.
    void make_jitty_strat_sufficiently_larger(const std::size_t i);

    strategy create_a_cpp_function_based_strategy(const function_symbol& f, const data_specification& data_spec);
    strategy create_a_rewriting_based_strategy(const function_symbol& f, const data_equation_list& rules1);
    strategy create_strategy(const function_symbol& f, const data_equation_list& rules1, const data_specification& data_spec);
    void rebuild_strategy(const data_specification& data_spec, const mcrl2::data::used_data_equation_selector& equation_selector);
};

/// \brief removes auxiliary expressions this_term_is_in_normal_form from data_expressions that are being rewritten.
/// \details The function below is intended to remove the auxiliary function this_term_is_in_normal_form from a term
///          such that it can for instance be pretty printed. This auxiliary function is used internally in terms
///          when rewriting to avoid to rewrite too often.
data_expression remove_normal_form_function(const data_expression& t);

}
}
}

#endif
