// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bool.cpp
/// \brief Solver of parameterised boolean equation systems.
/// \details This file contains the routines for the pbes2bool
/// tool that can solve parameterised boolean equation systems
/// by translating them to boolean equation systems and then
/// solving them with an adapted Gauss elimination procedure.

// ======================================================================
//
// file          : pbes2bool
// date          : 15-04-2007
// version       : 0.1.3
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//                 Jan Friso Groote <J.F.Groote@tue.nl>
//
// ======================================================================

#include "boost.hpp" // precompiled headers

//C++
#include <ostream>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include <sstream>

//MCRL-specific
#include "mcrl2/core/messaging.h"

//LPS-Framework
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/new_data/utility.h"
#include "mcrl2/new_data/sort_expression.h"
#include "mcrl2/new_data/data_specification.h"
#include "mcrl2/new_data/replace.h"
#include "mcrl2/new_data/substitution.h"
#include "mcrl2/pbes/pbes2bool.h"
#include "mcrl2/pbes/data_elimination.h"

//ATERM-specific
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/atermpp/map.h"
#include "_aterm.h"

//Tool-specific
// #include "pbes_rewrite_jfg.h"
// #include "sort_functions.h"
#include "bes.h"

using namespace std;
using atermpp::make_substitution;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::new_data;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::pbes_expr;
using bes::bes_expression;

//Function declarations used by main program
//------------------------------------------
static void calculate_bes(pbes<> pbes_spec,
                          t_tool_options tool_options,
                          bes::equations &bes_equations,
                          atermpp::indexed_set &variable_index,
                          const rewriter &r);

//Post: tool_options.infilename contains a PBES ("" indicates stdin)
//Ret:  The BES generated from the PBES

pbes<> load_pbes(t_tool_options tool_options);
//Post: tool_options.infilename contains a PBES ("" indicates stdin)
//Ret: The pbes loaded from infile

static void save_bes_in_pbes_format(
                          const string &outfilename,
                          bes::equations &bes_equations, 
                          const pbes<> &p);
static void save_bes_in_cwi_format(const string &outfilename,bes::equations &bes_equations);
static void save_bes_in_vasy_format(const string &outfilename,bes::equations bes_equations);
//Post: pbes_spec is saved in cwi-format
//Ret: -

static void save_rhs_in_cwi_form(ostream &outputfile,
                                     bes_expression p,
                                     bes::equations &bes_equations);
static void save_rhs_in_vasy_form(ostream &outputfile,
                                     bes_expression p,
                                     vector<unsigned long> &variable_index,
                                     const unsigned long current_rank,
                                     bes::equations &bes_equations);
static bool is_pair(ATerm t);

// Function used to convert a pbes_expression to the variant used by the cwi-output

static void print_tree_rec(const char c,
                           ATerm t,
                           const bool opt_precompile_pbes,
                           const rewriter &r,
                           ostream &f)
{
  if (is_pair(t))
  { print_tree_rec(',',ATgetArgument(t,0),opt_precompile_pbes,r,f);
    print_tree_rec(',',ATgetArgument(t,1),opt_precompile_pbes,r,f);
  }
  else
  { if (opt_precompile_pbes)
    { assert(0); // Precompiling is not longer possible.
      // data_expression t1(r.fromRewriteFormat((ATerm)t));
      // f << c << core::pp(t1);
    }
    else
    { data_expression t1(t);
      f << c << core::pp(t1);
    }
  }
}

static void print_counter_example_rec(bes::variable_type current_var,
                                      std::string indent,
                                      bes::equations &bes_equations,
                                      atermpp::indexed_set &variable_index,
                                      vector<bool> &already_printed,
                                      bool opt_precompile_pbes,
                                      const rewriter &r,
                                      const bool opt_store_as_tree,
                                      ostream &f)
{
  if (opt_store_as_tree)
  { ATerm t=variable_index.get(current_var);
    if (!is_pair(t))
    { f << ATgetName(ATgetAFun(t));
    }
    else
    { f << ATgetName(ATgetAFun(ATgetArgument(t,0)));
      print_tree_rec('(',ATgetArgument(t,1),opt_precompile_pbes,r,f);
    }
  }
  else
  {
    propositional_variable_instantiation X(variable_index.get(current_var));

    data_expression_list tl=X.parameters();
    string s=X.name();
    f << s;
    for(data_expression_list::iterator t=tl.begin();
          t!=tl.end(); t++)
    { f << (t==tl.begin()?"(":",");
      if (opt_precompile_pbes)
      { assert(0); // Precompiling does not work anymore.
        // ATermAppl term=*t;
        // f << core::pp(rewriter->fromRewriteFormat((ATerm)term));
      }
      else
      { f << core::pp(*t);
      }
    }
    f << ")";
  }

  if (already_printed[current_var])
  { f << "*\n";
  }
  else
  { f << "\n";
    already_printed[current_var]=true;

    for(std::deque < bes::counter_example>::iterator walker=bes_equations.counter_example_begin(current_var);
        walker!=bes_equations.counter_example_end(current_var) ; walker++)
    {
      f << indent << (*walker).get_variable() << ": " << (*walker).print_reason() << "  " ;
      print_counter_example_rec((*walker).get_variable(),indent+"  ",
                            bes_equations,variable_index,already_printed,
                            opt_precompile_pbes,
                            r,
                            opt_store_as_tree,f);
    }
  }
}

static void print_counter_example(bes::equations &bes_equations,
                                  atermpp::indexed_set &variable_index,
                                  const bool opt_precompile_pbes,
                                  const rewriter &r,
                                  const bool opt_store_as_tree,
                                  const string filename)
{ ofstream f;
  vector <bool> already_printed(bes_equations.nr_of_variables()+1,false);
  if (filename.empty())
  { // Print the counterexample to cout.
    cout << "Below the justification for this outcome is listed\n1: ";
    print_counter_example_rec(2,"  ",bes_equations,variable_index,already_printed,
                       opt_precompile_pbes,r,opt_store_as_tree,cout);
  }
  if (f!=NULL)
  {
    try
    {
      ofstream f(filename.c_str());
      f << "Below the justification for this outcome is listed\n1: ";
      print_counter_example_rec(2,"  ",bes_equations,variable_index,already_printed,
                       opt_precompile_pbes,r,opt_store_as_tree,f);
      f.close();
    }
    catch (std::exception& e)
    { cerr << "Fail to write counterexample to " << filename <<
               "(" << e.what() << ")\n";

    }
  }
}

template <typename Container>
static void do_lazy_algorithm(pbes<Container> pbes_spec,
                              t_tool_options tool_options,
                              bes::equations &bes_equations,
                              atermpp::indexed_set &variable_index,
                              const rewriter &r);

static bool solve_bes(const t_tool_options &,
                      bes::equations &,
                      atermpp::indexed_set &);

// Create a propositional variable instantiation with the checks needed in the naive algorithm

void process(t_tool_options const& tool_options)
{
  //Load PBES
  pbes<> pbes_spec;
  pbes_spec.load(tool_options.infilename);

  if (!pbes_spec.is_well_typed())
  { gsErrorMsg("The pbes is not well typed\n");
    exit(1);
  }
  if (!pbes_spec.is_closed())
  { gsErrorMsg("The pbes contains free pbes variables \n");
    exit(1);
  }

  if (!pbes_spec.instantiate_free_variables())
  { std::cerr << "Fail to instantiate all free variables in the pbes.\n";
    std::cerr << "Remaining free variables are: ";
    for(atermpp::set <variable>::iterator i=pbes_spec.free_variables().begin() ;
        i!=pbes_spec.free_variables().end() ; i++ )
    { std::cerr << core::pp(*i) << " ";
    }
    std::cerr << "\n";
    exit(1);
  }

  pbes_spec.normalize();  // normalize the pbes, such that the equations do not contain negations
                          // and implications.

  //Throw away unused parts of data specification
  if (tool_options.opt_data_elm)
  { pbes_spec = remove_unused_data(pbes_spec);
  }

  //Process the pbes

  if (tool_options.opt_use_hashtables)
  { bes::use_hashtables();
  }
  atermpp::indexed_set variable_index(10000, 50);
  bes::equations bes_equations;

  data_specification data = pbes_spec.data();
  const rewriter r(data,tool_options.rewrite_strategy);

  calculate_bes(pbes_spec, tool_options,bes_equations,variable_index,r);
  if (!tool_options.opt_construct_counter_example)
  { variable_index.reset();
  }

  if (tool_options.opt_outputformat == "cwi")
  { // in CWI format only if the result is a BES, otherwise Binary
    save_bes_in_cwi_format(tool_options.outfilename,bes_equations);
  }
  else if (tool_options.opt_outputformat == "vasy")
  { //Save resulting bes if necessary.
    save_bes_in_vasy_format(tool_options.outfilename,bes_equations);
  }
  else if (tool_options.opt_outputformat == "pbes")
  { //Save resulting bes if necessary.
    save_bes_in_pbes_format(tool_options.outfilename,bes_equations,pbes_spec);
  }
  else
  {
    gsMessage("The solution for the initial variable of the pbes is %s\n", solve_bes(tool_options,bes_equations,variable_index) ? "true" : "false");

    if (tool_options.opt_construct_counter_example)
    { print_counter_example(bes_equations,
                            variable_index,
                            tool_options.opt_precompile_pbes,
                            r,
                            tool_options.opt_store_as_tree,
                            tool_options.opt_counter_example_file);
    }
  }
}

//function calculate_bes
//-------------------
void calculate_bes(pbes<> pbes_spec,
                   t_tool_options tool_options,
                   bes::equations &bes_equations,
                   atermpp::indexed_set &variable_index,
                   const rewriter &r)
{
  if (!pbes_spec.is_well_typed())
  {
    gsErrorMsg("The PBES is not well formed. Abort computation. \n");
    exit(1);
  }
  if (!pbes_spec.is_closed())
  {
    gsErrorMsg("The PBES contains free variables. Computation aborted.\n");
    exit(1);
  }

  do_lazy_algorithm(pbes_spec, tool_options,bes_equations,variable_index,r);
  //return new pbes
  return;
}

/* Declare a protected PAIR symbol */
inline AFun initAFunPair(AFun& f)
{ f = ATmakeAFun("PAIR", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline AFun PAIR()
{ static AFun PAIR = initAFunPair(PAIR);
  return PAIR;
}

static ATermAppl apply_pair_symbol(ATermAppl t1, ATermAppl t2)
{
  return ATmakeAppl2(PAIR(),(ATerm)t1,(ATerm)t2);
}

static bool is_pair(ATerm t)
{ return ATgetAFun(t)==PAIR();
}

static unsigned int largest_power_of_2_smaller_than(int i)
{ unsigned int j=1;
  i=i>>1;
  while (i>0)
  { i=i>>1;
    j=j*2;
  };
  return j;
}


// This function sets the variables in substitution sigma given that the terms are stored
// in a tree structure.
static void assign_variables_in_tree(
                      ATerm t,
                      variable_list::iterator &var_iter,
                      new_data::mutable_substitution<new_data::variable, new_data::data_expression> &sigma, 
                      // const rewriter &r,
                      const bool opt_precompile_pbes)
{ if (is_pair(t))
  { assign_variables_in_tree(ATgetArgument(t,0),var_iter,sigma,opt_precompile_pbes);
    assign_variables_in_tree(ATgetArgument(t,1),var_iter,sigma,opt_precompile_pbes);
  }
  else
  {
    if (opt_precompile_pbes)
    { assert(0); // Precompiling does not work
      // rewriter->setSubstitutionInternal(*var_iter,t);
    }
    else
    { // r->setSubstitution(*var_iter,(ATermAppl)t);
      sigma[*var_iter]=new_data::data_expression(t);
    }
    var_iter++;
  }
}


// static ATermAppl store_as_tree(pbes_expression p)
static ATermAppl store_as_tree(propositional_variable_instantiation p)
/* We assume p is a propositional_variable_instantiation of the form B(x1,...,xn). If p has less than 3
 * arguments p is returned. Otherwise a term of the form B(pair(pair(...pair(x1,x2),...)))) is
 * returned, which is a balanced tree flushed to the right. For each input the resulting
 * tree is unique.
 */
{
  data_expression_list args=p.parameters();

  if ( args.size() ==0 )
  return p.name();

  unsigned int n=largest_power_of_2_smaller_than(args.size());

  atermpp::vector<ATermAppl> tree_store(n);

  /* put the arguments in the intermediate tree_store. The last elements are stored as
   * pairs, such that the args.size() elements are stored in n positions. */
  unsigned int i=0;
  for(data_expression_list::const_iterator t=args.begin() ; t!=args.end(); t++)
  { if (i<2*n-args.size())
    { tree_store[i]= (*t);
      i++;
    }
    else
    { ATermAppl t1(*t);
      t++;
      ATermAppl t2(*t);
      tree_store[i]= apply_pair_symbol(t1,t2);
      i++;
    }
  }

  while (n>1)
  { n=n>>1; // n=n/2;
    for (unsigned int i=0; i<n; i++)
    {
      tree_store[i] = apply_pair_symbol(tree_store[2*i],tree_store[2*i+1]);
    }
  }
  return apply_pair_symbol(p.name(),(ATermAppl)tree_store[0]);
}

//function add_propositional_variable_instantiations_to_indexed_set
//and translate to pbes expression to a bes_expression in BDD format.

static bes::bes_expression add_propositional_variable_instantiations_to_indexed_set_and_translate(
                   const pbes_expression p,
                   atermpp::indexed_set &variable_index,
                   unsigned long &nr_of_generated_variables,
                   const bool to_bdd,
                   const transformation_strategy strategy,
                   const bool construct_counter_example,
                   bes::equations  &bes_equations,
                   const bes::variable_type current_variable,
                   const bool opt_store_as_tree,
                   const bool opt_precompile_pbes,
                   const rewriter &r)
{
  if (is_propositional_variable_instantiation(p))
  {
    pair<unsigned long,bool> pr=variable_index.put((opt_store_as_tree)?pbes_expression(store_as_tree(p)):p);

    if (pr.second) /* p is added to the indexed set, so it is a new variable */
    { nr_of_generated_variables++;
      if (to_bdd)
      { return bes::if_(bes::variable(pr.first),bes::true_(),bes::false_());
      }
      else
      { return bes::variable(pr.first);
      }
    }
    else
    {
      if (strategy>::lazy)
      { bes_expression b=bes_equations.get_rhs(pr.first);
        if (bes::is_true(b) )
        {
          if (construct_counter_example)
          { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(pr.first,bes::FORWARD_SUBSTITUTION_TRUE));
          }
          return b;
        }
        if (bes::is_false(b))
        {
          if (construct_counter_example)
          { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(pr.first,bes::FORWARD_SUBSTITUTION_FALSE));
          }
          return b;
        }
      }
      if (to_bdd)
      { return bes::if_(bes::variable(pr.first),bes::true_(),bes::false_());
      }
      else
      { return bes::variable(pr.first);
      }
    }
  }
  else if (pbes_expr::is_and(p))
  { bes::bes_expression b1=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::left(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,bes_equations,current_variable,opt_store_as_tree,
                            opt_precompile_pbes,r);
    if (is_false(b1))
    { return b1;
    }
    bes::bes_expression b2=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::right(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,bes_equations,current_variable,opt_store_as_tree,
                            opt_precompile_pbes,r);
    if (is_false(b2))
    { return b2;
    }
    if (is_true(b1))
    { return b2;
    }
    if (is_true(b2))
    { return b1;
    }
    if (to_bdd)
    { return BDDif(b1,b2,bes::false_());
    }
    else
    { return and_(b1,b2);
    }
  }
  else if (pbes_expr::is_or(p))
  {
    bes::bes_expression b1=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::left(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,bes_equations,current_variable,opt_store_as_tree,
                            opt_precompile_pbes,r);
    if (bes::is_true(b1))
    { return b1;
    }

    bes::bes_expression b2=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::right(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,bes_equations,current_variable,opt_store_as_tree,
                            opt_precompile_pbes,r);
    if (bes::is_true(b2))
    { return b2;
    }
    if (bes::is_false(b1))
    { return b2;
    }
    if (bes::is_false(b2))
    { return b1;
    }

    if (to_bdd)
    { return BDDif(b1,bes::true_(),b2);
    }
    else
    { return or_(b1,b2);
    }
  }
  else if (pbes_expr::is_true(p))
  { return bes::true_();
  }
  else if (pbes_expr::is_false(p))
  { return bes::false_();
  }

  if (opt_precompile_pbes)
  { assert(0); // Precompilation is not longer possible.
    // cerr << "Unexpected expression. Most likely because expression fails to rewrite to true or false: " <<
    //                  core::pp(rewriter->fromRewriteFormat((ATerm)(ATermAppl)p)) << "\n";
  }
  else
  { cerr << "Unexpected expression. Most likely because expression fails to rewrite to true or false: " << core::pp(p) << "\n";
    abort();
  }
  exit(1);
  return bes::false_();
}

//function do_lazy_algorithm
//------------------------------

// Global variables
//  atermpp::indexed_set variable_index(10000, 50);
//  bes::equations bes_equations;

template <typename Container>
static void do_lazy_algorithm(pbes<Container> pbes_spec,
                              t_tool_options tool_options,
                              bes::equations &bes_equations,
                              atermpp::indexed_set &variable_index,
                              const rewriter &r)
{

  // Verbose msg: doing naive algorithm

  data_specification data = pbes_spec.data();

  // Variables in which the result is stored
  propositional_variable_instantiation new_initial_state;

  // atermpp::indexed_set variable_index(10000, 50);
  // In order to generate a counterexample, this must also be known outside
  // this procedure.

  variable_index.put(bes::true_());
  variable_index.put(bes::false_()); /* Put first two dummy terms that
                                       gets index 0 and 1 in the indexed set, to
                                       take care that the first variable gets an index 2, to
                                       make space for a first equation of the shape X1=X2. */

  /* The following list contains that variables that need to be explored.
     This list is only relevant if tool_options.opt_strategy>=on_the_fly,
     as in the other case the variables to be investigated are those
     with indices between nre_of_processed_variables and nr_of_generated
     variables. */

  deque < bes::variable_type> todo;
  if (tool_options.opt_strategy>=on_the_fly)
  { todo.push_front(2);
  }
  // Data rewriter
// replaced to make the code compile again
  pbes_expression p=pbes_spec.initial_state();
//  pbes_expression p=pbes_expression_rewrite_and_simplify(pbes_spec.initial_state(),
//                     r,
//                     tool_options.opt_precompile_pbes);
  variable_index.put((tool_options.opt_store_as_tree)?pbes_expression(store_as_tree(p)):p);

  if (tool_options.opt_strategy>=on_the_fly)
  { bes_equations.store_variable_occurrences();
    bes_equations.count_variable_relevance_on();
  }

  if (tool_options.opt_construct_counter_example)
  { bes_equations.construct_counter_example_on();
  }

  // Needed hashtables
  Container eqsys = pbes_spec.equations();
  atermpp::table pbes_equations(2*eqsys.size(), 50);   // (propvarname, pbes_equation)

  // Vector with the order of the variable names used for sorting the result

  atermpp::table variable_rank(2*eqsys.size(),50);

  // Fill the pbes_equations table
  gsVerboseMsg("Retrieving pbes_equations from equation system...\n");

  assert(eqsys.size()>0); // There should be at least one equation
  fixpoint_symbol current_fixpoint_symbol=eqsys.begin()->symbol();

  unsigned long rank=1;

  for (typename Container::iterator eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
  {
    pbes_equations.put(
            eqi->variable().name(),
            pbes_equation(
                eqi->symbol(),
                eqi->variable(),
// replaced to make the code compile again
                eqi->formula()));
//                pbes_expression_rewrite_and_simplify(
//                              eqi->formula(),
//                r,
//                tool_options.opt_precompile_pbes)));
    if (eqi->symbol()!=current_fixpoint_symbol)
    { current_fixpoint_symbol=eqi->symbol();
      rank=rank+1;
    }
    variable_rank.put(eqi->variable().name(),atermpp::aterm_int(rank));
  }

  unsigned long relevance_counter=0;
  unsigned long relevance_counter_limit=100;
  #define RELEVANCE_DIVIDE_FACTOR 100

  gsVerboseMsg("Computing a BES from the PBES....\n");

  // Set the first BES equation X1=X2
  bes_equations.add_equation(
                1,
                eqsys.begin()->symbol(),
                1,
                bes::variable(2));

  // Variables used in whole function
  unsigned long nr_of_processed_variables = 1;
  unsigned long nr_of_generated_variables = 2;

  // As long as there are states to be explored
  while ((tool_options.opt_strategy>=on_the_fly)
             ?todo.size()>0
             :(nr_of_processed_variables < nr_of_generated_variables))
  {
    bes::variable_type variable_to_be_processed;
    if (tool_options.opt_strategy>=on_the_fly)
    { variable_to_be_processed=todo.front();
      todo.pop_front();
    }
    else
    { variable_to_be_processed=nr_of_processed_variables+1;
    }

    if (bes_equations.is_relevant(variable_to_be_processed))
           // If v is not relevant, it does not need to be investigated.
    {

      pbes_equation current_pbeq;
      new_data::mutable_substitution<new_data::variable, new_data::data_expression> sigma;

      // Add the required substitutions
      if (tool_options.opt_store_as_tree)
      {  // The current varable instantiation is stored as a tree, and this tree must be unfolded.
         ATerm t=variable_index.get(variable_to_be_processed);
         if (!is_pair(t))
         { // Then t is the name of the current_variable_instantiation, and it has
           // no arguments.

           current_pbeq = pbes_equation(pbes_equations.get(t));
           assert(current_pbeq.variable().parameters().size()==0);
         }
         else
         { // t is a pair, with a name as its left hand side.
           current_pbeq = pbes_equation(pbes_equations.get(ATgetArgument(t,0)));
           // the right hand side of t are the parameters, in a tree structure.

           t=ATgetArgument(t,1);
           variable_list::iterator iter=current_pbeq.variable().parameters().begin();
           assign_variables_in_tree(t,iter,
                                    sigma,tool_options.opt_precompile_pbes);
         }

      }
      else // The current variable instantiation is a propositional_variable_instantiation
      {
        propositional_variable_instantiation current_variable_instantiation =
            propositional_variable_instantiation(variable_index.get(variable_to_be_processed));

        current_pbeq = pbes_equation(pbes_equations.get(current_variable_instantiation.name()));
        assert(current_pbeq!=pbes_equation());  // If this fails, a pbes variable is used in
                                                // a right hand side, and not in the left hand side
                                                // of an equation.

        data_expression_list::iterator elist=current_variable_instantiation.parameters().begin();

        for(variable_list::iterator vlist=current_pbeq.variable().parameters().begin() ;
               vlist!=current_pbeq.variable().parameters().end() ; vlist++)
        {
          assert(elist!=current_variable_instantiation.parameters().end());

          if (tool_options.opt_precompile_pbes)
          { assert(0); // precompilation is no longer possible.
            // rewriter->setSubstitutionInternal(*vlist,(atermpp::aterm)*elist);
          }
          else
          {
            // r->setSubstitution(*vlist,*elist);
            sigma[*vlist]=*elist;
          }
          elist++;
        }
        assert(elist==current_variable_instantiation.parameters().end());
      }

      pbes_expression new_pbes_expression = pbes_expression_substitute_and_rewrite(
                                current_pbeq.formula(), data, r,tool_options.opt_precompile_pbes,sigma);
      bes::bes_expression new_bes_expression=
           add_propositional_variable_instantiations_to_indexed_set_and_translate(
                        new_pbes_expression,
                        variable_index,
                        nr_of_generated_variables,
                        tool_options.opt_use_hashtables,
                        tool_options.opt_strategy,
                        tool_options.opt_construct_counter_example,
                        bes_equations,
                        variable_to_be_processed,
                        tool_options.opt_store_as_tree,
                        tool_options.opt_precompile_pbes,
                        r);

      /* No need to clear up sigma, as it was locally declared.
      for(variable_list::iterator vlist=current_pbeq.variable().parameters().begin() ;
               vlist!=current_pbeq.variable().parameters().end() ; vlist++)
      { r->clearSubstitution(*vlist);
      } */


      if (tool_options.opt_strategy>=on_the_fly_with_fixed_points)
      { // find a variable in the new_bes_expression from which `variable' to be
        // processed is reachable. If so, new_bes_expression can be set to
        // true or false.

        if (current_pbeq.symbol()==fixpoint_symbol::mu())
        {
          if (bes_equations.find_mu_loop(
                               new_bes_expression,
                               variable_to_be_processed,
                               atermpp::aterm_int(variable_rank.get(current_pbeq.variable().name())).value()))
          { new_bes_expression=bes::false_();
            if (tool_options.opt_construct_counter_example)
            { bes_equations.counter_example_queue(variable_to_be_processed).
                   push_back(bes::counter_example(variable_to_be_processed,bes::MU_CYCLE));
            }
          }
        }
        else
        {
          if (bes_equations.find_nu_loop(
                               new_bes_expression,
                               variable_to_be_processed,
                               atermpp::aterm_int(variable_rank.get(current_pbeq.variable().name())).value()))
          { new_bes_expression=bes::true_();
            if (tool_options.opt_construct_counter_example)
            { bes_equations.counter_example_queue(variable_to_be_processed).
                   push_back(bes::counter_example(variable_to_be_processed,bes::NU_CYCLE));
            }
          }
        }
      }

      if ((tool_options.opt_strategy>=on_the_fly))
      {
        bes_equations.add_equation(
                variable_to_be_processed,
                current_pbeq.symbol(),
                atermpp::aterm_int(variable_rank.get(current_pbeq.variable().name())).value(),
                new_bes_expression,
                todo);

        /* So now and then (after doing as many operations on the size of bes_equations,
           the relevances of variables must be reset, to avoid investigating irrelevant
           variables. There is an invariant in the system that all variables reachable
           from the initial variable 1, are always relevant. Furthermore, relevant
           variables that need to be investigated are always in the todo list */
        relevance_counter++;
        if (relevance_counter>=relevance_counter_limit)
        { relevance_counter_limit=bes_equations.nr_of_variables()/RELEVANCE_DIVIDE_FACTOR;
          relevance_counter=0;
          bes_equations.refresh_relevances(todo);
        }
      }
      else
      { bes_equations.add_equation(
                variable_to_be_processed,
                current_pbeq.symbol(),
                atermpp::aterm_int(variable_rank.get(current_pbeq.variable().name())).value(),
                new_bes_expression);
      }

      if (tool_options.opt_strategy>=on_the_fly)
      {
        if (bes::is_true(new_bes_expression)||bes::is_false(new_bes_expression))
        {
          // new_bes_expression is true or false and opt_strategy is on the fly or higher.
          // This means we must optimize the bes by substituting true/false for this variable
          // everywhere. For this we use the occurrence set.

          set <bes::variable_type> to_set_to_true_or_false;
          to_set_to_true_or_false.insert(variable_to_be_processed);
          for( ; !to_set_to_true_or_false.empty() ; )
          {
            bes::variable_type w=(*to_set_to_true_or_false.begin());

            // Take the lowest element for substitution, to generate
            // short counterexample.

            // gsVerboseMsg("------------------ %d\n",(unsigned long)w);
            to_set_to_true_or_false.erase(w);
            for( set <bes::variable_type>::iterator
                      v=bes_equations.variable_occurrence_set_begin(w);
                      v!=bes_equations.variable_occurrence_set_end(w);
                      v++)
            {

              bes_expression b=bes_equations.get_rhs(*v);
              if (tool_options.opt_construct_counter_example)
              { b=substitute_true_false(b,w,bes_equations.get_rhs(w),
                                         bes_equations.counter_example_queue(*v));
              }
              else
              { b=substitute_true_false(b,w,bes_equations.get_rhs(w));
              }

              if (bes::is_true(b)||bes::is_false(b))
              { to_set_to_true_or_false.insert(*v);
              }
              relevance_counter++;
              bes_equations.set_rhs(*v,b,w);
            }
            bes_equations.clear_variable_occurrence_set(w);
          }
        }
      }
    }
    nr_of_processed_variables++;
    if (nr_of_processed_variables % 1000 == 0)
    {
      gsVerboseMsg("Processed %d and generated %d boolean variables\n",
                                   nr_of_processed_variables,nr_of_generated_variables);
    }
  }
  bes_equations.refresh_relevances();
}

/* substitute boolean equation expression */
static bes_expression substitute_rank(
                bes_expression b,
                const unsigned long current_rank,
                const atermpp::vector<bes_expression> &approximation,
                bes::equations &bes_equations,
                const bool use_hashtable,
                atermpp::table &hashtable,
                bool store_counter_example=false,
                bes::variable_type current_variable=0)
{ /* substitute variables with rank larger and equal
     than current_rank with their approximations. */

  if (bes::is_true(b)||bes::is_false(b)||bes::is_dummy(b))
  { return b;
  }

  bes_expression result;
  if (use_hashtable)
  { result=hashtable.get(b);
    if (result!=NULL)
    { return result;
    }
  }

  if (is_variable(b))
  { bes::variable_type v=bes::get_variable(b);
    if (bes_equations.get_rank(v)==current_rank)
    {
      result=approximation[v];
      if (store_counter_example)
      { if (bes::is_true(result))
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION_TRUE));
        }
        else if (bes::is_false(result))
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION_FALSE));
        }
        else
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION));
        }
      }
    }
    else
    {
      result=b;
    }
  }

  else if (is_and(b))
  { bes_expression b1=substitute_rank(lhs(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
    if (is_false(b1))
    { result=b1;
    }
    else
    { bes_expression b2=substitute_rank(rhs(b),current_rank,approximation,bes_equations,
                                          use_hashtable,hashtable,store_counter_example,current_variable);
      if (is_false(b2))
      { result=b2;
      }
      else if (is_true(b1))
      { result=b2;
      }
      else if (is_true(b2))
      { result=b1;
      }
      else
      { result=and_(b1,b2);
      }
    }
  }
  else if (is_or(b))
  { bes_expression b1=substitute_rank(lhs(b),current_rank,approximation,bes_equations,use_hashtable,
                                          hashtable,store_counter_example,current_variable);
    if (is_true(b1))
    { result=b1;
    }
    else
    { bes_expression b2=substitute_rank(rhs(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
      if (is_true(b2))
      { result=b2;
      }
      else if (is_false(b1))
      { result=b2;
      }
      else if (is_false(b2))
      { result=b1;
      }
      else
      { result=or_(b1,b2);
      }
    }
  }


  else if (is_if(b))
  {
    bes::variable_type v=bes::get_variable(condition(b));
    if (bes_equations.get_rank(v)==current_rank)
    {
      if (bes::is_true(approximation[v]))
      { result=substitute_rank(then_branch(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        if (store_counter_example)
        { bes_equations.counter_example_queue(current_variable).
                     push_front(bes::counter_example(v,bes::APPROXIMATION_TRUE));
        }
      }
      else if (bes::is_false(approximation[v]))
      { result=substitute_rank(else_branch(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        if (store_counter_example)
        { bes_equations.counter_example_queue(current_variable).
                     push_front(bes::counter_example(v,bes::APPROXIMATION_FALSE));
        }
      }
      else
      { bes_expression b1=substitute_rank(then_branch(b),current_rank,approximation,
                                          bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        bes_expression b2=substitute_rank(else_branch(b),current_rank,approximation,
                                          bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        result=BDDif(approximation[v],b1,b2);
      }
    }
    else
    { /* the condition is not equal to v */
      bes_expression b1=substitute_rank(then_branch(b),current_rank,approximation,
                                        bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
      bes_expression b2=substitute_rank(else_branch(b),current_rank,approximation,
                                        bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
      if ((b1==then_branch(b)) && (b2==else_branch(b)))
      { result=b;
      }
      else
      { result=BDDif(bes::if_(condition(b),bes::true_(),bes::false_()),b1,b2);
      }
    }
  }
  else
  {
    assert(0);  // expect an if, variable, and, or, or a true or a false here.
  }

  if (use_hashtable)
  { hashtable.put(b,result);
  }
  return result;
}

/* substitute boolean equation expression */

static bes_expression evaluate_bex(
                bes_expression b,
                const atermpp::vector<bes_expression> &approximation,
                const unsigned long rank,
                bes::equations &bes_equations,
                const bool use_hashtable,
                atermpp::table &hashtable,
                const bool construct_counter_example,
                const bes::variable_type current_variable)
{ /* substitute the approximation for variables in b, given
     by approximation, for all those variables that have a
     rank higher or equal to the variable rank; */

  if (bes::is_true(b)||bes::is_false(b))
  { return b;
  }

  bes_expression result;
  if (use_hashtable)
  { result=hashtable.get(b);
    if (result!=NULL)
    return result;
  }

  if (is_variable(b))
  { const bes::variable_type v=bes::get_variable(b);
    if (bes_equations.get_rank(v)>=rank)
    {
      result=approximation[v];
      if (construct_counter_example)
      { if (bes::is_true(result))
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION_TRUE));
        }
        else if (bes::is_false(result))
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION_FALSE));
        }
        else
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION));
        }
      }
    }
    else
    { /* the condition has lower rank than the variable rank,
         leave it untouched */
      result=b;
    }
  }
  else if (is_and(b))
  { bes_expression b1=evaluate_bex(lhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
    if (is_false(b1))
    { result=b1;
    }
    else
    { bes_expression b2=evaluate_bex(rhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      if (is_false(b2))
      { result=b2;
      }
      else if (is_true(b1))
      { result=b2;
      }
      else if (is_true(b2))
      { result=b1;
      }
      else
      { result=and_(b1,b2);
      }
    }
  }
  else if (is_or(b))
  { bes_expression b1=evaluate_bex(lhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
    if (is_true(b1))
    { result=b1;
    }
    else
    { bes_expression b2=evaluate_bex(rhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      if (is_true(b2))
      { result=b2;
      }
      else if (is_false(b1))
      { result=b2;
      }
      else if (is_false(b2))
      { result=b1;
      }
      else
      { result=or_(b1,b2);
      }
    }
  }
  else if (is_if(b))
  {
    bes::variable_type v=bes::get_variable(condition(b));
    if (bes_equations.get_rank(v)>=rank)
    {
      bes_expression b1=evaluate_bex(then_branch(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      bes_expression b2=evaluate_bex(else_branch(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      result=BDDif(approximation[v],b1,b2);
    }
    else
    { /* the condition has lower rank than the variable rank,
         leave it untouched */
      bes_expression b1=evaluate_bex(then_branch(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      bes_expression b2=evaluate_bex(else_branch(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      if ((b1==then_branch(b)) && (b2==else_branch(b)))
      { result=b;
      }
      else
      { result=BDDif(bes::if_(condition(b),bes::true_(),bes::false_()),b1,b2);
      }
    }
  }
  else
  { assert(0); //expect an if, true or false, and, variable or or;
  }

  if (use_hashtable)
  { hashtable.put(b,result);
  }
  return result;
}

bool solve_bes(const t_tool_options &tool_options,
               bes::equations &bes_equations,
               atermpp::indexed_set &variable_index)
{
  gsVerboseMsg("Solving the BES with %d equations.\n",bes_equations.nr_of_variables());
  atermpp::vector<bes_expression> approximation(bes_equations.nr_of_variables()+1);

  atermpp::table bex_hashtable(10,5);

  /* Set the approximation to its initial value */
  for(bes::variable_type v=bes_equations.nr_of_variables(); v>0; v--)
  {
    bes_expression b=bes_equations.get_rhs(v);
    if (b!=bes::dummy())
    { if (bes::is_true(bes_equations.get_rhs(v)))
      { approximation[v]=bes::true_();
      }
      else if (bes::is_false(bes_equations.get_rhs(v)))
      { approximation[v]=bes::false_();
      }
      else if (bes_equations.get_fixpoint_symbol(v)==fixpoint_symbol::mu())
      { approximation[v]=bes::false_();
      }
      else
      { approximation[v]=bes::true_();
      }
    }
  }

  bes_equations.store_variable_occurrences();

  for(unsigned long current_rank=bes_equations.max_rank;
      current_rank>0 ; current_rank--)
  {
    if (gsVerbose)
    { std::cerr << "Solve equations of rank " << current_rank << ".\n";
    }

    /* Calculate the stable solution for the current rank */

    set <bes::variable_type> todo;

    for(bes::variable_type v=bes_equations.nr_of_variables(); v>0; v--)
    {
      if (bes_equations.is_relevant(v) && (bes_equations.get_rank(v)==current_rank))
      {
        // std::cerr << "Evaluate variable" << v << "\n";
        bes_expression t=evaluate_bex(
                             bes_equations.get_rhs(v),
                             approximation,
                             current_rank,
                             bes_equations,
                             tool_options.opt_use_hashtables,
                             bex_hashtable,
                             false,
                             v);

        if (toBDD(t)!=toBDD(approximation[v]))
        {
          if (tool_options.opt_use_hashtables)
          { bex_hashtable.reset();  /* we change the approximation, so the
                                       hashtable becomes invalid */
          }
          if (tool_options.opt_construct_counter_example)
          { if (tool_options.opt_use_hashtables)
            { bex_hashtable.reset();  /* We want to construct a counter example, which is
                                         not traceable when results in the hashtable are used */
            }
            approximation[v]=evaluate_bex(
                             bes_equations.get_rhs(v),
                             approximation,
                             current_rank,
                             bes_equations,
                             tool_options.opt_use_hashtables,
                             bex_hashtable,
                             true,
                             v);
          }
          else
          { approximation[v]=t;
          }
          todo.insert(v);
        }
      }
    }

    for( ; todo.size()>0 ; )
    { set<bes::variable_type>::iterator w= todo.begin();
      bes::variable_type w_value=*w;
      todo.erase(w);

      for(set <bes::variable_type>::iterator
         u=bes_equations.variable_occurrence_set_begin(w_value);
         u!=bes_equations.variable_occurrence_set_end(w_value);
         u++)
      {
        if (bes_equations.is_relevant(*u) && (bes_equations.get_rank(*u)==current_rank))
        { bes_expression t=evaluate_bex(
                              bes_equations.get_rhs(*u),
                              approximation,
                              current_rank,
                              bes_equations,
                              tool_options.opt_use_hashtables,
                              bex_hashtable,
                              false,
                              *u);

          if (toBDD(t)!=toBDD(approximation[*u]))
          {
            if (tool_options.opt_use_hashtables)
            { bex_hashtable.reset();  /* we change approximation, so the
                                         hashtable becomes invalid */
            }
            if (tool_options.opt_construct_counter_example)
            { if (tool_options.opt_use_hashtables)
              { bex_hashtable.reset();  /* We want to construct a counter example, which is
                                           not traceable when results in the hashtable are used */
              }
              approximation[*u]=evaluate_bex(
                                   bes_equations.get_rhs(*u),
                                   approximation,
                                   current_rank,
                                   bes_equations,
                                   tool_options.opt_use_hashtables,
                                   bex_hashtable,
                                   true,
                                   *u);
            }
            else
            { approximation[*u]=t;
            }
            todo.insert(*u);
          }
        }
      }
    }

    /* substitute the stable solution for the current rank in all other
       equations. */


    if (tool_options.opt_use_hashtables)
    { bex_hashtable.reset();
    }

    for(bes::variable_type v=bes_equations.nr_of_variables(); v>0; v--)
    { if (bes_equations.is_relevant(v))
      {
        // std::cerr << "Substitute values in lower rank" << v << "\n";
        if (bes_equations.get_rank(v)==current_rank)
        {
          if (tool_options.opt_construct_counter_example)
          { bes_equations.set_rhs(
                         v,
                         substitute_rank(
                                   bes_equations.get_rhs(v),
                                   current_rank,
                                   approximation,
                                   bes_equations,
                                   tool_options.opt_use_hashtables,
                                   bex_hashtable,
                                   true,v));
          }
          else
          { bes_equations.set_rhs(v,approximation[v]);
          }
        }
        else
        { bes_equations.set_rhs(
                         v,
                         substitute_rank(
                                   bes_equations.get_rhs(v),
                                   current_rank,
                                   approximation,
                                   bes_equations,
                                   tool_options.opt_use_hashtables,
                                   bex_hashtable,
                                   tool_options.opt_construct_counter_example,v));
        }
      }
    }
    if (tool_options.opt_use_hashtables)
    { bex_hashtable.reset();
    }

  }
  assert(bes::is_true(approximation[1])||
         bes::is_false(approximation[1]));
  return bes::is_true(approximation[1]);  /* 1 is the index of the initial variable */
}

//function save_bes_in_vasy_format
//--------------------------------

typedef enum { both, and_form, or_form} expression_sort;

static bes_expression translate_equation_for_vasy(const unsigned long i,
                                        const bes_expression b,
                                        const expression_sort s,
                                        bes::equations &bes_equations)
{
  if (bes::is_true(b))
  { return b;
  }
  else if (bes::is_false(b))
  { return b;
  }
  else if (bes::is_and(b))
  {
    if (s==or_form)
    { /* make a new equation B=b, and return B */
      bes::variable_type v=bes_equations.nr_of_variables()+1;
      bes_equations.add_equation(v,
                                 bes_equations.get_fixpoint_symbol(i),
                                 bes_equations.get_rank(i),
                                 b);
      return bes::variable(v);
    }
    else
    {
      bes_expression b1=translate_equation_for_vasy(i,lhs(b),and_form,bes_equations);
      bes_expression b2=translate_equation_for_vasy(i,rhs(b),and_form,bes_equations);
      return and_(b1,b2);
    }
  }
  else if (bes::is_or(b))
  {
    if (s==and_form)
    { /* make a new equation B=b, and return B */
      bes::variable_type v=bes_equations.nr_of_variables()+1;
      bes_equations.add_equation(v,
                                 bes_equations.get_fixpoint_symbol(i),
                                 bes_equations.get_rank(i),
                                 b);
      return bes::variable(v);
    }
    else
    {
      bes_expression b1=translate_equation_for_vasy(i,lhs(b),or_form,bes_equations);
      bes_expression b2=translate_equation_for_vasy(i,rhs(b),or_form,bes_equations);
      return or_(b1,b2);
    }
  }
  else if (bes::is_variable(b))
  {
    return b;
  }
  else if (bes::is_if(b))
  { //BESIF(x,y,z) is equivalent to (y & (x|z)) provided the expression is monotonic.
    return translate_equation_for_vasy(i,
                    and_optimized(then_branch(b),
                         or_optimized(condition(b),else_branch(b))),s,bes_equations);

    //BESIF(x,y,z) => (x & y) | (!x&z)
    //If y is true, this reduces to (x | z)
    //If z is false, this reduces to (x & y)
    //Otherwise, the result is not monotonic,
    //which ought not to be possible.
    /* const bes_expression y=then_branch(b);
    const bes_expression z=else_branch(b);
    if (is_true(y))
    { if (is_false(z))
      { return translate_equation_for_vasy(i,condition(b),s,bes_equations);
      }
      else
      { return translate_equation_for_vasy(i,or_(condition(b),z),s,bes_equations);
      }
    }
    else if (is_false(z))
    { // not is_true(y)
      { return translate_equation_for_vasy(i,and_(condition(b),y),s,bes_equations);
      }
    }
    else
    { gsErrorMsg("The generated equation system is not a monotonic BES. It cannot be saved in VASY-format.\n");
      exit(1);
    } */
  }
  else
  {
    gsErrorMsg("The generated equation system is not a BES. It cannot be saved in VASY-format.\n");
    exit(1);
  }
  return b;
}


static void save_bes_in_vasy_format(const string &outfilename,bes::equations bes_equations)
{
  gsVerboseMsg("Converting result to VASY-format...\n");
  // Use an indexed set to keep track of the variables and their vasy-representations

  /* First translate the right hand sides of the equations such that they only
     contain only conjunctions of disjunctions. Note that dynamically new
     equations are added during the translation process in "translate_equation_for_vasy"
     that must also be translated. */

  for(unsigned long i=1; i<=bes_equations.nr_of_variables() ; i++)
  {
    bes_equations.set_rhs(i,translate_equation_for_vasy(i,bes_equations.get_rhs(i),both,bes_equations));
  }

  /* Second give a consecutive index to each variable of a particular rank */

  std::vector<unsigned long> variable_index(bes_equations.nr_of_variables()+1);
  for(unsigned long r=1 ; r<=bes_equations.max_rank ; r++)
  { unsigned long index=0;
    for(unsigned long i=1; i<=bes_equations.nr_of_variables() ; i++)
    { if (bes_equations.get_rank(i)==r)
      {
        variable_index[i]=index;
        index++;
      }
    }
  }

  /* Third save the equations in the forms of blocks of equal rank */

  ofstream outputfile;
  if (outfilename!="")
  { outputfile.open(outfilename.c_str(), ios::trunc);
    if (!outputfile.is_open())
    { gsErrorMsg("Could not save BES to %s\n", outfilename.c_str());
      exit(1);
    }
  }

  for(unsigned long r=1 ; r<=bes_equations.max_rank ; r++)
  { bool first=true;
    for(unsigned long i=1; i<=bes_equations.nr_of_variables() ; i++)
    { if (bes_equations.is_relevant(i) && (bes_equations.get_rank(i)==r))
      { if (first)
        { ((outfilename=="")?cout:outputfile) <<
             "block " <<
             ((bes_equations.get_fixpoint_symbol(i)==fixpoint_symbol::mu()) ? "mu  B" : "nu B") <<
             r-1 <<
             " is " << endl;
           first=false;
        }
        ((outfilename=="")?cout:outputfile) << "  X" << variable_index[i] << " = ";
        save_rhs_in_vasy_form(((outfilename=="")?cout:outputfile),
                                 bes_equations.get_rhs(i),
                                 variable_index,
                                 r,
                                 bes_equations);
        ((outfilename=="")?cout:outputfile) << endl;
      }
    }
    ((outfilename=="")?cout:outputfile) << "end block" << endl << endl;
  }

  outputfile.close();
}

//function save_rhs_in_vasy_form
//---------------------------
static void save_rhs_in_vasy_form(ostream &outputfile,
                                     bes_expression b,
                                     std::vector<unsigned long> &variable_index,
                                     const unsigned long current_rank,
                                     bes::equations &bes_equations)
{
  if (bes::is_true(b))
  { outputfile << "true";
  }
  else if (bes::is_false(b))
  { outputfile << "false";
  }
  else if (bes::is_and(b))
  {
    //BESAnd(a,b) => a and b
    save_rhs_in_vasy_form(outputfile,lhs(b),variable_index,current_rank,bes_equations);
    outputfile << " and ";
    save_rhs_in_vasy_form(outputfile,rhs(b),variable_index,current_rank,bes_equations);
  }
  else if (bes::is_or(b))
  {
    //BESOr(a,b) => a or b
    save_rhs_in_vasy_form(outputfile,lhs(b),variable_index,current_rank,bes_equations);
    outputfile << " or ";
    save_rhs_in_vasy_form(outputfile,rhs(b),variable_index,current_rank,bes_equations);
  }
  else if (bes::is_variable(b))
  {
    // PropVar => <Int>
    outputfile << "X" << variable_index[get_variable(b)];
    if (bes_equations.get_rank(get_variable(b))!=current_rank)
    { outputfile << "_" << bes_equations.get_rank(get_variable(b))-1;
    }
  }
  else if (bes::is_if(b))
  { //BESIF(x,y,z) is equivalent to (y & (x|z)) provided the expression is monotonic.
    save_rhs_in_vasy_form(outputfile,
                          and_optimized(then_branch(b),
                            or_optimized(condition(b),else_branch(b))),
                          variable_index,
                          current_rank,
                          bes_equations);

    //BESIF(x,y,z) => (x & y) | (!x&z)
    //If y is true, this reduces to (x | z)
    //If z is false, this reduces to (x & y)
    //Otherwise, the result is not monotonic,
    //which ought not to be possible.
    /* const bes_expression y=then_branch(b);
    const bes_expression z=else_branch(b);
    if (is_true(y))
    { if (is_false(z))
      { save_rhs_in_vasy_form(outputfile,condition(b),variable_index,current_rank,bes_equations);
      }
      else
      { save_rhs_in_vasy_form(outputfile,or_(condition(b),z),variable_index,current_rank,bes_equations);
      }
    }
    else if (is_false(z))
    { // not is_true(y)
      save_rhs_in_vasy_form(outputfile,and_(condition(b),y),variable_index,current_rank,bes_equations);
    }
    else
    { gsErrorMsg("The generated equation system is not a monotonic BES. It cannot be saved in VASY-format.\n");
      exit(1);
    } */
  }
  else
  {
    gsErrorMsg("The generated equation system is not a BES. It cannot be saved in VASY-format.\n");
    exit(1);
  }
  return;
}

//function save_bes_in_cwi_format
//--------------------------------
static void save_bes_in_cwi_format(const string &outfilename,bes::equations &bes_equations)
{
  gsVerboseMsg("Converting result to CWI-format...\n");
  // Use an indexed set to keep track of the variables and their cwi-representations

  ofstream outputfile;
  if (outfilename!="")
  { outputfile.open(outfilename.c_str(), ios::trunc);
    if (!outputfile.is_open())
    { gsErrorMsg("Could not save BES to %s\n", outfilename.c_str());
      exit(1);
    }
  }

  for(unsigned long r=1 ; r<=bes_equations.max_rank ; r++)
  { for(unsigned long i=1; i<=bes_equations.nr_of_variables() ; i++)
    {
      if (bes_equations.is_relevant(i) && (bes_equations.get_rank(i)==r) )
      { ((outfilename=="")?cout:outputfile) <<
              ((bes_equations.get_fixpoint_symbol(i)==fixpoint_symbol::mu()) ? "min X" : "max X") << i << "=";
        save_rhs_in_cwi_form(((outfilename=="")?cout:outputfile),bes_equations.get_rhs(i),bes_equations);
        ((outfilename=="")?cout:outputfile) << endl;
      }
    }
  }

  outputfile.close();
}

//function save_rhs_in_cwi
//---------------------------
static void save_rhs_in_cwi_form(ostream &outputfile, bes_expression b,bes::equations &bes_equations)
{
  if (bes::is_true(b))
  { outputfile << "T";
  }
  else if (bes::is_false(b))
  { outputfile << "F";
  }
  else if (bes::is_and(b))
  {
    //BESAnd(a,b) => (a & b)
    outputfile << "(";
    save_rhs_in_cwi_form(outputfile,lhs(b),bes_equations);
    outputfile << "&";
    save_rhs_in_cwi_form(outputfile,rhs(b),bes_equations);
    outputfile << ")";
  }
  else if (bes::is_or(b))
  {
    //BESOr(a,b) => (a | b)
    outputfile << "(";
    save_rhs_in_cwi_form(outputfile,lhs(b),bes_equations);
    outputfile << "|";
    save_rhs_in_cwi_form(outputfile,rhs(b),bes_equations);
    outputfile << ")";
  }
  else if (bes::is_variable(b))
  {
    // PropVar => <Int>
    outputfile << "X" << get_variable(b);
  }
  else if (bes::is_if(b))
  { //BESIF(x,y,z) is equivalent to (y & (x|z)) provided the expression is monotonic.
    save_rhs_in_cwi_form(outputfile,and_optimized(then_branch(b),
                                    or_optimized(condition(b),else_branch(b))),bes_equations);

    //BESIF(x,y,z) => (x & y) | (!x&z)
    //If y is true, this reduces to (x | z)
    //If z is false, this reduces to (x & y)
    //Otherwise, the result is not monotonic,
    //which ought not to be possible.
    /* const bes_expression y=then_branch(b);
    const bes_expression z=else_branch(b);
    if (is_true(y))
    { if (is_false(z))
      { save_rhs_in_cwi_form(outputfile,condition(b),bes_equations);
      }
      else
      { save_rhs_in_cwi_form(outputfile,or_(condition(b),z),bes_equations);
      }
    }
    else if (is_false(z))
    { // not is_true(y)
      save_rhs_in_cwi_form(outputfile,and_(condition(b),y),bes_equations);
    }
    else
    { ATfprintf(stderr,"WHAT: %t\n",(ATerm)b);
      gsErrorMsg("The generated equation system is not a monotonic BES. It cannot be saved in CWI-format.\n");
      exit(1);
    } */
  }
  else
  {
    gsErrorMsg("The generated equation system is not a BES. It cannot be saved in CWI-format.\n");
    exit(1);
  }
  return;
}

static pbes_expression generate_rhs_as_formula(bes_expression b);

//function save_bes_in_pbes_format
//--------------------------------
static void save_bes_in_pbes_format(
                   const string &outfilename,
                   bes::equations &bes_equations,
                   const pbes<> &p)
{
  gsVerboseMsg("Converting result to PBES-format...\n");
  // Use an indexed set to keep track of the variables and their pbes-representations

  atermpp::vector < pbes_equation > eqns;
  for(unsigned long r=1 ; r<=bes_equations.max_rank ; r++)
  { for(unsigned long i=1; i<=bes_equations.nr_of_variables() ; i++)
    { 
      if (bes_equations.is_relevant(i) && (bes_equations.get_rank(i)==r) )
      {  pbes_expression pbe=generate_rhs_as_formula(bes_equations.get_rhs(i));
         stringstream variable_name;
         variable_name << "X" << i;
         eqns.push_back(
               pbes_equation(
                  bes_equations.get_fixpoint_symbol(i),
                  propositional_variable(variable_name.str()),
                  generate_rhs_as_formula(bes_equations.get_rhs(i))));
      }
    }
  }

  pbes<> p1(p.data(),eqns,atermpp::set<variable>(),propositional_variable_instantiation("X1"));
  p1.save(outfilename);
}

//function save_rhs_in_pbes
//---------------------------
static pbes_expression generate_rhs_as_formula(bes_expression b)
{
  if (bes::is_true(b))
  { return true_();
  }
  else if (bes::is_false(b))
  { return false_();
  }
  else if (bes::is_and(b))
  { return and_(generate_rhs_as_formula(lhs(b)),
                generate_rhs_as_formula(rhs(b)));
  }
  else if (bes::is_or(b))
  { return or_(generate_rhs_as_formula(lhs(b)),
               generate_rhs_as_formula(rhs(b)));
  }
  else if (bes::is_variable(b))
  { stringstream converter;
    converter << "X" << get_variable(b);
    return   propositional_variable_instantiation(converter.str());
  }
  else if (bes::is_if(b))
  { //BESIF(x,y,z) is equivalent to (y & (x|z)) provided the expression is monotonic.
    return generate_rhs_as_formula(and_optimized(then_branch(b),
                                   or_optimized(condition(b),else_branch(b))));

    //BESIF(x,y,z) => (x & y) | (!x&z)
    //If y is true, this reduces to (x | z)
    //If z is false, this reduces to (x & y)
    //Otherwise, the result is not monotonic,
    //which ought not to be possible.
    /* const bes_expression y=then_branch(b);
    const bes_expression z=else_branch(b);
    if (is_true(y))
    { if (is_false(z))
      { generate_rhs_as_formula(condition(b));
      }
      else
      { generate_rhs_as_formula(or_(condition(b),z));
      }
    }
    else if (is_false(z))
    { // not is_true(y)
      generate_rhs_as_formula(and_(condition(b),y));
    }
    else
    { gsErrorMsg("The generated equation system is not a monotonic BES. It cannot be saved in PBES-format.\n");
      exit(1);
    } */
  }
  else
  {
    gsErrorMsg("The generated equation system is not a BES. It cannot be saved in CWI-format.\n");
    exit(1);
  }
  return true_();
}

