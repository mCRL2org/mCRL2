#include <iostream>
#include "atermpp/aterm.h"
#include "mcrl2/mcrl2_visitor.h"

using namespace std;
using namespace mcrl2;
using namespace atermpp;

// function object that returns true if the function symbol f matches with
// the given aterm_appl t
struct StopAtMatch
{
  const function_symbol& f;

  StopAtMatch(const function_symbol& f_)
    : f(f_)
  {}

  bool operator()(aterm_appl t) const
  {
    return t.function() == f;
  }
};

//---------------------------------------------------------//
//                     simple_visitor
//---------------------------------------------------------//
class simple_visitor: public mcrl2_visitor
{
 public:
   void visit_Spec(aterm_appl t)                         { cout << "visiting Spec            " << endl; }
   void leave_Spec(aterm_appl t)                         { cout << "leaving  Spec            " << endl; }
   void visit_SortSpec(aterm_appl t)                     { cout << "visiting SortSpec        " << endl; }
   void leave_SortSpec(aterm_appl t)                     { cout << "leaving  SortSpec        " << endl; }
   void visit_ConsSpec(aterm_appl t)                     { cout << "visiting ConsSpec        " << endl; }
   void leave_ConsSpec(aterm_appl t)                     { cout << "leaving  ConsSpec        " << endl; }
   void visit_MapSpec(aterm_appl t)                      { cout << "visiting MapSpec         " << endl; }
   void leave_MapSpec(aterm_appl t)                      { cout << "leaving  MapSpec         " << endl; }
   void visit_DataEqnSpec(aterm_appl t)                  { cout << "visiting DataEqnSpec     " << endl; }
   void leave_DataEqnSpec(aterm_appl t)                  { cout << "leaving  DataEqnSpec     " << endl; }
   void visit_ActSpec(aterm_appl t)                      { cout << "visiting ActSpec         " << endl; }
   void leave_ActSpec(aterm_appl t)                      { cout << "leaving  ActSpec         " << endl; }
   void visit_SortDecl(aterm_appl t)                     { cout << "visiting SortDecl        " << endl; }
   void leave_SortDecl(aterm_appl t)                     { cout << "leaving  SortDecl        " << endl; }
   void visit_SortId(aterm_appl t)                       { cout << "visiting SortId          " << endl; }
   void leave_SortId(aterm_appl t)                       { cout << "leaving  SortId          " << endl; }
   void visit_SortRef(aterm_appl t)                      { cout << "visiting SortRef         " << endl; }
   void leave_SortRef(aterm_appl t)                      { cout << "leaving  SortRef         " << endl; }
   void visit_OpId(aterm_appl t)                         { cout << "visiting OpId            " << endl; }
   void leave_OpId(aterm_appl t)                         { cout << "leaving  OpId            " << endl; }
   void visit_DataEqn(aterm_appl t)                      { cout << "visiting DataEqn         " << endl; }
   void leave_DataEqn(aterm_appl t)                      { cout << "leaving  DataEqn         " << endl; }
   void visit_DataVarId(aterm_appl t)                    { cout << "visiting DataVarId       " << endl; }
   void leave_DataVarId(aterm_appl t)                    { cout << "leaving  DataVarId       " << endl; }
   void visit_DataExprOrNil(aterm_appl t)                { cout << "visiting DataExprOrNil   " << endl; }
   void leave_DataExprOrNil(aterm_appl t)                { cout << "leaving  DataExprOrNil   " << endl; }
   void visit_ActId(aterm_appl t)                        { cout << "visiting ActId           " << endl; }
   void leave_ActId(aterm_appl t)                        { cout << "leaving  ActId           " << endl; }
   void visit_ProcEqnSpec(aterm_appl t)                  { cout << "visiting ProcEqnSpec     " << endl; }
   void leave_ProcEqnSpec(aterm_appl t)                  { cout << "leaving  ProcEqnSpec     " << endl; }
   void visit_ProcEqn(aterm_appl t)                      { cout << "visiting ProcEqn         " << endl; }
   void leave_ProcEqn(aterm_appl t)                      { cout << "leaving  ProcEqn         " << endl; }
   void visit_ProcVarId(aterm_appl t)                    { cout << "visiting ProcVarId       " << endl; }
   void leave_ProcVarId(aterm_appl t)                    { cout << "leaving  ProcVarId       " << endl; }
   void visit_LPESummand(aterm_appl t)                   { cout << "visiting LPESummand      " << endl; }
   void leave_LPESummand(aterm_appl t)                   { cout << "leaving  LPESummand      " << endl; }
   void visit_MultActOrDelta(aterm_appl t)               { cout << "visiting MultActOrDelta  " << endl; }
   void leave_MultActOrDelta(aterm_appl t)               { cout << "leaving  MultActOrDelta  " << endl; }
   void visit_Action(aterm_appl t)                       { cout << "visiting Action          " << endl; }
   void leave_Action(aterm_appl t)                       { cout << "leaving  Action          " << endl; }
   void visit_Assignment(aterm_appl t)                   { cout << "visiting Assignment      " << endl; }
   void leave_Assignment(aterm_appl t)                   { cout << "leaving  Assignment      " << endl; }
   void visit_Init(aterm_appl t)                         { cout << "visiting Init            " << endl; }
   void leave_Init(aterm_appl t)                         { cout << "leaving  Init            " << endl; }
   void visit_SortExpr(aterm_appl t)                     { cout << "visiting SortExpr        " << endl; }
   void leave_SortExpr(aterm_appl t)                     { cout << "leaving  SortExpr        " << endl; }
   void visit_StructCons(aterm_appl t)                   { cout << "visiting StructCons      " << endl; }
   void leave_StructCons(aterm_appl t)                   { cout << "leaving  StructCons      " << endl; }
   void visit_StructProj(aterm_appl t)                   { cout << "visiting StructProj      " << endl; }
   void leave_StructProj(aterm_appl t)                   { cout << "leaving  StructProj      " << endl; }
   void visit_StringOrNil(aterm_appl t)                  { cout << "visiting StringOrNil     " << endl; }
   void leave_StringOrNil(aterm_appl t)                  { cout << "leaving  StringOrNil     " << endl; }
   void visit_DataExpr(aterm_appl t)                     { cout << "visiting DataExpr        " << endl; }
   void leave_DataExpr(aterm_appl t)                     { cout << "leaving  DataExpr        " << endl; }
   void visit_SortExprOrUnknown(aterm_appl t)            { cout << "visiting SortExprOrUnknow" << endl; }
   void leave_SortExprOrUnknown(aterm_appl t)            { cout << "leaving  SortExprOrUnknow" << endl; }
   void visit_BagEnumElt(aterm_appl t)                   { cout << "visiting BagEnumElt      " << endl; }
   void leave_BagEnumElt(aterm_appl t)                   { cout << "leaving  BagEnumElt      " << endl; }
   void visit_WhrDecl(aterm_appl t)                      { cout << "visiting WhrDecl         " << endl; }
   void leave_WhrDecl(aterm_appl t)                      { cout << "leaving  WhrDecl         " << endl; }
   void visit_ProcExpr(aterm_appl t)                     { cout << "visiting ProcExpr        " << endl; }
   void leave_ProcExpr(aterm_appl t)                     { cout << "leaving  ProcExpr        " << endl; }
   void visit_MultActName(aterm_appl t)                  { cout << "visiting MultActName     " << endl; }
   void leave_MultActName(aterm_appl t)                  { cout << "leaving  MultActName     " << endl; }
   void visit_RenameExpr(aterm_appl t)                   { cout << "visiting RenameExpr      " << endl; }
   void leave_RenameExpr(aterm_appl t)                   { cout << "leaving  RenameExpr      " << endl; }
   void visit_CommExpr(aterm_appl t)                     { cout << "visiting CommExpr        " << endl; }
   void leave_CommExpr(aterm_appl t)                     { cout << "leaving  CommExpr        " << endl; }
   void visit_String(aterm_appl t)                       { cout << "visiting String          " << endl; }
   void leave_String(aterm_appl t)                       { cout << "leaving  String          " << endl; }
   void visit_NumberString(aterm_appl t)                 { cout << "visiting NumberString    " << endl; }
   void leave_NumberString(aterm_appl t)                 { cout << "leaving  NumberString    " << endl; }
   void visit_SpecV1(aterm_appl t)                       { cout << "visiting SpecV1          " << endl; }
   void leave_SpecV1(aterm_appl t)                       { cout << "leaving  SpecV1          " << endl; }
   void visit_DataVarIdOpId(aterm_appl t)                { cout << "visiting DataVarIdOpId   " << endl; }
   void leave_DataVarIdOpId(aterm_appl t)                { cout << "leaving  DataVarIdOpId   " << endl; }
   void visit_DataApplProd(aterm_appl t)                 { cout << "visiting DataApplProd    " << endl; }
   void leave_DataApplProd(aterm_appl t)                 { cout << "leaving  DataApplProd    " << endl; }
   void visit_DataAppl(aterm_appl t)                     { cout << "visiting DataAppl        " << endl; }
   void leave_DataAppl(aterm_appl t)                     { cout << "leaving  DataAppl        " << endl; }
   void visit_Number(aterm_appl t)                       { cout << "visiting Number          " << endl; }
   void leave_Number(aterm_appl t)                       { cout << "leaving  Number          " << endl; }
   void visit_ListEnum(aterm_appl t)                     { cout << "visiting ListEnum        " << endl; }
   void leave_ListEnum(aterm_appl t)                     { cout << "leaving  ListEnum        " << endl; }
   void visit_SetEnum(aterm_appl t)                      { cout << "visiting SetEnum         " << endl; }
   void leave_SetEnum(aterm_appl t)                      { cout << "leaving  SetEnum         " << endl; }
   void visit_BagEnum(aterm_appl t)                      { cout << "visiting BagEnum         " << endl; }
   void leave_BagEnum(aterm_appl t)                      { cout << "leaving  BagEnum         " << endl; }
   void visit_SetBagComp(aterm_appl t)                   { cout << "visiting SetBagComp      " << endl; }
   void leave_SetBagComp(aterm_appl t)                   { cout << "leaving  SetBagComp      " << endl; }
   void visit_Forall(aterm_appl t)                       { cout << "visiting Forall          " << endl; }
   void leave_Forall(aterm_appl t)                       { cout << "leaving  Forall          " << endl; }
   void visit_Exists(aterm_appl t)                       { cout << "visiting Exists          " << endl; }
   void leave_Exists(aterm_appl t)                       { cout << "leaving  Exists          " << endl; }
   void visit_Lambda(aterm_appl t)                       { cout << "visiting Lambda          " << endl; }
   void leave_Lambda(aterm_appl t)                       { cout << "leaving  Lambda          " << endl; }
   void visit_Whr(aterm_appl t)                          { cout << "visiting Whr             " << endl; }
   void leave_Whr(aterm_appl t)                          { cout << "leaving  Whr             " << endl; }
   void visit_Nil(aterm_appl t)                          { cout << "visiting Nil             " << endl; }
   void leave_Nil(aterm_appl t)                          { cout << "leaving  Nil             " << endl; }
   void visit_LPE(aterm_appl t)                          { cout << "visiting LPE             " << endl; }
   void leave_LPE(aterm_appl t)                          { cout << "leaving  LPE             " << endl; }
   void visit_MultAct(aterm_appl t)                      { cout << "visiting MultAct         " << endl; }
   void leave_MultAct(aterm_appl t)                      { cout << "leaving  MultAct         " << endl; }
   void visit_Delta(aterm_appl t)                        { cout << "visiting Delta           " << endl; }
   void leave_Delta(aterm_appl t)                        { cout << "leaving  Delta           " << endl; }
   void visit_LPEInit(aterm_appl t)                      { cout << "visiting LPEInit         " << endl; }
   void leave_LPEInit(aterm_appl t)                      { cout << "leaving  LPEInit         " << endl; }
   void visit_SortList(aterm_appl t)                     { cout << "visiting SortList        " << endl; }
   void leave_SortList(aterm_appl t)                     { cout << "leaving  SortList        " << endl; }
   void visit_SortSet(aterm_appl t)                      { cout << "visiting SortSet         " << endl; }
   void leave_SortSet(aterm_appl t)                      { cout << "leaving  SortSet         " << endl; }
   void visit_SortBag(aterm_appl t)                      { cout << "visiting SortBag         " << endl; }
   void leave_SortBag(aterm_appl t)                      { cout << "leaving  SortBag         " << endl; }
   void visit_SortStruct(aterm_appl t)                   { cout << "visiting SortStruct      " << endl; }
   void leave_SortStruct(aterm_appl t)                   { cout << "leaving  SortStruct      " << endl; }
   void visit_SortArrowProd(aterm_appl t)                { cout << "visiting SortArrowProd   " << endl; }
   void leave_SortArrowProd(aterm_appl t)                { cout << "leaving  SortArrowProd   " << endl; }
   void visit_SortArrow(aterm_appl t)                    { cout << "visiting SortArrow       " << endl; }
   void leave_SortArrow(aterm_appl t)                    { cout << "leaving  SortArrow       " << endl; }
   void visit_Unknown(aterm_appl t)                      { cout << "visiting Unknown         " << endl; }
   void leave_Unknown(aterm_appl t)                      { cout << "leaving  Unknown         " << endl; }
   void visit_ActionProcess(aterm_appl t)                { cout << "visiting ActionProcess   " << endl; }
   void leave_ActionProcess(aterm_appl t)                { cout << "leaving  ActionProcess   " << endl; }
   void visit_Process(aterm_appl t)                      { cout << "visiting Process         " << endl; }
   void leave_Process(aterm_appl t)                      { cout << "leaving  Process         " << endl; }
   void visit_Tau(aterm_appl t)                          { cout << "visiting Tau             " << endl; }
   void leave_Tau(aterm_appl t)                          { cout << "leaving  Tau             " << endl; }
   void visit_Sum(aterm_appl t)                          { cout << "visiting Sum             " << endl; }
   void leave_Sum(aterm_appl t)                          { cout << "leaving  Sum             " << endl; }
   void visit_Restrict(aterm_appl t)                     { cout << "visiting Restrict        " << endl; }
   void leave_Restrict(aterm_appl t)                     { cout << "leaving  Restrict        " << endl; }
   void visit_Hide(aterm_appl t)                         { cout << "visiting Hide            " << endl; }
   void leave_Hide(aterm_appl t)                         { cout << "leaving  Hide            " << endl; }
   void visit_Rename(aterm_appl t)                       { cout << "visiting Rename          " << endl; }
   void leave_Rename(aterm_appl t)                       { cout << "leaving  Rename          " << endl; }
   void visit_Comm(aterm_appl t)                         { cout << "visiting Comm            " << endl; }
   void leave_Comm(aterm_appl t)                         { cout << "leaving  Comm            " << endl; }
   void visit_Allow(aterm_appl t)                        { cout << "visiting Allow           " << endl; }
   void leave_Allow(aterm_appl t)                        { cout << "leaving  Allow           " << endl; }
   void visit_Sync(aterm_appl t)                         { cout << "visiting Sync            " << endl; }
   void leave_Sync(aterm_appl t)                         { cout << "leaving  Sync            " << endl; }
   void visit_AtTime(aterm_appl t)                       { cout << "visiting AtTime          " << endl; }
   void leave_AtTime(aterm_appl t)                       { cout << "leaving  AtTime          " << endl; }
   void visit_Seq(aterm_appl t)                          { cout << "visiting Seq             " << endl; }
   void leave_Seq(aterm_appl t)                          { cout << "leaving  Seq             " << endl; }
   void visit_Cond(aterm_appl t)                         { cout << "visiting Cond            " << endl; }
   void leave_Cond(aterm_appl t)                         { cout << "leaving  Cond            " << endl; }
   void visit_BInit(aterm_appl t)                        { cout << "visiting BInit           " << endl; }
   void leave_BInit(aterm_appl t)                        { cout << "leaving  BInit           " << endl; }
   void visit_Merge(aterm_appl t)                        { cout << "visiting Merge           " << endl; }
   void leave_Merge(aterm_appl t)                        { cout << "leaving  Merge           " << endl; }
   void visit_LMerge(aterm_appl t)                       { cout << "visiting LMerge          " << endl; }
   void leave_LMerge(aterm_appl t)                       { cout << "leaving  LMerge          " << endl; }
   void visit_Choice(aterm_appl t)                       { cout << "visiting Choice          " << endl; }
   void leave_Choice(aterm_appl t)                       { cout << "leaving  Choice          " << endl; }
};

int main()
{
  string filename("data/abp_b.lpe");
  aterm_appl t = read_from_named_file(filename).to_appl();
  if (!t)
    cerr << "could not read file!" << endl;

  simple_visitor v;

  function_symbol f("LPE", 3);
  v.walk_SpecV1(t, StopAtMatch(f));

  cin.get();

  return 0;
}
