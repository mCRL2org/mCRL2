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

// linear process libraries.
#include "mcrl2/lps/linearise.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/alpha.h"
// #include "mcrl2/lps/specification.h"
#include "mcrl2/lps/sumelm.h"
#include <mcrl2/lps/constelm.h>
#include "mcrl2/exception.h"

// atermpp includes
#include "mcrl2/atermpp/set.h"

//mCRL2 data
// #include "mcrl2/data/variable.h"
// #include "mcrl2/data/data_expression.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/map_substitution.h"
#include "mcrl2/data/detail/internal_format_conversion.h"
// #include "mcrl2/data/replace.h"
// #include "mcrl2/data/data_specification.h"

//mCRL2 processes
#include "mcrl2/process/process_expression.h"
#include "mcrl2/process/process_equation.h"
#include <mcrl2/process/process_specification.h>

// Boost utilities
#include "boost/format.hpp"
#include "boost/utility.hpp"

// For Aterm library extension functions
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
               error } processstatustype;

/**************** Definitions of object class  ***********************/

typedef enum { none,
               _map,
               func,
               act,
               proc,
               variable_,
               sorttype,
               multiact } objecttype;

class objectdatatype
{ public:
    identifier_string objectname;
    bool constructor;
    process_expression representedprocess;
    process_identifier process_representing_action; /* for actions target sort is used to
                                   indicate the process representing this action. */
    process_expression processbody;
    variable_list parameters;
    processstatustype processstatus;
    objecttype object;
    bool canterminate;
    bool containstime;

    objectdatatype()
    {
      objectname.protect();
      constructor=false;
      representedprocess.protect();
      process_representing_action.protect();
      processbody.protect();
      parameters.protect();
      processstatus=unknown;
      object=none;
      canterminate=0;
      containstime=false;
    }

    objectdatatype(const objectdatatype &o)
    { objectname=o.objectname;
      objectname.protect();
      constructor=o.constructor;
      representedprocess=o.representedprocess;
      representedprocess.protect();
      process_representing_action=o.process_representing_action;
      process_representing_action.protect();
      processbody=o.processbody;
      processbody.protect();
      parameters=o.parameters;
      parameters.protect();
      processstatus=o.processstatus;
      object=o.object;
      canterminate=o.canterminate;
      containstime=o.containstime;
    }

    const objectdatatype& operator=(const objectdatatype &o)
    { objectname=o.objectname;
      objectname.protect();
      constructor=o.constructor;
      representedprocess=o.representedprocess;
      representedprocess.protect();
      process_representing_action=o.process_representing_action;
      process_representing_action.protect();
      processbody=o.processbody;
      processbody.protect();
      parameters=o.parameters;
      parameters.protect();
      processstatus=o.processstatus;
      object=o.object;
      canterminate=o.canterminate;
      containstime=o.containstime;
      return (*this);
    }

    ~objectdatatype()
    { objectname.unprotect();
      representedprocess.unprotect();
      process_representing_action.unprotect();
      processbody.unprotect();
      parameters.unprotect();
    }
};


class specification_basic_type:public boost::noncopyable
{ public:
    action_label_list acts;     /* storage place for actions */
    atermpp::set < variable > global_variables; /* storage place for free variables occurring
                                   in processes ranging over data */
    variable_list initdatavars; /* storage place for free variables in
                                   init clause */
    data_specification data;    /* contains the data specification for the current process.  */

  private:
    class stackoperations;
    class stacklisttype;
    class enumtype;
    class enumeratedtype;

    atermpp::vector < process_equation > procs;
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
    atermpp::vector < process_identifier > seq_varnames; /* Contains names of processes which represent a sequence
                                                            of process variables */
    std::vector < atermpp::vector < process_instance > > representedprocesses; /* contains the sequences of process
                                                         instances that are represented by the variables in seq_varnames */
    t_lin_options options;
    bool timeIsBeingUsed;
    std::vector < objectdatatype > objectdata;

    ATermIndexedSet objectIndexTable;
    atermpp::set < identifier_string > stringTable;
    std::map < identifier_string,unsigned int> freshstringIndices;
    std::vector < enumeratedtype > enumeratedtypes;
    stackoperations *stack_operations_list;

  public:
    specification_basic_type(const action_label_list as,
                             const atermpp::vector< process_equation > &ps,
                             const variable_list idvs,
                             const data_specification& ds,
                             const atermpp::set < data::variable > &glob_vars,
                             const t_lin_options &opt):
                     acts(),global_variables(glob_vars),data(ds),options(opt),
                     timeIsBeingUsed(false)
    { objectIndexTable=ATindexedSetCreate(1024,75);
      stack_operations_list=NULL;
      acts.protect();
      acts=as;
      storeact(acts);
      // global_variables.protect();
      // for(atermpp::set <data::variable>::const_reverse_iterator i=glob_vars.rbegin();
      //                i!=glob_vars.rend(); ++i)
      // { global_variables=push_front(global_variables,*i);
      // }
      procs=ps;
      storeprocs(procs);
      initdatavars.protect();
      initdatavars=idvs;
      if (!opt.norewrite)
      { rewr=mcrl2::data::rewriter(data,opt.rewrite_strategy);
      }
      // The terminationAction and the terminatedProcId must be defined after initialisation of
      // data as otherwise fresh name does not work properly.
      terminationAction.protect();
      terminationAction=action(action_label(fresh_name("Terminate"),sort_expression_list()),data_expression_list());
      terminatedProcId.protect();
      terminatedProcId=process_identifier(fresh_name("Terminated**"),variable_list());
// /* Changed delta() to DeltaAtZero on 24/12/2006. Moved back in spring 2007, as this introduces unwanted time constraints. */
      insertProcDeclaration(
           terminatedProcId,
           variable_list(),
           seq(terminationAction,delta()),
           pCRL,0,false);
      delta_process.protect();
      delta_process=newprocess(variable_list(),delta(),pCRL,0,false);
      tau_process.protect();
      tau_process=newprocess(variable_list(),tau(),pCRL,1,false);
    }

    ~specification_basic_type()
    { for( ; stack_operations_list!=NULL; )
      { stackoperations *temp=stack_operations_list->next;
        delete stack_operations_list;
        stack_operations_list=temp;
      }
      acts.unprotect();
      // global_variables.unprotect();
      initdatavars.unprotect();
      terminationAction.unprotect();
      terminatedProcId.unprotect();
      delta_process.unprotect();
      tau_process.unprotect();

      ATindexedSetDestroy(objectIndexTable);
    }

  private:
    // TODO. This summand constructor should become part of the lps library.
    summand summand_(const variable_list summation_variables,
                     const data_expression condition,
                     const bool is_delta,
                     const action_list actions,
                     const bool has_time,
                     const data_expression time,
                     const assignment_list assignments)
    { return
        (has_time?
             summand(summation_variables,condition,is_delta,actions,time,assignments):
             summand(summation_variables,condition,is_delta,actions,assignments));
    }

    process_expression delta_at_zero(void)
    { return at(delta(), data::sort_real::real_(0));
    }

    bool isDeltaAtZero(const process_expression t)
    { if (!is_at(t))
      { return false;
      }
      if (!is_delta(at(t).operand()))
      { return false;
      }
      return RewriteTerm(at(t).time_stamp())==data::sort_real::real_(0);
    }


    /*****************  store and retrieve basic objects  ******************/

    long addObject(ATermAppl o, bool &b)
    { ATbool isnew=ATfalse;
      unsigned int result=ATindexedSetPut(objectIndexTable,(ATerm)o,&isnew);
      if (objectdata.size()<=result)
      { objectdata.resize(result+1);
      }
      b=isnew?true:false;
      return result;
    }

    long objectIndex(ATermAppl o)
    { // assert(existsObjectIndex(o) >= 0);
      long result=ATindexedSetGetIndex(objectIndexTable,(ATerm)o);
      assert(result>=0); /* object index must always return the index
                            of an existing object, because at the
                            places where objectIndex is used, no
                            checks take place */
      return result;
    }

    void addString(const identifier_string str)
    { stringTable.insert(str);
    }

    bool existsString(const identifier_string str)
    { return stringTable.count(str)>0;
    }

    void insertalias(const alias& a)
    {
      data.add_alias(a);
    }

    void insertsort(const sort_expression sortterm)
    {
      data.add_sort(sortterm);

      if (sortterm.is_basic_sort())
      {
        long n=0;
        const basic_sort sort(sortterm);
        bool isnew=false;
        // const std::string str=sort.name();
        addString(sort.name());

        n=addObject(sort,isnew);

        if ((isnew==0) && (core::gsDebug))
        { std::cerr << "sort " + pp(sort) +  "is added twice\n";
          return;
        }

        objectdata[n].objectname=sortterm;
        objectdata[n].object=sorttype;
        objectdata[n].constructor=0;
        return;
      }
      if (sortterm.is_function_sort())
      { return;
      }
      throw mcrl2::runtime_error("expected a sortterm (2): " + pp(sortterm));
    }

    void insert_equation(const data_equation eqn)
    {
      // if (!options.norewrite) rewr.add_rule(mcrl2::data::data_equation(eqn));
      // if (!options.norewrite) rewr.add_rule(data.normalise_sorts(eqn));
      data.add_equation(eqn);
    }

    process_expression action_list_to_process(const action_list ma)
    { if (ma.size()==0)
      return tau();
      if (ma.size()==1)
      return ma.front();
      return process::sync(ma.front(),action_list_to_process(pop_front(ma)));
    }

    action_list to_action_list(const process_expression p)
    { if (is_tau(p))
      return action_list();

      if (is_action(p))
      return push_front(action_list(),action(p));

      if (is_sync(p))
      { return to_action_list(process::sync(p).left())+to_action_list(process::sync(p).right());
      }
      assert(0);
      return action_list();
    }

    action_label_list getnames(const process_expression multiAction)
    { if (is_action(multiAction))
      { return push_front(action_label_list(),action(multiAction).label());
      }
      assert(is_sync(multiAction));
      return getnames(process::sync(multiAction).left())+getnames(process::sync(multiAction).left());
    }

    // Returns a list of variables with the same sort as the expressions in the list.
    // If the expression is a variable not occurring in the occurs_set that variable
    // is used.
    variable_list make_parameters_rec(const data_expression_list l,
                                      atermpp::set < variable> &occurs_set)
    { variable_list result;
      for(data_expression_list::const_iterator l1=l.begin();
                      l1!=l.end() ; ++l1)
        {
          /* if the current argument of the multi-action is a variable that does
           not occur in result, use this variable. This is advantageous, when joining
           processes to one linear process where variable names are joined. If this
           is not being done (as happened before 4/1/2008) very long lists of parameters
           can occur when linearising using regular2 */
           if ((l1->is_variable()) && std::find(occurs_set.begin(),occurs_set.end(),*l1)==occurs_set.end())
           { const variable v=*l1;
             result=push_front(result,v);
             occurs_set.insert(v);
           }
           else
           { result=push_front(result,variable(get_fresh_variable("a",l1->sort())));
           }
      }
      return reverse(result);
    }

    variable_list getparameters_rec(const process_expression multiAction,
                                    atermpp::set < variable> &occurs_set)
    { if (is_action(multiAction))
      { return make_parameters_rec(action(multiAction).arguments(),occurs_set);
      }
      assert(is_sync(multiAction));
      return getparameters_rec(process::sync(multiAction).left(),occurs_set)+
             getparameters_rec(process::sync(multiAction).right(),occurs_set);
    }

    variable_list getparameters(const process_expression multiAction)
    { atermpp::set < variable > occurs_set;
      return getparameters_rec(multiAction,occurs_set);
    }

    data_expression_list getarguments(const action_list multiAction)
    { data_expression_list result;
      for(action_list::const_iterator l=multiAction.begin(); l!=multiAction.end(); ++l)
      { result=reverse(l->arguments()) + result;
      }
      return reverse(result);
    }

    action_list makemultiaction(const action_label_list actionIds, const data_expression_list args)
    { action_list result;
      data_expression_list::const_iterator e_walker=args.begin();
      for(action_label_list::const_iterator l=actionIds.begin() ; l!=actionIds.end() ; ++l)
      {
        long arity=l->sorts().size();
        data_expression_list temp_args;
        for(unsigned int i=0 ; i< static_cast< unsigned int >(arity); ++i,++e_walker)
        { assert(e_walker!=args.end());
          temp_args=push_front(temp_args,*e_walker);
        }
        temp_args=reverse(temp_args);
        result=push_front(result, action(*l,temp_args));
      }
      assert(e_walker==args.end());
      return reverse(result);
    }

    long addMultiAction(const process_expression multiAction, bool &isnew)
    { const action_label_list actionnames=getnames(multiAction);
      long n=addObject((ATermAppl)(ATermList)actionnames,isnew);

      if (isnew)
      { // tempvar is needed as objectdata can change during a call
        // of getparameters.
        const data_expression_list templist=getparameters(multiAction);
        objectdata[n].parameters=templist;
        objectdata[n].objectname=identifier_string((ATermAppl)(ATermList)actionnames);
        objectdata[n].object=multiact;
        // must separate assignment below as
        // objectdata may change as a side effect of make
        // multiaction.
        const action_list tempvar=makemultiaction(actionnames,
                                      objectdata[n].parameters);
        objectdata[n].processbody=action_list_to_process(tempvar);
      }
      return n;
    }

    void insertvariable(const variable var, const bool mustbenew)
    {
      addString(var.name());

      bool isnew=false;
      long n=addObject(var.name(),isnew);

      if ((!isnew)&&(mustbenew))
      { throw mcrl2::runtime_error("variable " + pp(var) + " already exists");
      }

      objectdata[n].objectname=var.name();
      objectdata[n].object=variable_;
    }

    void insertvariables(const variable_list vars, const bool mustbenew)
    { for(variable_list::const_iterator l=vars.begin(); l!=vars.end(); ++l)
      { insertvariable(*l,mustbenew); }
    }

    /************ upperpowerof2 *********************************************/

    int upperpowerof2(int i)
    /* function yields n for the smallest value n such that
       2^n>=i. This constitutes the number of bits necessary
       to represent a number smaller than i. i is assumed to
       be at least 1. */
    { int n=0;
      int powerof2=1;
      for( ; powerof2< i ; n++)
      { powerof2=2*powerof2; }
      return n;
    }

    data_expression RewriteTerm(const data_expression& t)
    { if (!options.norewrite) return rewr(t);
      return t;
    }

    data_expression_list RewriteTermList(data_expression_list const& t)
    { if (t.empty()) return t;
      return push_front(RewriteTermList(pop_front(t)), RewriteTerm(t.front()));
    }

    action RewriteAction(const action& t)
    { return action(t.label(),RewriteTermList(t.arguments()));
    }

    process_instance RewriteProcess(const process_instance& t)
    { return process_instance(t.identifier(),RewriteTermList(t.actual_parameters()));
    }

    process_expression RewriteMultAct(const process_expression& t)
    { if (is_tau(t))
      return t;

      if (is_action(t))
      return RewriteAction(action(t));

      assert(is_sync(t));  // A multi action is a sequence of actions with a sync operator in between.
      return process::sync(RewriteMultAct(process::sync(t).left()),RewriteMultAct(process::sync(t).right()));
    }


    process_expression pCRLrewrite(const process_expression& t)
    { if (options.norewrite) return t;

      if (is_if_then(t))
      { const data_expression new_cond=RewriteTerm(if_then(t).condition());
        const process_expression new_then_case=pCRLrewrite(if_then(t).then_case());
        if (new_cond==sort_bool::true_())
        { return new_then_case;
        }
        return if_then(new_cond,new_then_case);
      }

      if (is_seq(t))
      { /* only one summand is needed */
        return seq(
                 pCRLrewrite(seq(t).left()),
                 pCRLrewrite(seq(t).right()));
      }

      if (is_at(t))
      { const data_expression atTime=RewriteTerm(at(t).time_stamp());
        const process_expression t1=pCRLrewrite(at(t).operand());
        return at(t1,atTime);
      }

      if (is_delta(t) || is_tau(t))
      { return t;
      }

      if (is_action(t))
      { return RewriteAction(t);
      }

      if (is_process_instance(t))
      { return RewriteProcess(process_instance(t));
      }

      if (is_sync(t))
      { return RewriteMultAct(t);
      }

      throw mcrl2::runtime_error("Expected a term in pCRL format, using only basic process operators: " + pp(t));
      return process_expression();
    }

    /************ storeact ****************************************************/

    long insertAction(const action_label& actionId)
    { bool isnew=false;
      long n=addObject(actionId,isnew);

      if (isnew==0)
      { throw mcrl2::runtime_error("Action " + pp(actionId) + " is added twice\n");
      }

      const identifier_string str=actionId.name();
      addString(str);
      objectdata[n].objectname=str;
      objectdata[n].object=act;
      objectdata[n].process_representing_action=process_identifier();
      return n;
    }

    void storeact(const action_label_list& acts)
    { for(action_label_list::const_iterator l=acts.begin(); l!=acts.end(); ++l)
      { insertAction(*l);
      }
    }

    /************ storeprocs *************************************************/

    long insertProcDeclaration(
                      const process_identifier procId,
                      const variable_list parameters,
                      const process_expression body,
                      processstatustype s,
                      const bool canterminate,
                      const bool containstime)
    { assert(procId.sorts().size()==parameters.size());
      const std::string str=procId.name();
      addString(str);

      bool isnew=false;
      long n=addObject(procId,isnew);

      if (isnew==0)
      { throw mcrl2::runtime_error("Process " + pp(procId) + " is added twice\n");
      }

      objectdata[n].objectname=procId.name();
      objectdata[n].object=proc;
      objectdata[n].processbody=body;
      objectdata[n].canterminate=canterminate;
      objectdata[n].containstime=containstime;
      objectdata[n].processstatus=s;
      objectdata[n].parameters=parameters;
      insertvariables(parameters,false);
      return n;
    }

    void storeprocs(const atermpp::vector< process_equation > & procs)
    { for(atermpp::vector< process_equation >::const_iterator i=procs.begin();
                     i!=procs.end(); ++i)
      { insertProcDeclaration(
                i->identifier(),
                i->formal_parameters(),
                i->expression(),
                unknown,0,false);
      }
    }

    /************ storeinit *************************************************/

 public:
    process_identifier storeinit(const process_expression init)
    { /* init is used as the name of the initial process,
         because it cannot occur as a string in the input */

      process_identifier initprocess(std::string("init"),sort_expression_list());
      insertProcDeclaration(initprocess,variable_list(),init,unknown,0,false);
      return initprocess;
    }

 private:

    /********** various functions on action and multi actions  ***************/
    bool actioncompare(const action_label a1, const action_label a2)
    { /* first compare the strings in the actions */
      if (std::string(a1.name())<std::string(a2.name()))
      { return true; }

      if (a1.name()==a2.name())
      { /* the strings are equal; the sorts are used to
           determine the ordering */
        return a1.sorts()<a2.sorts();
      }

      return false;
    }

    action_list linInsertActionInMultiActionList(
                               const action act,
                               const action_list multiAction)
    { /* store the action in the multiAction, alphabetically
         sorted on the actionname in the actionId. Note that
         the empty multiAction represents tau. */

      if (multiAction.empty())
      { return push_front(multiAction,act);
      }
      const action firstAction=multiAction.front();

      /* Actions are compared on the basis of their position
         in memory, to order them. As the aterm library maintains
         pointers to objects that are not garbage collected, this
         is a safe way to do this. */
      if (actioncompare(act.label(),firstAction.label()))
      { return push_front(multiAction,act);
      }
      return push_front(linInsertActionInMultiActionList(
                               act,
                               pop_front(multiAction)),
                         firstAction);
    }

    action_list linMergeMultiActionList(const action_list ma1, const action_list ma2)
    { action_list result=ma2;
      for(action_list::const_iterator i=ma1.begin() ; i!=ma1.end() ; ++i)
      { result=linInsertActionInMultiActionList(*i,result);
      }
      return result;
    }


    action_list linMergeMultiActionListProcess(const process_expression ma1, const process_expression ma2)
    { return linMergeMultiActionList(to_action_list(ma1),to_action_list(ma2));
    }

    /************** determine_process_status ********************************/

    processstatustype determine_process_statusterm(
                    const process_expression body,
                    const processstatustype status)
    { /* In this procedure it is determined whether a process
         is of type mCRL, pCRL or a multiAction. pCRL processes
         occur strictly within mCRL processes, and multiActions
         occur strictly within pCRL processes. Processes that pass
         this procedure can be linearised. Bounded initialisation,
         the leftmerge and synchronization merge on the highest
         level are filtered out. */
      if (is_choice(body))
      { if (status==multiAction)
        { throw mcrl2::runtime_error("Choice operator occurs in a multi-action in " + pp(body) + ".");
        }
        const processstatustype s1=determine_process_statusterm(choice(body).left(),pCRL);
        const processstatustype s2=determine_process_statusterm(choice(body).right(),pCRL);
        if ((s1==mCRL)||(s2==mCRL))
        { throw mcrl2::runtime_error("mCRL operators occur within the scope of a choice operator in " + pp(body) +".");
        }
        return pCRL;
      }

      if (is_seq(body))
      { if (status==multiAction)
        { throw mcrl2::runtime_error("Sequential operator occurs in a multi-action in " + pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(seq(body).left(),pCRL);
        const processstatustype s2=determine_process_statusterm(seq(body).right(),pCRL);
        if ((s1==mCRL)||(s2==mCRL))
        { throw mcrl2::runtime_error("mCRL operators occur within the scope of a sequential operator in " + pp(body) +".");
        }
        return pCRL;
      }

      if (is_merge(body))
      { if (status!=mCRL)
        { throw mcrl2::runtime_error("Parallel operator occurs in the scope of pCRL operators in " + pp(body) +".");
        }
        determine_process_statusterm(process::merge(body).left(),mCRL);
        determine_process_statusterm(process::merge(body).right(),mCRL);
        return mCRL;
      }

      if (is_left_merge(body))
      { throw mcrl2::runtime_error("Cannot linearize because the specification contains a leftmerge.");
      }

      if (is_if_then(body))
      { if (status==multiAction)
        { throw mcrl2::runtime_error("If-then occurs in a multi-action in " + pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(if_then(body).then_case(),pCRL);
        if (s1==mCRL)
        { throw mcrl2::runtime_error("mCRL operators occur in the scope of the if-then operator in " + pp(body) +".");
        }
        return pCRL;
      }

      if (is_if_then_else(body))
      { if (status==multiAction)
        { throw mcrl2::runtime_error("If-then-else occurs in a multi-action in " + pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(if_then_else(body).then_case(),pCRL);
        const processstatustype s2=determine_process_statusterm(if_then_else(body).else_case(),pCRL);
        if ((s1==mCRL)||(s2==mCRL))
        { throw mcrl2::runtime_error("mCRL operators occur in the scope of the if-then-else operator in " + pp(body) +".");
        }
        return pCRL;
      }

      if (is_sum(body))
      { /* insert the variable names of variables, to avoid
           that this variable name will be reused later on */
        insertvariables(sum(body).bound_variables(),false);
        if (status==multiAction)
        { throw mcrl2::runtime_error("Sum operator occurs within a multi-action in " + pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(sum(body).operand(),pCRL);
        if (s1==mCRL)
        { throw mcrl2::runtime_error("mCRL operators occur in the scope of the sum operator in " + pp(body) +".");
        }
        return pCRL;
      }

      if (is_comm(body))
      { if (status!=mCRL)
        { throw mcrl2::runtime_error("Communication operator occurs in the scope of pCRL operators in " + pp(body) +".");
        }
        determine_process_statusterm(comm(body).operand(),mCRL);
        return mCRL;
      }

      if (is_bounded_init(body))
      { throw mcrl2::runtime_error("Cannot linearize a specification with the bounded initialization operator.");
      }

      if (is_at(body))
      { timeIsBeingUsed = true;
        if (status==multiAction)
        { throw mcrl2::runtime_error("Time operator occurs in a multi-action in " + pp(body) +".");
        }
        const processstatustype s1=determine_process_statusterm(at(body).operand(),pCRL);
        if (s1==mCRL)
        { throw mcrl2::runtime_error("mCRL operator occurs in the scope of a time operator in " + pp(body) +".");
        }
        return pCRL;
      }

      if (is_sync(body))
      { const processstatustype s1=determine_process_statusterm(process::sync(body).left(),pCRL);
        const processstatustype s2=determine_process_statusterm(process::sync(body).right(),pCRL);
        if ((s1!=multiAction)||(s2!=multiAction))
        {
          throw mcrl2::runtime_error("Other objects than multi-actions occur in the scope of a synch operator in " + pp(body) +".");
        }
        return multiAction;
      }

      if (is_action(body))
      { return multiAction;
      }

      if (is_process_instance(body))
      { determine_process_status(process_instance(body).identifier(),status);
        return status;
      }

      if (is_process_instance_assignment(body))
      { determine_process_status(process_instance_assignment(body).identifier(),status);
        return status;
      }

      if (is_delta(body))
      { return pCRL;
      }

      if (is_tau(body))
      { return multiAction;
      }

      if (is_hide(body))
      { if (status!=mCRL)
        { throw mcrl2::runtime_error("Hide operator occurs in the scope of pCRL operators in " + pp(body) +".");
        }
        determine_process_statusterm(hide(body).operand(),mCRL);
        return mCRL;
      }

      if (is_rename(body))
      { if (status!=mCRL)
        { throw mcrl2::runtime_error("Rename operator occurs in the scope of pCRL operators in " + pp(body) +".");
        }
        determine_process_statusterm(process::rename(body).operand(),mCRL);
        return mCRL;
      }

      if (is_allow(body))
      { if (status!=mCRL)
        { throw mcrl2::runtime_error("Allow operator occurs in the scope of pCRL operators in " + pp(body) +".");
        }
        determine_process_statusterm(allow(body).operand(),mCRL);
        return mCRL;
      }

      if (is_block(body))
      { if (status!=mCRL)
        { throw mcrl2::runtime_error("Block operator occurs in the scope of pCRL operators in " + pp(body) +".");
        }
        determine_process_statusterm(block(body).operand(),mCRL);
        return mCRL;
      }

      throw mcrl2::runtime_error("Process has unexpected format (2) " + pp(body) +".");
      return error;
    }


    void determine_process_status(
                       const process_identifier procDecl,
                       const processstatustype status)
    { processstatustype s;
      int n=objectIndex(procDecl);
      assert(n>=0); /* if this fails, the process does not exist */
      s=objectdata[n].processstatus;

      if (s==unknown)
      { objectdata[n].processstatus=status;
        if (status==pCRL)
        { determine_process_statusterm(objectdata[n].processbody,pCRL);
          return;
        }
        /* status==mCRL */
        s=determine_process_statusterm(objectdata[n].processbody,mCRL);
        if (s!=status)
        { /* s==pCRL and status==mCRL */
          objectdata[n].processstatus=s;
          determine_process_statusterm(objectdata[n].processbody,pCRL);
        }
      }
      if (s==mCRL)
      { if (status==pCRL)
        { objectdata[n].processstatus=pCRL;
          determine_process_statusterm(objectdata[n].processbody,pCRL);
        }
      }
    }

    /***********  collect pcrlprocessen **********************************/

    void collectPcrlProcesses_term(const process_expression body,
                                   atermpp::vector <process_identifier>  &pcrlprocesses,
                                   atermpp::set <process_identifier>  &visited)
    { if (is_if_then(body))
      { collectPcrlProcesses_term(if_then(body).then_case(),pcrlprocesses,visited);
        return;
      }

      if (is_if_then_else(body))
      { collectPcrlProcesses_term(if_then_else(body).then_case(),pcrlprocesses,visited);
        collectPcrlProcesses_term(if_then_else(body).else_case(),pcrlprocesses,visited);
        return;
      }

      if (is_choice(body))
      { collectPcrlProcesses_term(choice(body).left(),pcrlprocesses,visited);
        collectPcrlProcesses_term(choice(body).right(),pcrlprocesses,visited);
        return ;
      }

      if (is_seq(body))
      { collectPcrlProcesses_term(seq(body).left(),pcrlprocesses,visited);
        collectPcrlProcesses_term(seq(body).right(),pcrlprocesses,visited);
        return ;
      }

      if (is_merge(body))
      { collectPcrlProcesses_term(process::merge(body).left(),pcrlprocesses,visited);
        collectPcrlProcesses_term(process::merge(body).right(),pcrlprocesses,visited);
        return ;
      }

      if (is_sync(body))
      { collectPcrlProcesses_term(process::sync(body).left(),pcrlprocesses,visited);
        collectPcrlProcesses_term(process::sync(body).right(),pcrlprocesses,visited);
        return ;
      }

      if (is_sum(body))
      { collectPcrlProcesses_term(sum(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_at(body))
      { collectPcrlProcesses_term(at(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_process_instance(body))
      { collectPcrlProcesses(process_instance(body).identifier(),pcrlprocesses,visited);
        return;
      }

      if (is_process_instance_assignment(body))
      { collectPcrlProcesses(process_instance_assignment(body).identifier(),pcrlprocesses,visited);
        return;
      }

      if (is_hide(body))
      { collectPcrlProcesses_term(hide(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_rename(body))
      { collectPcrlProcesses_term(process::rename(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_allow(body))
      { collectPcrlProcesses_term(allow(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_block(body))
      { collectPcrlProcesses_term(block(body).operand(),pcrlprocesses,visited);
        return;
      }

      if (is_comm(body))
      { collectPcrlProcesses_term(comm(body).operand(),pcrlprocesses,visited);
        return;
      }

      if ((is_delta(body))||
          (is_tau(body))||
          (is_action(body)))
      { return;
      }

      throw mcrl2::runtime_error("process has unexpected format (1) " + pp(body) +".");
    }

    void collectPcrlProcesses(
                       const process_identifier procDecl,
                       atermpp::vector <process_identifier>  &pcrlprocesses,
                       atermpp::set <process_identifier>  &visited)
    { if (visited.count(procDecl)==0)
      { visited.insert(procDecl);
        int n=objectIndex(procDecl);
        assert(n>=0); /* if this fails, the process does not exist */
        if (objectdata[n].processstatus==pCRL)
        { pcrlprocesses.push_back(procDecl);
        }
        collectPcrlProcesses_term(objectdata[n].processbody,pcrlprocesses,visited);
      }
    }

    void collectPcrlProcesses(
                       const process_identifier procDecl,
                       atermpp::vector <process_identifier>  &pcrlprocesses)
    { atermpp::set <process_identifier>  visited;
      collectPcrlProcesses(procDecl, pcrlprocesses, visited);
    }

    /************ correctopenterm ********************************************/

    // TODO Should be replaced by a standard function.
    std::string fresh_name(const std::string &name)
    { /* it still has to be checked whether a name is already being used
         in that case a new name has to be generated. */
      identifier_string strng(name);
      unsigned int i=0;
      // strng=new_string(name);
      if (freshstringIndices.count(name)>0)
      { i=freshstringIndices[name];
      }

      for( ; (existsString(strng)) ; i++)
      { strng = identifier_string(str(boost::format(name + "%d") % i ));
      }
      freshstringIndices[name]=i;
      addString(strng);
      const std::string result(strng);
      return result;
    }

    /****************  occursinterm *** occursintermlist ***********/

    bool occursinterm(const variable var, const data_expression t)
    { return search_free_variable(t, var);
    }

    void filter_vars_by_term(
                        const data_expression t,
                        const atermpp::set < variable > &vars_set,
                        atermpp::set < variable > &vars_result_set)
    { if (t.is_variable())
      {
        if (vars_set.find(variable(t))!=vars_set.end())
        { vars_result_set.insert(t);
        }
        return;
      }

      if (t.is_function_symbol())
      { return;
      }

      if (t.is_abstraction())
      { fprintf(stderr,"Warning: filtering of variables expression with binders\n");
        return;
      }

      if (t.is_where_clause())
      { fprintf(stderr,"Warning: filtering of variables expression with where clause\n");
        return;
      }

      assert(t.is_application());

      filter_vars_by_term(application(t).head(),vars_set,vars_result_set);
      filter_vars_by_termlist(application(t).arguments(),vars_set,vars_result_set);
    }

    bool occursintermlist(const variable var, const data_expression_list r)
    { for(data_expression_list::const_iterator l=r.begin() ; l!=r.end() ; ++l)
      { if (occursinterm(var,*l))
        return true;
      }
      return false;
    }

    void filter_vars_by_termlist(
                        const data_expression_list tl,
                        const atermpp::set < variable > &vars_set,
                        atermpp::set < variable > &vars_result_set)
    { for(data_expression_list::const_iterator l=tl.begin(); l!=tl.end(); ++l)
      { filter_vars_by_term(*l,vars_set,vars_result_set);
      }
    }

    void filter_vars_by_multiaction(
                        const action_list multiaction,
                        const atermpp::set < variable > &vars_set,
                        atermpp::set < variable > &vars_result_set)
    { for(action_list::const_iterator ma=multiaction.begin() ; ma!=multiaction.end() ; ++ma)
      { filter_vars_by_termlist(ma->arguments(),vars_set,vars_result_set);
      }
      return;
    }

    void filter_vars_by_assignmentlist(
                     const assignment_list assignments,
                     const variable_list parameters,
                     const atermpp::set < variable > &vars_set,
                     atermpp::set < variable > &vars_result_set)
    { filter_vars_by_termlist(data_expression_list(parameters),vars_set,vars_result_set);
      for(assignment_list::const_iterator i=assignments.begin();
                i!=assignments.end(); ++i)
      { const data_expression rhs=i->rhs();
        filter_vars_by_term(rhs,vars_set,vars_result_set);
      }
    }

    bool occursinpCRLterm(const variable var,
                                 const process_expression p,
                                 const bool strict)
    { if (is_choice(p))
      { return occursinpCRLterm(var,choice(p).left(),strict)||
               occursinpCRLterm(var,choice(p).right(),strict);
      }
      if (is_seq(p))
      { return occursinpCRLterm(var,seq(p).left(),strict)||
               occursinpCRLterm(var,seq(p).right(),strict);
      }
      if (is_if_then(p))
      { return occursinterm(var,if_then(p).condition())||
               occursinpCRLterm(var,if_then(p).then_case(),strict);
      }

      if (is_sum(p))
      { if (strict)
           return occursintermlist(var,sum(p).bound_variables())||
                  occursinpCRLterm(var,sum(p).operand(),strict);
        /* below appears better? , but leads
           to errors. Should be investigated. */
         else
           return
              (!occursintermlist(var,sum(p).bound_variables()))&&
              occursinpCRLterm(var,sum(p).operand(),strict);
      }
      if (is_process_instance(p))
      { return occursintermlist(var,process_instance(p).actual_parameters());
      }
      if (is_action(p))
      { return occursintermlist(var,action(p).arguments());
      }
      if (is_sync(p))
      { return occursinpCRLterm(var,process::sync(p).left(),strict)||
               occursinpCRLterm(var,process::sync(p).right(),strict);
      }
      if (is_at(p))
      { return occursinterm(var,at(p).time_stamp()) ||
               occursinpCRLterm(var,at(p).operand(),strict);
      }
      if (is_delta(p))
       { return false; }
      if (is_tau(p))
       { return false; }
      throw mcrl2::runtime_error("unexpected process format in occursinCRLterm " + pp(p));
      return false;
    }

    void alphaconvertprocess(
               variable_list &sumvars,
               variable_list &rename_vars,
               data_expression_list &rename_terms,
               const process_expression p)
    { /* This function replaces the variables in sumvars
         by unique ones if these variables occur in occurvars
         or occurterms. It extends rename_vars and rename
         terms to rename the replaced variables to new ones. */
      variable_list newsumvars;

      for(variable_list::const_iterator l=sumvars.begin() ;
              l!=sumvars.end() ; ++l)
      { variable var=*l;
        if (occursinpCRLterm(var,p,1))
        { variable newvar=get_fresh_variable(var.name(),var.sort());
          newsumvars=push_front(newsumvars,newvar);
          rename_vars=push_front(rename_vars,var);
          rename_terms=push_front(rename_terms,data_expression(newvar));
        }
        else
          newsumvars=push_front(newsumvars,var);
      }
      sumvars=reverse(newsumvars);
    }


    void alphaconvert(
               variable_list &sumvars,
               variable_list &rename_vars,
               data_expression_list &rename_terms,
               const variable_list occurvars,
               const data_expression_list occurterms)
    { /* This function replaces the variables in sumvars
         by unique ones if these variables occur in occurvars
         or occurterms. It extends rename_vars and rename
         terms to rename the replaced variables to new ones. */
      variable_list newsumvars;

      for(variable_list::const_iterator l=sumvars.begin() ; l!=sumvars.end() ; ++l)
      { const variable var= *l;
        if (occursintermlist(var,data_expression_list(occurvars)) ||
            occursintermlist(var,occurterms))
        { const variable newvar=get_fresh_variable(var.name(),var.sort());
          newsumvars=push_front(newsumvars,newvar);
          rename_vars=push_front(rename_vars,var);
          rename_terms=push_front(rename_terms,data_expression(newvar));
        }
        else
        { newsumvars=push_front(newsumvars,var);
        }
      }
      sumvars=reverse(newsumvars);
    }

    /******************* substitute *****************************************/

    data_expression_list substitute_datalist(
                     const data_expression_list terms,
                     const variable_list vars,
                     const  data_expression_list tl)
    {
       std::map < variable, data_expression > sigma;
       data_expression_list::const_iterator j=terms.begin();
       for(variable_list::const_iterator i=vars.begin();
                        i!=vars.end(); ++i, ++j)
       { /* Substitutions are carried out from left to right. The first applicable substitution counts */
         if (sigma.count(*i)==0)
         { sigma[*i]=*j;
         }
       }
       return data::replace_free_variables(tl,make_map_substitution_adapter(sigma));
    }

    data_expression substitute_data(
                     const data_expression_list terms,
                     const variable_list vars,
                     const data_expression t)
    { /* The code below could be replaced by the code below, but this is too inefficient,
         as the reverse operator is expensive:
          return data::replace_free_variables(t,make_map_substitution(atermpp::reverse(vars), atermpp::reverse(terms)));
      */
      std::map < variable, data_expression > sigma;
      data_expression_list::const_iterator j=terms.begin();
      for(variable_list::const_iterator i=vars.begin();
                        i!=vars.end(); ++i, ++j)
      { /* Substitutions are carried out from left to right. The first applicable substitution counts */
        if (sigma.count(*i)==0)
        { sigma[*i]=*j;
        }
      }
      const data_expression result=data::replace_free_variables(t,make_map_substitution_adapter(sigma));
      return result;

    }

    action_list substitute_multiaction(
                     const data_expression_list terms,
                     const variable_list vars,
                     const action_list multiAction)
    {
      if (multiAction.empty())
      { return multiAction;
      }
      const action act=multiAction.front();
      return push_front(substitute_multiaction(terms,vars,pop_front(multiAction)),
                       action(act.label(),
                                    substitute_datalist(
                                           terms,
                                           vars,
                                           act.arguments())));
    }

    assignment_list substitute_assignmentlist(
                     const data_expression_list terms,
                     const variable_list vars,
                     const assignment_list assignments,
                     const variable_list parameters,
                     int replacelhs,
                     int replacerhs)
    { /* TODO: This should be replaced by standard functions. */
      /* precondition: the variables in the assignment occur in
         the same sequence as in the parameters, which stands for the
         total list of parameters.

         This function replaces the variables in vars by the terms in terms
         in the right hand side of the assignments if replacerhs holds, and
         in the lefthandside of an assignment if replacelhs holds. If for some variable
         occuring in the parameterlist no assignment is present, whereas
         this variable occurs in vars, an assignment for it is added.

      */

      assert(replacelhs==0 || replacelhs==1);
      assert(replacerhs==0 || replacerhs==1);

      if (parameters.empty())
      { assert(assignments.empty());
        return assignments;
      }

      variable parameter=parameters.front();

      if (!assignments.empty())
      { assignment ass=assignments.front();
        data_expression lhs=ass.lhs();
        if (parameter==lhs)
        { /* The assignment refers to parameter par. Substitute its
             left and righthandside and check whether the left and right
             handside have become equal, in which case no assignment
             is necessary anymore */
          data_expression rhs=ass.rhs();

          if (replacelhs)
          { lhs=substitute_data(terms,vars,lhs);
            assert(is_variable(lhs));
          }
          if (replacerhs)
          { rhs=substitute_data(terms,vars,rhs);
          }

          if (lhs==rhs)
          { return substitute_assignmentlist(
                        terms,
                        vars,
                        pop_front(assignments),
                        pop_front(parameters),
                        replacelhs,
                        replacerhs);
          }
          return push_front(
                    substitute_assignmentlist(
                        terms,
                        vars,
                        pop_front(assignments),
                        pop_front(parameters),
                        replacelhs,
                        replacerhs),
                    assignment(lhs,rhs));
        }
      }

      /* Here the first parameter is not equal to the first
         assignment. So, we must find out whether a value
         for this variable is substituted, that is different
         from the variable, in which case an assignment must
         be added. */

      data_expression lhs=parameter;
      data_expression rhs=parameter;

      if (replacelhs)
      { lhs=substitute_data(terms,vars,lhs);
        assert(is_variable(lhs));
      }
      if (replacerhs)
      { rhs=substitute_data(terms,vars,rhs);
      }

      if (lhs==rhs)
      { return substitute_assignmentlist(
                        terms,
                        vars,
                        assignments,
                        pop_front(parameters),
                        replacelhs,
                        replacerhs);
      }
      return push_front(
                    substitute_assignmentlist(
                        terms,
                        vars,
                        assignments,
                        pop_front(parameters),
                        replacelhs,
                        replacerhs),
                    assignment(lhs,rhs));
    }

    data_expression substitute_time(
                     const data_expression_list terms,
                     const variable_list vars,
                     const data_expression time)
    { return substitute_data(terms,vars,time);
    }

    process_expression substitute_pCRLproc(
                     const data_expression_list terms,
                     const variable_list vars,
                     const process_expression p)
    { assert(terms.size()==vars.size());
      if (is_choice(p))
      { return choice(
                    substitute_pCRLproc(terms,vars,choice(p).left()),
                    substitute_pCRLproc(terms,vars,choice(p).right()));
      }
      if (is_seq(p))
      { return seq(
                    substitute_pCRLproc(terms,vars,seq(p).left()),
                    substitute_pCRLproc(terms,vars,seq(p).right()));
      }
      if (is_sync(p))
      { return process::sync(
                    substitute_pCRLproc(terms,vars,process::sync(p).left()),
                    substitute_pCRLproc(terms,vars,process::sync(p).right()));
      }
      if (is_if_then(p))
      { data_expression condition=substitute_data(terms,vars,if_then(p).condition());
        if (condition==sort_bool::false_())
        { return delta_at_zero();
        }
        if (condition==sort_bool::true_())
        { return substitute_pCRLproc(terms,vars,if_then(p).then_case());
        }
        return if_then(condition,substitute_pCRLproc(terms,vars,if_then(p).then_case()));
      }
      if (is_if_then_else(p))
      {
        data_expression condition=substitute_data(terms,vars,if_then_else(p).condition());
        if (condition==sort_bool::false_())
        { return substitute_pCRLproc(terms,vars,if_then_else(p).else_case());
        }
        if (condition==sort_bool::true_())
        { return substitute_pCRLproc(terms,vars,if_then_else(p).then_case());
        }
        return if_then_else(
                    condition,
                    substitute_pCRLproc(terms,vars,if_then_else(p).then_case()),
                    substitute_pCRLproc(terms,vars,if_then_else(p).else_case()));
      }

      if (is_sum(p))
      { variable_list sumargs=sum(p).bound_variables();
        variable_list vars1=vars;
        data_expression_list terms1=terms;

        alphaconvert(sumargs,vars1,terms1,terms,vars);

        const process_expression result=sum(sumargs,
                   substitute_pCRLproc(terms1,vars1,sum(p).operand()));
        return result;
      }

      if (is_process_instance(p))
      {
        return process_instance(process_instance(p).identifier(),
                    substitute_datalist(terms,vars,process_instance(p).actual_parameters()));
      }

      if (is_process_instance_assignment(p))
      { const process_instance q=transform_process_assignment_to_process(p);
        return process_instance(q.identifier(),substitute_datalist(terms,vars,q.actual_parameters()));
      }

      if (is_action(p))
      { return action(action(p).label(),
                    substitute_datalist(terms,vars,action(p).arguments()));
      }

      if (is_at(p))
      {
        return at(substitute_pCRLproc(terms,vars,at(p).operand()),
                  substitute_data(terms,vars,at(p).time_stamp()));
      }

      if (is_delta(p))
         { return p; }

      if (is_tau(p))
         { return p; }

      if (is_sync(p))
      { return process::sync(
                    substitute_pCRLproc(terms,vars,process::sync(p).left()),
                    substitute_pCRLproc(terms,vars,process::sync(p).right()));

      }

      throw mcrl2::runtime_error("expected a pCRL process " + pp(p));
      return process_expression();
    }


    // The function below transforms a ProcessAssignment to a Process, provided
    // that the process is defined in objectnames.

    process_instance transform_process_assignment_to_process(const process_instance_assignment procId)
    { long n=objectIndex(procId.identifier());
      variable_list variables;
      data_expression_list terms;

      assignment_list assignments=procId.assignments();

      // Transform the assignments into a list of variables and substitutable terms;
      for(assignment_list::const_iterator i=assignments.begin(); i!=assignments.end(); ++i)
      { variables=push_front(variables,i->lhs());
        terms=push_front(terms,i->rhs());
      }

      return process_instance(procId.identifier(),
                           substitute_datalist(terms,variables,objectdata[n].parameters));
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
                  const variable_list parameters,
                  const process_expression body)
    { if (parameters.empty())
          return parameters;

      const variable_list parameters1=parameters_that_occur_in_body(pop_front(parameters),body);
      if (occursinpCRLterm(parameters.front(),body,0))
           return push_front(parameters1,parameters.front());
      return parameters1;
    }

    // The variable below is used to count the number of new processes that
    // are made. If this number is very high, it is likely that the regular
    // flag is used, and an unbounded number of new processes are generated.
    // In such a case a warning is printed suggesting to use regular2.

    process_identifier newprocess(
                        const variable_list parameters,
                        const process_expression body,
                        processstatustype ps,
                        const bool canterminate,
                        const bool containstime)
    { unsigned long numberOfNewProcesses=0, warningNumber=1000;
      numberOfNewProcesses++;
      if (numberOfNewProcesses == warningNumber)
      { std::cerr << "generated " << numberOfNewProcesses << " new internal processes.";
        if (!options.lin_method!=lmStack)
        { std::cerr << " A possible unbounded loop can be avoided by using `regular2' or `stack' as linearisation method.\n";
        }
        else if (options.lin_method==lmRegular2)
        { std::cerr << " A possible unbounded loop can be avoided by using `stack' as the linearisation method.\n";
        }
        else
        { std::cerr << "\n";
        }
        warningNumber=warningNumber*2;
      }
      const variable_list parameters1=parameters_that_occur_in_body(parameters, body);
      const process_identifier p(fresh_name("P"),get_sorts(parameters1));
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
    { if (is_choice(body))
      { return choice(
                  wraptime(choice(body).left(),time,freevars),
                  wraptime(choice(body).right(),time,freevars));
      }

      if (is_sum(body))
      { variable_list sumvars=sum(body).bound_variables();
        process_expression body1=sum(body).operand();
        variable_list renamevars;
        data_expression_list renameterms;
        alphaconvert(sumvars,renamevars,renameterms,freevars,data_expression_list());
        body1=substitute_pCRLproc(renameterms,renamevars,body1);
        const data_expression time1=substitute_data(renameterms,renamevars,time);
        body1=wraptime(body1,time1,sumvars+freevars);
        return sum(sumvars,body1);
      }

      if (is_if_then(body))
      { return if_then(if_then(body).condition(),wraptime(if_then(body).then_case(),time,freevars));
      }

      if (is_seq(body))
      { return seq(wraptime(seq(body).left(),time,freevars),seq(body).right());
      }

      if (is_at(body))
      { /* make a new process */
        const process_identifier newproc=newprocess(freevars,body,pCRL,
                      canterminatebody(body),containstimebody(body));
        return at(process_instance(
                     newproc,
                     objectdata[objectIndex(newproc)].parameters),
                  time);
      }

      if ((is_process_instance(body))||
          (is_sync(body))||
          (is_action(body))||
          (is_tau(body))||
          (is_delta(body)))
      { return at(body,time);
      }

      throw mcrl2::runtime_error("expected pCRL process in wraptime " + pp(body));
      return process_expression();
    }

    typedef enum { alt_state, sum_state, /* cond,*/ seq_state, name_state, multiaction_state } state;

    variable get_fresh_variable(const std::string &s, const sort_expression sort, const int reuse_index=-1)
    { /* If reuse_index is smaller than 0 (-1 is the default value), an unused variable name is returned,
         based on the string s with sort `sort'. If reuse_index is larger or equal to
         0 the reuse_index+1 generated variable is returned. If for a particular reuse_index
         this function is called for the first time, it is guaranteed that the returned
         variable is unique, and not used as variable elsewhere. Upon subsequent calls
         get_fresh_variable will return the same variable for the same s,sort and reuse_triple.
         This feature is added to make it possible to avoid generating too many different variables. */

      std::map < int , atermpp::map < variable,variable > > generated_variables;

      if (reuse_index<0)
      { variable v(fresh_name(s),sort);
        insertvariable(v,true);
        return v;
      }
      else
      { variable table_index_term(s,sort);
        variable old_variable;
        if (generated_variables[reuse_index].count(table_index_term)>0)
        { old_variable=generated_variables[reuse_index][table_index_term];
        }
        else
        { /* A new variable must be generated */
          old_variable=get_fresh_variable(s,sort);
          generated_variables[reuse_index][table_index_term]=old_variable;
        }
        return old_variable;
      }
    }

    variable_list make_pars(const sort_expression_list sortlist)
    { /* this function returns a list of variables,
         corresponding to the sorts in sortlist */

      if (sortlist.empty())
      { return data_expression_list();
      }

      sort_expression sort=sortlist.front();

      return push_front(
                make_pars(pop_front(sortlist)),get_fresh_variable("a",sort));
    }

    process_expression distributeActionOverConditions(
                          const action act,
                          const data_expression condition,
                          const process_expression restterm,
                          const variable_list freevars)
    { if (is_if_then(restterm))
      { /* Here we check whether the process body has the form
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
                                     freevars),
                           distributeActionOverConditions(
                                     act,
                                     lazy::and_(condition,lazy::not_(c)),
                                     delta_at_zero(),
                                     freevars));
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
                                     freevars),
                           distributeActionOverConditions(
                                     act,
                                     lazy::and_(condition,lazy::not_(c)),
                                     if_then_else(restterm).else_case(),
                                     freevars));
           return r;
      }
      const process_expression restterm1=bodytovarheadGNF(restterm,seq_state,freevars,later);
      return if_then(condition,seq(act,restterm1));
    }



    /* the following variables give the indices of the processes that represent tau
         and delta, respectively */


    process_expression bodytovarheadGNF(
                const process_expression body,
                state s,
                const variable_list freevars,
                variableposition v)
    { /* it is assumed that we only receive processes with
         operators alt, seq, sum_state, cond, name, delta, tau, sync, AtTime in it */

      if (is_choice(body))
       {
         if (alt_state>=s)
          {
            const process_expression body1=bodytovarheadGNF(choice(body).left(),alt_state,freevars,first);
            const process_expression body2=bodytovarheadGNF(choice(body).right(),alt_state,freevars,first);
            if (isDeltaAtZero(body1))
            { return body2;
            }
            if (isDeltaAtZero(body2))
            { return body1;
            }
            return choice(body1,body2);
          }
         const process_expression body1=bodytovarheadGNF(body,alt_state,freevars,first);
         const process_identifier newproc=newprocess(freevars,body1,pCRL,
                                        canterminatebody(body1),
                                        containstimebody(body1));
         return process_instance(newproc,objectdata[objectIndex(newproc)].parameters);
       }

      if (is_sum(body))
      {
        if (sum_state>=s)
        {
          variable_list renamevars;
          variable_list sumvars=sum(body).bound_variables();
          process_expression body1=sum(body).operand();

          data_expression_list renameterms;
          alphaconvert(sumvars,renamevars,renameterms,freevars,data_expression_list());
          body1=substitute_pCRLproc(renameterms,renamevars,body1);
          body1=bodytovarheadGNF(body1,sum_state,sumvars+freevars,first);
          /* Due to the optimisation below, suggested by Yaroslav Usenko, bodytovarheadGNF(...,sum_state,...)
             can deliver a process of the form c -> x + !c -> y. In this case, the
             sumvars must be distributed over both summands. */
          if (is_choice(body1))
          { return choice(sum(sumvars,choice(body1).left()),
                          sum(sumvars,choice(body1).right()));
          }
          return sum(sumvars,body1);
        }
        const process_expression body1=bodytovarheadGNF(body,alt_state,freevars,first);
        const process_identifier newproc=newprocess(freevars,body1,pCRL,
                                                    canterminatebody(body1),
                                                    containstimebody(body1));
        return process_instance(newproc,objectdata[objectIndex(newproc)].parameters);
      }

      if (is_if_then(body))
      { const data_expression condition=if_then(body).condition();
        const process_expression body1=if_then(body).then_case();

        if (s<=sum_state)
        {
          return if_then(
                    condition,
                    bodytovarheadGNF(body1,seq_state,freevars,first));
        }
        const process_expression body2=bodytovarheadGNF(body,alt_state,freevars,first);
        const process_identifier newproc=newprocess(freevars,body2,pCRL,
                                                    canterminatebody(body2),
                                                    containstimebody(body2));
        return process_instance(newproc,objectdata[objectIndex(newproc)].parameters);

      }

      if (is_if_then_else(body))
      {
        const data_expression condition=data_expression(if_then_else(body).condition());
        const process_expression body1=if_then_else(body).then_case();
        const process_expression body2=if_then_else(body).else_case();

        if ((isDeltaAtZero(body1))&&(isDeltaAtZero(body2)))
        { return body1;
        }

        if ((s<=sum_state) && ((isDeltaAtZero(body1))||(isDeltaAtZero(body2))))
        { if (isDeltaAtZero(body2))
          { return if_then(
                    condition,
                    bodytovarheadGNF(body1,seq_state,freevars,first));
          }
          /* body1=="Delta@0" */
          {
            return if_then(
                    lazy::not_(condition),
                    bodytovarheadGNF(body2,seq_state,freevars,first));
        } }
        if (alt_state==s) /* body1!=Delta@0 and body2!=Delta@0 */
        { return
            choice(
              if_then(
                    condition,
                    bodytovarheadGNF(body1,seq_state,freevars,first)),
              if_then(
                    lazy::not_(condition),
                    bodytovarheadGNF(body2,seq_state,freevars,first)));
        }
        const process_expression body3=bodytovarheadGNF(body,alt_state,freevars,first);
        const process_identifier newproc=newprocess(freevars,body3,pCRL,
                         canterminatebody(body3),
                         containstimebody(body3));
        return process_instance(newproc,objectdata[objectIndex(newproc)].parameters);

      }

      if (is_seq(body))
      { process_expression body1=seq(body).left();
        process_expression body2=seq(body).right();

        if (s<=seq_state)
        {
          body1=bodytovarheadGNF(body1,name_state,freevars,v);
          if ((is_if_then(body2)) && (s<=sum_state))
          { /* Here we check whether the process body has the form
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

            const data_expression c=data_expression(if_then(body2).condition());

            const process_expression r= choice(
                           distributeActionOverConditions(body1,c,if_then(body2).then_case(),freevars),
                           distributeActionOverConditions(body1,lazy::not_(c),delta_at_zero(),freevars));
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
                           distributeActionOverConditions(body1,c,if_then_else(body2).then_case(),freevars),
                           distributeActionOverConditions(body1,lazy::not_(c),if_then_else(body2).else_case(),freevars));
            return r;
          }
          body2=bodytovarheadGNF(body2,seq_state,freevars,later);
          return seq(body1,body2);
        }
        body1=bodytovarheadGNF(body,alt_state,freevars,first);
        const process_identifier newproc=newprocess(freevars,body1,pCRL,canterminatebody(body1),
                             containstimebody(body));
        return process_instance(newproc,objectdata[objectIndex(newproc)].parameters);
      }

      if (is_action(body))
      { if ((s==multiaction_state)||(v==first))
        {
          return body;
        }

        bool isnew=false;
        long n=addMultiAction(action(body),isnew);

        if (objectdata[n].process_representing_action==process_identifier())
        { /* this action does not yet have a corresponding process, which
             must be constructed. The resulting process is stored in
             the variable process_representing_action in objectdata. Tempvar below is
             needed as objectdata may be realloced as a side effect
             of newprocess */
          const process_identifier tempvar=newprocess(
                                 objectdata[n].parameters,
                                 objectdata[n].processbody,
                                 GNF,1,false);
          objectdata[n].process_representing_action=tempvar;
        }
        return process_instance(objectdata[n].process_representing_action,action(body).arguments());
      }

      if (is_sync(body))
      {
        bool isnew=false;
        const process_expression body1=process::sync(body).left();
        const process_expression body2=process::sync(body).right();
        const action_list ma=linMergeMultiActionListProcess(
                              bodytovarheadGNF(body1,multiaction_state,freevars,v),
                              bodytovarheadGNF(body2,multiaction_state,freevars,v));

        const process_expression mp=action_list_to_process(ma);
        if ((s==multiaction_state)||(v==first))
        { return mp;
        }

        long n=addMultiAction(mp,isnew);

        if (objectdata[n].process_representing_action==process_identifier())
        { /* this action does not yet have a corresponding process, which
             must be constructed. The resulting process is stored in
             the variable process_representing_action in objectdata. Tempvar below is needed
             as objectdata may be realloced as a side effect of newprocess */
          process_identifier tempvar=newprocess(
                                 objectdata[n].parameters,
                                 objectdata[n].processbody,
                                 GNF,1,false);
          objectdata[n].process_representing_action=tempvar;
        }
        return process_instance(process_identifier(objectdata[n].process_representing_action),getarguments(ma));
      }

      if (is_at(body))
      { process_expression body1=bodytovarheadGNF(
                             at(body).operand(),
                             s,
                             freevars,
                             first);
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
        return process_instance(newproc,objectdata[objectIndex(newproc)].parameters);
      }

      if (is_process_instance(body))
      { return body;
      }

      if (is_process_instance_assignment(body))
      { return transform_process_assignment_to_process(body);
      }


      if (is_tau(body))
      { if (v==first)
        {
          return tau();
        }
        return process_instance(tau_process,data_expression_list());
      }

      if (is_delta(body))
      { if (v==first)
           return body;
        return process_instance(delta_process,data_expression_list());
      }

      throw mcrl2::runtime_error("unexpected process format in bodytovarheadGNF " + pp(body) +".");
      return process_expression();
    }

    void procstovarheadGNF(const atermpp::vector < process_identifier> &procs)
    { /* transform the processes in procs into newprocs */
      for(atermpp::vector < process_identifier >::const_iterator i=procs.begin(); i!=procs.end(); ++i)
      { long n=objectIndex(*i);

        // The intermediate variable result is needed here
        // because objectdata can be realloced as a side
        // effect of bodytovarheadGNF.

        const process_expression result=
          bodytovarheadGNF(
                    objectdata[n].processbody,
                    alt_state,
                    objectdata[n].parameters,
                    first);
        objectdata[n].processbody=result;
      }
    }

    /**************** towards real GREIBACH normal form **************/

    typedef enum {terminating,infinite} terminationstatus;

    process_expression putbehind(const process_expression body1, const process_expression body2)
    { if (is_choice(body1))
      { return choice(
                 putbehind(choice(body1).left(),body2),
                 putbehind(choice(body1).right(),body2));
      }

      if (is_seq(body1))
      { return seq(seq(body1).left(), putbehind(seq(body1).right(),body2));
      }

      if (is_if_then(body1))
      { return if_then(if_then(body1).condition(),putbehind(if_then(body1).then_case(),body2));
      }

      if (is_sum(body1))
      { /* we must take care that no variables in body2 are
            inadvertently bound */
        variable_list sumvars=sum(body1).bound_variables();
        variable_list vars;
        data_expression_list terms;
        alphaconvertprocess(sumvars,vars,terms,body2);
        return sum(sumvars,
                   putbehind(substitute_pCRLproc(
                                terms,
                                vars,
                                sum(body1).operand()),
                        body2));
      }

      if (is_action(body1))
      { return seq(body1,body2);
      }

      if (is_sync(body1))
      { return seq(body1,body2);
      }

      if (is_process_instance(body1))
      { return seq(body1,body2);
      }

      if (is_delta(body1))
      { return body1;
      }

      if (is_tau(body1))
      { return seq(body1,body2);
        // throw mcrl2::runtime_error("Expect only multiactions, not a tau.");
      }

      if (is_at(body1))
      { return seq(body1,body2);
      }

      throw mcrl2::runtime_error("Internal error. Unexpected process format in putbehind " + pp(body1) +".");
      return process_expression();
    }

    process_expression distribute_condition(
                        const process_expression body1,
                        const data_expression condition)
    { if (is_choice(body1))
      { return choice(
                   distribute_condition(choice(body1).left(),condition),
                   distribute_condition(choice(body1).right(),condition));
      }

      if (is_seq(body1))
      { return if_then(condition,body1);
      }

      if (is_if_then(body1))
      { return if_then(
                  lazy::and_(if_then(body1).condition(),condition),
                  if_then(body1).then_case());
      }

      if (is_sum(body1))
      { /* we must take care that no variables in condition are
            inadvertently bound */
        variable_list sumvars=sum(body1).bound_variables();
        variable_list vars;
        data_expression_list terms;
        alphaconvert(sumvars,vars,terms,variable_list(),
                           push_front(data_expression_list(),condition));
        return sum(
                 sumvars,
                 distribute_condition(
                     substitute_pCRLproc(terms,vars,sum(body1).operand()),
                     condition));
      }

      if ((is_at(body1))||
          (is_action(body1))||
          (is_sync(body1))||
          (is_process_instance(body1))||
          (is_delta(body1))||
          (is_tau(body1)))
      { return if_then(condition,body1);
      }

      throw mcrl2::runtime_error("Internal error. Unexpected process format in distribute condition " + pp(body1) +".");
      return data_expression();
    }

    process_expression distribute_sum(
                                    const variable_list sumvars,
                                    const process_expression body1)
    { if (is_choice(body1))
      { return choice(
                   distribute_sum(sumvars,choice(body1).left()),
                   distribute_sum(sumvars,choice(body1).right()));
      }

      if (is_seq(body1)||
          is_if_then(body1)||
          is_sync(body1)||
          is_action(body1)||
          is_tau(body1)||
          is_at(body1)||
          is_process_instance(body1)||
          isDeltaAtZero(body1))
      { return sum(sumvars,body1);
      }

      if (is_sum(body1))
      { return sum(
                 sumvars+sum(body1).bound_variables(),
                 sum(body1).operand());
      }

      if (is_delta(body1)||
          is_tau(body1))
      { return body1;
      }

      throw mcrl2::runtime_error("Internal error. Unexpected process format in distribute_sum " + pp(body1) +".");
      return process_expression();
    }

    int match_sequence(
                  const atermpp::vector < process_instance > &s1,
                  const atermpp::vector < process_instance > &s2,
                  const bool regular2)
    { /* s1 and s2 are sequences of typed variables of
         the form Process(ProcVarId("P2",[SortId("Bit"),
         SortId("Bit")]),[OpId("b1",SortId("Bit")),OpId("b1",SortId("Bit"))]).
         This function yields true if the names and types of
         the processes in s1 and s2 match. */

      atermpp::vector < process_instance >::const_iterator i2=s2.begin();
      for(atermpp::vector < process_instance >::const_iterator i1=s1.begin();
                i1!=s1.end(); ++i1,++i2)
      { if (i2==s2.end()) return false;
        if (regular2)
        { if (i1->identifier()!=i2->identifier()) return false;
        }
        else
        { if (*i1!=*i2) return false;
        }
      }
      if (i2!=s2.end()) return false;
      return true;
    }

    bool exists_variable_for_sequence(
                         const atermpp::vector < process_instance > &process_names,
                         process_identifier &result)
    { std::vector < atermpp::vector < process_instance > >::const_iterator rwalker=representedprocesses.begin();
      for(atermpp::vector < process_identifier >::const_iterator walker=seq_varnames.begin();
                    walker!=seq_varnames.end(); ++walker,++rwalker)
      { assert(rwalker!=representedprocesses.end());
        const process_identifier process=*walker;
        if (match_sequence(process_names,*rwalker,options.lin_method==lmRegular2))
        { result=process;
          return true;
        }
      }
      assert(rwalker==representedprocesses.end());
      return false;
    }

    void extract_names(
          const process_expression sequence,
          atermpp::vector < process_instance > &result)
    { if (is_action(sequence)||is_process_instance(sequence))
      { result.push_back(sequence);
        return;
      }

      if (is_seq(sequence))
      { const process_expression first=seq(sequence).left();
        if (is_process_instance(first))
        { result.push_back(first);
          long n=objectIndex(process_instance(first).identifier());
          if (objectdata[n].canterminate)
          { extract_names(seq(sequence).right(),result);
          }
          return;
        }
      }

      throw mcrl2::runtime_error("Internal error. Expected sequence of process names (1) " + pp(sequence) + ".");
    }

    variable_list parscollect(const process_expression oldbody, process_expression &newbody)
    { /* we expect that oldbody is a sequence of process instances */

      if (is_process_instance(oldbody))
      { const process_identifier procId=process_instance(oldbody).identifier();
        const variable_list parameters=objectdata[objectIndex(procId)].parameters;
        newbody=process_instance(procId,parameters);
        return parameters;
      }

      if (is_seq(oldbody))
      { const process_expression first=seq(oldbody).left();
        if (is_process_instance(first))
        { long n=objectIndex(process_instance(first).identifier());
          if (objectdata[n].canterminate)
          { const process_identifier procId=process_instance(first).identifier();
            const variable_list pars=parscollect(seq(oldbody).right(),newbody);
            variable_list pars1, pars2;

            construct_renaming(pars,objectdata[objectIndex(procId)].parameters,pars1,pars2,false);

            newbody=seq(process_instance(procId,pars1),newbody);
            return pars1+pars;
          }
          else
          { const process_identifier procId=process_instance(first).identifier();
            const variable_list parameters=objectdata[objectIndex(procId)].parameters;
            newbody=process_instance(procId,parameters);
            return parameters;
          }
        }
      }

      throw mcrl2::runtime_error("Internal error. Expected a sequence of process names (2) " + pp(oldbody) +".");
      return variable_list();
    }

    data_expression_list argscollect(const process_expression t)
    { if (is_process_instance(t))
      return process_instance(t).actual_parameters();

      if (is_seq(t))
      { const process_instance firstproc=seq(t).left();
        long n=objectIndex(firstproc.identifier());
        if (objectdata[n].canterminate)
        { return firstproc.actual_parameters() + argscollect(seq(t).right());
        }
        return firstproc.actual_parameters();
      }

      throw mcrl2::runtime_error("Internal error. Expected a sequence of process names (3) " + pp(t) +".");
      return data_expression_list();
    }

    process_expression cut_off_unreachable_tail(const process_expression t)
    { if (is_process_instance(t)||is_delta(t)||is_action(t)||is_tau(t)||is_sync(t))
      return t;

      if (is_seq(t))
      { const process_expression firstproc=seq(t).left();
        long n=objectIndex(process_instance(firstproc).identifier());
        if (objectdata[n].canterminate)
        { return seq(firstproc,cut_off_unreachable_tail(seq(t).right()));
        }
        return firstproc;
      }

      throw mcrl2::runtime_error("Internal error. Expected a sequence of process names (4) " + pp(t) +".");
      return process_expression();
    }

    process_expression create_regular_invocation(
             const process_expression sequence1,
             atermpp::vector <process_identifier> &todo,
             const variable_list freevars)
    { process_identifier new_process;
      data_expression_list args;
      process_expression sequence(sequence1);

      /* Sequence consists of a sequence of process references,
         concatenated with the sequential composition operator */
      sequence=cut_off_unreachable_tail(sequence);
      sequence=pCRLrewrite(sequence);
      atermpp::vector < process_instance > process_names;
      extract_names(sequence,process_names);
      assert(!process_names.empty());

      if (process_names.size()==1)
      { /* length of list equals 1 */
        if (is_process_instance(sequence))
        { return sequence;
        }
        if (is_seq(sequence))
        { return seq(sequence).left();
        }
        throw mcrl2::runtime_error("Internal error. Expected a sequence of process names " + pp(sequence) +".");
      }
      /* There is more than one process name in the sequence,
         so, we must replace them by a single name */

      /* We first start out by searching whether
         there is already a variable with a matching sequence
         of variables */
      if (!exists_variable_for_sequence(process_names,new_process))
      { /* There does not exist an appropriate variable,
           so, make it and return its index in n */
        process_expression newbody;
        if (options.lin_method==lmRegular2)
        { variable_list pars=parscollect(sequence,newbody);
          new_process=newprocess(pars,newbody,pCRL,
                          canterminatebody(newbody),
                          containstimebody(newbody));
          representedprocesses.push_back(process_names);
        }
        else
        { new_process=newprocess(freevars,sequence,pCRL,
                            canterminatebody(sequence),containstimebody(sequence));
          representedprocesses.push_back(process_names);
        }
        seq_varnames.push_back(new_process);
        todo.push_back(new_process);
      }
      /* now we must construct arguments */
      if (options.lin_method==lmRegular2)
      { args=argscollect(sequence);
      }
      else
      { args=objectdata[objectIndex(new_process)].parameters;
      }
      return process_instance(new_process,args);
    }

    process_expression to_regular_form(
                        const process_expression t,
                        atermpp::vector <process_identifier>  &todo,
                        const variable_list freevars)
    /* t has the form of the sum, and condition over actions
       each followed by a sequence of variables. We replace
       this variable by a single one, putting the new variable
       on the todo list, to be transformed to regular form also. */
    { if (is_choice(t))
      { const process_expression t1=to_regular_form(choice(t).left(),todo,freevars);
        const process_expression t2=to_regular_form(choice(t).right(),todo,freevars);
        return choice(t1,t2);
      }

      if (is_seq(t))
      { const process_expression firstact=seq(t).left();
        assert(is_at(firstact)||is_tau(firstact)||is_action(firstact)||is_sync(firstact));
        /* the sequence of variables in
                   the second argument must be replaced */
        return seq(firstact,create_regular_invocation(seq(t).right(),todo,freevars));
      }

      if (is_if_then(t))
      { return if_then(if_then(t).condition(),to_regular_form(if_then(t).then_case(),todo,freevars));
      }

      if (is_sum(t))
      { const variable_list sumvars=sum(t).bound_variables();
        return sum(sumvars,
                   to_regular_form(
                        sum(t).operand(),
                        todo,
                        sumvars+freevars));
      }

      if (is_sync(t)||is_action(t)||is_delta(t)||is_tau(t)||is_at(t))
      { return t;
      }

      throw mcrl2::runtime_error("to regular form expects GNF " + pp(t) +".");
      return process_expression();
    }

    process_expression distributeTime(
                        const process_expression body,
                        const data_expression time,
                        const variable_list freevars,
                        data_expression &timecondition)
    { if (is_choice(body))
      { return choice(
                  distributeTime(choice(body).left(),time,freevars,timecondition),
                  distributeTime(choice(body).right(),time,freevars,timecondition));
      }

      if (is_sum(body))
      { variable_list sumvars=sum(body).bound_variables();
        process_expression body1=sum(body).operand();
        variable_list renamevars;
        data_expression_list renameterms;
        alphaconvert(sumvars,renamevars,renameterms,freevars,data_expression_list());
        body1=substitute_pCRLproc(renameterms,renamevars,body1);
        const data_expression time1=substitute_data(renameterms,renamevars,time);
        body1=distributeTime(body1,time1,sumvars+freevars,timecondition);
        return sum(sumvars,body1);
      }

      if (is_if_then(body))
      { data_expression timecondition=sort_bool::true_();
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
      { return seq(
                  distributeTime(seq(body).left(), time,freevars,timecondition),
                  seq(body).right());
      }

      if (is_at(body))
      { /* make a new process */
        timecondition=equal_to(time,data_expression(at(body).time_stamp()));
        return body;
      }

      if ((is_sync(body))||
          (is_action(body))||
          (is_tau(body))||
          (is_delta(body)))
      { return at(body,time);
      }

      throw mcrl2::runtime_error("expected pCRL process in distributeTime " + pp(body) +".");
      return process_expression();
    }

    process_expression procstorealGNFbody(
                const process_expression body,
                variableposition v,
                atermpp::vector <process_identifier> &todo,
                const bool regular,
                processstatustype mode,
                const variable_list freevars)
    /* This process delivers the transformation of body
       to GNF with actions as a head symbol, or it
       delivers NULL if body is not a pCRL process.
       If regular=1, then an attempt is made to obtain a
       GNF where one action is always followed by a
       variable. */
    { if (is_at(body))
      { data_expression timecondition=sort_bool::true_();
        process_expression body1=procstorealGNFbody(
                             at(body).operand(),
                             first,
                             todo,
                             regular,
                             mode,
                             freevars);
        return distributeTime(
                   body1,
                   at(body).time_stamp(),
                   freevars,
                   timecondition);
      }

      if (is_choice(body))
      { const process_expression body1=procstorealGNFbody(choice(body).left(),first,todo,
                         regular,mode,freevars);
        const process_expression body2=procstorealGNFbody(choice(body).right(),first,todo,
                         regular,mode,freevars);
        return choice(body1,body2);
      }

      if (is_seq(body))
      { const process_expression body1=procstorealGNFbody(seq(body).left(),v,
                       todo,regular,mode,freevars);
        const process_expression body2=procstorealGNFbody(seq(body).right(),later,
                       todo,regular,mode,freevars);
        process_expression t3=putbehind(body1,body2);
        if ((regular) && (v==first))
        { /* We must transform t3 to regular form */
          t3=to_regular_form(t3,todo,freevars);
        }
        return t3;
      }

      if (is_if_then(body))
      { const process_expression r=distribute_condition(
                  procstorealGNFbody(if_then(body).then_case(),first,
                            todo,regular,mode,freevars),
                  if_then(body).condition());
        return r;
      }

      if (is_sum(body))
      { const variable_list sumvars=sum(body).bound_variables();
        return distribute_sum(sumvars,
                 procstorealGNFbody(sum(body).operand(),first,
                      todo,regular,mode,sumvars+freevars));
      }

      if (is_action(body))
      { return body;
      }

      if (is_sync(body))
      { return body;
      }

      if (is_process_instance(body))
      {
        process_identifier t=process_instance(body).identifier();

        if (v==later)
        { if ((!regular)||(mode=mCRL))
              todo.push_back(t);
                /* single = in `mode=mCRL' is important, otherwise crash
                   I do not understand the reason for this at this moment
                   JFG (9/5/2000) */
          return body;
        }

        const long n=objectIndex(t);
        if (objectdata[n].processstatus==mCRL)
        { todo.push_back(t);
          return process_expression();
        }
        /* The variable is a pCRL process and v==first, so,
           we must now substitute */
        procstorealGNFrec(t,first,todo,regular);

        process_expression t3=substitute_pCRLproc(
             process_instance(body).actual_parameters(),
             objectdata[n].parameters,
             objectdata[n].processbody);

        if (regular)
        {
          t3=to_regular_form(t3,todo,freevars);
        }

        return t3;
      }

      if (is_delta(body))
      { return body;
      }

      if (is_tau(body))
      { return body;
      }

      if (is_merge(body))
      { procstorealGNFbody(process::merge(body).left(),later,
                         todo,regular,mode,freevars);
        procstorealGNFbody(process::merge(body).right(),later,
                         todo,regular,mode,freevars);
        return process_expression();
      }

      if (is_hide(body))
      { procstorealGNFbody(hide(body).operand(),later,todo,regular,mode,freevars);
        return process_expression();
      }

      if (is_rename(body))
      { procstorealGNFbody(process::rename(body).operand(),later,todo,regular,mode,freevars);
        return process_expression();
      }

      if (is_allow(body))
      { procstorealGNFbody(allow(body).operand(),later,todo,regular,mode,freevars);
        return process_expression();
      }

      if (is_block(body))
      { procstorealGNFbody(block(body).operand(),later,todo,regular,mode,freevars);
        return process_expression();
      }

      if (is_comm(body))
      { procstorealGNFbody(comm(body).operand(),later,todo,regular,mode,freevars);
        return process_expression();
      }

      throw mcrl2::runtime_error("unexpected process format in procstorealGNF " + pp(body) +".");
      return process_expression();
    }


    void procstorealGNFrec(
                    const process_identifier procIdDecl,
                    variableposition v,
                    atermpp::vector <process_identifier> &todo,
                    const bool regular)

    /* Do a depth first search on process variables and substitute
       for the headvariable of a pCRL process, in case it is a process,
       such that we obtain a Greibach Normal Form. All pCRL processes will
       be labelled with GNF to indicate that they are in
       Greibach Normal Form. */

    { long n=objectIndex(procIdDecl);

      if (objectdata[n].processstatus==pCRL)
      {
        objectdata[n].processstatus=GNFbusy;
        const process_expression t=procstorealGNFbody(objectdata[n].processbody,first,
                  todo,regular,pCRL,objectdata[n].parameters);
        if (objectdata[n].processstatus!=GNFbusy)
        { throw mcrl2::runtime_error("there is something wrong with recursion");
        }

        objectdata[n].processbody=t;
        objectdata[n].processstatus=GNF;
        return;
      }

      if (objectdata[n].processstatus==mCRL)
      {
        objectdata[n].processstatus=mCRLbusy;
        const process_expression t=procstorealGNFbody(objectdata[n].processbody,first,todo,
                 regular,mCRL,objectdata[n].parameters);
        /* if t is not equal to NULL,
           the body of this process is itself a processidentifier */

        objectdata[n].processstatus=mCRLdone;
        return;
      }

      if ((objectdata[n].processstatus==GNFbusy) && (v==first))
      { throw mcrl2::runtime_error("unguarded recursion in process " + pp(procIdDecl) +".");
      }

      if ((objectdata[n].processstatus==GNFbusy)||
          (objectdata[n].processstatus==GNF)||
          (objectdata[n].processstatus==mCRLdone)||
          (objectdata[n].processstatus==multiAction))
      {
        return;
      }

      if (objectdata[n].processstatus==mCRLbusy)
      { throw mcrl2::runtime_error("unguarded recursion without pCRL operators");
      }

      throw mcrl2::runtime_error("strange process type: " + str(boost::format("%d") % objectdata[n].processstatus));
    }

    void procstorealGNF(const process_identifier procsIdDecl,
                               const bool regular)
    { atermpp::vector <process_identifier> todo;
      todo.push_back(procsIdDecl);
      for(; !todo.empty() ;)
      { const process_identifier pi=todo.back();
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
                       atermpp::vector <process_identifier> &pCRLprocs)
    { if (is_choice(t))
      { makepCRLprocs(choice(t).left(),pCRLprocs);
        makepCRLprocs(choice(t).right(),pCRLprocs);
        return;
      }

      if (is_seq(t))
      { makepCRLprocs(seq(t).left(),pCRLprocs);
        makepCRLprocs(seq(t).right(),pCRLprocs);
        return;
      }

      if (is_if_then(t))
      { makepCRLprocs(if_then(t).then_case(),pCRLprocs);
        return;
      }

      if (is_sum(t))
      { makepCRLprocs(sum(t).operand(),pCRLprocs);
        return;
      }

      if (is_process_instance(t))
      { process_identifier t1=process_instance(t).identifier(); /* get procId */
        if (std::find(pCRLprocs.begin(),pCRLprocs.end(),t1)==pCRLprocs.end())
        { pCRLprocs.push_back(t1);
          makepCRLprocs(objectdata[objectIndex(t1)].processbody,pCRLprocs);
        }
        return;
      }

      if (is_sync(t)||is_action(t)||is_tau(t)||is_delta(t)||is_at(t))
      { return;
      }

      throw mcrl2::runtime_error("unexpected process format " + pp(t) + " in makepCRLprocs");
    }

    /**************** Collectparameterlist ******************************/

    bool alreadypresent(variable &var,const variable_list vl, const long n)
    { /* Note: variables can be different, although they have the
         same string, due to different types. If they have the
         same string, but different types, the conflict must
         be resolved by renaming the name of the variable */

      if (vl.empty()) return false;
      const variable var1=vl.front();
      assert(var1.is_variable());

      /* The variable with correct type is present: */
      if (var==var1)
      { return true;
      }

      /* Compare whether the string indicating the variable
         name is equal, but the types are different. In that
         case the variable needs to be renamed to a fresh one,
         and is not present in vl. */
      if (var.name()==var1.name())
      {
        variable var2=get_fresh_variable(var.name(),var.sort());
        // templist is needed as objectdata may be realloced
        // during the substitution. Same applies to tempvar
        // below.
        data_expression_list templist=substitute_datalist(push_front(variable_list(),var2),
                                               push_front(variable_list(),var),
                                               objectdata[n].parameters);
        objectdata[n].parameters=variable_list(templist);
        process_expression tempvar=substitute_pCRLproc(push_front(variable_list(),var2),
                                       push_front(variable_list(),var),
                                       objectdata[n].processbody);
        objectdata[n].processbody=tempvar;
        var=var2;
        return false;
      }

      /* otherwise it can be present in vl */
      return alreadypresent(var,pop_front(vl),n);
    }

    variable_list joinparameters(const variable_list par1,
                                        const variable_list par2,
                                        const long n)
    { if (par2.empty()) return par1;

      variable var2=par2.front();
      assert(var2.is_variable());

      if (alreadypresent(var2,par1,n))
      { return joinparameters(par1,pop_front(par2),n);
      }
      return push_front(joinparameters(par1,pop_front(par2),n),var2);
    }

    variable_list collectparameterlist(
                   const atermpp::vector < process_identifier> &pCRLprocs)
    { variable_list parameters;
      for (atermpp::vector < process_identifier>::const_iterator walker=pCRLprocs.begin();
                   walker!=pCRLprocs.end(); ++walker)
        { long n=objectIndex(*walker);
          parameters=joinparameters(parameters,objectdata[n].parameters,n);
        }
      return parameters;
    }

    /****************  Declare local datatypes  ******************************/

    void declare_control_state(
                    const atermpp::vector < process_identifier> &pCRLprocs)
    { create_enumeratedtype(pCRLprocs.size());
    }

    class stackoperations:public boost::noncopyable
    { public:
         variable_list parameter_list;
         sort_expression stacksort;
         sort_expression_list sorts;
         function_symbol_list get;
         function_symbol push;
         function_symbol emptystack;
         function_symbol empty;
         function_symbol pop;
         function_symbol getstate;
         stackoperations *next;

         stackoperations(const variable_list pl,
                         specification_basic_type &spec)
         { parameter_list.protect();
           parameter_list=pl;
           stacksort.protect();
           sorts.protect();
           get.protect();
           push.protect();
           emptystack.protect();
           empty.protect();
           pop.protect();
           getstate.protect();
           next=spec.stack_operations_list;
           spec.stack_operations_list=this;

           //create structured sort
           //  Stack = struct emptystack?is_empty
           //               | push(getstate: Pos, getx1: S1, ..., getxn: Sn, pop: Stack)
           //               ;

           basic_sort stack_sort_alias(spec.fresh_name("Stack"));
           structured_sort_constructor_argument_vector sp_push_arguments;
           for (variable_list::const_iterator l = pl.begin() ; l!=pl.end() ; ++l)
           { sp_push_arguments.push_back(structured_sort_constructor_argument(l->sort(),
                               spec.fresh_name("get" + std::string(l->name()))));
             sorts=push_front(sorts,l->sort());
           }
           sp_push_arguments.push_back(structured_sort_constructor_argument(stack_sort_alias,
                               spec.fresh_name("pop")));
           sorts=reverse(sorts);
           structured_sort_constructor sc_push(spec.fresh_name("push"), sp_push_arguments);
           structured_sort_constructor sc_emptystack(spec.fresh_name("emptystack"),spec.fresh_name("isempty"));

           structured_sort_constructor_vector constructors(1,sc_push);
           constructors.push_back(sc_emptystack);
           //add data declarations for structured sort
           spec.insertalias(alias(stack_sort_alias,structured_sort(constructors)));
           stacksort=spec.data.normalise_sorts(stack_sort_alias);
           push=sc_push.constructor_function(stack_sort_alias);
           emptystack=sc_emptystack.constructor_function(stack_sort_alias);
           empty=sc_emptystack.recogniser_function(stack_sort_alias);
           const atermpp::vector< function_symbol > projection_functions =
                                         sc_push.projection_functions(stack_sort_alias);
           pop=projection_functions.back();
           getstate=projection_functions.front();
           get=function_symbol_list(projection_functions.begin()+1,projection_functions.end()-1);
         }

         ~stackoperations()
         { parameter_list.unprotect();
           stacksort.unprotect();
           sorts.unprotect();
           get.unprotect();
           push.unprotect();
           emptystack.unprotect();
           empty.unprotect();
           pop.unprotect();
           getstate.unprotect();
         }
    };

    class stacklisttype:public boost::noncopyable
    { public:
        stackoperations *opns;
        variable_list parameters;
        variable stackvar;
        int no_of_states;
        /* the boolean state variables occur in reverse
           order, i.e. the least significant first, whereas
           in parameter lists, the order is reversed. */
        variable_list booleanStateVariables;


        /* All datatypes for different stacks that are being generated
           are stored in the following list, such that it can be investigated
           whether a suitable stacktype already exist, before generating a new
        one */


        stackoperations *find_suitable_stack_operations(
                                   const variable_list parameters,
                                   stackoperations *stack_operations_list)
        { if (stack_operations_list==NULL)
          { return NULL;
          }
          if (parameters==stack_operations_list->parameter_list)
          { return stack_operations_list;
          }
          return find_suitable_stack_operations(parameters,stack_operations_list->next);
        }

        /// \brief Constructor
        stacklisttype (const variable_list parlist,
                       specification_basic_type &spec,
                       const bool regular,
                       const atermpp::vector < process_identifier> &pCRLprocs,
                       const bool singlecontrolstate)
        { booleanStateVariables.protect();
          stackvar.protect();
          parameters.protect();
          parameters=parlist;

          no_of_states=pCRLprocs.size();
          process_identifier last=pCRLprocs.back();
          const std::string s3((spec.options.statenames)?std::string(last.name()):std::string("s3"));
          if ((spec.options.binary) && (spec.options.newstate))
          { int i=spec.upperpowerof2(no_of_states);
            for( ; i>0 ; i--)
            { variable name(spec.fresh_name("bst"),sort_bool::bool_());
              spec.insertvariable(name,true);
              booleanStateVariables=push_front(booleanStateVariables,name);
            }
          }

          if (regular)
          { opns=NULL;
            if (spec.options.newstate)
            { if (!spec.options.binary)
              { if (!singlecontrolstate)
                { const unsigned int e=spec.create_enumeratedtype(no_of_states);
                  stackvar=variable(spec.fresh_name(s3), spec.enumeratedtypes[e].sortId);
                }
                else
                { /* Generate a stackvariable that is never used */
                  stackvar=variable(spec.fresh_name("Never_used"), sort_bool::bool_());
                }
              }
              else
              { stackvar=variable(spec.fresh_name(s3),sort_bool::bool_());
              }
            }
            else
            { const basic_sort se_pos = sort_pos::pos();
              //declare sort Pos, if needed
              // insert_numeric_sort_decls(se_pos, spec);
              stackvar=variable(spec.fresh_name(s3), sort_pos::pos());
            }
            spec.insertvariable(stackvar,true);
          }
          else
          {
            if (spec.options.newstate)
            { throw mcrl2::runtime_error("cannot combine stacks with " +
                       (spec.options.binary?std::string("binary"):std::string("an enumerated type")));
            }
            opns=find_suitable_stack_operations(parlist,spec.stack_operations_list);

            if (opns!=NULL)
            { stackvar=variable(spec.fresh_name(s3),opns->stacksort);
              spec.insertvariable(stackvar,true);
            }
            else
            { opns=(stackoperations *) new stackoperations(push_front(parlist,
                                            variable("state",sort_pos::pos())),spec);
              stackvar = variable(spec.fresh_name(s3), opns->stacksort);
              spec.insertvariable(stackvar,true);
            }
          }
        }

        ~stacklisttype()
        { stackvar.unprotect();
          booleanStateVariables.unprotect();
          parameters.unprotect();
        }

    };

    data_expression getvar(const variable var,
                           const stacklisttype &stack)
    { /* first search whether the variable is a free process variable */

      for(atermpp::set <variable>::const_iterator walker=global_variables.begin() ;
                   walker!=global_variables.end() ; ++walker)
      { if (*walker==var)
        { return var;
        }
      }

      /* otherwise find out whether the variable matches a parameter */
      function_symbol_list::const_iterator getmappings=stack.opns->get.begin();
      for(variable_list::const_iterator walker=stack.parameters.begin() ;
                   walker!=stack.parameters.end() ; ++walker,++getmappings)
      { if (*walker==var)
        { return application(*getmappings,stack.stackvar);
        }
        assert(getmappings!=stack.opns->get.end());
      }
      assert(0); /* We cannot end up here, because that means that we
                    are looking for in non-existing variable */
      return var;
    }

    data_expression_list processencoding(
                         int i,
                         const data_expression_list t1,
                         const stacklisttype &stack)
    { data_expression_list t(t1);
      if (!options.newstate)
      { return push_front(t,sort_pos::pos(i));
      }

      i=i-1; /* below we count from 0 instead from 1 as done in the
                first version of the prover */

      if (!options.binary)
      { const unsigned int e=create_enumeratedtype(stack.no_of_states);
        function_symbol_list l=enumeratedtypes[e].elementnames;
        for( ; i>0 ; i--){l=pop_front(l);}
        return push_front(t,data_expression(l.front()));
      }
      /* else a sequence of boolean values needs to be generated,
         representing the value i, when there are l->n elements */
      {
        int k=upperpowerof2(stack.no_of_states);
        for( ; k>0 ; k--)
        { if ((i % 2)==0)
          { t=push_front(t,data_expression(sort_bool::false_()));
            i=i/2;
          }
          else
          { t=push_front(t,data_expression(sort_bool::true_()));
            i=(i-1)/2;
          }
        }
        return t;
      }
    }

    data_expression correctstatecond(
                  const process_identifier procId,
                  const atermpp::vector < process_identifier> &pCRLproc,
                  const stacklisttype &stack,
                  int regular)
    { int i;

      for(i=1 ; pCRLproc[i-1]!=procId ; ++i){}
      /* i is the index of the current process */

      if (!options.newstate)
      { if (regular)
        { return equal_to(stack.stackvar, data_expression(processencoding(i,data_expression_list(),stack).front()));
        }
        return equal_to(
                 application(stack.opns->getstate,stack.stackvar),
                     processencoding(i,data_expression_list(),stack).front());
      }

      if (!options.binary) /* Here a state encoding using enumerated types
                        must be declared */
      { create_enumeratedtype(stack.no_of_states);
        if (regular)
        { return equal_to(stack.stackvar,
                          processencoding(i,data_expression_list(),stack).front());
        }
        return equal_to(
                 application(stack.opns->getstate, stack.stackvar),
                 processencoding(i,data_expression_list(),stack).front());
      }

      /* in this case we must encode the condition using
         boolean variables */

      const variable_list vars=stack.booleanStateVariables;

      i=i-1; /* start counting from 0, instead from 1 */
      data_expression t3(sort_bool::true_());
      for(variable_list::const_iterator v=vars.begin(); v!=vars.end(); ++v)
      { if ((i % 2)==0)
        { t3=lazy::and_(lazy::not_(*v),t3);
          i=i/2;
        }
        else
        { t3=lazy::and_(*v,t3);
          i=(i-1)/2;
        }

      }
      assert(i==0);
      return t3;
    }

    data_expression adapt_term_to_stack(
                     const data_expression t,
                     const stacklisttype &stack,
                     const variable_list vars)
    { if (t.is_function_symbol()) return t;
      if (t.is_variable())
      { if (std::find(vars.begin(),vars.end(),t)!=vars.end())
        { /* t occurs in vars, so, t does not have to be reconstructed
             from the stack */
          return t;
        }
        else return getvar(t,stack);
      }

      if (t.is_application())
      { return application(
                adapt_term_to_stack(application(t).head(),stack,vars),
                adapt_termlist_to_stack(application(t).arguments(),stack,vars));
      }

      assert(0);  // expected a term;
      return data_expression();
    }

    template <typename Container>
    inline
    data_expression_vector adapt_termlist_to_stack(
      const Container& tl,
      const stacklisttype& stack,
      const variable_list& vars)
    {
      data_expression_vector result;
      for(typename Container::const_iterator i = tl.begin(); i != tl.end(); ++i)
      {
        result.push_back(adapt_term_to_stack(*i,stack, vars));
      }
      return result;
    }


/*
    data_expression_list adapt_termlist_to_stack(
                      application::arguments_range tl,
                      const stacklisttype &stack,
                      const variable_list vars)
    { data_expression_vector result;
      // TODO. This is not very efficient.
      for(application::arguments_range::const_iterator i = tl.begin(); i!=tl.end(); ++i)
      { result.push_back(adapt_term_to_stack(*i,stack,vars));
      }
      return convert< data_expression_list >(result);
    }

    data_expression_list adapt_termlist_to_stack(
                      const data_expression_list tl,
                      const stacklisttype &stack,
                      const variable_list vars)
    { data_expression_vector result;
      // TODO. This is not very efficient.
      for(data_expression_list::const_iterator i = tl.begin(); i!=tl.end(); ++i)
      { result.push_back(adapt_term_to_stack(*i,stack,vars));
      }
      return convert< data_expression_list >(result);
    }
*/

    action_list adapt_multiaction_to_stack_rec(
                       const action_list multiAction,
                       const stacklisttype &stack,
                       const variable_list vars)
    { if (multiAction.empty())
      { return multiAction;
      }

      const action act=action(multiAction.front());

      return push_front(
                adapt_multiaction_to_stack_rec(pop_front(multiAction),stack,vars),
                action(act.label(),
                       convert<data_expression_list>(adapt_termlist_to_stack(
                               act.arguments(),
                               stack,
                               vars))));
    }

    action_list adapt_multiaction_to_stack(
                       const action_list multiAction,
                       const stacklisttype &stack,
                       const variable_list vars)
    { return adapt_multiaction_to_stack_rec(multiAction,stack,vars);
    }

    data_expression representative_generator_internal(const sort_expression s)
    { if (!options.noglobalvars)
      { const variable newVariable(fresh_name("dc"),s);
        insertvariable(newVariable,true);
        global_variables.insert(newVariable);
        return newVariable;
      }
      return representative_generator(data)(s);
    }

    data_expression find_(
                   const variable s,
                   const variable_list pars,
                   const data_expression_list args,
                   const stacklisttype &stack,
                   const variable_list vars,
                   bool regular)
    { /* We generate the value for variable s in the list of
         the parameters of the process. If s is equal to some
         variable in pars, it is an argument of the current
         process, and it must be replaced by the corresponding
         argument in args.
           If s does not occur in pars, it must be replaced
         by a dummy value.
      */
      data_expression_list::const_iterator j=args.begin();
      for(variable_list::const_iterator i=pars.begin(); i!=pars.end(); ++i,++j)
      {  assert(j!=args.end());
         if (s==*i)
         { return (regular?*j:adapt_term_to_stack(*j,stack,vars));
         }
      }
      assert(j==args.end());
      const data_expression result=representative_generator_internal(s.sort());
      return (regular?result:adapt_term_to_stack(result,stack,vars));
    }


    data_expression_list findarguments(
                       const variable_list pars,
                       const variable_list parlist,
                       const data_expression_list args,
                       const data_expression_list t2,
                       const stacklisttype &stack,
                       const variable_list vars,
                       bool regular)
    { assert(pars.size()==args.size());
      if (parlist.empty())
      { return t2;
      }
      return push_front(
                findarguments(pars,pop_front(parlist),args,t2,stack,vars,regular),
                find_(parlist.front(),pars,args,stack,vars,regular));
    }


    data_expression_list push(
                  const process_identifier procId,
                  const data_expression_list args,
                  const data_expression_list t2,
                  const stacklisttype &stack,
                  const atermpp::vector < process_identifier > &pCRLprcs,
                  const variable_list vars,
                  bool regular,
                  bool singlestate)
    { data_expression_list t=findarguments(objectdata[objectIndex(procId)].parameters,
                stack.parameters,args,t2,stack,vars,regular);

      int i;
      for(i=1 ; pCRLprcs[i-1]!=procId ; ++i){}

      if (regular)
      { if (singlestate)
        { return t;
        }
        return processencoding(i,t,stack);
      }

      return push_front(
                data_expression_list(),data_expression(application(
                            stack.opns->push,
                            processencoding(i,t,stack))));
    }


    data_expression_list make_procargs(
                        const process_expression t,
                        const stacklisttype &stack,
                        const atermpp::vector < process_identifier > &pcrlprcs,
                        const variable_list vars,
                        const bool regular,
                        const bool singlestate)
    { /* t is a sequential composition of process variables */

      if (is_seq(t))
      { if (regular)
        { throw mcrl2::runtime_error("process is not regular, as it has stacking vars " + pp(t) +".");
        }
        const process_instance process=seq(t).left();
        const process_expression t2=seq(t).right();
        const process_identifier procId=process.identifier();
        const data_expression_list t1=process.actual_parameters();

        if (objectdata[objectIndex(procId)].canterminate)
        { data_expression_list t3=make_procargs(t2,stack,pcrlprcs,
                            vars,regular,singlestate);
          t3=push(procId,t1,t3,stack,pcrlprcs,vars,regular,singlestate);
          return push_front(data_expression_list(),t3.front());
        }

        const data_expression_list t3=push(procId,t1,push_front(function_symbol_list(),stack.opns->emptystack),
                     stack,pcrlprcs,vars,regular,singlestate);
        return push_front(data_expression_list(),t3.front());
      }

      if (is_process_instance(t))
      {
        const process_identifier procId=process_instance(t).identifier();
        const data_expression_list t1=process_instance(t).actual_parameters();

        if (regular)
        { return push(procId,
                      t1,
                      function_symbol_list(),
                      stack,
                      pcrlprcs,
                      vars,
                      regular,
                      singlestate);
        }
        if (objectdata[objectIndex(procId)].canterminate)
        { const data_expression_list t3=push(procId,
                  t1,
                  push_front(data_expression_list(),
                            data_expression(application(stack.opns->pop,stack.stackvar))),
                  stack,
                  pcrlprcs,
                  vars,
                  regular,
                  singlestate);
          return push_front(data_expression_list(),t3.front());
        }
        const data_expression_list t3= push(procId,
                 t1,
                 push_front(function_symbol_list(),stack.opns->emptystack),
                 stack,
                 pcrlprcs,
                 vars,
                 regular,
                 singlestate);
        return push_front(data_expression_list(),t3.front());
      }

      throw mcrl2::runtime_error("expected seq or name " + pp(t) +".");
      return data_expression_list();
    }

    bool occursin(const variable name,
                        const variable_list pars)
    { assert(is_variable(name));
      for(variable_list::const_iterator l=pars.begin() ; l!=pars.end(); ++l)
      { if (name.name()==l->name())
        { return true;
        }
      }
      return false;
    }


    data_expression_list pushdummyrec(
                         const variable_list totalpars,
                         const variable_list pars,
                         const stacklisttype &stack,
                         int regular)
    { /* totalpars is the total list of parameters of the
         aggregated pCRL process. The variable pars contains
         the list of all variables occuring in the initial
         process. */

      if (totalpars.empty())
      { if (regular)
        { return data_expression_list();
        }
        return push_front(data_expression_list(),data_expression(stack.opns->emptystack));
      }

      const variable par=totalpars.front();
      if (std::find(pars.begin(),pars.end(),par)!=pars.end())
      { return push_front(
                   pushdummyrec(pop_front(totalpars),pars,stack,regular),
                   data_expression(par));
      }
      /* otherwise the value of this argument is irrelevant, so
         make it Nil, if a regular translation is made. If a translation
         with stacks is made, then yield a default `unique' term. */
      return push_front(
                 pushdummyrec(pop_front(totalpars),pars,stack,regular),
                              representative_generator_internal(par.sort()));
    }

    data_expression_list pushdummy(
                         const variable_list parameters,
                         const stacklisttype &stack,
                         int regular)
    {
      return pushdummyrec(stack.parameters,
                  parameters,stack,regular);
    }

    assignment_list make_initialstate(
                         const process_identifier initialProcId,
                         const stacklisttype &stack,
                         const atermpp::vector < process_identifier > &pcrlprcs,
                         int regular,
                         int singlecontrolstate,
                         const variable_list parameters)
    { int i;
      for(i=1 ; pcrlprcs[i-1]!=initialProcId ; ++i){};
      /* i is the index of the initial state */

      data_expression_list result=
            pushdummy(objectdata[objectIndex(initialProcId)].parameters,stack,regular);

      if (regular)
      { if (!singlecontrolstate)
        { result= processencoding(i,result,stack);
        }
      }
      else result=push_front(data_expression_list(),
                       data_expression(application(stack.opns->push,
                               processencoding(i,result,stack))));
      return make_assignment_list(parameters,result);
    }

    /*************************  Routines for summands  **************************/

    variable_list dummyparameterlist(const stacklisttype &stack,
                                     const bool singlestate)
    { if (singlestate)
      { return stack.parameters;
      }

      return processencoding(1,stack.parameters,stack); /* Take 1 as dummy indicator */
      /* return push_front(stack.parameterlist,stack.stackvar); Erroneous, repaired 5/3 */
    }


    summand_list insert_summand(
                        const summand_list sumlist,
                        const variable_list parameters,
                        const variable_list sumvars,
                        const data_expression condition,
                        const action_list multiAction,
                        const data_expression actTime,
                        const data_expression_list procargs,
                        const bool has_time,
                        const bool is_delta_summand)
    { /* insert a new summand in sumlist; first try whether there is already
         a similar summand, such that this summand can be added with minimal
         increase of size. Otherwise add a fully new summand. Note that by
         a more careful matching, the number of summands can be reduced.

         Note also that a terminated term is indicated by taking procargs
         equal to NULL. */

      if (condition!=sort_bool::false_())
      { return push_front(sumlist,
               summand_(sumvars,condition,is_delta_summand,
               multiAction,has_time,actTime,
                 (is_delta_summand?assignment_list():
                            make_assignment_list(parameters,procargs))));
      }
      else
      return sumlist;
    }

    void add_summands(
                   const process_identifier procId,
                   summand_list &sumlist,
                   process_expression summandterm,
                   const atermpp::vector < process_identifier> &pCRLprocs,
                   const variable_list parameters,
                   const stacklisttype &stack,
                   const bool canterminate,
                   const bool regular,
                   const bool singlestate)
    { data_expression atTime;
      action_list multiAction;
      data_expression_list procargs;
      bool is_delta_summand=false;
      bool has_time=false;

      if (isDeltaAtZero(summandterm))
      { // delta@0 does not need to be added.
        return;
      }

      /* remove the sum operators; collect the sum variables in the
         list sumvars */

      variable_list sumvars;
      for( ; is_sum(summandterm) ; )
      { sumvars=sum(summandterm).bound_variables() + sumvars;
        summandterm=sum(summandterm).operand();
      }

      /* translate the condition */

      data_expression condition1;
      if ((regular)&&(singlestate))
      { condition1=sort_bool::true_();
      }
      else
      { condition1=correctstatecond(procId,pCRLprocs,stack,regular);
      }

      for( ; (is_if_then(summandterm)) ; )
      { const data_expression localcondition=data_expression(if_then(summandterm).condition());
        if (!((regular)&&(singlestate)))
        { condition1=lazy::and_(
                         condition1,
                         ((regular)?localcondition:
                                    adapt_term_to_stack(
                                           localcondition,
                                           stack,
                                           sumvars)));
        }
        else
        { /* regular and singlestate */
          condition1=lazy::and_(localcondition,condition1);
        }
        summandterm=if_then(summandterm).then_case();
      }

      if (is_seq(summandterm))
      { /* only one summand is needed */
        process_expression t1=seq(summandterm).left();
        process_expression t2=seq(summandterm).right();
        if (is_at(t1))
        { has_time=true;
          atTime=at(t1).time_stamp();
          t1=at(t1).operand();
        }

        if (t1==delta())
        { is_delta_summand=true;
        }
        else
        { assert(is_tau(t1)||is_action(t1)||is_sync(t1));
          multiAction=to_action_list(t1);
        }

        procargs=make_procargs(t2,stack,
                    pCRLprocs,sumvars,regular,singlestate);
        if (!regular)
        { if (!is_delta_summand)
          { multiAction=adapt_multiaction_to_stack(
                          multiAction,stack,sumvars);
          }
          if (has_time)
          { atTime=adapt_term_to_stack(
                          atTime,stack,sumvars);
          }
        }
        sumlist=insert_summand(sumlist,parameters,
                       sumvars,RewriteTerm(condition1),multiAction,
                       atTime,procargs,has_time,is_delta_summand);
        return;
      }

      /* There is a single initial multiaction or deadlock, possibly timed*/

      if (is_at(summandterm))
      { atTime=at(summandterm).time_stamp();
        summandterm=at(summandterm).operand();
        has_time=true;
      }
      else
      { // do nothing
      }

      if (is_delta(summandterm))
      { is_delta_summand=true;
      }
      else if (is_tau(summandterm))
      { // multiAction is already empty.
      }
      else if (is_action(summandterm))
      { multiAction=push_front(multiAction,action(summandterm));
      }
      else if (is_sync(summandterm))
      { multiAction=to_action_list(summandterm);
      }
      else
      { throw mcrl2::runtime_error("expected multiaction " + pp(summandterm) +".");
      }

      if (regular)
      { if (!is_delta_summand)
        /* As termination has been replaced by an explicit action terminated, followed
         * by delta, a single terminating action cannot exist for regular processes. */
        { throw mcrl2::runtime_error("terminating processes should not exist when using the regular flag");
        }
        sumlist=insert_summand(sumlist,parameters,
                       sumvars,
                       RewriteTerm(condition1),
                       multiAction,
                       atTime,
                       dummyparameterlist(stack,(bool) singlestate),
                       has_time,
                       is_delta_summand);
        return;
      }

      /* in this case we have two possibilities: the process
         can or cannot terminate after the action. So, we must
         generate two conditions. For regular processes, we assume
         that processes do not terminate */
      /* first we generate the non terminating summands */


      data_expression emptypops;
      data_expression condition2;
      if (canterminate)
      { emptypops=application(stack.opns->empty,
                       application(stack.opns->pop,stack.stackvar));
        const data_expression notemptypops=lazy::not_(emptypops);
        condition2=lazy::and_(notemptypops,condition1);
      }
      else condition2=condition1;

      multiAction=adapt_multiaction_to_stack(multiAction,stack,sumvars);
      procargs=push_front(data_expression_list(),data_expression(application(stack.opns->pop,stack.stackvar)));

      sumlist=insert_summand(sumlist,parameters,
                        sumvars,
                        RewriteTerm(condition2),
                        multiAction,
                        atTime,
                        procargs,
                        has_time,
                        is_delta_summand);

      if (canterminate)
      { condition2=lazy::and_(emptypops,condition1);
        sumlist=insert_summand(sumlist,parameters,
                      sumvars,
                      RewriteTerm(condition2),
                      multiAction,
                      atTime,
                      assignment_list(),
                      has_time,
                      is_delta_summand);
      }

      return;
    }


    void collectsumlistterm(
                     const process_identifier procId,
                     summand_list &sumlist,
                     const process_expression body,
                     const variable_list pars,
                     const stacklisttype &stack,
                     const bool canterminate,
                     const bool regular,
                     const bool singlestate,
                     const atermpp::vector < process_identifier> &pCRLprocs)
    {
      if (is_choice(body))
      { const process_expression t1=choice(body).left();
        const process_expression t2=choice(body).right();

        collectsumlistterm(procId,sumlist,t1,pars,stack,
                     canterminate,regular,singlestate,pCRLprocs);
        collectsumlistterm(procId,sumlist,t2,pars,stack,
                     canterminate,regular,singlestate,pCRLprocs);
        return;
      }
      else
      { add_summands(procId,sumlist,body,pCRLprocs,pars,stack,
                     canterminate,regular,singlestate);
      }
    }

    summand_list collectsumlist(
                     const atermpp::vector < process_identifier> &pCRLprocs,
                     const variable_list pars,
                     const stacklisttype &stack,
                     bool canterminate,
                     bool regular,
                     bool singlestate)
    { summand_list sumlist;
      for(atermpp::vector < process_identifier>::const_iterator walker=pCRLprocs.begin();
                  walker!=pCRLprocs.end(); ++walker)
      { const process_identifier procId= *walker;
        collectsumlistterm(
                  procId,
                  sumlist,
                  objectdata[objectIndex(procId)].processbody,
                  pars,
                  stack,
                  (canterminate&&objectdata[objectIndex(procId)].canterminate),
                  regular,
                  singlestate,
                  pCRLprocs);
      }

      return sumlist;
    }


    /**************** Enumtype and enumeratedtype **********************************/

    class enumeratedtype
    { public:
        unsigned int size;
        sort_expression sortId;
        data_expression_list elementnames;
        function_symbol_list functions;

        enumeratedtype(const unsigned int n,
                       specification_basic_type &spec)
        { size=n;
          sortId.protect();
          elementnames.protect();
          functions.protect();
          if (n==2)
          { sortId = sort_bool::bool_();
            elementnames = push_front(push_front(data_expression_list(),
                                data_expression(sort_bool::true_())),data_expression(sort_bool::false_()));
          }
          else
          { //create new sort identifier
            basic_sort sort_id(spec.fresh_name(str(boost::format("Enum%d") % n)));
            sortId=sort_id;
            //create structured sort
            //  Enumi = struct en_i | ... | e0_i;
            structured_sort_constructor_list struct_conss;
            for(unsigned int j=0 ; (j<n) ; j++)
            { //create constructor declaration of the structured sort
              const identifier_string s=spec.fresh_name(str(boost::format("e%d_%d") % j % n));
              const structured_sort_constructor struct_cons(s,"");

              struct_conss = push_front(struct_conss, struct_cons);
            }
            structured_sort sort_struct(struct_conss);

            //add declaration of standard functions
            spec.insertalias(alias(sort_id, sort_struct));

            //store new declarations in return value w
            sortId = sort_id;
            elementnames = data::convert< data::function_symbol_list >(sort_struct.constructor_functions());
          }
        }

        enumeratedtype(const enumeratedtype &e)
        { sortId.protect();
          elementnames.protect();
          functions.protect();
          size=e.size;
          sortId=e.sortId;
          elementnames=e.elementnames;
          functions=e.functions;
        }

        void operator=(const enumeratedtype &e)
        { sortId.protect();
          elementnames.protect();
          functions.protect();
          size=e.size;
          sortId=e.sortId;
          elementnames=e.elementnames;
          functions=e.functions;
        }

        ~enumeratedtype()
        { sortId.unprotect();
          elementnames.unprotect();
          functions.unprotect();
        }
    };

    unsigned int create_enumeratedtype(const unsigned int n)
    { unsigned int w;

      for(w=0; ((w<enumeratedtypes.size())&&(enumeratedtypes[w].size!=n)); ++w){};

      if (w==enumeratedtypes.size()) // There is no enumeratedtype of arity n.
      {
        enumeratedtypes.push_back(enumeratedtype(n,*this));
      }
      return w;
    }

    function_symbol find_case_function(unsigned int index, const sort_expression sort)
    {
      // std::cerr << "Use case function " << index << " Sort: " << pp(sort) << "\n";
      const function_symbol_list functions=enumeratedtypes[index].functions;
      for(function_symbol_list::const_iterator w=functions.begin();
                  w!=functions.end(); ++w)
      {
        function_sort::domain_const_range domain = function_sort(w->sort()).domain();
        assert(domain.size() >= 2);
        if (*(++domain.begin())==sort)
        { return *w;
        }
      };

      throw mcrl2::runtime_error("searching for nonexisting case function on sort " + pp(sort) +".");
      return function_symbol();
    }

    void define_equations_for_case_function(
                    const unsigned int index,
                    const function_symbol functionname,
                    const sort_expression sort)
    { variable_list vars;
      data_expression_list args;
      data_expression_list xxxterm;

      // std::cerr << "Define case function " << index << "Sort: " << pp(sort) << "\n";

      const variable v1=get_fresh_variable("x",sort);
      const unsigned int n=enumeratedtypes[index].size;
      for(unsigned int j=0; (j<n); j++)
      { const variable v=get_fresh_variable("y",sort);
        vars=push_front(vars,v);
        args=push_front(args,data_expression(v));
        xxxterm=push_front(xxxterm,data_expression(v1));
      }

       /* I generate here an equation of the form
          C(e,x,x,x,...x)=x for a variable x. */
      const sort_expression s=enumeratedtypes[index].sortId;
      const variable v=get_fresh_variable("e",s);
      insert_equation(
                data_equation(
                  push_front(push_front(variable_list(),v),v1),
                  application(functionname,push_front(xxxterm,data_expression(v))),
                  data_expression(v1)));

      variable_list auxvars=vars;

      const data_expression_list elementnames=enumeratedtypes[index].elementnames;
      for(data_expression_list::const_iterator w=elementnames.begin();
           w!=elementnames.end() ; ++w)
      { insert_equation(data_equation(
               vars,
               application(functionname,push_front(args,*w)),
               auxvars.front()));

        auxvars=pop_front(auxvars);
      }
    }

    void create_case_function_on_enumeratedtype(
                       const sort_expression sort,
                       const unsigned int enumeratedtype_index)
    { assert(enumeratedtype_index<enumeratedtypes.size());
      /* first find out whether the function exists already, in which
         case nothing needs to be done */

      const function_symbol_list functions=enumeratedtypes[enumeratedtype_index].functions;
      for(function_symbol_list::const_iterator w=functions.begin();
                    w!=functions.end(); ++w)
      { const sort_expression w1sort=w->sort();
        assert(function_sort(w1sort).domain().size()>1);
        // Check that the second sort of the case function equals sort
        if (*(++(function_sort(w1sort).domain().begin()))==sort)
        { return; // The case function does already exist
        }
      };

      /* The function does not exist;
         Create a new function of enumeratedtype e, on sort */

      if (enumeratedtypes[enumeratedtype_index].sortId==sort_bool::bool_())
      { /* take the if function on sort 'sort' */
        const function_symbol_list f=enumeratedtypes[enumeratedtype_index].functions;
        enumeratedtypes[enumeratedtype_index].functions=push_front(f,if_(sort));
        return;
      }
      // else
      sort_expression_list newsortlist;
      unsigned int n=enumeratedtypes[enumeratedtype_index].size;
      for(unsigned int j=0; j<n ; j++)
      { newsortlist=push_front(newsortlist, sort);
      }
      sort_expression sid=enumeratedtypes[enumeratedtype_index].sortId;
      newsortlist=push_front(newsortlist, sid);

      const function_sort newsort(newsortlist,sort);
      const function_symbol casefunction(
                          fresh_name(str(boost::format("C%d_%s") % n % (
                                         !newsort.is_basic_sort()?"":std::string(basic_sort(sort).name())))),
                          newsort);
      // insertmapping(casefunction,true);
      data.add_mapping(casefunction);
      const function_symbol_list f=enumeratedtypes[enumeratedtype_index].functions;
      enumeratedtypes[enumeratedtype_index].functions=push_front(f,casefunction);

      define_equations_for_case_function(enumeratedtype_index,casefunction,sort);
      return;
    }

    class enumtype : public boost::noncopyable
    { public:
        unsigned int enumeratedtype_index;
        variable var;

        enumtype(int n,
                 const sort_expression_list fsorts,
                 const sort_expression_list gsorts,
                 specification_basic_type &spec)
        { var.protect();

          enumeratedtype_index=spec.create_enumeratedtype(n);

          var=variable(spec.fresh_name("e"),spec.enumeratedtypes[enumeratedtype_index].sortId);
          spec.insertvariable(var,true);

          for(sort_expression_list::const_iterator w=fsorts.begin(); w!=fsorts.end(); ++w)
          { spec.create_case_function_on_enumeratedtype(*w,enumeratedtype_index);
          }

          for(sort_expression_list::const_iterator w=gsorts.begin(); w!=gsorts.end(); ++w)
          { spec.create_case_function_on_enumeratedtype(*w,enumeratedtype_index);
          }

          spec.create_case_function_on_enumeratedtype(sort_bool::bool_(),enumeratedtype_index);

          if (spec.timeIsBeingUsed)
          { spec.create_case_function_on_enumeratedtype(sort_real::real_(),enumeratedtype_index);
          }
        }

        /* enumtype(const enumtype &t)
        { var=t.var;
          var.protect();
          enumeratedtype_index=t.enumeratedtype_index;
        } */

        ~enumtype()
        { var.unprotect();
        }
    };
    /************** Merge summands using enumerated type ***********************/


    bool mergeoccursin(
                   variable &var,
                   const variable_list v,
                   variable_list &matchinglist,
                   variable_list &pars,
                   data_expression_list &args)
    { variable_list auxmatchinglist;

      bool result=false;

      /* First find out whether var:sort can be matched on a
         term in the matching list */

      /* first find out whether the variable occurs in the matching
         list, so, they can be joined */

      for(variable_list::const_iterator i=matchinglist.begin();
                   i!=matchinglist.end(); ++i)
      { variable var1=*i;
        if (var.sort()==var1.sort())
        { /* sorts match, so, we join the variables */
          result=true;
          if (var!=var1)
          { pars=push_front(pars,var);
            args=push_front(args,data_expression(var1));
          }
          /* copy remainder of matching list */
          for(++i ; i!=matchinglist.end(); ++i)
          {
            auxmatchinglist= push_front(auxmatchinglist,*i);
          }
          break;
        }
        else
        {
          auxmatchinglist=push_front(auxmatchinglist,var1);
        }
      }

      /* turn auxmatchinglist back in normal order, and put result
         in *matchinglist */

      matchinglist=reverse(auxmatchinglist);

      if (!result)
       { /* in this case no matching argument has been found.
         So, we must find out whether *var is an allowed variable, not
         occuring in the variablelist v.
         But if so, we must replace it by a new one. */
         for(variable_list::const_iterator i=v.begin() ; i!=v.end() ; ++i)
         {
           variable var1=*i;
           if (var.name()==var1.name())
           { pars=push_front(pars,var);
             var=get_fresh_variable(var.name(),var.sort());
             args=push_front(args,data_expression(var));
             break;
           }
         }
       }

      return result;
    }

    data_expression_list extend(const data_expression c, const data_expression_list cl)
    { if (cl.empty())
      { return cl;
      }

      return push_front(extend(c,pop_front(cl)),
                        data_expression(lazy::and_(c,cl.front())));
    }

    data_expression_list extend_conditions(
                         const variable var,
                         const data_expression_list conditionlist)
    { const data_expression unique=representative_generator_internal(var.sort());
      const data_expression newcondition=equal_to(var,unique);
      return extend(newcondition,conditionlist);
    }


    data_expression transform_matching_list(const variable_list matchinglist)
    { if (matchinglist.empty())
         return sort_bool::true_();

      const variable var=matchinglist.front();
      data_expression unique=representative_generator_internal(var.sort());
      return lazy::and_(
                   transform_matching_list(pop_front(matchinglist)),
                   equal_to(data_expression(var),unique));
    }


    data_expression_list addcondition(
                         const variable_list matchinglist,
                         const data_expression_list conditionlist)
    { return push_front(conditionlist,
                       transform_matching_list(matchinglist));
    }

    /// \brief Join the variables in v1 with those in v2, but do not add duplicates.
    /// \details As it stands renamings has no function.

    variable_list merge_var_simple(
                        const variable_list v1,
                        const variable_list v2)
    { const variable_list v1h=reverse(v1);
      variable_list result=v2;
      for(variable_list::const_iterator i1=v1h.begin() ; i1!=v1h.end() ; ++i1)
      {
        bool found=false;
        for(variable_list::const_iterator i2=v2.begin() ; i2!=v2.end() ; ++i2)
        {
          if (*i1==*i2)
          { // Check whether the names are the same, but the types differ
            found=true;
            break;
          }
          else
          { if (i1->name()==i2->name())
            { assert(0); // We need to deal with this later. This can
                         // only occur when variables with the same string
                         // and different types are used.
            }
          }
        }
        if (!found)
        { result=push_front(result,*i1);
        }
      }
      return result;
    }


    variable_list merge_var(
                        const variable_list v1,
                        const variable_list v2,
                        atermpp::vector < variable_list> &renamings_pars,
                        atermpp::vector < data_expression_list> &renamings_args,
                        data_expression_list &conditionlist)
    { data_expression_list renamingargs;
      variable_list renamingpars;
      variable_list matchinglist=v2;

      /* If the sequence of sum variables is reversed,
       * the variables are merged in the same sequence for all
       * summands (due to a suggestion of Muck van Weerdenburg) */

      variable_list v1h=(v2.empty()?reverse(v1):v1);

      variable_list result=v2;
      for(variable_list::const_iterator i1=v1h.begin(); i1!=v1h.end() ; ++i1)
      { variable v=*i1;
        if (!mergeoccursin(v,v2,
                matchinglist,renamingpars,renamingargs))
        { result=push_front(result,v);
          conditionlist=extend_conditions(v,conditionlist);
        }
      }
      conditionlist=addcondition(matchinglist,conditionlist);
      renamings_pars.push_back(renamingpars);
      renamings_args.push_back(renamingargs);
      return result;
    }

    variable_list make_binary_sums(
                          int n,
                          const sort_expression enumtypename,
                          data_expression &condition,
                          const variable_list tail)
    { variable_list result;
      assert(n>1);
      condition=sort_bool::true_();

      n=n-1;
      for(result=tail ; (n>0) ; n=n/2)
      { variable sumvar=get_fresh_variable("e",enumtypename);
        result=push_front(result,sumvar);
        if ((n % 2)==0)
        { condition=lazy::and_(sumvar,condition);
        }
        else
        { condition=lazy::or_(sumvar,condition);
        }
      }
      return result;
    }

    data_expression construct_binary_case_tree_rec(
                           int n,
                           const variable_list sums,
                           data_expression_list &terms,
                           const sort_expression termsort,
                           const enumtype &e)
    { assert(!terms.empty());

      if (n<=0)
      { assert(!terms.empty());
        const data_expression t=terms.front();
        terms=pop_front(terms);
        return t;
      }

      assert(!sums.empty());
      variable casevar=sums.front();

      const data_expression t=construct_binary_case_tree_rec(n / 2,pop_front(sums),terms,termsort,e);

      if (terms.empty())
      { return t; }

      const data_expression t1=construct_binary_case_tree_rec(n / 2,pop_front(sums),terms,termsort,e);

      if (t==t1)
      { return t;
      }
      return application(find_case_function(e.enumeratedtype_index, termsort), casevar, t, t1);
    }

    data_expression construct_binary_case_tree(
                            int n,
                            const variable_list sums,
                            data_expression_list terms,
                            const sort_expression termsort,
                            const enumtype &e)
    {
      return construct_binary_case_tree_rec(n-1,sums,terms,termsort,e);
    }

    bool summandsCanBeClustered(
                  const summand summand1,
                  const summand summand2)
    { if (summand1.has_time()!= summand2.has_time())
      return false;

      if (summand1.is_delta())
      { if (summand2.is_delta())
        { return true;
        }
        else return false;
      }

      if (summand2.is_delta())
      { return false;
      }

      /* Here the multiactions are proper multi actions,
         both with or without a time indication */

      /* The actions can be clustered if they contain
         the same actions, with the same sorts for the
         actions. We assume that the multiactions are
         ordered.
      */

      const action_list multiactionlist1=summand1.actions();
      const action_list multiactionlist2=summand2.actions();
      action_list::const_iterator i2=multiactionlist2.begin();
      for(action_list::const_iterator i1=multiactionlist1.begin(); i1!=multiactionlist1.end(); ++i1,++i2)
      { if (i2==multiactionlist2.end())
        { return false;
        }
        if (i1->label()!=i2->label())
        { return false;
        }
      }
      if (i2!=multiactionlist2.end())
      { return false;
      }
      return true;
    }

    data_expression getRHSassignment(const variable var, const assignment_list as)
    { for(assignment_list::const_iterator i=as.begin(); i!=as.end(); ++i)
      { if (i->lhs()==var)
        { return i->rhs();
        }
      }
      return data_expression(var);
    }

    summand collect_sum_arg_arg_cond(
                       const enumtype &e,
                       int n,
                       const summand_list sumlist,
                       const variable_list gsorts)
    { /* This function gets a list of summands, with
         the same multiaction and time
         status. It yields a single clustered summand
         by introducing an auxiliary sum operator, with
         a variable of enumtype. In case binary is used,
         a sequence of variables are introduced of sort Bool */

      variable_list resultsum;
      data_expression resultcondition;
      action_list resultmultiaction;
      data_expression resulttime;

      atermpp::vector < variable_list > rename_list_pars;
      atermpp::vector < data_expression_list > rename_list_args;
      /* rename list is a list of pairs of variable and term lists */
      data_expression_list conditionlist;
      data_expression binarysumcondition;
      int equaluptillnow=1;

      for(summand_list::const_iterator walker=sumlist.begin(); walker!=sumlist.end() ; ++walker)
      {
        const variable_list sumvars=walker->summation_variables();
        resultsum=merge_var(sumvars,resultsum,rename_list_pars,rename_list_args,conditionlist);
      }

      if (options.binary)
      { resultsum=make_binary_sums(
                         n,
                         enumeratedtypes[e.enumeratedtype_index].sortId,
                         binarysumcondition,
                         resultsum);
      }
      else
      { resultsum=push_front(resultsum,e.var);
      }

      /* we construct the resulting condition */
      data_expression_list auxresult;
      atermpp::vector < variable_list >  ::const_iterator auxrename_list_pars=rename_list_pars.begin();
      atermpp::vector < data_expression_list >::const_iterator auxrename_list_args=rename_list_args.begin();

      data_expression equalterm;
      equaluptillnow=1;
      for(summand_list::const_iterator walker=sumlist.begin(); walker!=sumlist.end(); ++walker)
      { const summand smmnd=*walker;
        const data_expression condition=smmnd.condition();

        assert(auxrename_list_pars!=rename_list_pars.end());
        assert(auxrename_list_args!=rename_list_args.end());
        const variable_list auxpars= *auxrename_list_pars;
        ++auxrename_list_pars;
        const data_expression_list auxargs= *auxrename_list_args;
        ++auxrename_list_args;
        const data_expression auxresult1=substitute_data(auxargs,auxpars,condition);
        if (equalterm==data_expression())
        { equalterm=auxresult1;
        }
        else
        { if (equaluptillnow)
          { equaluptillnow=(auxresult1==equalterm);
          }
        }
        auxresult=push_front(auxresult,auxresult1);
      }
      if (options.binary)
      { resultcondition=construct_binary_case_tree(n,
                    resultsum,auxresult,sort_bool::bool_(),e);
        resultcondition=lazy::and_(binarysumcondition,resultcondition);
        resultcondition=lazy::and_(
                       construct_binary_case_tree(n,
                           resultsum,conditionlist,sort_bool::bool_(),e),
                       resultcondition);
      }
      else
      { if (equaluptillnow)
        { resultcondition=lazy::and_(
                      application(
                              find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                              push_front(conditionlist,data_expression(e.var))),
                      equalterm);
        }
        else
        { resultcondition=application(
                         find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                         push_front(auxresult,data_expression(e.var)));
          resultcondition=lazy::and_(
                      application(
                              find_case_function(e.enumeratedtype_index,sort_bool::bool_()),
                              push_front(conditionlist,data_expression(e.var))),
                      resultcondition);
        }
      }

      /* now we construct the arguments of the action */
      /* First we collect all multi-actions in a separate vector
         of multiactions */
      atermpp::vector < action_list > multiActionList;
      bool multiActionIsDelta=false;

      for(summand_list::const_iterator walker=sumlist.begin(); walker!=sumlist.end() ; ++walker)
      { if (walker->is_delta())
        { multiActionIsDelta=true;
          break;
        }
        multiActionList.push_back(walker->actions());
      }

      action_list resultmultiactionlist;
      if (!multiActionIsDelta)
      { long multiactioncount= multiActionList[0].size(); // The number of multi actions.
        for( ; multiactioncount>0 ; multiactioncount-- )
        {
          data_expression_list resultf;
          // fcnt is the arity of the action with index multiactioncount-1;
          // const action a= *(multiActionList[0].begin()+(multiactioncount-1));
          action_list::const_iterator a=multiActionList[0].begin();
          for(long i=1 ; i<multiactioncount ; ++i,++a) {}
          // const action a= *((multiActionList[0]).begin()+(multiactioncount-1));
          long fcnt=(a->arguments()).size();
          data_expression f;

          for( ; fcnt>0 ; fcnt-- )
          { data_expression_list auxresult;
            data_expression equalterm;
            bool equaluptillnow=true;
            atermpp::vector < variable_list >  ::const_iterator auxrename_list_pars=rename_list_pars.begin();
            atermpp::vector < data_expression_list >::const_iterator auxrename_list_args=rename_list_args.begin();
            atermpp::vector<action_list>::const_iterator  multiactionwalker=multiActionList.begin();
            for(summand_list::const_iterator walker=sumlist.begin(); walker!=sumlist.end();
                                               ++walker,++multiactionwalker)
            { assert(auxrename_list_pars!=rename_list_pars.end());
              assert(auxrename_list_args!=rename_list_args.end());
              const variable_list auxpars= *auxrename_list_pars;
              ++auxrename_list_pars;
              const data_expression_list auxargs= *auxrename_list_args;
              ++auxrename_list_args;
              // f is the fcnt-th argument of the multiactioncount-th action in the list
              action_list::const_iterator a1=multiactionwalker->begin();
              for(long i=1; i<multiactioncount; ++i, ++a1){};
              data_expression_list::const_iterator d1=(a1->arguments()).begin();
              for(long i=1; i<fcnt; ++i, ++d1){};
              f= *d1;
              const data_expression auxresult1=substitute_data(auxargs,auxpars,f);

              if (equalterm==data_expression())
              { equalterm=auxresult1;
              }
              else
              { if (equaluptillnow)
                { equaluptillnow=(equalterm==auxresult1);
              } }
              auxresult=push_front(auxresult,auxresult1);
            }
            if (equaluptillnow)
            { resultf=push_front(resultf,equalterm);
            }
            else
            { if (!options.binary)
              { resultf=push_front(resultf,
                          data_expression(application(
                               find_case_function(e.enumeratedtype_index,f.sort()),
                               push_front(auxresult,data_expression(e.var)))));
              }
              else
              { data_expression temp=construct_binary_case_tree(
                                  n,
                                  resultsum,
                                  auxresult,
                                  f.sort(),
                                  e);
                resultf=push_front(resultf,temp);
              }
            }
          }
          resultmultiactionlist=
                push_front(
                  resultmultiactionlist,
                  action(multiActionList[0].front().label(),resultf));
        }
      }

      /* Construct resulttime, the time of the action ... */

      equaluptillnow=true;
      equalterm=data_expression();
      bool some_summand_has_time=false;
      bool all_summands_have_time=true;

      // first find out whether there is a summand with explicit time.
      for(summand_list::const_iterator walker=sumlist.begin() ; walker!=sumlist.end(); ++walker)
      { if (walker->has_time())
        { some_summand_has_time=true;
        }
        else
        { all_summands_have_time=false;
        }
      }

      if ((some_summand_has_time))
      { variable dummy_time_variable;
        if (!all_summands_have_time)
        { // Generate a fresh dummy variable, and add it to the summand variables
          dummy_time_variable=get_fresh_variable("dt",sort_real::real_());
          resultsum=push_front(resultsum,dummy_time_variable);
        }
        auxrename_list_pars=rename_list_pars.begin();
        auxrename_list_args=rename_list_args.begin();
        auxresult=data_expression_list();
        for(summand_list::const_iterator walker=sumlist.begin(); walker!=sumlist.end(); ++walker)
        { if (walker->has_time())
          { const data_expression actiontime=walker->time();

            assert(auxrename_list_pars!=rename_list_pars.end());
            assert(auxrename_list_args!=rename_list_args.end());
            const variable_list auxpars= *auxrename_list_pars;
            ++auxrename_list_pars;
            const data_expression_list auxargs= *auxrename_list_args;
            ++auxrename_list_args;

            const data_expression auxresult1=substitute_time(auxargs,auxpars,actiontime);
            if (equalterm==data_expression())
            { equalterm=auxresult1;
            }
            else
            { if (equaluptillnow)
              { equaluptillnow=(auxresult1==equalterm);
              }
            }
            auxresult=push_front(auxresult,auxresult1);
          }
          else
          { // this summand does not have time. But some summands have.
            auxresult=push_front(auxresult,data_expression(dummy_time_variable));
            equaluptillnow=false;
          }
        }
        if (options.binary==1)
        { resulttime=construct_binary_case_tree(n,
                      resultsum,auxresult,sort_real::real_(),e);
        }
        else
        {
          if (equaluptillnow)
          { resulttime=equalterm;
          }
          else
          { resulttime=application(
                           find_case_function(e.enumeratedtype_index,sort_real::real_()),
                           push_front(auxresult,data_expression(e.var)));
          }
        }
      }
      /* now we construct the arguments of the invoked
         process, i.e. the new function g */
      long fcnt=0;
      data_expression_list resultnextstate;


      for(variable_list::const_iterator var_it=gsorts.begin(); var_it!=gsorts.end(); ++var_it)
      {
        equalterm=data_expression();
        equaluptillnow=1;
        auxrename_list_pars=rename_list_pars.begin();
        auxrename_list_args=rename_list_args.begin();
        data_expression_list auxresult;
        for(summand_list::const_iterator walker=sumlist.begin(); walker!=sumlist.end(); ++walker)
        {
          const assignment_list nextstate=walker->assignments();

          assert(auxrename_list_pars!=rename_list_pars.end());
          assert(auxrename_list_args!=rename_list_args.end());
          const variable_list auxpars= *auxrename_list_pars;
          ++auxrename_list_pars;
          const data_expression_list auxargs= *auxrename_list_args;
          ++auxrename_list_args;

          data_expression nextstateparameter;
          nextstateparameter=getRHSassignment(*var_it,nextstate);

          data_expression auxresult1=substitute_data(auxargs,auxpars,nextstateparameter);
          if (equalterm==data_expression())
          { equalterm=auxresult1;
          }
          else if (equaluptillnow)
          { equaluptillnow=(equalterm==auxresult1);
          }

          auxresult=push_front(auxresult,auxresult1);
        }
        if (equaluptillnow)
        { resultnextstate=push_front(resultnextstate,equalterm);
        }
        else
        { if (!options.binary)
          { resultnextstate=
                 push_front(resultnextstate,
                      data_expression(application(
                           find_case_function(
                                e.enumeratedtype_index,
                                var_it->sort()),
                           push_front(auxresult,data_expression(e.var)))));
          }
          else
          { data_expression temp=construct_binary_case_tree(
                               n,
                               resultsum,
                               auxresult,
                               var_it->sort(),
                               e);
            resultnextstate=push_front(resultnextstate,temp);
          }
        }
        fcnt++;
      }
      /* Now turn *resultg around */
      resultnextstate=reverse(resultnextstate);
      /* The list of arguments in nextstate are now in a sequential form, and
           must be transformed back to a list of assignments */
      const assignment_list final_resultnextstate=make_assignment_list(gsorts,resultnextstate);
      return summand_(resultsum,resultcondition,multiActionIsDelta,resultmultiactionlist,
                                       some_summand_has_time,resulttime,final_resultnextstate);
    }

    sort_expression_list getActionSorts(const action_list actionlist)
    { sort_expression_list resultsorts;

      for(action_list::const_iterator i=actionlist.begin(); i!=actionlist.end(); ++i)
      { resultsorts=i->label().sorts()+resultsorts;
      }
      return resultsorts;
    }

    summand_list cluster_actions(
                           const summand_list sums,
                           const variable_list pars)
    { /* We cluster first the summands with the action
          occurring in the first summand of sums.
          These summands are first stored in w1. */

      summand_list result;
      summand_list reducible_sumlist=sums;

      for(summand_list::const_iterator i=sums.begin() ; i!=sums.end() ; ++i)
      { const summand summand1=*i;

        summand_list w1;
        summand_list w2;
        for(summand_list::const_iterator w3=reducible_sumlist.begin(); w3!=reducible_sumlist.end(); ++w3)
        { const summand summand2=*w3;
          if (summandsCanBeClustered(summand1,summand2))
          { w1=push_front(w1,summand2);
          }
          else
          { w2=push_front(w2,summand2);
          }
        }
        reducible_sumlist=w2;

        /* In w1 we now find all the summands labelled with
           similar multiactions, actiontime and terminationstatus.
           We must now construct its clustered form. */
        unsigned int n=w1.size();

        if (n>0)
        { if (n>1)
          { const action_list multiaction=w1.front().actions();
            sort_expression_list actionsorts;
            if (!w1.front().is_delta())
            { actionsorts=getActionSorts(multiaction);
            }

            const enumtype enumeratedtype_(options.binary?2:n,actionsorts,get_sorts(pars),*this);
            // const enumtype *enumeratedtype_=new enumtype(options.binary?2:n,actionsorts,get_sorts(pars),*this);

            result=push_front(result,
                   collect_sum_arg_arg_cond(enumeratedtype_,n,w1,pars));
          }
          else
          { result=w1 + result;
          }
        }
      }
      return result;
    }

    /**************** GENERaTE LPEpCRL **********************************/


    /* The variable regular indicates that we are interested in generating
       a LPE assuming the pCRL term under consideration is regular */

    summand_list generateLPEpCRL(const process_identifier procId,
                                        const bool canterminate,
                                        const bool containstime,
                                        const bool regular,
                                        variable_list &parameters,
                                        assignment_list &init)
    /* A pair of initial state and linear process must be extracted
       from the underlying GNF */
    { bool singlecontrolstate=false;
      int n=objectIndex(procId);

      atermpp::vector < process_identifier > pCRLprocs;
      pCRLprocs.push_back(procId);

      makepCRLprocs(objectdata[n].processbody,pCRLprocs);
      /* now pCRLprocs contains a list of all process id's in this
         pCRL process */

      /* collect the parameters, but assume that variables
         have a unique sort */
      if (pCRLprocs.size()==1)
      { singlecontrolstate=true;
      }
      parameters=collectparameterlist(pCRLprocs);

      alphaconversion(procId,parameters);
      /* We reverse the pCRLprocslist to give the processes that occur first the
         lowest index. In particular initial states get value 1, instead of the
         highest value, as happened hitherto (29/9/05). Not necessary anymore, now
         that we are using a vector (16/5/2009). */
      if ((!regular)||((!singlecontrolstate)&&(options.newstate)&&(!options.binary)))
      { declare_control_state(pCRLprocs);
      }
      stacklisttype stack(parameters,*this,regular,pCRLprocs,singlecontrolstate);

      if (regular)
      {
        if ((options.binary) && (options.newstate))
        { parameters=stack.parameters;
          if (!singlecontrolstate)
          { parameters=reverse(stack.booleanStateVariables) + parameters;
          }
        }
        else  /* !binary or oldstate */
        { parameters=
                ((!singlecontrolstate)?
                      push_front(stack.parameters,stack.stackvar):
                      stack.parameters);
        }
      }
      else /* not regular, use a stack */
      { parameters=push_front(variable_list(),stack.stackvar);
      }

      init=make_initialstate(procId,stack,pCRLprocs,
                                  regular,singlecontrolstate,parameters);

      summand_list sums=collectsumlist(pCRLprocs,parameters,stack,
               (canterminate&&objectdata[n].canterminate),regular,
                   singlecontrolstate);

      if (!options.no_intermediate_cluster)
      { sums=cluster_actions(sums,parameters);
      }

      if ((!containstime) || options.add_delta)
      { /* We add a delta summand to each process, if the flag
           add_delta is set, or if the process does not contain any
           explicit reference to time. This affects the behaviour of each
           process in the sense that each process can idle
           indefinitely. It has the advantage that large numbers
           numbers of timed delta summands are subsumed by
           this delta. As the removal of timed delta's
           is time consuming in the linearisation, the use
           of this flag, can speed up linearisation considerably */
        sums=push_front(sums,
                        summand_(variable_list(),sort_bool::true_(),true,action_list(),
                                 false,data_expression(),assignment_list()));
      }

      return sums;
    }


    /**************** hiding *****************************************/

    action_list hide_(const identifier_string_list hidelist, const action_list multiaction)
    { action_list resultactionlist;

      for (action_list::const_iterator walker=multiaction.begin();
                walker!=multiaction.end(); ++walker)
      { if (std::find(hidelist.begin(),hidelist.end(),walker->label().name())==hidelist.end())
        { resultactionlist=push_front(resultactionlist,*walker);
        }
      }

      /* reverse the actionlist to maintain the ordering */
      return reverse(resultactionlist);
    }

    summand_list hidecomposition(const identifier_string_list hidelist, const summand_list summands)
    { summand_list resultsumlist;

      for(summand_list::const_iterator i=summands.begin(); i!=summands.end() ; ++i)
      {
        const variable_list sumvars=i->summation_variables();
        const action_list multiaction=i->actions();
        const data_expression actiontime=i->time();
        const data_expression condition=i->condition();
        const assignment_list nextstate=i->assignments();

        action_list acts;
        if (!i->is_delta())
        { acts=hide_(hidelist,multiaction);
        }
        resultsumlist=push_front(
                        resultsumlist,
                        summand_(sumvars,condition,i->is_delta(),acts,
                                     i->has_time(),actiontime,nextstate));
      }
      return reverse(resultsumlist);
    }

    /**************** allow/block *************************************/

    bool implies_condition(const data_expression c1, const data_expression c2)
    {
      if (c2==sort_bool::true_())
      { return true;
      }

      if (c1==sort_bool::false_())
      { return true;
      }

      if (c1==sort_bool::true_())
      { return false;
      }

      if (c2==sort_bool::false_())
      { return false;
      }

      if (c1==c2)
      { return true;
      }

      /* Dealing with the conjunctions (&&) first and then the disjunctions (||)
         yields a 10-fold speed increase compared to the case where first the
         || occur, and then the &&. This result was measured on the alternating
         bit protocol, with --regular. */

      if (sort_bool::is_and_application(c2))
      {
        return implies_condition(c1,application(c2).left()) &&
               implies_condition(c1,application(c2).right());
      }

      if (sort_bool::is_or_application(c1))
      { return implies_condition(application(c1).left(),c2) &&
               implies_condition(application(c1).right(),c2);
      }

      if (sort_bool::is_and_application(c1))
      { return implies_condition(application(c1).left(),c2) ||
               implies_condition(application(c1).right(),c2);
      }

      if (sort_bool::is_or_application(c2))
      { return implies_condition(c1,application(c2).left()) ||
               implies_condition(c1,application(c2).right());
      }

      return false;
    }

    summand_list insert_timed_delta_summand(
                        const summand_list l,
                        const summand s)
    { /* The delta summands are put in front.
         The sequence of summands is maintained as
         good as possible, to eliminate summands as
         quickly as possible */
      assert(s.is_delta());
      summand_list result;

      const variable_list sumvars=s.summation_variables();
      const data_expression cond=s.condition();
      const data_expression actiontime=s.time();

      summand_list tail=l;
      for(summand_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      { const summand smmnd=*i;
        const data_expression cond1=i->condition();
        tail=pop_front(tail);
        if ((!options.add_delta) &&
            ((actiontime==i->time()) || (!i->has_time())) &&
            (implies_condition(cond,cond1)))
        { /* put the summand that was effective in removing
             this delta summand to the front, such that it
             is encountered early later on, removing a next
             delta summand */
          return push_front(reverse(result)+tail,smmnd);
        }
        if ((i->is_delta()) &&
            ((options.add_delta)||
                (((actiontime==smmnd.time())|| (!s.has_time())) &&
                 (implies_condition(cond1,cond)))))
        { /* do not add summand to result, as it is superseded by s */
        }
        else
        {
          result=push_front(result,smmnd);
        }
      }

      result=push_front(reverse(result),
                     summand_(sumvars,cond,s.is_delta(),s.actions(),s.has_time(),
                                          actiontime,s.assignments()));
      return result;
    }

    action_name_multiset_list sortMultiActionLabels(const action_name_multiset_list l)
    { action_name_multiset_list result;
      for(action_name_multiset_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      { result=push_front(result,sortActionLabels(*i));
      }
      return result;
    }

    /// \brief determine whether the multiaction has the same labels as the allow action,
    //         in which case true is delivered. If multiaction is the action Terminate,
    //         then true is also returned.

    bool allowsingleaction(const action_name_multiset allowaction,
                                  const action_list multiaction)
    { if (multiaction==push_front(action_list(),terminationAction))
      { // multiaction is equal to terminate. This action cannot be blocked.
        return true;
      }
      const identifier_string_list names=allowaction.names();
      identifier_string_list::const_iterator i=names.begin();
      for (action_list::const_iterator walker=multiaction.begin();
                  walker!=multiaction.end(); ++walker,++i)
      { if (i==names.end())
        { return false;
        }

        if (*i!=walker->label().name())
        { return false;
        }
      }
      if (i==names.end())
      { return true;
      }
      return false;
    }

    bool allow_(const action_name_multiset_list allowlist,
                       const action_list multiaction)
    { /* The empty multiaction, i.e. tau, is never blocked by allow */
      if (multiaction.empty())
      { return true; }

      for(action_name_multiset_list::const_iterator i=allowlist.begin();
               i!=allowlist.end(); ++i)
      { if (allowsingleaction(*i,multiaction))
        { return true;
        }
      }
      return false;
    }

    bool encap(const identifier_string_list encaplist, const action_list multiaction)
    { for (action_list::const_iterator walker=multiaction.begin();
                walker!=multiaction.end(); ++walker)
      { // for(identifier_string_list::const_iterator i=encaplist.begin(); i!=encaplist.end(); ++i)
        // building an iterator over an identifier_string_list does not work. TODO.
        for(identifier_string_list i=encaplist; !i.empty(); i=pop_front(i))
        { const identifier_string s1= i.front();
          const identifier_string s2=walker->label().name();
          if (s1==s2)
          { return true;
          }
        }
      }
      return false;
    }

    summand_list allowblockcomposition(
                          const action_name_multiset_list allowlist1,  // This is a list of list of identifierstring.
                          const summand_list sourcesumlist,
                          const bool is_allow)
    { /* This function calculates the allow or the block operator,
         depending on whether is_allow is true */

      summand_list resultdeltasumlist;
      summand_list resultsimpledeltasumlist;
      summand_list resultactionsumlist;
      action_name_multiset_list allowlist((is_allow)?sortMultiActionLabels(allowlist1):allowlist1);

      int sourcesumlist_length=sourcesumlist.size();
      if (sourcesumlist_length>2 || is_allow) // This condition prevents this message to be printed
                                              // when performing data elimination. In this case the
                                              // term delta is linearised, to determine which data
                                              // is essential for all processes. In these cases a
                                              // message about the block operator is very confusing.
      { if (core::gsVerbose)
        { std::cerr << "- calculating the " << (is_allow?"allow":"block") <<
                 " operator on " << sourcesumlist.size() << " summands";
        }
      }

      /* First add the resulting sums in two separate lists
         one for actions, and one for delta's. The delta's
         are added at the end to the actions, where for
         each delta summand it is determined whether it ought
         to be added, or is superseded by an action or another
         delta summand */
      for(summand_list::const_iterator i=sourcesumlist.begin(); i!=sourcesumlist.end(); ++i)
      { const summand smmnd= *i;
        const variable_list sumvars=smmnd.summation_variables();
        const action_list multiaction=smmnd.actions();
        const data_expression actiontime=smmnd.time();
        const data_expression condition=smmnd.condition();


        if (!i->is_delta() &&
            ((is_allow && allow_(allowlist,multiaction)) ||
             (!is_allow && !encap(allowlist,multiaction))))
        { resultactionsumlist=push_front(
                        resultactionsumlist,
                        smmnd);
        }
        else
        { if (smmnd.has_time())
          { resultdeltasumlist=push_front(
                        resultdeltasumlist,
                          summand( sumvars,
                                   condition,
                                   true,
                                   action_list(),
                                   actiontime,
                                   assignment_list())); // We are not interested in the nextstate after delta.
          }
          else
          { // summand has no time.
            if (condition==sort_bool::true_())
            { resultsimpledeltasumlist=push_front(
                          resultsimpledeltasumlist,
                            summand(
                                   sumvars,
                                   condition,
                                   true,
                                   action_list(),
                                   assignment_list())); // We are not interested in the nextstate after delta.
            }
            else
            { resultdeltasumlist=push_front(
                          resultdeltasumlist,
                            summand(
                                   sumvars,
                                   condition,
                                   true,
                                   action_list(),
                                   assignment_list())); // We are not interested in the nextstate after delta.
            }
          }
        }
      }

      summand_list resultsumlist=resultactionsumlist;

      if (options.nodeltaelimination)
      { resultsumlist=resultsimpledeltasumlist + resultdeltasumlist + resultactionsumlist;
      }
      else
      {
        if (!options.add_delta) /* if a delta summand is added, conditional, timed
                                   delta's are subsumed and do not need to be added */
        { for (summand_list::const_iterator j=resultsimpledeltasumlist.begin();
                 j!=resultsimpledeltasumlist.end(); ++j)
          { resultsumlist=insert_timed_delta_summand(resultsumlist,*j);
          }
          for (summand_list::const_iterator j=resultdeltasumlist.begin();
                 j!=resultdeltasumlist.end(); ++j)
          { resultsumlist=insert_timed_delta_summand(resultsumlist,*j);
          }
        }
        else
        { // Add a true -> delta
          resultsumlist=push_front(resultsumlist,
                 summand(variable_list(),sort_bool::true_(),true,action_list(),assignment_list()));
        }
      }
      if ((core::gsVerbose) && (sourcesumlist_length>2 || is_allow))
      { std::cerr << ", resulting in " << resultsumlist.size() << " summands\n";
      }

      return resultsumlist;
    }

    /**************** renaming ******************************************/

    action rename_action(const rename_expression_list renamings, const action act)
    { const action_label actionId=act.label();
      const identifier_string s=actionId.name();
      for (rename_expression_list::const_iterator i=renamings.begin(); i!=renamings.end(); ++i)
      { if (s==i->source())
        { 
          return action(action_label(i->target(),actionId.sorts()),
                        act.arguments());
        }
      }
      return act;
    }

    action_list rename_actions(const rename_expression_list renamings,
                               const action_list multiaction)
    { action_list resultactionlist;

      for (action_list::const_iterator walker=multiaction.begin();
                 walker!=multiaction.end(); ++walker)
      { resultactionlist=linInsertActionInMultiActionList(
                              rename_action(renamings,*walker),
                              resultactionlist);
      }
      return resultactionlist;
    }

    summand_list renamecomposition(const rename_expression_list renamings,
                                   const summand_list sourcesumlist)
    { summand_list resultsumlist;

      for(summand_list::const_iterator i=sourcesumlist.begin(); i!=sourcesumlist.end(); ++i)
      { if (i->is_delta())
        { resultsumlist=push_front(resultsumlist,*i);
        }
        else
        { const summand smmnd=*i;
          const variable_list sumvars=smmnd.summation_variables();
          const action_list multiaction=smmnd.actions();
          const data_expression actiontime=smmnd.time();
          const data_expression condition=smmnd.condition();
          const assignment_list nextstate=smmnd.assignments();

          resultsumlist=push_front(
                          resultsumlist,
                          summand_(sumvars,condition,false,rename_actions(renamings,multiaction),
                                        smmnd.has_time(),actiontime,nextstate));
        }
      }
      return reverse(resultsumlist);
    }

    /**************** equalargs ****************************************/

    bool occursinvarandremove(const variable var, variable_list &vl)
    { bool result=false;

      if (vl.empty())
      { return 0;
      }
      vl=pop_front(vl);
      const variable var1=vl.front();
      if (var==var1)
      { return true;
      }

      // Names of variables cannot be the same, even if they have different types.
      if (var.name()==var1.name())
      { throw mcrl2::runtime_error("variable conflict " + pp(var) + ":" + pp(var.sort()) + " versus " +
                   pp(var1) + ":" + pp(var1.sort()) + ".");
      }

      result=occursinvarandremove(var,vl);
      vl=push_front(vl,var1);
      return result;
    }

    /********************** construct renaming **************************/

    variable_list construct_renaming(
                           const variable_list pars1,
                           const variable_list pars2,
                           variable_list &pars3,
                           variable_list &pars4,
                           const bool unique=true)
    { /* check whether the variables in pars2 are unique,
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
      { pars3=variable_list();
        pars4=variable_list();
      }
      else
      { variable var2=pars2.front();
        variable var3=var2;
        for(int i=0 ; occursin(var3,pars1) ; i++)
        { var3=get_fresh_variable(var2.name(),var2.sort(),(unique?-1:i));
        }
        if (var3!=var2)
        { t1=push_front(construct_renaming(pars1,pop_front(pars2),t,t2,unique),var3);

          pars4=push_front(t2,var2);
          pars3=push_front(t,var3);
        }
        else
        { t1=construct_renaming(pars1,pop_front(pars2),t,pars4,unique);
          pars3=push_front(t,var2);
        }

      }
      return t1;
    }

    /**************** communication operator composition ****************/

    identifier_string_list insertActionLabel(
                          const identifier_string action,
                          const identifier_string_list actionlabels)
    { /* assume actionlabels is sorted, and put
         action at the proper place to yield a sorted
         list */
      if (actionlabels.empty())
      { return push_front(identifier_string_list(),action);
      }

      const identifier_string firstAction=actionlabels.front();

      if (std::string(action)<std::string(firstAction))
      { return push_front(actionlabels,action);
      }

      return push_front(insertActionLabel(
                           action,
                           pop_front(actionlabels)),
                        firstAction);
    }

    action_name_multiset sortActionLabels(const action_name_multiset actionlabels1)
    { identifier_string_list result;
      const identifier_string_list actionlabels(actionlabels1.names());
      for(identifier_string_list::const_iterator i=actionlabels.begin();i!=actionlabels.end(); ++i)
      { result=insertActionLabel(*i,result);
      }
      return action_name_multiset(result);
    }

    template <typename List>
    sort_expression_list get_sorts(const List l)
    { if (l.empty()) return sort_expression_list();
      return push_front(get_sorts(pop_front(l)), l.front().sort());
    }

    // Check that the sorts of both termlists match.
    data_expression pairwiseMatch(const data_expression_list l1, const data_expression_list l2)
    {
      if (l1.empty())
      { if (l2.empty())
        { return sort_bool::true_();
        }
        return sort_bool::false_();
      }

      if (l2.empty())
      { return sort_bool::false_();
      }

      const data_expression t1=l1.front();
      const data_expression t2=l2.front();

      if (t1.sort()!=t2.sort())
      { return sort_bool::false_();
      }

      data_expression result=pairwiseMatch(pop_front(l1),pop_front(l2));

      return lazy::and_(result,RewriteTerm(equal_to(t1,t2))); 
    }

    // a tuple_list contains pairs of actions (multi-action) and the condition under which this action
    // can occur.
    typedef struct {
        atermpp::vector < action_list > actions;
        atermpp::vector < data_expression > conditions;
    } tuple_list;

    tuple_list addActionCondition(
                         const action firstaction,
                         const data_expression condition,
                         const tuple_list &L,
                         tuple_list S)
    { /* if firstaction==action(), it should not be added */
      assert(condition!=sort_bool::false_()); // It makes no sense to add an action with condition false,
                                              // as it cannot happen anyhow.
      for(unsigned int i=0; i<L.actions.size(); ++i)
      { S.actions.push_back((firstaction!=action())?
                   linInsertActionInMultiActionList(firstaction,L.actions[i]):
                   L.actions[i]);
        S.conditions.push_back(lazy::and_(L.conditions[i],condition));
      }
      return S;
    }


    // Type and variables for a somewhat more efficient storage of the
    // communication function

    class comm_entry:public boost::noncopyable
    { public:
        atermpp::vector <identifier_string_list> lhs;
        atermpp::vector <identifier_string> rhs;
        atermpp::vector <identifier_string_list> tmp;
        std::vector< bool > match_failed;

      comm_entry(const communication_expression_list communications)
      { for (communication_expression_list::const_iterator l=communications.begin();
                  l!=communications.end(); ++l)
        { lhs.push_back(l->action_name().names());
          rhs.push_back(l->name());
          tmp.push_back(identifier_string_list());
          match_failed.push_back(false);
        }
      }

      ~comm_entry()
      {}

      int size() const
      { assert(lhs.size()==rhs.size() && rhs.size()==tmp.size() && tmp.size()==match_failed.size());
        return lhs.size();
      }
    };

    action_label can_communicate(const action_list m, comm_entry &comm_table)
    { /* this function indicates whether the actions in m
         consisting of actions and data occur in C, such that
         a communication can take place. If not action_label() is delivered,
         otherwise the resulting action is the result. */
      // first copy the left-hand sides of communications for use
      for(int i=0; i<comm_table.size(); i++)
      {
        comm_table.tmp[i] = comm_table.lhs[i];
        comm_table.match_failed[i]=false;
      }

      // m must match a lhs; check every action
      for(action_list::const_iterator mwalker=m.begin(); mwalker!=m.end(); ++mwalker)
      { identifier_string actionname=mwalker->label().name();

        // check every lhs for actionname
        bool comm_ok = false;
        for(int i=0; i<comm_table.size(); i++)
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
          { // no match
            comm_table.match_failed[i] = true;
          } else {
            // possible match; on to next action
            comm_table.tmp[i] = pop_front(comm_table.tmp[i]);
            comm_ok = true;
          }
        }
        if ( !comm_ok ) // no (possibly) matching lhs
        {
          return action_label();
        }
      }

      // there is a lhs containing m; find it
      for (int i=0; i<comm_table.size(); i++)
      {
        // lhs i matches only if comm_table[i] is empty
        if ( (!comm_table.match_failed[i]) && comm_table.tmp[i].empty())
        { if ( comm_table.rhs[i] == tau() )
          { throw mcrl2::runtime_error("Communication should not result in tau");
            return action_label();
          }
          return action_label(comm_table.rhs[i],m.front().label().sorts());
        }
      }
      // no match
      return action_label();
    }

    bool might_communicate(const action_list m, 
                           comm_entry &comm_table,
                           const action_list n,
                           const bool n_is_null)
    { /* this function indicates whether the actions in m
         consisting of actions and data occur in C, such that
         a communication might take place (i.e. m is a subbag
         of the lhs of a communication in C).
         if n is not empty, then all actions of a matching communication
         that are not in m should be in n (i.e. there must be a
         subbag o of n such that m+o can communicate. */

      // first copy the left-hand sides of communications for use
      for(int i=0; i<comm_table.size(); i++)
      { comm_table.match_failed[i]=false;
        comm_table.tmp[i] = comm_table.lhs[i];
      }

      // m must be contained in a lhs; check every action
      for(action_list::const_iterator mwalker=m.begin(); mwalker!=m.end(); ++mwalker)
      { const identifier_string actionname=mwalker->label().name();
        // check every lhs for actionname
        bool comm_ok = false;
        for(int i=0; i<comm_table.size(); i++)
        {
          if (comm_table.match_failed[i])
          { continue;
          }
          if (comm_table.tmp[i].empty()) // actionname not in here; ignore lhs
          { comm_table.match_failed[i]=true;
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
            } else {
              // ignore actions that are not in m
              comm_table.tmp[i] = pop_front(comm_table.tmp[i]);
              if (comm_table.tmp[i].empty())
              { comm_table.match_failed[i]=true;
                break;
              }
            }
          }
          if (actionname==commname) // actionname found
          {
            comm_table.tmp[i]=pop_front(comm_table.tmp[i]);
            comm_ok = true;
          }
        }
        if (!comm_ok)
        { return false;
        }
      }

      if (n_is_null)
      { // there is a matching lhs
        return true;
      }
      else
      { // the rest of actions of lhs that are not in m should be in n

        // rest[i] contains the part of n in which lhs i has to find matching actions
        // rest_is_null[i] contains indications whether rest[i] is NULL.
        atermpp::vector < action_list > rest(comm_table.size(),n);
        std::vector < bool > rest_is_null(comm_table.size(),n_is_null);

        // check every lhs
        for(int i=0; i<comm_table.size(); i++)
        {
          if (comm_table.match_failed[i]) // lhs i did not contain m
          { continue;
          }
          // as long as there are still unmatch actions in lhs i...
          while (!comm_table.tmp[i].empty())
          { // .. find them in rest[i]
            if (rest[i].empty()) // no luck
            { rest_is_null[i] = true;
              break;
            }
            // get first action in lhs i
            const identifier_string commname = comm_table.tmp[i].front();
            identifier_string restname;
            // find it in rest[i]
            while (commname!=(restname = rest[i].front().label().name()))
            { rest[i] = pop_front(rest[i]);
              if (rest[i].empty()) // no more
              { rest_is_null[i] = true;
                break;
              }
            }
            if (commname!=restname) // action was not found
            { break;
            }
            // action found; try next
            rest[i] = pop_front(rest[i]);
            comm_table.tmp[i] = pop_front(comm_table.tmp[i]);
          }

          if (!rest_is_null[i]) // lhs was found in rest[i]
          { return true;
          }
        }

        // no lhs completely matches
        return false;
      }
    }

    tuple_list phi(const action_list m,
                   const data_expression_list d,
                   const action_list w,
                   const action_list n,
                   const action_list r,
                   const bool r_is_null,
                   comm_entry &comm_table)
    { /* phi is a function that yields a list of pairs
         indicating how the actions in m|w|n can communicate.
         The pairs contain the resulting multi action and
         a condition on data indicating when communication
         can take place. In the communication all actions of
         m, none of w and a subset of n can take part in the
         communication. d is the data parameter of the communication
         and C contains a list of multiaction action pairs indicating
         possible communications */

      if (!might_communicate(m,comm_table,n,false))
      { return tuple_list();
      }
      if (n.empty())
      { action_label c=can_communicate(m,comm_table); /* returns action_label() if no communication
                                             is possible */
        if (c!=action_label())
        { const tuple_list T=makeMultiActionConditionList_aux(w,comm_table,r,r_is_null);
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
      const action_list o=pop_front(n);
      const data_expression condition=pairwiseMatch(d,firstaction.arguments());
      if (condition==sort_bool::false_())
      { return phi(m,d,push_back(w,firstaction),o,r,r_is_null,comm_table);
      } 
      else 
      { 
        const tuple_list T=phi(push_back(m,firstaction),d,w,o,r,r_is_null,comm_table);
        return addActionCondition(
                    action(),
                    condition,
                    T,
                    phi(m,d,push_back(w,firstaction),o,r,r_is_null,comm_table));
      }
    }

    bool xi(const action_list alpha, const action_list beta, comm_entry &comm_table)
    { if (beta.empty())
      { if (can_communicate(alpha,comm_table)!=action_label())
        { return true;
        }
        else
        { return false;
        }
      }
      else
      { const action a = beta.front();
        const action_list l = push_back(alpha,a);
        const action_list beta_next = pop_front(beta);

        if (can_communicate(l,comm_table)!=action_label())
        { return true;
        } else if ( might_communicate(l,comm_table,beta_next,false) )
        {
          return xi(l,beta_next,comm_table) || xi(alpha,beta_next,comm_table);
        } else {
          return xi(alpha,beta_next,comm_table);
        }
      }
    }

    data_expression psi(const action_list alpha_in, comm_entry &comm_table)
    { action_list alpha=reverse(alpha_in);
      data_expression cond = sort_bool::false_();
      while ( !alpha.empty() )
      {
        const action a = alpha.front();
        action_list beta = pop_front(alpha);

        while (!beta.empty())
        { const action_list actl=push_front(push_front(action_list(),beta.front()),a);
          if (might_communicate(actl,comm_table,pop_front(beta),false) && xi(actl,pop_front(beta),comm_table))
          { // sort and remove duplicates??
            cond = lazy::or_(cond,pairwiseMatch(a.arguments(),beta.front().arguments()));
          }
          beta = pop_front(beta);
        }

        alpha = pop_front(alpha);
      }
      return lazy::not_(cond);
    }

    // returns a list of tuples.
    tuple_list makeMultiActionConditionList_aux(
                       const action_list multiaction,
                       comm_entry &comm_table,
                       const action_list r,
                       const bool r_is_null)
    { /* This is the function gamma(m,C,r) provided
         by Muck van Weerdenburg in Calculation of
         Communication with open terms [1]. */
      if (multiaction.empty())
      { tuple_list t;
        t.conditions.push_back((r_is_null)?sort_bool::true_():static_cast< data_expression const& >(psi(r,comm_table)));
        t.actions.push_back(action_list());
        return t;
      }

      const action firstaction=multiaction.front();
      const action_list remainingmultiaction=pop_front(multiaction); /* This is m in [1] */

      const tuple_list S=phi(push_front(action_list(),firstaction),
                             firstaction.arguments(),
                             action_list(),
                             remainingmultiaction,
                             r,r_is_null,comm_table);
      const tuple_list T=makeMultiActionConditionList_aux(
                      remainingmultiaction,comm_table,
                      (r_is_null)?push_front(action_list(),firstaction):push_front(r,firstaction),false);
      return addActionCondition(firstaction,sort_bool::true_(),T,S);
    }

    tuple_list makeMultiActionConditionList(
                       const action_list multiaction,
                       const communication_expression_list communications)
    { comm_entry comm_table(communications);
      return makeMultiActionConditionList_aux(multiaction,comm_table,action_list(),true);
    }

    summand_list communicationcomposition(
                          const communication_expression_list communications,
                          const summand_list summands)
    { /* We follow the implementation of Muck van Weerdenburg, described in
         a note: Calculation of communication with open terms. */

      if (core::gsVerbose)
      { std::cerr << "- calculating the communication operator on " << summands.size() << " summands";
      }

      /* first we sort the multiactions in communications */
      communication_expression_list resultingCommunications;

      for(communication_expression_list::const_iterator i=communications.begin();
             i!=communications.end(); ++i)
      { const action_name_multiset source=i->action_name();
        const identifier_string target=i->name();
        if (gsIsNil(i->name()))  // Right hand side of communication is empty. We receive a bad datatype.
        { throw mcrl2::runtime_error("Right hand side of communication " + pp(source) + " in a comm command cannot be empty or tau");
        }
        resultingCommunications=push_front(resultingCommunications,
                   communication_expression(sortActionLabels(source),target));
      }
      communication_expression_list communications1=resultingCommunications;

      summand_list resultsumlist;
      summand_list resultingDeltaSummands;

      for(summand_list::const_iterator sourcesumlist=summands.begin();
                    sourcesumlist!=summands.end(); ++sourcesumlist)
      { const summand smmnd=*sourcesumlist;
        const variable_list sumvars=smmnd.summation_variables();
        const action_list multiaction=smmnd.actions();
        if (smmnd.is_delta())
        { resultingDeltaSummands=push_front(
                          resultingDeltaSummands,
                          smmnd);
        }
        else
        { 
          const data_expression condition=smmnd.condition();
          const assignment_list nextstate=smmnd.assignments();

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
          for(variable_list::const_iterator i=sumvars.begin(); i!=sumvars.end(); ++i)
          { const variable sumvar=*i;
            if (occursinterm(sumvar,condition) ||
                (smmnd.has_time() && occursinterm(sumvar,smmnd.time())))
            { newsumvars=push_front(newsumvars,sumvar);
            }
          }
          newsumvars=reverse(newsumvars);

          resultingDeltaSummands=
               push_front(resultingDeltaSummands,
                    summand_(newsumvars,condition,true,action_list(),
                                  smmnd.has_time(),smmnd.time(),nextstate));

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
          for(unsigned int i=0 ; i<multiactionconditionlist.actions.size(); i++)
          { const data_expression communicationcondition=
                        RewriteTerm(multiactionconditionlist.conditions[i]);

            const action_list multiaction=multiactionconditionlist.actions[i];
            const data_expression newcondition=RewriteTerm(
                             lazy::and_(condition,communicationcondition));
            summand new_summand=
                     summand_(sumvars,newcondition,false,multiaction,
                                       smmnd.has_time(),smmnd.time(),nextstate);
            if (!options.nosumelm)
            { action_summand act_summand(summand_to_action_summand(new_summand));
              sumelm(act_summand);
              new_summand = action_summand_to_aterm(act_summand);
            }

            if (newcondition!=sort_bool::false_())
            { resultsumlist=push_front(resultsumlist,new_summand);
            }
          }
        }
      }

      /* Now the resulting delta summands must be added again */

      if (options.nodeltaelimination)
      { resultsumlist=resultsumlist + resultingDeltaSummands;
      }
      else
      { for (summand_list::const_iterator w=resultingDeltaSummands.begin();
                  w!=resultingDeltaSummands.end(); ++w)
        { resultsumlist=insert_timed_delta_summand(resultsumlist,*w);
        }
      }

      if (core::gsVerbose)
      { std::cerr << " resulting in " << resultsumlist.size() << " summands\n";
      }
      return reverse(resultsumlist);
    }

    bool check_real_variable_occurrence(
                         const variable_list sumvars,
                         const data_expression actiontime,
                         const data_expression condition)
    { /* Check whether actiontime is an expression
         of the form t1 +...+ tn, where one of the
         ti is a variable in sumvars that does not occur in condition */

      if (actiontime.is_variable())
      { if (occursintermlist(actiontime,data_expression_list(sumvars)) && !occursinterm(actiontime,condition))
        { return true;
        }
      }

      if (sort_real::is_plus_application(actiontime))
      { return (check_real_variable_occurrence(sumvars,application(actiontime).left(),condition) ||
                check_real_variable_occurrence(sumvars,application(actiontime).right(),condition));
      }

      return false;
    }

    data_expression makesingleultimatedelaycondition(
                         const variable_list sumvars,
                         const variable_list freevars,
                         const data_expression condition,
                         const bool has_time,
                         const variable timevariable,
                         const data_expression actiontime,
                         variable_list &used_sumvars)
    { /* Generate a condition of the form:

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
      { result=condition;
      }
      else
      { result=RewriteTerm(
                 lazy::and_(
                 condition,
                 less(timevariable,actiontime)));
        variables=push_front(variables,timevariable);
      }
      for (variable_list::const_iterator i=freevars.begin(); i!=freevars.end(); ++i)
      { if (occursinterm(*i,result))
        { variables=push_front(variables,*i);
        }
      }

      for(atermpp::set<variable>::const_iterator p=global_variables.begin();
                p!=global_variables.end() ; ++p)
      { if (occursinterm(*p,result))
        { variables=push_front(variables,*p);
        }
      }

      for (variable_list::const_iterator s=sumvars.begin(); s!=sumvars.end(); ++s)
      {
        if (occursinterm(*s,result))
        {
          used_sumvars = push_front(used_sumvars, *s);
        }
      }
      used_sumvars = reverse(used_sumvars);

      return result;
    }

    data_expression getUltimateDelayCondition(
                     const summand_list sumlist,
                     const variable_list freevars,
                     const data_expression timevariable,
                     variable_list &existentially_quantified_variables)
    { assert(existentially_quantified_variables.empty());

      for (summand_list::const_iterator walker=sumlist.begin();
                       walker!=sumlist.end(); ++walker)
      { /* First walk through the summands to see whether
           a summand with condition true that does not refer
           to time exists. In that case the ultimate delay
           condition is true */

        const summand smmnd=*walker;
        const data_expression condition=smmnd.condition();

        if ((!walker->has_time()) && (condition==sort_bool::true_()))
        {
          return sort_bool::true_();
        }

      }

      /* Unfortunately, no ultimate delay condition true can
         be generated. So, we must now traverse all conditions
         to generate a non trivial ultimate delay condition */

      data_expression  result=sort_bool::false_();
      for (summand_list::const_iterator walker=sumlist.begin();
                      walker!=sumlist.end(); ++walker)
      { const summand smmnd=*walker;
        const variable_list sumvars=smmnd.summation_variables();
        const data_expression actiontime=smmnd.time();
        const data_expression condition=smmnd.condition();

        variable_list new_existentially_quantified_variables;
        const data_expression intermediate_result=
               makesingleultimatedelaycondition(
                         sumvars,
                         freevars,
                         condition,
                         walker->has_time(),
                         timevariable,
                         actiontime,
                         new_existentially_quantified_variables);
        existentially_quantified_variables=merge_var_simple(
                               existentially_quantified_variables,
                               new_existentially_quantified_variables);
        result=lazy::or_(result,intermediate_result);
      }
      return result;
    }


    /******** make_unique_variables **********************/

    variable_list make_unique_variables(
                  const variable_list var_list,
                  const std::string& hint)
    { /* This function generates a list of variables with the same sorts
         as in variable_list, where all names are unique */

      if (var_list.empty()) return var_list;

      variable v=var_list.front();
      assert(v.is_variable());
      variable new_variable=get_fresh_variable(std::string(v.name()) + ((hint.empty())?"":"_") + hint,
                                  v.sort());
      return push_front(
                make_unique_variables(pop_front(var_list),hint),
                new_variable);
    }

    /******** make_parameters_and_variables_unique **********************/

    summand_list make_parameters_and_sum_variables_unique(
                                  const summand_list summands,
                                  variable_list &pars,
                                  assignment_list &init,
                                  const std::string hint="")
    { summand_list result_summands;

      const variable_list unique_pars=make_unique_variables(pars,hint);
      assert(unique_pars.size()==pars.size());
      init=substitute_assignmentlist(unique_pars,pars,init,pars,1,0);  // Only substitute the variables
                                                                       // the variables at the lhs.
      for(summand_list::const_iterator s=summands.begin(); s!=summands.end(); ++s)
      { const summand smmnd= *s;
        const variable_list sumvars=smmnd.summation_variables();
        variable_list unique_sumvars=make_unique_variables(sumvars,hint);
        assert(unique_sumvars.size()==sumvars.size());
        data_expression condition=smmnd.condition();
        action_list multiaction=smmnd.actions();
        data_expression actiontime=smmnd.time();
        assignment_list nextstate=smmnd.assignments();

        condition=substitute_data(unique_pars,pars,condition);
        condition=substitute_data(unique_sumvars,sumvars,condition);

        actiontime=substitute_time(unique_pars,pars,actiontime);
        actiontime=substitute_time(unique_sumvars,sumvars,actiontime);

        multiaction=substitute_multiaction(unique_pars,pars,multiaction),
        multiaction=substitute_multiaction(unique_sumvars,sumvars,multiaction),

        nextstate=substitute_assignmentlist(unique_pars,pars,nextstate,pars,1,1);
        nextstate=substitute_assignmentlist(unique_sumvars,sumvars,nextstate,unique_pars,0,1);

        result_summands=push_front(result_summands,
              summand_(unique_sumvars,condition,s->is_delta(),multiaction,
                        s->has_time(),actiontime,nextstate));
      }
      pars=unique_pars;
      return result_summands;
    }





    /**************** parallel composition ******************************/

    summand_list combine_summand_lists(
                         const summand_list sumlist1,
                         const summand_list sumlist2,
                         const variable_list par1,
                         const variable_list par3,
                         const variable_list parametersOfsumlist2)

    { summand_list resultsumlist;
      variable_list allpars;

      allpars=par1 + par3;

      /* first we enumerate the summands of t1 */

      variable timevar=get_fresh_variable("timevar",sort_real::real_());
      variable_list ultimate_delay_sumvars1;
      data_expression ultimatedelaycondition=
                 (options.add_delta?sort_bool::true_():
                       static_cast< data_expression const& >(getUltimateDelayCondition(sumlist2,parametersOfsumlist2,
                                                      timevar,ultimate_delay_sumvars1)));

      for (summand_list::const_iterator walker1=sumlist1.begin();
                          walker1!=sumlist1.end(); ++walker1)
      { const summand summand1= *walker1;
        variable_list sumvars1=summand1.summation_variables() + ultimate_delay_sumvars1;
        action_list multiaction1=summand1.actions();
        data_expression actiontime1=summand1.time();
        data_expression condition1=summand1.condition();
        assignment_list nextstate1=summand1.assignments();
        bool has_time=summand1.has_time();

        if (multiaction1!=push_front(action_list(),terminationAction))
        {
          if (!has_time)
          { if (ultimatedelaycondition!=sort_bool::true_())
            { actiontime1=timevar;
              sumvars1=push_front(sumvars1,timevar);
              condition1=lazy::and_(ultimatedelaycondition,condition1);
              has_time=true;
            }
          }
          else
          { /* Summand1 has time. Substitute the time expression for
               timevar in ultimatedelaycondition, and extend the condition */
            const data_expression intermediateultimatedelaycondition=
                    substitute_data(
                       push_front(data_expression_list(),actiontime1),
                       push_front(variable_list(),timevar),
                       ultimatedelaycondition);
            condition1=lazy::and_(intermediateultimatedelaycondition,condition1);
          }

          condition1=RewriteTerm(condition1);
          if (condition1!=sort_bool::false_())
          { resultsumlist=
              push_front(
                resultsumlist,
                summand_(
                   sumvars1,
                   condition1,
                   summand1.is_delta(),
                   multiaction1, //substitute_multiaction(rename1_list,sums1renaming,multiaction1),
                   has_time,
                   actiontime1,
                   nextstate1));
          }
        }
      }
      /* second we enumerate the summands of sumlist2 */

      variable_list ultimate_delay_sumvars2;
      ultimatedelaycondition=(options.add_delta?sort_bool::true_():
                   static_cast< data_expression const& >(getUltimateDelayCondition(sumlist1,par1,
                                         timevar,ultimate_delay_sumvars2)));

      for (summand_list::const_iterator walker2=sumlist2.begin();
                  walker2!=sumlist2.end(); ++walker2)
      {
        const summand summand2= *walker2;
        variable_list sumvars2=summand2.summation_variables() + ultimate_delay_sumvars2;
        action_list multiaction2=summand2.actions();
        data_expression actiontime2=summand2.time();
        data_expression condition2=summand2.condition();
        assignment_list nextstate2=summand2.assignments();
        bool has_time=summand2.has_time();

        if (multiaction2!=push_front(action_list(),terminationAction))
        {
          if (!has_time)
          { if (ultimatedelaycondition!=sort_bool::true_())
            { actiontime2=data_expression(timevar);
              sumvars2=push_front(sumvars2,timevar);
              condition2=lazy::and_(ultimatedelaycondition,condition2);
              has_time=true;
            }
          }
          else
          { /* Summand2 has time. Substitute the time expression for
               timevar in ultimatedelaycondition, and extend the condition */
            const data_expression intermediateultimatedelaycondition=
                    substitute_data(
                       push_front(data_expression_list(),actiontime2),
                       push_front(variable_list(),timevar),
                       ultimatedelaycondition);
            condition2=lazy::and_(intermediateultimatedelaycondition,condition2);
          }

          condition2=RewriteTerm(condition2);
          if (condition2!=sort_bool::false_())
          {
            resultsumlist=
              push_front(
                resultsumlist,
                summand_(
                   sumvars2,
                   condition2,
                   summand2.is_delta(),
                   multiaction2,
                   has_time,
                   actiontime2,
                   nextstate2));
          }
        }
      }

      /* thirdly we enumerate all communications */

      for (summand_list::const_iterator walker1=sumlist1.begin();
                     walker1!=sumlist1.end(); ++walker1)
      { const summand summand1= *walker1;

        const variable_list sumvars1=summand1.summation_variables();
        const action_list multiaction1=summand1.actions();
        const data_expression actiontime1=summand1.time();
        const data_expression condition1=summand1.condition();
        const assignment_list nextstate1=summand1.assignments();

        for (summand_list::const_iterator walker2=sumlist2.begin();
                      walker2!=sumlist2.end(); ++walker2)
        {
          const summand summand2= *walker2;
          const variable_list sumvars2=summand2.summation_variables();
          const action_list multiaction2=summand2.actions();
          const data_expression actiontime2=summand2.time();
          const data_expression condition2=summand2.condition();
          const assignment_list nextstate2=summand2.assignments();

          if ((multiaction1==push_front(action_list(),terminationAction))==(multiaction2==push_front(action_list(),terminationAction)))
          { action_list multiaction3;
            bool is_delta3=false;
            if ((multiaction1==push_front(action_list(),terminationAction))&&(multiaction2==push_front(action_list(),terminationAction)))
            { multiaction3=push_front(action_list(),terminationAction);
            }
            else if (summand1.is_delta() || summand2.is_delta())
            { is_delta3=true;
            }
            else
            { multiaction3=linMergeMultiActionList(multiaction1,multiaction2);

            }
            const variable_list allsums=sumvars1+sumvars2;
            data_expression condition3= lazy::and_(condition1,condition2);
            data_expression action_time3;
            bool has_time3=false;

            if (!summand1.has_time())
            { if (!summand2.has_time())
              { has_time3=false;
              }
              else
              { /* summand 2 has time*/
                has_time3=summand2.has_time();
                action_time3=actiontime2;
              }
            }
            else
            { /* summand 1 has time */
              if (!summand2.has_time())
              { has_time3=summand1.has_time();
                action_time3=actiontime1;
              }
              else
              { /* both summand 1 and 2 have time */
                has_time3=true;
                action_time3=actiontime1;
                condition3=lazy::and_(
                              condition3,
                              equal_to(actiontime1,actiontime2));
              }
            }

            const assignment_list nextstate3=nextstate1+nextstate2;

            condition3=RewriteTerm(condition3);
            if ((condition3!=sort_bool::false_()) && (!is_delta3))
            { resultsumlist=
                push_front(
                  resultsumlist,
                  summand_(
                    allsums,
                    condition3,
                    is_delta3,
                    multiaction3,
                    has_time3,
                    action_time3,
                    nextstate3));
            }
          }
        }
      }

      return resultsumlist;
    }


    summand_list parallelcomposition(
                               const summand_list summands1,
                               const variable_list pars1,
                               const assignment_list init1,
                               const summand_list summands2,
                               const variable_list pars2,
                               const assignment_list init2,
                               variable_list &pars_result,
                               assignment_list &init_result)
    { if (core::gsVerbose)
      { std::cerr << "- calculating parallel composition: " << summands1.size() <<
                         " || " << summands2.size() << " = ";
      }

      // At this point the parameters of pars1 and pars2 are unique, except for
      // those that are constant in both processes. Constant parameters do not occur
      // in the initialisation of the processes.

      variable_list pars3;
      for(variable_list::const_iterator i=pars2.begin(); i!=pars2.end(); ++i)
      { if (std::find(pars1.begin(),pars1.end(),*i)==pars1.end())
        { // *i does not occur in pars1.
          pars3=push_front(pars3,*i);
        }
        else 
        { assert(!search_free_variable(init1,*i));
          assert(!search_free_variable(init2,*i));
        }
      }

      pars3=reverse(pars3);
      summand_list result=combine_summand_lists(summands1,summands2,pars1,pars3,pars2);

      if (core::gsVerbose)
      { std::cerr << result.size() << " resulting summands.\n";
      }
      pars_result=pars1+pars3;
      init_result=init1 + init2; 
      return result;
    }

    /**************** GENERaTE LPEmCRL **********************************/


    /// \brief Linearise a process indicated by procIdDecl.
    /// \details Returns a summand_list, the process parameters
    ///              and the initial assignment list.

    summand_list generateLPEmCRLterm(
                       const process_expression t,
                       const bool canterminate,
                       const bool regular,
                       const bool rename_variables,
                       variable_list &pars,
                       assignment_list &init)
    { if (is_process_instance(t))
      {
        summand_list t3=generateLPEmCRL(process_instance(t).identifier(),canterminate,regular,pars,init);
        long n=objectIndex(process_instance(t).identifier());
        data_expression_list args=process_instance(t).actual_parameters();
        init=substitute_assignmentlist(args,objectdata[n].parameters,init,pars,0,1);

        // Make the bound variables and parameters in this process unique.
        
        if ((objectdata[n].processstatus==GNF)||
            (objectdata[n].processstatus==pCRL)||
            (objectdata[n].processstatus==GNFalpha)||
            (objectdata[n].processstatus==multiAction))
        {
          t3=make_parameters_and_sum_variables_unique(t3,pars,init,std::string(objectdata[n].objectname));
        }
        else
        { if (rename_variables)
          { t3=make_parameters_and_sum_variables_unique(t3,pars,init);
          }
        }

        if (regular)
        { // We apply constant elimination on the obtained linear process.
          // In order to do so, we have to create a complete process specification first, as
          // this is what the interface of constelm requires.
          // Note that this is only useful, in regular mode. This does not make sense if
          // stacks are being used.
  
          linear_process lps(pars,deadlock_summand_vector(),action_summand_vector());
          lps.set_summands(t3);
          process_initializer initializer(init);
   
          specification temporary_spec(data,acts,global_variables,lps,initializer);
  
          constelm_algorithm < rewriter > alg(temporary_spec,rewr,core::gsVerbose);
          alg.run(true); // Remove constants from the specification, where global variables are
                         // also instantiated if they exist.
          // Reconstruct the variables from the temporary specification
  
          init=temporary_spec.initial_process().assignments();
          pars=temporary_spec.process().process_parameters();
         
          // Add all free variables in objectdata[n].parameters that are not already in the parameter list
          // and are not global variables to pars
  
          const std::set <variable> variable_list = data::find_free_variables(args);
          for(std::set <variable>::const_iterator i=variable_list.begin(); 
                 i!=variable_list.end(); ++i)
          { if (std::find(pars.begin(),pars.end(),*i)==pars.end() && // The free variable is not in pars
                global_variables.find(*i)==global_variables.end())   // and it is neither a glabal variable
            { pars=push_front(pars,*i);
            }
          }
  
          t3=summand_list();
          for(atermpp::vector < action_summand >::const_iterator i=temporary_spec.process().action_summands().begin();
                  i!=temporary_spec.process().action_summands().end(); ++i)
          { if (i->condition()!=sort_bool::false_())
            { t3=push_front(t3,summand_(i->summation_variables(),
                                      i->condition(),
                                      false, // Summand is a proper action summand, not a delta summand.
                                      i->multi_action().actions(),
                                      i->multi_action().has_time(),
                                      i->multi_action().time(),
                                      i->assignments()));
            }
          }
          for(atermpp::vector < deadlock_summand >::const_iterator i=temporary_spec.process().deadlock_summands().begin();
                  i!=temporary_spec.process().deadlock_summands().end(); ++i)
          { if (i->condition()!=sort_bool::false_())
            { t3=push_front(t3,summand_(i->summation_variables(),
                                      i->condition(),
                                      true,   // Summand is delta.
                                      action_list(),
                                      i->deadlock().has_time(),
                                      i->deadlock().time(),
                                      assignment_list()));
            }
          }
        }
        // Now constelm has been applied.

        return t3;
      }

      if (is_merge(t))
      { variable_list pars1,pars2;
        assignment_list init1,init2;
        const summand_list t1=generateLPEmCRLterm(process::merge(t).left(),canterminate,
                              regular,rename_variables,pars1,init1);
        const summand_list t2=generateLPEmCRLterm(process::merge(t).right(),canterminate,
                              regular,true,pars2,init2);
        summand_list t3=parallelcomposition(t1,pars1,init1,t2,pars2,init2,pars,init);
        return t3;
      }

      if (is_hide(t))
      {  const summand_list t2=generateLPEmCRLterm(hide(t).operand(),canterminate,
                              regular,rename_variables,pars,init);
         return hidecomposition(hide(t).hide_set(),t2);
      }

      if (is_allow(t))
      { const summand_list t2=generateLPEmCRLterm(allow(t).operand(),canterminate,
                              regular,rename_variables,pars,init);
        return allowblockcomposition(allow(t).allow_set(),t2,true);
      }

      if (is_block(t))
      { const summand_list t2=generateLPEmCRLterm(block(t).operand(),canterminate,
                              regular,rename_variables,pars,init);
        return allowblockcomposition(block(t).block_set(),t2,false);
      }

      if (is_rename(t))
      { const summand_list t2=generateLPEmCRLterm(process::rename(t).operand(),canterminate,
                              regular,rename_variables,pars,init);
        return renamecomposition(process::rename(t).rename_set(),t2);
      }

      if (is_comm(t))
      { const summand_list t1=generateLPEmCRLterm(comm(t).operand(),canterminate,
                              regular,rename_variables,pars,init);
        return communicationcomposition(comm(t).comm_set(),t1);
      }

      throw mcrl2::runtime_error("Internal error. Expect mCRL term " + pp(t) +".");
      return summand_list();
    }

    /**************** GENERaTE LPEmCRL **********************************/

    summand_list generateLPEmCRL(
                           const process_identifier procIdDecl,
                           const bool canterminate,
                           const bool regular,
                           variable_list &pars,
                           assignment_list &init)
    { /* If regular=1, then a regular version of the pCRL processes
         must be generated */

      long n=objectIndex(procIdDecl);

      if ((objectdata[n].processstatus==GNF)||
          (objectdata[n].processstatus==pCRL)||
          (objectdata[n].processstatus==GNFalpha)||
          (objectdata[n].processstatus==multiAction))
      {
        return generateLPEpCRL(procIdDecl,(canterminate&&objectdata[n].canterminate),
                                         objectdata[n].containstime,regular,pars,init);
      }
      /* process is a mCRLdone */
      if ((objectdata[n].processstatus==mCRLdone)||
                  (objectdata[n].processstatus==mCRLlin)||
                  (objectdata[n].processstatus==mCRL))
      { objectdata[n].processstatus=mCRLlin;
        return generateLPEmCRLterm(objectdata[n].processbody,
                        (canterminate&&objectdata[n].canterminate),
                         regular,false,pars,init);
      }

      throw mcrl2::runtime_error("laststatus: " + str(boost::format("%d") % objectdata[n].processstatus));
      return summand_list();
    }

    /**************** alphaconversion ********************************/

    process_expression alphaconversionterm(
                          const process_expression t,
                          const variable_list parameters,
                          const variable_list varlist,         // the variables varlist and tl must not be passed by reference.
                          const data_expression_list tl)
    { 
      if (is_choice(t))
      { return choice(
                  alphaconversionterm(choice(t).left(),parameters,varlist,tl),
                  alphaconversionterm(choice(t).right(),parameters,varlist,tl));
      }

      if (is_seq(t))
      { return seq(
                  alphaconversionterm(seq(t).left(),parameters,varlist,tl),
                  alphaconversionterm(seq(t).right(),parameters,varlist,tl));
      }

      if (is_sync(t))
      { return process::sync(
                  alphaconversionterm(process::sync(t).left(),parameters,varlist,tl),
                  alphaconversionterm(process::sync(t).right(),parameters,varlist,tl));
      }

      if (is_bounded_init(t))
      { return bounded_init(
                  alphaconversionterm(bounded_init(t).left(),parameters,varlist,tl),
                  alphaconversionterm(bounded_init(t).right(),parameters,varlist,tl));
      }

      if (is_merge(t))
      { alphaconversionterm(process::merge(t).left(),parameters,varlist,tl),
        alphaconversionterm(process::merge(t).right(),parameters,varlist,tl);
        return process_expression();
      }

      if (is_left_merge(t))
      { alphaconversionterm(left_merge(t).left(),parameters,varlist,tl),
        alphaconversionterm(left_merge(t).right(),parameters,varlist,tl);
        return process_expression();
      }

      if (is_at(t))
      {
        return at(alphaconversionterm(at(t).operand(),parameters,varlist,tl),
                  substitute_data(tl,varlist,data_expression(at(t).time_stamp())));
      }

      if (is_if_then(t))
      {
        return if_then(
                  substitute_data(tl,varlist,data_expression(if_then(t).condition())),
                  alphaconversionterm(if_then(t).then_case(),parameters,varlist,tl));
      }

      if (is_sum(t))
      { variable_list sumvars=sum(t).bound_variables();
        variable_list varlist1(varlist);
        data_expression_list tl1(tl);
        alphaconvert(sumvars,varlist1,tl1,variable_list(),parameters);
        const process_expression  result=sum(sumvars,alphaconversionterm(sum(t).operand(), sumvars+parameters,varlist1,tl1));
        return result;
      }

      if (is_process_instance(t))
      { const process_identifier procId=process_instance(t).identifier();
        alphaconversion(procId,parameters);
        return process_instance(procId,
                  substitute_datalist(tl,varlist,process_instance(t).actual_parameters()));
      }

      if (is_action(t))
      { return action(action(t).label(),
                      substitute_datalist(tl,varlist,action(t).arguments()));
      }

      if (is_delta(t)||
          is_tau(t))
      { return t;
      }

      if (is_hide(t))
      { alphaconversionterm(hide(t).operand(),parameters,varlist,tl);
        return process_expression();
      }

      if (is_rename(t))
      { alphaconversionterm(process::rename(t).operand(),parameters,varlist,tl);
        return process_expression();
      }

      if (is_comm(t))
      { alphaconversionterm(comm(t).operand(),parameters,varlist,tl);
        return process_expression();
      }

      if (is_allow(t))
      { alphaconversionterm(allow(t).operand(),parameters,varlist,tl);
        return process_expression();
      }

      if (is_block(t))
      { alphaconversionterm(block(t).operand(),parameters,varlist,tl);
        return process_expression();
      }

      throw mcrl2::runtime_error("unexpected process format in alphaconversionterm " + pp(t) +".");
      return process_expression();
    }

    void alphaconversion(const process_identifier procId, const variable_list parameters)
    {
      long n=objectIndex(procId);

      if ((objectdata[n].processstatus==GNF)||
          (objectdata[n].processstatus==multiAction))
      { objectdata[n].processstatus=GNFalpha;
        // tempvar below is needed as objectdata may be reallocated
        // during a call to alphaconversionterm.
        variable_list vars;
        data_expression_list dl;
        const process_expression tempvar=alphaconversionterm(objectdata[n].processbody,parameters,vars,dl);
        objectdata[n].processbody=tempvar;
      }
      else
      if (objectdata[n].processstatus==mCRLdone)
       { variable_list vars;
         data_expression_list dl;
         alphaconversionterm(objectdata[n].processbody,parameters,vars,dl);

       }
      else
      if (objectdata[n].processstatus==GNFalpha)
         return;
      else
      { throw mcrl2::runtime_error("unknown type " + str(boost::format("%d") % objectdata[n].processstatus) +
                                " in alphaconversion of " + pp(procId) +".");
      }
      return;
    }

    /***** determinewhetherprocessescontaintime; **********/

    bool containstimebody(
                  const process_expression t,
                  bool *stable,
                  atermpp::set < process_identifier > &visited,
                  bool allowrecursion,
                  bool &contains_if_then,
                  const bool print_info=false)
    { if (is_merge(t))
      { /* the construction below is needed to guarantee that
           both subterms are recursively investigated */
        bool r1=containstimebody(process::merge(t).left(),stable,visited,allowrecursion,contains_if_then,print_info);
        bool r2=containstimebody(process::merge(t).right(),stable,visited,allowrecursion,contains_if_then,print_info);
        return r1||r2;
      }

      if (is_process_instance(t))
      {
        if (allowrecursion)
        { return (containstime_rec(process_instance(t).identifier(),stable,visited,contains_if_then,print_info));
        }
        return objectdata[objectIndex(process_instance(t).identifier())].containstime;
      }

      if (is_process_instance_assignment(t))
      { if (allowrecursion)
        { return (containstime_rec(process_instance_assignment(t).identifier(),stable,visited,contains_if_then,print_info));
        }
        return objectdata[objectIndex(process_instance_assignment(t).identifier())].containstime;
      }

      if (is_hide(t))
      { return containstimebody(hide(t).operand(),stable,visited,allowrecursion,contains_if_then,print_info);
      }

      if (is_rename(t))
      { return containstimebody(process::rename(t).operand(),stable,visited,allowrecursion,contains_if_then,print_info);
      }

      if (is_allow(t))
      { return containstimebody(allow(t).operand(),stable,visited,allowrecursion,contains_if_then,print_info);
      }

      if (is_block(t))
      { return containstimebody(block(t).operand(),stable,visited,allowrecursion,contains_if_then,print_info);
      }

      if (is_comm(t))
      { return containstimebody(comm(t).operand(),stable,visited,allowrecursion,contains_if_then,print_info);
      }

      if (is_choice(t))
      { bool r1=containstimebody(choice(t).left(),stable,visited,allowrecursion,contains_if_then,print_info);
        bool r2=containstimebody(choice(t).right(),stable,visited,allowrecursion,contains_if_then,print_info);
        return r1||r2;
      }

      if (is_seq(t))
      { bool r1=containstimebody(seq(t).left(),stable,visited,allowrecursion,contains_if_then,print_info);
        bool r2=containstimebody(seq(t).right(),stable,visited,allowrecursion,contains_if_then,print_info);
        return r1||r2;
      }

      if (is_if_then(t))
      { contains_if_then=true;
        return true;
      }

      if (is_if_then_else(t))
      {
        bool r1=containstimebody(if_then_else(t).then_case(),stable,visited,allowrecursion,contains_if_then,print_info);
        bool r2=containstimebody(if_then_else(t).else_case(),stable,visited,allowrecursion,contains_if_then,print_info);
        return r1||r2;
      }

      if (is_sum(t))
      { return containstimebody(sum(t).operand(),stable,visited,allowrecursion,contains_if_then,print_info);
      }

      if (is_action(t)||
          is_delta(t)||
          is_tau(t))
      { return false;
      }

      if (is_at(t))
      { return true;
      }

      if (is_sync(t))
      { bool r1=containstimebody(process::sync(t).left(),stable,visited,allowrecursion,contains_if_then,print_info);
        bool r2=containstimebody(process::sync(t).right(),stable,visited,allowrecursion,contains_if_then,print_info);
        return r1||r2;
      }

      throw mcrl2::runtime_error("unexpected process format in containstime " + pp(t) +".");
      return false;
    }

    bool containstime_rec(
                  const process_identifier procId,
                  bool *stable,
                  atermpp::set < process_identifier > &visited,
                  bool &contains_if_then,
                  const bool print_info)
    { long n=objectIndex(procId);

      if (visited.count(procId)==0)
      { visited.insert(procId);
        bool ct=containstimebody(objectdata[n].processbody,stable,visited,1,contains_if_then,print_info);
        if ((ct) && !options.add_delta)
        { if (print_info)
          { if (core::gsVerbose)
            { std::cerr << "process " << procId.name() << " contains time.\n";
            }
          }
        }
        if (objectdata[n].containstime!=ct)
        { objectdata[n].containstime=ct;
          if (stable!=NULL)
          { *stable=false;
          }
        }
      }
      return (objectdata[n].containstime);
    }

    bool containstimebody(const process_expression t)
    { atermpp::set < process_identifier > visited;
      bool stable;
      bool contains_if_then;
      return containstimebody(t,&stable,visited,false,contains_if_then);
    }

    bool determinewhetherprocessescontaintime(const process_identifier procId)

    { /* This function sets for all reachable processes in the array objectdata
         whether they contain time in the field containstime. In verbose mode
         it prints the process variables that contain time. Furtermore, it returns
         whether there are processes that contain an if-then that will be translated
         to an if-then-else with an delta@0 in the else branch, introducing time */
      bool stable=0;
      bool print_info=true;
      bool contains_if_then=false;
      while (!stable)
      { atermpp::set < process_identifier > visited;
        stable=1;
        containstime_rec(procId,&stable,visited,contains_if_then,print_info);
        print_info=false;
      }
      return contains_if_then;
    }
    /***** determinewhetherprocessescanterminate(init); **********/

    bool canterminatebody(
                  const process_expression t,
                  bool &stable,
                  atermpp::set < process_identifier > &visited,
                  const bool allowrecursion)
    { if (is_merge(t))
      { /* the construction below is needed to guarantee that
           both subterms are recursively investigated */
        const bool r1=canterminatebody(process::merge(t).left(),stable,visited,allowrecursion);
        const bool r2=canterminatebody(process::merge(t).right(),stable,visited,allowrecursion);
        return r1&&r2;
      }

      if (is_process_instance(t))
      { if (allowrecursion)
        { return (canterminate_rec(process_instance(t).identifier(),stable,visited));
        }
        return objectdata[objectIndex(process_instance(t).identifier())].canterminate;
      }

      if (is_process_instance_assignment(t))
      { const process_instance_assignment u(t);
        if (allowrecursion)
        { return (canterminate_rec(u.identifier(),stable,visited));
        }
        return objectdata[objectIndex(u.identifier())].canterminate;
      }

      if (is_hide(t))
      { return (canterminatebody(hide(t).operand(),stable,visited,allowrecursion));
      }

      if (is_rename(t))
      { return (canterminatebody(process::rename(t).operand(),stable,visited,allowrecursion));
      }

      if (is_allow(t))
      { return (canterminatebody(allow(t).operand(),stable,visited,allowrecursion));
      }

      if (is_block(t))
      { return (canterminatebody(block(t).operand(),stable,visited,allowrecursion));
      }

      if (is_comm(t))
      { return (canterminatebody(comm(t).operand(),stable,visited,allowrecursion));
      }

      if (is_choice(t))
      { const bool r1=canterminatebody(choice(t).left(),stable,visited,allowrecursion);
        const bool r2=canterminatebody(choice(t).right(),stable,visited,allowrecursion);
        return r1||r2;
      }

      if (is_seq(t))
      { const bool r1=canterminatebody(seq(t).left(),stable,visited,allowrecursion);
        const bool r2=canterminatebody(seq(t).right(),stable,visited,allowrecursion);
        return r1&&r2;
      }

      if (is_if_then(t))
      { return canterminatebody(if_then(t).then_case(),stable,visited,allowrecursion);
      }

      if (is_if_then_else(t))
      { const bool r1=canterminatebody(if_then_else(t).then_case(),stable,visited,allowrecursion);
        const bool r2=canterminatebody(if_then_else(t).else_case(),stable,visited,allowrecursion);
        return r1||r2;
      }

      if (is_sum(t))
      { return (canterminatebody(sum(t).operand(),stable,visited,allowrecursion));
      }

      if (is_action(t))
      { return 1;
      }

      if (is_delta(t))
      { return 0;
      }

      if (is_tau(t))
      { return 1;
      }

      if (is_at(t))
      { return canterminatebody(at(t).operand(),stable,visited,allowrecursion);
      }

      if (is_sync(t))
      { const bool r1=canterminatebody(process::sync(t).left(),stable,visited,allowrecursion);
        const bool r2=canterminatebody(process::sync(t).right(),stable,visited,allowrecursion);
        return r1&&r2;
      }

      throw mcrl2::runtime_error("unexpected process format in canterminate " + pp(t) +".");
      return false;
    }

    bool canterminate_rec(
                  const process_identifier procId,
                  bool &stable,
                  atermpp::set < process_identifier > &visited)
    { long n=objectIndex(procId);

      if (visited.count(procId)==0)
      { visited.insert(procId);
        const bool ct=canterminatebody(objectdata[n].processbody,stable,visited,1);
        if (objectdata[n].canterminate!=ct)
        { objectdata[n].canterminate=ct;
          if (stable)
          { stable=false;
          }
        }
      }
      return (objectdata[n].canterminate);
    }

    bool canterminatebody(const process_expression t)
    { atermpp::set < process_identifier > visited;
      bool stable=false;
      return canterminatebody(t,stable,visited,false);
    }

    void determinewhetherprocessescanterminate(const process_identifier procId)
    { bool stable=false;
      while (!stable)
      { atermpp::set < process_identifier > visited;
        stable=true;
        canterminate_rec(procId,stable,visited);
      }
    }

    /*****  distinguishmCRLandpCRLprocsAndAddTerminatedAction  ******/

    process_identifier split_process(const process_identifier procId,
                                   atermpp::map < process_identifier,process_identifier > &visited_id,
                                   atermpp::map < process_expression,process_identifier > &visited_proc)
    { if (visited_id.count(procId)>0)
      { return visited_id[procId];
      }

      long n=objectIndex(procId);

      if ((objectdata[n].processstatus!=mCRL)&&
             (objectdata[n].canterminate==0))
      { /* no new process needs to be constructed */
        return procId;
      }

      const process_identifier newProcId(
                          fresh_name(procId.name()),
                          procId.sorts());

      visited_id[procId]=newProcId;

      if (objectdata[n].processstatus==mCRL)
      { insertProcDeclaration(
                    newProcId,
                    objectdata[n].parameters,
                    split_body(objectdata[n].processbody,
                               visited_id,visited_proc,
                               objectdata[n].parameters),
                    mCRL,0,false);
        return newProcId;
      }

      if (objectdata[n].canterminate)
      { insertProcDeclaration(
                    newProcId,
                    objectdata[n].parameters,
                    seq(objectdata[n].processbody, process_instance(terminatedProcId,data_expression_list())),
                    pCRL,canterminatebody(objectdata[n].processbody),
                         containstimebody(objectdata[n].processbody));
        return newProcId;
      }
      return procId;
    }

    process_expression split_body(
                        const process_expression t,
                        atermpp::map < process_identifier,process_identifier > &visited_id,
                        atermpp::map < process_expression,process_identifier > &visited_proc,
                        const variable_list parameters)
    { /* Replace pCRL process terms that occur in the scope of mCRL processes
         by a process identifier. E.g. (a+b)||c is replaced by X||c and
         a new process equation X=a+b is added. Furthermore, if the replaced
         process can terminate a termination action is put behind it.
         In the example X=(a+b).terminate.delta@0.

         Besides this each ProcessAssignment is transformed into a Process. */

      process_expression result;

      if (visited_proc.count(t)>0)
      return visited_proc[t];

      if (is_merge(t))
      { result=process::merge(
                    split_body(process::merge(t).left(),visited_id,visited_proc,parameters),
                    split_body(process::merge(t).right(),visited_id,visited_proc,parameters));
      }
      else
      if (is_process_instance(t))
      { result=process_instance(
                     split_process(process_instance(t).identifier(),visited_id,visited_proc),
                     process_instance(t).actual_parameters());
      }
      else
      if (is_process_instance_assignment(t))
      { const process_instance u=transform_process_assignment_to_process(t);
        result=process_instance(
                     split_process(u.identifier(),visited_id,visited_proc),
                     u.actual_parameters());
      }
      else
      if (is_hide(t))
      { result=hide(hide(t).hide_set(),
                     split_body(hide(t).operand(),visited_id,visited_proc,parameters));
      }
      else
      if (is_rename(t))
      { result=process::rename(
                     process::rename(t).rename_set(),
                     split_body(process::rename(t).operand(),visited_id,visited_proc,parameters));
      }
      else
      if (is_allow(t))
      { result=allow(allow(t).allow_set(),
                     split_body(allow(t).operand(),visited_id,visited_proc,parameters));
      }
      else
      if (is_block(t))
      { result=block(block(t).block_set(),
                     split_body(block(t).operand(),visited_id,visited_proc,parameters));
      }
      else
      if (is_comm(t))
      { result=comm(comm(t).comm_set(),
                    split_body(comm(t).operand(),visited_id,visited_proc,parameters));
      }
      else
      if (is_choice(t)||
          is_seq(t)||
          is_if_then_else(t)||
          is_if_then(t)||
          is_sum(t)||
          is_action(t)||
          is_delta(t)||
          is_tau(t)||
          is_at(t)||
          is_sync(t))
      { if (canterminatebody(t))
        { const process_identifier p=newprocess(parameters,
                                 seq(t,process_instance(terminatedProcId,data_expression_list())),
                                 pCRL,
                                 0,
                                 true);
          result=process_instance(p,objectdata[objectIndex(p)].parameters);
          visited_proc[t]=p;
        }
        else
        { const process_identifier p=newprocess(parameters,t,pCRL,0,true);
          result=process_instance(p,objectdata[objectIndex(p)].parameters);
          visited_proc[t]=p;
        }
      }
      else
      { throw mcrl2::runtime_error("unexpected process format in split process " + pp(t) +".");
      }

      return result;
    }

    process_identifier splitmCRLandpCRLprocsAndAddTerminatedAction(
                            const process_identifier procId)
    { atermpp::map < process_identifier,process_identifier> visited_id;
      atermpp::map < process_expression,process_identifier> visited_proc;
      return split_process(procId,visited_id,visited_proc);
    }

    /**************** AddTerminationActionIfNecessary ****************/

    void AddTerminationActionIfNecessary(const summand_list summands)
    { for (summand_list::const_iterator i=summands.begin(); i!=summands.end(); ++i)
      { const summand smd=*i;
        const action_list multiaction=smd.actions();
        if (multiaction==push_front(action_list(),terminationAction))
        { acts=push_front(acts,terminationAction.label());
          std::cerr << "The action " << pp(terminationAction) <<
                      " is added to signal termination of the linear process.\n";
          return;
        }
      }
    }

    /********************** SieveProcDataVars ***********************/
  public:
    variable_list SieveProcDataVarsSummands(
                            const atermpp::set <variable> &vars,
                            const summand_list summands,
                            const variable_list parameters)
    { /* In this routine it is checked which free variables
         in vars occur in the summands. Those variables
         that occur in the summands are returned. The
         parameters are needed to check occurrences of vars
         in the assignment list */

      // std::cerr << "INSIEVE1: " << pp(vars) << "\n";    
      atermpp::set < variable > vars_set(vars.begin(),vars.end());
      atermpp::set < variable > vars_result_set;

      for(summand_list::const_iterator smds=summands.begin();
                        smds!=summands.end(); ++smds)
      { const summand smd= *smds;

        if (!smd.is_delta())
        { filter_vars_by_multiaction(smd.actions(),vars_set,vars_result_set);
          filter_vars_by_assignmentlist(smd.assignments(),parameters,vars_set,vars_result_set);
        }
        if (smd.has_time())
        { filter_vars_by_term(smd.time(),vars_set,vars_result_set);
        }
        filter_vars_by_term(smd.condition(),vars_set,vars_result_set);
      }
      variable_list result;
      for(atermpp::set < variable >::reverse_iterator i=vars_result_set.rbegin();
               i!=vars_result_set.rend() ; i++)
      { result=push_front(result,*i);
      }

      // std::cerr << "OUTSIEVE1: " << pp(result) << "\n";    
      return result;
    }

  public:
    variable_list SieveProcDataVarsAssignments(
                            const atermpp::set <variable> &vars,
                            const assignment_list assignments,
                            const variable_list parameters)
    { // std::cerr << "INSIEVE: " << pp(vars) << "\n";
      const atermpp::set < variable > vars_set(vars.begin(),vars.end());
      atermpp::set < variable > vars_result_set;


      filter_vars_by_assignmentlist(assignments,parameters,vars_set,vars_result_set);

      variable_list result;
      for(atermpp::set < variable >::reverse_iterator i=vars_result_set.rbegin();
               i!=vars_result_set.rend() ; i++)
      { result=push_front(result,*i);
      }

      // std::cerr << "OUTSIEVE: " << pp(result) << "\n";
      return result;
    }

    /**************** transform **************************************/
 public:
    summand_list transform(
                         const process_identifier init,
                         variable_list &parameters,
                         assignment_list &initial_state)
    { /* Then select the BPA processes, and check that the others
         are proper parallel processes */
      determine_process_status(init,mCRL);
      determinewhetherprocessescanterminate(init);
      const process_identifier init1=splitmCRLandpCRLprocsAndAddTerminatedAction(init);
      if (determinewhetherprocessescontaintime(init1) && !(options.add_delta))
      { std::cerr << "Warning: specification contains time due to translating c->p to c->p<>delta@0. Use SQuADT option `Add delta summands' or command line option `-D' to suppress.\n";
      }
      atermpp::vector <process_identifier> pcrlprocesslist;
      collectPcrlProcesses(init1,pcrlprocesslist);
      if (pcrlprocesslist.size()==0)
      { throw mcrl2::runtime_error("there are no pCRL processes to be linearized");
        // Note that this can occur with a specification
        // proc P(x:Int) = P(x); init P(1);
      }

      /* Second, transform into GNF with possibly variables as a head,
         but no actions in the tail */
      procstovarheadGNF(pcrlprocesslist);

      /* Third, transform to GNF by subsitution, such that the
         first variable in a sequence is always an actionvariable */
      procstorealGNF(init1,options.lin_method!=lmStack);

      summand_list t3=generateLPEmCRL(init1,objectdata[objectIndex(init1)].canterminate,
                                 options.lin_method!=lmStack,parameters,initial_state);
      t3=allowblockcomposition(action_name_multiset_list(),t3,false); // This removes superfluous delta summands.
      if (options.final_cluster)
         t3=cluster_actions(t3,parameters);

      AddTerminationActionIfNecessary(t3);
      return t3;
    }

    /* normalise the sorts in action_label_lists */
    action_label_list normalise_sorts_action_labels(
                const action_label_list &l,
                const data_specification &data) const
    { action_label_list result;
      for(action_label_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      { result=push_back(result,action_label(i->name(),data.normalise_sorts(i->sorts())));
      }
      return reverse(result);
    } 

    action_list normalise_sorts(
                const action_list &l,
                const data_specification &data)
    { action_list result;
      for(action_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      { const action_label label(i->label());
        result=push_back(result,
                  action(action_label(label.name(),data.normalise_sorts(label.sorts())),
                         data.normalise_sorts(i->arguments()))
                 );
      }
      return reverse(result);
    }
  
    summand_list normalise_sorts(
                const summand_list &l,
                const data_specification &data)
    { summand_list result;
      for(summand_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      { result=push_back(result,
                    summand_(data.normalise_sorts(i->summation_variables()),
                             data.normalise_sorts(i->condition()),
                             i->is_delta(),
                             (i->is_delta()?i->actions():normalise_sorts(i->actions(),data)),
                             i->has_time(),
                             (i->has_time()?data.normalise_sorts(i->time()):i->time()),
                             (i->is_delta()?i->assignments():data.normalise_sorts(i->assignments()))));
      }
      return reverse(result);
    }

    
}; // End of the class specification basictype

/**************** linearise **************************************/

mcrl2::lps::specification mcrl2::lps::linearise(
                 const mcrl2::process::process_specification& type_checked_spec,
                 mcrl2::lps::t_lin_options lin_options)
{ 
  if (core::gsVerbose)
  { std::cerr << "Linearising the process specification using the '" + lin_method_to_string(lin_options.lin_method) + " ' method.\n"; 
  }
  data_specification data_spec=type_checked_spec.data();
  internal_format_conversion(data_spec);
  // data_spec.make_complete(type_checked_spec);  // Take care that all sorts occuring in the specification 
                                                  // are present in data_spec. As this does not work, we resort
                                                  // to the construction below.
  std::set<data::sort_expression> s;
  process::traverse_sort_expressions(type_checked_spec.action_labels(), std::inserter(s, s.end()));
  process::traverse_sort_expressions(type_checked_spec.equations(), std::inserter(s, s.end()));
  process::traverse_sort_expressions(type_checked_spec.init(), std::inserter(s, s.end()));
  s.insert(sort_real::real_());
  data_spec.make_complete(s);
  //data_spec.make_complete(type_checked_spec.action_labels());
  //data_spec.make_complete(type_checked_spec.equations());
  //data_spec.make_complete(type_checked_spec.init());
  //data_spec.make_complete(sort_real::real_()); // Take care essential rewrite rules are present.

  specification_basic_type spec(type_checked_spec.action_labels(),
                                type_checked_spec.equations(),
                                action_label_list(data::convert<data::variable_list>(type_checked_spec.global_variables())),
                                data_spec,
                                type_checked_spec.global_variables(),
                                lin_options);
  process_identifier init=spec.storeinit(type_checked_spec.init());

  //linearise spec
  variable_list parameters;
  assignment_list initial_state;
  const summand_list result = spec.transform(init,parameters,initial_state);

  // compute global variables
  data::variable_list globals1 = spec.data.normalise_sorts(spec.SieveProcDataVarsSummands(spec.global_variables,result,parameters));
  data::variable_list globals2 = spec.data.normalise_sorts(spec.SieveProcDataVarsAssignments(spec.global_variables,initial_state,parameters));
  atermpp::set<data::variable> global_variables;
  global_variables.insert(globals1.begin(), globals1.end());
  global_variables.insert(globals2.begin(), globals2.end());

  linear_process lps(spec.data.normalise_sorts(parameters),  
                     deadlock_summand_vector(),
                     action_summand_vector());
  lps.set_summands(spec.normalise_sorts(result,spec.data));

  lps::specification spec1(
              spec.data,
              spec.normalise_sorts_action_labels(spec.acts,spec.data),
              global_variables,
              lps,
              process_initializer(spec.data.normalise_sorts(initial_state)));

  // add missing sorts to the data specification
  lps::complete_data_specification(spec1);

  return spec1;
}
