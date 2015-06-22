// Author(s): Jan Friso Groote, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_TYPECHECK_H
#define MCRL2_DATA_TYPECHECK_H

#include "mcrl2/data/data_specification.h"

#ifdef MCRL2_USE_NEW_TYPE_CHECKER
#include "mcrl2/data/sort_type_checker.h"
#include "mcrl2/data/data_type_checker.h"
#endif

namespace mcrl2
{

namespace data
{

#ifndef MCRL2_USE_NEW_TYPE_CHECKER
class sort_type_checker
{
  protected:
    std::set<basic_sort> m_basic_sorts;
    std::map<basic_sort, sort_expression> m_aliases;
    std::map<sort_expression, basic_sort> m_normalized_aliases;

  public:
    /// \brief constructs a sort expression checker.
    sort_type_checker(const basic_sort_vector& sorts, const alias_vector& aliases, bool check_aliases = true);

    /** \brief     Type check a sort expression.
    *  Throws an exception if the expression is not well typed.
    *  \param[in] s A sort expression that has not been type checked.
    **/
    void operator()(const sort_expression& s);

  protected:
    // Auxiliary functions
    void add_basic_sort(const basic_sort& n);
    void add_alias(const data::alias& x);
    bool check_for_sort_alias_loop_through_function_sort_via_expression(
             const sort_expression& sort_expression_start_search,
             const basic_sort& end_search,
             std::set < basic_sort > &visited,
             const bool observed_a_sort_constructor);
    bool check_for_sort_alias_loop_through_function_sort(
             const basic_sort& start_search,
             const basic_sort& end_search,
             std::set<basic_sort>& visited,
             const bool observed_a_sort_constructor);
    void check_sort_is_declared(const sort_expression& x);
    void check_sort_list_is_declared(const sort_expression_list& x);
    void check_basic_sort_is_declared(const basic_sort& x);
    void check_for_empty_constructor_domains(function_symbol_list constructors);

    // Throws a runtime_error if a rewriting loop is detected.
    // Also closes the mapping m_normalized_aliases under itself(!)
    void check_alias_circularity(const data::basic_sort& lhs, const data::sort_expression& rhs);

    // Throws an exception if the alias lhs -> rhs is recursively defined
    void check_alias_recursion(const data::basic_sort& lhs, const data::sort_expression& rhs);

    // Throws an exception if there is a problem with the alias
    void check_alias(const data::basic_sort& lhs, const data::sort_expression& rhs);

    // Normalizes alias x, and adds it to m_normalized_aliases
    void normalize_alias(const data::alias& x);
};

class data_type_checker: public sort_type_checker
{
  protected:
    bool was_warning_upcasting;
    bool was_ambiguous;
    std::map<core::identifier_string,sort_expression_list> system_constants;   //name -> Set(sort expression)
    std::map<core::identifier_string,sort_expression_list> system_functions;   //name -> Set(sort expression)
    std::map<core::identifier_string,sort_expression> user_constants;          //name -> sort expression
    std::map<core::identifier_string,sort_expression_list> user_functions;     //name -> Set(sort expression)
    data_specification type_checked_data_spec;

  public:
    /** \brief     make a data type checker.
     *  Throws a mcrl2::runtime_error exception if the data_specification is not well typed.
     *  \param[in] data_spec A data specification that does not need to have been type checked.
     *  \return    a data expression where all untyped identifiers have been replace by typed ones.
     **/
    data_type_checker(const data_specification &data_spec);

    /** \brief     Type check a data expression.
     *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
     *  \param[in] d A data expression that has not been type checked.
     *  \param[in] Vars a mapping of variable names to their types.
     *  \return    a data expression where all untyped identifiers have been replace by typed ones.
     **/
    data_expression operator()(const data_expression &d,const std::map<core::identifier_string,sort_expression> &Vars);

    /** \brief     Type check a data expression.
     *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
     *  \param[in] l A list of variables that has not been type checked.
     *  \return    a data expression where all untyped identifiers have been replace by typed ones.
     **/
    variable_list operator()(const variable_list &l);

    /** \brief     Yields a type checked data specification, provided typechecking was successful.
     *  \return    a data specification where all untyped identifiers have been replace by typed ones.
     *  \post      sort_expr is type checked.
     **/
    const data_specification operator()();

  protected:
    void read_sort(const sort_expression& SortExpr);
    void read_constructors_and_mappings(const function_symbol_vector& constructors, const function_symbol_vector& mappings);
    void add_function(const data::function_symbol &f, const std::string msg, bool allow_double_decls=false);
    void add_constant(const data::function_symbol &OpId, const std::string msg);
    void initialise_system_defined_functions(void);
    void add_system_constant(const data::function_symbol& f);
    void add_system_function(const data::function_symbol& f);
    bool TypeMatchA(const sort_expression& Type_in, const sort_expression& PosType_in, sort_expression& result);
    bool TypeMatchL(const sort_expression_list& TypeList, const sort_expression_list &PosTypeList, sort_expression_list& result);
    sort_expression UnwindType(const sort_expression& Type);
    variable UnwindType(const variable& Type);
    template <class T>
    atermpp::term_list<T> UnwindType(const atermpp::term_list<T>& l)
    {
      std::vector<T> result;
      for(typename atermpp::term_list<T>::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        result.push_back(UnwindType(*i));
      }
      return atermpp::term_list<T>(result.begin(),result.end());
    }

    sort_expression TraverseVarConsTypeD(
                        const std::map<core::identifier_string,sort_expression> &DeclaredVars,
                        const std::map<core::identifier_string,sort_expression> &AllowedVars,
                        data_expression &DataTerm,
                        sort_expression PosType,
                        std::map<core::identifier_string,sort_expression> &FreeVars,
                        const bool strictly_ambiguous=true,
                        const bool warn_upcasting=false,
                        const bool print_cast_error=true);
    sort_expression TraverseVarConsTypeD(const std::map<core::identifier_string,sort_expression> &DeclaredVars,
                                         const std::map<core::identifier_string,sort_expression> &AllowedVars,
                                         data_expression &t1,
                                         sort_expression t2)
    {
      std::map<core::identifier_string,sort_expression> dummy_table;
      return TraverseVarConsTypeD(DeclaredVars, AllowedVars, t1, t2, dummy_table);
    }

    sort_expression TraverseVarConsTypeDN(
                           const std::map<core::identifier_string,sort_expression> &DeclaredVars,
                           const std::map<core::identifier_string,sort_expression> &AllowedVars,
                           data_expression &DataTerm,
                           sort_expression PosType,
                           std::map<core::identifier_string,sort_expression> &FreeVars,
                           const bool strictly_ambiguous=true,
                           const size_t nFactPars=std::string::npos,
                           const bool warn_upcasting=false,
                           const bool print_cast_error=true);
    void AddVars2Table(std::map<core::identifier_string, sort_expression>& variable_map, const variable_list& declared_variables)
    {
      for (const variable& v: declared_variables)
      {
        // TODO: this should be checked elsewhere
        check_sort_is_declared(v.sort());
        variable_map[v.name()] = v.sort();
      }
      result = variable_map;
    }

    bool InTypesA(sort_expression Type, sort_expression_list Types);
    bool EqTypesA(sort_expression Type1, sort_expression Type2);
    bool InTypesL(sort_expression_list Type, atermpp::term_list<sort_expression_list> Types);
    bool EqTypesL(sort_expression_list Type1, sort_expression_list Type2);
    bool MaximumType(const sort_expression &Type1, const sort_expression &Type2, sort_expression &result);
    sort_expression ExpandNumTypesUp(sort_expression Type);
    sort_expression_list ExpandNumTypesUpL(const sort_expression_list& type_list);
    sort_expression ExpandNumTypesDown(sort_expression Type);
    bool UnifyMinType(const sort_expression &Type1, const sort_expression &Type2, sort_expression &result);
    sort_expression determine_allowed_type(const data_expression &d, const sort_expression &proposed_type);
    bool MatchIf(const function_sort &type, sort_expression &result);
    bool MatchEqNeqComparison(const function_sort &type, sort_expression &result);
    bool MatchSqrt(const function_sort &type, sort_expression &result);
    bool MatchListOpCons(const function_sort &type, sort_expression &result);
    bool MatchListOpSnoc(const function_sort &type, sort_expression &result);
    bool MatchListOpConcat(const function_sort &type, sort_expression &result);
    bool MatchListOpEltAt(const function_sort &type, sort_expression &result);
    bool MatchListOpHead(const function_sort &type, sort_expression &result);
    bool MatchListOpTail(const function_sort &type, sort_expression &result);
    bool MatchSetOpSet2Bag(const function_sort &type, sort_expression &result);
    bool MatchFalseFunction(const function_sort &type, sort_expression &result);
    bool MatchListSetBagOpIn(const function_sort &type, sort_expression &result);
    bool match_fset_insert(const function_sort &type, sort_expression &result);
    bool match_fbag_cinsert(const function_sort &type, sort_expression &result);
    bool MatchSetBagOpUnionDiffIntersect(const function_sort &type, sort_expression &result);
    bool MatchSetOpSetCompl(const function_sort &type, sort_expression &result);
    bool MatchBagOpBag2Set(const function_sort &type, sort_expression &result);
    bool MatchBagOpBagCount(const function_sort &type, sort_expression &result);
    bool MatchFuncUpdate(const function_sort &type, sort_expression &result);
    bool MatchSetConstructor(const function_sort &type, sort_expression &result);
    bool MatchBagConstructor(const function_sort &type, sort_expression &result);
    bool UnArrowProd(sort_expression_list ArgTypes, sort_expression PosType, sort_expression &result);
    bool UnFSet(sort_expression PosType, sort_expression &result);
    bool UnFBag(sort_expression PosType, sort_expression &result);
    bool UnList(sort_expression PosType, sort_expression &result);
    void ErrorMsgCannotCast(sort_expression CandidateType, data_expression_list Arguments, sort_expression_list ArgumentTypes,std::string previous_reason);
    sort_expression UpCastNumericType(
                    sort_expression NeededType,
                    sort_expression Type,
                    data_expression &Par,
                    const std::map<core::identifier_string,sort_expression> &DeclaredVars,
                    const std::map<core::identifier_string,sort_expression> &AllowedVars,
                    std::map<core::identifier_string,sort_expression> &FreeVars,
                    const bool strictly_ambiguous,
                    bool warn_upcasting=false,
                    const bool print_cast_error=false);
    bool VarsUnique(const variable_list &VarDecls);
    void TransformVarConsTypeData(data_specification &data_spec);
    sort_expression_list GetNotInferredList(const atermpp::term_list<sort_expression_list> &TypeListList);
    sort_expression_list InsertType(const sort_expression_list TypeList, const sort_expression Type);
    std::pair<bool,sort_expression_list> AdjustNotInferredList(
            const sort_expression_list &PosTypeList,
            const atermpp::term_list<sort_expression_list> &TypeListList);
    bool IsTypeAllowedA(const sort_expression &Type, const sort_expression &PosType);
    bool IsTypeAllowedL(const sort_expression_list &TypeList, const sort_expression_list PosTypeList);
    bool IsNotInferredL(sort_expression_list TypeList);
    bool strict_type_check(const data_expression& d);
};
#endif // MCRL2_USE_NEW_TYPE_CHECKER

/** \brief     Type check a sort expression.
 *  Throws an exception if something went wrong.
 *  \param[in] sort_expr A sort expression that has not been type checked.
 *  \param[in] data_spec The data specification to use as context.
 *  \post      sort_expr is type checked.
 **/
inline
void type_check(const sort_expression& sort_expr, const data_specification& data_spec)
{
  try
  {
    sort_type_checker type_checker(data_spec.user_defined_sorts(), data_spec.user_defined_aliases());
    type_checker(sort_expr);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check sort " + pp(sort_expr));
  }
}

/** \brief     Type check a data expression.
 *  Throws an exception if something went wrong.
 *  \param[in] data_expr A data expression that has not been type checked.
 *  \param[in] first The start of a variables that can occur in the data expression.
 *  \param[in] last  The end of the potentially free variables in the expression.
 *  \param[in] data_spec The data specification that is used for type checking.
 *  \post      data_expr is type checked.
 **/
template <typename VariableIterator>
void type_check(data_expression& data_expr,
                const VariableIterator first,
                const VariableIterator last,
                const data_specification& data_spec = data_specification())
{
  data_expression t = data_expr;

  std::map<core::identifier_string,sort_expression> variables;
  for (VariableIterator v = first; v != last; ++v)
  {
    variables[v->name()]=v->sort();
  }

  // The typechecker replaces untyped identifiers by typed identifiers (when typechecking
  // succeeds) and adds type transformations between terms of sorts Pos, Nat, Int and Real if necessary.
  try
  {
    data_type_checker type_checker(data_spec);
    data_expr = type_checker(data_expr,variables);
#ifndef MCRL2_DISABLE_TYPECHECK_ASSERTIONS
    // assert(!search_sort_expression(data_expr, untyped_sort())); Terms with untyped sorts, such as [], {} and {:} are
    // returned by the typechecker.
#endif
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check data expression " + pp(t));
  }
}

/** \brief     Type check a data expression.
 *  Throws an exception if something went wrong.
 *  \param[in] data_expr A data expression that has not been type checked.
 *  \param[in] data_spec Data specification to be used as context.
 *  \post      data_expr is type checked.
 **/
inline
void type_check(data_expression& data_expr, const data_specification& data_spec = data_specification())
{
  variable_list v;
  return type_check(data_expr, v.begin(), v.end(), data_spec);
}

/** \brief     Type check a parsed mCRL2 data specification.
 *  Throws an exception if something went wrong.
 *  \param[in] data_spec A data specification that has not been type checked.
 *  \post      data_spec is type checked.
 **/
inline
void type_check(data_specification& data_spec)
{
  try
  {
    data_type_checker type_checker(data_spec);
    data_spec=type_checker();
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check data specification " + pp(data_spec));
  }
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TYPECHECK_H
