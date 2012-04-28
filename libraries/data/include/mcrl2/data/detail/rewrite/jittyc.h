// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/jittyc.h

#ifndef __REWR_JITTYC_H
#define __REWR_JITTYC_H

#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/utilities/uncompiledlibrary.h"

#include "nfs_array.h"

#ifdef MCRL2_JITTYC_AVAILABLE

#include <utility>
#include <string>

namespace mcrl2
{
namespace data
{
namespace detail
{

class RewriterCompilingJitty: public Rewriter
{
  public:
    typedef Rewriter::substitution_type substitution_type;
    typedef Rewriter::internal_substitution_type internal_substitution_type;

    RewriterCompilingJitty(const data_specification& DataSpec, const used_data_equation_selector &);
    virtual ~RewriterCompilingJitty();

    rewrite_strategy getStrategy();

    data_expression rewrite(const data_expression &term, substitution_type &sigma);

    atermpp::aterm_appl rewrite_internal(
         const atermpp::aterm_appl &term,
         internal_substitution_type &sigma);

    atermpp::aterm_appl toRewriteFormat(const data_expression &term);
    // data_expression fromRewriteFormat(const atermpp::aterm_appl term);
    bool addRewriteRule(const data_equation &rule);
    bool removeRewriteRule(const data_equation &rule);
    internal_substitution_type *global_sigma;

    // The data structures below are used to store the variable lists2
    // that are used in the compiling rewriter in forall, where and exists.
    std::vector<variable_list> rewriter_binding_variable_lists;
    std::map <variable_list, size_t> variable_list_indices1;
    size_t binding_variable_list_index(const variable_list &v);
    inline variable_list binding_variable_list_get(const size_t i)
    {
      return (rewriter_binding_variable_lists[i]);
    }

    // The set below contains function symbols that are locally used
    // in the compiling rewriter to represent functions of which it is
    // known that some of the arguments are in normal form. These are
    // used inside the compiling rewriter, but should never be returned
    // and show up in any normal form being returned from the rewriter.
    std::set < function_symbol > partially_rewritten_functions;

    // The data structures below are used to store single variables
    // that are bound in lambda, forall and exist operators. When required
    // in the compiled required, these variables can be retrieved from
    // the array rewriter_bound_variables. variable_indices0 is used
    // to prevent double occurrences in the vector.
    std::vector<variable> rewriter_bound_variables;
    std::map <variable, size_t> variable_indices0;
    size_t bound_variable_index(const variable &v);
    variable bound_variable_get(const size_t i)
    {
      return (rewriter_bound_variables[i]);
    }

  private:
    std::set < data_equation > rewrite_rules;
    // used_data_equation_selector data_equation_selector;
    bool need_rebuild;
    bool made_files;

    atermpp::aterm_int true_inner;
    int true_num;

    std::vector < data_equation_list >  jittyc_eqns;

    std::map <int,int> int2ar_idx;
    size_t ar_size;
    std::vector<ATermAppl> ar;
    ATermAppl build_ar_expr(ATerm expr, ATermAppl var);
    ATermAppl build_ar_expr_aux(const data_equation &eqn, const size_t arg, const size_t arity);
    ATermAppl build_ar_expr(const data_equation_list &eqns, const size_t arg, const size_t arity);
    bool always_rewrite_argument(const atermpp::aterm_int &opid, const size_t arity, const size_t arg);
    bool calc_ar(const ATermAppl &expr);
    void fill_always_rewrite_array();

    std::string rewriter_source;
    uncompiled_library *rewriter_so;

    void (*so_rewr_init)(RewriterCompilingJitty *);
    void (*so_rewr_cleanup)();
    atermpp::aterm_appl(*so_rewr)(const atermpp::aterm_appl &);
    // atermpp::aterm_appl(*so_rewr)(const _ATermAppl *);

    void add_base_nfs(nfs_array &a, const atermpp::aterm_int &opid, size_t arity);
    void extend_nfs(nfs_array &a, const atermpp::aterm_int &opid, size_t arity);
    bool opid_is_nf(const atermpp::aterm_int &opid, size_t num_args);
    void calc_nfs_list(nfs_array &a, size_t arity, ATermList args, int startarg, ATermList nnfvars);
    bool calc_nfs(ATerm t, int startarg, ATermList nnfvars);
    std::string calc_inner_terms(nfs_array &nfs, size_t arity,ATermList args, int startarg, ATermList nnfvars, nfs_array *rewr);
    std::pair<bool,std::string> calc_inner_term(ATerm t, int startarg, ATermList nnfvars, const bool rewr, const size_t total_arity);
    void calcTerm(FILE* f, ATerm t, int startarg, ATermList nnfvars, bool rewr = true);
    void implement_tree_aux(FILE* f, ATermAppl tree, int cur_arg, int parent, int level, int cnt, int d, int arity, bool* used, ATermList nnfvars);
    void implement_tree(FILE* f, ATermAppl tree, int arity, int d, int opid, bool* used);
    void implement_strategy(FILE* f, ATermList strat, int arity, int d, int opid, size_t nf_args);
    void CompileRewriteSystem(const data_specification& DataSpec);
    void CleanupRewriteSystem();
    void BuildRewriteSystem();
    FILE* MakeTempFiles();

};

}
}
}

#endif // MCRL2_JITTYC_AVAILABLE

#endif // __REWR_JITTYC_H
