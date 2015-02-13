/// The following defines are required to be set before loading this header file:
///   ADDR_OPID   -- The atermpp::detail::addressf() of the OpId function symbol.
///   ARITY_BOUND -- The maximum occurring arity + 1
///   INDEX_BOUND -- The maximum occurring index + 1

#include <cstdlib>
#include <cstring>
#include <cassert>
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/utilities/toolset_version_const.h"
#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"
#include "mcrl2/data/detail/rewrite.h"
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
static void rewrite_init(RewriterCompilingJitty*);
static data_expression rewrite(const data_expression& t);

//
// Type definitions
//
typedef data_expression (*rewriter_function)(const data_expression&);

struct argument_rewriter_struct
{
  data_expression operator()(const data_expression& arg) const
  {
    return rewrite(arg);
  }
};

//
// Declaration of global variables
//
RewriterCompilingJitty *this_rewriter;
rewriter_function* int2func[ARITY_BOUND];
rewriter_function* int2func_head_in_nf[ARITY_BOUND];

//
// Miscellaneous helper functions
//
static inline
const data_expression& pass_on(const data_expression& t)
{
  return t;
}

static inline
data_expression do_nothing(const data_expression& t)
{
  return t;
}

static inline
assignment_expression_list jittyc_local_push_front(assignment_expression_list l, const assignment& e)
{
  l.push_front(atermpp::deprecated_cast<assignment_expression>(e));
  return l;
}

//
// Rewriting functions
//
static inline
data_expression rewrite_appl_aux(const application& t)
{
  mcrl2::data::function_symbol thead;
  if (mcrl2::data::detail::head_is_function_symbol(t,thead))
  {
    const size_t function_index = mcrl2::core::index_traits<function_symbol, function_symbol_key_type, 2>::index(thead);
    const size_t total_arity = recursive_number_of_args(t);
    if (function_index < INDEX_BOUND)
    {
      assert(total_arity < ARITY_BOUND);
      assert(int2func[total_arity][function_index] != NULL);
      return int2func[total_arity][function_index](t);
    }
    else
    {
      return mcrl2::data::application(rewrite(t.head()), t.begin(), t.end(), argument_rewriter_struct());
    }
  }
  // Here the head symbol of, which can be deeply nested, is not a function_symbol.
  const data_expression& head0=get_nested_head(t);
  const data_expression head=
       (is_variable(head0)?
            (*(this_rewriter->global_sigma))(down_cast<const variable>(head0)):
       (is_where_clause(head0)?
            this_rewriter->rewrite_where(atermpp::down_cast<where_clause>(head0),*(this_rewriter->global_sigma)):
             head0));

  // Reconstruct term t.
  const application t1((head0==head)?t:replace_nested_head(t,head));

  const data_expression head1(get_nested_head(t1));
  // Here head1 has the shape
  // variable, function_symbol, lambda y1,....,ym.u, forall y1,....,ym.u or exists y1,....,ym.u,
  if (is_abstraction(head1))
  {
    const abstraction& heada(head1);
    const binder_type& binder(heada.binding_operator());
    if (is_lambda_binder(binder))
    {
      return this_rewriter->rewrite_lambda_application(t1,*(this_rewriter->global_sigma));
    }
    if (is_exists_binder(binder))
    {
      return this_rewriter->existential_quantifier_enumeration(head1,*(this_rewriter->global_sigma));
    }
    assert(is_forall_binder(binder));
    return this_rewriter->universal_quantifier_enumeration(head1,*(this_rewriter->global_sigma));
  }

  if (is_variable(head1))
  {
    return rewrite_all_arguments(t1, argument_rewriter_struct());
  }

  // Here t1 has the shape application(u0,u1,...,un).
  // Moreover, the head symbol of t1, head1, is a function symbol.
  const function_symbol& f = down_cast<function_symbol>(head1);
  const size_t function_index = mcrl2::core::index_traits<function_symbol, function_symbol_key_type, 2>::index(f);
  assert(function_index < INDEX_BOUND);
  const size_t total_arity = recursive_number_of_args(t1);
  assert(int2func_head_in_nf[total_arity][function_index] != NULL);
  return int2func_head_in_nf[total_arity][function_index](t1);
}

static inline
data_expression rewrite_aux(const data_expression& t)
{
  // Term t does not have the shape application(t1,...,tn)
  auto& sigma = *(this_rewriter->global_sigma);
  if (is_variable(t))
  {
    return sigma(down_cast<variable>(t));
  }
  if (is_abstraction(t))
  {
    const abstraction& ta(t);
    const binder_type& binder(ta.binding_operator());
    if (is_exists_binder(binder))
    {
      return this_rewriter->existential_quantifier_enumeration(t, sigma);
    }
    if (is_forall_binder(binder))
    {
      return this_rewriter->universal_quantifier_enumeration(t, sigma);
    }
    assert(is_lambda_binder(binder));
    return this_rewriter->rewrite_single_lambda(ta.variables(), ta.body(), false, sigma);
  }
  assert(is_where_clause(t));
  return this_rewriter->rewrite_where(down_cast<where_clause>(t), sigma);
}

static
data_expression rewrite(const data_expression& t)
{
  if (atermpp::detail::addressf(t.function()) == ADDR_OPID)
  {
    // Term t is a function_symbol
    const function_symbol& f = down_cast<function_symbol>(t);
    const size_t function_index = mcrl2::core::index_traits<function_symbol, function_symbol_key_type, 2>::index(f);
    if (function_index < INDEX_BOUND)
    {
      const size_t arity = 0;
      assert(int2func[arity][function_index] != NULL);
      return int2func[arity][function_index](t);
    }
    else
    {
      return t;
    }
  }
  else if (is_application_no_check(t))
  {
    const application& appl = down_cast<application>(t);
    const function_symbol& head = down_cast<function_symbol>(appl.head());
    if (atermpp::detail::addressf(head.function()) == ADDR_OPID)
    {
      const size_t function_index = mcrl2::core::index_traits<function_symbol, function_symbol_key_type, 2>::index(head);
      const size_t total_arity = appl.size();
      if (function_index < INDEX_BOUND)
      {
        assert(total_arity < ARITY_BOUND);
        assert(int2func[total_arity][function_index] != NULL);
        return int2func[total_arity][function_index](t);
      }
      else
      {
        return application(rewrite(appl.head()), appl.begin(), appl.end(), argument_rewriter_struct());
      }
    }
    else
    {
      return rewrite_appl_aux(appl);
    }
  }
  else
  {
    return rewrite_aux(t);
  }
}

// Generate code to cleanup int2func.
static
void rewrite_cleanup()
{
  for (size_t i = 0; i < ARITY_BOUND; ++i)
  {
    delete[] int2func[i];
    delete[] int2func_head_in_nf[i];
  }
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
  rewrite_init(i->rewriter);
  i->status = "rewriter loaded successfully.";
  return true;
}

