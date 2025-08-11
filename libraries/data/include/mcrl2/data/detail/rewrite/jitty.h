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

#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/rewrite_stack.h"
#include "mcrl2/data/detail/rewrite/strategy_rule.h"



namespace mcrl2::data::detail
{

struct jitty_variable_assignment_for_a_rewrite_rule
{
  const variable& var;
  const data_expression& term;
  bool variable_is_a_normal_form;

   jitty_variable_assignment_for_a_rewrite_rule(const variable& m_var,  const data_expression& m_term, bool m_nf)
   : var(m_var),
     term(m_term),
     variable_is_a_normal_form(m_nf)
  {}
};

struct jitty_assignments_for_a_rewrite_rule
{
  std::size_t size = 0;
  jitty_variable_assignment_for_a_rewrite_rule* assignment;

  jitty_assignments_for_a_rewrite_rule(jitty_variable_assignment_for_a_rewrite_rule* a)
      : assignment(a)
  {}

};

class RewriterJitty: public Rewriter
{
  public:
    friend class jitty_argument_rewriter;

    using substitution_type = Rewriter::substitution_type;

    RewriterJitty(const data_specification& data_spec, const used_data_equation_selector& );
    
    // The copy constructor.
    RewriterJitty(const RewriterJitty& other) = default;

    // The assignment operator.
    RewriterJitty& operator=(const RewriterJitty& other) = delete;

    ~RewriterJitty() override;

    rewrite_strategy getStrategy() override;

    data_expression rewrite(const data_expression& term, substitution_type& sigma) override;

    void rewrite(data_expression& result, const data_expression& term, substitution_type& sigma) override;

    std::shared_ptr<detail::Rewriter> clone() override { return std::shared_ptr<Rewriter>(new RewriterJitty(*this)); }

    const function_symbol& this_term_is_in_normal_form() 
    {
      return this_term_is_in_normal_form_symbol;
    }

  protected:

    // A dedicated function symbol that indicates that a term is in normal form. It has name "Rewritten@@term".
    // The function symbol below is used to administrate that a term is in normal form. It is put around a term.
    // Terms with this auxiliary function symbol cannot be printed using the pretty printer for data expressions.
    function_symbol this_term_is_in_normal_form_symbol;
    bool rewriting_in_progress = false;

    class rewrite_stack m_rewrite_stack;     // Stack for intermediate rewrite results.

    std::vector<data_expression> rhs_for_constants_cache; // Cache that contains normal forms for constants. 
    std::map< function_symbol, data_equation_list > jitty_eqns;
    std::vector<strategy> jitty_strat;

    atermpp::detail::thread_aterm_pool* m_thread_aterm_pool
        = nullptr; // Store an explicit reference to the thread aterm pool.

    template<class ITERATOR>
    void apply_cpp_code_to_higher_order_term(data_expression& result,
      const application& t,
      std::function<void(data_expression&, const data_expression&)> rewrite_cpp_code,
      ITERATOR begin,
      ITERATOR end,
      substitution_type& sigma);

    void rewrite_aux(data_expression& result, const data_expression& term, substitution_type& sigma);

    void rewrite_aux_function_symbol(
                      data_expression& result,
                      const function_symbol& op,
                      const application& term,
                      substitution_type& sigma);

    void rewrite_aux_const_function_symbol(
                      data_expression& result,
                      const function_symbol& op,
                      substitution_type& sigma);

    /// \brief Auxiliary function to take care that the array jitty_strat is sufficiently large
    ///        to access element i.
    void make_jitty_strat_sufficiently_larger(std::size_t i);

    strategy create_a_cpp_function_based_strategy(const function_symbol& f, const data_specification& data_spec);
    strategy create_a_rewriting_based_strategy(const function_symbol& f, const data_equation_list& rules1);
    strategy create_strategy(const function_symbol& f, const data_equation_list& rules1, const data_specification& data_spec);
    void rebuild_strategy(const data_specification& data_spec, const mcrl2::data::used_data_equation_selector& equation_selector);

    data_expression remove_normal_form_function(const data_expression& t);
    void subst_values(
            data_expression& result,
            const jitty_assignments_for_a_rewrite_rule& assignments,
            const data_expression& t,
            data::enumerator_identifier_generator& generator);

    void thread_initialise() override { m_thread_aterm_pool = &atermpp::detail::g_thread_term_pool(); }
};

/// \brief removes auxiliary expressions this_term_is_in_normal_form from data_expressions that are being rewritten.
/// \details The function below is intended to remove the auxiliary function this_term_is_in_normal_form from a term
///          such that it can for instance be pretty printed. This auxiliary function is used internally in terms
///          when rewriting to avoid to rewrite too often.
data_expression remove_normal_form_function(const data_expression& t);

} // namespace mcrl2::data::detail



#endif
