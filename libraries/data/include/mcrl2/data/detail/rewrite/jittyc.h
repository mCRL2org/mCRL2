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
    RewriterCompilingJitty(const data_specification& DataSpec, const bool add_rewrite_rules);
    virtual ~RewriterCompilingJitty();

    RewriteStrategy getStrategy();

    ATermAppl rewrite(ATermAppl Term);

    ATerm toRewriteFormat(ATermAppl Term);
    ATermAppl fromRewriteFormat(ATerm Term);
    ATerm rewriteInternal(ATerm Term);
    ATermList rewriteInternalList(ATermList Terms);

    bool addRewriteRule(ATermAppl Rule);
    bool removeRewriteRule(ATermAppl Rule);

  private:
    ATermTable tmp_eqns;
    bool need_rebuild;
    bool made_files;

    ATermInt true_inner;
    int true_num;

    ATermList* jittyc_eqns;

    ATermTable int2ar_idx;
    size_t ar_size;
    ATermAppl* ar;
    ATermAppl build_ar_expr(ATerm expr, ATermAppl var);
    ATermAppl build_ar_expr_aux(ATermList eqn, size_t arg, size_t arity);
    ATermAppl build_ar_expr(ATermList eqns, size_t arg, size_t arity);
    bool always_rewrite_argument(ATermInt opid, size_t arity, size_t arg);
    bool calc_ar(ATermAppl expr);
    void fill_always_rewrite_array();

    std::string rewriter_source;
    uncompiled_library *rewriter_so;

    void (*so_rewr_init)(RewriterCompilingJitty *);
    void (*so_rewr_cleanup)();
    ATermAppl(*so_rewr)(ATermAppl);

    void add_base_nfs(nfs_array &a, ATermInt opid, size_t arity);
    void extend_nfs(nfs_array &a, ATermInt opid, size_t arity);
    bool opid_is_nf(ATermInt opid, size_t num_args);
    void calc_nfs_list(nfs_array &a, size_t arity, ATermList args, int startarg, ATermList nnfvars);
    bool calc_nfs(ATerm t, int startarg, ATermList nnfvars);
    std::string calc_inner_terms(nfs_array &nfs, size_t arity,ATermList args, int startarg, ATermList nnfvars, nfs_array *rewr);
    std::pair<bool,std::string> calc_inner_term(ATerm t, int startarg, ATermList nnfvars, bool rewr = true);
    void calcTerm(FILE* f, ATerm t, int startarg, ATermList nnfvars, bool rewr = true);
    void implement_tree_aux(FILE* f, ATermAppl tree, int cur_arg, int parent, int level, int cnt, int d, int arity, bool* used, ATermList nnfvars);
    void implement_tree(FILE* f, ATermAppl tree, int arity, int d, int opid, bool* used);
    void implement_strategy(FILE* f, ATermList strat, int arity, int d, int opid, size_t nf_args);
    void CompileRewriteSystem(const data_specification& DataSpec, const bool add_rewrite_rules);
    void CleanupRewriteSystem();
    void BuildRewriteSystem();
	FILE* MakeTempFiles();

};

}
}
}

#endif // MCRL2_JITTYC_AVAILABLE

#endif // __REWR_JITTYC_H
