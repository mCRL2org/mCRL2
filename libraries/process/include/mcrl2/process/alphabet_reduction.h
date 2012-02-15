// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet_reduction.h
/// \brief Function for applying the alphabet axioms to a process specification.

#ifndef MCRL2_PROCESS_ALPHABET_REDUCTION_H
#define MCRL2_PROCESS_ALPHABET_REDUCTION_H

#include <iostream>
#include <string>
#include <sstream>
#include "mcrl2/process/process_specification.h"

namespace mcrl2
{

namespace process
{

class alphabet_reduction
{
  protected:

    typedef atermpp::term_list < lps::action_label_list > action_label_list_list;
    
    static inline process_identifier INIT_KEY(void)
    {
      return process_identifier(core::identifier_string("init"),mcrl2::data::sort_expression_list());
    }

    atermpp::map < process_expression, action_label_list_list > alphas;
    atermpp::map < process_identifier, action_label_list_list > alphas_process_identifiers;
    atermpp::map < ATermList, action_label_list_list > alphas_length;
    atermpp::map < ATermAppl, process_identifier > subs_alpha;
    atermpp::map < process_identifier, process_identifier > subs_alpha_rev;
    atermpp::map < process_identifier, mcrl2::data::variable_list > form_pars;
 
    atermpp::map < process_identifier, process_expression > procs;
    bool all_stable;
 
    atermpp::map < lps::action_label_list, core::identifier_string_list > untypes;

    //Classify the *reachable from init* process equations in 3 parts:
    //pCRL
    //n-parallel pCRL
    //mCRL
    //also check if process name depends recursively on itself.
    //(mCRL processes cannot recursively depend on itself for the *current* linearizer to work)
    //n-parallel pCRL processes always recursively depend on themselves
    atermpp::set < process_identifier > pCRL_set;
    atermpp::set < process_identifier > mCRL_set;
    atermpp::set < process_identifier > recursive_set;
    atermpp::set < process_identifier > non_recursive_set;

    //process dependencies : P(Pname,type) -> List(P(Pname,type))
    atermpp::map < process_identifier, process_identifier_list > deps;

    bool push_comm_through_allow; //at some point is set to false to avoid infinite recursion.

    core::identifier_string_list untypeMA(lps::action_label_list MAct);
    atermpp::term_list < core::identifier_string_list > untypeMAL(action_label_list_list LMAct);;
    template <class T>
    atermpp::term_list < T > sync_mact(atermpp::term_list < T > a, atermpp::term_list < T > b);
    action_label_list_list filter_block_list(action_label_list_list l, core::identifier_string_list H);
    action_label_list_list filter_hide_list(action_label_list_list l, core::identifier_string_list I);
    action_label_list_list filter_allow_list(action_label_list_list l, atermpp::term_list < core::identifier_string_list > V);
    atermpp::term_list < core::identifier_string_list > split_allow(
            atermpp::term_list < core::identifier_string_list > V, 
            atermpp::term_list < core::identifier_string_list > ulp, 
            atermpp::term_list < core::identifier_string_list > ulq);
    template < class T >
    action_label_list_list sync_list(
           atermpp::term_list < T > l,                       
           atermpp::term_list < T > m,     
            size_t length=0, 
            atermpp::term_list < core::identifier_string_list > allowed=atermpp::term_list < core::identifier_string_list >());
    action_label_list_list sync_list_ht(action_label_list_list l1, action_label_list_list l2, bool including_products_of_actions=true);
    atermpp::term_list< core::identifier_string_list > apply_unrename(core::identifier_string_list l, rename_expression_list R);
    atermpp::term_list < core::identifier_string_list > gsaMakeMultActNameL(atermpp::term_list< atermpp::term_list < core::identifier_string > > l);
    atermpp::term_list < core::identifier_string_list > apply_unrename_allow_list(atermpp::term_list < core::identifier_string_list > V, rename_expression_list R);
    alphabet_reduction::action_label_list_list  apply_comms(lps::action_label_list l, communication_expression_list C);
    atermpp::term_list < core::identifier_string_list > extend_allow_comm_with_alpha(
            atermpp::term_list < core::identifier_string_list > V,
            communication_expression_list  C, 
            action_label_list_list l);
    atermpp::term_list < core::identifier_string_list > extend_allow_comm(atermpp::term_list < core::identifier_string_list > V, communication_expression_list C);
    action_label_list_list filter_comm_list(action_label_list_list l, communication_expression_list C);
    process_expression PushBlock(core::identifier_string_list H, process_expression a);
    process_expression PushHide(core::identifier_string_list I, process_expression a);
    process_expression PushAllow(atermpp::term_list < core::identifier_string_list > V, process_expression a);
    process_expression PushComm(communication_expression_list C, process_expression a);
    action_label_list_list gsaGetAlpha(
            process_expression a, 
            size_t length=0, 
            atermpp::term_list < core::identifier_string_list > allowed=atermpp::term_list < core::identifier_string_list >());
    action_label_list_list gsaGetSyncAlpha(process_expression a, size_t length, action_label_list_list allowed, bool &success);
    process_expression gsApplyAlpha(process_expression a);
    process_identifier_list gsaGetDeps(process_expression a);
    action_label_list_list filter_rename_list(action_label_list_list l, rename_expression_list R);
    bool gsaGetProp(const process_expression a, process_identifier context);
    void gsAlpha(process_equation_list& equations, process_expression& init);

 
  public:
    /// \brief     Applies alphabet reduction to a process specification.
    /// \param[in,out] p An mCRL2 process specificiation that has been
    ///            type checked.
    /// \post      Alphabet reductions have been applied to p.
    /// \exception mcrl2::runtime_error Alphabet reduction failed.
    // void apply_alphabet_reduction(process_specification& p);

    void operator ()(process_specification& p);

    alphabet_reduction():
      all_stable(false),
      push_comm_through_allow(true)
    {}
  
};

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_REDUCTION_H

