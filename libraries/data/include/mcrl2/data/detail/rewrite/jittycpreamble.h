// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/jittycpreamble.h

#ifndef MCRL2_DATA_DETAIL_REWR_JITTYC_PREAMBLE_H
#define MCRL2_DATA_DETAIL_REWR_JITTYC_PREAMBLE_H

#include "mcrl2/utilities/toolset_version_const.h"
#include "mcrl2/utilities/export.h"
#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"
#include "mcrl2/data/detail/rewrite/jittyc.h"

using namespace mcrl2::data::detail;
using namespace mcrl2::data;
using atermpp::down_cast;

//
// Declaration of rewriter library interface
//

extern "C" {
  MCRL2_EXPORT bool init(rewriter_interface* i, RewriterCompilingJitty* this_rewriter);
}

// A rewrite_term is a term that may or may not be in normal form. If the method"
// normal_form is invoked, it will calculate a normal form for itself as efficiently as possible."
template <class REWRITE_TERM>
static data_expression& local_rewrite(const REWRITE_TERM& t)
{
  return t.normal_form();
}

template <class REWRITE_TERM>
static void local_rewrite(data_expression& result, const REWRITE_TERM& t)
{
  t.normal_form(result);
}

static const data_expression& local_rewrite(const data_expression& t)
{
  return t;
} 

static void local_rewrite(data_expression& result, const data_expression& t)
{
  result=t;
}

//
// Forward declarations
//
static void set_the_precompiled_rewrite_functions_in_a_lookup_table(RewriterCompilingJitty* this_rewriter);

template <bool ARGUMENTS_IN_NORMAL_FORM>
static void rewrite_aux(data_expression& result, const data_expression& t, RewriterCompilingJitty* this_rewriter);
static void rewrite_abstraction_aux(data_expression& result, const abstraction& a, const data_expression& t, RewriterCompilingJitty* this_rewriter);

static void rewrite_with_arguments_in_normal_form(
                        data_expression& result, 
                        const data_expression& t, 
                        RewriterCompilingJitty* this_rewriter)
{
  rewrite_aux<true>(result, t, this_rewriter);
  return;
}

static data_expression rewrite_with_arguments_in_normal_form(const data_expression& t, RewriterCompilingJitty* this_rewriter)
{
  data_expression result; // TODO: Optimize
  rewrite_aux<true>(result, t, this_rewriter);
  return result;
}

static void rewrite(data_expression& result, const data_expression& t, RewriterCompilingJitty* this_rewriter)
{
  rewrite_aux<false>(result, t, this_rewriter);
  return;
}

//
// Type definitions
//
typedef void(*rewriter_function)(data_expression&, const application&, RewriterCompilingJitty*);

// A class that contains terms which are explicitly tagged to be
// not in normal form. By invoking normal_form the normalform
// of this term is calculated.
class term_not_in_normal_form
{
  protected:
    const data_expression& m_term;
    RewriterCompilingJitty* this_rewriter;

  public:
    term_not_in_normal_form(const data_expression& term, RewriterCompilingJitty* tr)
       : m_term(term), this_rewriter(tr)
    {}

    data_expression& normal_form() const
    {
      data_expression& local_store = this_rewriter->m_rewrite_stack.new_stack_position<data_expression>();
      rewrite_aux<false>(local_store, m_term, this_rewriter);
      return local_store;
    }

    void normal_form(data_expression& result) const
    {
      rewrite_aux<false>(result, m_term, this_rewriter);
    }
};

// This is an abstraction, of which the arguments are not yet
// in normal form. This is done when the method "normal_form" is invoked.
template <class TERM_TO_BE_REWRITTEN>
class delayed_abstraction
{
  protected:
    const binder_type& m_binding_operator;
    const variable_list& m_variables;
    const TERM_TO_BE_REWRITTEN& m_body;
    RewriterCompilingJitty* this_rewriter;
  public:
    delayed_abstraction(const binder_type& binding_operator, 
                        const variable_list& variables, 
                        const TERM_TO_BE_REWRITTEN& body,
                        RewriterCompilingJitty* tr)
       : m_binding_operator(binding_operator), m_variables(variables), m_body(body), this_rewriter(tr)
    {}

    data_expression& normal_form() const
    {
      data_expression& t = this_rewriter->m_rewrite_stack.new_stack_position<data_expression>();
      make_abstraction(t, m_binding_operator, m_variables, local_rewrite(m_body));
      data_expression& result = this_rewriter->m_rewrite_stack.new_stack_position<data_expression>();
      rewrite_abstraction_aux(result, atermpp::down_cast<abstraction>(t),t,this_rewriter);
      return result;
    }

    void normal_form(data_expression& result) const
    {
      local_rewrite(result, m_body);  // TODO: HERE m_body is rewritten twice, even if it is in normal form. 
      make_abstraction(result, m_binding_operator, m_variables, result);
      rewrite_abstraction_aux(result, atermpp::down_cast<abstraction>(result),result,this_rewriter);
    }
};


struct rewrite_functor
{
  RewriterCompilingJitty* this_rewriter;

  rewrite_functor(RewriterCompilingJitty* tr)
   : this_rewriter(tr)
  {} 

  data_expression operator()(const data_expression& arg) const
  {
    data_expression result;
    rewrite_aux<false>(result, arg, this_rewriter);
    return result;
  }
};


// Miscellaneous helper functions
//
static inline
void pass_on(data_expression& result, const data_expression& t)
{
  result=t;
}

static inline
assignment_expression_list jittyc_local_push_front(assignment_expression_list l, const assignment& e)
{
  l.push_front(e);
  return l;
}

static inline
std::size_t get_index(const function_symbol& func)
{
  return atermpp::detail::index_traits<function_symbol, function_symbol_key_type, 2>::index(func);
}

static inline
RewriterCompilingJitty::substitution_type& sigma(RewriterCompilingJitty* this_rewriter)
{
  return *(this_rewriter->global_sigma);
}

static inline
uintptr_t uint_address(const atermpp::aterm& t)
{
  return reinterpret_cast<uintptr_t>(atermpp::detail::address(t));
}

//
// Rewriting functions
//

template <bool ARGUMENTS_IN_NORMAL_FORM>
static inline rewriter_function get_precompiled_rewrite_function(
             const function_symbol& f, 
             const std::size_t arity, 
             RewriterCompilingJitty* this_rewriter)
{
  const std::size_t index = get_index(f);
  if (index>=this_rewriter->index_bound || arity>=this_rewriter->arity_bound)
  {
    return nullptr;
  }
  if (ARGUMENTS_IN_NORMAL_FORM)
  {
    assert(this_rewriter -> arity_bound * index + arity<this_rewriter->functions_when_arguments_are_in_normal_form.size());
    return this_rewriter->functions_when_arguments_are_in_normal_form[this_rewriter->arity_bound * index + arity]; 
  }
  else
  {
    assert(this_rewriter->arity_bound * index + arity<this_rewriter->functions_when_arguments_are_not_in_normal_form.size());
    return this_rewriter->functions_when_arguments_are_not_in_normal_form[this_rewriter->arity_bound * index + arity];
  }
}

static inline 
void rewrite_abstraction_aux(data_expression& result, const abstraction& head, const data_expression& a, RewriterCompilingJitty* this_rewriter)
{
  const binder_type& binder(head.binding_operator());
  if (is_lambda_binder(binder))
  {
    this_rewriter->rewrite_lambda_application(result, a, sigma(this_rewriter));
    assert(result.sort()==a.sort());
    return;
  }
  if (is_exists_binder(binder))
  {
    this_rewriter->existential_quantifier_enumeration(result, head, sigma(this_rewriter));
    assert(result.sort()==a.sort());
    return;
  }
  assert(is_forall_binder(binder));
  this_rewriter->universal_quantifier_enumeration(result, head, sigma(this_rewriter));
  assert(result.sort()==a.sort());
  return;
}


static inline
void rewrite_appl_aux(data_expression& result, const application& t, RewriterCompilingJitty* this_rewriter)
{
  const data_expression& thead=get_nested_head(t);
  if (is_function_symbol(thead))
  {
    const std::size_t arity=recursive_number_of_args(t);
    const rewriter_function f = get_precompiled_rewrite_function<false>(atermpp::down_cast<function_symbol>(thead),arity,this_rewriter);
    if (f != nullptr)
    {
      f(result, t,this_rewriter);
      assert(t.sort()==result.sort());
      return;
    }
    make_application(result, t.head(), t.begin(), t.end(), rewrite_functor(this_rewriter));
    return; 
  }
  // Here the head symbol of, which can be deeply nested, is not a function_symbol.
  const data_expression& head0 = get_nested_head(t);
  data_expression head;
  if (is_variable(head0))
  { 
    head=sigma(this_rewriter)(down_cast<const variable>(head0));
  }
  else if (is_where_clause(head0))
  { 
    this_rewriter->rewrite_where(head, atermpp::down_cast<where_clause>(head0), sigma(this_rewriter));
  }
  else
  {
    head= head0;
  }

  // Reconstruct term t.
  const application t1((head0 == head) ? static_cast<const data_expression&>(t) : replace_nested_head(t, head));

  const data_expression head1(get_nested_head(t1));
  // Here head1 has the shape
  // variable, function_symbol, lambda y1,....,ym.u, forall y1,....,ym.u or exists y1,....,ym.u,
  if (is_variable(head1))
  {
    rewrite_all_arguments(result, t1, rewrite_functor(this_rewriter));
    return;
  }
  else
  if (is_abstraction(head1))
  {
    const abstraction& ha=down_cast<abstraction>(head1);
    rewrite_abstraction_aux(result, ha,t1,this_rewriter);
    return;
  }
  else
  {
    assert(is_function_symbol(head1));
    const std::size_t arity = recursive_number_of_args(t1);
    const rewriter_function f = get_precompiled_rewrite_function<false>(down_cast<function_symbol>(head1),arity,this_rewriter);
    if (f != nullptr)
    {
      f(result, t1, this_rewriter);
      assert(t1.sort()==result.sort());
      return;
    }
    make_application(result, head1, t1.begin(), t1.end(), rewrite_functor(this_rewriter)); 
    return;
  }
}

template <bool ARGUMENTS_IN_NORMAL_FORM>
static inline
void rewrite_aux(data_expression& result, const data_expression& t, RewriterCompilingJitty* this_rewriter)
{
  if (is_machine_number(t))
  {
    result=t;
    return;
  }
  else
  if (is_function_symbol(t))
  {
    const std::size_t index = get_index(down_cast<function_symbol>(t));
    if (index<this_rewriter->normal_forms_for_constants.size())
    {             
      // In this case we can use an unprotected_assign as the normal_forms_for_constants will 
      // not change anymore, while rewriting is going on. 
      result.unprotected_assign<false>(this_rewriter->normal_forms_for_constants[index]);
             
      if (!result.is_default_data_expression())
      {  
        assert(t.sort()==result.sort());
        return;
      }
    }
    result=t;
    return; 
  }
  else
  if (is_application_no_check(t))
  {
    const application& appl = down_cast<application>(t);
    const data_expression& head = appl.head();
    if (is_function_symbol(head))
    {
      const std::size_t appl_size=appl.size();
      const rewriter_function f = get_precompiled_rewrite_function<ARGUMENTS_IN_NORMAL_FORM>(down_cast<function_symbol>(head), appl_size, this_rewriter);
      if (f != nullptr)
      {
        f(result, appl,this_rewriter);
        assert(result.sort()==t.sort());
        return;
      }
      make_application(result, appl.head(), appl.begin(), appl.end(), rewrite_functor(this_rewriter));
      return;
    }
    else
    {
      rewrite_appl_aux(result, appl, this_rewriter);
      assert(result.sort()==appl.sort());
      return;
    }
  }
  else
  if (is_variable(t))
  {
    sigma(this_rewriter).apply(down_cast<variable>(t),result,
                               *this_rewriter->m_thread_aterm_pool);
    return;
  }
  else
  if (is_abstraction(t))
  {
    const abstraction& abstr=down_cast<abstraction>(t);
    const binder_type& binder(abstr.binding_operator());
    if (is_exists_binder(binder))
    {
      this_rewriter->existential_quantifier_enumeration(result, abstr, sigma(this_rewriter));
      assert(result.sort()==t.sort());
      return;
    }
    if (is_forall_binder(binder))
    {
      this_rewriter->universal_quantifier_enumeration(result, abstr, sigma(this_rewriter));
      assert(result.sort()==t.sort());
      return;
    }
    assert(is_lambda_binder(binder));
    this_rewriter->rewrite_single_lambda(result, abstr.variables(), abstr.body(), false, sigma(this_rewriter));
    return;
  }
  else
  {
    assert(is_where_clause(t));
    this_rewriter->rewrite_where(result,down_cast<where_clause>(t), sigma(this_rewriter));
    return;
  }
}

static
void rewrite_cleanup()
{
}

bool init(rewriter_interface* i, RewriterCompilingJitty* this_rewriter)
{
  if (mcrl2::utilities::MCRL2_VERSION != i->caller_toolset_version)
  {
    i->status = "rewriter version does not match the version of the calling application.";
    return false;
  }

  i->rewrite_external = &rewrite;
  i->rewrite_cleanup = &rewrite_cleanup;
  set_the_precompiled_rewrite_functions_in_a_lookup_table(this_rewriter);
  i->status = "rewriter loaded successfully.";
  return true;
}

#endif // MCRL2_DATA_DETAIL_REWR_JITTYC_PREAMBLE_H
