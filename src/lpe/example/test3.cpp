#include <iostream>
#include "atermpp/aterm.h"
#include "mcrl2/mcrl2_visitor.h"
#include "mcrl2/sort.h"

using namespace std;
using namespace mcrl2;
using namespace atermpp;

//---------------------------------------------------------//
//                     simple_visitor
//---------------------------------------------------------//
class simple_visitor: public mcrl2_visitor
{
 public:
   void visit_Spec(aterm_appl t)                         {}
   void leave_Spec(aterm_appl t)                         {}
   void visit_SortSpec(aterm_appl t)                     {}
   void leave_SortSpec(aterm_appl t)                     {}
   void visit_ConsSpec(aterm_appl t)                     {}
   void leave_ConsSpec(aterm_appl t)                     {}
   void visit_MapSpec(aterm_appl t)                      {}
   void leave_MapSpec(aterm_appl t)                      {}
   void visit_DataEqnSpec(aterm_appl t)                  {}
   void leave_DataEqnSpec(aterm_appl t)                  {}
   void visit_ActSpec(aterm_appl t)                      {}
   void leave_ActSpec(aterm_appl t)                      {}
   void visit_SortDecl(aterm_appl t)                     {}
   void leave_SortDecl(aterm_appl t)                     {}
   void visit_SortId(aterm_appl t)                       {}
   void leave_SortId(aterm_appl t)                       {}
   void visit_SortRef(aterm_appl t)                      {}
   void leave_SortRef(aterm_appl t)                      {}
   void visit_OpId(aterm_appl t)                         {}
   void leave_OpId(aterm_appl t)                         {}
   void visit_DataEqn(aterm_appl t)                      {}
   void leave_DataEqn(aterm_appl t)                      {}
   void visit_DataVarId(aterm_appl t)                    {}
   void leave_DataVarId(aterm_appl t)                    {}
   void visit_DataExprOrNil(aterm_appl t)                {}
   void leave_DataExprOrNil(aterm_appl t)                {}
   void visit_ActId(aterm_appl t)                        {}
   void leave_ActId(aterm_appl t)                        {}
   void visit_ProcEqnSpec(aterm_appl t)                  {}
   void leave_ProcEqnSpec(aterm_appl t)                  {}
   void visit_ProcEqn(aterm_appl t)                      {}
   void leave_ProcEqn(aterm_appl t)                      {}
   void visit_ProcVarId(aterm_appl t)                    {}
   void leave_ProcVarId(aterm_appl t)                    {}
   void visit_LPESummand(aterm_appl t)                   {}
   void leave_LPESummand(aterm_appl t)                   {}
   void visit_MultActOrDelta(aterm_appl t)               {}
   void leave_MultActOrDelta(aterm_appl t)               {}
   void visit_Action(aterm_appl t)                       {}
   void leave_Action(aterm_appl t)                       {}
   void visit_Assignment(aterm_appl t)                   {}
   void leave_Assignment(aterm_appl t)                   {}
   void visit_Init(aterm_appl t)                         {}
   void leave_Init(aterm_appl t)                         {}
   void visit_SortExpr(aterm_appl t)                     {}
   void leave_SortExpr(aterm_appl t)                     {}
   void visit_StructCons(aterm_appl t)                   {}
   void leave_StructCons(aterm_appl t)                   {}
   void visit_StructProj(aterm_appl t)                   {}
   void leave_StructProj(aterm_appl t)                   {}
   void visit_StringOrNil(aterm_appl t)                  {}
   void leave_StringOrNil(aterm_appl t)                  {}
   void visit_DataExpr(aterm_appl t)                     {}
   void leave_DataExpr(aterm_appl t)                     {}
   void visit_SortExprOrUnknown(aterm_appl t)            {}
   void leave_SortExprOrUnknown(aterm_appl t)            {}
   void visit_BagEnumElt(aterm_appl t)                   {}
   void leave_BagEnumElt(aterm_appl t)                   {}
   void visit_WhrDecl(aterm_appl t)                      {}
   void leave_WhrDecl(aterm_appl t)                      {}
   void visit_ProcExpr(aterm_appl t)                     {}
   void leave_ProcExpr(aterm_appl t)                     {}
   void visit_MultActName(aterm_appl t)                  {}
   void leave_MultActName(aterm_appl t)                  {}
   void visit_RenameExpr(aterm_appl t)                   {}
   void leave_RenameExpr(aterm_appl t)                   {}
   void visit_CommExpr(aterm_appl t)                     {}
   void leave_CommExpr(aterm_appl t)                     {}
   void visit_String(aterm_appl t)                       {}
   void leave_String(aterm_appl t)                       {}
   void visit_NumberString(aterm_appl t)                 {}
   void leave_NumberString(aterm_appl t)                 {}
   void visit_SpecV1(aterm_appl t)                       {}
   void leave_SpecV1(aterm_appl t)                       {}
   void visit_DataVarIdOpId(aterm_appl t)                {}
   void leave_DataVarIdOpId(aterm_appl t)                {}
   void visit_DataApplProd(aterm_appl t)                 {}
   void leave_DataApplProd(aterm_appl t)                 {}
   void visit_DataAppl(aterm_appl t)                     {}
   void leave_DataAppl(aterm_appl t)                     {}
   void visit_Number(aterm_appl t)                       {}
   void leave_Number(aterm_appl t)                       {}
   void visit_ListEnum(aterm_appl t)                     {}
   void leave_ListEnum(aterm_appl t)                     {}
   void visit_SetEnum(aterm_appl t)                      {}
   void leave_SetEnum(aterm_appl t)                      {}
   void visit_BagEnum(aterm_appl t)                      {}
   void leave_BagEnum(aterm_appl t)                      {}
   void visit_SetBagComp(aterm_appl t)                   {}
   void leave_SetBagComp(aterm_appl t)                   {}
   void visit_Forall(aterm_appl t)                       {}
   void leave_Forall(aterm_appl t)                       {}
   void visit_Exists(aterm_appl t)                       {}
   void leave_Exists(aterm_appl t)                       {}
   void visit_Lambda(aterm_appl t)                       {}
   void leave_Lambda(aterm_appl t)                       {}
   void visit_Whr(aterm_appl t)                          {}
   void leave_Whr(aterm_appl t)                          {}
   void visit_Nil(aterm_appl t)                          {}
   void leave_Nil(aterm_appl t)                          {}
   void visit_LPE(aterm_appl t)                          {}
   void leave_LPE(aterm_appl t)                          {}
   void visit_MultAct(aterm_appl t)                      {}
   void leave_MultAct(aterm_appl t)                      {}
   void visit_Delta(aterm_appl t)                        {}
   void leave_Delta(aterm_appl t)                        {}
   void visit_LPEInit(aterm_appl t)                      {}
   void leave_LPEInit(aterm_appl t)                      {}
   void visit_SortList(aterm_appl t)                     {}
   void leave_SortList(aterm_appl t)                     {}
   void visit_SortSet(aterm_appl t)                      {}
   void leave_SortSet(aterm_appl t)                      {}
   void visit_SortBag(aterm_appl t)                      {}
   void leave_SortBag(aterm_appl t)                      {}
   void visit_SortStruct(aterm_appl t)                   {}
   void leave_SortStruct(aterm_appl t)                   {}
   void visit_SortArrowProd(aterm_appl t)                {}
   void leave_SortArrowProd(aterm_appl t)                {}
   void visit_SortArrow(aterm_appl t)                    {}
   void leave_SortArrow(aterm_appl t)                    {}
   void visit_Unknown(aterm_appl t)                      {}
   void leave_Unknown(aterm_appl t)                      {}
   void visit_ActionProcess(aterm_appl t)                {}
   void leave_ActionProcess(aterm_appl t)                {}
   void visit_Process(aterm_appl t)                      {}
   void leave_Process(aterm_appl t)                      {}
   void visit_Tau(aterm_appl t)                          {}
   void leave_Tau(aterm_appl t)                          {}
   void visit_Sum(aterm_appl t)                          {}
   void leave_Sum(aterm_appl t)                          {}
   void visit_Restrict(aterm_appl t)                     {}
   void leave_Restrict(aterm_appl t)                     {}
   void visit_Hide(aterm_appl t)                         {}
   void leave_Hide(aterm_appl t)                         {}
   void visit_Rename(aterm_appl t)                       {}
   void leave_Rename(aterm_appl t)                       {}
   void visit_Comm(aterm_appl t)                         {}
   void leave_Comm(aterm_appl t)                         {}
   void visit_Allow(aterm_appl t)                        {}
   void leave_Allow(aterm_appl t)                        {}
   void visit_Sync(aterm_appl t)                         {}
   void leave_Sync(aterm_appl t)                         {}
   void visit_AtTime(aterm_appl t)                       {}
   void leave_AtTime(aterm_appl t)                       {}
   void visit_Seq(aterm_appl t)                          {}
   void leave_Seq(aterm_appl t)                          {}
   void visit_Cond(aterm_appl t)                         {}
   void leave_Cond(aterm_appl t)                         {}
   void visit_BInit(aterm_appl t)                        {}
   void leave_BInit(aterm_appl t)                        {}
   void visit_Merge(aterm_appl t)                        {}
   void leave_Merge(aterm_appl t)                        {}
   void visit_LMerge(aterm_appl t)                       {}
   void leave_LMerge(aterm_appl t)                       {}
   void visit_Choice(aterm_appl t)                       {}
   void leave_Choice(aterm_appl t)                       {}
};

int main()
{
  string filename("data/abp_b.lpe");
  aterm_appl t = read_from_named_file(filename).to_appl();
  if (!t)
    cerr << "could not read file!" << endl;
  
  simple_visitor v;
  v.walk_SpecV1(t);
  
  cin.get();

  return 0;
}
