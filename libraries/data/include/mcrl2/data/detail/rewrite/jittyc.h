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
#include "mcrl2/data/detail/rewrite/jitty.h"
#include "mcrl2/data/detail/rewrite/match_tree.h"
#include "mcrl2/utilities/uncompiledlibrary.h"
#include "mcrl2/utilities/toolset_version.h"
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

typedef std::vector < sort_expression_list> sort_list_vector;

///
/// \brief The normal_form_cache class stores normal forms of data_expressions that
///        are inserted in it. By keeping the cache on the stack, the normal forms
///        in it will not be freed by the ATerm library, and can therefore be used
///        in the generated jittyc code.
///
class normal_form_cache
{
private:
  RewriterJitty& m_rewriter;
  std::set<data_expression> m_lookup;
public:
  normal_form_cache(RewriterJitty& rewriter)
    : m_rewriter(rewriter)
  { }

  ///
  /// \brief insert stores the normal form of t in the cache, and returns a string
  ///        that is a C++ representation of the stored normal form. This string can
  ///        be used by the generated rewriter as long as the cache object is alive,
  ///        and its clear() method has not been called.
  /// \param t The term to normalize.
  /// \return A C++ string that evaluates to the cached normal form of t.
  ///
  std::string insert(const data_expression& t)
  {
    std::stringstream ss;
    RewriterJitty::substitution_type sigma;
    auto pair = m_lookup.insert(m_rewriter(t, sigma));
    ss << "*reinterpret_cast<const data_expression*>(" << (void*)&(*pair.first) << ")";
    return ss.str();
  }

  ///
  /// \brief clear clears the cache. This operation invalidates all the C++ strings
  ///        obtained via the insert() method.
  ///
  void clear()
  {
    m_lookup.clear();
  }
};

class RewriterCompilingJitty: public Rewriter
{
  public:
    typedef Rewriter::substitution_type substitution_type;

    RewriterCompilingJitty(const data_specification& DataSpec, const used_data_equation_selector &);
    virtual ~RewriterCompilingJitty();

    rewrite_strategy getStrategy();

    data_expression rewrite(const data_expression &term, substitution_type &sigma);

    substitution_type *global_sigma;

    // The data structures below are used to store the variable lists2
    // that are used in the compiling rewriter in forall, where and exists.
    std::vector<variable_list> rewriter_binding_variable_lists;
    std::map <variable_list, size_t> variable_list_indices1;
    size_t binding_variable_list_index(const variable_list &v);
    inline variable_list binding_variable_list_get(const size_t i)
    {
      return (rewriter_binding_variable_lists[i]);
    }

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
    class ImplementTree;
    friend class ImplementTree;
    
    RewriterJitty jitty_rewriter;
    std::set < data_equation > rewrite_rules;
    bool made_files;
    std::map<function_symbol, data_equation_list> jittyc_eqns;
    std::set<function_symbol> m_extra_symbols;

    normal_form_cache m_nf_cache;

    uncompiled_library *rewriter_so;

    void (*so_rewr_cleanup)();
    data_expression(*so_rewr)(const data_expression&);

    void add_base_nfs(nfs_array &a, const function_symbol &opid, size_t arity);
    void extend_nfs(nfs_array &a, const function_symbol &opid, size_t arity);
    bool opid_is_nf(const function_symbol &opid, size_t num_args);
    void calc_nfs_list(nfs_array &a, const application& args, variable_or_number_list nnfvars);
    bool calc_nfs(const data_expression& t, variable_or_number_list nnfvars);
    void CleanupRewriteSystem();
    void BuildRewriteSystem();
    void generate_code(const std::string& filename);
    void generate_rewr_functions(std::ostream& s, const data::function_symbol& func, const data_equation_list& eqs);
    bool lift_rewrite_rule_to_right_arity(data_equation& e, const size_t requested_arity);
    sort_list_vector get_residual_sorts(const sort_expression& s, const size_t actual_arity, const size_t requested_arity);
    match_tree_list create_strategy(const data_equation_list& rules, const size_t arity);

};

struct rewriter_interface
{
  std::string caller_toolset_version;
  std::string status;
  RewriterCompilingJitty* rewriter;
  data_expression (*rewrite_external)(const data_expression &t);
  void (*rewrite_cleanup)();
};

}
}
}

#endif // MCRL2_JITTYC_AVAILABLE

#endif // __REWR_JITTYC_H
