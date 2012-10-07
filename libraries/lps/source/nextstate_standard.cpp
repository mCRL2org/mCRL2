// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file nextstate_standard.cpp

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <memory>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/pp_deprecated.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/lps/nextstate/standard.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::log;

namespace mcrl2
{
namespace lps
{

#ifdef MCRL2_NEXTSTATE_DEBUG

static std::string print_state(const atermpp::aterm_appl &s, ns_info const& info)
{
  if (info.stateformat==GS_STATE_TREE)
  {
    mCRL2log(debug) << "GS_STATE_TREE... " << s << "\n";
    return "";
  }

  std::string result("state(");
  size_t index = 0;
  for (atermpp::aterm_appl::const_iterator i = s.begin(); i != s.end(); ++i)
  {
    if (index++ != 0)
    {
      result.append(", ");
    }

    result.append(data::pp(atermpp::aterm(info.m_rewriter.convert_from(static_cast<aterm>(*i)))));
  }
  result.append(")");
  return result;
}

static std::string print_assignments(const atermpp::aterm_list &a, ns_info const& info)
{
  atermpp::aterm_list res;
  for (atermpp::aterm_list::const_iterator i = a.begin(); i != a.end(); ++i)
  {
    aterm_appl t = atermpp::aterm_appl(*i);
    if (t == gsMakeNil())
    {
      res = atermpp::push_front(res, atermpp::aterm(t));
    }
    else
    {
      res = atermpp::push_front(res, atermpp::aterm(info.m_rewriter.convert_from(t)));
    }
  }
  res = atermpp::reverse(res);
  return core::pp_deprecated(res);
}
#endif // MCRL2_NEXTSTATE_DEBUG

/* Explanation of the tree building algorithm.
 *
 * If len(l) is a power of 2, than building the tree is easy. One just
 * pairs every element in the list with its neighbor, giving again a list
 * with the length a power of 2, repeating until there is only one element
 * (the result). For example, [0,1,2,3] -> [(0,1),(2,3)] -> ((0,1),(2,3)).
 *
 * With an odd number of elements, our first goal is to pair just a few
 * elements to get a list with the length a power of 2. We do this in such
 * a way that it is as easy as possible to lookup an element.
 *
 * The easiest way of looking up an element in a tree is probably when, for
 * a tree with n leafs, we know that numbers up to n/2 are on the left and
 * numbers above n/2 are on the right. This way we can just compare the
 * index we are looking for with n/2 and choose the right tree and repeat
 * the process.
 *
 * We will use an array tree_init to determine which elements to pair to
 * get a list with the length a power of 2. More specifically, if
 * tree_init[i] is true, then we pair element i-1 and i. It easy to
 * determine which elements should be true by recursively "dividing" the
 * array in two parts using /2.
 */
static void fill_tree_init(bool* init, size_t n, size_t l)
{
  if (l == 0)
  {
    return;
  }

  if (n > 2)
  {
    fill_tree_init(init,n/2,l/2);
    fill_tree_init(init+n/2,n-n/2,l-l/2);
  }
  else /* n == 2 */
  {
    init[1] = true;
  }
}
aterm NextState::buildTree(std::vector<aterm> &args)
{
  size_t n,m;

  if (info.statelen == 0)
  {
    return info.nil;
  }

  if (tree_init == NULL)
  {
    tree_init = (bool*) malloc(info.statelen*sizeof(bool));
    for (size_t i=0; i<info.statelen; i++)
    {
      tree_init[i] = false;
    }
    n = 1;
    while (n <= info.statelen)
    {
      n *= 2;
    }
    n /= 2;
    fill_tree_init(tree_init,info.statelen,info.statelen-n);
  }

  n = 0;
  m = 0;
  while (n < info.statelen)
  {
    if (tree_init[n])
    {
      args[m-1] = aterm_appl(info.pairAFun,args[m-1],args[n]);
    }
    else
    {
      args[m] = args[n];
      m++;
    }
    n++;
  }

  n = m;
  while (n > 1)
  {
    for (size_t i=0; i<n; i+=2)
    {
      args[i/2] = aterm_appl(info.pairAFun,args[i],args[i+1]);
    }

    n /= 2;
  }

  return args[0];
}

aterm NextState::getTreeElement(const aterm &tree1, size_t index)
{
  aterm tree=tree1;
  size_t n = info.statelen;
  size_t m = 0;
  while (m+1 != n)
  {
    size_t t = (m+n)/2;

    assert((tree.type() == AT_APPL) && (tree.function()==info.pairAFun));

    if (index < t)
    {
      tree = ((aterm_appl) tree)(0);
      n = t;
    }
    else
    {
      tree = ((aterm_appl) tree)(1);
      m = t;
    }
  }

  return tree;
}

size_t NextState::getStateLength()
{
  return info.statelen;
}

aterm_appl NextState::getStateArgument(const aterm &state, size_t index)
{
  switch (info.stateformat)
  {
    case GS_STATE_VECTOR:
      return info.m_rewriter.convert_from(atermpp::aterm_appl(((aterm_appl) state)(index)));
    case GS_STATE_TREE:
      return info.m_rewriter.convert_from(atermpp::aterm_appl(getTreeElement(state,index)));
    default:
      return aterm_appl();
  }
}

mcrl2::lps::state NextState::make_new_state_vector(const aterm &s)
{
  mcrl2::lps::state new_state;
  for (size_t i=0; i<info.statelen; i++)
  {
    new_state.push_back(data_expression(getStateArgument(s,i)));
  }
  return new_state;
}

aterm_appl NextState::makeStateVector(const aterm &state)
{
  if (!stateAFun_made)
  {
    stateAFun_made = true;
    info.stateAFun = atermpp::function_symbol("STATE",info.statelen);
  }

  // XXX can be done more efficiently in some cases
  for (size_t i=0; i<info.statelen; i++)
  {
    stateargs[i] = getStateArgument(state,i);
  }
  return aterm_appl(info.stateAFun,stateargs.begin(),stateargs.end());
}

//Prototype
static bool statearg_match(
     const data_expression &arg,
     const data_expression &pat,
     std::map < variable, data_expression > &vars);

static bool statearg_match_list(
     data_expression_list arg,
     data_expression_list pat,
     std::map < variable, data_expression > &vars)
{
  assert(arg.size() == pat.size());
  bool r = true;

  while (!arg.empty() && r)
  {
    r = r && statearg_match(arg.front(), pat.front(), vars);
    arg = pop_front(arg);
    pat = pop_front(pat);
  }

  return r;
}

static bool statearg_match(
     const data_expression &arg,
     const data_expression &pat,
     std::map < variable, data_expression > &vars)
{
  bool r;
  if (is_application(pat))
  {
    if (!is_application(arg))
    {
      r = false;
    }
    else
    {
      r = statearg_match(application(arg).head(),application(pat).head(),vars)
          && statearg_match_list(application(arg).arguments(),application(pat).arguments(),vars);
    }
  }
  else if (is_variable(pat))
  {
    std::map < variable, data_expression >::const_iterator it=vars.find(pat);
    if (it==vars.end())  // not found
    {
      vars[pat]=arg;
      r = true;
    }
    else if (arg== it->second)
    {
      r = true;
    }
    else
    {
      r = false;
    }
  }
  else
  {
    r = arg==pat;
  }

  return r;
}

static bool statearg_match(const data_expression &arg, const data_expression &pat)
{
  std::map < variable, data_expression > vars;
  return statearg_match(arg,pat,vars);

}

aterm NextState::parse_state_vector_new(const mcrl2::lps::state &s, const mcrl2::lps::state &match, bool check_match)
{
  if (!stateAFun_made)
  {
    stateAFun_made = true;
    info.stateAFun = atermpp::function_symbol("STATE",info.statelen);
  }

  bool valid = true;
  aterm_list l = info.procvars;
  for (size_t i=0; i<info.statelen; i++)
  {
    stateargs[i] = s[i];
    if (mcrl2::data::data_expression((aterm_appl) stateargs[i]).sort() != mcrl2::data::data_expression(ATAgetFirst(l)).sort())
    {
      valid = false;
      break;
    }

    if (check_match && !statearg_match(data_expression(stateargs[i]),match[i]))
    {
      valid = false;
      break;
    }
    stateargs[i] = info.m_rewriter.convert_to((data_expression)(aterm_appl) stateargs[i]);
    l = l.tail();
  }
  if (valid)
  {
    switch (info.stateformat)
    {
      case GS_STATE_VECTOR:
        return aterm_appl(info.stateAFun,stateargs.begin(),stateargs.end());
        break;
      case GS_STATE_TREE:
        return buildTree(stateargs);
        break;
      default:
        break;
    }
  }

  return aterm();
}

aterm NextState::parseStateVector(const aterm_appl &state, const aterm &match)
{
  if (!stateAFun_made)
  {
    stateAFun_made = true;
    info.stateAFun = atermpp::function_symbol("STATE",info.statelen);
  }

  if (info.stateAFun==state.function())
  {
    bool valid = true;
    aterm_list l = info.procvars;
    for (size_t i=0; i<info.statelen; i++)
    {
      stateargs[i] = state(i);
      if (mcrl2::data::data_expression((aterm_appl) stateargs[i]).sort() != mcrl2::data::data_expression(ATAgetFirst(l)).sort())
      {
        valid = false;
        break;
      }
      if ((match != aterm()) && !statearg_match(
                                 data_expression(stateargs[i]),
                                 data_expression(getStateArgument(match,i))))
      {
        valid = false;
        break;
      }
      stateargs[i] = info.m_rewriter.convert_to((data_expression)(aterm_appl) stateargs[i]);
      l = l.tail();
    }
    if (valid)
    {
      switch (info.stateformat)
      {
        case GS_STATE_VECTOR:
          return aterm_appl(info.stateAFun,stateargs.begin(),stateargs.end());
          break;
        case GS_STATE_TREE:
          return buildTree(stateargs);
          break;
        default:
          break;
      }
    }
  }

  return aterm();
}

aterm NextState::SetVars(const aterm &a, const aterm_list &free_vars)
{
  if (!usedummies)
  {
    return a;
  }

  mcrl2::data::representative_generator generator(info.m_specification);

  if (a.type_is_list())
  {
    aterm_list l(a);
    aterm_list m;
    for (; !l.empty(); l=l.tail())
    {
      m = push_front(m,SetVars(l.front(),free_vars));
    }
    return reverse(m);
  }
  else if (gsIsDataVarId((aterm_appl) a))
  {
    if (std::find(free_vars.begin(),free_vars.end(),a) != free_vars.end())
    {
      return static_cast< aterm >(static_cast< aterm_appl >(
                                         generator(mcrl2::data::sort_expression(aterm_cast<aterm_appl>((aterm_appl) a)(1)))));
    }
    else
    {
      return a;
    }
  }
  else if (gsIsDataAppl((aterm_appl) a))
  {
    return gsMakeDataAppl((aterm_appl) SetVars(((aterm_appl) a)(0),free_vars),(aterm_list) SetVars(((aterm_appl) a)(1),free_vars));
  }
  else
  {
    return a;
  }
}

aterm_list NextState::ListToFormat(const aterm_list &l, const aterm_list &free_vars)
{
  if (l.empty())
  {
    return l;
  }
  else
  {
    return push_front<aterm>(ListToFormat(l.tail(),free_vars),info.m_rewriter.convert_to((data_expression)(aterm_appl) SetVars(l.front(),free_vars)));
  }
}

aterm_list NextState::ListFromFormat(const aterm_list &l)
{
  if ( l.empty() )
  {
    return l;
  }
  else
  {
    return push_front<aterm>(ListFromFormat(l.tail()),info.m_rewriter.convert_from(atermpp::aterm_appl(l.front())));
  }
}

aterm_list NextStateGenerator::ListFromFormat(const aterm_list &l)
{
  if ( l.empty() )
  {
    return l;
  }
  else
  {
    return push_front<aterm>(ListFromFormat(l.tail()),info.m_rewriter.convert_from(atermpp::aterm_appl(l.front())));
  }
}

aterm_appl NextState::ActionToRewriteFormat(const aterm_appl &act, const aterm_list &free_vars)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextState::ActionToRewriteFormat(act, free_vars) called, with" << std::endl <<
            "  act = " << atermpp::aterm_appl(act) << std::endl <<
            "  act (human readable): " << core::pp_deprecated(atermpp::aterm_appl(act)) << std::endl <<
            "  free_vars = " << atermpp::aterm_list(free_vars) << std::endl;
#endif
  aterm_list l = aterm_cast<aterm_list>(act(0));
  aterm_list m;

  for (; !l.empty(); l=l.tail())
  {
    aterm_appl a = ATAgetFirst(l);
    a = gsMakeAction(aterm_cast<aterm_appl>(a(0)),ListToFormat(aterm_cast<aterm_list>(a(1)),free_vars));
    m = push_front<aterm>(m, a);
  }
  m = reverse(m);

  return gsMakeMultAct(m);
}

aterm_list NextState::AssignsToRewriteFormat(const aterm_list &assigns, const aterm_list &free_vars)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextState::AssignsToRewriteFormat(assigns, free_vars) called, with: " << std::endl <<
            "  assigns = " << atermpp::aterm_list(assigns) << std::endl <<
            "  (human readable assigns): " << core::pp_deprecated(atermpp::aterm_list(assigns)) << std::endl <<
            "  free_vars = " << atermpp::aterm_list(free_vars) << std::endl;
#endif
  size_t i = 0;
  for (aterm_list l=pars; !l.empty(); l=l.tail(),i++)
  {
    bool set = false;

    assert(stateargs.size() == pars.size());
    for (aterm_list m=assigns; !m.empty(); m=m.tail())
    {
      if (aterm_cast<aterm_appl>(ATAgetFirst(m)(0))==ATAgetFirst(l))
      {
        stateargs[i] = info.m_rewriter.convert_to((data_expression)(aterm_appl) SetVars((ATAgetFirst(m))(1),free_vars));
        set = true;
        break;
      }
    }
    if (!set)
    {
      stateargs[i] = gsMakeNil();
    }
  }

  aterm_list r;
  i=info.statelen;
  while (i != 0)
  {
    i--;
    r = push_front(r,stateargs[i]);
  }
  return r;
}

NextState::NextState(mcrl2::lps::specification const& spec,
                                     bool allow_free_vars,
                                     int state_format,
                                     const mcrl2::data::detail::legacy_rewriter& rewriter) : info(spec.data(), rewriter)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextState::Nextate(spec, allow_free_vars, state_format, rewriter), with" <<  std::endl <<
            "  allow_free_vars = " << allow_free_vars << std::endl <<
            "  state_format = " << state_format << std::endl << std::endl;
#endif
  aterm_list l,m,n,free_vars;

  next_id = 0;
  info.current_id = &current_id;

  info.parent = this;

  usedummies = !allow_free_vars;

  tree_init = NULL;

  info.stateformat = state_format;
  info.pairAFun = atermpp::function_symbol("@STATE_PAIR@",2);

  info.nil = gsMakeNil();

  // Declare all constructors to the rewriter to prevent unnecessary compilation.
  // This can be removed if the jittyc compiler is not in use anymore.
  // In certain cases it could be useful to add the mappings also, but this appears to
  // give a substantial performance penalty, due to the addition of symbols to the
  // rewriter that are not used.

  std::set < mcrl2::data::variable > vset=mcrl2::lps::find_variables(spec);
  std::set < mcrl2::data::variable > vfset=mcrl2::lps::find_free_variables(spec);
  std::set < mcrl2::data::variable > diff_set;
  std::set_difference(vfset.begin(),vfset.end(),vset.begin(),vset.end(),std::inserter(diff_set,diff_set.begin()));

  std::set < sort_expression > bounded_sorts;
  for(std::set < mcrl2::data::variable > :: const_iterator i=diff_set.begin(); i!=diff_set.end(); ++i)
  {
    bounded_sorts.insert(i->sort());
  }
  for(std::set < sort_expression > :: const_iterator i=bounded_sorts.begin(); i!=bounded_sorts.end(); ++i)
  {
    const function_symbol_vector constructors(spec.data().constructors(*i));
    for (function_symbol_vector::const_iterator j = constructors.begin(); j != constructors.end(); ++j)
    {
      info.m_rewriter.convert_to(mcrl2::data::data_expression(*j));
    }
  }

  const mcrl2::data::function_symbol_vector constructors(spec.data().constructors());
  for (mcrl2::data::function_symbol_vector::const_iterator i = constructors.begin(); i != constructors.end(); ++i)
  {
    info.m_rewriter.convert_to(mcrl2::data::data_expression(*i));
  }

  free_vars = atermpp::convert< mcrl2::data::variable_list >(spec.global_variables());

  pars = spec.process().process_parameters();

  info.statelen = pars.size();
  if (info.stateformat == GS_STATE_VECTOR)
  {
    stateAFun_made = true;
    info.stateAFun = atermpp::function_symbol("STATE",info.statelen);
  }
  else
  {
    stateAFun_made = false;
  }

  info.procvars = spec.process().process_parameters();

  stateargs = std::vector<aterm>(info.statelen); 

  smndAFun = atermpp::function_symbol("@SMND@",4);
  aterm_list sums = mcrl2::lps::deprecated::linear_process_summands(spec.process());
  l = aterm_list();
  for (bool b=true; !sums.empty(); sums=sums.tail())
  {
    if (b && !gsIsNil(aterm_cast<aterm_appl>(ATAgetFirst(sums)(3))))   // Summand is timed
    {
      mCRL2log(warning) << "specification uses time, which is (currently) not supported; ignoring timing" << std::endl;
      b = false;
    }
    if (!gsIsDelta(aterm_cast<aterm_appl>(ATAgetFirst(sums)(2))))
    {
      l = push_front(l,sums.front());
    }
  }
  sums = reverse(l);
  // info.num_summands = sums.size();
  info.num_prioritised = 0;
  info.summands = std::vector < aterm_appl> (sums.size());
  
  for (size_t i=0; !sums.empty(); sums=sums.tail(),i++)
  {
    info.summands[i] =
      aterm_appl(
        smndAFun,
        ATAgetFirst(sums)(0),
        info.m_rewriter.convert_to((data_expression)SetVars(ATAgetFirst(sums)(1),free_vars)),
        ActionToRewriteFormat(aterm_cast<aterm_appl>(ATAgetFirst(sums)(2)),free_vars),
        AssignsToRewriteFormat(aterm_cast<aterm_list>(ATAgetFirst(sums)(4)),free_vars));
  }

  l = pars;
  m = spec.initial_process().assignments();

  for (size_t i=0; !l.empty(); l=l.tail(), i++)
  {
    n = m;
    bool set = false;
    for (; !n.empty(); n=n.tail())
    {
      if (aterm_cast<aterm_appl>(ATAgetFirst(n)(0))==ATAgetFirst(l))
      {
        stateargs[i] = info.m_rewriter.convert_to((data_expression)SetVars(ATAgetFirst(n)(1),free_vars));
        set = true;
        break;
      }
    }
    if (!set)
    {
      mCRL2log(error) << "Parameter '" << atermpp::aterm(ATAgetFirst(l)(0)) << "' does not have an initial value." << std::endl;
      initial_state = aterm_appl();
      return;
    }
  }

  // Rewrite the state arguments en block, as otherwise the generation of new symbols in the
  // rewriter is intermingled with rewriting, causing the rewriter to rewrite too often.

  internal_substitution_type dummy;
  for (size_t i=0; i<info.statelen; i++)
  {
    stateargs[i] = info.m_rewriter.rewrite_internal(atermpp::aterm_cast<const atermpp::aterm_appl>(stateargs[i]),dummy);
  }

  initial_state = aterm_appl();

  switch (info.stateformat)
  {
    case GS_STATE_VECTOR:
      initial_state = aterm_appl(info.stateAFun,stateargs.begin(),stateargs.end());
      break;
    case GS_STATE_TREE:
      initial_state = buildTree(stateargs);
      break;
    default:
      break;
  }
}

NextState::~NextState()
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextState::~NextState called" << std::endl;
#endif

  free(tree_init);
}

static bool only_action(const aterm_list &ma, const char* action)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "only_action(ma, action) called, with " << std::endl <<
            "  ma = " << atermpp::aterm_list(ma) << std::endl <<
            "  action = " << std::string(action) << std::endl;
#endif
  if (ma.empty())
  {
    return false;
  }

  for (aterm_list::const_iterator i=ma.begin(); i!=ma.end(); ++i) 
  {
    if (strcmp(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(*i)(0))(0).function().name().c_str(),action))
    {
      return false;
    }
  }
  return true;
}
void NextState::prioritise(const char* action)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "Nextstate::prioritise called" << std::endl;
#endif
  // XXX this function invalidates currently used generators!
  // perhaps
  bool is_tau = !strcmp(action,"tau");
  size_t pos = 0;
  size_t rest = 0;

  while (pos < info.summands.size())
  {
    aterm_appl s = info.summands[pos];
    aterm_list ma = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(s(2))(0));
    if ((is_tau && ma.empty()) || (!is_tau && only_action(ma,action)))
    {
      //if ( rest < pos )
      //{
      info.summands[pos] = info.summands[rest];
      info.summands[rest] = s;
      //}
      rest++;
    }
    pos++;
  }

  info.num_prioritised += rest;
}

aterm NextState::getInitialState()
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextState::getInitialState()." << std::endl <<
            "Result: " << atermpp::aterm(initial_state) << std::endl <<
            "Result (human readable): " << print_state(atermpp::aterm_appl(initial_state), info) << std::endl;
#endif
  return initial_state;
}

NextStateGenerator* NextState::getNextStates(const aterm &state, NextStateGenerator* old)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextState::getNextStates(state, old) called, with " <<
            "  state = " << atermpp::aterm(state) << "old = " << old << std::endl <<
            "  (human readable state) " << print_state(atermpp::aterm_appl(state), info) << std::endl;
#endif
  if (old == NULL)
  {
    return new NextStateGenerator(state,info,next_id++);
  }

  static_cast< NextStateGenerator* >(old)->reset(state);

  return old;
}

class NextStateGeneratorSummand : public NextStateGenerator
{

  public:

    NextStateGeneratorSummand(size_t summand, const aterm &state, ns_info& info, size_t identifier)
      : NextStateGenerator(state, info, identifier, true, summand)
    {

#ifdef MCRL2_NEXTSTATE_DEBUG
      std::clog << "NextStateGeneratorSummand(summand, state, info, identifier)" <<
                "  summand = " << summand << std::endl <<
                "  state = " << atermpp::aterm(state) << std::endl <<
                "  (human readable state) " << print_state(atermpp::aterm_appl(state), info) << std::endl <<
                "  identifier = " << identifier << std::endl;
#endif
      assert(summand < info.summands.size());
    }
};

NextStateGenerator* NextState::getNextStates(const aterm &state, size_t index, NextStateGenerator* old)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextState::getNextStates(state, index, old) called, with " <<
            "  state = " << atermpp::aterm(state) << std::endl <<
            "  (human readable state) " << print_state(atermpp::aterm_appl(state), info) << std::endl <<
            "  index = " << index << std::endl <<
            "  old = " << old << std::endl;
#endif
  assert(index < info.summands.size());
  if (old != 0)
  {
    static_cast< NextStateGeneratorSummand* >(old)->reset(state, index);

    return old;
  }

  return new NextStateGeneratorSummand(index, state,info,next_id++);
}

aterm NextStateGenerator::makeNewState(const aterm &old, const aterm_list &assigns)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextStateGenerator::makeNewState(old, assigns) called, with " << std::endl <<
            "  old = " << atermpp::aterm(old) << std::endl <<
            "  (human readable old): " << print_state(atermpp::aterm_appl(old), info) << std::endl <<
            "  assigns = " << atermpp::aterm_list(assigns) << std::endl;
  std::clog <<
            "  (human readable assigns): " << print_assignments(atermpp::aterm_list(assigns), info) << std::endl;
#endif

  if (*info.current_id != id)
  {
    set_substitutions();
  }

  size_t i=0;
  aterm_list::const_iterator a=assigns.begin();
  for (atermpp::aterm_list::const_iterator j=info.procvars.begin(); j!=info.procvars.end(); ++i, ++j, ++a)
  // for (size_t i=0; i<info.statelen; i++)
  {
    if (*a==info.nil)
    {
      switch (info.stateformat)
      {
        default:
        case GS_STATE_VECTOR:
          stateargs[i] = atermpp::aterm_cast<atermpp::aterm_appl>(old)(i);
          break;
        case GS_STATE_TREE:
          stateargs[i] = current_substitution(atermpp::aterm_cast<const variable>(*j));
          if (stateargs[i]== *j)   // Make sure substitutions were not reset by enumerator
          {
            set_substitutions();
            stateargs[i] = current_substitution(atermpp::aterm_cast<const variable>(*j));
          }
          break;
      }
    }
    else
    {

      stateargs[i] = info.m_rewriter.rewrite_internal(atermpp::aterm_cast<atermpp::aterm_appl>(*a),current_substitution);
      // The assertion below is not true if there are global variables,
      // which is for instance the case for lpsxsim and lpssim.
      // assert(mcrl2::data::find_variables(atermpp::make_list(mcrl2::data::data_expression(info.m_rewriter.convert_from(stateargs[i])))).empty());
    }
  }
  switch (info.stateformat)
  {
    case GS_STATE_VECTOR:
    {
      return atermpp::aterm_appl(info.stateAFun,stateargs.begin(),stateargs.end());
    }
    case GS_STATE_TREE:
      return info.parent->buildTree(stateargs);
    default:
      return aterm();
  }
}

aterm_appl NextStateGenerator::rewrActionArgs(const aterm_appl &act)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextStateGenerator::rewrActionArgs(act) called, with " << std::endl <<
            "  act = " << atermpp::aterm_appl(act) << std::endl;
#endif
  aterm_list l = aterm_cast<aterm_list>(act(0));
  aterm_list m;

  for (; !l.empty(); l=l.tail())
  {
    aterm_appl a = ATAgetFirst(l);
    const atermpp::term_list <atermpp::aterm_appl> l(aterm_cast<aterm_list>(a(1)));
    a = gsMakeAction(aterm_cast<aterm_appl>(a(0)),ListFromFormat(info.m_rewriter.rewrite_internal_list(l,current_substitution)));
    m = push_front<aterm>(m, a);
  }
  m = reverse(m);

  return gsMakeMultAct(m);
}

void NextStateGenerator::SetTreeStateVars(const aterm &tree, aterm_list* vars)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextStateGenerator::SetTreeStateVars(tree, vars) called, with " << std::endl <<
            "  tree = " << atermpp::aterm(tree) << std::endl <<
            "  *vars = " << atermpp::aterm_list(*vars) << std::endl;
#endif

  if (tree.type_is_appl())
  {
    if (tree==info.nil)
    {
      return;
    }
    else if (tree.function()==info.pairAFun)
    {
      SetTreeStateVars(atermpp::aterm_cast<const atermpp::aterm_appl>(tree)(0),vars);
      SetTreeStateVars(atermpp::aterm_cast<const atermpp::aterm_appl>(tree)(1),vars);
      return;
    }
  }

  current_substitution[atermpp::aterm_cast<const variable>(vars->front())]=atermpp::aterm_cast<atermpp::aterm_appl>(tree);
  *vars = pop_front(*vars);
}

NextStateGenerator::NextStateGenerator(const aterm &State, ns_info& Info, size_t identifier, bool SingleSummand, size_t SingleSummandIndex) : info(Info)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextStateGenerator::NextStateGenerator(State, Info, identifier, SingleSummand) called, with " << std::endl <<
            "  State = " << atermpp::aterm(State) <<
            "  State (human readable): " << print_state(atermpp::aterm_appl(State), info) << std::endl <<
            "  identifier = " << identifier << std::endl <<
            "  SingleSummand = " << (SingleSummand?"true":"false") << std::endl <<
            "  SingleSummandIndex = " << SingleSummandIndex << std::endl;
#endif
  id = identifier;
  single_summand = SingleSummand;

  cur_state = aterm_appl();
  cur_act = aterm_appl();
  cur_nextstate = aterm_list(aterm());

  stateargs = std::vector <aterm>(info.statelen);

  reset(State, SingleSummandIndex);
}

NextStateGenerator::~NextStateGenerator()
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextStateGenerator::~NextStateGenerator called" << std::endl;
#endif
}

void NextStateGenerator::set_substitutions()
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextStateGenerator::set_substitutions called" << std::endl;
#endif

  atermpp::aterm_list l = info.procvars;
  switch (info.stateformat)
  {
    case GS_STATE_VECTOR:
      for (size_t i=0; !l.empty(); l=l.tail(),i++)
      {
        atermpp::aterm_appl a (atermpp::aterm_cast<const atermpp::aterm_appl>(cur_state)(i));
        if (a!=info.nil)
        {
          current_substitution[atermpp::aterm_cast<const variable>(l.front())]=a;
#ifdef MCRL2_NEXTSTATE_DEBUG
          mCRL2log(debug) << "Set substitution " << data::pp(info.m_rewriter.convert_from(l.front())) << ":=" <<
                    data::pp(info.m_rewriter.convert_from(a)) << "\n";
#endif
        }
      }

      break;
    case GS_STATE_TREE:
      SetTreeStateVars(cur_state,&l);
      break;
  }

  *info.current_id = id;
}

void NextStateGenerator::reset(const aterm &State, size_t SummandIndex)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextStateGenerator::reset(State, SummandIndex) called with:" << std::endl <<
            "  State = " << atermpp::aterm(State) << std::endl <<
            "  State (human readable) = " << print_state(atermpp::aterm_appl(State), info) << std::endl <<
            "  SummandIndex = " << SummandIndex << std::endl;
#endif
  // error = false;

  cur_state = State;

  set_substitutions();

  if (info.summands.size() == 0)
  {
    enumerated_variables=variable_list();
    valuations = info.get_sols(aterm_list(),info.m_rewriter.convert_to(mcrl2::data::sort_bool::false_()),current_substitution);
  }
  else
  {
    assert(SummandIndex < info.summands.size());
#ifdef MCRL2_NEXTSTATE_DEBUG
    std::clog << "Getting solutions for this summand" << std::endl <<
              "  Sum variables: " << atermpp::aterm(aterm_cast<aterm_list>(info.summands[SummandIndex](0))) << std::endl <<
              "                 " << core::pp_deprecated(atermpp::aterm(aterm_cast<aterm_list>(info.summands[SummandIndex](0)))) << std::endl <<
              "  Condition: " << atermpp::aterm(info.summands[SummandIndex](1)) << std::endl <<
              "             " << core::pp_deprecated(atermpp::aterm_appl(info.m_rewriter.convert_from(info.summands[SummandIndex](1)))) << std::endl;
#endif

    cur_act = atermpp::aterm_cast<atermpp::aterm_appl>(info.summands[SummandIndex](2));
    cur_nextstate = (aterm_list) info.summands[SummandIndex](3);

    enumerated_variables=atermpp::aterm_cast<const variable_list>(info.summands[SummandIndex](0));
    valuations = info.get_sols(aterm_cast<aterm_list>(info.summands[SummandIndex](0)),
                               info.summands[SummandIndex](1),
                               current_substitution);
  }

  sum_idx = SummandIndex + 1;
}

bool NextStateGenerator::next(mcrl2::lps::multi_action &Transition, aterm* State, bool* prioritised)
{
  using namespace atermpp;
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextStateGenerator::next(Transition, State, prioritised) called" << std::endl;
#endif
  while (valuations == ns_info::enumerator_type::iterator_internal() && (sum_idx < info.summands.size()))   // valuations is empty.
  {
    if (single_summand)
    {
#ifdef MCRL2_NEXTSTATE_DEBUG
      std::clog << "NextStateGenerator::next: single_summand is true, and no next state found in this summand;" << std::endl <<
                "  sum_idx was: " << sum_idx << std::endl;
#endif
      return false;
    }

    cur_act = atermpp::aterm_cast<atermpp::aterm_appl>(info.summands[sum_idx](2));
    cur_nextstate = atermpp::aterm_cast<const atermpp::aterm_list>(info.summands[sum_idx](3));

    if (*info.current_id != id)
    {
      set_substitutions();
    }

#ifdef MCRL2_NEXTSTATE_DEBUG
    std::clog << "Getting solutions for summand " << sum_idx << std::endl <<
              "  Sum variables: " << atermpp::aterm(aterm_cast<aterm_list>(info.summands[sum_idx](0))) << std::endl <<
              "                 " << core::pp_deprecated(atermpp::aterm(aterm_cast<aterm_list>(info.summands[sum_idx](0)))) << std::endl <<
              "  Condition: " << atermpp::aterm(info.summands[sum_idx](1)) << std::endl <<
              "             " << core::pp_deprecated(atermpp::aterm_appl(info.m_rewriter.convert_from(info.summands[sum_idx](1)))) << std::endl;
#endif

    enumerated_variables=atermpp::aterm_cast<const variable_list>(info.summands[sum_idx](0));
    valuations = info.get_sols(atermpp::aterm_cast<atermpp::aterm_list>(info.summands[sum_idx](0)),
                               info.summands[sum_idx](1),
                               current_substitution);

    ++sum_idx;
  }

  if (valuations != ns_info::enumerator_type::iterator_internal())   // valuations contains unprocessed valuations.
  {
    if (*info.current_id != id)
    {
      set_substitutions();
    }
    const atermpp::aterm_list &assignments= *valuations;
    assert(assignments.size()==enumerated_variables.size());
    variable_list::const_iterator j=enumerated_variables.begin();
    for(aterm_list::const_iterator a=assignments.begin() ; a!=assignments.end() ;  ++a, ++j)
    {
      // atermpp::aterm_appl t(assignments.front());
      current_substitution[*j]=aterm_cast<aterm_appl>(*a);
    }

    if (!valuations.solution_is_exact())
    {
      throw mcrl2::runtime_error("term does not evaluate to true or false " +
                 data::pp(info.m_rewriter.convert_from(info.m_rewriter.rewrite_internal(
                          atermpp::aterm_appl(info.summands[sum_idx-1](1)),current_substitution))));
    }

    Transition = mcrl2::lps::multi_action(rewrActionArgs(cur_act));
    *State = makeNewState(cur_state,cur_nextstate);

    if (prioritised != NULL)
    {
      *prioritised = (sum_idx <= info.num_prioritised);
    }

    for(variable_list::const_iterator j=enumerated_variables.begin();
              j!=enumerated_variables.end(); ++j)
    {
      current_substitution[*j]=atermpp::aterm_appl(*j);
    }
    ++valuations;

    return true;
  }
  else
  {
    Transition = mcrl2::lps::multi_action();
    *State = aterm_appl();
  }

  return false;
}

aterm NextStateGenerator::get_state() const
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "NextStateGenerator::get_state() called" << std::endl <<
            "  result: " << atermpp::aterm(cur_state) << std::endl <<
            "  result (human readable): " << print_state(atermpp::aterm_appl(cur_state), info) << std::endl;
#endif
  return cur_state;
}

NextState* createNextState(
  mcrl2::lps::specification const& spec,
  const mcrl2::data::rewriter &rewriter,
  bool allow_free_vars,
  int state_format,
  NextStateStrategy strategy
)
{
#ifdef MCRL2_NEXTSTATE_DEBUG
  std::clog << "createNextState(spec, e, allow_free_vars, state_format, strategy) called" << std::endl;
#endif
  switch (strategy)
  {
    default:
    case nsStandard:
      return new NextState(spec,allow_free_vars,state_format,rewriter);
  }

  return NULL;
}


} // namespace lps
} // namespace mcrl2
