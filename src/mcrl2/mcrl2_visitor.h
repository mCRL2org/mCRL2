// ======================================================================
//
// Copyright (c) 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : mcrl2_visitor.h
// date          : 07/14/05
// version       : 0.1
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef MCRL2_VISITOR_H
#define MCRL2_VISITOR_H

#include <string>

#include "atermpp/aterm.h"
#include "mcrl2/mcrl2_error.h"

namespace mcrl2
{

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::aterm_list;

//---------------------------------------------------------//
//                     mcrl2_visitor
//---------------------------------------------------------//
class mcrl2_visitor
{
 protected:
    typedef void (mcrl2_visitor::*aterm_appl_function)(aterm_appl);

    /**
      * Applies member function f to all elements of list t.
      */
    void walk_list(aterm_list t, aterm_appl_function f)
    {
      assert(t.type() == AT_LIST);
      for (aterm_list::iterator i = t.begin(); i != t.end(); ++i)
        (this->*f)((*i).to_appl());
    }

 public:
    mcrl2_visitor()
    {}

    virtual ~mcrl2_visitor()
    {}

//--- begin visitors
    virtual void visit_Spec(aterm_appl t) {}
    virtual void leave_Spec(aterm_appl t) {}
    virtual void visit_SortSpec(aterm_appl t) {}
    virtual void leave_SortSpec(aterm_appl t) {}
    virtual void visit_ConsSpec(aterm_appl t) {}
    virtual void leave_ConsSpec(aterm_appl t) {}
    virtual void visit_MapSpec(aterm_appl t) {}
    virtual void leave_MapSpec(aterm_appl t) {}
    virtual void visit_DataEqnSpec(aterm_appl t) {}
    virtual void leave_DataEqnSpec(aterm_appl t) {}
    virtual void visit_ActSpec(aterm_appl t) {}
    virtual void leave_ActSpec(aterm_appl t) {}
    virtual void visit_SortDecl(aterm_appl t) {}
    virtual void leave_SortDecl(aterm_appl t) {}
    virtual void visit_SortId(aterm_appl t) {}
    virtual void leave_SortId(aterm_appl t) {}
    virtual void visit_SortRef(aterm_appl t) {}
    virtual void leave_SortRef(aterm_appl t) {}
    virtual void visit_OpId(aterm_appl t) {}
    virtual void leave_OpId(aterm_appl t) {}
    virtual void visit_DataEqn(aterm_appl t) {}
    virtual void leave_DataEqn(aterm_appl t) {}
    virtual void visit_DataVarId(aterm_appl t) {}
    virtual void leave_DataVarId(aterm_appl t) {}
    virtual void visit_DataExprOrNil(aterm_appl t) {}
    virtual void leave_DataExprOrNil(aterm_appl t) {}
    virtual void visit_ActId(aterm_appl t) {}
    virtual void leave_ActId(aterm_appl t) {}
    virtual void visit_ProcEqnSpec(aterm_appl t) {}
    virtual void leave_ProcEqnSpec(aterm_appl t) {}
    virtual void visit_ProcEqn(aterm_appl t) {}
    virtual void leave_ProcEqn(aterm_appl t) {}
    virtual void visit_ProcVarId(aterm_appl t) {}
    virtual void leave_ProcVarId(aterm_appl t) {}
    virtual void visit_LPESummand(aterm_appl t) {}
    virtual void leave_LPESummand(aterm_appl t) {}
    virtual void visit_MultActOrDelta(aterm_appl t) {}
    virtual void leave_MultActOrDelta(aterm_appl t) {}
    virtual void visit_Action(aterm_appl t) {}
    virtual void leave_Action(aterm_appl t) {}
    virtual void visit_Assignment(aterm_appl t) {}
    virtual void leave_Assignment(aterm_appl t) {}
    virtual void visit_Init(aterm_appl t) {}
    virtual void leave_Init(aterm_appl t) {}
    virtual void visit_SortExpr(aterm_appl t) {}
    virtual void leave_SortExpr(aterm_appl t) {}
    virtual void visit_StructCons(aterm_appl t) {}
    virtual void leave_StructCons(aterm_appl t) {}
    virtual void visit_StructProj(aterm_appl t) {}
    virtual void leave_StructProj(aterm_appl t) {}
    virtual void visit_StringOrNil(aterm_appl t) {}
    virtual void leave_StringOrNil(aterm_appl t) {}
    virtual void visit_DataExpr(aterm_appl t) {}
    virtual void leave_DataExpr(aterm_appl t) {}
    virtual void visit_SortExprOrUnknown(aterm_appl t) {}
    virtual void leave_SortExprOrUnknown(aterm_appl t) {}
    virtual void visit_BagEnumElt(aterm_appl t) {}
    virtual void leave_BagEnumElt(aterm_appl t) {}
    virtual void visit_WhrDecl(aterm_appl t) {}
    virtual void leave_WhrDecl(aterm_appl t) {}
    virtual void visit_ProcExpr(aterm_appl t) {}
    virtual void leave_ProcExpr(aterm_appl t) {}
    virtual void visit_MultActName(aterm_appl t) {}
    virtual void leave_MultActName(aterm_appl t) {}
    virtual void visit_RenameExpr(aterm_appl t) {}
    virtual void leave_RenameExpr(aterm_appl t) {}
    virtual void visit_CommExpr(aterm_appl t) {}
    virtual void leave_CommExpr(aterm_appl t) {}
    virtual void visit_String(aterm_appl t) {}
    virtual void leave_String(aterm_appl t) {}
    virtual void visit_NumberString(aterm_appl t) {}
    virtual void leave_NumberString(aterm_appl t) {}
    virtual void visit_SpecV1(aterm_appl t) {}
    virtual void leave_SpecV1(aterm_appl t) {}
    virtual void visit_DataVarIdOpId(aterm_appl t) {}
    virtual void leave_DataVarIdOpId(aterm_appl t) {}
    virtual void visit_DataApplProd(aterm_appl t) {}
    virtual void leave_DataApplProd(aterm_appl t) {}
    virtual void visit_DataAppl(aterm_appl t) {}
    virtual void leave_DataAppl(aterm_appl t) {}
    virtual void visit_Number(aterm_appl t) {}
    virtual void leave_Number(aterm_appl t) {}
    virtual void visit_ListEnum(aterm_appl t) {}
    virtual void leave_ListEnum(aterm_appl t) {}
    virtual void visit_SetEnum(aterm_appl t) {}
    virtual void leave_SetEnum(aterm_appl t) {}
    virtual void visit_BagEnum(aterm_appl t) {}
    virtual void leave_BagEnum(aterm_appl t) {}
    virtual void visit_SetBagComp(aterm_appl t) {}
    virtual void leave_SetBagComp(aterm_appl t) {}
    virtual void visit_Forall(aterm_appl t) {}
    virtual void leave_Forall(aterm_appl t) {}
    virtual void visit_Exists(aterm_appl t) {}
    virtual void leave_Exists(aterm_appl t) {}
    virtual void visit_Lambda(aterm_appl t) {}
    virtual void leave_Lambda(aterm_appl t) {}
    virtual void visit_Whr(aterm_appl t) {}
    virtual void leave_Whr(aterm_appl t) {}
    virtual void visit_Nil(aterm_appl t) {}
    virtual void leave_Nil(aterm_appl t) {}
    virtual void visit_LPE(aterm_appl t) {}
    virtual void leave_LPE(aterm_appl t) {}
    virtual void visit_MultAct(aterm_appl t) {}
    virtual void leave_MultAct(aterm_appl t) {}
    virtual void visit_Delta(aterm_appl t) {}
    virtual void leave_Delta(aterm_appl t) {}
    virtual void visit_LPEInit(aterm_appl t) {}
    virtual void leave_LPEInit(aterm_appl t) {}
    virtual void visit_SortList(aterm_appl t) {}
    virtual void leave_SortList(aterm_appl t) {}
    virtual void visit_SortSet(aterm_appl t) {}
    virtual void leave_SortSet(aterm_appl t) {}
    virtual void visit_SortBag(aterm_appl t) {}
    virtual void leave_SortBag(aterm_appl t) {}
    virtual void visit_SortStruct(aterm_appl t) {}
    virtual void leave_SortStruct(aterm_appl t) {}
    virtual void visit_SortArrowProd(aterm_appl t) {}
    virtual void leave_SortArrowProd(aterm_appl t) {}
    virtual void visit_SortArrow(aterm_appl t) {}
    virtual void leave_SortArrow(aterm_appl t) {}
    virtual void visit_Unknown(aterm_appl t) {}
    virtual void leave_Unknown(aterm_appl t) {}
    virtual void visit_ActionProcess(aterm_appl t) {}
    virtual void leave_ActionProcess(aterm_appl t) {}
    virtual void visit_Process(aterm_appl t) {}
    virtual void leave_Process(aterm_appl t) {}
    virtual void visit_Tau(aterm_appl t) {}
    virtual void leave_Tau(aterm_appl t) {}
    virtual void visit_Sum(aterm_appl t) {}
    virtual void leave_Sum(aterm_appl t) {}
    virtual void visit_Restrict(aterm_appl t) {}
    virtual void leave_Restrict(aterm_appl t) {}
    virtual void visit_Hide(aterm_appl t) {}
    virtual void leave_Hide(aterm_appl t) {}
    virtual void visit_Rename(aterm_appl t) {}
    virtual void leave_Rename(aterm_appl t) {}
    virtual void visit_Comm(aterm_appl t) {}
    virtual void leave_Comm(aterm_appl t) {}
    virtual void visit_Allow(aterm_appl t) {}
    virtual void leave_Allow(aterm_appl t) {}
    virtual void visit_Sync(aterm_appl t) {}
    virtual void leave_Sync(aterm_appl t) {}
    virtual void visit_AtTime(aterm_appl t) {}
    virtual void leave_AtTime(aterm_appl t) {}
    virtual void visit_Seq(aterm_appl t) {}
    virtual void leave_Seq(aterm_appl t) {}
    virtual void visit_Cond(aterm_appl t) {}
    virtual void leave_Cond(aterm_appl t) {}
    virtual void visit_BInit(aterm_appl t) {}
    virtual void leave_BInit(aterm_appl t) {}
    virtual void visit_Merge(aterm_appl t) {}
    virtual void leave_Merge(aterm_appl t) {}
    virtual void visit_LMerge(aterm_appl t) {}
    virtual void leave_LMerge(aterm_appl t) {}
    virtual void visit_Choice(aterm_appl t) {}
    virtual void leave_Choice(aterm_appl t) {}

    void walk_Spec(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Spec");
      aterm_list l = t.argument_list();
      assert(l.size() == 7);
      aterm_list::iterator i = l.begin();

      visit_Spec(t);
      walk_SortSpec((*i++).to_appl());
      walk_ConsSpec((*i++).to_appl());
      walk_MapSpec((*i++).to_appl());
      walk_DataEqnSpec((*i++).to_appl());
      walk_ActSpec((*i++).to_appl());
      walk_ProcEqnSpec((*i++).to_appl());
      walk_Init((*i++).to_appl());
      leave_Spec(t);
    }

    void walk_SortSpec(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SortSpec");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_SortSpec(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_SortDecl);
      leave_SortSpec(t);
    }

    void walk_ConsSpec(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "ConsSpec");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_ConsSpec(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_OpId);
      leave_ConsSpec(t);
    }

    void walk_MapSpec(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "MapSpec");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_MapSpec(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_OpId);
      leave_MapSpec(t);
    }

    void walk_DataEqnSpec(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "DataEqnSpec");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_DataEqnSpec(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataEqn);
      leave_DataEqnSpec(t);
    }

    void walk_ActSpec(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "ActSpec");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_ActSpec(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_ActId);
      leave_ActSpec(t);
    }

    void walk_SortDecl(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      if (t.function().name() == "SortId")
        walk_SortId(t);
      else if (t.function().name() == "SortRef")
        walk_SortRef(t);
      else
        mcrl2_error(std::string("Found unknown term: ") + t.function().name());
    }

    void walk_SortId(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SortId");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_SortId(t);
      walk_String((*i++).to_appl());
      leave_SortId(t);
    }

    void walk_SortRef(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SortRef");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_SortRef(t);
      walk_String((*i++).to_appl());
      walk_SortExpr((*i++).to_appl());
      leave_SortRef(t);
    }

    void walk_OpId(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "OpId");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_OpId(t);
      walk_String((*i++).to_appl());
      walk_SortExpr((*i++).to_appl());
      leave_OpId(t);
    }

    void walk_DataEqn(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "DataEqn");
      aterm_list l = t.argument_list();
      assert(l.size() == 4);
      aterm_list::iterator i = l.begin();

      visit_DataEqn(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_DataExprOrNil((*i++).to_appl());
      walk_DataExpr((*i++).to_appl());
      walk_DataExpr((*i++).to_appl());
      leave_DataEqn(t);
    }

    void walk_DataVarId(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "DataVarId");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_DataVarId(t);
      walk_String((*i++).to_appl());
      walk_SortExpr((*i++).to_appl());
      leave_DataVarId(t);
    }

    void walk_DataExprOrNil(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      if (t.function().name() == "DataVarId")
        walk_DataVarId(t);
      else if (t.function().name() == "OpId")
        walk_OpId(t);
      else if (t.function().name() == "DataVarIdOpId")
        walk_DataVarIdOpId(t);
      else if (t.function().name() == "DataApplProd")
        walk_DataApplProd(t);
      else if (t.function().name() == "DataAppl")
        walk_DataAppl(t);
      else if (t.function().name() == "Number")
        walk_Number(t);
      else if (t.function().name() == "ListEnum")
        walk_ListEnum(t);
      else if (t.function().name() == "SetEnum")
        walk_SetEnum(t);
      else if (t.function().name() == "BagEnum")
        walk_BagEnum(t);
      else if (t.function().name() == "SetBagComp")
        walk_SetBagComp(t);
      else if (t.function().name() == "Forall")
        walk_Forall(t);
      else if (t.function().name() == "Exists")
        walk_Exists(t);
      else if (t.function().name() == "Lambda")
        walk_Lambda(t);
      else if (t.function().name() == "Whr")
        walk_Whr(t);
      else if (t.function().name() == "Nil")
        walk_Nil(t);
      else
        mcrl2_error(std::string("Found unknown term: ") + t.function().name());
    }

    void walk_ActId(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "ActId");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_ActId(t);
      walk_String((*i++).to_appl());
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_SortExpr);
      leave_ActId(t);
    }

    void walk_ProcEqnSpec(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      if (t.function().name() == "ProcEqnSpec")
        walk_ProcEqnSpec(t);
      else if (t.function().name() == "LPE")
        walk_LPE(t);
      else
        mcrl2_error(std::string("Found unknown term: ") + t.function().name());
    }

    void walk_ProcEqn(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "ProcEqn");
      aterm_list l = t.argument_list();
      assert(l.size() == 4);
      aterm_list::iterator i = l.begin();

      visit_ProcEqn(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_ProcVarId((*i++).to_appl());
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_ProcExpr((*i++).to_appl());
      leave_ProcEqn(t);
    }

    void walk_ProcVarId(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "ProcVarId");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_ProcVarId(t);
      walk_String((*i++).to_appl());
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_SortExpr);
      leave_ProcVarId(t);
    }

    void walk_LPESummand(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "LPESummand");
      aterm_list l = t.argument_list();
      assert(l.size() == 5);
      aterm_list::iterator i = l.begin();

      visit_LPESummand(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_DataExpr((*i++).to_appl());
      walk_MultActOrDelta((*i++).to_appl());
      walk_DataExprOrNil((*i++).to_appl());
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_Assignment);
      leave_LPESummand(t);
    }

    void walk_MultActOrDelta(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      if (t.function().name() == "MultAct")
        walk_MultAct(t);
      else if (t.function().name() == "Delta")
        walk_Delta(t);
      else
        mcrl2_error(std::string("Found unknown term: ") + t.function().name());
    }

    void walk_Action(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Action");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Action(t);
      walk_ActId((*i++).to_appl());
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataExpr);
      leave_Action(t);
    }

    void walk_Assignment(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Assignment");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Assignment(t);
      walk_DataVarId((*i++).to_appl());
      walk_DataExpr((*i++).to_appl());
      leave_Assignment(t);
    }

    void walk_Init(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      if (t.function().name() == "Init")
        walk_Init(t);
      else if (t.function().name() == "LPEInit")
        walk_LPEInit(t);
      else
        mcrl2_error(std::string("Found unknown term: ") + t.function().name());
    }

    void walk_SortExpr(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      if (t.function().name() == "SortId")
        walk_SortId(t);
      else if (t.function().name() == "SortList")
        walk_SortList(t);
      else if (t.function().name() == "SortSet")
        walk_SortSet(t);
      else if (t.function().name() == "SortBag")
        walk_SortBag(t);
      else if (t.function().name() == "SortStruct")
        walk_SortStruct(t);
      else if (t.function().name() == "SortArrowProd")
        walk_SortArrowProd(t);
      else if (t.function().name() == "SortArrow")
        walk_SortArrow(t);
      else
        mcrl2_error(std::string("Found unknown term: ") + t.function().name());
    }

    void walk_StructCons(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "StructCons");
      aterm_list l = t.argument_list();
      assert(l.size() == 3);
      aterm_list::iterator i = l.begin();

      visit_StructCons(t);
      walk_String((*i++).to_appl());
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_StructProj);
      walk_StringOrNil((*i++).to_appl());
      leave_StructCons(t);
    }

    void walk_StructProj(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "StructProj");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_StructProj(t);
      walk_StringOrNil((*i++).to_appl());
      walk_SortExpr((*i++).to_appl());
      leave_StructProj(t);
    }

    void walk_StringOrNil(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      if (t.function().name() == "Nil")
        walk_Nil(t);
      else if (t.function().name() == "String")
        walk_String(t);
    }

    void walk_DataExpr(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      if (t.function().name() == "DataVarId")
        walk_DataVarId(t);
      else if (t.function().name() == "OpId")
        walk_OpId(t);
      else if (t.function().name() == "DataVarIdOpId")
        walk_DataVarIdOpId(t);
      else if (t.function().name() == "DataApplProd")
        walk_DataApplProd(t);
      else if (t.function().name() == "DataAppl")
        walk_DataAppl(t);
      else if (t.function().name() == "Number")
        walk_Number(t);
      else if (t.function().name() == "ListEnum")
        walk_ListEnum(t);
      else if (t.function().name() == "SetEnum")
        walk_SetEnum(t);
      else if (t.function().name() == "BagEnum")
        walk_BagEnum(t);
      else if (t.function().name() == "SetBagComp")
        walk_SetBagComp(t);
      else if (t.function().name() == "Forall")
        walk_Forall(t);
      else if (t.function().name() == "Exists")
        walk_Exists(t);
      else if (t.function().name() == "Lambda")
        walk_Lambda(t);
      else if (t.function().name() == "Whr")
        walk_Whr(t);
      else
        mcrl2_error(std::string("Found unknown term: ") + t.function().name());
    }

    void walk_SortExprOrUnknown(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      if (t.function().name() == "SortId")
        walk_SortId(t);
      else if (t.function().name() == "SortList")
        walk_SortList(t);
      else if (t.function().name() == "SortSet")
        walk_SortSet(t);
      else if (t.function().name() == "SortBag")
        walk_SortBag(t);
      else if (t.function().name() == "SortStruct")
        walk_SortStruct(t);
      else if (t.function().name() == "SortArrowProd")
        walk_SortArrowProd(t);
      else if (t.function().name() == "SortArrow")
        walk_SortArrow(t);
      else if (t.function().name() == "Unknown")
        walk_Unknown(t);
      else
        mcrl2_error(std::string("Found unknown term: ") + t.function().name());
    }

    void walk_BagEnumElt(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "BagEnumElt");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_BagEnumElt(t);
      walk_DataExpr((*i++).to_appl());
      walk_DataExpr((*i++).to_appl());
      leave_BagEnumElt(t);
    }

    void walk_WhrDecl(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "WhrDecl");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_WhrDecl(t);
      walk_String((*i++).to_appl());
      walk_DataExpr((*i++).to_appl());
      leave_WhrDecl(t);
    }

    void walk_ProcExpr(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      if (t.function().name() == "Action")
        walk_Action(t);
      else if (t.function().name() == "ActionProcess")
        walk_ActionProcess(t);
      else if (t.function().name() == "Process")
        walk_Process(t);
      else if (t.function().name() == "Delta")
        walk_Delta(t);
      else if (t.function().name() == "Tau")
        walk_Tau(t);
      else if (t.function().name() == "Sum")
        walk_Sum(t);
      else if (t.function().name() == "Restrict")
        walk_Restrict(t);
      else if (t.function().name() == "Hide")
        walk_Hide(t);
      else if (t.function().name() == "Rename")
        walk_Rename(t);
      else if (t.function().name() == "Comm")
        walk_Comm(t);
      else if (t.function().name() == "Allow")
        walk_Allow(t);
      else if (t.function().name() == "Sync")
        walk_Sync(t);
      else if (t.function().name() == "AtTime")
        walk_AtTime(t);
      else if (t.function().name() == "Seq")
        walk_Seq(t);
      else if (t.function().name() == "Cond")
        walk_Cond(t);
      else if (t.function().name() == "BInit")
        walk_BInit(t);
      else if (t.function().name() == "Merge")
        walk_Merge(t);
      else if (t.function().name() == "LMerge")
        walk_LMerge(t);
      else if (t.function().name() == "Choice")
        walk_Choice(t);
      else
        mcrl2_error(std::string("Found unknown term: ") + t.function().name());
    }

    void walk_MultActName(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "MultActName");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_MultActName(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_String);
      leave_MultActName(t);
    }

    void walk_RenameExpr(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "RenameExpr");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_RenameExpr(t);
      walk_String((*i++).to_appl());
      walk_String((*i++).to_appl());
      leave_RenameExpr(t);
    }

    void walk_CommExpr(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "CommExpr");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_CommExpr(t);
      walk_MultActName((*i++).to_appl());
      walk_StringOrNil((*i++).to_appl());
      leave_CommExpr(t);
    }

    void walk_String(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.is_quoted());
      aterm_list l = t.argument_list();
      assert(l.size() == 0);
      aterm_list::iterator i = l.begin();

      visit_String(t);
      
      leave_String(t);
    }

    void walk_NumberString(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.is_quoted());
      aterm_list l = t.argument_list();
      assert(l.size() == 0);
      aterm_list::iterator i = l.begin();

      visit_NumberString(t);
      
      leave_NumberString(t);
    }

    void walk_SpecV1(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SpecV1");
      aterm_list l = t.argument_list();
      assert(l.size() == 7);
      aterm_list::iterator i = l.begin();

      visit_SpecV1(t);
      walk_SortSpec((*i++).to_appl());
      walk_ConsSpec((*i++).to_appl());
      walk_MapSpec((*i++).to_appl());
      walk_DataEqnSpec((*i++).to_appl());
      walk_ActSpec((*i++).to_appl());
      walk_ProcEqnSpec((*i++).to_appl());
      walk_Init((*i++).to_appl());
      leave_SpecV1(t);
    }

    void walk_DataVarIdOpId(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "DataVarIdOpId");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_DataVarIdOpId(t);
      walk_String((*i++).to_appl());
      leave_DataVarIdOpId(t);
    }

    void walk_DataApplProd(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "DataApplProd");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_DataApplProd(t);
      walk_DataExpr((*i++).to_appl());
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataExpr);
      leave_DataApplProd(t);
    }

    void walk_DataAppl(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "DataAppl");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_DataAppl(t);
      walk_DataExpr((*i++).to_appl());
      walk_DataExpr((*i++).to_appl());
      leave_DataAppl(t);
    }

    void walk_Number(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Number");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Number(t);
      walk_NumberString((*i++).to_appl());
      walk_SortExprOrUnknown((*i++).to_appl());
      leave_Number(t);
    }

    void walk_ListEnum(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "ListEnum");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_ListEnum(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataExpr);
      walk_SortExprOrUnknown((*i++).to_appl());
      leave_ListEnum(t);
    }

    void walk_SetEnum(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SetEnum");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_SetEnum(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataExpr);
      walk_SortExprOrUnknown((*i++).to_appl());
      leave_SetEnum(t);
    }

    void walk_BagEnum(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "BagEnum");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_BagEnum(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_BagEnumElt);
      walk_SortExprOrUnknown((*i++).to_appl());
      leave_BagEnum(t);
    }

    void walk_SetBagComp(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SetBagComp");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_SetBagComp(t);
      walk_DataVarId((*i++).to_appl());
      walk_DataExpr((*i++).to_appl());
      leave_SetBagComp(t);
    }

    void walk_Forall(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Forall");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Forall(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_DataExpr((*i++).to_appl());
      leave_Forall(t);
    }

    void walk_Exists(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Exists");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Exists(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_DataExpr((*i++).to_appl());
      leave_Exists(t);
    }

    void walk_Lambda(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Lambda");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Lambda(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_DataExpr((*i++).to_appl());
      leave_Lambda(t);
    }

    void walk_Whr(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Whr");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Whr(t);
      walk_DataExpr((*i++).to_appl());
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_WhrDecl);
      leave_Whr(t);
    }

    void walk_Nil(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Nil");
      aterm_list l = t.argument_list();
      assert(l.size() == 0);
      aterm_list::iterator i = l.begin();

      visit_Nil(t);
      
      leave_Nil(t);
    }

    void walk_LPE(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "LPE");
      aterm_list l = t.argument_list();
      assert(l.size() == 3);
      aterm_list::iterator i = l.begin();

      visit_LPE(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_LPESummand);
      leave_LPE(t);
    }

    void walk_MultAct(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "MultAct");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_MultAct(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_Action);
      leave_MultAct(t);
    }

    void walk_Delta(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Delta");
      aterm_list l = t.argument_list();
      assert(l.size() == 0);
      aterm_list::iterator i = l.begin();

      visit_Delta(t);
      
      leave_Delta(t);
    }

    void walk_LPEInit(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "LPEInit");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_LPEInit(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_Assignment);
      leave_LPEInit(t);
    }

    void walk_SortList(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SortList");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_SortList(t);
      walk_SortExpr((*i++).to_appl());
      leave_SortList(t);
    }

    void walk_SortSet(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SortSet");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_SortSet(t);
      walk_SortExpr((*i++).to_appl());
      leave_SortSet(t);
    }

    void walk_SortBag(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SortBag");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_SortBag(t);
      walk_SortExpr((*i++).to_appl());
      leave_SortBag(t);
    }

    void walk_SortStruct(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SortStruct");
      aterm_list l = t.argument_list();
      assert(l.size() == 1);
      aterm_list::iterator i = l.begin();

      visit_SortStruct(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_StructCons);
      leave_SortStruct(t);
    }

    void walk_SortArrowProd(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SortArrowProd");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_SortArrowProd(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_SortExpr);
      walk_SortExpr((*i++).to_appl());
      leave_SortArrowProd(t);
    }

    void walk_SortArrow(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "SortArrow");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_SortArrow(t);
      walk_SortExpr((*i++).to_appl());
      walk_SortExpr((*i++).to_appl());
      leave_SortArrow(t);
    }

    void walk_Unknown(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Unknown");
      aterm_list l = t.argument_list();
      assert(l.size() == 0);
      aterm_list::iterator i = l.begin();

      visit_Unknown(t);
      
      leave_Unknown(t);
    }

    void walk_ActionProcess(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "ActionProcess");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_ActionProcess(t);
      walk_String((*i++).to_appl());
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataExpr);
      leave_ActionProcess(t);
    }

    void walk_Process(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Process");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Process(t);
      walk_ProcVarId((*i++).to_appl());
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataExpr);
      leave_Process(t);
    }

    void walk_Tau(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Tau");
      aterm_list l = t.argument_list();
      assert(l.size() == 0);
      aterm_list::iterator i = l.begin();

      visit_Tau(t);
      
      leave_Tau(t);
    }

    void walk_Sum(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Sum");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Sum(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_DataVarId);
      walk_ProcExpr((*i++).to_appl());
      leave_Sum(t);
    }

    void walk_Restrict(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Restrict");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Restrict(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_String);
      walk_ProcExpr((*i++).to_appl());
      leave_Restrict(t);
    }

    void walk_Hide(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Hide");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Hide(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_String);
      walk_ProcExpr((*i++).to_appl());
      leave_Hide(t);
    }

    void walk_Rename(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Rename");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Rename(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_RenameExpr);
      walk_ProcExpr((*i++).to_appl());
      leave_Rename(t);
    }

    void walk_Comm(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Comm");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Comm(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_CommExpr);
      walk_ProcExpr((*i++).to_appl());
      leave_Comm(t);
    }

    void walk_Allow(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Allow");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Allow(t);
      walk_list((*i++).to_list(), &mcrl2_visitor::walk_MultActName);
      walk_ProcExpr((*i++).to_appl());
      leave_Allow(t);
    }

    void walk_Sync(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Sync");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Sync(t);
      walk_ProcExpr((*i++).to_appl());
      walk_ProcExpr((*i++).to_appl());
      leave_Sync(t);
    }

    void walk_AtTime(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "AtTime");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_AtTime(t);
      walk_ProcExpr((*i++).to_appl());
      walk_DataExpr((*i++).to_appl());
      leave_AtTime(t);
    }

    void walk_Seq(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Seq");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Seq(t);
      walk_ProcExpr((*i++).to_appl());
      walk_ProcExpr((*i++).to_appl());
      leave_Seq(t);
    }

    void walk_Cond(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Cond");
      aterm_list l = t.argument_list();
      assert(l.size() == 3);
      aterm_list::iterator i = l.begin();

      visit_Cond(t);
      walk_DataExpr((*i++).to_appl());
      walk_ProcExpr((*i++).to_appl());
      walk_ProcExpr((*i++).to_appl());
      leave_Cond(t);
    }

    void walk_BInit(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "BInit");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_BInit(t);
      walk_ProcExpr((*i++).to_appl());
      walk_ProcExpr((*i++).to_appl());
      leave_BInit(t);
    }

    void walk_Merge(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Merge");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Merge(t);
      walk_ProcExpr((*i++).to_appl());
      walk_ProcExpr((*i++).to_appl());
      leave_Merge(t);
    }

    void walk_LMerge(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "LMerge");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_LMerge(t);
      walk_ProcExpr((*i++).to_appl());
      walk_ProcExpr((*i++).to_appl());
      leave_LMerge(t);
    }

    void walk_Choice(aterm_appl t)
    {
      assert(t.type() == AT_APPL);
      assert(t.function().name() == "Choice");
      aterm_list l = t.argument_list();
      assert(l.size() == 2);
      aterm_list::iterator i = l.begin();

      visit_Choice(t);
      walk_ProcExpr((*i++).to_appl());
      walk_ProcExpr((*i++).to_appl());
      leave_Choice(t);
    }

//--- end visitors
};

} // namespace mcrl2

#endif // MCRL2_VISITOR_H
