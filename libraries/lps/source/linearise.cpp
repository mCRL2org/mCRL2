// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearise.cpp
/// \brief Add your file description here.

/* This file contains the implementation of an mCRL2 lineariser.

   It is based on the implementation of the mCRL lineariser, on which work
   started on 12 juli 1997.  This lineariser was based on the CWI technical
   report "The Syntax and Semantics of Timed mCRL", by J.F. Groote.

   Everybody is free to use this software, provided it is not changed.

   In case problems are encountered when using this software, please report
   them to J.F. Groote, TU/e, Eindhoven, jfg@win.tue.nl

   This software comes as it is. I.e. the author assumes no responsibility for
   the use of this software.
*/

// Standard C libraries
#include <cassert>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <memory>
#include <algorithm>

// Boost utilities
#include "boost/format.hpp"
#include "boost/utility.hpp"

// ATermpp libraries
#include "mcrl2/atermpp/indexed_set.h"

// linear process libraries.
#include "mcrl2/lps/linearise.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lps/sumelm.h"
#include "mcrl2/lps/constelm.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/lps/find.h"

//mCRL2 data
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/map_substitution.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

//mCRL2 processes
#include "mcrl2/process/find.h"
#include "mcrl2/process/process_expression.h"
#include "mcrl2/process/process_equation.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/replace.h"


// For Aterm library extension functions
using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;
using namespace mcrl2::process;

/*  Preamble */

typedef enum { unknown,
               mCRL,
               mCRLdone,
               mCRLbusy,
               mCRLlin,
               pCRL,
               multiAction,
               GNF,
               GNFalpha,
               GNFbusy,
               error
             } processstatustype;

/**************** Definitions of object class  ***********************/

typedef enum { none,
               _map,
               func,
               act,
               proc,
               variable_,
               sorttype,
               multiact
             } objecttype;

class objectdatatype
{
  public:
    identifier_string objectname;
    process::action_label_list multi_action_names;
    bool constructor;
    process_expression representedprocess;
    process_identifier process_representing_action; /* for actions target sort is used to
                                   indicate the process representing this action. */
    process_expression processbody;
    std::set <variable> free_variables;
    bool free_variables_defined;
    variable_list parameters;
    variable_list old_parameters;
    processstatustype processstatus;
    objecttype object;
    bool canterminate;
    bool containstime;

    objectdatatype()
    {
      constructor=false;
      processstatus=unknown;
      object=none;
      canterminate=0;
      containstime=false;
    }

    objectdatatype(const objectdatatype& o)
    {
      objectname=o.objectname;
      multi_action_names=o.multi_action_names;
      constructor=o.constructor;
      representedprocess=o.representedprocess;
      process_representing_action=o.process_representing_action;
      processbody=o.processbody;
      free_variables=o.free_variables;
      free_variables_defined=o.free_variables_defined;
      parameters=o.parameters;
      processstatus=o.processstatus;
      object=o.object;
      canterminate=o.canterminate;
      containstime=o.containstime;
    }

    const objectdatatype& operator=(const objectdatatype& o)
    {
      objectname=o.objectname;
      multi_action_names=o.multi_action_names;
      constructor=o.constructor;
      representedprocess=o.representedprocess;
      process_representing_action=o.process_representing_action;
      processbody=o.processbody;
      free_variables=o.free_variables;
      free_variables_defined=o.free_variables_defined;
      parameters=o.parameters;
      processstatus=o.processstatus;
      object=o.object;
      canterminate=o.canterminate;
      containstime=o.containstime;
      return (*this);
    }

    ~objectdatatype()
    {
    }
};


class specification_basic_type:public boost::noncopyable
{
  public:
    process::action_label_list acts;     /* storage place for actions */
    std::set < variable > global_variables; /* storage place for free variables occurring
                                   in processes ranging over data */
    variable_list initdatavars; /* storage place for free variables in
                                   init clause */
    data_specification data;    /* contains the data specification for the current process.  */

  private:
    class stackoperations;
    class stacklisttype;
    class enumtype;
    class enumeratedtype;

    std::vector < process_equation > procs;
    /* storage place for processes,
       uses alt, seq, par, lmer, cond,sum,
       com, bound, at, name, delta,
       tau, hide, rename, encap */
    mcrl2::data::rewriter rewr; /* The rewriter used while linearising */
    action terminationAction;   /* A list of length one with the action that denotes termination */
    process_identifier terminatedProcId; /* A process identifier of which the body consists of the termination
                                            action */
    process_identifier tau_process;
    process_identifier delta_process;
    std::vector < process_identifier > seq_varnames; /* Contains names of processes which represent a sequence
                                                            of process variables */
    std::vector < std::vector < process_instance_assignment > > representedprocesses; /* contains the sequences of process
                                                         instances that are represented by the variables in seq_varnames */
    t_lin_options options;
    bool timeIsBeingUsed;
    bool stochastic_operator_is_being_used;
    bool fresh_equation_added;
    std::deque < objectdatatype > objectdata; // This is a double ended queue to guarantee that the objects will not
                                              // be moved to another place when the object data structure grows. This
                                              // is because objects in this datatype  are passed around by reference.

    indexed_set<aterm_appl> objectIndexTable;
    set_identifier_generator fresh_identifier_generator;
    std::vector < enumeratedtype > enumeratedtypes;
    stackoperations* stack_operations_list;

  public:
    specification_basic_type(const process::action_label_list& as,
                             const std::vector< process_equation >& ps,
                             const variable_list& idvs,
                             const data_specification& ds,
                             const std::set < data::variable >& glob_vars,
                             const t_lin_options& opt,
                             const process_specification& procspec):
      acts(),
      global_variables(glob_vars),
      data(ds),
      rewr(data,opt.rewrite_strategy),
      options(opt),
      timeIsBeingUsed(false),
      stochastic_operator_is_being_used(false),
      fresh_equation_added(false)
    {
      objectIndexTable=indexed_set<aterm_appl>(1024,75);

      // find_identifiers does not find the identifiers in the enclosed data specification.
      fresh_identifier_generator.add_identifiers(process::find_identifiers(procspec));
      // So, the identifiers in the data type must be added explicitly.
      fresh_identifier_generator.add_identifiers(data::find_identifiers(ds.equations()));
      fresh_identifier_generator.add_identifiers(data::find_identifiers(ds.sorts()));
      fresh_identifier_generator.add_identifiers(data::find_identifiers(ds.constructors()));
      fresh_identifier_generator.add_identifiers(data::find_identifiers(ds.mappings()));

      stack_operations_list=NULL;
      acts=as;
      storeact(acts);
      procs=ps;
      storeprocs(procs);
      initdatavars=idvs;
      // The terminationAction and the terminatedProcId must be defined after initialisation of
      // data as otherwise fresh name does not work properly.
      terminationAction=action(action_label(fresh_identifier_generator("Terminate"),sort_expression_list()),data_expression_list());
      terminatedProcId=process_identifier(fresh_identifier_generator("Terminated**"), variable_list());
// /* Changed delta() to DeltaAtZero on 24/12/2006. Moved back in spring 2007, as this introduces unwanted time constraints. */
      insertProcDeclaration(
        terminatedProcId,
        variable_list(),
        seq(terminationAction,delta()),
        pCRL,0,false);
      delta_process=newprocess(variable_list(),delta(),pCRL,false,false);
      tau_process=newprocess(variable_list(),tau(),pCRL,true,false);
    }

    ~specification_basic_type()
    {
      for (; stack_operations_list!=NULL;)
      {
        stackoperations* temp=stack_operations_list->next;
        delete stack_operations_list;
        stack_operations_list=temp;
      }

    }

  private:
    data_expression real_zero()
    {
      static data_expression zero=sort_real::creal(sort_int::cint(sort_nat::c0()),sort_pos::c1());
      return zero;
    }

    data_expression real_one()
    {
      static data_expression one=sort_real::creal(sort_int::cint(sort_nat::cnat(sort_pos::c1())),sort_pos::c1());
      return one;
    }

    data_expression real_times_optimized(const data_expression& r1, const data_expression& r2)
    {
      if (r1==real_zero() || r2==real_zero())
      {
        return real_zero();
      }
      if (r1==real_one())
      {
        return r2;
      }
      if (r2==real_one())
      {
        return r1;
      }
      return sort_real::times(r1,r2);
    }

    process_expression delta_at_zero(void)
    {
      return at(delta(), data::sort_real::real_(0));
    }

    bool isDeltaAtZero(const process_expression t)
    {
      if (!is_at(t))
      {
        return false;
      }
      if (!is_delta(at(t).operand()))
      {
        return false;
      }
      return RewriteTerm(at(t).time_stamp())==data::sort_real::real_(0);
    }


    /*****************  store and retrieve basic objects  ******************/

    size_t addObject(aterm_appl o, bool& b)
    {
      std::pair<size_t, bool> result=objectIndexTable.put(o);
      if (objectdata.size()<=result.first)
      {
        objectdata.resize(result.first+1);
      }
      b=result.second;
      return result.first;
    }

    size_t objectIndex(aterm_appl o)
    {
      size_t result=objectIndexTable.index(o);
      if (result==atermpp::npos)
      {
        if (is_process_identifier(o))
        {
          throw mcrl2::runtime_error("Fail to recognize " + process::pp(process_identifier(o)) + ". Most likely due to unguarded recursion in a process equation.\n");
        }
        else
        {
          throw mcrl2::runtime_error("Fail to recognize " + process::pp(o) + ". This is an internal error in the lineariser.\n");
        }
      }
      return result;
    }

    void addString(const identifier_string str)
    {
      fresh_identifier_generator.add_identifier(str);
    }

    process_expression action_list_to_process(const action_list ma)
    {
      if (ma.size()==0)
      {
        return tau();
      }
      if (ma.size()==1)
      {
        return ma.front();
      }
      return process::sync(ma.front(),action_list_to_process(ma.tail()));
    }

    action_list to_action_list(const process_expression p)
    {
      if (is_tau(p))
      {
        return action_list();
      }

      if (is_action(p))
      {
        return { action(p) };
      }

      if (is_sync(p))
      {
        return to_action_list(process::sync(p).left())+to_action_list(process::sync(p).right());
      }
      assert(0);
      return action_list();
    }

    process::action_label_list getnames(const process_expression multiAction)
    {
      if (is_action(multiAction))
      {
        return { action(multiAction).label() };
      }
      assert(is_sync(multiAction));
      return getnames(process::sync(multiAction).left())+getnames(process::sync(multiAction).right());
    }

    // Returns a list of variables with the same sort as the expressions in the list.
    // If the expression is a variable not occurring in the occurs_set that variable
    // is used.
    variable_list make_parameters_rec(const data_expression_list l,
                                      std::set < variable>& occurs_set)
    {
      variable_list result;
      for (data_expression_list::const_iterator l1=l.begin();
           l1!=l.end() ; ++l1)
      {
        /* if the current argument of the multi-action is a variable that does
         not occur in result, use this variable. This is advantageous, when joining
         processes to one linear process where variable names are joined. If this
         is not being done (as happened before 4/1/2008) very long lists of parameters
         can occur when linearising using regular2 */
        if (is_variable(*l1) && std::find(occurs_set.begin(),occurs_set.end(),*l1)==occurs_set.end())
        {
          const variable& v = atermpp::down_cast<variable>(*l1);
          result.push_front(v);
          occurs_set.insert(v);
        }
        else
        {
          result.push_front(variable(get_fresh_variable("a",l1->sort())));
        }
      }
      return reverse(result);
    }

    variable_list getparameters_rec(const process_expression multiAction,
                                    std::set < variable>& occurs_set)
    {
      if (is_action(multiAction))
      {
        return make_parameters_rec(action(multiAction).arguments(),occurs_set);
      }
      assert(is_sync(multiAction));
      return getparameters_rec(process::sync(multiAction).left(),occurs_set)+
             getparameters_rec(process::sync(multiAction).right(),occurs_set);
    }

    variable_list getparameters(const process_expression multiAction)
    {
      std::set < variable > occurs_set;
      return getparameters_rec(multiAction,occurs_set);
    }

    data_expression_list getarguments(const action_list multiAction)
    {
      data_expression_list result;
      for (action_list::const_iterator l=multiAction.begin(); l!=multiAction.end(); ++l)
      {
        result=reverse(l->arguments()) + result;
      }
      return reverse(result);
    }

    action_list makemultiaction(const process::action_label_list actionIds, const data_expression_list args)
    {
      action_list result;
      data_expression_list::const_iterator e_walker=args.begin();
      for (process::action_label_list::const_iterator l=actionIds.begin() ; l!=actionIds.end() ; ++l)
      {
        size_t arity=l->sorts().size();
        data_expression_list temp_args;
        for (size_t i=0 ; i< arity; ++i,++e_walker)
        {
          assert(e_walker!=args.end());
          temp_args.push_front(*e_walker);
        }
        temp_args=reverse(temp_args);
        result.push_front(action(*l,temp_args));
      }
      assert(e_walker==args.end());
      return reverse(result);
    }

    size_t addMultiAction(const process_expression multiAction, bool& isnew)
    {
      const process::action_label_list actionnames=getnames(multiAction);
      size_t n=addObject((aterm_appl)(aterm_list)actionnames,isnew);

      if (isnew)
      {
        // tempvar is needed as objectdata can change during a call
        // of getparameters.
        const variable_list templist=getparameters(multiAction);
        objectdata[n].parameters=templist;
        objectdata[n].object=multiact;
        // must separate assignment below as
        // objectdata may change as a side effect of make
        // multiaction.
        const action_list tempvar=makemultiaction(actionnames, data_expression_list(objectdata[n].parameters));
        objectdata[n].processbody=action_list_to_process(tempvar);
        objectdata[n].free_variables=std::set<variable>(objectdata[n].parameters.begin(), objectdata[n].parameters.end());
        objectdata[n].free_variables_defined=true;
      }
      return n;
    }

    const std::set<variable>& get_free_variables(const size_t n)
    {
      if (!objectdata[n].free_variables_defined)
      {
        objectdata[n].free_variables=find_free_variables_process(objectdata[n].processbody);
        objectdata[n].free_variables_defined=true;
      }
      return objectdata[n].free_variables;
    }

    void insertvariable(const variable& var, const bool mustbenew)
    {
      addString(var.name());

      bool isnew=false;
      size_t n=addObject(var.name(),isnew);

      if ((!isnew) && mustbenew)
      {
        throw mcrl2::runtime_error("variable " + data::pp(var) + " already exists");
      }

      objectdata[n].objectname=var.name();
      objectdata[n].object=variable_;
    }

    void insertvariables(const variable_list& vars, const bool mustbenew)
    {
      for (variable_list::const_iterator l=vars.begin(); l!=vars.end(); ++l)
      {
        insertvariable(*l,mustbenew);
      }
    }

    template <class SUBSTITUTION>
    std::set<data::variable> sigma_variables(const SUBSTITUTION& sigma)
    {
      std::set<data::variable> result;
      for (typename SUBSTITUTION::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
      {
        std::set<data::variable> V = data::find_free_variables(i->second);
        V.erase(i->first);
        result.insert(V.begin(), V.end());
      }
      return result;
    }


    /************ upperpowerof2 *********************************************/

    size_t upperpowerof2(size_t i)
    /* function yields n for the smallest value n such that
       2^n>=i. This constitutes the number of bits necessary
       to represent a number smaller than i. i is assumed to
       be at least 1. */
    {
      size_t n=0;
      size_t powerof2=1;
      for (; powerof2< i ; n++)
      {
        powerof2=2*powerof2;
      }
      return n;
    }

    data_expression RewriteTerm(const data_expression& t)
    {
      if (!options.norewrite)
      {
        if (fresh_equation_added)
        {
          rewr=rewriter(data,options.rewrite_strategy);
          fresh_equation_added=false;
        }
        return rewr(t);
      }
      return t;
    }

    data_expression_list RewriteTermList(const data_expression_list& t)
    {
      data_expression_vector v;
      for(data_expression_list::const_iterator i=t.begin(); i!=t.end(); ++i)
      {
        v.push_back(RewriteTerm(*i));
      }
      return data_expression_list(v.begin(),v.end());
    }

    assignment_list rewrite_assignments(const assignment_list& t)
    {
      assignment_vector v;
      for(assignment_list::const_iterator i=t.begin(); i!=t.end(); ++i)
      {
        v.push_back(assignment(i->lhs(), RewriteTerm(i->rhs())));
      }
      return assignment_list(v.begin(),v.end());
    }

    action RewriteAction(const action& t)
    {
      return action(t.label(),RewriteTermList(t.arguments()));
    }

    process_instance_assignment RewriteProcess(const process_instance_assignment& t)
    {
      return process_instance_assignment(t.identifier(),rewrite_assignments(t.assignments()));
    }

    process_expression RewriteMultAct(const process_expression& t)
    {
      if (is_tau(t))
      {
        return t;
      }

      if (is_action(t))
      {
        return RewriteAction(action(t));
      }

      assert(is_sync(t));  // A multi action is a sequence of actions with a sync operator in between.
      return process::sync(RewriteMultAct(process::sync(t).left()),RewriteMultAct(process::sync(t).right()));
    }


    process_expression pCRLrewrite(const process_expression& t)
    {
      if (options.norewrite)
      {
        return t;
      }

      if (is_if_then(t))
      {
        const data_expression new_cond=RewriteTerm(if_then(t).condition());
        const process_expression new_then_case=pCRLrewrite(if_then(t).then_case());
        if (new_cond==sort_bool::true_())
        {
          return new_then_case;
        }
        return if_then(new_cond,new_then_case);
      }

      if (is_seq(t))
      {
        /* only one summand is needed */
        return seq(
                 pCRLrewrite(seq(t).left()),
                 pCRLrewrite(seq(t).right()));
      }

      if (is_at(t))
      {
        const data_expression atTime=RewriteTerm(at(t).time_stamp());
        const process_expression t1=pCRLrewrite(at(t).operand());
        return at(t1,atTime);
      }

      if (is_stochastic_operator(t))
      {
        const stochastic_operator& sto=atermpp::down_cast<const stochastic_operator>(t);
        return stochastic_operator(sto.variables(),RewriteTerm(sto.distribution()),pCRLrewrite(sto.operand()));
      }

      if (is_delta(t) || is_tau(t))
      {
        return t;
      }

      if (is_action(t))
      {
        return RewriteAction(atermpp::down_cast<process::action>(t));
      }

      if (is_process_instance_assignment(t))
      {
        return RewriteProcess(process_instance_assignment(t));
      }

      if (is_sync(t))
      {
        return RewriteMultAct(t);
      }

      throw mcrl2::runtime_error("Expected a term in pCRL format, using only basic process operators: " + process::pp(t));
      return process_expression();
    }

    /************ storeact ****************************************************/

    size_t insertAction(const action_label& actionId)
    {
      bool isnew=false;
      size_t n=addObject(actionId,isnew);

      if (isnew==0)
      {
        throw mcrl2::runtime_error("Action " + process::pp(actionId) + " is added twice\n");
      }

      const identifier_string str=actionId.name();
      addString(str);
      objectdata[n].objectname=str;
      objectdata[n].object=act;
      objectdata[n].process_representing_action=process_identifier();
      return n;
    }

    void storeact(const process::action_label_list& acts)
    {
      for (process::action_label_list::const_iterator l=acts.begin(); l!=acts.end(); ++l)
      {
        insertAction(*l);
      }
    }

    /************ storeprocs *************************************************/

    size_t insertProcDeclaration(
      const process_identifier procId, // This should not be a reference.
      const variable_list parameters,  // This should not be a reference.
      const process_expression& body,
      processstatustype s,
      const bool canterminate,
      const bool containstime)
    {
      assert(procId.variables().size()==parameters.size());
      const std::string str=procId.name();
      addString(str);

      bool isnew=false;
      size_t n=addObject(procId,isnew);

      if (isnew==0)
      {
        throw mcrl2::runtime_error("Process " + process::pp(procId) + " is added twice\n");
      }

      objectdata[n].objectname=procId.name();
      objectdata[n].object=proc;
      objectdata[n].processbody=body;
      objectdata[n].free_variables_defined=false;
      objectdata[n].canterminate=canterminate;
      objectdata[n].containstime=containstime;
      objectdata[n].processstatus=s;
      objectdata[n].parameters=parameters;
      insertvariables(parameters,false);
      return n;
    }

    void storeprocs(const std::vector< process_equation >& procs)
    {
      for (std::vector< process_equation >::const_iterator i=procs.begin();
           i!=procs.end(); ++i)
      {
        insertProcDeclaration(
          i->identifier(),
          i->formal_parameters(),
          i->expression(),
          unknown,0,false);
      }
    }

    bool searchProcDeclaration(
      const variable_list parameters,  
      const process_expression& body,
      const processstatustype s,
      const bool canterminate,
      const bool containstime,
      process_identifier& p)
    {
      for(const objectdatatype& d:objectdata)
      { 
        if (d.object==proc &&
            d.parameters==parameters &&
            d.processbody==body &&
            d.canterminate==canterminate &&
            d.containstime==containstime &&
            d.processstatus==s)
        {
          p=process_identifier(d.objectname,d.parameters);
          return true;
        }
      }
      return false;
    }


    /************ storeinit *************************************************/

  public:
    process_identifier storeinit(const process_expression& init)
    {
      /* init is used as the name of the initial process,
         because it cannot occur as a string in the input */

      process_identifier initprocess(std::string("init"), variable_list());
      insertProcDeclaration(initprocess,variable_list(),init,unknown,0,false);
      return initprocess;
    }

  private:

    /********** various functions on action and multi actions  ***************/
    bool actioncompare(const action_label& a1, const action_label& a2)
    {
      /* first compare the strings in the actions */
      if (std::string(a1.name())<std::string(a2.name()))
      {
        return true;
      }

      if (a1.name()==a2.name())
      {
        /* the strings are equal; the sorts are used to
           determine the ordering */
        return a1.sorts()<a2.sorts();
      }

      return false;
    }

    action_list linInsertActionInMultiActionList(
      const action& act,
      action_list multiAction)
    {
      /* store the action in the multiAction, alphabetically
         sorted on the actionname in the actionId. Note that
         the empty multiAction represents tau. */

      if (multiAction.empty())
      {
        return { act };
      }
      const action firstAction=multiAction.front();

      /* Actions are compared on the basis of their position
         in memory, to order them. As the aterm library maintains
         pointers to objects that are not garbage collected, this
         is a safe way to do this. */
      if (actioncompare(act.label(),firstAction.label()))
      {
        multiAction.push_front(act);
        return multiAction;
      }
      action_list result= linInsertActionInMultiActionList(
                          act,
                          multiAction.tail());
      result.push_front(firstAction);
      return result;
    }

    action_list linMergeMultiActionList(const action_list& ma1, const action_list& ma2)
    {
      action_list result=ma2;
      for (action_list::const_iterator i=ma1.begin() ; i!=ma1.end() ; ++i)
      {
        result=linInsertActionInMultiActionList(*i,result);
      }
      return result;
    }


    action_list linMergeMultiActionListProcess(const process_expression& ma1, const process_expression& ma2)
    {
      return linMergeMultiActionList(to_action_list(ma1),to_action_list(ma2));
    }

    /************** determine_process_status ********************************/

    processstatustype determine_process_statusterm(
      const process_expression body,  // intentionally not a reference.
      const processstatustype status)
    {
      /* In this procedure it is determined whether a process
         is of type mCRL, pCRL or a multiAction. pCRL processes
         occur strictly within mCRL processes, and multiActions
         occur strictly within pCRL processes. Processes that pass
         this procedure can be linearised. Bounded initialisation,
         the leftmerge and synchronization merge on the highest
         level are filtered out. */
      if (is_choice(body))
      {
        if (status==multiAction)
        {
          throw mcrl2::runtime_error("Choice operator occurs in a multi-action in " + process::pp(body) + ".");
        }
        const processstatustype s1=determine_process_statusterm(choice(body).left(),pCRL);
        const processstatustype s2=determine_process_statusterm(choice(body).right(),pCRL);
        if ((s1==mCRL)||(s2==mCRL))
        {
          throw mcrl2::runtime_error("mCRL operators occur within the scope of a choice operator in " + process::pp(body) +".");
        }
        return pCRL;
      }

      if (is_seq(body))
      {
        if (status==multiAction)
        {
          throw mcrl2::runtime_error("Sequential operator occurs in a multi-action in " + process::pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(seq(body).left(),pCRL);
        const processstatustype s2=determine_process_statusterm(seq(body).right(),pCRL);
        if ((s1==mCRL)||(s2==mCRL))
        {
          throw mcrl2::runtime_error("mCRL operators occur within the scope of a sequential operator in " + process::pp(body) +".");
        }
        return pCRL;
      }

      if (is_merge(body))
      {
        if (status!=mCRL)
        {
          throw mcrl2::runtime_error("Parallel operator occurs in the scope of pCRL operators in " + process::pp(body) +".");
        }
        determine_process_statusterm(process::merge(body).left(),mCRL);
        determine_process_statusterm(process::merge(body).right(),mCRL);
        return mCRL;
      }

      if (is_left_merge(body))
      {
        throw mcrl2::runtime_error("Cannot linearize because the specification contains a leftmerge.");
      }

      if (is_if_then(body))
      {
        if (status==multiAction)
        {
          throw mcrl2::runtime_error("If-then occurs in a multi-action in " + process::pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(if_then(body).then_case(),pCRL);
        if (s1==mCRL)
        {
          throw mcrl2::runtime_error("mCRL operators occur in the scope of the if-then operator in " + process::pp(body) +".");
        }
        return pCRL;
      }

      if (is_if_then_else(body))
      {
        if (status==multiAction)
        {
          throw mcrl2::runtime_error("If-then-else occurs in a multi-action in " + process::pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(if_then_else(body).then_case(),pCRL);
        const processstatustype s2=determine_process_statusterm(if_then_else(body).else_case(),pCRL);
        if ((s1==mCRL)||(s2==mCRL))
        {
          throw mcrl2::runtime_error("mCRL operators occur in the scope of the if-then-else operator in " + process::pp(body) +".");
        }
        return pCRL;
      }

      if (is_sum(body))
      {
        /* insert the variable names of variables, to avoid
           that this variable name will be reused later on */
        insertvariables(sum(body).variables(),false);
        if (status==multiAction)
        {
          throw mcrl2::runtime_error("Sum operator occurs within a multi-action in " + process::pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(sum(body).operand(),pCRL);
        if (s1==mCRL)
        {
          throw mcrl2::runtime_error("mCRL operators occur in the scope of the sum operator in " + process::pp(body) +".");
        }
        return pCRL;
      }

      if (is_stochastic_operator(body))
      {
        stochastic_operator_is_being_used=true;
        /* insert the variable names of variables, to avoid
           that this variable name will be reused later on */
        const stochastic_operator& sto=down_cast<const stochastic_operator>(body);
        insertvariables(sto.variables(),false);
        if (status==multiAction)
        {
          throw mcrl2::runtime_error("Stochastic operator occurs within a multi-action in " + process::pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(sto.operand(),pCRL);
        if (s1==mCRL)
        {
          throw mcrl2::runtime_error("mCRL operators occur in the scope of the stochastic operator in " + process::pp(body) +".");
        }
        return pCRL;
      }

      if (is_comm(body))
      {
        if (status!=mCRL)
        {
          throw mcrl2::runtime_error("Communication operator occurs in the scope of pCRL operators in " + process::pp(body) +".");
        }
        determine_process_statusterm(comm(body).operand(),mCRL);
        return mCRL;
      }

      if (is_bounded_init(body))
      {
        throw mcrl2::runtime_error("Cannot linearize a specification with the bounded initialization operator.");
      }

      if (is_at(body))
      {
        timeIsBeingUsed = true;
        if (status==multiAction)
        {
          throw mcrl2::runtime_error("Time operator occurs in a multi-action in " + process::pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(at(body).operand(),pCRL);
        if (s1==mCRL)
        {
          throw mcrl2::runtime_error("mCRL operator occurs in the scope of a time operator in " + process::pp(body) +".");
        }
        return pCRL;
      }

      if (is_sync(body))
      {
        const processstatustype s1=determine_process_statusterm(process::sync(body).left(),pCRL);
        const processstatustype s2=determine_process_statusterm(process::sync(body).right(),pCRL);
        if ((s1!=multiAction)||(s2!=multiAction))
        {
          throw mcrl2::runtime_error("Other objects than multi-actions occur in the scope of a synch operator in " + process::pp(body) +".");
        }
        return multiAction;
      }

      if (is_action(body))
      {
        return multiAction;
      }

      if (is_process_instance(body))
      {
        assert(0);
        determine_process_status(process_instance(body).identifier(),status);
        return status;
      }

      if (is_process_instance_assignment(body))
      {
        determine_process_status(process_instance_assignment(body).identifier(),status);
        return status;
      }

      if (is_delta(body))
      {
        return pCRL;
      }

      if (is_tau(body))
      {
        return multiAction;
      }

      if (is_hide(body))
      {
        if (status!=mCRL)
        {
          throw mcrl2::runtime_error("Hide operator occurs in the scope of pCRL operators in " + process::pp(body) +".");
        }
        determine_process_statusterm(hide(body).operand(),mCRL);
        return mCRL;
      }

      if (is_rename(body))
      {
        if (status!=mCRL)
        {
          throw mcrl2::runtime_error("Rename operator occurs in the scope of pCRL operators in " + process::pp(body) +".");
        }
        determine_process_statusterm(process::rename(body).operand(),mCRL);
        return mCRL;
      }

      if (is_allow(body))
      {
        if (status!=mCRL)
        {
          throw mcrl2::runtime_error("Allow operator occurs in the scope of pCRL operators in " + process::pp(body) +".");
        }
        determine_process_statusterm(allow(body).operand(),mCRL);
        return mCRL;
      }

      if (is_block(body))
      {
        if (status!=mCRL)
        {
          throw mcrl2::runtime_error("Block operator occurs in the scope of pCRL operators in " + process::pp(body) +".");
        }
        determine_process_statusterm(block(body).operand(),mCRL);
        return mCRL;
      }

      throw mcrl2::runtime_error("Process has unexpected format (2) " + process::pp(body) +".");
      return error;
    }


    void determine_process_status(
      const process_identifier procDecl,
      const processstatustype status)
    {
      processstatustype s;
      size_t n=objectIndex(procDecl);
      s=objectdata[n].processstatus;

      if (s==unknown)
      {
        objectdata[n].processstatus=status;
        if (status==pCRL)
        {
          determine_process_statusterm(objectdata[n].processbody,pCRL);
          return;
        }
        /* status==mCRL */
        s=determine_process_statusterm(objectdata[n].processbody,mCRL);
        if (s!=status)
        {
          /* s==pCRL and status==mCRL */
          objectdata[n].processstatus=s;
          determine_process_statusterm(objectdata[n].processbody,pCRL);
        }
      }
      if (s==mCRL)
      {
        if (status==pCRL)
        {
          objectdata[n].processstatus=pCRL;
          determine_process_statusterm(objectdata[n].processbody,pCRL);
        }
      }
    }

    /***********  collect pcrlprocessen **********************************/

    void collectPcrlProcesses_term(const process_expression body,  // Intentionally not a reference.
                                   std::vector <process_identifier>& pcrlprocesses,
                                   std::set <process_identifier>& visited)
    {
      if (is_if_then(body))
      {
        collectPcrlProcesses_term(if_then(body).then_case(),pcrlprocesses,visited);
        return;
      }

      if (is_if_then_else(body))
      {
        collectPcrlProcesses_term(if_then_else(body).then_case(),pcrlprocesses,visited);
        collectPcrlProcesses_term(if_then_else(body).else_case(),pcrlprocesses,visited);
        return;
      }

      if (is_choice(body))
      {
        collectPcrlProcesses_term(choice(body).left(),pcrlprocesses,visited);
        collectPcrlProcesses_term(choice(body).right(),pcrlprocesses,visited);
        return ;
      }

      if (is_seq(body))
      {
        collectPcrlProcesses_term(seq(body).left(),pcrlprocesses,visited);
        collectPcrlProcesses_term(seq(body).right(),pcrlprocesses,visited);
        return ;
      }

      if (is_merge(body))
      {
        collectPcrlProcesses_term(process::merge(body).left(),pcrlprocesses,visited);
        collectPcrlProcesses_term(process::merge(body).right(),pcrlprocesses,visited);
        return ;
      }

      if (is_sync(body))
      {
        collectPcrlProcesses_term(process::sync(body).left(),pcrlprocesses,visited);
        collectPcrlProcesses_term(process::sync(body).right(),pcrlprocesses,visited);
        return ;
      }

      if (is_sum(body))
      {
        collectPcrlProcesses_term(sum(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_stochastic_operator(body))
      {
        collectPcrlProcesses_term(stochastic_operator(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_at(body))
      {
        collectPcrlProcesses_term(at(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_process_instance(body))
      {
        assert(0);
        collectPcrlProcesses(process_instance(body).identifier(),pcrlprocesses,visited);
        return;
      }

      if (is_process_instance_assignment(body))
      {
        collectPcrlProcesses(process_instance_assignment(body).identifier(),pcrlprocesses,visited);
        return;
      }

      if (is_hide(body))
      {
        collectPcrlProcesses_term(hide(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_rename(body))
      {
        collectPcrlProcesses_term(process::rename(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_allow(body))
      {
        collectPcrlProcesses_term(allow(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_block(body))
      {
        collectPcrlProcesses_term(block(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_comm(body))
      {
        collectPcrlProcesses_term(comm(body).operand(),pcrlprocesses,visited);
        return;
      }

      if ((is_delta(body))||
          (is_tau(body))||
          (is_action(body)))
      {
        return;
      }

      throw mcrl2::runtime_error("process has unexpected format (1) " + process::pp(body) +".");
    }

    void collectPcrlProcesses(
      const process_identifier procDecl,
      std::vector <process_identifier>& pcrlprocesses,
      std::set <process_identifier>& visited)
    {
      if (visited.count(procDecl)==0)
      {
        visited.insert(procDecl);
        size_t n=objectIndex(procDecl);
        if (objectdata[n].processstatus==pCRL)
        {
          pcrlprocesses.push_back(procDecl);
        }
        collectPcrlProcesses_term(objectdata[n].processbody,pcrlprocesses,visited);
      }
    }

    void collectPcrlProcesses(
      const process_identifier procDecl,
      std::vector <process_identifier>& pcrlprocesses)
    {
      std::set <process_identifier>  visited;
      collectPcrlProcesses(procDecl, pcrlprocesses, visited);
    }

    /****************  occursinterm *** occursintermlist ***********/

    bool occursinterm(const variable var, const data_expression t)
    {
      return data::search_free_variable(t, var);
    }

    void filter_vars_by_term(
      const data_expression t,
      const std::set < variable >& vars_set,
      std::set < variable >& vars_result_set)
    {
      if (is_variable(t))
      {
        const variable& v = atermpp::down_cast<variable>(t);
        if (vars_set.find(v)!=vars_set.end())
        {
          vars_result_set.insert(v);
        }
        return;
      }

      if (is_function_symbol(t))
      {
        return;
      }

      if (is_abstraction(t))
      {
        // mCRL2log(mcrl2::log::warning) << "filtering of variables expression with binders" << std::endl;
        return;
      }

      if (is_where_clause(t))
      {
        // mCRL2log(mcrl2::log::warning) << "filtering of variables expression with where clause" << std::endl;
        return;
      }

      if (!is_application(t))
      {
        mCRL2log(mcrl2::log::error) << "term of unexpected type " << t << std::endl;
      }

      assert(is_application(t));

      const application& a=atermpp::down_cast<const application>(t);
      filter_vars_by_term(a.head(),vars_set,vars_result_set);
      filter_vars_by_termlist(a.begin(),a.end(),vars_set,vars_result_set);
    }

    bool occursintermlist(const variable& var, const data_expression_list& r)
    {
      for (data_expression_list::const_iterator l=r.begin() ; l!=r.end() ; ++l)
      {
        if (occursinterm(var,*l))
        {
          return true;
        }
      }
      return false;
    }

    bool occursintermlist(const variable& var, const assignment_list& r, const process_identifier& proc_name)
    {
      std::set<variable> assigned_variables;
      for (assignment_list::const_iterator l=r.begin() ; l!=r.end() ; ++l)
      {
        if (occursinterm(var,l->rhs()))
        {
          return true;
        }
       assigned_variables.insert(l->lhs());
      }
      // Check whether x does not occur in the assignment list. Then variable x is assigned to
      // itself, and it occurs in the process.
      variable_list parameters=objectdata[objectIndex(proc_name)].parameters;
      for (variable_list::const_iterator i=parameters.begin(); i!=parameters.end(); ++i)
      {
        if (var==*i)
        {
          if (assigned_variables.count(var)==0) // This variable is not assigned, so it does occur!
          {
            return true;
          }
        }
      }
      return false;
    }

    template <typename Iterator>
    void filter_vars_by_termlist(
      Iterator begin,
      const Iterator& end,
      const std::set < variable >& vars_set,
      std::set < variable >& vars_result_set)
    {
      for (; begin != end; ++begin)
      {
        filter_vars_by_term(*begin,vars_set,vars_result_set);
      }
    }

    void filter_vars_by_multiaction(
      const action_list& multiaction,
      const std::set < variable >& vars_set,
      std::set < variable >& vars_result_set)
    {
      for (action_list::const_iterator ma=multiaction.begin() ; ma!=multiaction.end() ; ++ma)
      {
        filter_vars_by_termlist(ma->arguments().begin(), ma->arguments().end(),vars_set,vars_result_set);
      }
      return;
    }

    void filter_vars_by_assignmentlist(
      const assignment_list& assignments,
      const variable_list& parameters,
      const std::set < variable >& vars_set,
      std::set < variable >& vars_result_set)
    {
      const data_expression_list& l=atermpp::container_cast<data_expression_list>(parameters);
      filter_vars_by_termlist(l.begin(),l.end(),vars_set,vars_result_set);
      for (assignment_list::const_iterator i=assignments.begin();
           i!=assignments.end(); ++i)
      {
        const data_expression rhs=i->rhs();
        filter_vars_by_term(rhs,vars_set,vars_result_set);
      }
    }

    bool occursinpCRLterm(const variable var,
                          const process_expression p,
                          const bool strict)
    {
      if (is_choice(p))
      {
        return occursinpCRLterm(var,choice(p).left(),strict)||
               occursinpCRLterm(var,choice(p).right(),strict);
      }
      if (is_seq(p))
      {
        return occursinpCRLterm(var,seq(p).left(),strict)||
               occursinpCRLterm(var,seq(p).right(),strict);
      }
      if (is_if_then(p))
      {
        return occursinterm(var,if_then(p).condition())||
               occursinpCRLterm(var,if_then(p).then_case(),strict);
      }

      if (is_sum(p))
      {
        if (strict)
          return occursintermlist(var,data_expression_list(sum(p).variables()))||
                 occursinpCRLterm(var,sum(p).operand(),strict);
        /* below appears better? , but leads
           to errors. Should be investigated. */
        else
          return
            (!occursintermlist(var,data_expression_list(sum(p).variables()))) &&
            occursinpCRLterm(var,sum(p).operand(),strict);
      }
      if (is_stochastic_operator(p))
      {
        const stochastic_operator& sto=atermpp::down_cast<const stochastic_operator>(p);
        if (strict)
        {
          return occursintermlist(var,data_expression_list(sto.variables()))||
                      occursinterm(var,sto.distribution()) ||
                      occursinpCRLterm(var,sto.operand(),strict);
        }
        else
        {
          return (!occursintermlist(var,data_expression_list(sto.variables()))) &&
                      (occursinterm(var,sto.distribution()) ||
                       occursinpCRLterm(var,sto.operand(),strict));
        }
      }
      if (is_process_instance(p))
      {
        assert(0);
        return occursintermlist(var,process_instance(p).actual_parameters());
      }
      if (is_process_instance_assignment(p))
      {
        return occursintermlist(var,process_instance_assignment(p).assignments(),process_instance_assignment(p).identifier());
      }
      if (is_action(p))
      {
        return occursintermlist(var,action(p).arguments());
      }
      if (is_sync(p))
      {
        return occursinpCRLterm(var,process::sync(p).left(),strict)||
               occursinpCRLterm(var,process::sync(p).right(),strict);
      }
      if (is_at(p))
      {
        return occursinterm(var,at(p).time_stamp()) ||
               occursinpCRLterm(var,at(p).operand(),strict);
      }
      if (is_delta(p))
      {
        return false;
      }
      if (is_tau(p))
      {
        return false;
      }
      throw mcrl2::runtime_error("unexpected process format in occursinCRLterm " + process::pp(p));
      return false;
    }

    template <class MutableSubstitution>
    void alphaconvertprocess(
      variable_list& sumvars,
      MutableSubstitution& sigma,
      const process_expression p,
      std::set < variable >& lhs_variables_in_sigma)
    {
      /* This function replaces the variables in sumvars
         by unique ones if these variables occur in occurvars
         or occurterms. It extends rename_vars and rename
         terms to rename the replaced variables to new ones. */
      variable_list newsumvars;

      for (variable_list::const_iterator l=sumvars.begin() ;
           l!=sumvars.end() ; ++l)
      {
        const variable var=*l;
        if (occursinpCRLterm(var,p,true))
        {
          const variable newvar=get_fresh_variable(var.name(),var.sort());
          newsumvars.push_front(newvar);
          sigma[var]=newvar;
          lhs_variables_in_sigma.insert(newvar);
        }
        else
        {
          newsumvars.push_front(var);
        }
      }
      sumvars=reverse(newsumvars);
    }

    template <class MutableSubstitution>
    void alphaconvert(
      variable_list& sumvars,
      MutableSubstitution& sigma,
      const variable_list& occurvars,
      const data_expression_list& occurterms,
      std::set<variable>& variables_occurring_in_rhs_of_sigma)
    {
      /* This function replaces the variables in sumvars
         by unique ones if these variables occur in occurvars
         or occurterms. It extends rename_vars and rename
         terms to rename the replaced variables to new ones. */
      variable_list newsumvars;

      for (variable_list::const_iterator l=sumvars.begin() ; l!=sumvars.end() ; ++l)
      {
        const variable var= *l;
        if (occursintermlist(var,data_expression_list(occurvars)) ||
            occursintermlist(var,occurterms))
        {
          const variable newvar=get_fresh_variable(var.name(),var.sort());
          newsumvars.push_front(newvar);
          /* rename_vars.push_front(var);
             rename_terms.push_front(data_expression(newvar)); */
          sigma[var]=newvar;
          variables_occurring_in_rhs_of_sigma.insert(newvar);
        }
        else
        {
          newsumvars.push_front(var);
        }
      }
      sumvars=reverse(newsumvars);
    }

    /******************* find_free_variables_process *****************************************/


    /* We define our own variant of the standard function find_free_variables, because
       find_free_variables is not correctly defined on processes, due to process_instance_assignments,
       where variables can occur by not being mentioned. It is necessary to know the parameters of
       a process to retrieve these. Concrete example in P() defined by P(x:Nat)= ..., the variable x
       appears as a free variable, although it is not explicitly mentioned.
       If the standard function find_free_variable on processes is repaired, this function can
       be removed */
    void find_free_variables_process(const process_expression& p, std::set< variable >& free_variables_in_p)
    {
      if (is_choice(p))
      {
         find_free_variables_process(choice(p).left(),free_variables_in_p);
         find_free_variables_process(choice(p).right(),free_variables_in_p);
         return;
      }
      if (is_seq(p))
      {
        find_free_variables_process(seq(p).left(),free_variables_in_p);
        find_free_variables_process(seq(p).right(),free_variables_in_p);
        return;
      }
      if (is_sync(p))
      {
        find_free_variables_process(process::sync(p).left(),free_variables_in_p);
        find_free_variables_process(process::sync(p).right(),free_variables_in_p);
        return;
      }
      if (is_if_then(p))
      {
        std::set<variable> s=find_free_variables(if_then(p).condition());
        for(std::set<variable>::const_iterator i=s.begin(); i!=s.end(); ++i)
        {
          free_variables_in_p.insert(*i);
        }
        find_free_variables_process(if_then(p).then_case(),free_variables_in_p);
        return;
      }
      if (is_if_then_else(p))
      {
        std::set<variable> s=find_free_variables(if_then(p).condition());
        for(std::set<variable>::const_iterator i=s.begin(); i!=s.end(); ++i)
        {
          free_variables_in_p.insert(*i);
        }
        find_free_variables_process(if_then_else(p).then_case(),free_variables_in_p);
        find_free_variables_process(if_then_else(p).else_case(),free_variables_in_p);
        return;
      }

      if (is_sum(p))
      {
        find_free_variables_process(sum(p).operand(),free_variables_in_p);
        const variable_list& sumargs=sum(p).variables();

        for(variable_list::const_iterator i=sumargs.begin(); i!=sumargs.end(); ++i)
        {
          free_variables_in_p.erase(*i);
        }
        return;
      }

      if (is_stochastic_operator(p))
      {
        const stochastic_operator& sto=down_cast<const stochastic_operator>(p);
        find_free_variables_process(sto.operand(),free_variables_in_p);
        std::set<variable> s=find_free_variables(sto.distribution());
        for(std::set<variable>::const_iterator i=s.begin(); i!=s.end(); ++i)
        {
          free_variables_in_p.insert(*i);
        }

        const variable_list& sumargs=sto.variables();
        for(variable_list::const_iterator i=sumargs.begin(); i!=sumargs.end(); ++i)
        {
          free_variables_in_p.erase(*i);
        }
        return;
      }

      if (is_process_instance(p))
      {
        const process_instance q(p);
        std::set<variable> free_variables=find_free_variables(q.actual_parameters());
        for(std::set<variable> ::const_iterator i=free_variables.begin(); i!=free_variables.end(); ++i)
        {
          free_variables_in_p.insert(*i);
        }
        return;
      }
      if (is_process_instance_assignment(p))
      {
        const process_instance_assignment q(p);
        size_t n=objectIndex(q.identifier());
        const variable_list parameters=objectdata[n].parameters;
        std::set<variable> parameter_set(parameters.begin(),parameters.end());
        const assignment_list& assignments=q.assignments();
        for(assignment_list::const_iterator i=assignments.begin(); i!=assignments.end(); ++i)
        {
          std::set<variable> s=find_free_variables(i->rhs());
          for(std::set<variable>::const_iterator j=s.begin(); j!=s.end(); ++j)
          {
            free_variables_in_p.insert(*j);
          }
          parameter_set.erase(i->lhs());
        }
        // Add all remaining variables in the parameter_set, as they have an identity assignment.
        for(std::set<variable>::const_iterator i=parameter_set.begin(); i!=parameter_set.end(); ++i)
        {
          free_variables_in_p.insert(*i);
        }
        return;
      }

      if (is_action(p))
      {
        std::set<variable> s=process::find_free_variables(p);
        for(std::set<variable>::const_iterator i=s.begin(); i!=s.end(); ++i)
        {
          free_variables_in_p.insert(*i);
        }
        return;
      }

      if (is_at(p))
      {
        std::set<variable> s=data::find_free_variables(at(p).time_stamp());
        for(std::set<variable>::const_iterator i=s.begin(); i!=s.end(); ++i)
        {
          free_variables_in_p.insert(*i);
        }
        find_free_variables_process(at(p).operand(),free_variables_in_p);
        return;
      }

      if (is_delta(p))
      {
        return;
      }

      if (is_tau(p))
      {
        return;
      }

      if (is_sync(p))
      {
        find_free_variables_process(process::sync(p).left(),free_variables_in_p);
        find_free_variables_process(process::sync(p).right(),free_variables_in_p);
        return;
      }

      if (is_left_merge(p))
      {
        find_free_variables_process(process::left_merge(p).left(),free_variables_in_p);
        find_free_variables_process(process::left_merge(p).right(),free_variables_in_p);
        return;
      }

      if (is_merge(p))
      {
        find_free_variables_process(process::merge(p).left(),free_variables_in_p);
        find_free_variables_process(process::merge(p).right(),free_variables_in_p);
        return;
      }

      if (is_allow(p))
      {
        find_free_variables_process(process::allow(p).operand(),free_variables_in_p);
        return;
      }

      if (is_comm(p))
      {
        find_free_variables_process(process::comm(p).operand(),free_variables_in_p);
        return;
      }

      if (is_block(p))
      {
        find_free_variables_process(process::block(p).operand(),free_variables_in_p);
        return;
      }

      if (is_hide(p))
      {
        find_free_variables_process(process::hide(p).operand(),free_variables_in_p);
        return;
      }

      if (is_rename(p))
      {
        find_free_variables_process(process::rename(p).operand(),free_variables_in_p);
        return;
      }

      throw mcrl2::runtime_error("expected a pCRL process (1) " + process::pp(p));
    }

    std::set< variable > find_free_variables_process(const process_expression& p)
    {
      std::set<variable> free_variables_in_p;
      find_free_variables_process(p,free_variables_in_p);
      return free_variables_in_p;
    }
    /******************* substitute *****************************************/


    template <class Substitution>
    assignment_list substitute_assignmentlist(
      const assignment_list& assignments,
      const variable_list& parameters,
      const bool replacelhs,
      const bool replacerhs,
      Substitution& sigma,
      const std::set<variable>& variables_in_rhs_of_sigma)
    {
      /* precondition: the variables in the assignment occur in
         the same sequence as in the parameters, which stands for the
         total list of parameters.

         This function replaces the variables in vars by the terms in terms
         in the right hand side of the assignments if replacerhs holds, and
         in the lefthandside of an assignment if replacelhs holds. If for some variable
         occuring in the parameterlist no assignment is present, whereas
         this variable occurs in vars, an assignment for it is added.

         It is not possible to use standard substitution functions to replace substitute_assignmentlis
         because they do not take the parameters of processes into account.
         For instance consider a process P(b:D)=... of which an instance P() exists.
         If the substitution sigma(b)=t is applied to P() the result should be P(b=t).
         The standard substitutions do not take this parameterlist into account, as it stands.
      */
      assert(replacelhs==0 || replacelhs==1);
      assert(replacerhs==0 || replacerhs==1);

      if (parameters.empty())
      {
        assert(assignments.empty());
        return assignments;
      }

      variable parameter=parameters.front();

      if (!assignments.empty())
      {
        assignment ass=assignments.front();
        variable lhs=ass.lhs();
        if (parameter==lhs)
        {
          /* The assignment refers to parameter par. Substitute its
             left and righthandside */
          data_expression rhs=ass.rhs();

          if (replacelhs)
          {
            lhs = atermpp::down_cast<variable>(sigma(lhs));
          }
          if (replacerhs)
          {
            rhs=data::replace_variables_capture_avoiding(rhs,sigma,variables_in_rhs_of_sigma);
          }

          assignment_list result=
                   substitute_assignmentlist(
                     assignments.tail(),
                     parameters.tail(),
                     replacelhs,
                     replacerhs,
                     sigma,
                     variables_in_rhs_of_sigma);
          result.push_front(assignment(lhs,rhs));
          return result;
        }
      }

      /* Here the first parameter is not equal to the first
         assignment. So, we must find out whether a value
         for this variable is substituted, that is different
         from the variable, in which case an assignment must
         be added. */

      variable lhs=parameter;
      data_expression rhs=parameter;

      if (replacelhs)
      {
        lhs = atermpp::down_cast<data::variable>(sigma(lhs));
      }
      if (replacerhs)
      {
        rhs=data::replace_variables_capture_avoiding(rhs,sigma,variables_in_rhs_of_sigma);
      }

      if (lhs==rhs)
      {
        return substitute_assignmentlist(
                 assignments,
                 parameters.tail(),
                 replacelhs,
                 replacerhs,
                 sigma,
                 variables_in_rhs_of_sigma);
      }
      assignment_list result=
               substitute_assignmentlist(
                 assignments,
                 parameters.tail(),
                 replacelhs,
                 replacerhs,
                 sigma,
                 variables_in_rhs_of_sigma);
      result.push_front(assignment(lhs,rhs));
      return result;
    }

    // Sort the assignments, such that they have the same order as the parameters
    assignment_list sort_assignments(const assignment_list& ass, const variable_list parameters)
    {
      std::map<variable,data_expression>assignment_map;
      for(assignment_list::const_iterator i=ass.begin(); i!=ass.end(); ++i)
      {
        assignment_map[i->lhs()]=i->rhs();
      }

      assignment_vector result;
      for(variable_list::const_iterator i=parameters.begin(); i!=parameters.end(); ++i)
      {
        const std::map<variable,data_expression>::const_iterator j=assignment_map.find(*i);
        if (j!=assignment_map.end()) // found
        {
          result.push_back(assignment(j->first,j->second));
        }
      }
      return assignment_list(result.begin(),result.end());
    }

    bool check_valid_process_instance_assignment(
               const process_identifier& id,
               const assignment_list& assignments)
    {
      size_t n=objectIndex(id);
      variable_list parameters=objectdata[n].parameters;
      for(assignment_list::const_iterator i=assignments.begin(); i!=assignments.end(); ++i)
      {
        // Every assignment must occur in the parameter list, in the right sequence.

        variable v;
        do
        {
          if (parameters.empty())
          {
            return false;
          }
          v=parameters.front();
          parameters.pop_front();
        }
        while (v!=i->lhs());

      }
      return true;
    }

    /* The function below calculates sigma(p) and replaces
       all variables that are bound by a sum in p by unique
       variables */

    process_expression substitute_pCRLproc(
      const process_expression& p,
      mutable_map_substitution<>& sigma,
      const std::set< variable >& rhs_variables_in_sigma)
    {
      // mutable_map_substitution<> sigma_aux(sigma);
      // return process::replace_variables_capture_avoiding(p,sigma_aux,rhs_variables_in_sigma);
      if (is_choice(p))
      {
        return choice(
                 substitute_pCRLproc(choice(p).left(),sigma,rhs_variables_in_sigma),
                 substitute_pCRLproc(choice(p).right(),sigma,rhs_variables_in_sigma));
      }
      if (is_seq(p))
      {
        return seq(
                 substitute_pCRLproc(seq(p).left(),sigma,rhs_variables_in_sigma),
                 substitute_pCRLproc(seq(p).right(),sigma,rhs_variables_in_sigma));
      }
      if (is_sync(p))
      {
        return process::sync(
                 substitute_pCRLproc(process::sync(p).left(),sigma,rhs_variables_in_sigma),
                 substitute_pCRLproc(process::sync(p).right(),sigma,rhs_variables_in_sigma));
      }
      if (is_if_then(p))
      {
        data_expression condition=data::replace_variables_capture_avoiding(if_then(p).condition(), sigma,rhs_variables_in_sigma);
        if (condition==sort_bool::false_())
        {
          return delta_at_zero();
        }
        if (condition==sort_bool::true_())
        {
          return substitute_pCRLproc(if_then(p).then_case(),sigma,rhs_variables_in_sigma);
        }
        return if_then(condition,substitute_pCRLproc(if_then(p).then_case(),sigma,rhs_variables_in_sigma));
      }
      if (is_if_then_else(p))
      {
        data_expression condition=data::replace_variables_capture_avoiding(if_then_else(p).condition(), sigma,rhs_variables_in_sigma);
        if (condition==sort_bool::false_())
        {
          return substitute_pCRLproc(if_then_else(p).else_case(),sigma,rhs_variables_in_sigma);
        }
        if (condition==sort_bool::true_())
        {
          return substitute_pCRLproc(if_then_else(p).then_case(),sigma,rhs_variables_in_sigma);
        }
        return if_then_else(
                 condition,
                 substitute_pCRLproc(if_then_else(p).then_case(),sigma,rhs_variables_in_sigma),
                 substitute_pCRLproc(if_then_else(p).else_case(),sigma,rhs_variables_in_sigma));
      }

      if (is_sum(p))
      {
        variable_list sumargs=sum(p).variables();
        variable_list vars;
        data_expression_list terms;

        for( std::map < variable, data_expression >::const_iterator i=sigma.begin(); i!=sigma.end(); ++i)
        {
          vars=push_back(vars,i->first);
          terms=push_back(terms,i->second);
        }

        mutable_map_substitution<> local_sigma=sigma;
        std::set<variable> local_rhs_variables_in_sigma=rhs_variables_in_sigma;
        alphaconvert(sumargs,local_sigma,vars,terms,local_rhs_variables_in_sigma);

        return sum(sumargs,
                   substitute_pCRLproc(sum(p).operand(),local_sigma,local_rhs_variables_in_sigma));
      }

      if (is_stochastic_operator(p))
      {
        const stochastic_operator& sto=down_cast<const stochastic_operator>(p);
        variable_list sumargs=sto.variables();
        variable_list vars;
        data_expression_list terms;

        for( std::map < variable, data_expression >::const_iterator i=sigma.begin(); i!=sigma.end(); ++i)
        {
          vars=push_back(vars,i->first);
          terms=push_back(terms,i->second);
        }

        mutable_map_substitution<> local_sigma=sigma;
        std::set<variable> local_rhs_variables_in_sigma=rhs_variables_in_sigma;
        alphaconvert(sumargs,local_sigma,vars,terms,local_rhs_variables_in_sigma);

        return stochastic_operator(
                            sumargs,
                            data::replace_variables_capture_avoiding(sto.distribution(),sigma,rhs_variables_in_sigma),
                            substitute_pCRLproc(sto.operand(),local_sigma,local_rhs_variables_in_sigma));
      }

      if (is_process_instance(p))
      {
        assert(0);
      }

      if (is_process_instance_assignment(p))
      {
        const process_instance_assignment q(p);
        size_t n=objectIndex(q.identifier());
        const variable_list parameters=objectdata[n].parameters;
        const assignment_list new_assignments=substitute_assignmentlist(q.assignments(),parameters,false,true,sigma,rhs_variables_in_sigma);
        assert(check_valid_process_instance_assignment(q.identifier(),new_assignments));
        return process_instance_assignment(q.identifier(),new_assignments);
      }

      if (is_action(p))
      {
        return action(action(p).label(),
                      data::replace_variables_capture_avoiding(action(p).arguments(), sigma,rhs_variables_in_sigma));
      }

      if (is_at(p))
      {
        return at(substitute_pCRLproc(at(p).operand(),sigma,rhs_variables_in_sigma),
                  data::replace_variables_capture_avoiding(at(p).time_stamp(),sigma,rhs_variables_in_sigma));
      }

      if (is_delta(p))
      {
        return p;
      }

      if (is_tau(p))
      {
        return p;
      }

      if (is_sync(p))
      {
        return process::sync(
                 substitute_pCRLproc(process::sync(p).left(),sigma,rhs_variables_in_sigma),
                 substitute_pCRLproc(process::sync(p).right(),sigma,rhs_variables_in_sigma));
      }

      throw mcrl2::runtime_error("expected a pCRL process (2) " + process::pp(p));
      return process_expression();
    }


    // The function below transforms a ProcessAssignment to a Process, provided
    // that the process is defined in objectnames.

    process_instance_assignment transform_process_instance_to_process_instance_assignment(
              const process_instance& procId,
              const std::set<variable>& bound_variables=std::set<variable>())
    {
      size_t n=objectIndex(procId.identifier());
      const variable_list process_parameters=objectdata[n].parameters;
      const data_expression_list rhss=procId.actual_parameters();

      assignment_vector new_assignments;
      data_expression_list::const_iterator j=rhss.begin();
      for(variable_list::const_iterator i=process_parameters.begin(); i!=process_parameters.end(); ++i, ++j)
      {
        assert(j!=rhss.end());
        if (*i==*j)
        {
          if (bound_variables.count(*i)>0) // Now *j is a different variable than *i
          {
            new_assignments.push_back(assignment(*i,*j));
          }
        }
        else
        {
          new_assignments.push_back(assignment(*i,*j));
        }
      }
      assert(j==rhss.end());

      assert(check_valid_process_instance_assignment(procId.identifier(),assignment_list(new_assignments.begin(),new_assignments.end())));
      process_instance_assignment p(procId.identifier(), assignment_list(new_assignments.begin(),new_assignments.end()));
      return p;
    }

    /********************************************************************/
    /*                                                                  */
    /*   BELOW THE PROCEDURES ARE GIVEN TO TRANSFORM PROCESSES TO       */
    /*   LINEAR PROCESSES.                                              */
    /*                                                                  */
    /*                                                                  */
    /*                                                                  */
    /********************************************************************/

    typedef enum { first, later } variableposition;

    /****************  tovarheadGNF  *********************************/

    variable_list parameters_that_occur_in_body(
      const variable_list& parameters,
      const process_expression& body)
    {
      if (parameters.empty())
      {
        return parameters;
      }

      variable_list parameters1=parameters_that_occur_in_body(parameters.tail(),body);
      if (occursinpCRLterm(parameters.front(),body,false))
      {

        parameters1.push_front(parameters.front());
      }
      return parameters1;
    }

    // The variable below is used to count the number of new processes that
    // are made. If this number is very high, it is likely that the regular
    // flag is used, and an unbounded number of new processes are generated.
    // In such a case a warning is printed suggesting to use regular2.

    process_identifier newprocess(
      const variable_list parameters,  // Intentionally not a reference.
      const process_expression body,   // Intentionally not a reference.
      const processstatustype ps,
      const bool canterminate,
      const bool containstime)
    {
      assert(canterminatebody(body)==canterminate);
      assert(containstimebody(body)==containstime);

      process_identifier p1;
      if (searchProcDeclaration(parameters,body,ps, canterminate,containstime,p1))
      {
        return p1;  // The process did already exist. No need to make a new one.
      }

      static size_t numberOfNewProcesses=0, warningNumber=25;
      numberOfNewProcesses++;
      if (numberOfNewProcesses == warningNumber)
      {
        mCRL2log(mcrl2::log::warning) << "Generated " << numberOfNewProcesses << " new internal processes.";

        if (options.lin_method==lmRegular)
        {
          mCRL2log(mcrl2::log::warning) << " A possible unbounded loop can be avoided by using `regular2' or `stack' as linearisation method." << std::endl;
        }
        else if (options.lin_method==lmRegular2)
        {
          mCRL2log(mcrl2::log::warning) << " A possible unbounded loop can be avoided by using `stack' as the linearisation method." << std::endl;
        }
        else
        {
          mCRL2log(mcrl2::log::warning) << std::endl;
        }
        warningNumber=warningNumber*5;
      }
      const variable_list parameters1=parameters_that_occur_in_body(parameters, body);
      const core::identifier_string s=fresh_identifier_generator("P");
      const process_identifier p(s, parameters1);
      assert(std::string(p.name()).size()>0);
      insertProcDeclaration(
        p,
        parameters1,
        body,
        ps,
        canterminate,
        containstime);
      return p;
    }


    process_expression wraptime(
      const process_expression body,
      const data_expression time,
      const variable_list freevars)
    {
      if (is_choice(body))
      {
        return choice(
                 wraptime(choice(body).left(),time,freevars),
                 wraptime(choice(body).right(),time,freevars));
      }

      if (is_sum(body))
      {
        variable_list sumvars=sum(body).variables();
        process_expression body1=sum(body).operand();

        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        alphaconvert(sumvars,sigma,freevars,data_expression_list(),variables_occurring_in_rhs_of_sigma);
        body1=substitute_pCRLproc(body1, sigma,variables_occurring_in_rhs_of_sigma);
        mutable_map_substitution<> sigma_aux(sigma);
        const data_expression time1=data::replace_variables_capture_avoiding(time, sigma_aux,variables_occurring_in_rhs_of_sigma);
        body1=wraptime(body1,time1,sumvars+freevars);
        return sum(sumvars,body1);
      }

      if (is_stochastic_operator(body))
      {
        const stochastic_operator& sto=down_cast<const stochastic_operator>(body);
        variable_list sumvars=sto.variables();
        process_expression body1=sto.operand();

        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        alphaconvert(sumvars,sigma,freevars,data_expression_list(),variables_occurring_in_rhs_of_sigma);
        body1=substitute_pCRLproc(body1, sigma,variables_occurring_in_rhs_of_sigma);
        const data_expression new_distribution=data::replace_variables_capture_avoiding(sto.distribution(), sigma,variables_occurring_in_rhs_of_sigma);
        mutable_map_substitution<> sigma_aux(sigma);
        const data_expression time1=data::replace_variables_capture_avoiding(time, sigma_aux,variables_occurring_in_rhs_of_sigma);
        body1=wraptime(body1,time1,sumvars+freevars);
        return stochastic_operator(sumvars,new_distribution,body1);
      }

      if (is_if_then(body))
      {
        return if_then(if_then(body).condition(),wraptime(if_then(body).then_case(),time,freevars));
      }

      if (is_seq(body))
      {
        return seq(wraptime(seq(body).left(),time,freevars),seq(body).right());
      }

      if (is_at(body))
      {
        /* make a new process */
        const process_identifier newproc=newprocess(freevars,body,pCRL,
                                         canterminatebody(body),containstimebody(body));
        assert(check_valid_process_instance_assignment(newproc,assignment_list()));
        return at(process_instance_assignment(
                    newproc,
                    assignment_list()),  //data::data_expression_list(objectdata[objectIndex(newproc)].parameters)),
                  time);
      }

      if (// (is_process_instance(body))||
          (is_process_instance_assignment(body))||
          (is_sync(body))||
          (is_action(body))||
          (is_tau(body))||
          (is_delta(body)))
      {
        return at(body,time);
      }

      throw mcrl2::runtime_error("expected pCRL process in wraptime " + process::pp(body));
      return process_expression();
    }

    typedef enum { alt_state, sum_state, /* cond,*/ seq_state, name_state, multiaction_state } state;

    variable get_fresh_variable(const std::string& s, const sort_expression sort, const int reuse_index=-1)
    {
      /* If reuse_index is smaller than 0 (-1 is the default value), an unused variable name is returned,
         based on the string s with sort `sort'. If reuse_index is larger or equal to
         0 the reuse_index+1 generated variable is returned. If for a particular reuse_index
         this function is called for the first time, it is guaranteed that the returned
         variable is unique, and not used as variable elsewhere. Upon subsequent calls
         get_fresh_variable will return the same variable for the same s,sort and reuse_triple.
         This feature is added to make it possible to avoid generating too many different variables. */

      std::map < int , std::map < variable,variable > > generated_variables;

      if (reuse_index<0)
      {
        variable v(fresh_identifier_generator(s),sort);
        insertvariable(v,true);
        return v;
      }
      else
      {
        variable table_index_term(s,sort);
        variable old_variable;
        if (generated_variables[reuse_index].count(table_index_term)>0)
        {
          old_variable=generated_variables[reuse_index][table_index_term];
        }
        else
        {
          /* A new variable must be generated */
          old_variable=get_fresh_variable(s,sort);
          generated_variables[reuse_index][table_index_term]=old_variable;
        }
        return old_variable;
      }
    }

    variable_list make_pars(const sort_expression_list sortlist)
    {
      /* this function returns a list of variables,
         corresponding to the sorts in sortlist */

      if (sortlist.empty())
      {
        return variable_list();
      }

      sort_expression sort=sortlist.front();

      variable_list result=make_pars(sortlist.tail());
      result.push_front(get_fresh_variable("a",sort));
      return result;
    }

    process_expression distributeActionOverConditions(
      const process_expression act, // This is a multi-action, actually.
      const data_expression condition,
      const process_expression restterm,
      const variable_list freevars,
      const std::set<variable>& variables_bound_in_sum)
    {
      if (is_if_then(restterm))
      {
        /* Here we check whether the process body has the form
           a (c -> x). For state space generation it turns out
           to be beneficial to rewrite this to c-> a x + !c -> a.delta@0, as in
           certain cases this leads to a reduction of the number
           of states. In this code, we recursively check whether
           the action must be distributed over x. This optimisation
           was observed by Yaroslav Usenko, May 2006. Implemented by JFG.
           On industrial examples, it appears to reduce the state space
           with a factor up to 2.
           Before october 2008 this code was wrong, as it transformed
           an expression to c-> a x, ommitting the a.delta@0. */

        const data_expression c=if_then(restterm).condition();
        const process_expression r=choice(
                                     distributeActionOverConditions(
                                       act,
                                       lazy::and_(condition,c),
                                       if_then(restterm).then_case(),
                                       freevars,variables_bound_in_sum),
                                     distributeActionOverConditions(
                                       act,
                                       lazy::and_(condition,lazy::not_(c)),
                                       delta_at_zero(),
                                       freevars,variables_bound_in_sum));
        return r;
      }
      if (is_if_then_else(restterm))
      {
        /* Here we check whether the process body has the form
           a (c -> x <> y). For state space generation it turns out
           to be beneficial to rewrite this to c-> a x + !c -> a y, as in
           certain cases this leads to a reduction of the number
           of states, despite the duplication of the a action. In this code,
           we recursively check whether the action must be distributed over
           x and y. This optimisation
           was observed by Yaroslav Usenko, May 2006. Implemented by JFG.
           On industrial examples, it appears to reduce the state space
           with a factor up to 2. */

        const data_expression c=if_then_else(restterm).condition();
        const process_expression r=choice(
                                     distributeActionOverConditions(
                                       act,
                                       lazy::and_(condition,c),
                                       if_then_else(restterm).then_case(),
                                       freevars,variables_bound_in_sum),
                                     distributeActionOverConditions(
                                       act,
                                       lazy::and_(condition,lazy::not_(c)),
                                       if_then_else(restterm).else_case(),
                                       freevars,variables_bound_in_sum));
        return r;
      }
      const process_expression restterm1=bodytovarheadGNF(restterm,seq_state,freevars,later,variables_bound_in_sum);
      return if_then(condition,seq(act,restterm1));
    }


   assignment_list parameters_to_assignment_list(const variable_list parameters, const std::set<variable>& variables_bound_in_sum)
   {
     assignment_vector result;
     for(variable_list::const_iterator i=parameters.begin(); i!=parameters.end(); ++i)
     {
       if (variables_bound_in_sum.count(*i)>0)
       {
         result.push_back(assignment(*i,*i)); // rhs is another variable than the lhs!!
       }
     }
     return assignment_list(result.begin(),result.end());
   }


    process_expression bodytovarheadGNF(
      const process_expression body, // intentionally not a reference.
      const state s,
      const variable_list freevars, // intentionally not a reference.
      const variableposition v,
      const std::set<variable>& variables_bound_in_sum)
    {
      /* it is assumed that we only receive processes with
         operators alt, seq, sum_state, cond, name, delta, tau, sync, at and stochastic operator in it */

      if (is_choice(body))
      {
        if (alt_state>=s)
        {
          const process_expression body1=bodytovarheadGNF(choice(body).left(),alt_state,freevars,first,variables_bound_in_sum);
          const process_expression body2=bodytovarheadGNF(choice(body).right(),alt_state,freevars,first,variables_bound_in_sum);
          if (isDeltaAtZero(body1))
          {
            return body2;
          }
          if (isDeltaAtZero(body2))
          {
            return body1;
          }
          return choice(body1,body2);
        }
        const process_expression body1=bodytovarheadGNF(body,alt_state,freevars,first,variables_bound_in_sum);
        const process_identifier newproc=newprocess(freevars,body1,pCRL,
                                         canterminatebody(body1),
                                         containstimebody(body1));
        assert(check_valid_process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum)));
        return process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum));
      }

      if (is_sum(body))
      {
        if (sum_state>=s)
        {
          variable_list sumvars=sum(body).variables();
          process_expression body1=sum(body).operand();

          mutable_map_substitution<> sigma;
          std::set<variable> variables_occurring_in_rhs_of_sigma;
          alphaconvert(sumvars,sigma,freevars,data_expression_list(),variables_occurring_in_rhs_of_sigma);
          body1=substitute_pCRLproc(body1,sigma,variables_occurring_in_rhs_of_sigma);
          std::set<variable> variables_bound_in_sum1=variables_bound_in_sum;
          variables_bound_in_sum1.insert(sumvars.begin(),sumvars.end());
          body1=bodytovarheadGNF(body1,sum_state,sumvars+freevars,first,variables_bound_in_sum1);
          /* Due to the optimisation below, suggested by Yaroslav Usenko, bodytovarheadGNF(...,sum_state,...)
             can deliver a process of the form c -> x + !c -> y. In this case, the
             sumvars must be distributed over both summands. */
          if (is_choice(body1))
          {
            return choice(sum(sumvars,choice(body1).left()),
                          sum(sumvars,choice(body1).right()));
          }
          return sum(sumvars,body1);
        }
        const process_expression body1=bodytovarheadGNF(body,alt_state,freevars,first,variables_bound_in_sum);
        const process_identifier newproc=newprocess(freevars,body1,pCRL,
                                         canterminatebody(body1),
                                         containstimebody(body1));
        assert(check_valid_process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum)));
        return process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum));
      }

      if (is_stochastic_operator(body))
      {
        if (seq_state>=s)
        {
          const stochastic_operator& sto=down_cast<const stochastic_operator>(body);
          variable_list sumvars=sto.variables();
          data_expression distribution=sto.distribution();
          process_expression body1=sto.operand();

          mutable_map_substitution<> sigma;
          std::set<variable> variables_occurring_in_rhs_of_sigma;
          alphaconvert(sumvars,sigma,freevars,data_expression_list(),variables_occurring_in_rhs_of_sigma);
          distribution=data::replace_variables_capture_avoiding(distribution,sigma,variables_occurring_in_rhs_of_sigma);
          body1=substitute_pCRLproc(body1,sigma,variables_occurring_in_rhs_of_sigma);
          std::set<variable> variables_bound_in_sum1=variables_bound_in_sum;
          variables_bound_in_sum1.insert(sumvars.begin(),sumvars.end());
          body1=bodytovarheadGNF(body1,seq_state,sumvars+freevars,v,variables_bound_in_sum1);
          /* Due to the optimisation below, suggested by Yaroslav Usenko, bodytovarheadGNF(...,sum_state,...)
             can deliver a process of the form c -> x + !c -> y. In this case, the
             sumvars must be distributed over both summands. */
          return stochastic_operator(sumvars,distribution,body1);
        }
        const process_expression body1=bodytovarheadGNF(body,seq_state,freevars,first,variables_bound_in_sum);
        const process_identifier newproc=newprocess(freevars,body1,pCRL,
                                         canterminatebody(body1),
                                         containstimebody(body1));
        assert(check_valid_process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum)));
        return process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum));
      }

      if (is_if_then(body))
      {
        const data_expression condition=if_then(body).condition();
        const process_expression body1=if_then(body).then_case();

        if (s<=sum_state)
        {
          return if_then(
                   condition,
                   bodytovarheadGNF(body1,seq_state,freevars,first,variables_bound_in_sum));
        }
        const process_expression body2=bodytovarheadGNF(body,alt_state,freevars,first,variables_bound_in_sum);
        const process_identifier newproc=newprocess(freevars,body2,pCRL,
                                         canterminatebody(body2),
                                         containstimebody(body2));
        assert(check_valid_process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum)));
        return process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum));

      }

      if (is_if_then_else(body))
      {
        const data_expression condition=data_expression(if_then_else(body).condition());
        const process_expression body1=if_then_else(body).then_case();
        const process_expression body2=if_then_else(body).else_case();

        if (isDeltaAtZero(body1) && isDeltaAtZero(body2))
        {
          return body1;
        }

        if ((s<=sum_state) && ((isDeltaAtZero(body1))||(isDeltaAtZero(body2))))
        {
          if (isDeltaAtZero(body2))
          {
            return if_then(
                     condition,
                     bodytovarheadGNF(body1,seq_state,freevars,first,variables_bound_in_sum));
          }
          /* body1=="Delta@0" */
          {
            return if_then(
                     lazy::not_(condition),
                     bodytovarheadGNF(body2,seq_state,freevars,first,variables_bound_in_sum));
          }
        }
        if (alt_state==s) /* body1!=Delta@0 and body2!=Delta@0 */
        {
          return
            choice(
              if_then(
                condition,
                bodytovarheadGNF(body1,seq_state,freevars,first,variables_bound_in_sum)),
              if_then(
                lazy::not_(condition),
                bodytovarheadGNF(body2,seq_state,freevars,first,variables_bound_in_sum)));
        }
        const process_expression body3=bodytovarheadGNF(body,alt_state,freevars,first,variables_bound_in_sum);
        const process_identifier newproc=newprocess(freevars,body3,pCRL,
                                         canterminatebody(body3),
                                         containstimebody(body3));
        assert(check_valid_process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum)));
        return process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum));

      }

      if (is_seq(body))
      {
        process_expression body1=seq(body).left();
        process_expression body2=seq(body).right();

        if (s<=seq_state)
        {
          body1=bodytovarheadGNF(body1,name_state,freevars,v,variables_bound_in_sum);
          if (!canterminatebody(body1))
          {
            /* In this case there is no need to investigate body2, as it cannot be reached. */
            return body1;
          }
          if ((is_if_then(body2)) && (s<=sum_state))
          {
            /* Here we check whether the process body has the form
               a (c -> x) + !c -> delta@0. For state space generation it turns out
               to be beneficial to rewrite this to c-> a x, as in
               certain cases this leads to a reduction of the number
               of states. An extra change (24/12/2006) is that the
               conditions are distributed recursively over
               all conditions. The optimisation
               was observed by Yaroslav Usenko, May 2006. Implemented by JFG.
               On industrial examples, it appears to reduce the state space
               with a factor up to 2. Until 1/11/2008 this code was incorrect,
               because the summand a (!c -> delta@0) was not forgotten.*/

            const data_expression c(if_then(body2).condition());

            const process_expression r= choice(
                                          distributeActionOverConditions(body1,c,if_then(body2).then_case(),freevars,variables_bound_in_sum),
                                          distributeActionOverConditions(body1,lazy::not_(c),delta_at_zero(),freevars,variables_bound_in_sum));
            return r;
          }
          if ((is_if_then_else(body2)) && (s<=sum_state))
          {

            /* Here we check whether the process body has the form
               a (c -> x <> y). For state space generation it turns out
               to be beneficial to rewrite this to c-> a x + !c -> a y, as in
               certain cases this leads to a reduction of the number
               of states, despite the duplication of the a action. An extra
               change (24/12/2006) is that the conditions are distributed recursively over
               all conditions. The optimisation
               was observed by Yaroslav Usenko, May 2006. Implemented by JFG.
               On industrial examples, it appears to reduce the state space
               with a factor up to 2. */


            const data_expression c(if_then_else(body2).condition());
            const process_expression r= choice(
                                          distributeActionOverConditions(body1,c,if_then_else(body2).then_case(),freevars,variables_bound_in_sum),
                                          distributeActionOverConditions(body1,lazy::not_(c),if_then_else(body2).else_case(),freevars,variables_bound_in_sum));
            return r;
          }
          body2=bodytovarheadGNF(body2,seq_state,freevars,later,variables_bound_in_sum);
          return seq(body1,body2);
        }
        body1=bodytovarheadGNF(body,alt_state,freevars,first,variables_bound_in_sum);
        const process_identifier newproc=newprocess(freevars,body1,pCRL,canterminatebody(body1),
                                         containstimebody(body1));

        assert(check_valid_process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum)));
        return process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum));
      }

      if (is_action(body))
      {
        if ((s==multiaction_state)||(v==first))
        {
          return body;
        }

        bool isnew=false;
        size_t n=addMultiAction(action(body),isnew);

        if (objectdata[n].process_representing_action==process_identifier())
        {
          /* this action does not yet have a corresponding process, which
             must be constructed. The resulting process is stored in
             the variable process_representing_action in objectdata. Tempvar below is
             needed as objectdata may be realloced as a side effect
             of newprocess */
          const process_identifier tempvar=newprocess(
                                             objectdata[n].parameters,
                                             objectdata[n].processbody,
                                             GNF,true,false);
          objectdata[n].process_representing_action=tempvar;
        }
        return transform_process_instance_to_process_instance_assignment(
                          process_instance(objectdata[n].process_representing_action,
                          action(body).arguments()));
      }

      if (is_sync(body))
      {
        bool isnew=false;
        const process_expression body1=process::sync(body).left();
        const process_expression body2=process::sync(body).right();
        const action_list ma=linMergeMultiActionListProcess(
                               bodytovarheadGNF(body1,multiaction_state,freevars,v,variables_bound_in_sum),
                               bodytovarheadGNF(body2,multiaction_state,freevars,v,variables_bound_in_sum));

        const process_expression mp=action_list_to_process(ma);
        if ((s==multiaction_state)||(v==first))
        {
          return mp;
        }

        size_t n=addMultiAction(mp,isnew);

        if (objectdata[n].process_representing_action==process_identifier())
        {
          /* this action does not yet have a corresponding process, which
             must be constructed. The resulting process is stored in
             the variable process_representing_action in objectdata. Tempvar below is needed
             as objectdata may be realloced as a side effect of newprocess */
          process_identifier tempvar=newprocess(
                                       objectdata[n].parameters,
                                       objectdata[n].processbody,
                                       GNF,true,false);
          objectdata[n].process_representing_action=tempvar;
        }
        return transform_process_instance_to_process_instance_assignment(
                      process_instance(
                           process_identifier(objectdata[n].process_representing_action),
                           getarguments(ma)));
      }

      if (is_at(body))
      {
        process_expression body1=bodytovarheadGNF(
                                   at(body).operand(),
                                   s,
                                   freevars,
                                   first,variables_bound_in_sum);
        data_expression time=data_expression(at(body).time_stamp());
        /* put the time operator around the first action or process */
        body1=wraptime(body1,time,freevars);
        if (v==first)
        {
          return body1;
        }

        /* make a new process, containing this process */
        const process_identifier newproc=newprocess(freevars,body1,pCRL,
                                         canterminatebody(body1),
                                         containstimebody(body1));
        assert(check_valid_process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum)));
        return process_instance_assignment(newproc,parameters_to_assignment_list(objectdata[objectIndex(newproc)].parameters,variables_bound_in_sum));
      }

      if (is_process_instance(body))
      {
        assert(0);
        // return body;
        return transform_process_instance_to_process_instance_assignment(atermpp::down_cast<process_instance>(body));
      }

      if (is_process_instance_assignment(body))
      {
        // return transform_process_assignment_to_process(body);
        return body;
      }


      if (is_tau(body))
      {
        if (v==first)
        {
          return tau();
        }
        assert(check_valid_process_instance_assignment(tau_process,assignment_list()));
        return process_instance_assignment(tau_process,assignment_list());
      }

      if (is_delta(body))
      {
        if (v==first)
        {
          return body;
        }
        assert(check_valid_process_instance_assignment(delta_process,assignment_list()));
        return process_instance_assignment(delta_process,assignment_list());
      }

      throw mcrl2::runtime_error("unexpected process format in bodytovarheadGNF " + process::pp(body) +".");
      return process_expression();
    }

    void procstovarheadGNF(const std::vector < process_identifier>& procs)
    {
      /* transform the processes in procs into newprocs */
      for (const process_identifier& i:procs)
      {
        size_t n=objectIndex(i);

        // The intermediate variable result is needed here
        // because objectdata can be realloced as a side
        // effect of bodytovarheadGNF.

        std::set<variable> variables_bound_in_sum;
        const process_expression result=
          bodytovarheadGNF(
            objectdata[n].processbody,
            alt_state,
            objectdata[n].parameters,
            first,
            variables_bound_in_sum);
        objectdata[n].processbody=result;
      }
    }

    /**************** towards real GREIBACH normal form **************/

    typedef enum {terminating,infinite} terminationstatus;

    process_expression putbehind(const process_expression body1, const process_expression body2)
    {
      if (is_choice(body1))
      {
        return choice(
                 putbehind(choice(body1).left(),body2),
                 putbehind(choice(body1).right(),body2));
      }

      if (is_seq(body1))
      {
        return seq(seq(body1).left(), putbehind(seq(body1).right(),body2));
      }

      if (is_if_then(body1))
      {
        return if_then(if_then(body1).condition(),putbehind(if_then(body1).then_case(),body2));
      }

      if (is_sum(body1))
      {
        /* we must take care that no variables in body2 are
            inadvertently bound */
        variable_list sumvars=sum(body1).variables();

        mutable_map_substitution<> sigma;
        std::set < variable > lhs_variables_in_sigma;
        alphaconvertprocess(sumvars,sigma,body2,lhs_variables_in_sigma);
        return sum(sumvars,
                   putbehind(substitute_pCRLproc(sum(body1).operand(), sigma,lhs_variables_in_sigma),
                             body2));
      }

      if (is_stochastic_operator(body1))
      {
        const stochastic_operator& sto=atermpp::down_cast<stochastic_operator>(body1);
        variable_list stochvars=sto.variables();

        // See explanation at sum operator above.
        mutable_map_substitution<> sigma;
        std::set < variable > lhs_variables_in_sigma;
        alphaconvertprocess(stochvars,sigma,body2,lhs_variables_in_sigma);
        return stochastic_operator(
                 stochvars,
                 sto.distribution(),
                 putbehind(
                       substitute_pCRLproc(sto.operand(),sigma,lhs_variables_in_sigma),
                       body2));
      }


      if (is_action(body1))
      {
        return seq(body1,body2);
      }

      if (is_sync(body1))
      {
        return seq(body1,body2);
      }

      if (is_process_instance_assignment(body1))
      {
        return seq(body1,body2);
      }

      if (is_delta(body1))
      {
        return body1;
      }

      if (is_tau(body1))
      {
        return seq(body1,body2);
        // throw mcrl2::runtime_error("Expect only multiactions, not a tau.");
      }

      if (is_at(body1))
      {
        return seq(body1,body2);
      }

      throw mcrl2::runtime_error("Internal error. Unexpected process format in putbehind " + process::pp(body1) +".");
      return process_expression();
    }

    process_expression distribute_condition(
      const process_expression body1,
      const data_expression condition)
    {
      if (is_choice(body1))
      {
        return choice(
                 distribute_condition(choice(body1).left(),condition),
                 distribute_condition(choice(body1).right(),condition));
      }

      if (is_seq(body1))
      {
        return if_then(condition,body1);
      }

      if (is_if_then(body1))
      {
        return if_then(
                 lazy::and_(if_then(body1).condition(),condition),
                 if_then(body1).then_case());
      }

      if (is_sum(body1))
      {
        /* we must take care that no variables in condition are
            inadvertently bound */
        variable_list sumvars=sum(body1).variables();
        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        alphaconvert(sumvars,sigma,variable_list(), { condition },variables_occurring_in_rhs_of_sigma);
        return sum(
                 sumvars,
                 distribute_condition(
                   substitute_pCRLproc(sum(body1).operand(),sigma,variables_occurring_in_rhs_of_sigma),
                   condition));
      }

      if (is_stochastic_operator(body1))
      {
        /* we must take care that no variables in condition are
            inadvertently bound */
        const stochastic_operator& sto=atermpp::down_cast<stochastic_operator>(body1);
        variable_list stochvars=sto.variables();
        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        alphaconvert(stochvars,sigma,variable_list(), { condition },variables_occurring_in_rhs_of_sigma);
        return stochastic_operator(
                 stochvars,
                 data::replace_variables_capture_avoiding(
                                               sto.distribution(),
                                               sigma,
                                               variables_occurring_in_rhs_of_sigma),
                 distribute_condition(
                       substitute_pCRLproc(sto.operand(),sigma,variables_occurring_in_rhs_of_sigma),
                       condition));
      }

      if (is_at(body1)||
          is_action(body1)||
          is_sync(body1)||
          is_process_instance_assignment(body1)||
          is_delta(body1)||
          is_tau(body1))
      {
        return if_then(condition,body1);
      }

      throw mcrl2::runtime_error("Internal error. Unexpected process format in distribute condition " + process::pp(body1) +".");
    }

    process_expression distribute_sum(
      const variable_list sumvars,
      const process_expression body1)
    {
      if (is_choice(body1))
      {
        return choice(
                 distribute_sum(sumvars,choice(body1).left()),
                 distribute_sum(sumvars,choice(body1).right()));
      }

      if (is_seq(body1)||
          is_if_then(body1)||
          is_sync(body1)||
          is_action(body1)||
          is_tau(body1)||
          is_at(body1)||
          is_process_instance_assignment(body1)||
          isDeltaAtZero(body1))
      {
        return sum(sumvars,body1);
      }

      if (is_sum(body1))
      {
        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        variable_list inner_sumvars=sum(body1).variables();
        alphaconvert(inner_sumvars,sigma,sumvars,data_expression_list(),variables_occurring_in_rhs_of_sigma);
        const process_expression new_body1=substitute_pCRLproc(sum(body1).operand(), sigma, variables_occurring_in_rhs_of_sigma);
        return sum(sumvars+inner_sumvars,new_body1);
      }

      if (is_stochastic_operator(body1))
      {
        const stochastic_operator& sto=atermpp::down_cast<stochastic_operator>(body1);
        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        variable_list inner_sumvars=sto.variables();
        alphaconvert(inner_sumvars,sigma,sumvars,data_expression_list(),variables_occurring_in_rhs_of_sigma);
        const process_expression new_body1=substitute_pCRLproc(sto.operand(), sigma, variables_occurring_in_rhs_of_sigma);
        const data_expression new_distribution=data::replace_variables_capture_avoiding(sto.distribution(), sigma, variables_occurring_in_rhs_of_sigma);
        return stochastic_operator(sumvars+inner_sumvars,new_distribution,new_body1);
      }

      if (is_delta(body1)||
          is_tau(body1))
      {
        return body1;
      }

      throw mcrl2::runtime_error("Internal error. Unexpected process format in distribute_sum " + process::pp(body1) +".");
      return process_expression();
    }

    int match_sequence(
      const std::vector < process_instance_assignment >& s1,
      const std::vector < process_instance_assignment >& s2,
      const bool regular2)
    {
      /* s1 and s2 are sequences of typed variables of
         the form Process(ProcVarId("P2",[SortId("Bit"),
         SortId("Bit")]),[OpId("b1",SortId("Bit")),OpId("b1",SortId("Bit"))]).
         This function yields true if the names and types of
         the processes in s1 and s2 match. */

      std::vector < process_instance_assignment >::const_iterator i2=s2.begin();
      for (std::vector < process_instance_assignment >::const_iterator i1=s1.begin();
           i1!=s1.end(); ++i1,++i2)
      {
        if (i2==s2.end())
        {
          return false;
        }
        if (regular2)
        {
          if (i1->identifier()!=i2->identifier())
          {
            return false;
          }
        }
        else
        {
          if (*i1!=*i2)
          {
            return false;
          }
        }
      }
      if (i2!=s2.end())
      {
        return false;
      }
      return true;
    }

    bool exists_variable_for_sequence(
      const std::vector < process_instance_assignment >& process_names,
      process_identifier& result)
    {
      std::vector < std::vector < process_instance_assignment > >::const_iterator rwalker=representedprocesses.begin();
      for (std::vector < process_identifier >::const_iterator walker=seq_varnames.begin();
           walker!=seq_varnames.end(); ++walker,++rwalker)
      {
        assert(rwalker!=representedprocesses.end());
        const process_identifier process=*walker;
        if (match_sequence(process_names,*rwalker,options.lin_method==lmRegular2))
        {
          result=process;
          return true;
        }
      }
      assert(rwalker==representedprocesses.end());
      return false;
    }

    void extract_names(
      const process_expression sequence,
      std::vector < process_instance_assignment >& result)
    {
      if (is_action(sequence)||is_process_instance_assignment(sequence))
      {
        result.push_back(atermpp::down_cast<process_instance_assignment>(sequence));
        return;
      }

      if (is_stochastic_operator(sequence))
      {
        const stochastic_operator& sto=atermpp::down_cast<stochastic_operator>(sequence);
        extract_names(sto.operand(),result);
        return;
      }

      if (is_seq(sequence))
      {
        const process_expression first=seq(sequence).left();
        if (is_process_instance_assignment(first))
        {
          result.push_back(atermpp::down_cast<process_instance_assignment>(first));
          size_t n=objectIndex(atermpp::down_cast<process_instance_assignment>(first).identifier());
          if (objectdata[n].canterminate)
          {
            extract_names(seq(sequence).right(),result);
          }
          return;
        }
      }

      throw mcrl2::runtime_error("Internal error. Expected sequence of process names (1) " + process::pp(sequence) + ".");
    }

    variable_list parscollect(const process_expression& oldbody, process_expression& newbody)
    {
      /* we expect that oldbody is a sequence of process instances */

      if (is_process_instance_assignment(oldbody))
      {
        const process_identifier procId=process_instance_assignment(oldbody).identifier();
        const variable_list parameters=objectdata[objectIndex(procId)].parameters;
        assert(check_valid_process_instance_assignment(procId,data::assignment_list()));
        newbody=process_instance_assignment(procId,data::assignment_list());
        return parameters;
      }

      if (is_seq(oldbody))
      {
        const process_expression first=seq(oldbody).left();
        if (is_process_instance_assignment(first))
        {
          size_t n=objectIndex(process_instance_assignment(first).identifier());
          if (objectdata[n].canterminate)
          {
            const process_identifier procId=process_instance_assignment(first).identifier();
            const variable_list pars=parscollect(seq(oldbody).right(),newbody);
            variable_list pars1, pars2;

            const variable_list new_pars=construct_renaming(pars,objectdata[objectIndex(procId)].parameters,pars1,pars2,false);
            assignment_vector new_assignment;
            for(variable_list::const_iterator i=pars2.begin(), j=new_pars.begin(); i!=pars2.end(); ++i,++j)
            {
              assert(j!=new_pars.end());
              new_assignment.push_back(assignment(*i,*j));
            }
            assert(check_valid_process_instance_assignment(procId,assignment_list(new_assignment.begin(),new_assignment.end())));
            newbody=seq(process_instance_assignment(procId,assignment_list(new_assignment.begin(),new_assignment.end())),newbody);
            return pars1+pars;
          }
          else
          {
            return parscollect(first,newbody);
          }
        }
      }

      throw mcrl2::runtime_error("Internal error. Expected a sequence of process names (2) " + process::pp(oldbody) +".");
      return variable_list();
    }

    assignment_list argscollect_regular(
                         const process_expression& t,
                         const variable_list& vl,
                         const std::set<variable>& variables_bound_in_sum)
    {
      assignment_vector result;
      for(variable_list::const_iterator i=vl.begin(); i!=vl.end(); ++i)
      {
        if (variables_bound_in_sum.count(*i)>0 && occursinpCRLterm(*i,t,false))
        {
          result.push_back(assignment(*i,*i)); // Here an identity assignment is used, as it is possible
                                               // that the *i at the lhs is not the same variable as *i at the rhs.
        }
      }
      return assignment_list(result.begin(),result.end());
    }

    assignment_list argscollect_regular2(const process_expression& t, variable_list& vl)
    {
      if (is_process_instance_assignment(t))
      {
        const process_instance_assignment p(t);
        size_t n=objectIndex(p.identifier());

        const variable_list pars=objectdata[n].parameters; // These are the old parameters of the process.
        assert(pars.size()<=vl.size());

        std::map<variable,data_expression>sigma;
        for(assignment_list::const_iterator i=p.assignments().begin();
              i!=p.assignments().end(); ++i)
        {
          sigma[i->lhs()]=i->rhs();
        }

        assignment_list result;
        for(variable_list::const_iterator i=pars.begin(); i!=pars.end(); ++i, vl.pop_front())
        {
          assert(!vl.empty());
          const data_expression new_rhs=make_map_substitution(sigma)(*i);
          result.push_front(assignment(vl.front(),new_rhs)); // Identity assignments are stored as lhs and rhs may
                                                             // refer to different variables.
        }
        return reverse(result);
      }

      if (is_seq(t))
      {
        const process_instance_assignment firstproc=atermpp::down_cast<process_instance_assignment>(seq(t).left());
        size_t n=objectIndex(firstproc.identifier());
        const assignment_list first_assignment=argscollect_regular2(firstproc,vl);
        if (objectdata[n].canterminate)
        {
          return first_assignment + argscollect_regular2(seq(t).right(),vl);
        }
        return first_assignment;
      }

      throw mcrl2::runtime_error("Internal error. Expected a sequence of process names (3) " + process::pp(t) +".");
    }

    process_expression cut_off_unreachable_tail(const process_expression t)
    {
      if (is_process_instance_assignment(t)||is_delta(t)||is_action(t)||is_tau(t)||is_sync(t))
      {
        return t;
      }

      if (is_stochastic_operator(t))
      {
        const stochastic_operator& sto=atermpp::down_cast<const stochastic_operator>(t);
        return stochastic_operator(sto.variables(),sto.distribution(), cut_off_unreachable_tail(sto.operand()));
      }

      if (is_seq(t))
      {
        const process_expression firstproc=seq(t).left();
        size_t n=objectIndex(process_instance_assignment(firstproc).identifier());
        if (objectdata[n].canterminate)
        {
          return seq(firstproc,cut_off_unreachable_tail(seq(t).right()));
        }
        return firstproc;
      }

      throw mcrl2::runtime_error("Internal error. Expected a sequence of process names (4) " + process::pp(t) +".");
      return process_expression();
    }

    process_expression create_regular_invocation(
      process_expression sequence,
      std::vector <process_identifier>& todo,
      const variable_list& freevars,
      const std::set<variable>& variables_bound_in_sum)
    {
      process_identifier new_process;

      /* Sequence consists of a sequence of process references,
         concatenated with the sequential composition operator, which may be preceded by 
         a stochastic operator.  */
      if (is_stochastic_operator(sequence))
      {
        /* Leave the stochastic operators in place */
        const stochastic_operator& sto=atermpp::down_cast<const stochastic_operator>(sequence);
        std::set<variable> variables_bound_in_sum_new=variables_bound_in_sum;
        variables_bound_in_sum_new.insert(sto.variables().begin(),sto.variables().end());
        return stochastic_operator(sto.variables(),sto.distribution(), create_regular_invocation(sto.operand(),todo,freevars,variables_bound_in_sum_new));
      }

      sequence=cut_off_unreachable_tail(sequence);
      sequence=pCRLrewrite(sequence);
      std::vector < process_instance_assignment > process_names;
      extract_names(sequence,process_names);
      assert(!process_names.empty());

      if (process_names.size()==1)
      {
        /* length of list equals 1 */
        if (is_process_instance_assignment(sequence))
        {
          return sequence;
        }
        if (is_seq(sequence))
        {
          return seq(sequence).left();
        }
        throw mcrl2::runtime_error("Internal error. Expected a sequence of process names " + process::pp(sequence) +".");
      }
      /* There is more than one process name in the sequence,
         so, we must replace them by a single name */

      /* We first start out by searching whether
         there is already a variable with a matching sequence
         of variables */
      if (!exists_variable_for_sequence(process_names,new_process))
      {
        /* There does not exist an appropriate variable,
           so, make it and return its index in n */
        process_expression newbody;
        if (options.lin_method==lmRegular2)
        {
          variable_list pars=parscollect(sequence,newbody);
          new_process=newprocess(pars,newbody,pCRL,
                                 canterminatebody(newbody),
                                 containstimebody(newbody));
          representedprocesses.push_back(process_names);
        }
        else
        {
          new_process=newprocess(freevars,sequence,pCRL,
                                 canterminatebody(sequence),containstimebody(sequence));
          representedprocesses.push_back(process_names);
        }
        seq_varnames.push_back(new_process);
        todo.push_back(new_process);
      }
      /* now we must construct arguments */
      variable_list parameters=objectdata[objectIndex(new_process)].parameters;
      if (options.lin_method==lmRegular2)
      {
        const assignment_list args=argscollect_regular2(sequence,parameters);
        assert(check_valid_process_instance_assignment(new_process,args));
        const process_expression p=process_instance_assignment(new_process,args);
        return p;
      }
      else
      {
        assert(check_valid_process_instance_assignment(new_process,argscollect_regular(sequence,parameters,variables_bound_in_sum)));
        return process_instance_assignment(new_process,argscollect_regular(sequence,parameters,variables_bound_in_sum));
      }
    }

    process_expression to_regular_form(
      const process_expression t,
      std::vector <process_identifier>& todo,
      const variable_list freevars,
      const std::set<variable>& variables_bound_in_sum)
    /* t has the form of the sum, and condition over actions
       each followed by a sequence of variables. We replace
       this variable by a single one, putting the new variable
       on the todo list, to be transformed to regular form also. */
    {
      if (is_choice(t))
      {
        const process_expression t1=to_regular_form(choice(t).left(),todo,freevars,variables_bound_in_sum);
        const process_expression t2=to_regular_form(choice(t).right(),todo,freevars,variables_bound_in_sum);
        return choice(t1,t2);
      }

      if (is_seq(t))
      {
        const process_expression firstact=seq(t).left();
        assert(is_at(firstact)||is_tau(firstact)||is_action(firstact)||is_sync(firstact));
        /* the sequence of variables in
                   the second argument must be replaced */
        return seq(firstact,create_regular_invocation(seq(t).right(),todo,freevars,variables_bound_in_sum));
      }

      if (is_if_then(t))
      {
        return if_then(if_then(t).condition(),to_regular_form(if_then(t).then_case(),todo,freevars,variables_bound_in_sum));
      }

      if (is_sum(t))
      {
        variable_list sumvars=sum(t).variables();

        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        alphaconvert(sumvars,sigma,freevars,data_expression_list(),variables_occurring_in_rhs_of_sigma);
        const process_expression body=substitute_pCRLproc(sum(t).operand(), sigma, variables_occurring_in_rhs_of_sigma);

        std::set<variable> variables_bound_in_sum1=variables_bound_in_sum;
        variables_bound_in_sum1.insert(sumvars.begin(),sumvars.end());
        return sum(sumvars,
                   to_regular_form(
                     body,
                     todo,
                     sumvars+freevars,
                     variables_bound_in_sum1));
      }

      if (is_stochastic_operator(t))
      {
        const stochastic_operator& sto=down_cast<const stochastic_operator>(t);
        variable_list sumvars=sto.variables();

        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        alphaconvert(sumvars,sigma,freevars,data_expression_list(),variables_occurring_in_rhs_of_sigma);
        const data_expression distribution=data::replace_variables_capture_avoiding(
                                               sto.distribution(),
                                               sigma,
                                               variables_occurring_in_rhs_of_sigma);
        const process_expression body=substitute_pCRLproc(sto.operand(),sigma,variables_occurring_in_rhs_of_sigma);

        std::set<variable> variables_bound_in_sum1=variables_bound_in_sum;
        variables_bound_in_sum1.insert(sumvars.begin(),sumvars.end());
        return stochastic_operator(
                   sumvars,
                   distribution,
                   to_regular_form(
                     body,
                     todo,
                     sumvars+freevars,
                     variables_bound_in_sum1));
      }

      if (is_sync(t)||is_action(t)||is_delta(t)||is_tau(t)||is_at(t))
      {
        return t;
      }

      throw mcrl2::runtime_error("to regular form expects GNF " + process::pp(t) +".");
      return process_expression();
    }

    process_expression distributeTime(
      const process_expression body,
      const data_expression time,
      const variable_list freevars,
      data_expression& timecondition)
    {
      if (is_choice(body))
      {
        return choice(
                 distributeTime(choice(body).left(),time,freevars,timecondition),
                 distributeTime(choice(body).right(),time,freevars,timecondition));
      }

      if (is_sum(body))
      {
        variable_list sumvars=sum(body).variables();
        process_expression body1=sum(body).operand();
        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        alphaconvert(sumvars,sigma,freevars,data_expression_list(),variables_occurring_in_rhs_of_sigma);
        body1=substitute_pCRLproc(body1, sigma, variables_occurring_in_rhs_of_sigma);
        mutable_map_substitution<> sigma_aux(sigma);
        const data_expression time1=data::replace_variables_capture_avoiding(time,sigma_aux,variables_occurring_in_rhs_of_sigma);
        body1=distributeTime(body1,time1,sumvars+freevars,timecondition);
        return sum(sumvars,body1);
      }

      if (is_stochastic_operator(body))
      {
        const stochastic_operator& sto=down_cast<const stochastic_operator>(body);
        variable_list sumvars=sto.variables();
        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        alphaconvert(sumvars,sigma,freevars,data_expression_list(),variables_occurring_in_rhs_of_sigma);
        process_expression new_body=substitute_pCRLproc(sto.operand(), sigma, variables_occurring_in_rhs_of_sigma);
        data_expression new_distribution=data::replace_variables_capture_avoiding(sto.distribution(), sigma, variables_occurring_in_rhs_of_sigma);
        mutable_map_substitution<> sigma_aux(sigma);
        const data_expression time1=data::replace_variables_capture_avoiding(time,sigma_aux,variables_occurring_in_rhs_of_sigma);
        new_body=distributeTime(new_body,time1,sumvars+freevars,timecondition);
        return stochastic_operator(sumvars,new_distribution,new_body);
      }

      if (is_if_then(body))
      {
        data_expression timecondition=sort_bool::true_();
        process_expression body1=distributeTime(
                                   if_then(body).then_case(),
                                   time,
                                   freevars,
                                   timecondition);

        return if_then(
                 lazy::and_(data_expression(if_then(body).condition()),timecondition),
                 body1);
      }

      if (is_seq(body))
      {
        return seq(
                 distributeTime(seq(body).left(), time,freevars,timecondition),
                 seq(body).right());
      }

      if (is_at(body))
      {
        /* make a new process */
        timecondition=equal_to(time,data_expression(at(body).time_stamp()));
        return body;
      }

      if ((is_sync(body))||
          (is_action(body))||
          (is_tau(body))||
          (is_delta(body)))
      {
        return at(body,time);
      }

      throw mcrl2::runtime_error("expected pCRL process in distributeTime " + process::pp(body) +".");
      return process_expression();
    }

    process_expression procstorealGNFbody(
      const process_expression body,
      variableposition v,
      std::vector <process_identifier>& todo,
      const bool regular,
      processstatustype mode,
      const variable_list freevars,
      const std::set <variable>& variables_bound_in_sum)
    /* This process delivers the transformation of body
       to GNF with actions as a head symbol, or it
       delivers NULL if body is not a pCRL process.
       If regular=1, then an attempt is made to obtain a
       GNF where one action is always followed by a
       variable. */
    {
      if (is_at(body))
      {
        data_expression timecondition=sort_bool::true_();
        process_expression body1=procstorealGNFbody(
                                   at(body).operand(),
                                   first,
                                   todo,
                                   regular,
                                   mode,
                                   freevars,
                                   variables_bound_in_sum);
        return distributeTime(
                 body1,
                 at(body).time_stamp(),
                 freevars,
                 timecondition);
      }

      if (is_choice(body))
      {
        const process_expression body1=procstorealGNFbody(choice(body).left(),first,todo,
                                       regular,mode,freevars,variables_bound_in_sum);
        const process_expression body2=procstorealGNFbody(choice(body).right(),first,todo,
                                       regular,mode,freevars,variables_bound_in_sum);
        return choice(body1,body2);
      }

      if (is_seq(body))
      {
        const process_expression body1=procstorealGNFbody(seq(body).left(),v,
                                       todo,regular,mode,freevars,variables_bound_in_sum);
        const process_expression body2=procstorealGNFbody(seq(body).right(),later,
                                       todo,regular,mode,freevars,variables_bound_in_sum);
        process_expression t3=putbehind(body1,body2);
        if ((regular) && (v==first))
        {
          /* We must transform t3 to regular form */
          t3=to_regular_form(t3,todo,freevars,variables_bound_in_sum);
        }
        return t3;
      }

      if (is_if_then(body))
      {
        const process_expression r=distribute_condition(
                                     procstorealGNFbody(if_then(body).then_case(),first,
                                         todo,regular,mode,freevars,variables_bound_in_sum),
                                     if_then(body).condition());
        return r;
      }

      if (is_sum(body))
      {
        const variable_list sumvars=sum(body).variables();
        std::set<variable> variables_bound_in_sum1=variables_bound_in_sum;
        variables_bound_in_sum1.insert(sumvars.begin(),sumvars.end());
        return distribute_sum(sumvars,
                              procstorealGNFbody(sum(body).operand(),first,
                                  todo,regular,mode,sumvars+freevars,variables_bound_in_sum1));
      }

      if (is_stochastic_operator(body))
      {
        const stochastic_operator& sto=down_cast<const stochastic_operator>(body);
        const variable_list sumvars=sto.variables();
        std::set<variable> variables_bound_in_sum1=variables_bound_in_sum;
        variables_bound_in_sum1.insert(sumvars.begin(),sumvars.end());
        return stochastic_operator(
                              sumvars,
                              sto.distribution(),
                              procstorealGNFbody(sto.operand(),v,
                                  todo,regular,mode,sumvars+freevars,variables_bound_in_sum1));
      }

      if (is_action(body))
      {
        return body;
      }

      if (is_sync(body))
      {
        return body;
      }

      if (is_process_instance_assignment(body))
      {
        process_identifier t=process_instance_assignment(body).identifier();

        if (v==later)
        {
          if (regular)
          {
            mode=mCRL;
          }
          todo.push_back(t);
          /* if ((!regular)||(mode=mCRL))
              todo.push_back(t);
                / * single = in `mode=mCRL' is important, otherwise crash
                   I do not understand the reason for this at this moment
                   JFG (9/5/2000) */
          return body;
        }

        const size_t n=objectIndex(t);
        if (objectdata[n].processstatus==mCRL)
        {
          todo.push_back(t);
          return process_expression();
        }
        /* The variable is a pCRL process and v==first, so,
           we must now substitute */
        procstorealGNFrec(t,first,todo,regular);


        const assignment_list& dl= process_instance_assignment(body).assignments();

        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;

        for(assignment_list::const_iterator i=dl.begin(); i!=dl.end(); ++i)
        {
          sigma[i->lhs()]=i->rhs();
          const std::set<variable> varset=find_free_variables(i->rhs());
          variables_occurring_in_rhs_of_sigma.insert(varset.begin(),varset.end());
        }
        process_expression t3=substitute_pCRLproc(objectdata[n].processbody,sigma,variables_occurring_in_rhs_of_sigma);
        if (regular)
        {
          t3=to_regular_form(t3,todo,freevars,variables_bound_in_sum);
        }

        return t3;
      }

      if (is_delta(body))
      {
        return body;
      }

      if (is_tau(body))
      {
        return body;
      }

      if (is_merge(body))
      {
        procstorealGNFbody(process::merge(body).left(),later,
                           todo,regular,mode,freevars,variables_bound_in_sum);
        procstorealGNFbody(process::merge(body).right(),later,
                           todo,regular,mode,freevars,variables_bound_in_sum);
        return process_expression();
      }

      if (is_hide(body))
      {
        procstorealGNFbody(hide(body).operand(),later,todo,regular,mode,freevars,variables_bound_in_sum);
        return process_expression();
      }

      if (is_rename(body))
      {
        procstorealGNFbody(process::rename(body).operand(),later,todo,regular,mode,freevars,variables_bound_in_sum);
        return process_expression();
      }

      if (is_allow(body))
      {
        procstorealGNFbody(allow(body).operand(),later,todo,regular,mode,freevars,variables_bound_in_sum);
        return process_expression();
      }

      if (is_block(body))
      {
        procstorealGNFbody(block(body).operand(),later,todo,regular,mode,freevars,variables_bound_in_sum);
        return process_expression();
      }

      if (is_comm(body))
      {
        procstorealGNFbody(comm(body).operand(),later,todo,regular,mode,freevars,variables_bound_in_sum);
        return process_expression();
      }

      // The exception below can especially trigger on a process_instance as these ought to have
      // been removed earlier in favour of process_instance_assignments.
      throw mcrl2::runtime_error("unexpected process format in procstorealGNF " + process::pp(body) +".");
      return process_expression();
    }


    void procstorealGNFrec(
      const process_identifier procIdDecl,
      const variableposition v,
      std::vector <process_identifier>& todo,
      const bool regular)

    /* Do a depth first search on process variables and substitute
       for the headvariable of a pCRL process, in case it is a process,
       such that we obtain a Greibach Normal Form. All pCRL processes will
       be labelled with GNF to indicate that they are in
       Greibach Normal Form. */

    {
      size_t n=objectIndex(procIdDecl);
      if (objectdata[n].processstatus==pCRL)
      {
        objectdata[n].processstatus=GNFbusy;
        std::set<variable> variables_bound_in_sum;
        const process_expression t=procstorealGNFbody(objectdata[n].processbody,first,
                                   todo,regular,pCRL,objectdata[n].parameters,variables_bound_in_sum);
        if (objectdata[n].processstatus!=GNFbusy)
        {
          throw mcrl2::runtime_error("There is something wrong with recursion.");
        }

        objectdata[n].processbody=t;
        objectdata[n].processstatus=GNF;
        return;
      }

      if (objectdata[n].processstatus==mCRL)
      {
        objectdata[n].processstatus=mCRLbusy;
        std::set<variable> variables_bound_in_sum;
        procstorealGNFbody(objectdata[n].processbody,first,todo,
                                   regular,mCRL,objectdata[n].parameters,variables_bound_in_sum);
        /* if the last result is not equal to NULL,
           the body of this process is itself a processidentifier */

        objectdata[n].processstatus=mCRLdone;
        return;
      }

      if ((objectdata[n].processstatus==GNFbusy) && (v==first))
      {
        throw mcrl2::runtime_error("Unguarded recursion in process " + process::pp(procIdDecl) +".");
      }

      if ((objectdata[n].processstatus==GNFbusy)||
          (objectdata[n].processstatus==GNF)||
          (objectdata[n].processstatus==mCRLdone)||
          (objectdata[n].processstatus==multiAction))
      {
        return;
      }

      if (objectdata[n].processstatus==mCRLbusy)
      {
        throw mcrl2::runtime_error("unguarded recursion without pCRL operators");
      }

      throw mcrl2::runtime_error("strange process type: " + str(boost::format("%d") % objectdata[n].processstatus));
    }

    void procstorealGNF(const process_identifier procsIdDecl,
                        const bool regular)
    {
      std::vector <process_identifier> todo;
      todo.push_back(procsIdDecl);
      for (; !todo.empty() ;)
      {
        const process_identifier pi=todo.back();
        todo.pop_back();
        procstorealGNFrec(pi,first,todo,regular);
      }
    }


    /**************** GENERaTE LPE **********************************/
    /*                                                              */
    /*                                                              */
    /*                                                              */
    /*                                                              */
    /*                                                              */
    /*                                                              */
    /****************************************************************/


    /**************** Make pCRL procs  ******************************/

    void makepCRLprocs(const process_expression t,
                       std::vector <process_identifier>& pCRLprocs)
    {
      if (is_choice(t))
      {
        makepCRLprocs(choice(t).left(),pCRLprocs);
        makepCRLprocs(choice(t).right(),pCRLprocs);
        return;
      }

      if (is_seq(t))
      {
        makepCRLprocs(seq(t).left(),pCRLprocs);
        makepCRLprocs(seq(t).right(),pCRLprocs);
        return;
      }

      if (is_if_then(t))
      {
        makepCRLprocs(if_then(t).then_case(),pCRLprocs);
        return;
      }

      if (is_sum(t))
      {
        makepCRLprocs(sum(t).operand(),pCRLprocs);
        return;
      }

      if (is_stochastic_operator(t))
      {
        makepCRLprocs(stochastic_operator(t).operand(),pCRLprocs);
        return;
      }

      if (is_process_instance_assignment(t))
      {
        process_identifier t1=process_instance_assignment(t).identifier(); /* get procId */
        if (std::find(pCRLprocs.begin(),pCRLprocs.end(),t1)==pCRLprocs.end())
        {
          pCRLprocs.push_back(t1);
          makepCRLprocs(objectdata[objectIndex(t1)].processbody,pCRLprocs);
        }
        return;
      }

      if (is_sync(t)||is_action(t)||is_tau(t)||is_delta(t)||is_at(t))
      {
        return;
      }

      throw mcrl2::runtime_error("unexpected process format " + process::pp(t) + " in makepCRLprocs");
    }

    /**************** Collectparameterlist ******************************/

    bool alreadypresent(variable& var,const variable_list vl, const size_t n)
    {
      /* Note: variables can be different, although they have the
         same string, due to different types. If they have the
         same string, but different types, the conflict must
         be resolved by renaming the name of the variable */

      if (vl.empty())
      {
        return false;
      }
      const variable var1=vl.front();
      assert(is_variable(var1));

      /* The variable with correct type is present: */
      if (var==var1)
      {
        return true;
      }

      /* Compare whether the string indicating the variable
         name is equal, but the types are different. In that
         case the variable needs to be renamed to a fresh one,
         and is not present in vl. */
      if (var.name()==var1.name())
      {
        assert(0); // Renaming of parameters is not allowed, given that assignments are being used in processes, using the names of the variables.
        variable var2=get_fresh_variable(var.name(),var.sort());
        // templist is needed as objectdata may be realloced
        // during the substitution. Same applies to tempvar
        // below.
        mutable_map_substitution<> sigma;
        std::set<variable> variables_occurring_in_rhs_of_sigma;
        sigma[var]=var2;
        variables_occurring_in_rhs_of_sigma.insert(var2);
        data_expression_list templist=data::replace_free_variables(atermpp::container_cast<data_expression_list>(objectdata[n].parameters), sigma);
        objectdata[n].parameters=variable_list(templist);
        process_expression tempvar=substitute_pCRLproc(objectdata[n].processbody, sigma,variables_occurring_in_rhs_of_sigma);
        objectdata[n].processbody=tempvar;
        var=var2;
        return false;
      }

      /* otherwise it can be present in vl */
      return alreadypresent(var,vl.tail(),n);
    }

    variable_list joinparameters(const variable_list par1,
                                 const variable_list par2,
                                 const size_t n)
    {
      if (par2.empty())
      {
        return par1;
      }

      variable var2=par2.front();
      assert(is_variable(var2));

      variable_list result=joinparameters(par1,par2.tail(),n);
      if (alreadypresent(var2,par1,n))
      {
        return result;
      }

      result.push_front(var2);
      return result;
    }

    variable_list collectparameterlist(std::vector < process_identifier>& pCRLprocs)
    {
      variable_list parameters;
      /* pCRLprocs can grow. Therefore, no iterators can be used. */
      for (size_t i=0; i<pCRLprocs.size(); ++i) 
      {
        size_t n=objectIndex(pCRLprocs[i]);
        parameters=joinparameters(parameters,objectdata[n].parameters,n);
        std::set<process_identifier> visited;
        const process_expression process_with_stochastic_distribution=obtain_initial_distribution(pCRLprocs[i],visited, pCRLprocs);
        if (is_stochastic_operator(process_with_stochastic_distribution))
        {
          parameters=joinparameters(parameters,stochastic_operator(process_with_stochastic_distribution).variables(),n);
        }
      }
      for (std::vector < process_identifier>::const_iterator walker=pCRLprocs.begin();
           walker!=pCRLprocs.end(); ++walker)
      {
        size_t n=objectIndex(*walker);
        parameters=joinparameters(parameters,objectdata[n].parameters,n);
      }
      return parameters;
    }

    /****************  Declare local datatypes  ******************************/

    void declare_control_state(
      const std::vector < process_identifier>& pCRLprocs)
    {
      create_enumeratedtype(pCRLprocs.size());
    }

    class stackoperations:public boost::noncopyable
    {
      public:
        variable_list parameter_list;
        sort_expression stacksort;
        sort_expression_list sorts;
        function_symbol_list get;
        data::function_symbol push;
        data::function_symbol emptystack;
        data::function_symbol empty;
        data::function_symbol pop;
        data::function_symbol getstate;
        stackoperations* next;

        stackoperations(const variable_list pl,
                        specification_basic_type& spec)
        {
          parameter_list=pl;
          next=spec.stack_operations_list;
          spec.stack_operations_list=this;

          //create structured sort
          //  Stack = struct emptystack?is_empty
          //               | push(getstate: Pos, getx1: S1, ..., getxn: Sn, pop: Stack)
          //               ;

          basic_sort stack_sort_alias(spec.fresh_identifier_generator("Stack"));
          structured_sort_constructor_argument_vector sp_push_arguments;
          for (variable_list::const_iterator l = pl.begin() ; l!=pl.end() ; ++l)
          {
            sp_push_arguments.push_back(structured_sort_constructor_argument(spec.fresh_identifier_generator("get" + std::string(l->name())), l->sort()));
            sorts.push_front(l->sort());
          }
          sp_push_arguments.push_back(structured_sort_constructor_argument(spec.fresh_identifier_generator("pop"), stack_sort_alias));
          sorts=reverse(sorts);
          structured_sort_constructor sc_push(spec.fresh_identifier_generator("push"), sp_push_arguments);
          structured_sort_constructor sc_emptystack(spec.fresh_identifier_generator("emptystack"),spec.fresh_identifier_generator("isempty"));

          structured_sort_constructor_vector constructors(1,sc_push);
          constructors.push_back(sc_emptystack);
          //add data declarations for structured sort
          spec.data.add_alias(alias(stack_sort_alias,structured_sort(constructors)));
          stacksort=data::normalize_sorts(stack_sort_alias,spec.data);
          push=sc_push.constructor_function(stack_sort_alias);
          emptystack=sc_emptystack.constructor_function(stack_sort_alias);
          empty=sc_emptystack.recogniser_function(stack_sort_alias);
          const std::vector< data::function_symbol > projection_functions =
            sc_push.projection_functions(stack_sort_alias);
          pop=projection_functions.back();
          getstate=projection_functions.front();
          get=function_symbol_list(projection_functions.begin()+1,projection_functions.end()-1);
        }

        ~stackoperations()
        {
        }
    };

    class stacklisttype:public boost::noncopyable
    {
      public:
        stackoperations* opns;
        variable_list parameters;
        variable stackvar;
        size_t no_of_states;
        /* the boolean state variables occur in reverse
           order, i.e. the least significant first, whereas
           in parameter lists, the order is reversed. */
        variable_list booleanStateVariables;


        /* All datatypes for different stacks that are being generated
           are stored in the following list, such that it can be investigated
           whether a suitable stacktype already exist, before generating a new
        one */


        stackoperations* find_suitable_stack_operations(
          const variable_list parameters,
          stackoperations* stack_operations_list)
        {
          if (stack_operations_list==NULL)
          {
            return NULL;
          }
          if (parameters==stack_operations_list->parameter_list)
          {
            return stack_operations_list;
          }
          return find_suitable_stack_operations(parameters,stack_operations_list->next);
        }

        /// \brief Constructor
        stacklisttype(const variable_list parlist,
                      specification_basic_type& spec,
                      const bool regular,
                      const std::vector < process_identifier>& pCRLprocs,
                      const bool singlecontrolstate)
        {
          parameters=parlist;

          no_of_states=pCRLprocs.size();
          process_identifier last=pCRLprocs.back();
          const std::string s3((spec.options.statenames)?std::string(last.name()):std::string("s3"));
          if ((spec.options.binary) && (spec.options.newstate))
          {
            size_t i=spec.upperpowerof2(no_of_states);
            for (; i>0 ; i--)
            {
              variable name(spec.fresh_identifier_generator("bst"),sort_bool::bool_());
              spec.insertvariable(name,true);
              booleanStateVariables.push_front(name);
            }
          }

          if (regular)
          {
            opns=NULL;
            if (spec.options.newstate)
            {
              if (!spec.options.binary)
              {
                if (!singlecontrolstate)
                {
                  const size_t e=spec.create_enumeratedtype(no_of_states);
                  stackvar=variable(spec.fresh_identifier_generator(s3), spec.enumeratedtypes[e].sortId);
                }
                else
                {
                  /* Generate a stackvariable that is never used */
                  stackvar=variable(spec.fresh_identifier_generator("Never_used"), sort_bool::bool_());
                }
              }
              else
              {
                stackvar=variable(spec.fresh_identifier_generator(s3),sort_bool::bool_());
              }
            }
            else
            {
              stackvar=variable(spec.fresh_identifier_generator(s3), sort_pos::pos());
            }
            spec.insertvariable(stackvar,true);
          }
          else
          {
            if (spec.options.newstate)
            {
              throw mcrl2::runtime_error("cannot combine stacks with " +
                                         (spec.options.binary?std::string("binary"):std::string("an enumerated type")));
            }
            opns=find_suitable_stack_operations(parlist,spec.stack_operations_list);

            if (opns!=NULL)
            {
              stackvar=variable(spec.fresh_identifier_generator(s3),opns->stacksort);
              spec.insertvariable(stackvar,true);
            }
            else
            {
              variable_list temp=parlist;
              temp.push_front(variable("state",sort_pos::pos()));
              opns=(stackoperations*) new stackoperations(temp,spec);
              stackvar = variable(spec.fresh_identifier_generator(s3), opns->stacksort);
              spec.insertvariable(stackvar,true);
            }
          }
        }

        ~stacklisttype()
        {
        }

    };

    bool is_global_variable(const data_expression& d) const
    {
      return is_variable(d) && global_variables.count(atermpp::down_cast<variable>(d)) > 0;
    }

    data_expression getvar(const variable var,
                           const stacklisttype& stack)
    {
      /* first search whether the variable is a free process variable */

      for (std::set <variable>::const_iterator walker=global_variables.begin() ;
           walker!=global_variables.end() ; ++walker)
      {
        if (*walker==var)
        {
          return var;
        }
      }

      /* otherwise find out whether the variable matches a parameter */
      function_symbol_list::const_iterator getmappings=stack.opns->get.begin();
      for (variable_list::const_iterator walker=stack.parameters.begin() ;
           walker!=stack.parameters.end() ; ++walker,++getmappings)
      {
        if (*walker==var)
        {
          return application(*getmappings,stack.stackvar);
        }
        assert(getmappings!=stack.opns->get.end());
      }
      assert(0); /* We cannot end up here, because that means that we
                    are looking for in non-existing variable */
      return var;
    }

    assignment_list processencoding(
      size_t i,
      const assignment_list& t1,
      const stacklisttype& stack)
    {
      assert(i>0);
      assignment_list t(t1);
      if (!options.newstate)
      {
        assignment_list result=t;
        result.push_front(assignment(stack.stackvar,sort_pos::pos(i)));
        return result;
      }

      i=i-1; /* below we count from 0 instead from 1 as done in the
                first version of the prover */

      if (!options.binary)
      {
        const size_t e=create_enumeratedtype(stack.no_of_states);
        function_symbol_list l(enumeratedtypes[e].elementnames);
        for (; i>0 ; i--)
        {
          assert(l.size()>0);
          l.pop_front();
        }
        assignment_list result=t;
        assert(l.size()>0);
        result.push_front(assignment(stack.stackvar,l.front()));
        return result;
      }
      /* else a sequence of boolean values needs to be generated,
         representing the value i, when there are l->n elements */
      {
        size_t k=upperpowerof2(stack.no_of_states);
        variable_list::const_iterator boolean_state_variables=stack.booleanStateVariables.begin();
        for (; k>0 ; k--, ++boolean_state_variables)
        {
          if ((i % 2)==0)
          {
            t.push_front(assignment(*boolean_state_variables,sort_bool::false_()));
            i=i/2;
          }
          else
          {
            t.push_front(assignment(*boolean_state_variables,sort_bool::true_()));
            i=(i-1)/2;
          }
        }
        return t;
      }
    }

    data_expression_list processencoding(
      size_t i,
      const data_expression_list& t1,
      const stacklisttype& stack)
    {
      data_expression_list t(t1);
      if (!options.newstate)
      {
        data_expression_list result=t;
        result.push_front(sort_pos::pos(i));
        return result;
      }

      i=i-1; /* below we count from 0 instead from 1 as done in the
                first version of the prover */

      if (!options.binary)
      {
        const size_t e=create_enumeratedtype(stack.no_of_states);
        function_symbol_list l(enumeratedtypes[e].elementnames);
        for (; i>0 ; i--)
        {
          l.pop_front();
        }
        data_expression_list result=t;
        result.push_front(l.front());
        return result;
      }
      /* else a sequence of boolean values needs to be generated,
         representing the value i, when there are l->n elements */
      {
        size_t k=upperpowerof2(stack.no_of_states);
        variable_list::const_iterator boolean_state_variables=stack.booleanStateVariables.begin();
        for (; k>0 ; k--, ++boolean_state_variables)
        {
          if ((i % 2)==0)
          {
            t.push_front(sort_bool::false_());
            i=i/2;
          }
          else
          {
            t.push_front(sort_bool::true_());
            i=(i-1)/2;
          }
        }
        return t;
      }
    }

    data_expression correctstatecond(
      const process_identifier procId,
      const std::vector < process_identifier>& pCRLproc,
      const stacklisttype& stack,
      int regular)
    {
      size_t i;

      for (i=1 ; pCRLproc[i-1]!=procId ; ++i) {}
      /* i is the index of the current process */

      if (!options.newstate)
      {
        if (regular)
        {
          return equal_to(stack.stackvar, processencoding(i,assignment_list(),stack).front().rhs());
        }
        return equal_to(
                 application(stack.opns->getstate,stack.stackvar),
                 processencoding(i,assignment_list(),stack).front().rhs());
      }

      if (!options.binary) /* Here a state encoding using enumerated types
                        must be declared */
      {
        create_enumeratedtype(stack.no_of_states);
        if (regular)
        {
          return equal_to(stack.stackvar,
                          processencoding(i,assignment_list(),stack).front().rhs());
        }
        return equal_to(
                 application(stack.opns->getstate, stack.stackvar),
                 processencoding(i,assignment_list(),stack).front().rhs());
      }

      /* in this case we must encode the condition using
         boolean variables */

      const variable_list vars=stack.booleanStateVariables;

      i=i-1; /* start counting from 0, instead from 1 */
      data_expression t3(sort_bool::true_());
      for (variable_list::const_iterator v=vars.begin(); v!=vars.end(); ++v)
      {
        if ((i % 2)==0)
        {
          t3=lazy::and_(lazy::not_(*v),t3);
          i=i/2;
        }
        else
        {
          t3=lazy::and_(*v,t3);
          i=(i-1)/2;
        }

      }
      assert(i==0);
      return t3;
    }

    data_expression adapt_term_to_stack(
      const data_expression t,
      const stacklisttype& stack,
      const variable_list vars)
    {
      if (is_function_symbol(t))
      {
        return t;
      }

      if (is_variable(t))
      {
        if (std::find(vars.begin(),vars.end(),t)!=vars.end())
        {
          /* t occurs in vars, so, t does not have to be reconstructed
             from the stack */
          return t;
        }
        else
        {
          return getvar(atermpp::down_cast<variable>(t), stack);
        }
      }

      if (is_application(t))
      {
        const application&a=atermpp::down_cast<application>(t);
        return application(
                 adapt_term_to_stack(a.head(),stack,vars),
                 adapt_termlist_to_stack(a.begin(),a.end(),stack,vars));
      }

      if (is_abstraction(t))
      {
        const abstraction& abs_t(t);
        return abstraction(
                 abs_t.binding_operator(),
                 abs_t.variables(),
                 adapt_term_to_stack(abs_t.body(),stack,abs_t.variables() + vars));
      }

      if (is_where_clause(t))
      {
        const where_clause where_t(t);
        const assignment_list old_assignments=reverse(where_t.assignments());
        variable_list new_vars=vars;
        assignment_list new_assignments;
        for (assignment_list::const_iterator i=old_assignments.begin();
             i!=old_assignments.end(); ++i)
        {
          new_vars.push_front(i->lhs());
          new_assignments.push_front(
                             assignment(
                               i->lhs(),
                               adapt_term_to_stack(i->rhs(),stack,vars)));

        }
        return where_clause(
                 adapt_term_to_stack(where_t,stack,new_vars),
                 new_assignments);

      }

      assert(0);  // expected a term;
      return t;   // in case of non-debug mode, try to return something as decent as possible.
    }

    template <typename Iterator>
    data_expression_vector adapt_termlist_to_stack(
      Iterator begin,
      const Iterator& end,
      const stacklisttype& stack,
      const variable_list& vars)
    {
      data_expression_vector result;
      for (; begin != end; ++begin)
      {
        result.push_back(adapt_term_to_stack(*begin,stack, vars));
      }
      return result;
    }


    action_list adapt_multiaction_to_stack_rec(
      const action_list multiAction,
      const stacklisttype& stack,
      const variable_list vars)
    {
      if (multiAction.empty())
      {
        return multiAction;
      }

      const action act=action(multiAction.front());

      action_list result=adapt_multiaction_to_stack_rec(multiAction.tail(),stack,vars);

      const data_expression_vector vec(adapt_termlist_to_stack(
                            act.arguments().begin(),
                            act.arguments().end(),
                            stack,
                            vars));
      result.push_front(action(act.label(),data_expression_list(vec.begin(),vec.end())));
      return result;
    }

    action_list adapt_multiaction_to_stack(
      const action_list multiAction,
      const stacklisttype& stack,
      const variable_list vars)
    {
      return adapt_multiaction_to_stack_rec(multiAction,stack,vars);
    }

    data_expression representative_generator_internal(const sort_expression s, const bool allow_dont_care_var=true)
    {
      if ((!options.noglobalvars) && allow_dont_care_var)
      {
        const variable newVariable(fresh_identifier_generator("dc"),s);
        insertvariable(newVariable,true);
        global_variables.insert(newVariable);
        return newVariable;
      }
      return representative_generator(data)(s);
    }

    data_expression find_(
      const variable s,
      const assignment_list args,
      const stacklisttype& stack,
      const variable_list vars,
      const std::set<variable>& free_variables_in_body)
    {
      /* We generate the value for variable s in the list of
         the parameters of the process. If s is equal to some
         variable in pars, it is an argument of the current
         process, and it must be replaced by the corresponding
         argument in args.
           If s does not occur in pars, it must be replaced
         by a dummy value.
      */
      for (assignment_list::const_iterator i=args.begin(); i!=args.end(); ++i)
      {
        if (s==i->lhs())
        {
          return adapt_term_to_stack(i->rhs(),stack,vars);
        }
      }

      if (free_variables_in_body.find(s)==free_variables_in_body.end())
      {
        const data_expression result=representative_generator_internal(s.sort());
        return adapt_term_to_stack(result,stack,vars);
      }
      return adapt_term_to_stack(s,stack,vars);
    }


    data_expression_list findarguments(
      const variable_list pars,
      const variable_list parlist,
      const assignment_list args,
      const data_expression_list t2,
      const stacklisttype& stack,
      const variable_list vars,
      const std::set<variable>& free_variables_in_body)
    {
      if (parlist.empty())
      {
        return t2;
      }
      data_expression_list result=findarguments(pars,parlist.tail(),args,t2,stack,vars,free_variables_in_body);
      data_expression rhs=find_(parlist.front(),args,stack,vars,free_variables_in_body);

      result.push_front(rhs);
      return result;
    }

    assignment_list find_dummy_arguments(
      const variable_list parlist,   // The list of all parameters.
      const assignment_list args,
      const std::set<variable>& free_variables_in_body,
      const variable_list& stochastic_variables)
    {
      std::map<variable,data_expression> assignment_map;
      for(assignment_list::const_iterator k=args.begin(); k!=args.end(); ++k)
      {
        assignment_map[k->lhs()]=k->rhs();
      }

      assignment_vector result;
      for(variable_list::const_iterator i=parlist.begin(); i!=parlist.end(); ++i)
      {
        if (std::find(stochastic_variables.begin(),stochastic_variables.end(),*i)!=stochastic_variables.end())
        {
          // *i is a stochastic variable. Insert the identity assignment.
          result.push_back(assignment(*i,*i));
        }
        else if (free_variables_in_body.find(*i)==free_variables_in_body.end())
        {
          {
            // The variable *i must get a default value.
            const data_expression rhs=representative_generator_internal(i->sort());
            result.push_back(assignment(*i,rhs));
          }
        }
        else
        {
          const std::map<variable,data_expression>::iterator k=assignment_map.find(*i);
          if (k!=assignment_map.end())  // There is assignment for *i. Use it.
          {
            result.push_back(assignment(k->first,k->second));
            assignment_map.erase(k);
          }
        }

      }
      return assignment_list(result.begin(), result.end());
    }



    assignment_list push_regular(
      const process_identifier procId,
      const assignment_list args,
      const stacklisttype& stack,
      const std::vector < process_identifier >& pCRLprocs,
      bool singlestate,
      const variable_list& stochastic_variables)
    {
      const size_t n=objectIndex(procId);
      const assignment_list t=find_dummy_arguments(stack.parameters,args,get_free_variables(n),stochastic_variables);

      if (singlestate)
      {
        return t;
      }

      size_t i;
      for (i=1 ; pCRLprocs[i-1]!=procId ; ++i) {}
      return processencoding(i,t,stack);
    }


    assignment_list make_procargs_regular(
      const process_expression& t,
      const stacklisttype& stack,
      const std::vector < process_identifier >& pcrlprcs,
      const bool singlestate,
      const variable_list& stochastic_variables)
    {
      /* t is a sequential composition of process variables */

      if (is_seq(t))
      {
        throw mcrl2::runtime_error("Process is not regular, as it has stacking vars: " + process::pp(t) + ".");
      }

      if (is_process_instance_assignment(t))
      {
        const process_identifier procId=process_instance_assignment(t).identifier();
        const assignment_list t1=process_instance_assignment(t).assignments();
        return push_regular(procId,
                            t1,
                            stack,
                            pcrlprcs,
                            singlestate,
                            stochastic_variables);
      }

      throw mcrl2::runtime_error("Expect seq or name constructing a recursive invocation: " + process::pp(t) + ".");
    }

    data_expression push_stack(
      const process_identifier procId,
      const assignment_list args,
      const data_expression_list t2,
      const stacklisttype& stack,
      const std::vector < process_identifier >& pCRLprocs,
      const variable_list vars)
    {
      const size_t n=objectIndex(procId);
      const data_expression_list t=findarguments(objectdata[n].parameters,
                                                 stack.parameters,args,t2,stack,vars,get_free_variables(n));

      size_t i;
      for (i=1 ; pCRLprocs[i-1]!=procId ; ++i) {}

      const data_expression_list l=processencoding(i,t,stack);
      assert(l.size()==function_sort(stack.opns->push.sort()).domain().size());
      return application(stack.opns->push,l);
    }

    data_expression make_procargs_stack(
      const process_expression& t,
      const stacklisttype& stack,
      const std::vector < process_identifier >& pcrlprcs,
      const variable_list& vars,
      const variable_list& stochastic_variables)
    {
      /* t is a sequential composition of process variables */

      if (is_seq(t))
      {
        const process_instance_assignment process=atermpp::down_cast<process_instance_assignment>(seq(t).left());
        const process_expression process2=seq(t).right();
        const process_identifier procId=process.identifier();
        const assignment_list t1=process.assignments();

        if (objectdata[objectIndex(procId)].canterminate)
        {
          const data_expression stackframe=make_procargs_stack(process2,stack,pcrlprcs, vars,stochastic_variables);
          return push_stack(procId,t1, { stackframe },stack,pcrlprcs,vars);
        }

        return push_stack(procId,t1, { stack.opns->emptystack },
                                           stack,pcrlprcs,vars);
      }

      if (is_process_instance_assignment(t))
      {
        const process_identifier procId=process_instance_assignment(t).identifier();
        const assignment_list t1=process_instance_assignment(t).assignments();

        if (objectdata[objectIndex(procId)].canterminate)
        {
          return push_stack(procId,
                            t1,
                            { application(stack.opns->pop,stack.stackvar) },
                            stack,
                            pcrlprcs,
                            vars);
        }
        return push_stack(procId,
                          t1,
                          { stack.opns->emptystack },
                          stack,
                          pcrlprcs,
                          vars);
      }

      throw mcrl2::runtime_error("Expect seq or name putting processes on a stack: " + process::pp(t) +".");
    }

    assignment_list make_procargs(
      const process_expression& t,
      const stacklisttype& stack,
      const std::vector < process_identifier >& pcrlprcs,
      const variable_list& vars,
      const bool regular,
      const bool singlestate,
      const variable_list& stochastic_variables)
    {
      if (regular)
      {
        return make_procargs_regular(t,stack,pcrlprcs,singlestate,stochastic_variables);
      }
      /* return a stackframe */
      data_expression sf=make_procargs_stack(t,stack,pcrlprcs,vars,stochastic_variables);
      return { assignment(stack.stackvar,sf) };
    }

    bool occursin(const variable& name,
                  const variable_list& pars)
    {
      assert(is_variable(name));
      for (variable_list::const_iterator l=pars.begin() ; l!=pars.end(); ++l)
      {
        if (name.name()==l->name())
        {
          return true;
        }
      }
      return false;
    }


    assignment_list pushdummyrec_regular(
      const variable_list& totalpars,
      const variable_list& pars,
      const stacklisttype& stack,
      const variable_list& stochastic_variables)
    {
      /* totalpars is the total list of parameters of the
         aggregated pCRL process. The variable pars contains
         the list of all variables occuring in the initial
         process. */

      if (totalpars.empty())
      {
        return assignment_list();
      }

      const variable par=totalpars.front();
      if (std::find(pars.begin(),pars.end(),par)!=pars.end())
      {
        assignment_list result=pushdummyrec_regular(totalpars.tail(),pars,stack,stochastic_variables);
        return result;
      }
      // Check whether it is a stochastic variable.
      if (std::find(stochastic_variables.begin(),stochastic_variables.end(),par)!=pars.end())
      {
        assignment_list result=pushdummyrec_regular(totalpars.tail(),pars,stack,stochastic_variables);
        result.push_front(assignment(par,par));
        return result;
      }
      /* otherwise the value of this argument is irrelevant, so
         make it a don't care variable. */

      assignment_list result=pushdummyrec_regular(totalpars.tail(),pars,stack,stochastic_variables);
      result.push_front(assignment(par,representative_generator_internal(par.sort())));
      return result;
    }

    assignment_list pushdummy_regular(
      const variable_list& parameters,
      const stacklisttype& stack,
      const variable_list& stochastic_variables)
    {
      return pushdummyrec_regular(stack.parameters,parameters,stack,stochastic_variables);
    }

    data_expression_list pushdummyrec_stack(
      const variable_list& totalpars,
      const variable_list& pars,
      const stacklisttype& stack,
      const variable_list& stochastic_variables)
    {
      /* totalpars is the total list of parameters of the
         aggregated pCRL process. The variable pars contains
         the list of all variables occuring in the initial
         process. */

      if (totalpars.empty())
      {
        return { stack.opns->emptystack };
      }

      const variable par=totalpars.front();
      if (std::find(pars.begin(),pars.end(),par)!=pars.end())
      {
        data_expression_list result=pushdummyrec_stack(totalpars.tail(),pars,stack,stochastic_variables);
        result.push_front(par);
        return result;
      }
      /* Check whether the parameter par refers to a stochastic variables */
      if (std::find(stochastic_variables.begin(),stochastic_variables.end(),par)!=pars.end())
      {
        data_expression_list result=pushdummyrec_stack(totalpars.tail(),pars,stack,stochastic_variables);
        result.push_front(par);
        return result;
      }
      /* otherwise the value of this argument is irrelevant, so
         make it Nil, if a regular translation is made. If a translation
         with stacks is made, then yield a default `unique' term. */
      data_expression_list result=pushdummyrec_stack(totalpars.tail(),pars,stack,stochastic_variables);
      result.push_front(representative_generator_internal(par.sort()));
      return result;
    }

    data_expression_list pushdummy_stack(
      const variable_list& parameters,
      const stacklisttype& stack,
      const variable_list& stochastic_variables)
    {
      return pushdummyrec_stack(stack.parameters,parameters,stack,stochastic_variables);
    }

    assignment_list make_initialstate(
      const process_identifier& initialProcId,
      const stacklisttype& stack,
      std::vector < process_identifier >& pcrlprcs,
      const bool regular,
      const bool singlecontrolstate,
      stochastic_distribution& initial_stochastic_distribution)
    {
      size_t i;
      for (i=1 ; pcrlprcs[i-1]!=initialProcId ; ++i) {};
      /* i is the index of the initial state */

      /* Calculate the initial distribution */
      std::set<process_identifier> visited;
      const process_expression initial_process_with_stochastic_distribution=
                                         obtain_initial_distribution(initialProcId,visited,pcrlprcs);
      if (is_stochastic_operator(initial_process_with_stochastic_distribution))
      {
        const stochastic_operator& sto=atermpp::down_cast<stochastic_operator>(initial_process_with_stochastic_distribution);
        initial_stochastic_distribution=stochastic_distribution(sto.variables(),sto.distribution());
      }
      else
      {
        initial_stochastic_distribution=stochastic_distribution(variable_list(),real_one());
      }

      if (regular)
      {
        assignment_list result=
          pushdummy_regular(objectdata[objectIndex(initialProcId)].parameters,
                            stack,
                            initial_stochastic_distribution.variables());
        if (!singlecontrolstate)
        {
          return processencoding(i,result,stack);
        }
        return result;
      }
      else
      {
        data_expression_list result=
                pushdummy_stack(objectdata[objectIndex(initialProcId)].parameters,
                                stack,
                                initial_stochastic_distribution.variables());
        const data_expression_list l=processencoding(i,result,stack);
        assert(l.size()==function_sort(stack.opns->push.sort()).domain().size());
        return { assignment(stack.stackvar,application(stack.opns->push,l)) };
      }
    }

    /*************************  Routines for summands  **************************/

    assignment_list dummyparameterlist(const stacklisttype& stack,
                                       const bool singlestate)
    {
      if (singlestate)
      {
        return assignment_list();
      }

      return processencoding(1,assignment_list(),stack); /* Take 1 as dummy indicator */
    }


    void insert_summand(
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      const variable_list& sumvars,
      const data_expression& condition,
      const action_list& multiAction,
      const data_expression& actTime,
      const stochastic_distribution& distribution,
      const assignment_list& procargs,
      const bool has_time,
      const bool is_deadlock_summand)
    {
      assert(distribution!=stochastic_distribution());
      const data_expression rewritten_condition=RewriteTerm(condition);
      if (rewritten_condition==sort_bool::false_())
      {
        return;
      }

      if (is_deadlock_summand)
      {
        deadlock_summands.push_back(deadlock_summand(sumvars,
                                                     rewritten_condition,
                                                     has_time?deadlock(actTime):deadlock()));
      }
      else
      {
        action_summands.push_back(stochastic_action_summand(
                                                 sumvars,
                                                 rewritten_condition,
                                                 has_time?multi_action(multiAction,actTime):multi_action(multiAction),
                                                 procargs,
                                                 stochastic_distribution(distribution.variables(),
                                                                         RewriteTerm(distribution.distribution()))));
      }
    }


    void add_summands(
      const process_identifier& procId,
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      process_expression summandterm,
      std::vector < process_identifier>& pCRLprocs,
      const stacklisttype& stack,
      const bool regular,
      const bool singlestate,
      const variable_list& process_parameters)
    {
      data_expression atTime;
      action_list multiAction;
      bool is_delta_summand=false;
      bool has_time=false;

      if (isDeltaAtZero(summandterm))
      {
        // delta@0 does not need to be added.
        return;
      }

      /* remove the sum operators; collect the sum variables in the
         list sumvars */

      variable_list sumvars;
      for (; is_sum(summandterm) ;)
      {
        sumvars=sum(summandterm).variables() + sumvars;
        summandterm=sum(summandterm).operand();
      }

      // Check whether the variables in sumvars clash with the parameter list,
      // and rename the summandterm accordingly.
      mutable_map_substitution<> local_sigma;
      std::set<variable> local_rhs_variables_in_sigma;
      alphaconvert(sumvars,local_sigma,process_parameters,data_expression_list(),local_rhs_variables_in_sigma);
      summandterm=substitute_pCRLproc(summandterm, local_sigma, local_rhs_variables_in_sigma);


      /* translate the condition */

      data_expression condition1;
      if (regular && singlestate)
      {
        condition1=sort_bool::true_();
      }
      else
      {
        condition1=correctstatecond(procId,pCRLprocs,stack,regular);
      }

      stochastic_distribution cumulative_distribution(variable_list(),real_one());

      /* The conditions are collected for use. The stochastic operators before the action are ignored */
      for (; (is_if_then(summandterm)||is_stochastic_operator(summandterm)) ;)
      {
        if (is_if_then(summandterm))
        {
          const data_expression localcondition=data_expression(if_then(summandterm).condition());
          if (!(regular && singlestate))
          {
            condition1=lazy::and_(
                         condition1,
                         ((regular)?localcondition:
                          adapt_term_to_stack(
                            localcondition,
                            stack,
                            sumvars)));
          }
          else
          {
            /* regular and singlestate */
            condition1=lazy::and_(localcondition,condition1);
          }
          summandterm=if_then(summandterm).then_case();
        }
        else
        {
          const stochastic_operator& sto=atermpp::down_cast<const stochastic_operator>(summandterm);
          cumulative_distribution=stochastic_distribution(
                                    cumulative_distribution.variables()+sto.variables(),
                                    real_times_optimized(cumulative_distribution.distribution(),
                                                         sto.distribution()));
          summandterm=sto.operand();
        }
      }

      if (!cumulative_distribution.variables().empty() && !sumvars.empty())
      {
        throw mcrl2::runtime_error("Cannot permute sum operator and stochastic distribution in " + data::pp(summandterm));
      }

      if (is_seq(summandterm))
      {
        /* only one summand is needed */
        process_expression t1=seq(summandterm).left();
        process_expression t2=seq(summandterm).right();
        if (is_at(t1))
        {
          has_time=true;
          atTime=at(t1).time_stamp();
          t1=at(t1).operand();
        }

        if (t1==delta())
        {
          is_delta_summand=true;
        }
        else
        {
          assert(is_tau(t1)||is_action(t1)||is_sync(t1));
          multiAction=to_action_list(t1);
        }

        std::set<process_identifier> visited;
        const process_expression process_with_stochastic_distribution=obtain_initial_distribution_term(t2,visited,pCRLprocs);
        stochastic_distribution distribution(variable_list(),real_one());
        process_expression t2_new=t2;
        if (is_stochastic_operator(process_with_stochastic_distribution))
        {
          const stochastic_operator& sto=atermpp::down_cast<const stochastic_operator>(process_with_stochastic_distribution);
          distribution=stochastic_distribution(sto.variables(),sto.distribution());
          t2_new=sto.operand();
        }
        const assignment_list procargs=make_procargs(t2_new,stack,pCRLprocs,sumvars,regular,singlestate,distribution.variables());
        if (!regular)
        {
          if (!is_delta_summand)
          {
            multiAction=adapt_multiaction_to_stack(
                          multiAction,stack,sumvars);
          }
          if (has_time)
          {
            atTime=adapt_term_to_stack(
                     atTime,stack,sumvars);
          }
        }
        insert_summand(action_summands,deadlock_summands,
                       sumvars,condition1,multiAction,
                       atTime,distribution,procargs,
                       has_time,is_delta_summand);
        return;
      }

      /* There is a single initial multiaction or deadlock, possibly timed*/
      if (is_at(summandterm))
      {
        atTime=at(summandterm).time_stamp();
        summandterm=at(summandterm).operand();
        has_time=true;
      }
      else
      {
        // do nothing
      }

      if (is_delta(summandterm))
      {
        is_delta_summand=true;
      }
      else if (is_tau(summandterm))
      {
        // multiAction is already empty.
      }
      else if (is_action(summandterm))
      {
        multiAction.push_front(action(summandterm));
      }
      else if (is_sync(summandterm))
      {
        multiAction=to_action_list(summandterm);
      }
      else
      {
        throw mcrl2::runtime_error("expected multiaction " + process::pp(summandterm) +".");
      }

      if (regular)
      {
        if (!is_delta_summand)
          /* As termination has been replaced by an explicit action terminated, followed
           * by delta, a single terminating action cannot exist for regular processes. */
        {
          throw mcrl2::runtime_error("terminating processes should not exist when using the regular flag");
        }
        insert_summand(action_summands,deadlock_summands,
                       sumvars,
                       condition1,
                       multiAction,
                       atTime,
                       stochastic_distribution(variable_list(),real_one()),
                       dummyparameterlist(stack,singlestate),
                       has_time,
                       is_delta_summand);
        return;
      }

      multiAction=adapt_multiaction_to_stack(multiAction,stack,sumvars);
      assignment_list procargs = { assignment(stack.stackvar,application(stack.opns->pop,stack.stackvar)) };

      insert_summand(
                action_summands,deadlock_summands,
                sumvars,
                condition1,
                multiAction,
                atTime,
                stochastic_distribution(variable_list(),real_one()),   // TODO: UNLIKELY THAT THIS IS CORRECT.
                procargs,
                has_time,
                is_delta_summand);

      return;
    }


    void collectsumlistterm(
      const process_identifier& procId,
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      const process_expression& body,
      const variable_list& pars,
      const stacklisttype& stack,
      const bool regular,
      const bool singlestate,
      std::vector < process_identifier>& pCRLprocs)
    {
      if (is_choice(body))
      {
        const process_expression t1=choice(body).left();
        const process_expression t2=choice(body).right();

        collectsumlistterm(procId,action_summands,deadlock_summands,t1,pars,stack,
                           regular,singlestate,pCRLprocs);
        collectsumlistterm(procId,action_summands,deadlock_summands,t2,pars,stack,
                           regular,singlestate,pCRLprocs);
        return;
      }
      if (is_stochastic_operator(body))
      {
        /* Remove leading stochastic operators. They will not become
           part of this summand */
        const stochastic_operator& sto=atermpp::down_cast<const stochastic_operator>(body);
        collectsumlistterm(procId,action_summands,deadlock_summands,sto.operand(),pars,stack,
                           regular,singlestate,pCRLprocs);
      }
      else
      {
        add_summands(procId,action_summands,deadlock_summands,body,pCRLprocs,stack,
                     regular,singlestate,pars);
      }
    }

    void collectsumlist(
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      std::vector < process_identifier>& pCRLprocs,
      const variable_list& pars,
      const stacklisttype& stack,
      bool regular,
      bool singlestate)
    {
      /* The vector pCRLprocs can be extended, while this loop is executed. Therefore, we cannot use an iterator. */
      for (size_t i=0; i<pCRLprocs.size(); ++i) 
      {
        const process_identifier procId= pCRLprocs[i];
        collectsumlistterm(
          procId,
          action_summands,
          deadlock_summands,
          objectdata[objectIndex(procId)].processbody,
          pars,
          stack,
          regular,
          singlestate,
          pCRLprocs);
      }
    }


    /**************** Enumtype and enumeratedtype **********************************/

    class enumeratedtype
    {
      public:
        size_t size;
        sort_expression sortId;
        data_expression_list elementnames;
        function_symbol_list functions;

        enumeratedtype(const size_t n,
                       specification_basic_type& spec)
        {
          size=n;
          if (n==2)
          {
            sortId = sort_bool::bool_();
            // elementnames = make_list(data_expression(sort_bool::false_()),data_expression(sort_bool::true_()));
            elementnames = { sort_bool::false_(), sort_bool::true_()};
          }
          else
          {
            //create new sort identifier
            basic_sort sort_id(spec.fresh_identifier_generator(str(boost::format("Enum%d") % n)));
            sortId=sort_id;
            //create structured sort
            //  Enumi = struct en_i | ... | e0_i;
            structured_sort_constructor_list struct_conss;
            for (size_t j=0 ; (j<n) ; j++)
            {
              //create constructor declaration of the structured sort
              const identifier_string s=spec.fresh_identifier_generator(str(boost::format("e%d_%d") % j % n));
              const structured_sort_constructor struct_cons(s,"");

              struct_conss.push_front(struct_cons);
            }
            structured_sort sort_struct(struct_conss);

            //add declaration of standard functions
            spec.data.add_alias(alias(sort_id, sort_struct));

            //store new declarations in return value w
            sortId = sort_id;
            const function_symbol_vector& constructors=spec.data.constructors(sort_id);
            assert(constructors.size()==n);
            elementnames = data::data_expression_list(constructors.begin(),constructors.end());
          }
        }

        enumeratedtype(const enumeratedtype& e)
        {
          size=e.size;
          sortId=e.sortId;
          elementnames=e.elementnames;
          functions=e.functions;
        }

        void operator=(const enumeratedtype& e)
        {
          size=e.size;
          sortId=e.sortId;
          elementnames=e.elementnames;
          functions=e.functions;
        }

        ~enumeratedtype()
        {
        }
    };

    size_t create_enumeratedtype(const size_t n)
    {
      size_t w;

      for (w=0; ((w<enumeratedtypes.size()) && (enumeratedtypes[w].size!=n)); ++w) {};

      if (w==enumeratedtypes.size()) // There is no enumeratedtype of arity n.
      {
        enumeratedtypes.push_back(enumeratedtype(n,*this));
      }
      return w;
    }

    data::function_symbol find_case_function(size_t index, const sort_expression& sort)
    {
      const function_symbol_list functions=enumeratedtypes[index].functions;
      for (function_symbol_list::const_iterator w=functions.begin();
           w!=functions.end(); ++w)
      {
        sort_expression_list domain = function_sort(w->sort()).domain();
        assert(domain.size() >= 2);
        if (*(++domain.begin())==sort)
        {
          return *w;
        }
      };

      throw mcrl2::runtime_error("searching for a nonexisting case function on sort " + data::pp(sort) +".");
      return data::function_symbol();
    }

    void define_equations_for_case_function(
      const size_t index,
      const data::function_symbol& functionname,
      const sort_expression& sort)
    {
      variable_list vars;
      data_expression_list args;
      data_expression_list xxxterm;

      const sort_expression normalised_sort=sort;
      const variable v1=get_fresh_variable("x",normalised_sort);
      const size_t n=enumeratedtypes[index].size;
      for (size_t j=0; (j<n); j++)
      {
        const variable v=get_fresh_variable("y",normalised_sort);
        vars.push_front(v);
        args.push_front(data_expression(v));
        xxxterm.push_front(data_expression(v1));
      }

      /* I generate here an equation of the form
         C(e,x,x,x,...x)=x for a variable x. */
      const sort_expression s=enumeratedtypes[index].sortId;
      const variable v=get_fresh_variable("e",s);
      data_expression_list tempxxxterm=xxxterm;
      tempxxxterm.push_front(data_expression(v));
      data.add_equation(
        data_equation(
          variable_list({ v1, v }),
          application(functionname,tempxxxterm),
          data_expression(v1)));
      fresh_equation_added=true;

      variable_list auxvars=vars;

      const data_expression_list elementnames=enumeratedtypes[index].elementnames;
      for (data_expression_list::const_iterator w=elementnames.begin();
           w!=elementnames.end() ; ++w)
      {
        assert(auxvars.size()>0);
        data_expression_list tempargs=args;
        tempargs.push_front(*w);
        data.add_equation(data_equation(
                          vars,
                          application(functionname,tempargs),
                          auxvars.front()));
        fresh_equation_added=true;

        auxvars.pop_front();
      }
    }

    void create_case_function_on_enumeratedtype(
      const sort_expression& sort,
      const size_t enumeratedtype_index)
    {
      assert(enumeratedtype_index<enumeratedtypes.size());
      /* first find out whether the function exists already, in which
         case nothing needs to be done */

      const function_symbol_list functions=enumeratedtypes[enumeratedtype_index].functions;
      for (function_symbol_list::const_iterator w=functions.begin();
           w!=functions.end(); ++w)
      {
        const sort_expression w1sort=w->sort();
        assert(function_sort(w1sort).domain().size()>1);
        // Check that the second sort of the case function equals sort
        if (*(++(function_sort(w1sort).domain().begin()))==sort)
        {
          return; // The case function does already exist
        }
      };

      /* The function does not exist;
         Create a new function of enumeratedtype e, on sort */

      if (enumeratedtypes[enumeratedtype_index].sortId==sort_bool::bool_())
      {
        /* take the if function on sort 'sort' */
        function_symbol_list f=enumeratedtypes[enumeratedtype_index].functions;
        f.push_front(if_(sort));
        enumeratedtypes[enumeratedtype_index].functions=f;
        return;
      }
      // else
      sort_expression_list newsortlist;
      size_t n=enumeratedtypes[enumeratedtype_index].size;
      for (size_t j=0; j<n ; j++)
      {
        newsortlist.push_front(sort);
      }
      sort_expression sid=enumeratedtypes[enumeratedtype_index].sortId;
      newsortlist.push_front(sid);

      const function_sort newsort(newsortlist,sort);
      const data::function_symbol casefunction(
        fresh_identifier_generator(str(boost::format("C%d_%s") % n % (
                         !is_basic_sort(newsort)?"":std::string(basic_sort(sort).name())))),
        newsort);
      // insertmapping(casefunction,true);
      data.add_mapping(casefunction);
      function_symbol_list f=enumeratedtypes[enumeratedtype_index].functions;
      f.push_front(casefunction);
      enumeratedtypes[enumeratedtype_index].functions=f;

      define_equations_for_case_function(enumeratedtype_index,casefunction,sort);
      return;
    }

    class enumtype : public boost::noncopyable
    {
      public:
        size_t enumeratedtype_index;
        variable var;

        enumtype(size_t n,
                 const sort_expression_list& fsorts,
                 const sort_expression_list& gsorts,
                 specification_basic_type& spec)
        {

          enumeratedtype_index=spec.create_enumeratedtype(n);

          var=variable(spec.fresh_identifier_generator("e"),spec.enumeratedtypes[enumeratedtype_index].sortId);
          spec.insertvariable(var,true);

          for (sort_expression_list::const_iterator w=fsorts.begin(); w!=fsorts.end(); ++w)
          {
            spec.create_case_function_on_enumeratedtype(*w,enumeratedtype_index);
          }

          for (sort_expression_list::const_iterator w=gsorts.begin(); w!=gsorts.end(); ++w)
          {
            spec.create_case_function_on_enumeratedtype(*w,enumeratedtype_index);
          }

          spec.create_case_function_on_enumeratedtype(sort_bool::bool_(),enumeratedtype_index);

          if (spec.timeIsBeingUsed || spec.stochastic_operator_is_being_used)
          {
            spec.create_case_function_on_enumeratedtype(sort_real::real_(),enumeratedtype_index);
          }
        }

        ~enumtype()
        {
        }
    };
    /************** Merge summands using enumerated type ***********************/

    /* The function below returns true if the variable var could be mapped
       on an existing variable v' in matchinglist. The pars and args form pair
       form a substitution that will be extended with the pair [var,v']. i
       It returns false if the variable is new.

       If var is added (and not mapped on some other variable in the matchinglist/aka v)
       it is checked whether var occurs in  v or in the process_parameters,
       in which case var is renamed to a fresh variable. The renaming is added
       to the substitution encoded in pars/args.
    */


    bool mergeoccursin(
      variable& var,
      const variable_list& v,
      variable_list& matchinglist,
      variable_list& pars,
      data_expression_list& args,
      const variable_list& process_parameters)
    {
      variable_list auxmatchinglist;

      /* First find out whether var:sort can be matched to a
         term in the matching list */

      /* first find out whether the variable occurs in the matching
         list, so, they can be joined */

      for (variable_list::const_iterator i=matchinglist.begin();
           i!=matchinglist.end(); ++i)
      {
        variable var1=*i;
        if (var.sort()==var1.sort())
        {
          /* sorts match, so, we join the variables */
          if (var!=var1)
          {
            pars.push_front(var);
            args.push_front(data_expression(var1));
          }
          /* copy remainder of matching list */
          for (++i ; i!=matchinglist.end(); ++i)
          {
            auxmatchinglist.push_front(*i);
          }
          matchinglist=reverse(auxmatchinglist);
          return true;
        }
        else
        {
          auxmatchinglist.push_front(var1);
        }
      }

      /* turn auxmatchinglist back in normal order, and put the result
         in *matchinglist */

      matchinglist=reverse(auxmatchinglist);

      /* in this case no matching argument has been found.
      So, we must find out whether *var is an allowed variable, not
      occuring in the variablelist v.
      But if so, we must replace it by a new one. */
      for (variable_list::const_iterator i=v.begin() ; i!=v.end() ; ++i)
      {
        variable var1=*i;
        if (var.name()==var1.name())
        {
          pars.push_front(var);
          var=get_fresh_variable(var.name(),var.sort());
          args.push_front(data_expression(var));
          return false;
        }
      }

      /* Check whether the variable occurs in the prcoess parameter list, in which case
         it also needs to be renamed */
      for (variable_list::const_iterator i=process_parameters.begin() ; i!=process_parameters.end() ; ++i)
      {
        variable var1=*i;
        if (var.name()==var1.name())
        {
          pars.push_front(var);
          var=get_fresh_variable(var.name(),var.sort());
          args.push_front(data_expression(var));
          return false;
        }
      }


      return false;
    }

    data_expression_list extend(const data_expression& c, const data_expression_list& cl)
    {
      if (cl.empty())
      {
        return cl;
      }
      data_expression_list result=extend(c,cl.tail());
      result.push_front(data_expression(lazy::and_(c,cl.front())));
      return result;
    }
 
    data_expression variables_are_equal_to_default_values(const variable_list& vl)
    {
      data_expression result=sort_bool::true_();
      for(const variable& v: vl)
      {
        const data_expression unique=representative_generator_internal(v.sort(),false);
        result=lazy::and_(result,equal_to(v,unique));
      }
      return result;
    }


    data_expression_list extend_conditions(
      const variable& var,
      const data_expression_list& conditionlist)
    {
      const data_expression unique=representative_generator_internal(var.sort(),false);
      const data_expression newcondition=equal_to(var,unique);
      return extend(newcondition,conditionlist);
    }


    data_expression transform_matching_list(const variable_list& matchinglist)
    {
      if (matchinglist.empty())
      {
        return sort_bool::true_();
      }

      const variable var=matchinglist.front();
      data_expression unique=representative_generator_internal(var.sort(),false);
      return lazy::and_(
               transform_matching_list(matchinglist.tail()),
               equal_to(data_expression(var),unique));
    }


    data_expression_list addcondition(
      const variable_list& matchinglist,
      const data_expression_list& conditionlist)
    {
      data_expression_list result=conditionlist;
      result.push_front(transform_matching_list(matchinglist));
      return result;
    }

/* Join the variables of v1 to v2 and rename the variables in v1
 * if needed. The conditionlist gives conditions to restrain variables
 * that did not occur in the other list. renaming pars and args give
 * renamings to be applied if variables in v1 had to be renamed. It
 * is not allowed to rename to names already occurring in the parameter
 * list. */

   variable_list merge_var(
      const variable_list& v1,
      const variable_list& v2,
      std::vector < variable_list>& renamings_pars,
      std::vector < data_expression_list>& renamings_args,
      data_expression_list& conditionlist,
      const variable_list& process_parameters)
    {
      data_expression_list renamingargs;
      variable_list renamingpars;
      variable_list matchinglist=v2;

      /* If the sequence of sum variables is reversed,
       * the variables are merged in the same sequence for all
       * summands (due to a suggestion of Muck van Weerdenburg) */

      variable_list v1h=(v2.empty()?reverse(v1):v1);

      variable_list result=v2;
      for (variable_list::const_iterator i1=v1h.begin(); i1!=v1h.end() ; ++i1)
      {
        variable v=*i1;
        if (!mergeoccursin(v,v2,
                           matchinglist,renamingpars,renamingargs,process_parameters))
        {
          result.push_front(v);
          conditionlist=extend_conditions(v,conditionlist);
        }
      }
      conditionlist=addcondition(matchinglist,conditionlist);
      renamings_pars.push_back(renamingpars);
      renamings_args.push_back(renamingargs);
      return result;
    }

    variable_list make_binary_sums(
      size_t n,
      const sort_expression& enumtypename,
      data_expression& condition,
      const variable_list& tail)
    {
      variable_list result;
      assert(n>1);
      condition=sort_bool::true_();

      n=n-1;
      for (result=tail ; (n>0) ; n=n/2)
      {
        variable sumvar=get_fresh_variable("e",enumtypename);
        result.push_front(sumvar);
        if ((n % 2)==0)
        {
          condition=lazy::and_(sumvar,condition);
        }
        else
        {
          condition=lazy::or_(sumvar,condition);
        }
      }
      return result;
    }

    data_expression construct_binary_case_tree_rec(
      size_t n,
      const variable_list& sums,
      data_expression_list& terms,
      const sort_expression& termsort,
      const enumtype& e)
    {
      assert(!terms.empty());

      if (n<=0)
      {
        assert(!terms.empty());
        const data_expression t=terms.front();
        terms.pop_front();
        return t;
      }

      assert(!sums.empty());
      variable casevar=sums.front();

      const data_expression t=construct_binary_case_tree_rec(n / 2,sums.tail(),terms,termsort,e);

      if (terms.empty())
      {
        return t;
      }

      const data_expression t1=construct_binary_case_tree_rec(n / 2,sums.tail(),terms,termsort,e);

      if (t==t1)
      {
        return t;
      }
      return application(find_case_function(e.enumeratedtype_index, termsort), casevar, t, t1);
    }

    template <class T>
    bool all_equal(const atermpp::term_list<T>& l)
    {
      if (l.empty())
      {
        return true;
      }
      typename atermpp::term_list<T>::const_iterator i=l.begin();
      const T& first=*i;
      for(++i ; i!=l.end(); ++i)
      {
        if (*i!=first)
        {
          return false;
        }
      }
      return true;
    }

    data_expression construct_binary_case_tree(
      size_t n,
      const variable_list& sums,
      data_expression_list terms,
      const sort_expression& termsort,
      const enumtype& e)
    {
      return construct_binary_case_tree_rec(n-1,sums,terms,termsort,e);
    }

    bool summandsCanBeClustered(
      const stochastic_action_summand& summand1,
      const stochastic_action_summand& summand2)
    {
      if (summand1.has_time()!= summand2.has_time())
      {
        return false;
      }

      /* Here the multiactions are proper multi actions,
         both with or without a time indication */

      /* The actions can be clustered if they contain
         the same actions, with the same sorts for the
         actions. We assume that the multiactions are
         ordered.
      */

      const action_list multiactionlist1=summand1.multi_action().actions();
      const action_list multiactionlist2=summand2.multi_action().actions();
      action_list::const_iterator i2=multiactionlist2.begin();
      for (action_list::const_iterator i1=multiactionlist1.begin(); i1!=multiactionlist1.end(); ++i1,++i2)
      {
        if (i2==multiactionlist2.end())
        {
          return false;
        }
        if (i1->label()!=i2->label())
        {
          return false;
        }
      }
      if (i2!=multiactionlist2.end())
      {
        return false;
      }
      return true;
    }

    data_expression getRHSassignment(const variable& var, const assignment_list& as)
    {
      for (assignment_list::const_iterator i=as.begin(); i!=as.end(); ++i)
      {
        if (i->lhs()==var)
        {
          return i->rhs();
        }
      }
      return data_expression(var);
    }

    stochastic_action_summand collect_sum_arg_arg_cond(
      const enumtype& e,
      size_t n,
      const stochastic_action_summand_vector& action_summands,
      const variable_list& parameters)
    {
      /* This function gets a list of summands, with
         the same multiaction and time
         status. It yields a single clustered summand
         by introducing an auxiliary sum operator, with
         a variable of enumtype. In case binary is used,
         a sequence of variables are introduced of sort Bool */

      variable_list resultsum;
      data_expression resultcondition;
      action_list resultmultiaction;
      data_expression resulttime;

      std::vector < variable_list > rename_list_pars;
      std::vector < data_expression_list > rename_list_args;
      /* rename list is a list of pairs of variable and term lists */
      data_expression_list conditionlist;
      data_expression binarysumcondition;
      int equaluptillnow=1;

      for (stochastic_action_summand_vector::const_iterator walker=action_summands.begin(); walker!=action_summands.end() ; ++walker)
      {
        const variable_list sumvars=walker->summation_variables();
        resultsum=merge_var(sumvars,resultsum,rename_list_pars,rename_list_args,conditionlist,parameters);
      }

      if (options.binary)
      {
        resultsum=make_binary_sums(
                    n,
                    enumeratedtypes[e.enumeratedtype_index].sortId,
                    binarysumcondition,
                    resultsum);
      }
      else
      {
        resultsum.push_front(e.var);
      }

      /* we construct the resulting condition */
      data_expression_list auxresult;
      std::vector < variable_list >::const_iterator auxrename_list_pars=rename_list_pars.begin();
      std::vector < data_expression_list >::const_iterator auxrename_list_args=rename_list_args.begin();

      data_expression equalterm;
      equaluptillnow=1;
      for (stochastic_action_summand_vector::const_iterator walker=action_summands.begin(); walker!=action_summands.end(); ++walker)
      {
        const stochastic_action_summand smmnd=*walker;
        const data_expression condition=smmnd.condition();
        assert(auxrename_list_pars!=rename_list_pars.end());
        assert(auxrename_list_args!=rename_list_args.end());
        const variable_list auxpars= *auxrename_list_pars;
        ++auxrename_list_pars;
        const data_expression_list auxargs= *auxrename_list_args;
        ++auxrename_list_args;
        std::map<variable,data_expression> sigma;
        std::set<variable> variables_in_rhs_of_sigma;
        data_expression_list::const_iterator j=auxargs.begin();
        for (variable_list::const_iterator i=auxpars.begin();
             i!=auxpars.end(); ++i, ++j)
        {
          /* Substitutions are carried out from left to right. The first applicable substitution counts */
          if (sigma.count(*i)==0)
          {
            sigma[*i]=*j;
            const std::set<variable> varset=find_free_variables(*j);
            variables_in_rhs_of_sigma.insert(varset.begin(),varset.end());
          }
        }
        mutable_map_substitution<> mutable_sigma(sigma);

        const data_expression auxresult1=data::replace_variables_capture_avoiding(condition,mutable_sigma,variables_in_rhs_of_sigma);
        if (equalterm==data_expression()||is_global_variable(equalterm))
        {
          equalterm=auxresult1;
        }
        else
        {
          if (equaluptillnow)
          {
            equaluptillnow=((auxresult1==equalterm)||is_global_variable(auxresult1));
          }
        }
        auxresult.push_front(auxresult1);
      }
      if (options.binary)
      {
        resultcondition=construct_binary_case_tree(n,
                        resultsum,auxresult,sort_bool::bool_(),e);
        resultcondition=lazy::and_(binarysumcondition,resultcondition);
        resultcondition=lazy::and_(
                          construct_binary_case_tree(n,
                              resultsum,conditionlist,sort_bool::bool_(),e),
                          resultcondition);
      }
      else
      {
        if (equaluptillnow)
        {
          if (all_equal(conditionlist))
          {
            resultcondition=lazy::and_(conditionlist.front(), equalterm);
          }
          else
          {
            data_expression_list tempconditionlist=conditionlist;
            tempconditionlist.push_front(data_expression(e.var));
            resultcondition=lazy::and_(
                              application(
                                find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                                tempconditionlist),
                              equalterm);
          }
        }
        else
        {
          data_expression_list tempauxresult=auxresult;
          tempauxresult.push_front(data_expression(e.var));
          resultcondition=application(
                            find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                            tempauxresult);
          if (all_equal(conditionlist))
          {
           resultcondition=lazy::and_(conditionlist.front(),resultcondition);
          }
          else
          {
           data_expression_list tempconditionlist=conditionlist;
           tempconditionlist.push_front(data_expression(e.var));
           resultcondition=lazy::and_(
                            application(
                              find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                              tempconditionlist),
                            resultcondition);
          }
        }
      }

      /* now we construct the arguments of the action */
      /* First we collect all multi-actions in a separate vector
         of multiactions */
      std::vector < action_list > multiActionList;

      for (stochastic_action_summand_vector::const_iterator walker=action_summands.begin(); walker!=action_summands.end() ; ++walker)
      {
        multiActionList.push_back(walker->multi_action().actions());
      }

      action_list resultmultiactionlist;
      size_t multiactioncount= multiActionList[0].size(); // The number of multi actions.
      for (; multiactioncount>0 ; multiactioncount--)
      {
        data_expression_list resultf;
        // fcnt is the arity of the action with index multiactioncount-1;
        // const action a= *(multiActionList[0].begin()+(multiactioncount-1));
        action_list::const_iterator a=multiActionList[0].begin();
        for (size_t i=1 ; i<multiactioncount ; ++i,++a) {}
        // const action a= *((multiActionList[0]).begin()+(multiactioncount-1));
        size_t fcnt=(a->arguments()).size();
        data_expression f;

        for (; fcnt>0 ; fcnt--)
        {
          data_expression_list auxresult;
          data_expression equalterm;
          bool equaluptillnow=true;
          std::vector < variable_list >  ::const_iterator auxrename_list_pars=rename_list_pars.begin();
          std::vector < data_expression_list >::const_iterator auxrename_list_args=rename_list_args.begin();
          std::vector<action_list>::const_iterator  multiactionwalker=multiActionList.begin();
          for (stochastic_action_summand_vector::const_iterator walker=action_summands.begin(); walker!=action_summands.end();
               ++walker,++multiactionwalker)
          {
            assert(auxrename_list_pars!=rename_list_pars.end());
            assert(auxrename_list_args!=rename_list_args.end());
            const variable_list auxpars= *auxrename_list_pars;
            ++auxrename_list_pars;
            const data_expression_list auxargs= *auxrename_list_args;
            ++auxrename_list_args;
            // f is the fcnt-th argument of the multiactioncount-th action in the list
            action_list::const_iterator a1=multiactionwalker->begin();
            for (size_t i=1; i<multiactioncount; ++i, ++a1) {};
            data_expression_list::const_iterator d1=(a1->arguments()).begin();
            for (size_t i=1; i<fcnt; ++i, ++d1) {};
            f= *d1;
            std::map<variable,data_expression> sigma;
            std::set<variable> variables_in_rhs_sigma;
            data_expression_list::const_iterator j=auxargs.begin();
            for (variable_list::const_iterator i=auxpars.begin();
                 i!=auxpars.end(); ++i, ++j)
            {
              /* Substitutions are carried out from left to right. The first applicable substitution counts */
              if (sigma.count(*i)==0)
              {
                sigma[*i]=*j;
                std::set<variable> varset=find_free_variables(*j);
                variables_in_rhs_sigma.insert(varset.begin(),varset.end());
              }
            }

            mutable_map_substitution<> mutable_sigma(sigma);
            const data_expression auxresult1=data::replace_variables_capture_avoiding(f,mutable_sigma,variables_in_rhs_sigma);

            if (equalterm==data_expression()||is_global_variable(equalterm))
            {
              equalterm=auxresult1;
            }
            else
            {
              if (equaluptillnow)
              {
                equaluptillnow=((equalterm==auxresult1)||is_global_variable(auxresult1));
              }
            }
            auxresult.push_front(auxresult1);
          }
          if (equaluptillnow)
          {
            resultf.push_front(equalterm);
          }
          else
          {
            if (!options.binary)
            {
              data_expression_list tempauxresult=auxresult;
              tempauxresult.push_front(data_expression(e.var));
              resultf.push_front(data_expression(application(
                                                   find_case_function(e.enumeratedtype_index,f.sort()),
                                                   tempauxresult)));
            }
            else
            {
              data_expression temp=construct_binary_case_tree(
                                     n,
                                     resultsum,
                                     auxresult,
                                     f.sort(),
                                     e);
              resultf.push_front(temp);
            }
          }
        }
        a=multiActionList[0].begin();
        for (size_t i=1 ; i<multiactioncount ; ++i,++a) {}
        resultmultiactionlist.push_front(action(a->label(),resultf));
      }

      /* Construct resulttime, the time of the action ... */

      equaluptillnow=true;
      equalterm=data_expression();
      bool some_summand_has_time=false;
      bool all_summands_have_time=true;

      // first find out whether there is a summand with explicit time.
      for (stochastic_action_summand_vector::const_iterator walker=action_summands.begin() ; walker!=action_summands.end(); ++walker)
      {
        if (walker->has_time())
        {
          some_summand_has_time=true;
        }
        else
        {
          all_summands_have_time=false;
        }
      }

      if (some_summand_has_time)
      {
        variable dummy_time_variable;
        if (!all_summands_have_time)
        {
          // Generate a fresh dummy variable, and add it to the summand variables
          dummy_time_variable=get_fresh_variable("dt",sort_real::real_());
          resultsum.push_front(dummy_time_variable);
        }
        std::vector < variable_list >::const_iterator auxrename_list_pars=rename_list_pars.begin();
        std::vector < data_expression_list >::const_iterator auxrename_list_args=rename_list_args.begin();
        data_expression_list auxresult;
        for (stochastic_action_summand_vector::const_iterator walker=action_summands.begin(); walker!=action_summands.end(); ++walker)
        {
          if (walker->has_time())
          {
            const data_expression actiontime=walker->multi_action().time();

            assert(auxrename_list_pars!=rename_list_pars.end());
            assert(auxrename_list_args!=rename_list_args.end());
            const variable_list auxpars= *auxrename_list_pars;
            ++auxrename_list_pars;
            const data_expression_list auxargs= *auxrename_list_args;
            ++auxrename_list_args;

            std::map < variable, data_expression > sigma;
            std::set<variable> variables_in_rhs_sigma;
            data_expression_list::const_iterator j=auxargs.begin();
            for (variable_list::const_iterator i=auxpars.begin();
                 i!=auxpars.end(); ++i, ++j)
            {
              /* Substitutions are carried out from left to right. The first applicable substitution counts */
              if (sigma.count(*i)==0)
              {
                sigma[*i]=*j;
                std::set<variable> varset=find_free_variables(*j);
                variables_in_rhs_sigma.insert(varset.begin(),varset.end());
              }
            }

            mutable_map_substitution<> mutable_sigma(sigma);
            const data_expression auxresult1=data::replace_variables_capture_avoiding(actiontime, mutable_sigma,variables_in_rhs_sigma);
            if (equalterm==data_expression()||is_global_variable(equalterm))
            {
              equalterm=auxresult1;
            }
            else
            {
              if (equaluptillnow)
              {
                equaluptillnow=((auxresult1==equalterm)||is_global_variable(auxresult1));
              }
            }
            auxresult.push_front(auxresult1);
          }
          else
          {
            // this summand does not have time. But some summands have.
            auxresult.push_front(data_expression(dummy_time_variable));
            equaluptillnow=false;
          }
        }
        if (options.binary==1)
        {
          resulttime=construct_binary_case_tree(n,
                                                resultsum,auxresult,sort_real::real_(),e);
        }
        else
        {
          if (equaluptillnow)
          {
            resulttime=equalterm;
          }
          else
          {
            data_expression_list tempauxresult=auxresult;
            tempauxresult.push_front(data_expression(e.var));
            resulttime=application(
                         find_case_function(e.enumeratedtype_index,sort_real::real_()),
                         tempauxresult);
          }
        }
      }

      /* we construct the resulting distribution --------------------------------------------------------- */


      variable_list resulting_stochastic_variables;
      std::vector < variable_list > stochastic_rename_list_pars;
      std::vector < data_expression_list > stochastic_rename_list_args;
      data_expression resulting_distribution=real_one();
      {
        data_expression_list stochastic_conditionlist;
        for (stochastic_action_summand_vector::const_iterator walker=action_summands.begin(); walker!=action_summands.end() ; ++walker)
        {
          const variable_list stochastic_vars=walker->distribution().variables();
          resulting_stochastic_variables=merge_var(stochastic_vars,resulting_stochastic_variables,
                              stochastic_rename_list_pars,stochastic_rename_list_args,stochastic_conditionlist,parameters);
        }

        std::vector < variable_list >::const_iterator auxrename_list_pars=rename_list_pars.begin();
        std::vector < data_expression_list >::const_iterator auxrename_list_args=rename_list_args.begin();

        std::vector < variable_list >::const_iterator stochastic_auxrename_list_pars=stochastic_rename_list_pars.begin();
        std::vector < data_expression_list >::const_iterator stochastic_auxrename_list_args=stochastic_rename_list_args.begin();

        data_expression_list aux_result;
        data_expression equalterm;
        equaluptillnow=1;
        for (stochastic_action_summand_vector::const_iterator walker=action_summands.begin(); walker!=action_summands.end(); ++walker)
        {
          const stochastic_action_summand smmnd=*walker;
          const data_expression dist=smmnd.distribution().distribution();
          const variable_list stochastic_variables=smmnd.distribution().variables();
          assert(auxrename_list_pars!=rename_list_pars.end());
          assert(auxrename_list_args!=rename_list_args.end());
          assert(stochastic_auxrename_list_pars!=stochastic_rename_list_pars.end());
          assert(stochastic_auxrename_list_args!=stochastic_rename_list_args.end());

          const variable_list auxpars= *auxrename_list_pars;
          ++auxrename_list_pars;
          const data_expression_list auxargs= *auxrename_list_args;
          ++auxrename_list_args;
          const variable_list stochastic_auxpars= *stochastic_auxrename_list_pars;
          ++stochastic_auxrename_list_pars;
          const data_expression_list stochastic_auxargs= *stochastic_auxrename_list_args;
          ++stochastic_auxrename_list_args;
          std::map<variable,data_expression> sigma;
          std::set<variable> variables_in_rhs_of_sigma;
          data_expression_list::const_iterator j=stochastic_auxargs.begin();
          for (variable_list::const_iterator i=stochastic_auxpars.begin();
               i!=stochastic_auxpars.end(); ++i, ++j)
          {
            /* Substitutions are carried out from left to right. The first applicable substitution counts */
            if (sigma.count(*i)==0)
            {
              sigma[*i]=*j;
              const std::set<variable> varset=find_free_variables(*j);
              variables_in_rhs_of_sigma.insert(varset.begin(),varset.end());
            }
          }
          j=auxargs.begin();
          for (variable_list::const_iterator i=auxpars.begin();
               i!=auxpars.end(); ++i, ++j)
          {
            /* Substitutions are carried out from left to right. The first applicable substitution counts */
            if (sigma.count(*i)==0)
            {
              sigma[*i]=*j;
              const std::set<variable> varset=find_free_variables(*j);
              variables_in_rhs_of_sigma.insert(varset.begin(),varset.end());
            }
          }
          mutable_map_substitution<> mutable_sigma(sigma);

          const data_expression auxresult1=data::replace_variables_capture_avoiding(dist,mutable_sigma,variables_in_rhs_of_sigma);
          if (equalterm==data_expression()||is_global_variable(equalterm))
          {
            equalterm=auxresult1;
          }
          else
          {
            if (equaluptillnow)
            {
              equaluptillnow=((auxresult1==equalterm)||is_global_variable(auxresult1));
            }
          }
          aux_result.push_front(auxresult1);
        }
        if (options.binary)
        {
          resulting_distribution=construct_binary_case_tree(n,
                          resultsum,aux_result,sort_real::real_(),e);
          resulting_distribution=lazy::and_(
                            construct_binary_case_tree(n,
                                resultsum,stochastic_conditionlist,sort_bool::bool_(),e),
                            resulting_distribution);
        }
        else
        {
          if (equaluptillnow)
          {
            if (all_equal(stochastic_conditionlist))
            {
              if (stochastic_conditionlist.front()==sort_bool::true_())
              {
                resulting_distribution=equalterm;
              }
              else
              {
                resulting_distribution=real_times_optimized(
                                                  if_(stochastic_conditionlist.front(),real_one(),real_zero()),
                                                  equalterm);
              }
            }
            else
            {
              data_expression_list temp_stochastic_conditionlist=stochastic_conditionlist;
              temp_stochastic_conditionlist.push_front(data_expression(e.var));
              resulting_distribution=real_times_optimized(
                                if_(application(
                                      find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                                      temp_stochastic_conditionlist),
                                      real_one(),
                                      real_zero()),
                                equalterm);
            }
          }
          else
          {
            data_expression_list tempauxresult=aux_result;
            tempauxresult.push_front(data_expression(e.var));
            resulting_distribution=application(
                              find_case_function(e.enumeratedtype_index,sort_real::real_()),
                              tempauxresult);
            if (all_equal(stochastic_conditionlist))
            {
              if (stochastic_conditionlist.front()==sort_bool::true_())
              {
               resulting_distribution=real_times_optimized(
                                                 if_(stochastic_conditionlist.front(),real_one(),real_zero()),
                                                 resulting_distribution);
              }
              else
              {
               resulting_distribution=real_times_optimized(
                                                 if_(stochastic_conditionlist.front(),real_one(),real_zero()),
                                                 resulting_distribution);
              }
            }
            else
            {
             data_expression_list temp_stochastic_conditionlist=stochastic_conditionlist;
             temp_stochastic_conditionlist.push_front(data_expression(e.var));
             resulting_distribution=real_times_optimized(
                              if_(application(
                                     find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                                     temp_stochastic_conditionlist),
                                  real_one(),
                                  real_zero()),
                              resulting_distribution);
            }
          }
        }
      }
      /* now we construct the arguments of the invoked -----------------------------------------------------
         process, i.e. the new function g */
      size_t fcnt=0;
      data_expression_list resultnextstate;

      for (variable_list::const_iterator var_it=parameters.begin(); var_it!=parameters.end(); ++var_it)
      {
        equalterm=data_expression();
        equaluptillnow=1;
        std::vector < variable_list >::const_iterator auxrename_list_pars=rename_list_pars.begin();
        std::vector < data_expression_list >::const_iterator auxrename_list_args=rename_list_args.begin();
        std::vector < variable_list >::const_iterator stochastic_auxrename_list_pars=stochastic_rename_list_pars.begin();
        std::vector < data_expression_list >::const_iterator stochastic_auxrename_list_args=stochastic_rename_list_args.begin();
        data_expression_list auxresult;
        for (stochastic_action_summand_vector::const_iterator walker=action_summands.begin(); walker!=action_summands.end(); ++walker)
        {
          const assignment_list nextstate=walker->assignments();
          assert(auxrename_list_pars!=rename_list_pars.end());
          assert(auxrename_list_args!=rename_list_args.end());
          assert(stochastic_auxrename_list_pars!=stochastic_rename_list_pars.end());
          assert(stochastic_auxrename_list_args!=stochastic_rename_list_args.end());
          const variable_list auxpars= *auxrename_list_pars;
          ++auxrename_list_pars;
          const data_expression_list auxargs= *auxrename_list_args;
          ++auxrename_list_args;
          const variable_list stochastic_auxpars= *stochastic_auxrename_list_pars;
          ++stochastic_auxrename_list_pars;
          const data_expression_list stochastic_auxargs= *stochastic_auxrename_list_args;
          ++stochastic_auxrename_list_args;

          data_expression nextstateparameter;
          nextstateparameter=getRHSassignment(*var_it,nextstate);

          std::map < variable, data_expression > sigma;
          std::set<variable> variables_in_rhs_sigma;
          data_expression_list::const_iterator j=stochastic_auxargs.begin();
          for (variable_list::const_iterator i=stochastic_auxpars.begin();
                 i!=stochastic_auxpars.end(); ++i, ++j)
          {
            /* Substitutions are carried out from left to right. The first applicable substitution counts */
            if (sigma.count(*i)==0)
            {
              sigma[*i]=*j;
              std::set<variable> varset=find_free_variables(*j);
              variables_in_rhs_sigma.insert(varset.begin(),varset.end());
            }
          }
          j=auxargs.begin();
          for (variable_list::const_iterator i=auxpars.begin();
                 i!=auxpars.end(); ++i, ++j)
          {
            /* Substitutions are carried out from left to right. The first applicable substitution counts */
            if (sigma.count(*i)==0)
            {
              sigma[*i]=*j;
              std::set<variable> varset=find_free_variables(*j);
              variables_in_rhs_sigma.insert(varset.begin(),varset.end());
            }
          }

          mutable_map_substitution<> mutable_sigma(sigma);
          data_expression auxresult1=data::replace_variables_capture_avoiding(nextstateparameter, mutable_sigma,variables_in_rhs_sigma);
          if (equalterm==data_expression()) // ||is_global_variable(equalterm)) Adding this last part leads to smaller case functions,
                                            // but bigger and less structured state spaces, as parameters are less often put to default
                                            // values. Constant elimination can less often be applied as constant eliminiation does not
                                            // have subtle knowledge of case functions.
          {
            equalterm=auxresult1;
          }
          else if (equaluptillnow)
          {
            // set equaluptillnow if the arguments of this case function are all equal,
            // or are all equal to global variables. Setting a case function
            // C(e,v,dc1) to the value v, where dc1 is a global variable can result in
            // the growth of the state space, as dc1 is not set to a default value, but
            // keeps the value v.
            equaluptillnow=((equalterm==auxresult1)||
                               ((equalterm==data_expression()||is_global_variable(equalterm)) &&
                                                    is_global_variable(auxresult1)));
          }

          auxresult.push_front(auxresult1);
        }
        if (equaluptillnow)
        {
          resultnextstate.push_front(equalterm);
        }
        else
        {
          if (!options.binary)
          {
            data_expression_list tempauxresult=auxresult;
            tempauxresult.push_front(data_expression(e.var));
            resultnextstate.push_front(
                         data_expression(application(
                                           find_case_function(
                                             e.enumeratedtype_index,
                                             var_it->sort()),
                                           tempauxresult)));
          }
          else
          {
            data_expression temp=construct_binary_case_tree(
                                   n,
                                   resultsum,
                                   auxresult,
                                   var_it->sort(),
                                   e);
            resultnextstate.push_front(temp);
          }
        }
        fcnt++;
      }
      /* Now turn *resultg around */
      resultnextstate=reverse(resultnextstate);
      /* The list of arguments in nextstate are now in a sequential form, and
           must be transformed back to a list of assignments */
      const assignment_list final_resultnextstate=make_assignment_list(parameters,resultnextstate);
      return stochastic_action_summand(
                            resultsum,
                            resultcondition,
                            some_summand_has_time?multi_action(resultmultiactionlist,resulttime):multi_action(resultmultiactionlist),
                            final_resultnextstate,
                            stochastic_distribution(resulting_stochastic_variables,resulting_distribution));
    }

    deadlock_summand collect_sum_arg_arg_cond(
      const enumtype& e,
      size_t n,
      const deadlock_summand_vector& deadlock_summands,
      const variable_list& parameters)
    {
      /* This function gets a list of summands, with
         the same multiaction and time
         status. It yields a single clustered summand
         by introducing an auxiliary sum operator, with
         a variable of enumtype. In case binary is used,
         a sequence of variables are introduced of sort Bool */

      variable_list resultsum;
      data_expression resultcondition;
      action_list resultmultiaction;
      data_expression resulttime;

      std::vector < variable_list > rename_list_pars;
      std::vector < data_expression_list > rename_list_args;
      /* rename list is a list of pairs of variable and term lists */
      data_expression_list conditionlist;
      data_expression binarysumcondition;
      int equaluptillnow=1;

      for (deadlock_summand_vector::const_iterator walker=deadlock_summands.begin(); walker!=deadlock_summands.end() ; ++walker)
      {
        const variable_list sumvars=walker->summation_variables();
        resultsum=merge_var(sumvars,resultsum,rename_list_pars,rename_list_args,conditionlist,parameters);
      }

      if (options.binary)
      {
        resultsum=make_binary_sums(
                    n,
                    enumeratedtypes[e.enumeratedtype_index].sortId,
                    binarysumcondition,
                    resultsum);
      }
      else
      {
        resultsum.push_front(e.var);
      }

      /* we construct the resulting condition */
      data_expression_list auxresult;
      std::vector < variable_list >::const_iterator auxrename_list_pars=rename_list_pars.begin();
      std::vector < data_expression_list >::const_iterator auxrename_list_args=rename_list_args.begin();

      data_expression equalterm;
      equaluptillnow=1;
      for (deadlock_summand_vector::const_iterator walker=deadlock_summands.begin(); walker!=deadlock_summands.end(); ++walker)
      {
        const deadlock_summand smmnd=*walker;
        const data_expression condition=smmnd.condition();
        assert(auxrename_list_pars!=rename_list_pars.end());
        assert(auxrename_list_args!=rename_list_args.end());
        const variable_list auxpars= *auxrename_list_pars;
        ++auxrename_list_pars;
        const data_expression_list auxargs= *auxrename_list_args;
        ++auxrename_list_args;
        std::map < variable, data_expression > sigma;
        std::set<variable> variables_in_rhs_sigma;
        data_expression_list::const_iterator j=auxargs.begin();
        for (variable_list::const_iterator i=auxpars.begin();
             i!=auxpars.end(); ++i, ++j)
        {
          /* Substitutions are carried out from left to right. The first applicable substitution counts */
          if (sigma.count(*i)==0)
          {
            sigma[*i]=*j;
            std::set<variable> varset=find_free_variables(*j);
            variables_in_rhs_sigma.insert(varset.begin(),varset.end());
          }
        }

        mutable_map_substitution<> mutable_sigma(sigma);
        const data_expression auxresult1=data::replace_variables_capture_avoiding(condition, mutable_sigma,variables_in_rhs_sigma);
        if (equalterm==data_expression()||is_global_variable(equalterm))
        {
          equalterm=auxresult1;
        }
        else
        {
          if (equaluptillnow)
          {
            equaluptillnow=((auxresult1==equalterm)||is_global_variable(auxresult1));
          }
        }
        auxresult.push_front(auxresult1);
      }
      if (options.binary)
      {
        resultcondition=construct_binary_case_tree(n,
                        resultsum,auxresult,sort_bool::bool_(),e);
        resultcondition=lazy::and_(binarysumcondition,resultcondition);
        resultcondition=lazy::and_(
                          construct_binary_case_tree(n,
                              resultsum,conditionlist,sort_bool::bool_(),e),
                          resultcondition);
      }
      else
      {
        if (equaluptillnow)
        {
          if (all_equal(conditionlist))
          {
            resultcondition=lazy::and_(conditionlist.front(),equalterm);
          }
          else
          {
            data_expression_list tempconditionlist=conditionlist;
            tempconditionlist.push_front(data_expression(e.var));
            resultcondition=lazy::and_(
                            application(
                              find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                              tempconditionlist),
                            equalterm);
          }
        }
        else
        {
          data_expression_list tempauxresult=auxresult;
          tempauxresult.push_front(data_expression(e.var));
          resultcondition=application(
                            find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                            tempauxresult);
          if (all_equal(conditionlist))
          {
            resultcondition=lazy::and_(conditionlist.front(),resultcondition);
          }
          else
          {
            data_expression_list tempconditionlist=conditionlist;
            tempconditionlist.push_front(data_expression(e.var));
            resultcondition=lazy::and_(
                            application(
                              find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                              tempconditionlist),
                            resultcondition);
          }
        }
      }

      /* now we construct the arguments of the action */
      /* First we collect all multi-actions in a separate vector
         of multiactions */
      std::vector < action_list > multiActionList;

      action_list resultmultiactionlist;

      /* Construct resulttime, the time of the action ... */

      equaluptillnow=true;
      equalterm=data_expression();
      bool some_summand_has_time=false;
      bool all_summands_have_time=true;

      // first find out whether there is a summand with explicit time.
      for (deadlock_summand_vector::const_iterator walker=deadlock_summands.begin() ; walker!=deadlock_summands.end(); ++walker)
      {
        if (walker->deadlock().has_time())
        {
          some_summand_has_time=true;
        }
        else
        {
          all_summands_have_time=false;
        }
      }

      if ((some_summand_has_time))
      {
        variable dummy_time_variable;
        if (!all_summands_have_time)
        {
          // Generate a fresh dummy variable, and add it to the summand variables
          dummy_time_variable=get_fresh_variable("dt",sort_real::real_());
          resultsum.push_front(dummy_time_variable);
        }
        std::vector < variable_list >::const_iterator auxrename_list_pars=rename_list_pars.begin();
        std::vector < data_expression_list >::const_iterator auxrename_list_args=rename_list_args.begin();
        data_expression_list auxresult;
        for (deadlock_summand_vector::const_iterator walker=deadlock_summands.begin(); walker!=deadlock_summands.end(); ++walker)
        {
          if (walker->deadlock().has_time())
          {
            const data_expression actiontime=walker->deadlock().time();

            assert(auxrename_list_pars!=rename_list_pars.end());
            assert(auxrename_list_args!=rename_list_args.end());
            const variable_list auxpars= *auxrename_list_pars;
            ++auxrename_list_pars;
            const data_expression_list auxargs= *auxrename_list_args;
            ++auxrename_list_args;

            std::map < variable, data_expression > sigma;
            std::set<variable> variables_in_rhs_sigma;
            data_expression_list::const_iterator j=auxargs.begin();
            for (variable_list::const_iterator i=auxpars.begin();
                 i!=auxpars.end(); ++i, ++j)
            {
              /* Substitutions are carried out from left to right. The first applicable substitution counts */
              if (sigma.count(*i)==0)
              {
                sigma[*i]=*j;
                std::set<variable> varset=find_free_variables(*j);
                variables_in_rhs_sigma.insert(varset.begin(),varset.end());
              }
            }

            mutable_map_substitution<> mutable_sigma(sigma);
            const data_expression auxresult1=data::replace_variables_capture_avoiding(actiontime, mutable_sigma,variables_in_rhs_sigma);
            if (equalterm==data_expression()||is_global_variable(equalterm))
            {
              equalterm=auxresult1;
            }
            else
            {
              if (equaluptillnow)
              {
                equaluptillnow=((auxresult1==equalterm)||is_global_variable(auxresult1));
              }
            }
            auxresult.push_front(auxresult1);
          }
          else
          {
            // this summand does not have time. But some summands have.
            auxresult.push_front(data_expression(dummy_time_variable));
            equaluptillnow=false;
          }
        }
        if (options.binary==1)
        {
          resulttime=construct_binary_case_tree(n,
                                                resultsum,auxresult,sort_real::real_(),e);
        }
        else
        {
          if (equaluptillnow)
          {
            resulttime=equalterm;
          }
          else
          {
            data_expression_list tempauxresult=auxresult;
            tempauxresult.push_front(data_expression(e.var));
            resulttime=application(
                         find_case_function(e.enumeratedtype_index,sort_real::real_()),
                         tempauxresult);
          }
        }
      }

      return deadlock_summand(resultsum,
                              resultcondition,
                              some_summand_has_time?deadlock(resulttime):deadlock());
    }

    sort_expression_list getActionSorts(const action_list& actionlist)
    {
      sort_expression_list resultsorts;

      for (action_list::const_iterator i=actionlist.begin(); i!=actionlist.end(); ++i)
      {
        resultsorts=i->label().sorts()+resultsorts;
      }
      return resultsorts;
    }

    void cluster_actions(
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      const variable_list& pars)
    {
      {
        /* We cluster first the action summands with the action
            occurring in the first summand of sums.
            These summands are first stored in w1. */

        stochastic_action_summand_vector result;
        stochastic_action_summand_vector reducible_sumlist=action_summands;

        for (stochastic_action_summand_vector::const_iterator i=action_summands.begin() ; i!=action_summands.end() ; ++i)
        {
          const stochastic_action_summand summand1=*i;

          stochastic_action_summand_vector w1;
          stochastic_action_summand_vector w2;
          for (stochastic_action_summand_vector::const_iterator w3=reducible_sumlist.begin(); w3!=reducible_sumlist.end(); ++w3)
          {
            const stochastic_action_summand summand2=*w3;
            if (summandsCanBeClustered(summand1,summand2))
            {
              w1.push_back(summand2);
            }
            else
            {
              w2.push_back(summand2);
            }
          }
          reducible_sumlist.swap(w2);

          /* In w1 we now find all the summands labelled with
             similar multiactions, actiontime and terminationstatus.
             We must now construct its clustered form. */
          size_t n=w1.size();

          if (n>0)
          {
            if (n>1)
            {
              const action_list multiaction=w1.front().multi_action().actions();
              sort_expression_list actionsorts;
              actionsorts=getActionSorts(multiaction);

              const enumtype enumeratedtype_(options.binary?2:n,actionsorts,get_sorts(pars),*this);

              result.push_back(collect_sum_arg_arg_cond(enumeratedtype_,n,w1,pars));
            }
            else
            {
              // result=w1 + result;
              for(stochastic_action_summand_vector::const_iterator i=result.begin(); i!=result.end(); ++i)
              {
                w1.push_back(*i);
              }
              w1.swap(result);
            }
          }
        }
        action_summands=result;
      }

      // Now the delta summands are clustered.
      deadlock_summand_vector result;
      deadlock_summand_vector reducible_sumlist=deadlock_summands;

      for (deadlock_summand_vector::const_iterator i=deadlock_summands.begin() ; i!=deadlock_summands.end() ; ++i)
      {
        const deadlock_summand summand1=*i;

        deadlock_summand_vector w1;
        deadlock_summand_vector w2;
        for (deadlock_summand_vector::const_iterator w3=reducible_sumlist.begin(); w3!=reducible_sumlist.end(); ++w3)
        {
          const deadlock_summand summand2=*w3;
          if (summand1.deadlock().has_time()!=summand2.deadlock().has_time())
          {
            w1.push_back(summand2);
          }
          else
          {
            w2.push_back(summand2);
          }
        }
        reducible_sumlist.swap(w2);

        /* In w1 we now find all the summands labelled with
           similar multiactions, actiontime and terminationstatus.
           We must now construct its clustered form. */
        size_t n=w1.size();

        if (n>0)
        {
          if (n>1)
          {
            sort_expression_list actionsorts;

            const enumtype enumeratedtype_(options.binary?2:n,actionsorts,get_sorts(pars),*this);

            result.push_back(collect_sum_arg_arg_cond(enumeratedtype_,n,w1,pars));
          }
          else
          {
            // result=w1 + result;
            for(deadlock_summand_vector::const_iterator i=w1.begin(); i!=w1.end(); ++i)
            {
              result.push_back(*i);
            }
          }
        }
      }
      deadlock_summands.swap(result);
    }

    /**************** GENERaTE LPEpCRL **********************************/


    /* The variable regular indicates that we are interested in generating
       a LPE assuming the pCRL term under consideration is regular */

    void generateLPEpCRL(stochastic_action_summand_vector& action_summands,
                         deadlock_summand_vector& deadlock_summands,
                         const process_identifier& procId,
                         const bool containstime,
                         const bool regular,
                         variable_list& parameters,
                         assignment_list& init,
                         stochastic_distribution& initial_stochastic_distribution)
    /* A pair of initial state and linear process must be extracted
       from the underlying GNF */
    {
      // We use action_summands and deadlock_summands as an output.
      assert(action_summands.size()==0);
      assert(deadlock_summands.size()==0);

      bool singlecontrolstate=false;
      size_t n=objectIndex(procId);

      std::vector < process_identifier > pCRLprocs;
      pCRLprocs.push_back(procId);
      makepCRLprocs(objectdata[n].processbody,pCRLprocs);
      /* now pCRLprocs contains a list of all process id's in this
         pCRL process */

      /* collect the parameters, but assume that variables
         have a unique sort */
      if (pCRLprocs.size()==1)
      {
        singlecontrolstate=true;
      }
      parameters=collectparameterlist(pCRLprocs);

      if ((!regular)||((!singlecontrolstate) && (options.newstate) && (!options.binary)))
      {
        declare_control_state(pCRLprocs);
      }
      stacklisttype stack(parameters,*this,regular,pCRLprocs,singlecontrolstate);

      if (regular)
      {
        if ((options.binary) && (options.newstate))
        {
          parameters=stack.parameters;
          if (!singlecontrolstate)
          {
            parameters=reverse(stack.booleanStateVariables) + parameters;
          }
        }
        else  /* !binary or oldstate */
        {
          variable_list tempparameters=stack.parameters;
          tempparameters.push_front(stack.stackvar);
          parameters=
            ((!singlecontrolstate)?tempparameters:stack.parameters);
        }
      }
      else /* not regular, use a stack */
      {
        parameters = { stack.stackvar };
      }

      init=make_initialstate(procId,stack,pCRLprocs,regular,singlecontrolstate,initial_stochastic_distribution);

      collectsumlist(action_summands,deadlock_summands,pCRLprocs,parameters,stack,regular,singlecontrolstate);

      if (!options.no_intermediate_cluster)
      {
        cluster_actions(action_summands,deadlock_summands,parameters);
      }

      if ((!containstime) || options.add_delta)
      {
        /* We add a delta summand to each process, if the flag
           add_delta is set, or if the process does not contain any
           explicit reference to time. This affects the behaviour of each
           process in the sense that each process can idle
           indefinitely. It has the advantage that large numbers
           numbers of timed delta summands are subsumed by
           this delta. As the removal of timed delta's
           is time consuming in the linearisation, the use
           of this flag, can speed up linearisation considerably */
        deadlock_summands.push_back(deadlock_summand(variable_list(),sort_bool::true_(),deadlock()));
      }
    }


    /**************** hiding *****************************************/

    action_list hide_(const identifier_string_list& hidelist, const action_list& multiaction)
    {
      action_list resultactionlist;

      for (action_list::const_iterator walker=multiaction.begin();
           walker!=multiaction.end(); ++walker)
      {
        if (std::find(hidelist.begin(),hidelist.end(),walker->label().name())==hidelist.end())
        {
          resultactionlist.push_front(*walker);
        }
      }

      /* reverse the actionlist to maintain the ordering */
      return reverse(resultactionlist);
    }

    void hidecomposition(const identifier_string_list& hidelist, stochastic_action_summand_vector& action_summands)
    {
      for (stochastic_action_summand_vector::iterator i=action_summands.begin(); i!=action_summands.end() ; ++i)
      {
        const action_list acts=hide_(hidelist,i->multi_action().actions());
        *i=stochastic_action_summand(i->summation_variables(),
                          i->condition(),
                          i->has_time()?multi_action(acts,i->multi_action().time()):multi_action(acts),
                          i->assignments(),
                          i->distribution());
      }
    }

    /**************** allow/block *************************************/

    bool implies_condition(const data_expression& c1, const data_expression& c2)
    {
      if (c2==sort_bool::true_())
      {
        return true;
      }

      if (c1==sort_bool::false_())
      {
        return true;
      }

      if (c1==sort_bool::true_())
      {
        return false;
      }

      if (c2==sort_bool::false_())
      {
        return false;
      }

      if (c1==c2)
      {
        return true;
      }

      /* Dealing with the conjunctions (&&) first and then the disjunctions (||)
         yields a 10-fold speed increase compared to the case where first the
         || occur, and then the &&. This result was measured on the alternating
         bit protocol, with --regular. */

      if (sort_bool::is_and_application(c2))
      {
        return implies_condition(c1,data::binary_left(application(c2))) &&
               implies_condition(c1,data::binary_right(application(c2)));
      }

      if (sort_bool::is_or_application(c1))
      {
        return implies_condition(data::binary_left(application(c1)),c2) &&
               implies_condition(data::binary_right(application(c1)),c2);
      }

      if (sort_bool::is_and_application(c1))
      {
        return implies_condition(data::binary_left(application(c1)),c2) ||
               implies_condition(data::binary_right(application(c1)),c2);
      }

      if (sort_bool::is_or_application(c2))
      {
        return implies_condition(c1,data::binary_left(application(c2))) ||
               implies_condition(c1,data::binary_right(application(c2)));
      }

      return false;
    }

    void insert_timed_delta_summand(
      const stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      const deadlock_summand& s)
    {
      /* The delta summands are put in front.
         The sequence of summands is maintained as
         good as possible, to eliminate summands as
         quickly as possible */
      deadlock_summand_vector result;

      // const variable_list sumvars=s.summation_variables();
      const data_expression cond=s.condition();
      const data_expression actiontime=s.deadlock().time();

      // First check whether the delta summand is subsumed by an action summands.
      for (stochastic_action_summand_vector::const_iterator i=action_summands.begin(); i!=action_summands.end(); ++i)
      {
        const data_expression cond1=i->condition();
        if ((!options.add_delta) &&
            ((actiontime==i->multi_action().time()) || (!i->multi_action().has_time())) &&
            (implies_condition(cond,cond1)))
        {
          /* De delta summand is subsumed by action summand *i. So, it does not
             have to be added. */

          return;
        }
      }

      for (deadlock_summand_vector::iterator i=deadlock_summands.begin(); i!=deadlock_summands.end(); ++i)
      {
        const deadlock_summand smmnd=*i;
        const data_expression cond1=i->condition();
        if ((!options.add_delta) &&
            ((actiontime==i->deadlock().time()) || (!i->deadlock().has_time())) &&
            (implies_condition(cond,cond1)))
        {
          /* put the summand that was effective in removing
             this delta summand to the front, such that it
             is encountered early later on, removing a next
             delta summand */

          copy(i,deadlock_summands.end(),back_inserter(result));
          deadlock_summands.swap(result);
          return;
        }
        if (((options.add_delta)||
             (((actiontime==smmnd.deadlock().time())|| (!s.deadlock().has_time())) &&
              (implies_condition(cond1,cond)))))
        {
          /* do not add summand to result, as it is superseded by s */
        }
        else
        {
          result.push_back(smmnd);
        }
      }

      result.push_back(s);
      deadlock_summands.swap(result);
    }

    action_name_multiset_list sortMultiActionLabels(const action_name_multiset_list& l)
    {
      action_name_multiset_list result;
      for (action_name_multiset_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        result.push_front(sortActionLabels(*i));
      }
      return result;
    }

    /// \brief determine whether the multiaction has the same labels as the allow action,
    //         in which case true is delivered. If multiaction is the action Terminate,
    //         then true is also returned.

    bool allowsingleaction(const action_name_multiset& allowaction,
                           const action_list& multiaction)
    {
      if (multiaction == action_list({ terminationAction }))
      {
        // multiaction is equal to terminate. This action cannot be blocked.
        return true;
      }
      const identifier_string_list names=allowaction.names();
      identifier_string_list::const_iterator i=names.begin();

      for (action_list::const_iterator walker=multiaction.begin();
           walker!=multiaction.end(); ++walker,++i)
      {
        if (i==names.end())
        {
          return false;
        }
        if (*i!=walker->label().name())
        {
          return false;
        }
      }
      if (i==names.end())
      {
        return true;
      }
      return false;
    }

    bool allow_(const action_name_multiset_list& allowlist,
                const action_list& multiaction)
    {
      /* The empty multiaction, i.e. tau, is never blocked by allow */
      if (multiaction.empty())
      {
        return true;
      }

      for (action_name_multiset_list::const_iterator i=allowlist.begin();
           i!=allowlist.end(); ++i)
      {
        if (allowsingleaction(*i,multiaction))
        {
          return true;
        }
      }
      return false;
    }

    bool encap(const identifier_string_list& encaplist, const action_list& multiaction)
    {
      for (action_list::const_iterator walker=multiaction.begin();
           walker!=multiaction.end(); ++walker)
      {
        for (identifier_string_list::const_iterator i=encaplist.begin(); i!=encaplist.end(); ++i)
        {
          const identifier_string s1= *i;
          const identifier_string s2=walker->label().name();
          if (s1==s2)
          {
            return true;
          }
        }
      }
      return false;
    }

    void allowblockcomposition(
      const action_name_multiset_list& allowlist1,  // This is a list of list of identifierstring.
      const bool is_allow,
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands)
    {
      /* This function calculates the allow or the block operator,
         depending on whether is_allow is true */

      stochastic_action_summand_vector sourcesumlist;
      action_summands.swap(sourcesumlist);

      deadlock_summand_vector resultdeltasumlist;
      deadlock_summand_vector resultsimpledeltasumlist;
      deadlock_summands.swap(resultdeltasumlist);

      action_name_multiset_list allowlist((is_allow)?sortMultiActionLabels(allowlist1):allowlist1);

      size_t sourcesumlist_length=sourcesumlist.size();
      if (sourcesumlist_length>2 || is_allow) // This condition prevents this message to be printed
        // when performing data elimination. In this case the
        // term delta is linearised, to determine which data
        // is essential for all processes. In these cases a
        // message about the block operator is very confusing.
      {
        mCRL2log(mcrl2::log::verbose) << "- calculating the " << (is_allow?"allow":"block") <<
              " operator on " << sourcesumlist.size() << " action summands and " << resultdeltasumlist.size() << " delta summands ";
      }

      /* First add the resulting sums in two separate lists
         one for actions, and one for delta's. The delta's
         are added at the end to the actions, where for
         each delta summand it is determined whether it ought
         to be added, or is superseded by an action or another
         delta summand */
      for (stochastic_action_summand_vector::const_iterator i=sourcesumlist.begin(); i!=sourcesumlist.end(); ++i)
      {
        const stochastic_action_summand smmnd= *i;
        const variable_list sumvars=smmnd.summation_variables();
        const action_list multiaction=smmnd.multi_action().actions();
        const data_expression actiontime=smmnd.multi_action().time();
        const data_expression condition=smmnd.condition();


        if ((is_allow && allow_(allowlist,multiaction)) ||
             (!is_allow && !encap(deprecated_cast<identifier_string_list>(allowlist),multiaction)))
        {
          action_summands.push_back(smmnd);
        }
        else
        {
          if (smmnd.has_time())
          {
            resultdeltasumlist.push_back(deadlock_summand(sumvars, condition, deadlock(actiontime)));
          }
          else
          {
            // summand has no time.
            if (condition==sort_bool::true_())
            {
              resultsimpledeltasumlist.push_back(deadlock_summand(sumvars, condition, deadlock()));
            }
            else
            {
              resultdeltasumlist.push_back(deadlock_summand(sumvars, condition, deadlock()));
            }
          }
        }
      }

      if (options.nodeltaelimination)
      {
        deadlock_summands.swap(resultsimpledeltasumlist);
        copy(resultdeltasumlist.begin(),resultdeltasumlist.end(),back_inserter(deadlock_summands));
      }
      else
      {
        if (!options.add_delta) /* if a delta summand is added, conditional, timed
                                   delta's are subsumed and do not need to be added */
        {
          for (deadlock_summand_vector::const_iterator j=resultsimpledeltasumlist.begin();
               j!=resultsimpledeltasumlist.end(); ++j)
          {
            insert_timed_delta_summand(action_summands,deadlock_summands,*j);
          }
          for (deadlock_summand_vector::const_iterator j=resultdeltasumlist.begin();
               j!=resultdeltasumlist.end(); ++j)
          {
            insert_timed_delta_summand(action_summands,deadlock_summands,*j);
          }
        }
        else
        {
          // Add a true -> delta
          insert_timed_delta_summand(action_summands,deadlock_summands,deadlock_summand(variable_list(),sort_bool::true_(),deadlock()));
        }
      }
      if (mCRL2logEnabled(mcrl2::log::verbose) && (sourcesumlist_length>2 || is_allow))
      {
        mCRL2log(mcrl2::log::verbose) << ", resulting in " << action_summands.size() << " action summands and " << deadlock_summands.size() << " delta summands\n";
      }
    }

    /**************** renaming ******************************************/

    action rename_action(const rename_expression_list& renamings, const action& act)
    {
      const action_label actionId=act.label();
      const identifier_string s=actionId.name();
      for (rename_expression_list::const_iterator i=renamings.begin(); i!=renamings.end(); ++i)
      {
        if (s==i->source())
        {
          return action(action_label(i->target(),actionId.sorts()),
                        act.arguments());
        }
      }
      return act;
    }

    action_list rename_actions(const rename_expression_list& renamings,
                               const action_list& multiaction)
    {
      action_list resultactionlist;

      for (action_list::const_iterator walker=multiaction.begin();
           walker!=multiaction.end(); ++walker)
      {
        resultactionlist=linInsertActionInMultiActionList(
                           rename_action(renamings,*walker),
                           resultactionlist);
      }
      return resultactionlist;
    }

    void renamecomposition(
      const rename_expression_list& renamings,
      stochastic_action_summand_vector& action_summands)
    {
      for (stochastic_action_summand_vector::iterator i=action_summands.begin(); i!=action_summands.end(); ++i)
      {
        const action_list actions=rename_actions(renamings,i->multi_action().actions());

        *i= stochastic_action_summand(i->summation_variables(),
                           i->condition(),
                           i->multi_action().has_time()?multi_action(actions,i->multi_action().time()):multi_action(actions),
                           i->assignments(),
                           i->distribution());

      }
    }

    /**************** equalargs ****************************************/

    bool occursinvarandremove(const variable& var, variable_list& vl)
    {
      bool result=false;

      if (vl.empty())
      {
        return 0;
      }
      vl.pop_front();
      const variable var1=vl.front();
      if (var==var1)
      {
        return true;
      }

      // Names of variables cannot be the same, even if they have different types.
      if (var.name()==var1.name())
      {
        throw mcrl2::runtime_error("variable conflict " + data::pp(var) + ":" + data::pp(var.sort()) + " versus " +
                                   data::pp(var1) + ":" + data::pp(var1.sort()) + ".");
      }

      result=occursinvarandremove(var,vl);
      vl.push_front(var1);
      return result;
    }

    /********************** construct renaming **************************/

    variable_list construct_renaming(
      const variable_list& pars1,
      const variable_list& pars2,
      variable_list& pars3,
      variable_list& pars4,
      const bool unique=true)
    {
      /* check whether the variables in pars2 are unique,
         wrt to those in pars1, and deliver:
         - in pars3 a list of renamed parameters pars2, such that
           pars3 is unique with respect to pars1;
         - in pars4 a list of parameters that need to be renamed;
         - as a return result, new values for the parameters in pars4.
           This allows using substitute_data(list) to rename
           action and process arguments and conditions to adapt
           to the new parameter names.
         The variable unique indicates whether the generated variables
         are unique, and not occurring elsewhere. If unique is false,
         it is attempted to reuse previously generated variable names,
         as long as they do not occur in pars1. The default value for
         unique is true.
       */

      variable_list t, t1, t2;

      if (pars2.empty())
      {
        pars3=variable_list();
        pars4=variable_list();
      }
      else
      {
        variable var2=pars2.front();
        variable var3=var2;
        for (int i=0 ; occursin(var3,pars1) ; ++i)
        {
          var3=get_fresh_variable(var2.name(),var2.sort(),(unique?-1:i));
        }
        if (var3!=var2)
        {
          t1=construct_renaming(pars1,pars2.tail(),t,t2,unique);
          t1.push_front(var3);

          pars4=t2;
          pars4.push_front(var2);
          pars3=t;
          pars3.push_front(var3);
        }
        else
        {
          t1=construct_renaming(pars1,pars2.tail(),t,pars4,unique);
          pars3=t;
          pars3.push_front(var2);
        }

      }
      return t1;
    }

    /**************** communication operator composition ****************/

    identifier_string_list insertActionLabel(
      const identifier_string& action,
      const identifier_string_list& actionlabels)
    {
      /* assume actionlabels is sorted, and put
         action at the proper place to yield a sorted
         list */
      if (actionlabels.empty())
      {
        return { action };
      }

      const identifier_string firstAction=actionlabels.front();

      if (std::string(action)<std::string(firstAction))
      {
        identifier_string_list result=actionlabels;
        result.push_front(action);
        return result;
      }
      identifier_string_list result=insertActionLabel(action,actionlabels.tail());
      result.push_front(firstAction);
      return result;
    }

    action_name_multiset sortActionLabels(const action_name_multiset& actionlabels1)
    {
      identifier_string_list result;
      const identifier_string_list actionlabels(actionlabels1.names());
      for (identifier_string_list::const_iterator i=actionlabels.begin(); i!=actionlabels.end(); ++i)
      {
        result=insertActionLabel(*i,result);
      }
      return action_name_multiset(result);
    }

    template <typename List>
    sort_expression_list get_sorts(const List& l)
    {
      if (l.empty())
      {
        return sort_expression_list();
      }
      sort_expression_list result=get_sorts(l.tail());
      result.push_front(l.front().sort());
      return result;
    }

    // Check that the sorts of both termlists match.
    data_expression pairwiseMatch(const data_expression_list& l1, const data_expression_list& l2)
    {
      if (l1.empty())
      {
        if (l2.empty())
        {
          return sort_bool::true_();
        }
        return sort_bool::false_();
      }

      if (l2.empty())
      {
        return sort_bool::false_();
      }

      const data_expression t1=l1.front();
      const data_expression t2=l2.front();

      if (t1.sort()!=t2.sort())
      {
        return sort_bool::false_();
      }

      data_expression result=pairwiseMatch(l1.tail(),l2.tail());

      return lazy::and_(result,RewriteTerm(equal_to(t1,t2)));
    }

    // a tuple_list contains pairs of actions (multi-action) and the condition under which this action
    // can occur.
    typedef struct
    {
      std::vector < action_list > actions;
      std::vector < data_expression > conditions;
    } tuple_list;

    tuple_list addActionCondition(
      const action& firstaction,
      const data_expression& condition,
      const tuple_list& L,
      tuple_list S)
    {
      /* if firstaction==action(), it should not be added */
      assert(condition!=sort_bool::false_()); // It makes no sense to add an action with condition false,
      // as it cannot happen anyhow.
      for (size_t i=0; i<L.actions.size(); ++i)
      {
        S.actions.push_back((firstaction!=action())?
                            linInsertActionInMultiActionList(firstaction,L.actions[i]):
                            L.actions[i]);
        S.conditions.push_back(lazy::and_(L.conditions[i],condition));
      }
      return S;
    }


    // Type and variables for a somewhat more efficient storage of the
    // communication function

    class comm_entry:public boost::noncopyable
    {
      public:
        std::vector <identifier_string_list> lhs;
        std::vector <identifier_string> rhs;
        std::vector <identifier_string_list> tmp;
        std::vector< bool > match_failed;

        comm_entry(const communication_expression_list& communications)
        {
          for (communication_expression_list::const_iterator l=communications.begin();
               l!=communications.end(); ++l)
          {
            lhs.push_back(l->action_name().names());
            rhs.push_back(l->name());
            tmp.push_back(identifier_string_list());
            match_failed.push_back(false);
          }
        }

        ~comm_entry()
        {}

        size_t size() const
        {
          assert(lhs.size()==rhs.size() && rhs.size()==tmp.size() && tmp.size()==match_failed.size());
          return lhs.size();
        }
    };

    process::action_label can_communicate(const action_list& m, comm_entry& comm_table)
    {
      /* this function indicates whether the actions in m
         consisting of actions and data occur in C, such that
         a communication can take place. If not action_label() is delivered,
         otherwise the resulting action is the result. */
      // first copy the left-hand sides of communications for use
      for (size_t i=0; i<comm_table.size(); ++i)
      {
        comm_table.tmp[i] = comm_table.lhs[i];
        comm_table.match_failed[i]=false;
      }

      // m must match a lhs; check every action
      for (action_list::const_iterator mwalker=m.begin(); mwalker!=m.end(); ++mwalker)
      {
        identifier_string actionname=mwalker->label().name();

        // check every lhs for actionname
        bool comm_ok = false;
        for (size_t i=0; i<comm_table.size(); ++i)
        {
          if (comm_table.match_failed[i]) // lhs i does not match
          {
            continue;
          }
          if (comm_table.tmp[i].empty()) // lhs cannot match actionname
          {
            comm_table.match_failed[i]=true;
            continue;
          }
          if (actionname != comm_table.tmp[i].front())
          {
            // no match
            comm_table.match_failed[i] = true;
          }
          else
          {
            // possible match; on to next action
            comm_table.tmp[i].pop_front();
            comm_ok = true;
          }
        }
        if (!comm_ok)   // no (possibly) matching lhs
        {
          return action_label();
        }
      }

      // there is a lhs containing m; find it
      for (size_t i=0; i<comm_table.size(); ++i)
      {
        // lhs i matches only if comm_table[i] is empty
        if ((!comm_table.match_failed[i]) && comm_table.tmp[i].empty())
        {
          if (comm_table.rhs[i] == tau())
          {
            throw mcrl2::runtime_error("Cannot linearise a process with a communication operator, containing a communication that results in tau or that has an empty right hand side");
            return action_label();
          }
          return action_label(comm_table.rhs[i],m.front().label().sorts());
        }
      }
      // no match
      return action_label();
    }

    bool might_communicate(const action_list& m,
                           comm_entry& comm_table,
                           const action_list& n,
                           const bool n_is_null)
    {
      /* this function indicates whether the actions in m
         consisting of actions and data occur in C, such that
         a communication might take place (i.e. m is a subbag
         of the lhs of a communication in C).
         if n is not empty, then all actions of a matching communication
         that are not in m should be in n (i.e. there must be a
         subbag o of n such that m+o can communicate. */

      // first copy the left-hand sides of communications for use
      for (size_t i=0; i<comm_table.size(); ++i)
      {
        comm_table.match_failed[i]=false;
        comm_table.tmp[i] = comm_table.lhs[i];
      }

      // m must be contained in a lhs; check every action
      for (action_list::const_iterator mwalker=m.begin(); mwalker!=m.end(); ++mwalker)
      {
        const identifier_string actionname=mwalker->label().name();
        // check every lhs for actionname
        bool comm_ok = false;
        for (size_t i=0; i<comm_table.size(); ++i)
        {
          if (comm_table.match_failed[i])
          {
            continue;
          }
          if (comm_table.tmp[i].empty()) // actionname not in here; ignore lhs
          {
            comm_table.match_failed[i]=true;
            continue;
          }

          identifier_string commname;
          while (actionname != (commname = comm_table.tmp[i].front()))
          {
            if (!n_is_null)
            {
              // action is not in m, so it should be in n
              // but all actions in m come before n
              comm_table.match_failed[i]=true;
              comm_table.tmp[i]=identifier_string_list();
              break;
            }
            else
            {
              // ignore actions that are not in m
              comm_table.tmp[i].pop_front();
              if (comm_table.tmp[i].empty())
              {
                comm_table.match_failed[i]=true;
                break;
              }
            }
          }
          if (actionname==commname) // actionname found
          {
            comm_table.tmp[i].pop_front();
            comm_ok = true;
          }
        }
        if (!comm_ok)
        {
          return false;
        }
      }

      if (n_is_null)
      {
        // there is a matching lhs
        return true;
      }
      else
      {
        // the rest of actions of lhs that are not in m should be in n

        // rest[i] contains the part of n in which lhs i has to find matching actions
        // rest_is_null[i] contains indications whether rest[i] is NULL.
        std::vector < action_list > rest(comm_table.size(),n);
        std::vector < bool > rest_is_null(comm_table.size(),n_is_null);

        // check every lhs
        for (size_t i=0; i<comm_table.size(); ++i)
        {
          if (comm_table.match_failed[i]) // lhs i did not contain m
          {
            continue;
          }
          // as long as there are still unmatch actions in lhs i...
          while (!comm_table.tmp[i].empty())
          {
            // .. find them in rest[i]
            if (rest[i].empty()) // no luck
            {
              rest_is_null[i] = true;
              break;
            }
            // get first action in lhs i
            const identifier_string commname = comm_table.tmp[i].front();
            identifier_string restname;
            // find it in rest[i]
            while (commname!=(restname = rest[i].front().label().name()))
            {
              rest[i].pop_front();
              if (rest[i].empty()) // no more
              {
                rest_is_null[i] = true;
                break;
              }
            }
            if (commname!=restname) // action was not found
            {
              break;
            }
            // action found; try next
            rest[i].pop_front();
            comm_table.tmp[i].pop_front();
          }

          if (!rest_is_null[i]) // lhs was found in rest[i]
          {
            return true;
          }
        }

        // no lhs completely matches
        return false;
      }
    }

    tuple_list phi(const action_list& m,
                   const data_expression_list& d,
                   const action_list& w,
                   const action_list& n,
                   const action_list& r,
                   const bool r_is_null,
                   comm_entry& comm_table)
    {
      /* phi is a function that yields a list of pairs
         indicating how the actions in m|w|n can communicate.
         The pairs contain the resulting multi action and
         a condition on data indicating when communication
         can take place. In the communication all actions of
         m, none of w and a subset of n can take part in the
         communication. d is the data parameter of the communication
         and C contains a list of multiaction action pairs indicating
         possible communications */

      if (!might_communicate(m,comm_table,n,false))
      {
        return tuple_list();
      }
      if (n.empty())
      {
        process::action_label c=can_communicate(m,comm_table); /* returns action_label() if no communication
                                             is possible */
        if (c!=action_label())
        {
          const tuple_list T=makeMultiActionConditionList_aux(w,comm_table,r,r_is_null);
          return addActionCondition(
                   (c==action_label()?action():action(c,d)),  //Check. Nil kan niet geleverd worden.
                   sort_bool::true_(),
                   T,
                   tuple_list());
        }
        /* c==NULL, actions in m cannot communicate */
        return tuple_list();
      }
      /* if n=[a(f)] \oplus o */
      const action firstaction=n.front();
      const action_list o=n.tail();
      const data_expression condition=pairwiseMatch(d,firstaction.arguments());
      if (condition==sort_bool::false_())
      {
        action_list tempw=w;
        tempw=push_back(tempw,firstaction);
        return phi(m,d,tempw,o,r,r_is_null,comm_table);
      }
      else
      {
        action_list tempm=m;
        tempm=push_back(tempm,firstaction);
        const tuple_list T=phi(tempm,d,w,o,r,r_is_null,comm_table);
        action_list tempw=w;
        tempw=push_back(tempw,firstaction);
        return addActionCondition(
                 action(),
                 condition,
                 T,
                 phi(m,d,tempw,o,r,r_is_null,comm_table));
      }
    }

    bool xi(const action_list& alpha, const action_list& beta, comm_entry& comm_table)
    {
      if (beta.empty())
      {
        if (can_communicate(alpha,comm_table)!=action_label())
        {
          return true;
        }
        else
        {
          return false;
        }
      }
      else
      {
        const action a = beta.front();
        action_list l=alpha;
        l=push_back(l,a);
        const action_list beta_next = beta.tail();

        if (can_communicate(l,comm_table)!=action_label())
        {
          return true;
        }
        else if (might_communicate(l,comm_table,beta_next,false))
        {
          return xi(l,beta_next,comm_table) || xi(alpha,beta_next,comm_table);
        }
        else
        {
          return xi(alpha,beta_next,comm_table);
        }
      }
    }

    data_expression psi(const action_list& alpha_in, comm_entry& comm_table)
    {
      action_list alpha=reverse(alpha_in);
      data_expression cond = sort_bool::false_();
      while (!alpha.empty())
      {
        const action a = alpha.front();
        action_list beta = alpha.tail();

        while (!beta.empty())
        {
          const action_list actl = { a, beta.front() };
          if (might_communicate(actl,comm_table,beta.tail(),false) && xi(actl,beta.tail(),comm_table))
          {
            // sort and remove duplicates??
            cond = lazy::or_(cond,pairwiseMatch(a.arguments(),beta.front().arguments()));
          }
          beta.pop_front();
        }

        alpha.pop_front();
      }
      return lazy::not_(cond);
    }

    // returns a list of tuples.
    tuple_list makeMultiActionConditionList_aux(
      const action_list& multiaction,
      comm_entry& comm_table,
      const action_list& r,
      const bool r_is_null)
    {
      /* This is the function gamma(m,C,r) provided
         by Muck van Weerdenburg in Calculation of
         Communication with open terms [1]. */
      if (multiaction.empty())
      {
        tuple_list t;
        t.conditions.push_back((r_is_null)?sort_bool::true_():static_cast< data_expression const& >(psi(r,comm_table)));
        t.actions.push_back(action_list());
        return t;
      }

      const action firstaction=multiaction.front();
      const action_list remainingmultiaction=multiaction.tail(); /* This is m in [1] */

      const tuple_list S=phi({ firstaction },
                             firstaction.arguments(),
                             action_list(),
                             remainingmultiaction,
                             r,r_is_null,comm_table);
      action_list tempr=r;
      tempr.push_front(firstaction);
      const tuple_list T=makeMultiActionConditionList_aux(
                           remainingmultiaction,comm_table,
                           (r_is_null) ? action_list({ firstaction }) : tempr, false);
      return addActionCondition(firstaction,sort_bool::true_(),T,S);
    }

    tuple_list makeMultiActionConditionList(
      const action_list& multiaction,
      const communication_expression_list& communications)
    {
      comm_entry comm_table(communications);
      return makeMultiActionConditionList_aux(multiaction,comm_table,action_list(),true);
    }

    void communicationcomposition(
      const communication_expression_list& communications,
      const action_name_multiset_list& allowlist1,  // This is a list of list of identifierstring.
      const bool is_allow,                          // If is_allow or is_block is set, perform inline allow/block filtering.
      const bool is_block,
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands)

    {
      /* We follow the implementation of Muck van Weerdenburg, described in
         a note: Calculation of communication with open terms. */

      mCRL2log(mcrl2::log::verbose) <<
            (is_allow ? "- calculating the communication operator modulo the allow operator on " :
             is_block ? "- calculating the communication operator modulo the block operator on " :
                        "- calculating the communication operator on ") << action_summands.size() << " action summands";

      /* first we sort the multiactions in communications */
      communication_expression_list resultingCommunications;

      for (communication_expression_list::const_iterator i=communications.begin();
           i!=communications.end(); ++i)
      {
        const action_name_multiset source=i->action_name();
        const identifier_string target=i->name();
        if (is_nil(i->name()))  // Right hand side of communication is empty. We receive a bad datatype.
        {
          throw mcrl2::runtime_error("Right hand side of communication " + process::pp(source) + " in a comm command cannot be empty or tau");
        }
        resultingCommunications.push_front(communication_expression(sortActionLabels(source),target));
      }
      communication_expression_list communications1=resultingCommunications;

      stochastic_action_summand_vector resultsumlist;
      deadlock_summand_vector resultingDeltaSummands;
      deadlock_summands.swap(resultingDeltaSummands);

      bool inline_allow = is_allow || is_block;
      if (inline_allow)
      {
        // Inline allow is only supported for add_delta,
        // for in other cases generation of delta summands cannot be inlined in any simple way.
        assert(!options.nodeltaelimination && options.add_delta);
        deadlock_summands.push_back(deadlock_summand(variable_list(),sort_bool::true_(),deadlock()));
      }
      action_name_multiset_list allowlist((is_allow)?sortMultiActionLabels(allowlist1):allowlist1);

      for (stochastic_action_summand_vector::const_iterator sourcesumlist=action_summands.begin();
           sourcesumlist!=action_summands.end(); ++sourcesumlist)
      {
        const stochastic_action_summand smmnd=*sourcesumlist;
        const variable_list sumvars=smmnd.summation_variables();
        const action_list multiaction=smmnd.multi_action().actions();
        const data_expression condition=smmnd.condition();
        const assignment_list nextstate=smmnd.assignments();
        const stochastic_distribution dist=smmnd.distribution();

        if (!inline_allow)
        {
          /* Recall a delta summand for every non delta summand.
           * The reason for this is that with communication, the
           * conditions for summands can become much more complex.
           * Many of the actions in these summands are replaced by
           * delta's later on. Due to the more complex conditions it
           * will be hard to remove them. By adding a default delta
           * with a simple condition, makes this job much easier
           * later on, and will in general reduce the number of delta
           * summands in the whole system */

          /* But first remove free variables from sumvars */

          variable_list newsumvars;
          for (variable_list::const_iterator i=sumvars.begin(); i!=sumvars.end(); ++i)
          {
            const variable sumvar=*i;
            if (occursinterm(sumvar,condition) ||
                (smmnd.has_time() && occursinterm(sumvar,smmnd.multi_action().time())))
            {
              newsumvars.push_front(sumvar);
            }
          }
          newsumvars=reverse(newsumvars);

          resultingDeltaSummands.push_back(deadlock_summand(newsumvars,
                                                            condition,
                                                            smmnd.multi_action().has_time()?deadlock(smmnd.multi_action().time()):deadlock()));
        }

        /* the multiactionconditionlist is a list containing
           tuples, with a multiaction and the condition,
           expressing whether the multiaction can happen. All
           conditions exclude each other. Furthermore, the list
           is not empty. If no communications can take place,
           the original multiaction is delivered, with condition
           true. */
        const tuple_list multiactionconditionlist=
          makeMultiActionConditionList(
            multiaction,
            communications1);

        assert(multiactionconditionlist.actions.size()==
               multiactionconditionlist.conditions.size());
        for (size_t i=0 ; i<multiactionconditionlist.actions.size(); ++i)
        {
          const action_list multiaction=multiactionconditionlist.actions[i];

          if (is_allow && !allow_(allowlist,multiaction))
          {
            continue;
          }
          if (is_block && encap(deprecated_cast<identifier_string_list>(allowlist),multiaction))
          {
            continue;
          }

          const data_expression communicationcondition=
            RewriteTerm(multiactionconditionlist.conditions[i]);

          const data_expression newcondition=RewriteTerm(
                                               lazy::and_(condition,communicationcondition));
          stochastic_action_summand new_summand(sumvars,
                                     newcondition,
                                     smmnd.multi_action().has_time()?multi_action(multiaction, smmnd.multi_action().time()):multi_action(multiaction),
                                     nextstate,
                                     dist);
          if (!options.nosumelm)
          {
            if (sumelm(new_summand))
            {
              new_summand.condition() = RewriteTerm(new_summand.condition());
            }
          }

          if (new_summand.condition()!=sort_bool::false_())
          {
            resultsumlist.push_back(new_summand);
          }
        }

      }

      /* Now the resulting delta summands must be added again */

      action_summands.swap(resultsumlist);

      if (!inline_allow && !options.nodeltaelimination)
      {
        for (deadlock_summand_vector::const_iterator w=resultingDeltaSummands.begin();
             w!=resultingDeltaSummands.end(); ++w)
        {
          insert_timed_delta_summand(action_summands,deadlock_summands,*w);
        }
      }

      mCRL2log(mcrl2::log::verbose) << " resulting in " << action_summands.size() << " action summands and " << deadlock_summands.size() << " delta summands\n";
    }

    bool check_real_variable_occurrence(
      const variable_list& sumvars,
      const data_expression& actiontime,
      const data_expression& condition)
    {
      /* Check whether actiontime is an expression
         of the form t1 +...+ tn, where one of the
         ti is a variable in sumvars that does not occur in condition */

      if (is_variable(actiontime))
      {
        const variable& t = atermpp::down_cast<variable>(actiontime);
        if (occursintermlist(t, data_expression_list(sumvars)) && !occursinterm(t, condition))
        {
          return true;
        }
      }

      if (sort_real::is_plus_application(actiontime))
      {
        return (check_real_variable_occurrence(sumvars,data::binary_left(application(actiontime)),condition) ||
                check_real_variable_occurrence(sumvars,data::binary_right(application(actiontime)),condition));
      }

      return false;
    }

    data_expression makesingleultimatedelaycondition(
      const variable_list& sumvars,
      const variable_list& freevars,
      const data_expression& condition,
      const bool has_time,
      const variable& timevariable,
      const data_expression& actiontime,
      variable_list& used_sumvars)
    {
      /* Generate a condition of the form:

           exists sumvars. condition && timevariable<actiontime

         where the sumvars are added to the existentially quantified
         variables, and the resulting expression is

           condition && timevariable<actiontime

         The comments below refer to old code, where an explicit
         existential quantor was generated.

         OLD:
         Currently, the existential quantifier must use an equation,
         which represents a higher order function. The existential
         quantifier is namely of type exists:sorts1->Bool, where sorts1
         are the sorts of the quantified variables.

         If the sum variables do not occur in the expression, they
         are not quantified.

         If the actiontime is of the form t1+t2+...+tn where one
         of the ti is a quantified real variable in sumvars, and this
         variable does not occur in the condition, then the expression
         of the form timevariable < actiontime is omitted.
      */

      assert(used_sumvars.empty());
      data_expression result;
      variable_list variables;
      if (!has_time || (check_real_variable_occurrence(sumvars,actiontime,condition)))
      {
        result=condition;
      }
      else
      {
        result=RewriteTerm(
                 lazy::and_(
                   condition,
                   less(timevariable,actiontime)));
        variables.push_front(timevariable);
      }
      for (variable_list::const_iterator i=freevars.begin(); i!=freevars.end(); ++i)
      {
        if (occursinterm(*i,result))
        {
          variables.push_front(*i);
        }
      }

      for (std::set<variable>::const_iterator p=global_variables.begin();
           p!=global_variables.end() ; ++p)
      {
        if (occursinterm(*p,result))
        {
          variables.push_front(*p);
        }
      }

      for (variable_list::const_iterator s=sumvars.begin(); s!=sumvars.end(); ++s)
      {
        if (occursinterm(*s,result))
        {
          used_sumvars.push_front(*s);
        }
      }
      used_sumvars = reverse(used_sumvars);

      return result;
    }

    data_expression getUltimateDelayCondition(
      const stochastic_action_summand_vector& action_summands,
      const deadlock_summand_vector& deadlock_summands,
      const variable_list& freevars,
      const data_expression& timevariable,
      variable_list& existentially_quantified_variables)
    {
      assert(existentially_quantified_variables.empty());

      /* First walk through the summands to see whether
         a summand with condition true that does not refer
         to time exists. In that case the ultimate delay
         condition is true */

      for (deadlock_summand_vector::const_iterator i=deadlock_summands.begin();
           i!=deadlock_summands.end(); ++i)
      {
        if ((!i->deadlock().has_time()) && (i->condition()==sort_bool::true_()))
        {
          return sort_bool::true_();
        }
      }

      for (stochastic_action_summand_vector::const_iterator i=action_summands.begin();
           i!=action_summands.end(); ++i)
      {
        if ((!i->multi_action().has_time()) && (i->condition()==sort_bool::true_()))
        {
          return sort_bool::true_();
        }
      }

      /* Unfortunately, no ultimate delay condition true can
         be generated. So, we must now traverse all conditions
         to generate a non trivial ultimate delay condition */

      data_expression_list results;
      data_expression_list condition_list;
      std::vector < variable_list> renamings_pars;
      std::vector < data_expression_list> renamings_args;
      const variable& t = atermpp::down_cast<variable>(timevariable); // why has timevariable the wrong type?
      for (deadlock_summand_vector::const_iterator i=deadlock_summands.begin();
           i!=deadlock_summands.end(); ++i)
      {
        variable_list new_existentially_quantified_variables;
        const data_expression ult_del_condition=
             makesingleultimatedelaycondition(
                             i->summation_variables(),
                             freevars,
                             i->condition(),
                             i->deadlock().has_time(),
                             t,
                             i->deadlock().time(),
                             new_existentially_quantified_variables);
        existentially_quantified_variables=merge_var(
                                             new_existentially_quantified_variables,
                                             existentially_quantified_variables,
                                             renamings_pars,
                                             renamings_args,
                                             condition_list,
                                             variable_list());
        results.push_front(ult_del_condition);
      }

      for (stochastic_action_summand_vector::const_iterator i=action_summands.begin();
           i!=action_summands.end(); ++i)
      {
        variable_list new_existentially_quantified_variables;
        const data_expression ult_del_condition=
             makesingleultimatedelaycondition(
                             i->summation_variables(),
                             freevars,
                             i->condition(),
                             i->multi_action().has_time(),
                             t,
                             i->multi_action().time(),
                             new_existentially_quantified_variables);
        existentially_quantified_variables=merge_var(
                                             new_existentially_quantified_variables,
                                             existentially_quantified_variables,
                                             renamings_pars,
                                             renamings_args,
                                             condition_list,
                                             variable_list());
        results.push_front(ult_del_condition);
      }

      data_expression result=sort_bool::false_();

      assert(results.size()==condition_list.size());
      assert(results.size()==renamings_pars.size());
      assert(results.size()==renamings_args.size());

      std::vector < variable_list>::const_iterator renamings_par=renamings_pars.begin();
      std::vector < data_expression_list>::const_iterator renamings_arg=renamings_args.begin();
      condition_list=reverse(condition_list);
      results=reverse(results);
      data_expression_list::const_iterator j=condition_list.begin();
      for(data_expression_list::const_iterator i=results.begin();
              i!=results.end(); ++i,++j,++renamings_par,++renamings_arg)
      {
        const variable_list& auxpars=*renamings_par;
        const data_expression_list& auxargs=*renamings_arg;

        std::map < variable, data_expression > sigma;
        std::set<variable> variables_in_rhs_sigma;
        data_expression_list::const_iterator j1=auxargs.begin();
        for (variable_list::const_iterator i1=auxpars.begin();
             i1!=auxpars.end(); ++i1, ++j1)
        {
          /* Substitutions are carried out from left to right. The first applicable substitution counts */
          if (sigma.count(*i1)==0)
          {
            sigma[*i1]=*j1;
            std::set<variable> varset=find_free_variables(*j);
            variables_in_rhs_sigma.insert(varset.begin(),varset.end());

          }
        }

        mutable_map_substitution<> mutable_sigma(sigma);
        result=lazy::or_(result,data::replace_variables_capture_avoiding(lazy::and_(*i,*j), mutable_sigma, variables_in_rhs_sigma));
      }
      return result;
    }


    /******** make_unique_variables **********************/

    data::mutable_map_substitution<> make_unique_variables(
      const variable_list& var_list,
      const std::string& hint,
      std::set<data::variable>& rhs_variables)
    {
      /* This function generates a list of variables with the same sorts
         as in variable_list, where all names are unique */

      data::mutable_map_substitution<> sigma;

      for(variable_list::const_iterator i=var_list.begin(); i!=var_list.end(); ++i)
      {
        const data::variable v = get_fresh_variable(std::string(i->name()) + ((hint.empty())?"":"_") + hint, i->sort());
        sigma[*i] = v;
        rhs_variables.insert(v);
      }
      return sigma;
    }

    /******** make_parameters_and_variables_unique **********************/

    void make_parameters_and_sum_variables_unique(
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      variable_list& pars,
      assignment_list& init,
      const std::string hint="")
    {
      stochastic_action_summand_vector result_action_summands;

      std::set<data::variable> rhs_variables_sigma;
      data::mutable_map_substitution<> sigma=make_unique_variables(pars,hint, rhs_variables_sigma);
      const variable_list unique_pars=data::replace_variables(pars, sigma);

      init=substitute_assignmentlist(init,pars,true,false, sigma,rhs_variables_sigma);  // Only substitute the variables in the lhs.
      for (stochastic_action_summand_vector::const_iterator s=action_summands.begin(); s!=action_summands.end(); ++s)
      {
        const stochastic_action_summand smmnd= *s;

        std::set<data::variable> rhs_variables_sumvars;
        const variable_list sumvars=smmnd.summation_variables();
        data::mutable_map_substitution<> sigma_sumvars=make_unique_variables(sumvars,hint,rhs_variables_sumvars);
        const variable_list unique_sumvars=data::replace_variables(sumvars, sigma_sumvars);

        stochastic_distribution distribution=smmnd.distribution();
        std::set<data::variable> rhs_variables_stochastic_vars;
        const variable_list stochastic_vars=distribution.variables();
        data::mutable_map_substitution<> sigma_stochastic_vars=make_unique_variables(stochastic_vars,hint,rhs_variables_stochastic_vars);
        const variable_list unique_stochastic_vars=data::replace_variables(stochastic_vars, sigma_stochastic_vars);

        data_expression condition=smmnd.condition();
        action_list multiaction=smmnd.multi_action().actions();
        data_expression actiontime=smmnd.multi_action().time();
        assignment_list nextstate=smmnd.assignments();

        condition=data::replace_variables_capture_avoiding(condition, sigma_sumvars, rhs_variables_sumvars);
        condition=data::replace_variables_capture_avoiding(condition, sigma, rhs_variables_sigma);

        actiontime=data::replace_variables_capture_avoiding(actiontime, sigma_sumvars, rhs_variables_sumvars);
        actiontime=data::replace_variables_capture_avoiding(actiontime, sigma, rhs_variables_sigma);
        multiaction=lps::replace_variables_capture_avoiding(multiaction, sigma_sumvars, rhs_variables_sumvars);
        multiaction=lps::replace_variables_capture_avoiding(multiaction, sigma, rhs_variables_sigma);

        distribution=stochastic_distribution(
                       unique_stochastic_vars,
                       data::replace_variables_capture_avoiding(distribution.distribution(), sigma_sumvars, rhs_variables_sumvars));
        distribution=stochastic_distribution(
                       unique_stochastic_vars,
                       data::replace_variables_capture_avoiding(distribution.distribution(), sigma_stochastic_vars, rhs_variables_stochastic_vars));
        distribution=stochastic_distribution(
                       distribution.variables(),
                       data::replace_variables_capture_avoiding(distribution.distribution(), sigma, rhs_variables_sigma));

        nextstate=substitute_assignmentlist(nextstate,pars,false,true,sigma_sumvars,rhs_variables_sumvars);
        nextstate=substitute_assignmentlist(nextstate,pars,false,true,sigma_stochastic_vars,rhs_variables_stochastic_vars);
        nextstate=substitute_assignmentlist(nextstate,pars,true,true,sigma,rhs_variables_sigma);

        result_action_summands.push_back(stochastic_action_summand(
                                                        unique_sumvars,
                                                        condition,
                                                        s->multi_action().has_time()?multi_action(multiaction,actiontime):multi_action(multiaction),
                                                        nextstate,
                                                        distribution));
      }
      pars=unique_pars;
      action_summands.swap(result_action_summands);

      deadlock_summand_vector result_deadlock_summands;

      assert(unique_pars.size()==pars.size());

      for (deadlock_summand_vector::const_iterator s=deadlock_summands.begin(); s!=deadlock_summands.end(); ++s)
      {
        std::set<data::variable> rhs_variables_sumvars;
        const deadlock_summand smmnd= *s;
        const variable_list sumvars=smmnd.summation_variables();
        data::mutable_map_substitution<> sigma_sumvars=make_unique_variables(sumvars,hint, rhs_variables_sumvars);
        const variable_list unique_sumvars=data::replace_variables(sumvars, sigma_sumvars);

        assert(unique_sumvars.size()==sumvars.size());
        data_expression condition=smmnd.condition();
        data_expression actiontime=smmnd.deadlock().time();

        condition=data::replace_variables_capture_avoiding(condition, sigma_sumvars, rhs_variables_sumvars);
        condition=data::replace_variables_capture_avoiding(condition, sigma, rhs_variables_sigma);

        actiontime=data::replace_variables_capture_avoiding(actiontime, sigma_sumvars, rhs_variables_sumvars);
        actiontime=data::replace_variables_capture_avoiding(actiontime, sigma, rhs_variables_sigma);

        result_deadlock_summands.push_back(deadlock_summand(unique_sumvars,
                                                            condition,
                                                            s->deadlock().has_time()?deadlock(actiontime):deadlock()));
      }
      pars=unique_pars;
      result_deadlock_summands.swap(deadlock_summands);
    }





    /**************** parallel composition ******************************/

    void combine_summand_lists(
      const stochastic_action_summand_vector& action_summands1,
      const deadlock_summand_vector& deadlock_summands1,
      const stochastic_action_summand_vector& action_summands2,
      const deadlock_summand_vector& deadlock_summands2,
      const variable_list& par1,
      const variable_list& par3,
      const variable_list& parametersOfsumlist2,
      const action_name_multiset_list& allowlist1,  // This is a list of list of identifierstring.
      const bool is_allow,                          // If is_allow or is_block is set, perform inline allow/block filtering.
      const bool is_block,
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands)

    {
      assert(action_summands.size()==0);
      assert(deadlock_summands.size()==0);

      variable_list allpars;

      allpars=par1 + par3;

      bool inline_allow = is_allow || is_block;
      if (inline_allow)
      {
        // Inline allow is only supported for add_delta,
        // for in other cases generation of delta summands cannot be inlined in any simple way.
        assert(!options.nodeltaelimination && options.add_delta);
        deadlock_summands.push_back(deadlock_summand(variable_list(),sort_bool::true_(),deadlock()));
      }
      action_name_multiset_list allowlist((is_allow)?sortMultiActionLabels(allowlist1):allowlist1);

      /* first we enumerate the summands of t1 */

      variable timevar=get_fresh_variable("timevar",sort_real::real_());
      variable_list ultimate_delay_sumvars1;
      data_expression ultimatedelaycondition=
        (options.add_delta?data_expression(sort_bool::true_()):
           getUltimateDelayCondition(action_summands2,deadlock_summands2,parametersOfsumlist2,timevar,ultimate_delay_sumvars1));

      if (!inline_allow)
      {
        for (deadlock_summand_vector::const_iterator walker1=deadlock_summands1.begin();
             walker1!=deadlock_summands1.end(); ++walker1)
        {
          const deadlock_summand summand1= *walker1;
          variable_list sumvars1=summand1.summation_variables() + ultimate_delay_sumvars1;
          // action_list multiaction1=summand1.actions();
          data_expression actiontime1=summand1.deadlock().time();
          data_expression condition1=summand1.condition();
          // assignment_list nextstate1=summand1.assignments();
          bool has_time=summand1.deadlock().has_time();

          if (!has_time)
          {
            if (ultimatedelaycondition!=sort_bool::true_())
            {
              actiontime1=timevar;
              sumvars1.push_front(timevar);
              condition1=lazy::and_(ultimatedelaycondition,condition1);
              has_time=true;
            }
          }
          else
          {
            /* Summand1 has time. Substitute the time expression for
               timevar in ultimatedelaycondition, and extend the condition */
            mutable_map_substitution<> sigma;
            const std::set<variable> variables_in_rhs_sigma=find_free_variables(actiontime1);
            sigma[timevar]=actiontime1;
            const data_expression intermediateultimatedelaycondition=
                        data::replace_variables_capture_avoiding(ultimatedelaycondition, sigma, variables_in_rhs_sigma);
            condition1=lazy::and_(intermediateultimatedelaycondition,condition1);
          }

          condition1=RewriteTerm(condition1);
          if (condition1!=sort_bool::false_())
          {
            deadlock_summands.push_back(deadlock_summand(sumvars1,condition1, has_time?deadlock(actiontime1):deadlock()));
          }

        }
      }

      for (stochastic_action_summand_vector::const_iterator walker1=action_summands1.begin();
           walker1!=action_summands1.end(); ++walker1)
      {
        const stochastic_action_summand summand1= *walker1;
        variable_list sumvars1=summand1.summation_variables() + ultimate_delay_sumvars1;
        action_list multiaction1=summand1.multi_action().actions();
        data_expression actiontime1=summand1.multi_action().time();
        data_expression condition1=summand1.condition();
        assignment_list nextstate1=summand1.assignments();
        const stochastic_distribution distribution1=summand1.distribution();
        bool has_time=summand1.has_time();

        if (multiaction1 != action_list({ terminationAction }))
        {
          if (is_allow && !allow_(allowlist,multiaction1))
          {
            continue;
          }
          if (is_block && encap(deprecated_cast<identifier_string_list>(allowlist),multiaction1))
          {
            continue;
          }

          if (!has_time)
          {
            if (ultimatedelaycondition!=sort_bool::true_())
            {
              actiontime1=timevar;
              sumvars1.push_front(timevar);
              condition1=lazy::and_(ultimatedelaycondition,condition1);
              has_time=true;
            }
          }
          else
          {
            /* Summand1 has time. Substitute the time expression for
               timevar in ultimatedelaycondition, and extend the condition */
            mutable_map_substitution<> sigma;
            const std::set<variable> variables_in_rhs_sigma=find_free_variables(actiontime1);
            sigma[timevar]=actiontime1;
            const data_expression intermediateultimatedelaycondition=
                       data::replace_variables_capture_avoiding(ultimatedelaycondition,sigma,variables_in_rhs_sigma);
            condition1=lazy::and_(intermediateultimatedelaycondition,condition1);
          }

          condition1=RewriteTerm(condition1);
          if (condition1!=sort_bool::false_())
          {
            action_summands.push_back(stochastic_action_summand(
                                             sumvars1,
                                             condition1,
                                             has_time?multi_action(multiaction1, actiontime1):multi_action(multiaction1),
                                             nextstate1,
                                             distribution1));
          }
        }
      }
      /* second we enumerate the summands of sumlist2 */

      variable_list ultimate_delay_sumvars2;
      ultimatedelaycondition=(options.add_delta?data_expression(sort_bool::true_()):
                  getUltimateDelayCondition(action_summands1,deadlock_summands1,par1, timevar,ultimate_delay_sumvars2));

      if (!inline_allow)
      {
        for (deadlock_summand_vector::const_iterator walker2=deadlock_summands2.begin();
             walker2!=deadlock_summands2.end(); ++walker2)
        {
          const deadlock_summand summand2= *walker2;
          variable_list sumvars2=summand2.summation_variables() + ultimate_delay_sumvars2;
          data_expression actiontime2=summand2.deadlock().time();
          data_expression condition2=summand2.condition();
          bool has_time=summand2.deadlock().has_time();

          if (!has_time)
          {
            if (ultimatedelaycondition!=sort_bool::true_())
            {
              actiontime2=data_expression(timevar);
              sumvars2.push_front(timevar);
              condition2=lazy::and_(ultimatedelaycondition,condition2);
              has_time=true;
            }
          }
          else
          {
            /* Summand2 has time. Substitute the time expression for
               timevar in ultimatedelaycondition, and extend the condition */
            mutable_map_substitution<> sigma;
            const std::set<variable> variables_in_rhs_sigma=find_free_variables(actiontime2);
            sigma[timevar]=actiontime2;

            const data_expression intermediateultimatedelaycondition=
                          data::replace_variables_capture_avoiding(ultimatedelaycondition,sigma,variables_in_rhs_sigma);
            condition2=lazy::and_(intermediateultimatedelaycondition,condition2);
          }

          condition2=RewriteTerm(condition2);
          if (condition2!=sort_bool::false_())
          {
            deadlock_summands.push_back(deadlock_summand(sumvars2,
                                                         condition2,
                                                         has_time?deadlock(actiontime2):deadlock()));
          }

        }
      }

      for (stochastic_action_summand_vector::const_iterator walker2=action_summands2.begin();
           walker2!=action_summands2.end(); ++walker2)
      {
        const stochastic_action_summand summand2= *walker2;
        variable_list sumvars2=summand2.summation_variables() + ultimate_delay_sumvars2;
        action_list multiaction2=summand2.multi_action().actions();
        data_expression actiontime2=summand2.multi_action().time();
        data_expression condition2=summand2.condition();
        assignment_list nextstate2=summand2.assignments();
        const stochastic_distribution distribution2=summand2.distribution();
        bool has_time=summand2.multi_action().has_time();

        if (multiaction2 != action_list({ terminationAction }))
        {
          if (is_allow && !allow_(allowlist,multiaction2))
          {
            continue;
          }
          if (is_block && encap(deprecated_cast<identifier_string_list>(allowlist),multiaction2))
          {
            continue;
          }

          if (!has_time)
          {
            if (ultimatedelaycondition!=sort_bool::true_())
            {
              actiontime2=data_expression(timevar);
              sumvars2.push_front(timevar);
              condition2=lazy::and_(ultimatedelaycondition,condition2);
              has_time=true;
            }
          }
          else
          {
            /* Summand2 has time. Substitute the time expression for
               timevar in ultimatedelaycondition, and extend the condition */
            mutable_map_substitution<> sigma;
            const std::set<variable> variables_in_rhs_sigma=find_free_variables(actiontime2);
            sigma[timevar]=actiontime2;

            const data_expression intermediateultimatedelaycondition=
                                data::replace_variables_capture_avoiding(ultimatedelaycondition,sigma,variables_in_rhs_sigma);
            condition2=lazy::and_(intermediateultimatedelaycondition,condition2);
          }

          condition2=RewriteTerm(condition2);
          if (condition2!=sort_bool::false_())
          {
            action_summands.push_back(stochastic_action_summand(
                                                     sumvars2,
                                                     condition2,
                                                     has_time?multi_action(multiaction2,actiontime2):multi_action(multiaction2),
                                                     nextstate2,
                                                     distribution2));
          }
        }
      }

      /* thirdly we enumerate all multi actions*/

      for (stochastic_action_summand_vector::const_iterator walker1=action_summands1.begin();
           walker1!=action_summands1.end(); ++walker1)
      {
        const stochastic_action_summand summand1= *walker1;

        const variable_list sumvars1=summand1.summation_variables();
        const action_list multiaction1=summand1.multi_action().actions();
        const data_expression actiontime1=summand1.multi_action().time();
        const data_expression condition1=summand1.condition();
        const assignment_list nextstate1=summand1.assignments();
        const stochastic_distribution distribution1=summand1.distribution();

        for (stochastic_action_summand_vector::const_iterator walker2=action_summands2.begin();
             walker2!=action_summands2.end(); ++walker2)
        {
          const stochastic_action_summand summand2= *walker2;
          const variable_list sumvars2=summand2.summation_variables();
          const action_list multiaction2=summand2.multi_action().actions();
          const data_expression actiontime2=summand2.multi_action().time();
          const data_expression condition2=summand2.condition();
          const assignment_list nextstate2=summand2.assignments();
          const stochastic_distribution distribution2=summand2.distribution();

          if ((multiaction1 == action_list({ terminationAction })) == (multiaction2 == action_list({ terminationAction })))
          {
            action_list multiaction3;
            if ((multiaction1 == action_list({ terminationAction })) && (multiaction2 == action_list({ terminationAction })))
            {
              multiaction3.push_front(terminationAction);
            }
            else
            {
              multiaction3=linMergeMultiActionList(multiaction1,multiaction2);

            }

            if (is_allow && !allow_(allowlist,multiaction3))
            {
              continue;
            }
            if (is_block && encap(deprecated_cast<identifier_string_list>(allowlist),multiaction3))
            {
              continue;
            }

            const variable_list allsums=sumvars1+sumvars2;
            data_expression condition3= lazy::and_(condition1,condition2);
            data_expression action_time3;
            bool has_time3=false;

            if (!summand1.has_time())
            {
              if (!summand2.has_time())
              {
                has_time3=false;
              }
              else
              {
                /* summand 2 has time*/
                has_time3=summand2.has_time();
                action_time3=actiontime2;
              }
            }
            else
            {
              /* summand 1 has time */
              if (!summand2.has_time())
              {
                has_time3=summand1.has_time();
                action_time3=actiontime1;
              }
              else
              {
                /* both summand 1 and 2 have time */
                has_time3=true;
                action_time3=actiontime1;
                condition3=lazy::and_(
                             condition3,
                             equal_to(actiontime1,actiontime2));
              }
            }

            const assignment_list nextstate3=nextstate1+nextstate2;
            const stochastic_distribution distribution3(
                                              distribution1.variables()+distribution2.variables(),
                                              real_times_optimized(distribution1.distribution(),distribution2.distribution()));

            condition3=RewriteTerm(condition3);
            if (condition3!=sort_bool::false_())
            {
              action_summands.push_back(stochastic_action_summand(
                                           allsums,
                                           condition3,
                                           has_time3?multi_action(multiaction3,action_time3):multi_action(multiaction3),
                                           nextstate3,
                                           distribution3));
            }
          }
        }
      }
    }


    void parallelcomposition(
      const stochastic_action_summand_vector& action_summands1,
      const deadlock_summand_vector& deadlock_summands1,
      const variable_list& pars1,
      const assignment_list& init1,
      const stochastic_action_summand_vector& action_summands2,
      const deadlock_summand_vector& deadlock_summands2,
      const variable_list& pars2,
      const assignment_list& init2,
      const action_name_multiset_list& allowlist1,  // This is a list of list of identifierstring.
      const bool is_allow,                          // If is_allow or is_block is set, perform inline allow/block filtering.
      const bool is_block,
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      variable_list& pars_result,
      assignment_list& init_result)
    {
      mCRL2log(mcrl2::log::verbose) <<
            (is_allow ? "- calculating parallel composition modulo the allow operator: " :
             is_block ? "- calculating parallel composition modulo the block operator: " :
                        "- calculating parallel composition: ") <<
            action_summands1.size() <<
            " action summands + " << deadlock_summands1.size() <<
            " deadlock summands || " << action_summands2.size() <<
            " action summands + " << deadlock_summands2.size() << " deadlock summands = ";

      // At this point the parameters of pars1 and pars2 are unique, except for
      // those that are constant in both processes.

      variable_list pars3;
      for (variable_list::const_iterator i=pars2.begin(); i!=pars2.end(); ++i)
      {
        if (std::find(pars1.begin(),pars1.end(),*i)==pars1.end())
        {
          // *i does not occur in pars1.
          pars3.push_front(*i);
        }
      }

      pars3=reverse(pars3);
      assert(action_summands.size()==0);
      assert(deadlock_summands.size()==0);
      combine_summand_lists(action_summands1,deadlock_summands1,action_summands2,deadlock_summands2,pars1,pars3,pars2,allowlist1,is_allow,is_block,action_summands,deadlock_summands);

      mCRL2log(mcrl2::log::verbose) << action_summands.size() << " actions and " << deadlock_summands.size() << " delta summands.\n";
      pars_result=pars1+pars3;
      init_result=init1 + init2;
    }

    /**************** GENERaTE LPEmCRL **********************************/


    /// \brief Linearise a process indicated by procIdDecl.
    /// \details Returns actions_summands, deadlock_summands, the process parameters
    ///              and the initial assignment list.

    void generateLPEmCRLterm(
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      const process_expression t,    // This process expression cannot be a reference.
      const bool regular,
      const bool rename_variables,
      variable_list& pars,
      assignment_list& init,
      stochastic_distribution& initial_stochastic_distribution)
    {
      if (is_process_instance_assignment(t))
      {
        generateLPEmCRL(action_summands,deadlock_summands,process_instance_assignment(t).identifier(),regular,pars,init,initial_stochastic_distribution);
        size_t n=objectIndex(process_instance_assignment(t).identifier());
        const assignment_list ass=process_instance_assignment(t).assignments();

        mutable_map_substitution<> sigma;
        // std::map<variable,data_expression> sigma;
        std::set<variable> variables_occurring_in_rhs_sigma;
        for (assignment_list::const_iterator i=ass.begin(); i!=ass.end(); ++i)
        {
          sigma[i->lhs()]=i->rhs();
          const std::set<variable> varset=find_free_variables(i->rhs());
          variables_occurring_in_rhs_sigma.insert(varset.begin(),varset.end());
        }

        init=substitute_assignmentlist(init,pars,false,true,sigma,variables_occurring_in_rhs_sigma);

        // Make the bound variables and parameters in this process unique.

        if ((objectdata[n].processstatus==GNF)||
            (objectdata[n].processstatus==pCRL)||
            (objectdata[n].processstatus==GNFalpha))
        {
          make_parameters_and_sum_variables_unique(action_summands,deadlock_summands,pars,init,std::string(objectdata[n].objectname));
        }
        else
        {
          if (rename_variables)
          {
            make_parameters_and_sum_variables_unique(action_summands,deadlock_summands,pars,init);
          }
        }

        if (regular && !options.do_not_apply_constelm)
        {
          // We apply constant elimination on the obtained linear process.
          // In order to do so, we have to create a complete process specification first, as
          // this is what the interface of constelm requires.
          // Note that this is only useful, in regular mode. This does not make sense if
          // stacks are being used.

          stochastic_linear_process lps(pars,deadlock_summands,action_summands);
          stochastic_process_initializer initializer(init,stochastic_distribution(variable_list(),real_one())); // Default distribution.

          stochastic_specification temporary_spec(data,acts,global_variables,lps,initializer);
          constelm_algorithm < rewriter, stochastic_specification > alg(temporary_spec,rewr);
          alg.run(true); // Remove constants from the specification, where global variables are
          // also instantiated if they exist.
          // Reconstruct the variables from the temporary specification

          init=temporary_spec.initial_process().assignments();
          pars=temporary_spec.process().process_parameters();

          // Add all free variables in objectdata[n].parameters that are not already in the parameter list
          // and are not global variables to pars. This can occur when a parameter of the process is replaced
          // by a constant, which by itself is a parameter.

          std::set <variable> variable_list = lps::find_free_variables(temporary_spec.process().action_summands());
          const std::set <variable> variable_list1 = lps::find_free_variables(temporary_spec.process().deadlock_summands());
          variable_list.insert(variable_list1.begin(),variable_list1.end());
          for (std::set <variable>::const_iterator i=variable_list.begin();
               i!=variable_list.end(); ++i)
          {
            if (std::find(pars.begin(),pars.end(),*i)==pars.end() && // The free variable is not in pars,
                global_variables.find(*i)==global_variables.end() // it is neither a global variable
                // (lps::search_free_variable(temporary_spec.process().action_summands(),*i) || lps::search_free_variable(temporary_spec.process().deadlock_summands(),*i))
               )          // and it occurs in the summands.
            {
              pars.push_front(*i);
            }
          }

          action_summands=temporary_spec.process().action_summands();
          deadlock_summands=temporary_spec.process().deadlock_summands();
        }
        // Now constelm has been applied.
        return;
      }

      if (is_merge(t))
      {
        variable_list pars1,pars2;
        assignment_list init1,init2;
        stochastic_action_summand_vector action_summands1, action_summands2;
        deadlock_summand_vector deadlock_summands1, deadlock_summands2;
        generateLPEmCRLterm(action_summands1,deadlock_summands1,process::merge(t).left(),
                              regular,rename_variables,pars1,init1,initial_stochastic_distribution);
        generateLPEmCRLterm(action_summands2,deadlock_summands2,process::merge(t).right(),
                              regular,true,pars2,init2,initial_stochastic_distribution);
        parallelcomposition(action_summands1,deadlock_summands1,pars1,init1,
                              action_summands2,deadlock_summands2,pars2,init2,
                              action_name_multiset_list(),false,false,
                              action_summands,deadlock_summands,pars,init);
        return;
      }

      if (is_hide(t))
      {
        generateLPEmCRLterm(action_summands,deadlock_summands,hide(t).operand(),
                              regular,rename_variables,pars,init,initial_stochastic_distribution);
        hidecomposition(hide(t).hide_set(),action_summands);
        return;
      }

      if (is_allow(t))
      {
        process_expression par = allow(t).operand();
        if (!options.nodeltaelimination && options.add_delta && is_merge(par))
        {
          // Perform parallel composition with inline allow.
          variable_list pars1,pars2;
          assignment_list init1,init2;
          stochastic_action_summand_vector action_summands1, action_summands2;
          deadlock_summand_vector deadlock_summands1, deadlock_summands2;
          generateLPEmCRLterm(action_summands1,deadlock_summands1,process::merge(par).left(),
                                regular,rename_variables,pars1,init1,initial_stochastic_distribution);
          generateLPEmCRLterm(action_summands2,deadlock_summands2,process::merge(par).right(),
                                regular,true,pars2,init2,initial_stochastic_distribution);
          parallelcomposition(action_summands1,deadlock_summands1,pars1,init1,
                                action_summands2,deadlock_summands2,pars2,init2,
                                allow(t).allow_set(),true,false,
                                action_summands,deadlock_summands,pars,init);
          return;
        }
        else if (!options.nodeltaelimination && options.add_delta && is_comm(par))
        {
          generateLPEmCRLterm(action_summands,deadlock_summands,comm(par).operand(),
                                regular,rename_variables,pars,init,initial_stochastic_distribution);
          communicationcomposition(comm(par).comm_set(),allow(t).allow_set(),true,false,action_summands,deadlock_summands);
          return;
        }

        generateLPEmCRLterm(action_summands,deadlock_summands,par,regular,rename_variables,pars,init,initial_stochastic_distribution);
        allowblockcomposition(allow(t).allow_set(),true,action_summands,deadlock_summands);
        return;
      }

      if (is_block(t))
      {
        process_expression par = block(t).operand();
        if (!options.nodeltaelimination && options.add_delta && is_merge(par))
        {
          // Perform parallel composition with inline block.
          variable_list pars1,pars2;
          assignment_list init1,init2;
          stochastic_action_summand_vector action_summands1, action_summands2;
          deadlock_summand_vector deadlock_summands1, deadlock_summands2;
          generateLPEmCRLterm(action_summands1,deadlock_summands1,process::merge(par).left(),
                                regular,rename_variables,pars1,init1,initial_stochastic_distribution);
          generateLPEmCRLterm(action_summands2,deadlock_summands2,process::merge(par).right(),
                                regular,true,pars2,init2,initial_stochastic_distribution);
          parallelcomposition(action_summands1,deadlock_summands1,pars1,init1,
                                action_summands2,deadlock_summands2,pars2,init2,
                                action_name_multiset_list(block(t).block_set()),false,true,
                                action_summands,deadlock_summands,pars,init);
          return;
        }
        else if (!options.nodeltaelimination && options.add_delta && is_comm(par))
        {
          generateLPEmCRLterm(action_summands,deadlock_summands,comm(par).operand(),
                                regular,rename_variables,pars,init,initial_stochastic_distribution);
          communicationcomposition(comm(par).comm_set(),action_name_multiset_list(block(t).block_set()),false,true,action_summands,deadlock_summands);
          return;
        }

        generateLPEmCRLterm(action_summands,deadlock_summands,par,regular,rename_variables,pars,init,initial_stochastic_distribution);
        allowblockcomposition(action_name_multiset_list(block(t).block_set()),false,action_summands,deadlock_summands);
        return;
      }

      if (is_rename(t))
      {
        generateLPEmCRLterm(action_summands,deadlock_summands,process::rename(t).operand(),
                              regular,rename_variables,pars,init,initial_stochastic_distribution);
        renamecomposition(process::rename(t).rename_set(),action_summands);
        return;
      }

      if (is_comm(t))
      {
        generateLPEmCRLterm(action_summands,deadlock_summands,comm(t).operand(),
                              regular,rename_variables,pars,init,initial_stochastic_distribution);
        communicationcomposition(comm(t).comm_set(),action_name_multiset_list(),false,false,action_summands,deadlock_summands);
        return;
      }

      throw mcrl2::runtime_error("Internal error. Expect mCRL term " + process::pp(t) +".");
    }

    /**************** GENERaTE LPEmCRL **********************************/

    void generateLPEmCRL(
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      const process_identifier& procIdDecl,
      const bool regular,
      variable_list& pars,
      assignment_list& init,
      stochastic_distribution& initial_stochastic_distribution)
    {
      /* If regular=1, then a regular version of the pCRL processes
         must be generated */

      size_t n=objectIndex(procIdDecl);

      if ((objectdata[n].processstatus==GNF)||
          (objectdata[n].processstatus==pCRL)||
          (objectdata[n].processstatus==GNFalpha)||
          (objectdata[n].processstatus==multiAction))
      {
        generateLPEpCRL(action_summands,deadlock_summands,procIdDecl,
                               objectdata[n].containstime,regular,pars,init,initial_stochastic_distribution);
        return;
      }
      /* process is a mCRLdone */
      if ((objectdata[n].processstatus==mCRLdone)||
          (objectdata[n].processstatus==mCRLlin)||
          (objectdata[n].processstatus==mCRL))
      {
        objectdata[n].processstatus=mCRLlin;
        return generateLPEmCRLterm(action_summands,deadlock_summands,objectdata[n].processbody,
                                   regular,false,pars,init,initial_stochastic_distribution);
      }

      throw mcrl2::runtime_error("laststatus: " + str(boost::format("%d") % objectdata[n].processstatus));
    }

    /**************** alphaconversion ********************************/

    process_expression alphaconversionterm(
      const process_expression& t,
      const variable_list& parameters,
      mutable_map_substitution<> sigma,
      const std::set < variable >& variables_occurring_in_rhs_of_sigma)
    {
      if (is_choice(t))
      {
        return choice(
                 alphaconversionterm(choice(t).left(),parameters,sigma,variables_occurring_in_rhs_of_sigma),
                 alphaconversionterm(choice(t).right(),parameters,sigma,variables_occurring_in_rhs_of_sigma));
      }

      if (is_seq(t))
      {
        return seq(
                 alphaconversionterm(seq(t).left(),parameters,sigma,variables_occurring_in_rhs_of_sigma),
                 alphaconversionterm(seq(t).right(),parameters,sigma,variables_occurring_in_rhs_of_sigma));
      }

      if (is_sync(t))
      {
        return process::sync(
                 alphaconversionterm(process::sync(t).left(),parameters,sigma,variables_occurring_in_rhs_of_sigma),
                 alphaconversionterm(process::sync(t).right(),parameters,sigma,variables_occurring_in_rhs_of_sigma));
      }

      if (is_bounded_init(t))
      {
        return bounded_init(
                 alphaconversionterm(bounded_init(t).left(),parameters,sigma,variables_occurring_in_rhs_of_sigma),
                 alphaconversionterm(bounded_init(t).right(),parameters,sigma,variables_occurring_in_rhs_of_sigma));
      }

      if (is_merge(t))
      {
        return process::merge(
                 alphaconversionterm(process::merge(t).left(),parameters,sigma,variables_occurring_in_rhs_of_sigma),
                 alphaconversionterm(process::merge(t).right(),parameters,sigma,variables_occurring_in_rhs_of_sigma));
      }

      if (is_left_merge(t))
      {
        return left_merge(
                 alphaconversionterm(left_merge(t).left(),parameters,sigma,variables_occurring_in_rhs_of_sigma),
                 alphaconversionterm(left_merge(t).right(),parameters,sigma,variables_occurring_in_rhs_of_sigma));
      }

      if (is_at(t))
      {
        return at(alphaconversionterm(at(t).operand(),parameters,sigma,variables_occurring_in_rhs_of_sigma),
                  data::replace_variables_capture_avoiding(at(t).time_stamp(),sigma,variables_occurring_in_rhs_of_sigma));
      }

      if (is_if_then(t))
      {
        return if_then(
                 data::replace_variables_capture_avoiding(if_then(t).condition(), sigma, variables_occurring_in_rhs_of_sigma),
                 alphaconversionterm(if_then(t).then_case(),parameters,sigma,variables_occurring_in_rhs_of_sigma));
      }

      if (is_sum(t))
      {
        variable_list sumvars=sum(t).variables();
        mutable_map_substitution<> local_sigma=sigma;
        std::set<variable> variables_occurring_in_rhs_of_local_sigma=variables_occurring_in_rhs_of_sigma;

        alphaconvert(sumvars,local_sigma,variable_list(),data_expression_list(parameters),variables_occurring_in_rhs_of_local_sigma);
        return sum(sumvars,alphaconversionterm(sum(t).operand(), sumvars+parameters,
                                                   local_sigma,variables_occurring_in_rhs_of_local_sigma));
      }

      if (is_stochastic_operator(t))
      {
        const stochastic_operator& sto=down_cast<const stochastic_operator>(t);
        variable_list sumvars=sto.variables();
        mutable_map_substitution<> local_sigma=sigma;
        std::set<variable> variables_occurring_in_rhs_of_local_sigma=variables_occurring_in_rhs_of_sigma;

        alphaconvert(sumvars,local_sigma,variable_list(),data_expression_list(parameters),variables_occurring_in_rhs_of_local_sigma);
        return stochastic_operator(
                       sumvars,
                       data::replace_variables_capture_avoiding(sto.distribution(),
                                                                sigma, variables_occurring_in_rhs_of_sigma),
                       alphaconversionterm(sto.operand(), sumvars+parameters,
                                                   local_sigma,variables_occurring_in_rhs_of_local_sigma));
      }

      if (is_process_instance_assignment(t))
      {
        const process_identifier procId=process_instance_assignment(t).identifier();
        size_t n=objectIndex(procId);

        const variable_list instance_parameters=objectdata[n].parameters;
        alphaconversion(procId,instance_parameters);

        const process_instance_assignment result(procId,
                                                 substitute_assignmentlist(process_instance_assignment(t).assignments(),
                                                                           instance_parameters,false, true,sigma,variables_occurring_in_rhs_of_sigma));
        assert(check_valid_process_instance_assignment(result.identifier(),result.assignments()));
        return result;
      }

      if (is_action(t))
      {
        return action(action(t).label(), data::replace_variables_capture_avoiding(action(t).arguments(), sigma, variables_occurring_in_rhs_of_sigma));
      }

      if (is_delta(t)||
          is_tau(t))
      {
        return t;
      }

      if (is_hide(t))
      {
        return alphaconversionterm(hide(t).operand(),parameters,sigma,variables_occurring_in_rhs_of_sigma);
      }

      if (is_rename(t))
      {
        return alphaconversionterm(process::rename(t).operand(),parameters,sigma,variables_occurring_in_rhs_of_sigma);
      }

      if (is_comm(t))
      {
        return alphaconversionterm(comm(t).operand(),parameters,sigma,variables_occurring_in_rhs_of_sigma);
      }

      if (is_allow(t))
      {
        return alphaconversionterm(allow(t).operand(),parameters,sigma,variables_occurring_in_rhs_of_sigma);
      }

      if (is_block(t))
      {
        return alphaconversionterm(block(t).operand(),parameters,sigma,variables_occurring_in_rhs_of_sigma);
      }

      throw mcrl2::runtime_error("unexpected process format in alphaconversionterm " + process::pp(t) +".");
      return process_expression();
    }

    void alphaconversion(const process_identifier& procId, const variable_list& parameters)
    {
      size_t n=objectIndex(procId);

      if ((objectdata[n].processstatus==GNF)||
          (objectdata[n].processstatus==multiAction))
      {
        objectdata[n].processstatus=GNFalpha;
        // tempvar below is needed as objectdata may be reallocated
        // during a call to alphaconversionterm.
        std::map < variable, data_expression > sigma;
        const process_expression tempvar=alphaconversionterm(objectdata[n].processbody,parameters,sigma,std::set<variable>());
        objectdata[n].processbody=tempvar;
      }
      else if (objectdata[n].processstatus==mCRLdone)
      {
        std::map < variable, data_expression > sigma;
        alphaconversionterm(objectdata[n].processbody,parameters,sigma,std::set<variable>());

      }
      else if (objectdata[n].processstatus==GNFalpha)
      {
        return;
      }
      else
      {
        throw mcrl2::runtime_error("unknown type " + str(boost::format("%d") % objectdata[n].processstatus) +
                                   " in alphaconversion of " + process::pp(procId) +".");
      }
      return;
    }

    /***** determinewhetherprocessescontaintime; **********/

    bool containstimebody(
      const process_expression& t,
      bool* stable,
      std::set < process_identifier >& visited,
      bool allowrecursion,
      bool& contains_if_then)
    {
      if (is_merge(t))
      {
        /* the construction below is needed to guarantee that
           both subterms are recursively investigated */
        bool r1=containstimebody(process::merge(t).left(),stable,visited,allowrecursion,contains_if_then);
        bool r2=containstimebody(process::merge(t).right(),stable,visited,allowrecursion,contains_if_then);
        return r1||r2;
      }

      if (is_process_instance(t))
      {
        assert(0);
        if (allowrecursion)
        {
          return (containstime_rec(process_instance(t).identifier(),stable,visited,contains_if_then));
        }
        return objectdata[objectIndex(process_instance(t).identifier())].containstime;
      }

      if (is_process_instance_assignment(t))
      {
        if (allowrecursion)
        {
          return (containstime_rec(process_instance_assignment(t).identifier(),stable,visited,contains_if_then));
        }
        return objectdata[objectIndex(process_instance_assignment(t).identifier())].containstime;
      }

      if (is_hide(t))
      {
        return containstimebody(hide(t).operand(),stable,visited,allowrecursion,contains_if_then);
      }

      if (is_rename(t))
      {
        return containstimebody(process::rename(t).operand(),stable,visited,allowrecursion,contains_if_then);
      }

      if (is_allow(t))
      {
        return containstimebody(allow(t).operand(),stable,visited,allowrecursion,contains_if_then);
      }

      if (is_block(t))
      {
        return containstimebody(block(t).operand(),stable,visited,allowrecursion,contains_if_then);
      }

      if (is_comm(t))
      {
        return containstimebody(comm(t).operand(),stable,visited,allowrecursion,contains_if_then);
      }

      if (is_choice(t))
      {
        bool r1=containstimebody(choice(t).left(),stable,visited,allowrecursion,contains_if_then);
        bool r2=containstimebody(choice(t).right(),stable,visited,allowrecursion,contains_if_then);
        return r1||r2;
      }

      if (is_seq(t))
      {
        bool r1=containstimebody(seq(t).left(),stable,visited,allowrecursion,contains_if_then);
        bool r2=containstimebody(seq(t).right(),stable,visited,allowrecursion,contains_if_then);
        return r1||r2;
      }

      if (is_if_then(t))
      {
        // If delta is added, c->p is translated into c->p<>delta,
        // otherwise into c->p<>delta@0. In this last case the process
        // contains time.
        contains_if_then=true;
        if (options.add_delta)
        {
          return containstimebody(if_then(t).then_case(),stable,visited,allowrecursion,contains_if_then);
        }
        else
        {
          return true;
        }
      }

      if (is_if_then_else(t))
      {
        bool r1=containstimebody(if_then_else(t).then_case(),stable,visited,allowrecursion,contains_if_then);
        bool r2=containstimebody(if_then_else(t).else_case(),stable,visited,allowrecursion,contains_if_then);
        return r1||r2;
      }

      if (is_sum(t))
      {
        return containstimebody(sum(t).operand(),stable,visited,allowrecursion,contains_if_then);
      }

      if (is_stochastic_operator(t))
      {
        return containstimebody(stochastic_operator(t).operand(),stable,visited,allowrecursion,contains_if_then);
      }

      if (is_action(t)||
          is_delta(t)||
          is_tau(t))
      {
        return false;
      }

      if (is_at(t))
      {
        return true;
      }

      if (is_sync(t))
      {
        bool r1=containstimebody(process::sync(t).left(),stable,visited,allowrecursion,contains_if_then);
        bool r2=containstimebody(process::sync(t).right(),stable,visited,allowrecursion,contains_if_then);
        return r1||r2;
      }

      throw mcrl2::runtime_error("unexpected process format in containstime " + process::pp(t) +".");
      return false;
    }

    bool containstime_rec(
      const process_identifier& procId,
      bool* stable,
      std::set < process_identifier >& visited,
      bool& contains_if_then)
    {
      size_t n=objectIndex(procId);

      if (visited.count(procId)==0)
      {
        visited.insert(procId);
        const bool ct=containstimebody(objectdata[n].processbody,stable,visited,true,contains_if_then);
        static bool show_only_once=true;
        if (ct && options.add_delta && show_only_once)
        {
          mCRL2log(mcrl2::log::warning) << "process " << procId.name() <<
              " contains time, which is now not preserved. \n"  <<
              "Use --timed or -T, or untick `add deadlocks' for a correct timed linearisation...\n";
          show_only_once=false;
        }
        if (objectdata[n].containstime!=ct)
        {
          objectdata[n].containstime=ct;
          if (stable!=NULL)
          {
            *stable=false;
          }
        }
      }
      return (objectdata[n].containstime);
    }

    bool containstimebody(const process_expression& t)
    {
      std::set < process_identifier > visited;
      bool stable;
      bool contains_if_then;
      return containstimebody(t,&stable,visited,false,contains_if_then);
    }

    bool determinewhetherprocessescontaintime(const process_identifier& procId)

    {
      /* This function sets for all reachable processes in the array objectdata
         whether they contain time in the field containstime. In verbose mode
         it prints the process variables that contain time. Furtermore, it returns
         whether there are processes that contain an if-then that will be translated
         to an if-then-else with an delta@0 in the else branch, introducing time */
      bool stable=0;
      bool contains_if_then=false;

      while (!stable)
      {
        std::set < process_identifier > visited;
        stable=1;
        containstime_rec(procId,&stable,visited,contains_if_then);
      }
      return contains_if_then;
    }
    /***** obtain_initial_distribution **********/

    process_expression obtain_initial_distribution_term(
                            const process_expression& t,
                            std::set<process_identifier>& visited,
                            std::vector < process_identifier >& pCRLprocs)
    {
      if (is_merge(t))
      {
        const process_expression r1_=obtain_initial_distribution_term(process::merge(t).left(),visited,pCRLprocs);
        const process_expression r2_=obtain_initial_distribution_term(process::merge(t).right(),visited,pCRLprocs);

        if (is_stochastic_operator(r1_))
        {
          const stochastic_operator& r1=down_cast<const stochastic_operator>(r1_);
          if (is_stochastic_operator(r2_))
          {
            const stochastic_operator& r2=down_cast<const stochastic_operator>(r2_);
            process_expression new_body1=r1.operand();
            process_expression new_body2=r2.operand();
            std::set<variable> variables_occurring_in_rhs_of_sigma;
            variable_list stochvars=r2.variables();
            mutable_map_substitution<> local_sigma;
            alphaconvert(stochvars,local_sigma, r1.variables(),data_expression_list(),variables_occurring_in_rhs_of_sigma);
            new_body2=substitute_pCRLproc(new_body2, local_sigma, variables_occurring_in_rhs_of_sigma);
            const data_expression new_distribution=
                   process::replace_variables_capture_avoiding(r2.distribution(),local_sigma,variables_occurring_in_rhs_of_sigma);

            return stochastic_operator(r1.variables() + stochvars,
                                       real_times_optimized(r1.distribution(),new_distribution),
                                       merge(new_body1,new_body2));
          }
          /* r1 is and r2_ is not a stochastic operator */
          return stochastic_operator(r1.variables(),r1.distribution(),merge(r1.operand(),r2_));
        }
        if (is_stochastic_operator(r2_))
        {
          /* r1_ is not and r2_ is a stochastic operator */
          const stochastic_operator& r2=down_cast<const stochastic_operator>(r2_);
          return stochastic_operator(r2.variables(),r2.distribution(),merge(r1_,r2.operand()));
        }
        /* neither r1_ nor r2_ is stochastic */
        return t;
      }

      if (is_process_instance(t))
      {
        assert(0);
      }

      if (is_process_instance_assignment(t))
      {
        const process_instance_assignment u(t);
        const process_identifier old_identifier=u.identifier();

        size_t n=objectIndex(old_identifier);
        const process_expression new_process=obtain_initial_distribution(old_identifier,visited,pCRLprocs);
        if (is_stochastic_operator(new_process))
        {
          // Remove the initial stochastic_distribution.
          const stochastic_operator& sto=down_cast<const stochastic_operator>(new_process);
          if (!is_process_instance_assignment(sto.operand()))
          {
            process_identifier new_identifier=
                               newprocess(objectdata[n].parameters + sto.variables(),
                                          sto.operand(),
                                          objectdata[n].processstatus,
                                          objectdata[n].canterminate,
                                          objectdata[n].containstime);
            if (std::find(pCRLprocs.begin(),pCRLprocs.end(),new_identifier)==pCRLprocs.end())
            {
              pCRLprocs.push_back(new_identifier);
            }
            return stochastic_operator(sto.variables(),
                                       sto.distribution(),
                                       process_instance_assignment(new_identifier,u.assignments()));
          }
          std::set<variable> variables_occurring_in_rhs_of_sigma;
          mutable_map_substitution<> local_sigma;
          for(const assignment& a: u.assignments())
          {
            local_sigma[a.lhs()]=a.rhs();
            const std::set<variable> varset=find_free_variables(a.rhs());
            variables_occurring_in_rhs_of_sigma.insert(varset.begin(),varset.end());
          }
          return substitute_pCRLproc(sto,local_sigma, variables_occurring_in_rhs_of_sigma); 
        }
        return t;

      }

      if (is_hide(t))
      {
        const process_expression r_=obtain_initial_distribution_term(hide(t).operand(),visited,pCRLprocs);
        if (is_stochastic_operator(r_))
        {
          const stochastic_operator& r=down_cast<const stochastic_operator>(r_);
          return stochastic_operator(r.variables(),r.distribution(),hide(hide(t).hide_set(),r.operand()));
        }
        return t;
      }

      if (is_rename(t))
      {
        const process_expression r_=obtain_initial_distribution_term(process::rename(t).operand(),visited,pCRLprocs);
        if (is_stochastic_operator(r_))
        {
          const stochastic_operator& r=down_cast<const stochastic_operator>(r_);
          return stochastic_operator(r.variables(),
                                     r.distribution(),
                                     process::rename(process::rename(t).rename_set(),r.operand()));
        }
        return t;
      }

      if (is_allow(t))
      {
        const process_expression r_=obtain_initial_distribution_term(allow(t).operand(),visited,pCRLprocs);
        if (is_stochastic_operator(r_))
        {
          const stochastic_operator& r=down_cast<const stochastic_operator>(r_);
          return stochastic_operator(r.variables(),r.distribution(),allow(allow(t).allow_set(),r.operand()));
        }
        return t;
      }

      if (is_block(t))
      {
        const process_expression r_=obtain_initial_distribution_term(block(t).operand(),visited,pCRLprocs);
        if (is_stochastic_operator(r_))
        {
          const stochastic_operator& r=down_cast<const stochastic_operator>(r_);
          return stochastic_operator(r.variables(),r.distribution(),block(block(t).block_set(),r.operand()));
        }
        return t;
      }

      if (is_comm(t))
      {
        const process_expression r_=obtain_initial_distribution_term(comm(t).operand(),visited,pCRLprocs);
        if (is_stochastic_operator(r_))
        {
          const stochastic_operator& r=down_cast<const stochastic_operator>(r_);
          return stochastic_operator(r.variables(),r.distribution(),comm(comm(t).comm_set(),r.operand()));
        }
        return t;
      }

      if (is_choice(t))
      {
        const process_expression r1_=obtain_initial_distribution_term(choice(t).left(),visited,pCRLprocs);
        const process_expression r2_=obtain_initial_distribution_term(choice(t).right(),visited,pCRLprocs);
        if (is_stochastic_operator(r1_))
        {
          const stochastic_operator& r1=down_cast<const stochastic_operator>(r1_);
          if (is_stochastic_operator(r2_))
          {
            /* both r1_ and r2_ are stochastic */
            const stochastic_operator& r2=down_cast<const stochastic_operator>(r2_);
            process_expression new_body1=r1.operand();
            process_expression new_body2=r2.operand();
            std::set<variable> variables_occurring_in_rhs_of_sigma;
            variable_list stochvars=r2.variables();
            mutable_map_substitution<> local_sigma;
            alphaconvert(stochvars,local_sigma, r1.variables(),data_expression_list(),variables_occurring_in_rhs_of_sigma);
            new_body2=substitute_pCRLproc(new_body2, local_sigma, variables_occurring_in_rhs_of_sigma);
            const data_expression new_distribution=
                   process::replace_variables_capture_avoiding(r2.distribution(),local_sigma,variables_occurring_in_rhs_of_sigma);

            return stochastic_operator(r1.variables() + stochvars,
                                       real_times_optimized(r1.distribution(),new_distribution),
                                       choice(new_body1,new_body2));
          }
          /* r1 is and r2_ is not a stochastic operator */
          return stochastic_operator(r1.variables(),r1.distribution(),choice(r1.operand(),r2_));
        }
        if (is_stochastic_operator(r2_))
        {
          /* r1_ is not and r2_ is a stochastic operator */
          const stochastic_operator& r2=down_cast<const stochastic_operator>(r2_);
          return stochastic_operator(r2.variables(),r2.distribution(),choice(r1_,r2.operand()));
        }
        /* neither r1_ nor r2_ is stochastic */
        return t;
      }

      if (is_seq(t))
      {
        const process_expression r_=obtain_initial_distribution_term(seq(t).left(),visited,pCRLprocs);
        if (is_stochastic_operator(r_))
        {
          const stochastic_operator& r=down_cast<const stochastic_operator>(r_);
          return stochastic_operator(r.variables(),r.distribution(),seq(r.operand(),seq(t).right()));
        }
        return t;
      }

      if (is_if_then(t))
      {
        const process_expression r_=obtain_initial_distribution_term(if_then(t).then_case(),visited,pCRLprocs);
        if (is_stochastic_operator(r_))
        {
          const if_then& t_if_then=atermpp::down_cast<if_then>(t);
          const stochastic_operator& r=down_cast<const stochastic_operator>(r_);
          return stochastic_operator(r.variables(),
                                     if_(t_if_then.condition(),r.distribution(),if_(variables_are_equal_to_default_values(r.variables()),real_one(),real_zero())),
                                     if_then(t_if_then.condition(),r.operand()));
        }
        return t;
      }

      if (is_if_then_else(t))
      {
        const process_expression r1_=obtain_initial_distribution_term(if_then_else(t).then_case(),visited,pCRLprocs);
        const process_expression r2_=obtain_initial_distribution_term(if_then_else(t).else_case(),visited,pCRLprocs);
        if (is_stochastic_operator(r1_))
        {
          const stochastic_operator& r1=down_cast<const stochastic_operator>(r1_);
          if (is_stochastic_operator(r2_))
          {
            /* both r1_ and r2_ are stochastic */

            const stochastic_operator& r2=down_cast<const stochastic_operator>(r2_);
            process_expression new_body1=r1.operand();
            process_expression new_body2=r2.operand();
            std::set<variable> variables_occurring_in_rhs_of_sigma;
            variable_list stochvars=r2.variables();
            mutable_map_substitution<> local_sigma;
            alphaconvert(stochvars,local_sigma, r1.variables(),data_expression_list(),variables_occurring_in_rhs_of_sigma);
            new_body2=substitute_pCRLproc(new_body2, local_sigma, variables_occurring_in_rhs_of_sigma);
            const data_expression new_distribution=
                   process::replace_variables_capture_avoiding(r2.distribution(),local_sigma,variables_occurring_in_rhs_of_sigma);

            return stochastic_operator(r1.variables() + stochvars,
                                       if_(if_then_else(t).condition(),
                                               lazy::and_(variables_are_equal_to_default_values(stochvars),r1.distribution()),
                                               lazy::and_(variables_are_equal_to_default_values(r1.variables()),new_distribution)),
                                       if_then_else(if_then_else(t).condition(),new_body1,new_body2));
          }
          /* r1 is and r2_ is not a stochastic operator */
          return stochastic_operator(r1.variables(),
                                     if_(if_then_else(t).condition(),r1.distribution(),
                                               if_(variables_are_equal_to_default_values(r1.variables()),real_one(),real_zero())),
                                     if_then_else(if_then_else(t).condition(),r1.operand(),r2_));
        }
        if (is_stochastic_operator(r2_))
        {
          /* r1_ is not and r2_ is a stochastic operator */
          const stochastic_operator& r2=down_cast<const stochastic_operator>(r2_);
          return stochastic_operator(r2.variables(),
                                     if_(if_then_else(t).condition(),
                                         if_(variables_are_equal_to_default_values(r2.variables()),real_one(),real_zero()),
                                         r2.distribution()),
                                     if_then_else(if_then_else(t).condition(),r1_,r2.operand()));
        }
        /* neither r1_ nor r2_ is stochastic */
        return t;

      }

      if (is_sum(t))
      {
        const sum& s=down_cast<const sum>(t);
        const process_expression r_= obtain_initial_distribution_term(s.operand(),visited,pCRLprocs);
        if (is_stochastic_operator(r_))
        {
          const stochastic_operator& r=down_cast<const stochastic_operator>(r_);
          std::set <variable> variables_in_distribution=find_all_variables(r.distribution());
          for(variable_list::const_iterator i=s.variables().begin(); i!=s.variables().end(); ++i)
          {
            if (variables_in_distribution.count(*i)>0)
            {
              throw mcrl2::runtime_error("Cannot commute a sum operator over a stochastic operator in " +
                                                   process::pp(t) + ".\n" +
                                         "The problematic variable is " + pp(*i) + ":" + pp(i->sort()) + ".");
            }
          }
          return stochastic_operator(r.variables(),
                                     r.distribution(),
                                     sum(s.variables(),r.operand()));
         }
        return t;
      }

      if (is_stochastic_operator(t))
      {
        const stochastic_operator& sto=down_cast<const stochastic_operator>(t);
        const process_expression r_= obtain_initial_distribution_term(sto.operand(),visited,pCRLprocs);

        if (is_stochastic_operator(r_))
        {
          const stochastic_operator& r=down_cast<const stochastic_operator>(r_);
          return stochastic_operator(sto.variables()+r.variables(),
                                     real_times_optimized(sto.distribution(),r.distribution()),
                                     r.operand());
        }
        return stochastic_operator(sto.variables(),sto.distribution(),r_);
      }

      if (is_action(t))
      {
        return t;
      }

      if (is_delta(t))
      {
        return t;
      }

      if (is_tau(t))
      {
        return t;
      }

      if (is_at(t))
      {
        const process_expression r_=obtain_initial_distribution_term(at(t).operand(),visited,pCRLprocs);
        if (is_stochastic_operator(r_))
        {
          const stochastic_operator& r=down_cast<const stochastic_operator>(r_);
          return stochastic_operator(r.variables(),r.distribution(),at(r.operand(),at(t).time_stamp()));
        }
        return t;
      }

      if (is_sync(t))
      {
        return t;
      }

      throw mcrl2::runtime_error("unexpected process format in canterminate " + process::pp(t) +".");
    }

    process_expression obtain_initial_distribution(const process_identifier& procId,
                                                   std::set < process_identifier >& visited,
                                                   std::vector < process_identifier>& pCRLprocs)
    {
      if (visited.count(procId)>0)
      {
        throw mcrl2::runtime_error("Unguarded recursion in process " + process::pp(procId));
      }
      visited.insert(procId);
      size_t n=objectIndex(procId);
      const process_expression initial_distribution_=obtain_initial_distribution_term(objectdata[n].processbody,visited,pCRLprocs);
      visited.erase(procId);
      if (!is_stochastic_operator(initial_distribution_))
      {
        return process_instance_assignment(procId,assignment_list());
      }
      const stochastic_operator& initial_distribution=down_cast<const stochastic_operator>(initial_distribution_);
      if (!is_process_instance_assignment(initial_distribution.operand()))
      {
        const process_identifier new_procId=
                 newprocess(objectdata[n].parameters + initial_distribution.variables(),
                            initial_distribution.operand(),
                            pCRL, canterminatebody(initial_distribution.operand()), containstimebody(initial_distribution.operand()));

        if (std::find(pCRLprocs.begin(),pCRLprocs.end(),new_procId)==pCRLprocs.end())
        {
          pCRLprocs.push_back(new_procId);
        }

        return stochastic_operator(initial_distribution.variables(),
                                   initial_distribution.distribution(),
                                   process_instance_assignment(new_procId,assignment_list())); // TODO add correct assignment here.
      } 
      return initial_distribution;
    }

    /***** determinewhetherprocessescanterminate(init); **********/

    bool canterminatebody(
      const process_expression& t,
      bool& stable,
      std::set < process_identifier >& visited,
      const bool allowrecursion)
    {
      if (is_merge(t))
      {
        /* the construction below is needed to guarantee that
           both subterms are recursively investigated */
        const bool r1=canterminatebody(process::merge(t).left(),stable,visited,allowrecursion);
        const bool r2=canterminatebody(process::merge(t).right(),stable,visited,allowrecursion);
        return r1&&r2;
      }

      if (is_process_instance(t))
      {
        assert(0);
        if (allowrecursion)
        {
          return (canterminate_rec(process_instance(t).identifier(),stable,visited));
        }
        return objectdata[objectIndex(process_instance(t).identifier())].canterminate;
      }

      if (is_process_instance_assignment(t))
      {
        const process_instance_assignment u(t);
        if (allowrecursion)
        {
          return (canterminate_rec(u.identifier(),stable,visited));
        }
        return objectdata[objectIndex(u.identifier())].canterminate;
      }

      if (is_hide(t))
      {
        return (canterminatebody(hide(t).operand(),stable,visited,allowrecursion));
      }

      if (is_rename(t))
      {
        return (canterminatebody(process::rename(t).operand(),stable,visited,allowrecursion));
      }

      if (is_allow(t))
      {
        return (canterminatebody(allow(t).operand(),stable,visited,allowrecursion));
      }

      if (is_block(t))
      {
        return (canterminatebody(block(t).operand(),stable,visited,allowrecursion));
      }

      if (is_comm(t))
      {
        return (canterminatebody(comm(t).operand(),stable,visited,allowrecursion));
      }

      if (is_choice(t))
      {
        const bool r1=canterminatebody(choice(t).left(),stable,visited,allowrecursion);
        const bool r2=canterminatebody(choice(t).right(),stable,visited,allowrecursion);
        return r1||r2;
      }

      if (is_seq(t))
      {
        const bool r1=canterminatebody(seq(t).left(),stable,visited,allowrecursion);
        const bool r2=canterminatebody(seq(t).right(),stable,visited,allowrecursion);
        return r1&&r2;
      }

      if (is_if_then(t))
      {
        return canterminatebody(if_then(t).then_case(),stable,visited,allowrecursion);
      }

      if (is_if_then_else(t))
      {
        const bool r1=canterminatebody(if_then_else(t).then_case(),stable,visited,allowrecursion);
        const bool r2=canterminatebody(if_then_else(t).else_case(),stable,visited,allowrecursion);
        return r1||r2;
      }

      if (is_sum(t))
      {
        return (canterminatebody(sum(t).operand(),stable,visited,allowrecursion));
      }

      if (is_stochastic_operator(t))
      {
        return (canterminatebody(stochastic_operator(t).operand(),stable,visited,allowrecursion));
      }

      if (is_action(t))
      {
        return true;
      }

      if (is_delta(t))
      {
        return false;
      }

      if (is_tau(t))
      {
        return true;
      }

      if (is_at(t))
      {
        return canterminatebody(at(t).operand(),stable,visited,allowrecursion);
      }

      if (is_sync(t))
      {
        const bool r1=canterminatebody(process::sync(t).left(),stable,visited,allowrecursion);
        const bool r2=canterminatebody(process::sync(t).right(),stable,visited,allowrecursion);
        return r1&&r2;
      }

      throw mcrl2::runtime_error("unexpected process format in canterminate " + process::pp(t) +".");
      return false;
    }

    bool canterminate_rec(
      const process_identifier& procId,
      bool& stable,
      std::set < process_identifier >& visited)
    {
      size_t n=objectIndex(procId);

      if (visited.count(procId)==0)
      {
        visited.insert(procId);
        const bool ct=canterminatebody(objectdata[n].processbody,stable,visited,1);
        if (objectdata[n].canterminate!=ct)
        {
          objectdata[n].canterminate=ct;
          if (stable)
          {
            stable=false;
          }
        }
      }
      return (objectdata[n].canterminate);
    }

    bool canterminatebody(const process_expression& t)
    {
      std::set < process_identifier > visited;
      bool stable=false;
      return canterminatebody(t,stable,visited,false);
    }

    void determinewhetherprocessescanterminate(const process_identifier& procId)
    {
      bool stable=false;
      while (!stable)
      {
        std::set < process_identifier > visited;
        stable=true;
        canterminate_rec(procId,stable,visited);
      }
    }

    /*****  distinguishmCRLandpCRLprocsAndAddTerminatedAction  ******/

    process_identifier split_process(const process_identifier& procId,
                                     std::map < process_identifier,process_identifier >& visited_id,
                                     std::map < process_expression,process_expression >& visited_proc)
    {
      if (visited_id.count(procId)>0)
      {
        return visited_id[procId];
      }

      size_t n=objectIndex(procId);

      if ((objectdata[n].processstatus!=mCRL) &&
          (objectdata[n].canterminate==0))
      {
        /* no new process needs to be constructed */
        return procId;
      }

      const process_identifier newProcId(
                   fresh_identifier_generator(procId.name()),
                   objectdata[n].parameters);
      visited_id[procId]=newProcId;

      if (objectdata[n].processstatus==mCRL)
      {
        insertProcDeclaration(
          newProcId,
          objectdata[n].parameters,
          split_body(objectdata[n].processbody,
                     visited_id,visited_proc,
                     objectdata[n].parameters),
          mCRL,0,false);
        return newProcId;
      }

      if (objectdata[n].canterminate)
      {
        assert(check_valid_process_instance_assignment(terminatedProcId,assignment_list()));
        insertProcDeclaration(
          newProcId,
          objectdata[n].parameters,
          seq(objectdata[n].processbody, process_instance_assignment(terminatedProcId,assignment_list())),
          pCRL,canterminatebody(objectdata[n].processbody),
          containstimebody(objectdata[n].processbody));
        return newProcId;
      }
      return procId;
    }

/* Transform process_arguments
 *   This function replaces process_instances by process_instance_assignments.
 *   All assignments in a process_instance_assignment are ordered in the same
 *   sequence as the parameters belonging to that assignment.
 *   All assignments in a process_instance_assignment of the form x=x where
 *   x is not a bound variable are removed.
*/


    void transform_process_arguments(
            const process_identifier& procId,
            std::set<process_identifier>& visited_processes)
    {
      if (visited_processes.count(procId)==0)
      {
        visited_processes.insert(procId);
        size_t n=objectIndex(procId);
        const std::set<variable> bound_variables;
        objectdata[n].processbody=transform_process_arguments_body(
                                         objectdata[n].processbody,
                                         bound_variables,
                                         visited_processes);
      }
    }

    void transform_process_arguments(const process_identifier& procId)
    {
      std::set<process_identifier> visited_processes;
      transform_process_arguments(procId,visited_processes);
    }

    process_expression transform_process_arguments_body(
      const process_expression t, // intentionally not a reference.
      const std::set<variable>& bound_variables,
      std::set<process_identifier>& visited_processes)
    {
      if (is_process_instance(t))
      {
        transform_process_arguments(process_instance(t).identifier(),visited_processes);
        return transform_process_instance_to_process_instance_assignment(atermpp::down_cast<process_instance>(t),bound_variables);
      }
      if (is_process_instance_assignment(t))
      {
        transform_process_arguments(process_instance_assignment(t).identifier(),visited_processes);
        const process_instance_assignment u(t);
        size_t n=objectIndex(u.identifier());
        assert(check_valid_process_instance_assignment(u.identifier(),
                 sort_assignments(u.assignments(),objectdata[n].parameters)));
        return process_instance_assignment(
                     u.identifier(),
                     sort_assignments(u.assignments(),objectdata[n].parameters));
      }
      if (is_hide(t))
      {
        return hide(hide(t).hide_set(),
                    transform_process_arguments_body(atermpp::down_cast<process_instance>(hide(t).operand()),bound_variables,visited_processes));
      }
      if (is_rename(t))
      {
        return process::rename(
                 process::rename(t).rename_set(),
                 transform_process_arguments_body(process::rename(t).operand(),bound_variables,visited_processes));
      }
      if (is_allow(t))
      {
        return allow(allow(t).allow_set(),
                     transform_process_arguments_body(allow(t).operand(),bound_variables,visited_processes));
      }
      if (is_block(t))
      {
        return block(block(t).block_set(),
                     transform_process_arguments_body(block(t).operand(),bound_variables,visited_processes));
      }
      if (is_comm(t))
      {
        return comm(comm(t).comm_set(),
                    transform_process_arguments_body(comm(t).operand(),bound_variables,visited_processes));
      }
      if (is_merge(t))
      {
        return merge(
                 transform_process_arguments_body(merge(t).left(),bound_variables,visited_processes),
                 transform_process_arguments_body(merge(t).right(),bound_variables,visited_processes));
      }
      if (is_choice(t))
      {
        return choice(
                 transform_process_arguments_body(choice(t).left(),bound_variables,visited_processes),
                 transform_process_arguments_body(choice(t).right(),bound_variables,visited_processes));
      }
      if (is_seq(t))
      {
        return seq(
                 transform_process_arguments_body(seq(t).left(),bound_variables,visited_processes),
                 transform_process_arguments_body(seq(t).right(),bound_variables,visited_processes));
      }
      if (is_if_then_else(t))
      {
        return if_then_else(
                 if_then_else(t).condition(),
                 transform_process_arguments_body(if_then_else(t).then_case(),bound_variables,visited_processes),
                 transform_process_arguments_body(if_then_else(t).else_case(),bound_variables,visited_processes));
      }
      if (is_if_then(t))
      {
        return if_then(
                 if_then(t).condition(),
                 transform_process_arguments_body(if_then(t).then_case(),bound_variables,visited_processes));
      }
      if (is_sum(t))
      {
        std::set<variable> bound_variables1=bound_variables;
        const variable_list sum_vars=sum(t).variables();
        bound_variables1.insert(sum_vars.begin(),sum_vars.end());
        return sum(
                 sum_vars,
                 transform_process_arguments_body(sum(t).operand(),bound_variables1,visited_processes));
      }
      if (is_action(t))
      {
        return t;
      }
      if (is_delta(t))
      {
        return t;
      }
      if (is_tau(t))
      {
        return t;
      }
      if (is_at(t))
      {
        return at(
                 transform_process_arguments_body(at(t).operand(),bound_variables,visited_processes),
                 at(t).time_stamp());
      }
      if (is_sync(t))
      {
        return process::sync(
                 transform_process_arguments_body(process::sync(t).left(),bound_variables,visited_processes),
                 transform_process_arguments_body(process::sync(t).right(),bound_variables,visited_processes));
      }
      if (is_stochastic_operator(t))
      {
        const stochastic_operator tso=down_cast<const stochastic_operator>(t);
        return stochastic_operator(
                 tso.variables(),
                 tso.distribution(),
                 transform_process_arguments_body(tso.operand(),bound_variables,visited_processes));
      }
      throw mcrl2::runtime_error("unexpected process format in transform_process_arguments_body " + process::pp(t) +".");
    }

//---------------------------------------------------------------------------------------------------------------------------------------------------
/* guarantee that all process parameters have a unique sort.
 *   If different process parameters or sum variables occur with the same string, e.g. x:A en x:B,
 *   then one of them is renamed, such that all variable strings have a unique
 *   type. The names are replaced in object_data and in assignment lists.
*/


    void guarantee_that_parameters_have_unique_type(
            const process_identifier& procId,
            std::set<process_identifier>& visited_processes,
            std::set<identifier_string>& used_variable_names,
            mutable_map_substitution<>& parameter_mapping,
            std::set<variable>& variables_in_lhs_of_parameter_mapping,
            std::set<variable>& variables_in_rhs_of_parameter_mapping)
    {
      if (visited_processes.count(procId)==0)
      {
        visited_processes.insert(procId);
        size_t n=objectIndex(procId);
        const variable_list parameters=objectdata[n].parameters;
        for(variable_list::const_iterator i=parameters.begin(); i!=parameters.end(); ++i)
        {
          if (used_variable_names.count(i->name())==0)
          {
            used_variable_names.insert(i->name());
            parameter_mapping[*i]=*i;  // This is the first parameter with this name. Map it to itself.
            variables_in_lhs_of_parameter_mapping.insert(*i);
            variables_in_rhs_of_parameter_mapping.insert(*i);
          }
          else
          {
            // A variable already exists with this name.
            if (variables_in_lhs_of_parameter_mapping.count(*i)==0) // The variables must be separately stored, as the parameter_mapping
                                                                    // forgets variables mapped to itself.
            {
              // This parameter needs a fresh name.
              const variable fresh_var(fresh_identifier_generator(i->name()),i->sort());
              parameter_mapping[*i]=fresh_var;
              variables_in_lhs_of_parameter_mapping.insert(*i);
              variables_in_rhs_of_parameter_mapping.insert(fresh_var);
            }
          }
        }
        objectdata[n].old_parameters=objectdata[n].parameters;
        objectdata[n].parameters=data::replace_variables(parameters,parameter_mapping);
        objectdata[n].processbody=guarantee_that_parameters_have_unique_type_body(
                                         objectdata[n].processbody,
                                         visited_processes,
                                         used_variable_names,
                                         parameter_mapping,
                                         variables_in_lhs_of_parameter_mapping,
                                         variables_in_rhs_of_parameter_mapping);
      }
    }

    void guarantee_that_parameters_have_unique_type(const process_identifier& procId)
    {
      std::set<process_identifier> visited_processes;
      std::set<identifier_string> used_variable_names;
      mutable_map_substitution<> parameter_mapping;
      std::set<variable> variables_in_lhs_of_parameter_mapping;
      std::set<variable> variables_in_rhs_of_parameter_mapping;
      guarantee_that_parameters_have_unique_type(procId,
                                                 visited_processes,
                                                 used_variable_names,
                                                 parameter_mapping,
                                                 variables_in_lhs_of_parameter_mapping,
                                                 variables_in_rhs_of_parameter_mapping);
    }

    process_expression guarantee_that_parameters_have_unique_type_body(
      const process_expression t, // intentionally not a reference.
      std::set<process_identifier>& visited_processes,
      std::set<identifier_string>& used_variable_names,
      mutable_map_substitution<>& parameter_mapping,
      std::set<variable>& variables_in_lhs_of_parameter_mapping,
      std::set<variable>& variables_in_rhs_of_parameter_mapping)
    {
      if (is_process_instance_assignment(t))
      {
        guarantee_that_parameters_have_unique_type(process_instance_assignment(t).identifier(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping);
        const process_instance_assignment u(t);
        size_t n=objectIndex(u.identifier());
        assert(check_valid_process_instance_assignment(u.identifier(),
                 substitute_assignmentlist(u.assignments(),objectdata[n].old_parameters,true,true,parameter_mapping,variables_in_rhs_of_parameter_mapping)));
        return process_instance_assignment(
                     u.identifier(),
                     substitute_assignmentlist(u.assignments(),objectdata[n].old_parameters,true,true,parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_hide(t))
      {
        return hide(hide(t).hide_set(),
                    guarantee_that_parameters_have_unique_type_body(hide(t).operand(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_rename(t))
      {
        return process::rename(
                 process::rename(t).rename_set(),
                 guarantee_that_parameters_have_unique_type_body(process::rename(t).operand(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_allow(t))
      {
        return allow(allow(t).allow_set(),
                     guarantee_that_parameters_have_unique_type_body(allow(t).operand(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_block(t))
      {
        return block(block(t).block_set(),
                     guarantee_that_parameters_have_unique_type_body(block(t).operand(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_comm(t))
      {
        return comm(comm(t).comm_set(),
                    guarantee_that_parameters_have_unique_type_body(comm(t).operand(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_merge(t))
      {
        return merge(
                 guarantee_that_parameters_have_unique_type_body(merge(t).left(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping),
                 guarantee_that_parameters_have_unique_type_body(merge(t).right(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_choice(t))
      {
        return choice(
                 guarantee_that_parameters_have_unique_type_body(choice(t).left(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping),
                 guarantee_that_parameters_have_unique_type_body(choice(t).right(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_seq(t))
      {
        return seq(
                 guarantee_that_parameters_have_unique_type_body(seq(t).left(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping),
                 guarantee_that_parameters_have_unique_type_body(seq(t).right(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_if_then_else(t))
      {
        return if_then_else(
                 data::replace_variables_capture_avoiding(if_then_else(t).condition(),parameter_mapping,variables_in_rhs_of_parameter_mapping),
                 guarantee_that_parameters_have_unique_type_body(if_then_else(t).then_case(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping),
                 guarantee_that_parameters_have_unique_type_body(if_then_else(t).else_case(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_if_then(t))
      {
        return if_then(
                 data::replace_variables_capture_avoiding(if_then(t).condition(),parameter_mapping,variables_in_rhs_of_parameter_mapping),
                 guarantee_that_parameters_have_unique_type_body(if_then(t).then_case(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_sum(t))
      {
        // Also rename bound variables in a sum, such that there are no two variables with
        // the same name, but different types. We do the renaming globally, i.e. all occurrences of variables
        // x:D that require renaming are renamed to x':D.
        const variable_list parameters=sum(t).variables();
        for(variable_list::const_iterator i=parameters.begin(); i!=parameters.end(); ++i)
        {
          if (used_variable_names.count(i->name())==0)
          {
            used_variable_names.insert(i->name());
            parameter_mapping[*i]=*i;  // This is the first parameter with this name. Map it to itself.
            variables_in_lhs_of_parameter_mapping.insert(*i);
            variables_in_rhs_of_parameter_mapping.insert(*i);
          }
          else
          {
            // A variable already exists with this name.
            if (variables_in_lhs_of_parameter_mapping.count(*i)==0) // The variables must be separately stored, as the parameter_mapping
                                                                    // forgets variables mapped to itself.
            {
              // This parameter needs a fresh name.
              const variable fresh_var(fresh_identifier_generator(i->name()),i->sort());
              parameter_mapping[*i]=fresh_var;
              variables_in_lhs_of_parameter_mapping.insert(*i);
              variables_in_rhs_of_parameter_mapping.insert(fresh_var);
            }
          }
        }
        return sum(
                 data::replace_variables(sum(t).variables(),parameter_mapping),
                 guarantee_that_parameters_have_unique_type_body(sum(t).operand(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_action(t))
      {
        return lps::replace_variables_capture_avoiding(action(t),parameter_mapping,variables_in_rhs_of_parameter_mapping);
      }
      if (is_delta(t))
      {
        return t;
      }
      if (is_tau(t))
      {
        return t;
      }
      if (is_at(t))
      {
        return at(
                 guarantee_that_parameters_have_unique_type_body(at(t).operand(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping),
                 data::replace_variables_capture_avoiding(at(t).time_stamp(),parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_sync(t))
      {
        return process::sync(
                 guarantee_that_parameters_have_unique_type_body(process::sync(t).left(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping),
                 guarantee_that_parameters_have_unique_type_body(process::sync(t).right(),visited_processes,used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      if (is_stochastic_operator(t))
      {
        const stochastic_operator& sto=down_cast<const stochastic_operator>(t);
        // Also rename bound variables in a stochastic operator, such that there are no two variables with
        // the same name, but different types. We do the renaming globally, i.e. all occurrences of variables
        // x:D that require renaming are renamed to x':D.
        const variable_list parameters=sto.variables();
        for(variable_list::const_iterator i=parameters.begin(); i!=parameters.end(); ++i)
        {
          if (used_variable_names.count(i->name())==0)
          {
            used_variable_names.insert(i->name());
            parameter_mapping[*i]=*i;  // This is the first parameter with this name. Map it to itself.
            variables_in_lhs_of_parameter_mapping.insert(*i);
            variables_in_rhs_of_parameter_mapping.insert(*i);
          }
          else
          {
            // A variable already exists with this name.
            if (variables_in_lhs_of_parameter_mapping.count(*i)==0) // The variables must be separately stored, as the parameter_mapping
                                                                    // forgets variables mapped to itself.
            {
              // This parameter needs a fresh name.
              const variable fresh_var(fresh_identifier_generator(i->name()),i->sort());
              parameter_mapping[*i]=fresh_var;
              variables_in_lhs_of_parameter_mapping.insert(*i);
              variables_in_rhs_of_parameter_mapping.insert(fresh_var);
            }
          }
        }
        return stochastic_operator(
                   data::replace_variables(sto.variables(),parameter_mapping),
                   data::replace_variables_capture_avoiding(sto.distribution(),parameter_mapping,variables_in_rhs_of_parameter_mapping),
                   guarantee_that_parameters_have_unique_type_body(sto.operand(),visited_processes,
                           used_variable_names,parameter_mapping,variables_in_lhs_of_parameter_mapping,variables_in_rhs_of_parameter_mapping));
      }
      throw mcrl2::runtime_error("unexpected process format in guarantee_that_parameters_have_unique_type_body " + process::pp(t) +".");
    }

/* -----------------------------   split body  --------------------------- */

    process_expression split_body(
      const process_expression t, // intentionally not a reference.
      std::map < process_identifier,process_identifier >& visited_id,
      std::map < process_expression,process_expression>& visited_proc,
      const variable_list parameters)  //intentionally not a reference.
    {
      /* Replace pCRL process terms that occur in the scope of mCRL processes
         by a process identifier. E.g. (a+b)||c is replaced by X||c and
         a new process equation X=a+b is added. Furthermore, if the replaced
         process can terminate a termination action is put behind it.
         In the example X=(a+b).terminate.delta@0.

         Besides this each ProcessAssignment is transformed into a Process. */

      process_expression result;

      if (visited_proc.count(t)>0)
      {
        return visited_proc[t];
      }

      if (is_merge(t))
      {
        result=process::merge(
                 split_body(process::merge(t).left(),visited_id,visited_proc,parameters),
                 split_body(process::merge(t).right(),visited_id,visited_proc,parameters));
      }
      else if (is_process_instance(t))
      {
        assert(0);
        const process_instance_assignment u=transform_process_instance_to_process_instance_assignment(atermpp::down_cast<process_instance>(t));
        assert(check_valid_process_instance_assignment(split_process(u.identifier(),visited_id,visited_proc),
                 u.assignments()));
        result=process_instance_assignment(
                 split_process(u.identifier(),visited_id,visited_proc),
                 u.assignments());
      }
      else if (is_process_instance_assignment(t))
      {
        const process_instance_assignment u(t);
        size_t n=objectIndex(u.identifier());
        assert(check_valid_process_instance_assignment(split_process(u.identifier(),visited_id,visited_proc),
                 sort_assignments(u.assignments(),objectdata[n].parameters)));
        result=process_instance_assignment(
                 split_process(u.identifier(),visited_id,visited_proc),
                 sort_assignments(u.assignments(),objectdata[n].parameters));
      }
      else if (is_hide(t))
      {
        result=hide(hide(t).hide_set(),
                    split_body(hide(t).operand(),visited_id,visited_proc,parameters));
      }
      else if (is_rename(t))
      {
        result=process::rename(
                 process::rename(t).rename_set(),
                 split_body(process::rename(t).operand(),visited_id,visited_proc,parameters));
      }
      else if (is_allow(t))
      {
        result=allow(allow(t).allow_set(),
                     split_body(allow(t).operand(),visited_id,visited_proc,parameters));
      }
      else if (is_block(t))
      {
        result=block(block(t).block_set(),
                     split_body(block(t).operand(),visited_id,visited_proc,parameters));
      }
      else if (is_comm(t))
      {
        result=comm(comm(t).comm_set(),
                    split_body(comm(t).operand(),visited_id,visited_proc,parameters));
      }
      else if (is_choice(t)||
               is_seq(t)||
               is_if_then_else(t)||
               is_if_then(t)||
               is_sum(t)||
               is_stochastic_operator(t)||
               is_action(t)||
               is_delta(t)||
               is_tau(t)||
               is_at(t)||
               is_sync(t))
      {
        if (canterminatebody(t))
        {
          assert(check_valid_process_instance_assignment(terminatedProcId,assignment_list()));
          const process_identifier p=newprocess(parameters,
                                                seq(t,process_instance_assignment(terminatedProcId,assignment_list())),
                                                pCRL,
                                                false,
                                                containstimebody(t));
          assert(check_valid_process_instance_assignment(p,assignment_list()));
          result=process_instance_assignment(p,assignment_list());
          visited_proc[t]=result;
        }
        else
        {
          const process_identifier p=newprocess(parameters,t,pCRL,false,containstimebody(t));
          assert(check_valid_process_instance_assignment(p,assignment_list()));
          result=process_instance_assignment(p,assignment_list());
          visited_proc[t]=result;
        }
      }
      else
      {
        throw mcrl2::runtime_error("unexpected process format in split process " + process::pp(t) +".");
      }

      return result;
    }

    process_identifier splitmCRLandpCRLprocsAndAddTerminatedAction(
      const process_identifier& procId)
    {
      std::map < process_identifier,process_identifier> visited_id;
      std::map < process_expression,process_expression> visited_proc;
      return split_process(procId,visited_id,visited_proc);
    }

    /**************** AddTerminationActionIfNecessary ****************/

    void AddTerminationActionIfNecessary(const stochastic_action_summand_vector& summands)
    {
      for (stochastic_action_summand_vector::const_iterator i=summands.begin(); i!=summands.end(); ++i)
      {
        const stochastic_action_summand smd=*i;
        const action_list multiaction=smd.multi_action().actions();
        if (multiaction == action_list({ terminationAction }))
        {
          acts.push_front(terminationAction.label());
          mCRL2log(mcrl2::log::warning) << "The action " << process::pp(terminationAction) << " is added to signal termination of the linear process." << std::endl;
          return;
        }
      }
    }

  public:
    /********************** SieveProcDataVars ***********************/

    variable_list SieveProcDataVarsSummands(
      const std::set <variable>& vars,
      const stochastic_action_summand_vector& action_summands,
      const deadlock_summand_vector& deadlock_summands,
      const variable_list& parameters)
    {
      /* In this routine it is checked which free variables
         in vars occur in the summands. Those variables
         that occur in the summands are returned. The
         parameters are needed to check occurrences of vars
         in the assignment list */

      std::set < variable > vars_set(vars.begin(),vars.end());
      std::set < variable > vars_result_set;

      for (deadlock_summand_vector::const_iterator smds=deadlock_summands.begin();
           smds!=deadlock_summands.end(); ++smds)
      {
        const deadlock_summand smd= *smds;

        if (smd.deadlock().has_time())
        {
          filter_vars_by_term(smd.deadlock().time(),vars_set,vars_result_set);
        }
        filter_vars_by_term(smd.condition(),vars_set,vars_result_set);
      }
      for (stochastic_action_summand_vector::const_iterator smds=action_summands.begin();
           smds!=action_summands.end(); ++smds)
      {
        const stochastic_action_summand smd= *smds;

        filter_vars_by_multiaction(smd.multi_action().actions(),vars_set,vars_result_set);
        filter_vars_by_assignmentlist(smd.assignments(),parameters,vars_set,vars_result_set);

        if (smd.multi_action().has_time())
        {
          filter_vars_by_term(smd.multi_action().time(),vars_set,vars_result_set);
        }
        filter_vars_by_term(smd.condition(),vars_set,vars_result_set);
      }
      variable_list result;
      for (std::set < variable >::reverse_iterator i=vars_result_set.rbegin();
           i!=vars_result_set.rend() ; ++i)
      {
        result.push_front(*i);
      }

      return result;
    }

  public:
    variable_list SieveProcDataVarsAssignments(
      const std::set <variable>& vars,
      const assignment_list& assignments,
      const variable_list& parameters)
    {
      const std::set < variable > vars_set(vars.begin(),vars.end());
      std::set < variable > vars_result_set;


      filter_vars_by_assignmentlist(assignments,parameters,vars_set,vars_result_set);

      variable_list result;
      for (std::set < variable >::reverse_iterator i=vars_result_set.rbegin();
           i!=vars_result_set.rend() ; ++i)
      {
        result.push_front(*i);
      }

      return result;
    }

    /**************** transform **************************************/
  public:
    void transform(
      const process_identifier& init,
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      variable_list& parameters,
      assignment_list& initial_state,
      stochastic_distribution& initial_stochastic_distribution)
    {
      /* Then select the BPA processes, and check that the others
         are proper parallel processes */
      transform_process_arguments(init);
      guarantee_that_parameters_have_unique_type(init);
      determine_process_status(init,mCRL);
      determinewhetherprocessescanterminate(init);
      const process_identifier init_=splitmCRLandpCRLprocsAndAddTerminatedAction(init);
      determinewhetherprocessescontaintime(init_);

      std::vector <process_identifier> pcrlprocesslist;
      collectPcrlProcesses(init_,pcrlprocesslist);
      if (pcrlprocesslist.size()==0)
      {
        throw mcrl2::runtime_error("There are no pCRL processes to be linearized. This is most likely due to the use of unguarded recursion in process equations");
        // Note that this can occur with a specification
        // proc P(x:Int) = P(x); init P(1);
      }

      /* Second, transform into GNF with possibly variables as a head,
         but no actions in the tail */
      procstovarheadGNF(pcrlprocesslist);

      /* Third, transform to GNF by subsitution, such that the
         first variable in a sequence is always an actionvariable */
      procstorealGNF(init_,options.lin_method!=lmStack);

      generateLPEmCRL(action_summands,deadlock_summands,init_, options.lin_method!=lmStack,parameters,initial_state,initial_stochastic_distribution);
      allowblockcomposition(action_name_multiset_list(),false,action_summands,deadlock_summands); // This removes superfluous delta summands.
      if (options.final_cluster)
      {
        cluster_actions(action_summands,deadlock_summands,parameters);
      }

      AddTerminationActionIfNecessary(action_summands);
    }

}; // End of the class specification basictype

/**************** linearise **************************************/

mcrl2::lps::stochastic_specification mcrl2::lps::linearise(
  const mcrl2::process::process_specification& type_checked_spec,
  mcrl2::lps::t_lin_options lin_options)
{
  mCRL2log(mcrl2::log::verbose) << "linearising the process specification using the '" << lin_options.lin_method << " ' method.\n";
  data_specification data_spec=type_checked_spec.data();
  std::set<data::sort_expression> s;
  process::find_sort_expressions(type_checked_spec.action_labels(), std::inserter(s, s.end()));
  process::find_sort_expressions(type_checked_spec.equations(), std::inserter(s, s.end()));
  process::find_sort_expressions(type_checked_spec.init(), std::inserter(s, s.end()));
  s.insert(sort_real::real_());
  data_spec.add_context_sorts(s);

  specification_basic_type spec(type_checked_spec.action_labels(),
                                type_checked_spec.equations(),
                                data::variable_list(type_checked_spec.global_variables().begin(),type_checked_spec.global_variables().end()),
                                data_spec,
                                type_checked_spec.global_variables(),
                                lin_options,
                                type_checked_spec);
  process_identifier init=spec.storeinit(type_checked_spec.init());

  //linearise spec
  variable_list parameters;
  assignment_list initial_state;
  stochastic_action_summand_vector action_summands;
  deadlock_summand_vector deadlock_summands;
  stochastic_distribution initial_distribution(
                              variable_list(),
                              sort_real::creal(sort_int::cint(sort_nat::cnat(sort_pos::c1())),sort_pos::c1()));
  spec.transform(init,action_summands,deadlock_summands,parameters,initial_state,initial_distribution);

  // compute global variables
  data::variable_list globals1 = spec.SieveProcDataVarsSummands(spec.global_variables,action_summands,deadlock_summands,parameters);
  data::variable_list globals2 = spec.SieveProcDataVarsAssignments(spec.global_variables,initial_state,parameters);
  std::set<data::variable> global_variables;
  global_variables.insert(globals1.begin(), globals1.end());
  global_variables.insert(globals2.begin(), globals2.end());

  stochastic_linear_process lps(parameters,
                                deadlock_summands,
                                action_summands);
  lps::stochastic_specification spec1(
      spec.data,
      spec.acts,
      global_variables,
      lps,
      stochastic_process_initializer(initial_state,initial_distribution));

  // add missing sorts to the data specification
  lps::complete_data_specification(spec1);
  return spec1;
}
