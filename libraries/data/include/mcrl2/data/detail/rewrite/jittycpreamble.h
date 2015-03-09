/// The following defines are required to be set before loading this header file:
///   ARITY_BOUND -- The maximum occurring arity + 1
///   INDEX_BOUND -- The maximum occurring index + 1

#include <cassert>
#include "mcrl2/utilities/toolset_version_const.h"
#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"
#include "mcrl2/data/detail/rewrite/jittyc.h"

using namespace mcrl2::data::detail;
using namespace mcrl2::data;
using atermpp::down_cast;

//
// Declaration of rewriter library interface
//
#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif // _MSC_VER

extern "C" {
  DLLEXPORT bool init(rewriter_interface* i);
}

//
// Forward declarations
//
static void fill_int2func();
static data_expression rewrite(const data_expression& t);

//
// Type definitions
//
typedef data_expression (*rewriter_function)(const application&);

struct rewrite_functor
{
  data_expression operator()(const data_expression& arg) const
  {
    return rewrite(arg);
  }
};

//
// Declaration of global variables
//
static RewriterCompilingJitty *this_rewriter;
static rewriter_function int2func[ARITY_BOUND * INDEX_BOUND] = {};
//static rewriter_function int2func_head_in_nf[ARITY_BOUND * INDEX_BOUND];
static const application dummy_application;

//
// Miscellaneous helper functions
//
static inline
const data_expression& pass_on(const data_expression& t)
{
  return t;
}

static inline
assignment_expression_list jittyc_local_push_front(assignment_expression_list l, const assignment& e)
{
  l.push_front(atermpp::deprecated_cast<assignment_expression>(e));
  return l;
}

static inline
size_t get_index(const function_symbol& func)
{
  return mcrl2::core::index_traits<function_symbol, function_symbol_key_type, 2>::index(func);
}

static inline
RewriterCompilingJitty::substitution_type& sigma()
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

///
/// \brief rewrite calls the function in the int2func table, with the given application t
///        as its argument. If t is not given, then a dummy application is passed. This
///        happens when a rewrite function is called on a function symbol. This strange
///        behaviour is due to the fact that rewriting of function symbols (constants) and
///        of applications is treated in the same way. A better solution would be to have
///        a separate int2func table of type (data_expression (*rewriter_function)(void))*.
/// \param arity The arity of the rewrite rule.
/// \param index The index of the outer function symbol of the rule.
/// \param t The parameter to pass to the rewr_*_*_term function.
/// \return The rewritten term.
///
static inline
data_expression rewrite(size_t arity, size_t index, const application& t = dummy_application)
{
  assert(arity < ARITY_BOUND);
  assert(index < INDEX_BOUND);
  rewriter_function f = int2func[ARITY_BOUND * index + arity];
  if (f != NULL)
  {
    return f(t);
  }
  return t;
}

static inline
data_expression rewrite_appl_aux(const application& t)
{
  function_symbol thead;
  if (head_is_function_symbol(t, thead))
  {
    const size_t index = get_index(thead);
    if (index < INDEX_BOUND)
    {
      return rewrite(recursive_number_of_args(t), index, t);
    }
    else
    {
      return application(rewrite(t.head()), t.begin(), t.end(), rewrite_functor());
    }
  }
  // Here the head symbol of, which can be deeply nested, is not a function_symbol.
  const data_expression& head0 = get_nested_head(t);
  const data_expression head = (is_variable(head0)
                             ? sigma()(down_cast<const variable>(head0))
                             : (is_where_clause(head0)
                               ? this_rewriter->rewrite_where(atermpp::down_cast<where_clause>(head0), sigma())
                               : head0));

  // Reconstruct term t.
  const application t1((head0 == head) ? t : replace_nested_head(t, head));

  const data_expression head1(get_nested_head(t1));
  // Here head1 has the shape
  // variable, function_symbol, lambda y1,....,ym.u, forall y1,....,ym.u or exists y1,....,ym.u,
  if (is_variable(head1))
  {
    return rewrite_all_arguments(t1, rewrite_functor());
  }
  else
  if (is_abstraction(head1))
  {
    assert(is_abstraction(head1));
    const abstraction& heada(head1);
    const binder_type& binder(heada.binding_operator());
    if (is_lambda_binder(binder))
    {
      return this_rewriter->rewrite_lambda_application(t1, sigma());
    }
    if (is_exists_binder(binder))
    {
      return this_rewriter->existential_quantifier_enumeration(head1, sigma());
    }
    assert(is_forall_binder(binder));
    return this_rewriter->universal_quantifier_enumeration(head1, sigma());
  }
  else
  {
    assert(is_function_symbol(head1));
    return rewrite(recursive_number_of_args(t1), get_index(down_cast<function_symbol>(head1)), t1);
  }
}

static inline
data_expression rewrite(const data_expression& t)
{
  if (is_function_symbol(t))
  {
    const size_t index = get_index(down_cast<function_symbol>(t));
    if (index < INDEX_BOUND)
    {
      return rewrite(0, index);
    }
    else
    {
      return t;

    }
  }
  else
  if (is_application_no_check(t))
  {
    const application& appl = down_cast<application>(t);
    const function_symbol& head = down_cast<function_symbol>(appl.head());
    const size_t index = get_index(head);
    if (is_function_symbol(head))
    {
      if (index < INDEX_BOUND)
      {
        return rewrite(appl.size(), index, appl);
      }
      else
      {
        return application(rewrite(appl.head()), appl.begin(), appl.end(), rewrite_functor());
      }
    }
    else
    {
      return rewrite_appl_aux(appl);
    }
  }
  else
  if (is_variable(t))
  {
    return sigma()(down_cast<variable>(t));
  }
  else
  if (is_abstraction(t))
  {
    const abstraction& abstr(t);
    const binder_type& binder(abstr.binding_operator());
    if (is_exists_binder(binder))
    {
      return this_rewriter->existential_quantifier_enumeration(t, sigma());
    }
    if (is_forall_binder(binder))
    {
      return this_rewriter->universal_quantifier_enumeration(t, sigma());
    }
    assert(is_lambda_binder(binder));
    return this_rewriter->rewrite_single_lambda(abstr.variables(), abstr.body(), false, sigma());
  }
  else
  {
    assert(is_where_clause(t));
    return this_rewriter->rewrite_where(down_cast<where_clause>(t), sigma());
  }
}

// Generate code to cleanup int2func.
static
void rewrite_cleanup()
{
}

bool init(rewriter_interface* i)
{
  if (mcrl2::utilities::MCRL2_VERSION != i->caller_toolset_version)
  {
    i->status = "rewriter version does not match the version of the calling application.";
    return false;
  }
  i->rewrite_external = &rewrite;
  i->rewrite_cleanup = &rewrite_cleanup;
  this_rewriter = i->rewriter;
  fill_int2func();
  i->status = "rewriter loaded successfully.";
  return true;
}

