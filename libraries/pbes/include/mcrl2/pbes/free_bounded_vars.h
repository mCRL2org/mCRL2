// Author(s): Simona Orzan.
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file free_bounded_vars.h

#ifndef MCRL2_PBES_FREE_BOUNDED_VARS_H
#define MCRL2_PBES_FREE_BOUNDED_VARS_H

#include <set>
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/new_data/find.h"
#include "mcrl2/new_data/utility.h"
#include "mcrl2/new_data/detail/find.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/util.h"

std::set<std::string> context;

// auxiliary function that replaces all variables
// from the given list bv with new variables
mcrl2::pbes_system::pbes_expression newnames(mcrl2::pbes_system::pbes_expression p, mcrl2::new_data::variable_list bv)
{
  if (bv.empty()) return p;
  mcrl2::new_data::variable_list newbv = fresh_variables(bv,context,"%d");
  for (mcrl2::new_data::variable_list::const_iterator x = newbv.begin(); x != newbv.end(); x++)
    context.insert(x->name());
  //  std::cout<<"\nNEWNAMES bv="<< pp(bv).c_str()<<" , newbv=" << pp(newbv).c_str()<<"\n";
  return p.substitute(make_list_substitution(bv, newbv));
}




/// Renames some bounded variable occurences with new variables,
/// such that, in the returned expression:
///  . every variable is bounded by only one quantifier
///  . no variable occurs both bounded and free
/// If necessary, more than one new names for the same old name will be introduced.
/// For instance, forall x.A(x) /\ exists x.B(x)
/// becomes forall x0.A(x0) /\ exists x1.B(x1).
///
/// As side effect, two variable lists are returned:
/// the free and the bounded occurences
mcrl2::pbes_system::pbes_expression remove_double_variables_rec
(mcrl2::pbes_system::pbes_expression p, mcrl2::new_data::variable_list* fv, mcrl2::new_data::variable_list* bv)
{
  using namespace mcrl2;
  using namespace mcrl2::new_data;
  using namespace mcrl2::pbes_system;
  using namespace mcrl2::pbes_system::pbes_expr;
  using namespace mcrl2::pbes_system::accessors;

  //  std::cout<<"RDV: start "<<pp(p).c_str()<<"\n";

  if ((is_and(p)) || (is_or(p)) || (is_imp(p))) {
    variable_list fvl,fvr,bvl,bvr;
    pbes_expression pleft = remove_double_variables_rec(left(p),&fvl,&bvl);
    pbes_expression pright = remove_double_variables_rec(right(p),&fvr,&bvr);
    variable_list toreplace = intersect(bvl,fvr);
    dunion(toreplace,intersect(bvl,bvr));
    pbes_expression pleft_ok = newnames(pleft,toreplace);
    toreplace = intersect(bvr,fvl);
    pbes_expression pright_ok = newnames(pright,toreplace);
    *fv = dunion(fvl,fvr); // the free vars are never renamed
    *bv = dunion(bvl,bvr); // no need to add the new names, because they will
                           // not occur again later
    //   std::cout<<"RDV: end "<<pp(pleft_ok).c_str()<<"     *     "<<pp(pright_ok).c_str()<<"\n";

    if (is_and(p)) return and_(pleft_ok,pright_ok);
    else if (is_or(p)) return or_(pleft_ok,pright_ok);
    else return imp(pleft_ok,pright_ok);
  }
  else if ((is_forall(p))||(is_exists(p))) {
    pbes_expression punder = remove_double_variables_rec(arg(p),fv,bv);
    variable_list qv = var(p);
    // if the quantifier is useless, dump it
    if (intersect(qv,*fv).empty()) return punder;
    if (!intersect(qv,*bv).empty()) return punder;
    // otherwise, extend the bv list and leave the quantifier unchanged
    dunion(bv,qv);
    //    std::cout<<"RDV: end  Q"<< pp(qv).c_str()<<pp(punder).c_str()<<"\n";
    return (is_forall(p)? forall(qv,punder):exists(qv,punder));
  }
  else if (is_not(p)){
    return not_(remove_double_variables_rec(arg(p),fv,bv));
  }
  else if (is_data(p)){
    // fill in the list of occuring variables
    std::set<variable> setfv = find_all_variables(p);
    for (std::set<variable>::iterator i=setfv.begin(); i!=setfv.end();i++)
      *fv = push_back(*fv,*i);
    //    std::cout<<"RDV: end " <<pp(p).c_str()<<"\n";
    return p;
  }
  else // true,false or propositional variable instantiation
    {
      //    std::cout<<"RDV: end " <<pp(p).c_str()<<"\n";
      return p;
    }
}


/// Renames some bounded variable occurences with new variables,
/// such that, in the returned expression:
///  . every variable is bounded by only one quantifier
///  . no variable occurs both bounded and free
/// If necessary, more than one new names for the same old name will be introduced.
/// For instance, forall x.A(x) /\ exists x.B(x)
/// becomes forall x0.A(x0) /\ exists x1.B(x1).
///
/// As side effect, two variable lists are returned:
/// the free and the bounded occurences
mcrl2::pbes_system::pbes_expression remove_double_variables(mcrl2::pbes_system::pbes_expression p)
{
  mcrl2::new_data::variable_list fv;
  mcrl2::new_data::variable_list bv;
  context = mcrl2::new_data::detail::find_variable_name_strings(p);
  return (remove_double_variables_rec(p,&fv,&bv));
}





/// Same effect as remove_double_variables,
/// but differently implemented (BRUTE FORCE):
/// all quantified variables are simply renamed with new names,
/// within the scope of their quantifier.
mcrl2::pbes_system::pbes_expression remove_double_variables_fast(mcrl2::pbes_system::pbes_expression p)
{

  // !!!! IMPLEMENT THIS

 return p;
}

#endif // MCRL2_PBES_FREE_BOUNDED_VARS_H
