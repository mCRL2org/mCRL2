// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/jittyc.h

#ifndef MCRL2_DATA_DETAIL_REWR_JITTYC_H
#define MCRL2_DATA_DETAIL_REWR_JITTYC_H

#include <utility>
#include <string>

#include "mcrl2/utilities/uncompiledlibrary.h"
#include "mcrl2/utilities/toolset_version.h"
#include "mcrl2/atermpp/standard_containers/vector.h"
#include "mcrl2/data/detail/rewrite/jitty.h"
#include "mcrl2/data/detail/rewrite/match_tree.h"
#include "mcrl2/data/detail/rewrite/nfs_array.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

#ifdef MCRL2_ENABLE_JITTYC

namespace mcrl2::data::detail
{

using sort_list_vector = std::vector<sort_expression_list>;

///
/// \brief The normal_form_cache class stores normal forms of data_expressions that
///        are inserted in it. By keeping the cache on the stack, the normal forms
///        in it will not be freed by the ATerm library, and can therefore be used
///        in the generated jittyc code.
///
class normal_form_cache
{
  private:
    std::set<data_expression> m_lookup;
  public:
    normal_form_cache() = default;

    // Caches cannot be copied or moved. The addresses in the cache must remain available the lifetime of
    // all rewriters using this cache. 
    normal_form_cache(const normal_form_cache& ) = delete;
    normal_form_cache(normal_form_cache&& ) = delete;
    normal_form_cache& operator=(const normal_form_cache& ) = delete;
    normal_form_cache& operator=(normal_form_cache&& ) = delete;
  
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
    const data_expression* cached_term = &*(m_lookup.insert(t).first);
    ss << "*reinterpret_cast<const data_expression*>(" << (void*)(cached_term) << ")";
    return ss.str();
  }

  /// \brief Checks whether the cache is empty.
  /// \return A boolean indicating whether the cache is empty. 
  bool empty() const
  {
    return m_lookup.empty();
  }

  ~normal_form_cache() = default;
};

class RewriterCompilingJitty: public Rewriter
{
  public:
    using substitution_type = Rewriter::substitution_type;
    using rewriter_function = void (*)(data_expression&, const application&, RewriterCompilingJitty*);

    RewriterCompilingJitty(const data_specification& DataSpec, const used_data_equation_selector&);
    ~RewriterCompilingJitty() override;

    rewrite_strategy getStrategy() override;

    data_expression rewrite(const data_expression& term, substitution_type& sigma) override;

    void rewrite(data_expression& result, const data_expression& term, substitution_type& sigma) override;

    // The variable global_sigma is a temporary store to maintain the substitution 
    // sigma during rewriting a single term. It is not a variable for public use.
    substitution_type* global_sigma = nullptr;
    bool rewriting_in_progress;
    rewrite_stack m_rewrite_stack;

    // The data structures below are used to store the variable lists2
    // that are used in the compiling rewriter in forall, where and exists.
    std::vector<variable_list> rewriter_binding_variable_lists;
    std::map <variable_list, std::size_t> variable_list_indices1;

    // This function assigns a unique index to variable list vl and stores
    // vl at this position in the vector rewriter_binding_variable_lists. This is
    // used in the compiling rewriter to obtain this variable again.
    // Note that the static variable variable_indices is not cleared
    // during several runs, as generally the variables bound in rewrite
    // rules do not change.
    std::size_t binding_variable_list_index(const variable_list& vl)
    {
      if (variable_list_indices1.count(vl)>0)
      {
        return variable_list_indices1[vl];
      }
      const std::size_t index_for_vl=rewriter_binding_variable_lists.size();
      variable_list_indices1[vl]=index_for_vl;
      rewriter_binding_variable_lists.push_back(vl);
      return index_for_vl;
    }

    inline variable_list binding_variable_list_get(const std::size_t i)
    {
      return (rewriter_binding_variable_lists[i]);
    }

    // The data structures below are used to store single variables
    // that are bound in lambda, forall and exist operators. When required
    // in the compiling rewriter, these variables can be retrieved from
    // the array rewriter_bound_variables. Variable_indices0 is used
    // to find the right place in this vector. It delivers a variable_index_where_stack_pair.
    // The first variable in this pair indicates the index of the variable.
    // The where_stack is used as lambda, forall, and exist can be intermixed with bound variables.
    // Whenever the variable is bound by a where clause, true is put on the stack.
    // For a variable that is bound otherwise, false is put on the stack.

    struct variable_index_where_stack_pair
    {
      std::size_t variable_index = 0UL;
      // a stacked value true indicates variable bound in a where. Otherwise the
      // variable is bound by a lambda, forall or exist. 
      std::vector<std::string> declaration_stack;

      // Constructor
      variable_index_where_stack_pair(const std::size_t& vi, const std::vector<std::string>& ds)
        : variable_index(vi),
          declaration_stack(ds)
      {}

      // Default constructor
      variable_index_where_stack_pair() = default;
    };

    std::vector<variable> rewriter_bound_variables;
    std::map <variable, variable_index_where_stack_pair > variable_indices0;

    std::size_t bound_variable_index(const variable& v)
    {
      assert(variable_indices0.count(v)>0);
      assert(variable_indices0[v].declaration_stack.back().empty());
      return variable_indices0[v].variable_index;
    }

    // Declare a bound variable, and indicate whether it comes from a where clause in which case
    // it has a non trivial name. 
    // When the variable is bound in a lambda, forall or exists, is_where must be set to false.
    void bound_variable_index_declare(const variable& v, const std::string& name)
    {
      if (variable_indices0.count(v)>0)  
      {
        variable_indices0[v].declaration_stack.push_back(name);
        return;
      }
      const std::size_t index_for_v=rewriter_bound_variables.size();
      variable_indices0[v]=variable_index_where_stack_pair(index_for_v,std::vector<std::string>(1,name));
      rewriter_bound_variables.push_back(v);
    }

    // Declare a list of variables, which are not variables in a where. 
    void bound_variables_index_declare(const variable_list& vl)
    {
      for(const variable& v: vl)
      {
        bound_variable_index_declare(v, "");
      }
    }

    void bound_variable_index_undeclare(const variable& v)
    {
      assert(variable_indices0.count(v)>0);
      assert(variable_indices0[v].declaration_stack.size()>0);
      variable_indices0[v].declaration_stack.pop_back();
    }
   
    void bound_variables_index_undeclare(const variable_list& vl)
    {
      for(const variable& v: vl)
      {
        bound_variable_index_undeclare(v);
      }
    }

    const variable& bound_variable_get(const std::size_t i)
    {
      assert(i<rewriter_bound_variables.size());
      return (rewriter_bound_variables[i]);
    }

    // provides a non empty string if this variable represents a where clause. 
    std::string bound_variable_stems_from_whr_clause(const variable& v)
    {
      assert(variable_indices0.count(v)>0);
      assert(variable_indices0[v].declaration_stack.size()>0);
      return variable_indices0[v].declaration_stack.back(); // if true, the variable is bound in a where clause. 
    }

    // The following values are used to locate rewrite functions in the tables of
    // precompiled functions. 
    //   arity_bound -- The maximum occurring arity + 1
    //   index_bound -- The maximum occurring index + 1
    std::size_t arity_bound = 0UL;
    std::size_t index_bound = 0UL;

    // The two arrays below are intended to contain the precompiled functions used
    // for rewriting. They are used to find the relevant compiled rewriting code quickly. 
    std::vector<rewriter_function> functions_when_arguments_are_not_in_normal_form;
    std::vector<rewriter_function> functions_when_arguments_are_in_normal_form;
 
    // The following vector is to store normal forms of constants, indexed by the sequence number in a constant. 
    std::vector<data_expression> normal_forms_for_constants;

    // Standard assignment operator.
    RewriterCompilingJitty& operator=(const RewriterCompilingJitty& other)=delete;

    std::shared_ptr<detail::Rewriter> clone() override
    {
      return std::shared_ptr<Rewriter>(new RewriterCompilingJitty(*this));
    }

  protected:
    class ImplementTree;
    friend class ImplementTree;
    
    RewriterJitty jitty_rewriter;
    std::set < data_equation > rewrite_rules;
    const match_tree dummy=match_tree();
    bool made_files;
    std::map<function_symbol, data_equation_list> jittyc_eqns;
    std::set<function_symbol> m_extra_symbols;

    std::shared_ptr<uncompiled_library> rewriter_so;
    std::shared_ptr<normal_form_cache> m_nf_cache;

    // The rewriter maintains a copy of busy and forbidden flag,
    // to allow for faster access to them. These flags are used extensively and
    // in clang version 2021 access to them via the compiler, using tlv_get_access,
    // is relatively slow. It is expected that in some future version of the compiler
    // such access is faster, and no copy of these flags is needed anymore. 
  public:
    atermpp::detail::thread_aterm_pool* m_thread_aterm_pool = nullptr;

  protected:
    // Copy construction. Not (yet) for public use.
    RewriterCompilingJitty(RewriterCompilingJitty& other) = default;

    void (*so_rewr_cleanup)();
    void (*so_rewr)(data_expression& result, const data_expression&, RewriterCompilingJitty*);

    void add_base_nfs(nfs_array& a, const function_symbol& opid, std::size_t arity);
    void extend_nfs(nfs_array& a, const function_symbol& opid, std::size_t arity);
    bool opid_is_nf(const function_symbol& opid, std::size_t num_args);
    void calc_nfs_list(nfs_array& a, const application& args, variable_or_number_list nnfvars);
    bool calc_nfs(const data_expression& t, variable_or_number_list nnfvars);
    void CleanupRewriteSystem();
    void BuildRewriteSystem();
    void generate_code(const std::string& filename);
    void generate_rewr_functions(std::ostream& s, const data::function_symbol& func, const data_equation_list& eqs);
    bool lift_rewrite_rule_to_right_arity(data_equation& e, std::size_t requested_arity);
    sort_list_vector
    get_residual_sorts(const sort_expression& s, std::size_t actual_arity, std::size_t requested_arity);
    match_tree_list create_strategy(const data_equation_list& rules, std::size_t arity);
    void term2seq(const data_expression& t, match_tree_list& s, std::size_t* var_cnt, bool omit_head);
    match_tree_list create_sequence(const data_equation& rule, std::size_t* var_cnt);
    match_tree_list subst_var(const match_tree_list& l,
      const variable& old,
      const variable& new_val,
      std::size_t num,
      const mutable_map_substitution<>& substs);
    match_tree build_tree(build_pars pars, std::size_t i);
    match_tree create_tree(const data_equation_list& rules);

    void thread_initialise() override
    {
      mCRL2log(mcrl2::log::debug) << "Initialise busy/forbidden flags\n";
      m_thread_aterm_pool = &atermpp::detail::g_thread_term_pool();
  }
};

struct rewriter_interface
{
  std::string caller_toolset_version;
  std::string status;
  RewriterCompilingJitty* rewriter;
  void (*rewrite_external)(data_expression& result, const data_expression& t, RewriterCompilingJitty*);
  void (*rewrite_cleanup)();
};

}

#endif // MCRL2_ENABLE_JITTYC

#endif // MCRL2_DATA_DETAIL_REWR_JITTYC_H
