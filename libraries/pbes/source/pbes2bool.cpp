// Deprecated. 


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
/* #include "bes_deprecated.h"

using namespace std;
// using atermpp::make_substitution;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::pbes_expr;
using bes::bes_expression;
using namespace bes; */

//Function declarations used by main program
//------------------------------------------
/* void calculate_bes(pbes<> pbes_spec,
                          t_tool_options tool_options,
                          bes::boolean_equation_system &bes_equations,
                          atermpp::indexed_set &variable_index,
                          Rewriter *rewriter); */

//Post: tool_options.infilename contains a PBES ("" indicates stdin)
//Ret:  The BES generated from the PBES

// pbes<> load_pbes(t_tool_options tool_options);
//Post: tool_options.infilename contains a PBES ("" indicates stdin)
//Ret: The pbes loaded from infile


// static bool is_pair(ATerm t); 

// Function used to convert a pbes_expression to the variant used by the cwi-output

/* static void print_tree_rec(const char c,
                           ATerm t,
                           const bool opt_precompile_pbes,
                           Rewriter *rewriter,
                           ostream &f)
{
  if (is_pair(t))
  { print_tree_rec(',',ATgetArgument(t,0),opt_precompile_pbes,rewriter,f);
    print_tree_rec(',',ATgetArgument(t,1),opt_precompile_pbes,rewriter,f);
  }
  else
  { if (opt_precompile_pbes)
    { data_expression t1(rewriter->fromRewriteFormat((ATerm)t));
      f << c << pp(t1);
    }
    else
    { data_expression t1(t);
      f << c << pp(t1);
    }
  }
} */

/* static void print_counter_example_rec(bes::variable_type current_var,
                                      std::string indent,
                                      bes::boolean_equation_system &bes_equations,
                                      // atermpp::indexed_set &variable_index,
                                      vector<bool> &already_printed,
                                      bool opt_precompile_pbes,
                                      Rewriter *rewriter,
                                      const bool opt_store_as_tree,
                                      ostream &f)
{
  if (opt_store_as_tree)
  { ATerm t=bes_equations.variable_index().get(current_var);
    if (!is_pair(t))
    { f << ATgetName(ATgetAFun(t));
    }
    else
    { f << ATgetName(ATgetAFun(ATgetArgument(t,0)));
      print_tree_rec('(',ATgetArgument(t,1),opt_precompile_pbes,rewriter,f);
    }
  }
  else
  {
    propositional_variable_instantiation X(bes_equations.variable_index().get(current_var));

    data_expression_list tl=X.parameters();
    string s=X.name();
    f << s;
    for(data_expression_list::iterator t=tl.begin();
          t!=tl.end(); t++)
    { f << (t==tl.begin()?"(":",");
      if (opt_precompile_pbes)
      { ATermAppl term=*t;
        f << pp(rewriter->fromRewriteFormat((ATerm)term));
      }
      else
      { f << pp(*t);
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
                            bes_equations,already_printed,
                            opt_precompile_pbes,
                            rewriter,
                            opt_store_as_tree,f);
    }
  }
}

static void print_counter_example(bes::equations &bes_equations,
                                  // atermpp::indexed_set &variable_index,
                                  const bool opt_precompile_pbes,
                                  Rewriter *rewriter,
                                  const bool opt_store_as_tree,
                                  const string filename)
{ ofstream f;
  vector <bool> already_printed(bes_equations.nr_of_variables()+1,false);
  if (filename.empty())
  { // Print the counterexample to cout.
    cout << "Below the justification for this outcome is listed\n1: ";
    print_counter_example_rec(2,"  ",bes_equations,already_printed,
                       opt_precompile_pbes,rewriter,opt_store_as_tree,cout);
  }
  if (f!=NULL)
  {
    try
    {
      ofstream f(filename.c_str());
      f << "Below the justification for this outcome is listed\n1: ";
      print_counter_example_rec(2,"  ",bes_equations,already_printed,
                       opt_precompile_pbes,rewriter,opt_store_as_tree,f);
      f.close();
    }
    catch (std::exception& e)
    { cerr << "Fail to write counterexample to " << filename <<
               "(" << e.what() << ")\n";

    }
  }
}*/


/* static bool solve_bes(const t_tool_options &,
                      bes::equations &,
                      atermpp::indexed_set &); */

/* / Create a propositional variable instantiation with the checks needed in the naive algorithm

// void process(t_tool_options const& tool_options)
template <typename Container, typename PbesRewriter>
bool pbes2bool(const mcrl2::pbes_system::pbes<Container>& pbes_spec,
               PbesRewriter pbesr,
               t_tool_options tool_options / * = t_tool_options()* /)

{
  //Load PBES
  / * pbes<> pbes_spec;
     pbes_spec.load(tool_options.infilename); * /

  if (!pbes_spec.is_well_typed())
  { throw mcrl2::runtime_error("The pbes is not well typed\n");
  }

  if (!pbes_spec.instantiate_free_variables())
  { std::stringstream message;
    message << "Fail to instantiate all free variables in the pbes.\n";
    message << "Remaining free variables are: ";
    for(atermpp::set <data_variable>::iterator i=pbes_spec.free_variables().begin() ;
        i!=pbes_spec.free_variables().end() ; i++ )
    { message << pp(*i) << " ";
    }
    message << "\n";
    throw mcrl2::runtime_error(message.str());
  }

  if (!pbes_spec.is_closed())
  { throw mcrl2::runtime_error("The pbes contains free pbes variables \n");
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
  Rewriter *rewriter = createRewriter(data,tool_options.rewrite_strategy);
  assert(rewriter != 0);


  calculate_bes(pbes_spec, tool_options,bes_equations,variable_index,rewriter);
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
                            rewriter,
                            tool_options.opt_store_as_tree,
                            tool_options.opt_counter_example_file);
    }
  }
  delete rewriter;
} */

//function calculate_bes
//-------------------
/* void calculate_bes(pbes<> pbes_spec,
                   t_tool_options tool_options,
                   bes::equations &bes_equations,
                   atermpp::indexed_set &variable_index,
                   Rewriter *rewriter)
{

  do_lazy_algorithm(pbes_spec, tool_options,bes_equations,variable_index,rewriter);
  //return new pbes
  return;
} */

/* Declare a protected PAIR symbol * /
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

static void assign_variables_in_tree(
                      ATerm t,
                      data_variable_list::iterator &var_iter,
                      Rewriter *rewriter,
                      const bool opt_precompile_pbes)
{ if (is_pair(t))
  { assign_variables_in_tree(ATgetArgument(t,0),var_iter,rewriter,opt_precompile_pbes);
    assign_variables_in_tree(ATgetArgument(t,1),var_iter,rewriter,opt_precompile_pbes);
  }
  else
  {
    if (opt_precompile_pbes)
    { rewriter->setSubstitutionInternal(*var_iter,t);
    }
    else
    { rewriter->setSubstitution(*var_iter,(ATermAppl)t);
    }
    var_iter++;
  }
}


// static ATermAppl store_as_tree(pbes_expression p)
static ATermAppl store_as_tree(propositional_variable_instantiation p)
/ * We assume p is a propositional_variable_instantiation of the form B(x1,...,xn). If p has less than 3
 * arguments p is returned. Otherwise a term of the form B(pair(pair(...pair(x1,x2),...)))) is
 * returned, which is a balanced tree flushed to the right. For each input the resulting
 * tree is unique.
 * /
{
  data_expression_list args=p.parameters();

  if ( args.size() ==0 )
  return p.name();

  unsigned int n=largest_power_of_2_smaller_than(args.size());

  atermpp::vector<ATermAppl> tree_store(n);

  / * put the arguments in the intermediate tree_store. The last elements are stored as
   * pairs, such that the args.size() elements are stored in n positions. * /
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
} */

//function add_propositional_variable_instantiations_to_indexed_set
//and translate to pbes expression to a bes_expression in BDD format.

/* static bes::bes_expression add_propositional_variable_instantiations_to_indexed_set_and_translate(
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
                   Rewriter *rewriter)
{
  if (is_propositional_variable_instantiation(p))
  {
    pair<unsigned long,bool> pr=variable_index.put((opt_store_as_tree)?pbes_expression(store_as_tree(p)):p);

    if (pr.second) / * p is added to the indexed set, so it is a new variable * /
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
      if (strategy>lazy)
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
                            opt_precompile_pbes,rewriter);
    if (is_false(b1))
    { return b1;
    }
    bes::bes_expression b2=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::right(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,bes_equations,current_variable,opt_store_as_tree,
                            opt_precompile_pbes,rewriter);
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
                            opt_precompile_pbes,rewriter);
    if (bes::is_true(b1))
    { return b1;
    }

    bes::bes_expression b2=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::right(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,bes_equations,current_variable,opt_store_as_tree,
                            opt_precompile_pbes,rewriter);
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
  { cerr << "Unexpected expression. Most likely because expression fails to rewrite to true or false: " <<
                     pp(rewriter->fromRewriteFormat((ATerm)(ATermAppl)p)) << "\n";
  }
  else
  { cerr << "Unexpected expression. Most likely because expression fails to rewrite to true or false: " << pp(p) << "\n";
    abort();
  }
  exit(1);
  return bes::false_();
} */


/* substitute boolean equation expression * /
static bes_expression substitute_rank(
                bes_expression b,
                const unsigned long current_rank,
                const atermpp::vector<bes_expression> &approximation,
                bes::boolean_equation_system &bes_equations,
                const bool use_hashtable,
                atermpp::table &hashtable,
                bool store_counter_example=false,
                bes::variable_type current_variable=0)
{ / * substitute variables with rank larger and equal
     than current_rank with their approximations. * /

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
    { / * the condition is not equal to v * /
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
} */

/* substitute boolean equation expression * /

static bes_expression evaluate_bex(
                bes_expression b,
                const atermpp::vector<bes_expression> &approximation,
                const unsigned long rank,
                bes::boolean_equation_system &bes_equations,
                const bool use_hashtable,
                atermpp::table &hashtable,
                const bool construct_counter_example,
                const bes::variable_type current_variable)
{ / * substitute the approximation for variables in b, given
     by approximation, for all those variables that have a
     rank higher or equal to the variable rank; * /

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
    { / * the condition has lower rank than the variable rank,
         leave it untouched * /
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
    { / * the condition has lower rank than the variable rank,
         leave it untouched * /
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
} */

/* bool solve_bes(bes::boolean_equation_system &bes_equations,
               const bool opt_use_hashtables,
               const bool opt_construct_counter_example)
{
  if (mcrl2::core::gsVerbose)
  { std::cerr << "Solving the BES with " << bes_equations.nr_of_variables() <<
                                  "equations.\n";
  }
  atermpp::vector<bes_expression> approximation(bes_equations.nr_of_variables()+1);

  atermpp::table bex_hashtable(10,5);

  / * Set the approximation to its initial value * /
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

    / * Calculate the stable solution for the current rank * /

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
                             opt_use_hashtables,
                             bex_hashtable,
                             false,
                             v);

        if (toBDD(t)!=toBDD(approximation[v]))
        {
          if (opt_use_hashtables)
          { bex_hashtable.reset();  / * we change the approximation, so the
                                       hashtable becomes invalid * /
          }
          if (opt_construct_counter_example)
          { if (opt_use_hashtables)
            { bex_hashtable.reset();  / * We want to construct a counter example, which is
                                         not traceable when results in the hashtable are used * /
            }
            approximation[v]=evaluate_bex(
                             bes_equations.get_rhs(v),
                             approximation,
                             current_rank,
                             bes_equations,
                             opt_use_hashtables,
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
                              opt_use_hashtables,
                              bex_hashtable,
                              false,
                              *u);

          if (toBDD(t)!=toBDD(approximation[*u]))
          {
            if (opt_use_hashtables)
            { bex_hashtable.reset();  / * we change approximation, so the
                                         hashtable becomes invalid * /
            }
            if (opt_construct_counter_example)
            { if (opt_use_hashtables)
              { bex_hashtable.reset();  / * We want to construct a counter example, which is
                                           not traceable when results in the hashtable are used * /
              }
              approximation[*u]=evaluate_bex(
                                   bes_equations.get_rhs(*u),
                                   approximation,
                                   current_rank,
                                   bes_equations,
                                   opt_use_hashtables,
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

    / * substitute the stable solution for the current rank in all other
       equations. * /


    if (opt_use_hashtables)
    { bex_hashtable.reset();
    }

    for(bes::variable_type v=bes_equations.nr_of_variables(); v>0; v--)
    { if (bes_equations.is_relevant(v))
      {
        // std::cerr << "Substitute values in lower rank" << v << "\n";
        if (bes_equations.get_rank(v)==current_rank)
        {
          if (opt_construct_counter_example)
          { bes_equations.set_rhs(
                         v,
                         substitute_rank(
                                   bes_equations.get_rhs(v),
                                   current_rank,
                                   approximation,
                                   bes_equations,
                                   opt_use_hashtables,
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
                                   opt_use_hashtables,
                                   bex_hashtable,
                                   opt_construct_counter_example,v));
        }
      }
    }
    if (opt_use_hashtables)
    { bex_hashtable.reset();
    }

  }
  assert(bes::is_true(approximation[1])||
         bes::is_false(approximation[1]));
  return bes::is_true(approximation[1]);  / * 1 is the index of the initial variable * /
} */

