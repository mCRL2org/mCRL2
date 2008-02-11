// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/pbes/detail/quantifier_rename_builder.h"
#include "mcrl2/pbes/rename.h"
#include "mcrl2/pbes/complement.h"
#include "mcrl2/pbes/normalize.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::modal;
using namespace mcrl2::modal::detail;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;

const std::string SPECIFICATION =
"act a:Nat;                               \n"
"                                         \n"
"map smaller: Nat#Nat -> Bool;            \n"
"                                         \n"
"var x,y : Nat;                           \n"
"                                         \n"
"eqn smaller(x,y) = x < y;                \n"
"                                         \n"
"proc P(n:Nat) = sum m: Nat. a(m). P(m);  \n"
"                                         \n"
"init P(0);                               \n";

const std::string ABP_SPECIFICATION =
"% This file contains the alternating bit protocol, as described in W.J.    \n"
"% Fokkink, J.F. Groote and M.A. Reniers, Modelling Reactive Systems.       \n"
"%                                                                          \n"
"% The only exception is that the domain D consists of two data elements to \n"
"% facilitate simulation.                                                   \n"
"                                                                           \n"
"sort                                                                       \n"
"  D     = struct d1 | d2;                                                  \n"
"  Error = struct e;                                                        \n"
"                                                                           \n"
"act                                                                        \n"
"  r1,s4: D;                                                                \n"
"  s2,r2,c2: D # Bool;                                                      \n"
"  s3,r3,c3: D # Bool;                                                      \n"
"  s3,r3,c3: Error;                                                         \n"
"  s5,r5,c5: Bool;                                                          \n"
"  s6,r6,c6: Bool;                                                          \n"
"  s6,r6,c6: Error;                                                         \n"
"  i;                                                                       \n"
"                                                                           \n"
"proc                                                                       \n"
"  S(b:Bool)     = sum d:D. r1(d).T(d,b);                                   \n"
"  T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));             \n"
"                                                                           \n"
"  R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+                      \n"
"                  (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);                    \n"
"                                                                           \n"
"  K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;           \n"
"                                                                           \n"
"  L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;                   \n"
"                                                                           \n"
"init                                                                       \n"
"  allow({r1,s4,c2,c3,c5,c6,i},                                             \n"
"    comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},                     \n"
"        S(true) || K || L || R(true)                                       \n"
"    )                                                                      \n"
"  );                                                                       \n"
;

const std::string TRIVIAL_FORMULA  = "[true*]<true*>true";

const std::string MPSU_SPECIFICATION =
"% This file describes a controller for a simplified Movable Patient                   \n"
"% Support Unit. It is described in Fokkink, Groote and Reniers,                       \n"
"% Modelling reactive systems.                                                         \n"
"%                                                                                     \n"
"% Jan Friso Groote, September, 2006.                                                  \n"
"                                                                                      \n"
"sort Mode = struct Normal | Emergency ;                                               \n"
"     MotorStatus = struct turnleft | turnright | stopped ;                            \n"
"                                                                                      \n"
"act pressStop, pressResume,                                                           \n"
"    pressUndock, pressLeft,                                                           \n"
"    pressRight, motorLeft,                                                            \n"
"    motorRight, motorOff,                                                             \n"
"    applyBrake, releaseBrake,                                                         \n"
"    isDocked, unlockDock,                                                             \n"
"    atInnermost, atOutermost;                                                         \n"
"                                                                                      \n"
"proc Controller(m:Mode,docked,rightmost,leftmost:Bool,ms:MotorStatus)=                \n"
"       pressStop.Controller(Emergency,docked,rightmost,leftmost,ms)+                  \n"
"       pressResume.Controller(Normal,docked,rightmost,leftmost,ms)+                   \n"
"       pressUndock.                                                                   \n"
"         (docked && rightmost)                                                        \n"
"                -> applyBrake.unlockDock.Controller(m,false,rightmost,leftmost,ms)    \n"
"                <> Controller(m,docked,rightmost,leftmost,ms)+                        \n"
"       pressLeft.                                                                     \n"
"          (docked && ms!=turnleft && !leftmost && m==Normal)                          \n"
"                -> releaseBrake.motorLeft.                                            \n"
"                     Controller(m,docked,false,leftmost,turnleft)                     \n"
"                <> Controller(m,docked,rightmost,leftmost,ms)+                        \n"
"       pressRight.                                                                    \n"
"          (docked && ms!=turnright && !rightmost && m==Normal)                        \n"
"                -> releaseBrake.motorRight.                                           \n"
"                     Controller(m,docked,rightmost,false,turnright)                   \n"
"                <> Controller(m,docked,rightmost,leftmost,ms)+                        \n"
"       isDocked.Controller(m,true,rightmost,leftmost,ms)+                             \n"
"       atInnermost.motorOff.applyBrake.Controller(m,docked,true,false,stopped)+       \n"
"       atOutermost.motorOff.applyBrake.Controller(m,docked,false,true,stopped);       \n"
"                                                                                      \n"
"                                                                                      \n"
"                                                                                      \n"
"init Controller(Normal,true,false,false,stopped);                                     \n"
;

const std::string FORMULA  = "nu X(n:Nat = 1). [forall m:Nat. a(m)](val(n < 10)  && X(n+2))";
const std::string FORMULA2 = "forall m:Nat. [a(m)]false";

const std::string MPSU_FORMULA =
"% This file describes the modal formulas for property 5 used in section \n"
"% 5.3 of Designing and understanding the behaviour of systems           \n"
"% by J.F. Groote and M.A. Reniers.                                      \n"
"                                                                        \n"
"nu X(b1:Bool=false, b2:Bool=true,b3:Bool=true,b4:Bool=true).            \n"
"        val(b1 && b2 && b3 && b4) => ([pressLeft]                       \n"
"              (mu Y.[!motorLeft &&                                      \n"
"                    !unlockDock &&                                      \n"
"                    !pressStop &&                                       \n"
"                    !atInnermost]Y))                                    \n";

const std::string REQ1_1 =
"nu X . [true] X &&                                                                     \n" 
"  forall nana:Node .                                                                   \n"
"    forall trans:Transaction .                                                         \n"
"      [cUQ(nana,trans)](nu Y . [true]Y && [exists nana2:Node . cUQ(nana2,trans)]false) \n"
; 

const std::string REQ1_2 =
"nu X . [true]X && forall n:Node . forall t:Transaction . [cUQ(n,t)](mu Y \n" 
". [!exists r:Phase . sUR(n,t,r)]Y && <true>true)                         \n"
;

const std::string MODEL =
"sort                                                                                                                                                                                   \n" 
"  Node = struct N1 | N2;                                                                                                                                                               \n" 
"  Result = struct commit | abort;                                                                                                                                                      \n" 
"  Action = struct r | w;                                                                                                                                                               \n" 
"  Phase = struct read | send | lock | committed | aborted;                                                                                                                             \n" 
"  Transaction = struct t1 | t2;                                                                                                                                                        \n" 
"  TransactionItem = struct tr(identifier:Transaction, phase:Phase);                                                                                                                    \n" 
"  TransactionQueue = struct tq(node:Node, transaction:TransactionItem);                                                                                                                \n" 
"                                                                                                                                                                                       \n" 
"                                                                                                                                                                                       \n" 
"act                                                                                                                                                                                    \n" 
"  % Communication node with user                                                                                                                                                       \n" 
"  rUQ, sUQ, cUQ:Node#Transaction;                                                                                                                                                      \n" 
"  rUR, sUR, cUR:Node#Transaction#Phase;                                                                                                                                                \n" 
"                                                                                                                                                                                       \n" 
"  % Communication node with network                                                                                                                                                    \n" 
"  rSend, sSend, cSend: Node#Transaction;                                                                                                                                               \n" 
"  rSend, sSend, cSend: Node#Transaction#Result;                                                                                                                                        \n" 
"  rReceive, sReceive, cReceive: Node#Transaction;                                                                                                                                      \n" 
"  rReceive, sReceive, cReceive: Node#Transaction#Result;                                                                                                                               \n" 
"                                                                                                                                                                                       \n" 
"  % Communication with the lock manager                                                                                                                                                \n" 
"  rLock, sLock, cLock : Node#Transaction#Action;                                                                                                                                       \n" 
"  rLockResult, sLockResult, cLockResult : Node#Transaction#Result;                                                                                                                     \n" 
"  rLockDecision, sLockDecision, cLockDecision : Node#Transaction#Result;                                                                                                               \n" 
"  rRejectLock, sRejectLock, cRejectLock : Node#Transaction;                                                                                                                            \n" 
"                                                                                                                                                                                       \n" 
"  rLockResultWS, sLockResultWS, cLockResultWS : Node#Transaction#Result;                                                                                                               \n" 
"                                                                                                                                                                                       \n" 
"  % Internal decisions of the lock manager                                                                                                                                             \n" 
"  NoLocks, NoConflictingLocks, ConflictingLocks:Node#Transaction;                                                                                                                      \n" 
"                                                                                                                                                                                       \n" 
"map filter : List(TransactionQueue)#Node -> List(TransactionQueue);                                                                                                                    \n" 
"  remove : List(TransactionQueue)#TransactionQueue -> List(TransactionQueue);                                                                                                          \n" 
"  remove : List(TransactionItem)#TransactionItem -> List(TransactionItem);                                                                                                             \n" 
"  update : List(TransactionItem)#TransactionItem#Phase -> List(TransactionItem);                                                                                                       \n" 
"var                                                                                                                                                                                    \n" 
"  LTQ : List(TransactionQueue);                                                                                                                                                        \n" 
"  LTI : List(TransactionItem);                                                                                                                                                         \n" 
"  n: Node;                                                                                                                                                                             \n" 
"  t: TransactionItem;                                                                                                                                                                  \n" 
"  tq: TransactionQueue;                                                                                                                                                                \n" 
"  p: Phase;                                                                                                                                                                            \n" 
"eqn                                                                                                                                                                                    \n" 
"  (#LTQ == 0) -> remove(LTQ,tq) = [];                                                                                                                                                  \n" 
"  (#LTQ != 0) -> remove(LTQ,tq) = if (identifier(transaction(head(LTQ))) == identifier(transaction(tq)) && node(head(LTQ)) == node(tq),                                                \n" 
"                    remove(tail(LTQ),tq) ,                                                                                                                                             \n" 
"                    [head(LTQ)] ++ remove(tail(LTQ),tq)                                                                                                                                \n" 
"                  );                                                                                                                                                                   \n" 
"                                                                                                                                                                                       \n" 
"  (#LTI == 0) -> remove(LTI,t) = [];                                                                                                                                                   \n" 
"  (#LTI != 0) -> remove(LTI,t) = if (identifier(head(LTI)) == identifier(t),                                                                                                           \n" 
"                    remove(tail(LTI),t) ,                                                                                                                                              \n" 
"                    [head(LTI)] ++ remove(tail(LTI),t)                                                                                                                                 \n" 
"                  );                                                                                                                                                                   \n" 
"                                                                                                                                                                                       \n" 
"  (#LTQ == 0) -> filter(LTQ,n) = [];                                                                                                                                                   \n" 
"  (#LTQ != 0) -> filter(LTQ,n) = if(n == node(head(LTQ)), [head(LTQ)] ++ filter(tail(LTQ),n), filter(tail(LTQ),n));                                                                    \n" 
"                                                                                                                                                                                       \n" 
"  (#LTI == 0) -> update(LTI,t,p) = [];                                                                                                                                                 \n" 
"  (#LTI != 0) -> update(LTI,t,p) = if ( identifier(head(LTI)) == identifier(t) , [tr(identifier(head(LTI)), p)] ++ update(tail(LTI), t, p) , [head(LTI)] ++ update(tail(LTI), t, p) ); \n" 
"                                                                                                                                                                                       \n" 
"%proc USER(t:List(Transaction)) =                                                                                                                                                      \n" 
"%  (#t > 0)                                                                                                                                                                            \n" 
"%    -> sum n:Node . sUQ(n, head(t)) . USER(tail(t))                                                                                                                                   \n" 
"%    <> delta;                                                                                                                                                                         \n" 
"                                                                                                                                                                                       \n" 
"                                                                                                                                                                                       \n" 
" proc USER(t:List(Transaction)) =                                                                                                                                                      \n" 
"  (#t > 1)                                                                                                                                                                             \n" 
"    -> sum n:Node . sUQ(n,head(t)) . USER(tail(t))                                                                                                                                     \n" 
"    <> sum n:Node . sUQ(n,head(t)) . delta;                                                                                                                                            \n" 
"                                                                                                                                                                                       \n" 
"proc NETWORK(transactions: List(TransactionQueue), results: List(TransactionQueue)) =                                                                                                  \n" 
"  sum n:Node . (                                                                                                                                                                       \n" 
"    % Receive inputs from users                                                                                                                                                        \n" 
"    sum t:Transaction . (                                                                                                                                                              \n" 
"      rSend(n,t) . NETWORK(transactions ++ [tq(N1,tr(t,lock)), tq(N2,tr(t,lock))], results)                                                                                            \n" 
"      +                                                                                                                                                                                \n" 
"      sum r:Result . rSend(n,t,r) . (                                                                                                                                                  \n" 
"        (r == abort)                                                                                                                                                                   \n" 
"        -> NETWORK(transactions, results ++ [tq(N1,tr(t,aborted)), tq(N2,tr(t,aborted))])                                                                                              \n" 
"        <> NETWORK(transactions, results ++ [tq(N1,tr(t,committed)), tq(N2,tr(t,committed))])                                                                                          \n" 
"      )                                                                                                                                                                                \n" 
"    )                                                                                                                                                                                  \n" 
"    +                                                                                                                                                                                  \n" 
"                                                                                                                                                                                       \n" 
"    % Multicast the transaction messages                                                                                                                                               \n" 
"    (#filter(transactions, n) != 0)                                                                                                                                                    \n" 
"    ->  sReceive(n, identifier(transaction(head(filter(transactions, n))))) .                                                                                                          \n" 
"      NETWORK(remove(transactions, head(filter(transactions, n))), results)                                                                                                            \n" 
"    <>  delta                                                                                                                                                                          \n" 
"    +                                                                                                                                                                                  \n" 
"                                                                                                                                                                                       \n" 
"    % Multicast the results (in arbitrary order)                                                                                                                                       \n" 
"    sum t:Transaction . sum p:Phase . (                                                                                                                                                \n" 
"      (tq(n,tr(t,p)) in results)                                                                                                                                                       \n" 
"      ->  ( (p == committed)                                                                                                                                                           \n" 
"        ->  sReceive(n,t,commit)                                                                                                                                                       \n" 
"        <>  sReceive(n,t,abort)                                                                                                                                                        \n" 
"      ) . NETWORK(transactions, remove(results, tq(n,tr(t,p))))                                                                                                                        \n" 
"      <>  delta                                                                                                                                                                        \n" 
"	)                                                                                                                                                                                     \n" 
");                                                                                                                                                                                     \n" 
"                                                                                                                                                                                       \n" 
"proc NODE(n:Node) = NODE_TRANSACTION_MANAGER(n,[]) || NODE_COMMUNICATION_MANAGER(n) || NODE_LOCK_MANAGER(n,[],[],[]);                                                                  \n" 
"                                                                                                                                                                                       \n" 
"proc NODE_TRANSACTION_MANAGER(n:Node, local:List(TransactionItem)) =                                                                                                                   \n" 
"  %Receive user request                                                                                                                                                                \n" 
"  sum t:Transaction .                                                                                                                                                                  \n" 
"    rUQ(n,t) .                                                                                                                                                                         \n" 
"    NODE_TRANSACTION_MANAGER(n,local ++ [tr(t,read)])                                                                                                                                  \n" 
"                                                                                                                                                                                       \n" 
"  +   % Handle user requests                                                                                                                                                           \n" 
"  sum t:Transaction .                                                                                                                                                                  \n" 
"    (tr(t,read) in local)                                                                                                                                                              \n" 
"    ->  sLock(n,t,r) .                                                                                                                                                                 \n" 
"        sum r:Result .                                                                                                                                                                 \n" 
"          rLockResult(n, t, r) .                                                                                                                                                       \n" 
"          NODE_TRANSACTION_MANAGER(n, update(local, tr(t,read), send))                                                                                                                 \n" 
"    <> delta                                                                                                                                                                           \n" 
"                                                                                                                                                                                       \n" 
"  +  % Send the lock request for all write operations to the GCM                                                                                                                       \n" 
"  sum t:Transaction .                                                                                                                                                                  \n" 
"    (tr(t,send) in local)                                                                                                                                                              \n" 
"    ->   sSend(n,t) .                                                                                                                                                                  \n" 
"      NODE_TRANSACTION_MANAGER(n, update(local, tr(t,send), lock))                                                                                                                     \n" 
"    <>   delta                                                                                                                                                                         \n" 
"                                                                                                                                                                                       \n" 
"  + % Listen for request for abortion by the lock manager                                                                                                                              \n" 
"    sum t:Transaction .                                                                                                                                                                \n" 
"      rRejectLock(n,t) . (                                                                                                                                                             \n" 
"        % The lock can belong to a message that is (not yet) send:                                                                                                                     \n" 
"        (tr(t,send) in local) 		% Not yet send; release locks and forget about it:                                                                                                    \n" 
"          ->sLockDecision(n,t,abort) .                                                                                                                                                 \n" 
"		    sClearLock(n,t) .                                                                                                                                                               \n" 
"            sUR(n,t,aborted) .                                                                                                                                                         \n" 
"            NODE_TRANSACTION_MANAGER(n, remove(local, tr(t,read)))                                                                                                                     \n" 
"          <> delta                                                                                                                                                                     \n" 
"        +                                                                                                                                                                              \n" 
"        (tr(t,lock) in local) 		% Already send, abort. Locks will be released automatically:                                                                                          \n" 
"          -> sSend(n,t,abort) .		                                                                                                                                                    \n" 
"             sUR(n,t,aborted) .		                                                                                                                                                    \n" 
"             NODE_TRANSACTION_MANAGER(n, remove(local, tr(t,lock)))                                                                                                                    \n" 
"          <> delta                                                                                                                                                                     \n" 
"      )                                                                                                                                                                                \n" 
"                                                                                                                                                                                       \n" 
"  +  % Listen for results of write set lock requests:                                                                                                                                  \n" 
"    sum t:Transaction .                                                                                                                                                                \n" 
"      sum r:Result .                                                                                                                                                                   \n" 
"        rLockResultWS(n,t,r) . (                                                                                                                                                       \n" 
"          (tr(t,lock) in local)                                                                                                                                                        \n" 
"            -> ((r == abort) ->                                                                                                                                                        \n" 
"			         sUR(n,t,aborted)                                                                                                                                                         \n" 
"				  <> sUR(n,t,committed)                                                                                                                                                         \n" 
"			   )                                                                                                                                                                              \n" 
"               . sSend(n,t,r) .                                                                                                                                                        \n" 
"               NODE_TRANSACTION_MANAGER(n, remove(local, tr(t, lock)))                                                                                                                 \n" 
"            <> NODE_TRANSACTION_MANAGER(n, local)                                                                                                                                      \n" 
"      );                                                                                                                                                                               \n" 
"                                                                                                                                                                                       \n" 
"proc NODE_COMMUNICATION_MANAGER(n:Node) =                                                                                                                                              \n" 
"  sum t:Transaction .                                                                                                                                                                  \n" 
"    rReceive(n, t) .                                                                                                                                                                   \n" 
"    sLock(n, t, w) .                                                                                                                                                                   \n" 
"    sum r:Result .                                                                                                                                                                     \n" 
"      rLockResult(n, t, r) .                                                                                                                                                           \n" 
"      sLockResultWS(n,t,r) .                                                                                                                                                           \n" 
"      NODE_COMMUNICATION_MANAGER(n)                                                                                                                                                    \n" 
"  +                                                                                                                                                                                    \n" 
"  sum t:Transaction .                                                                                                                                                                  \n" 
"    sum r:Result .                                                                                                                                                                     \n" 
"      rReceive(n, t, r) .                                                                                                                                                              \n" 
"      sLockDecision(n,t,r) .                                                                                                                                                           \n" 
"      NODE_COMMUNICATION_MANAGER(n)                                                                                                                                                    \n" 
";                                                                                                                                                                                      \n" 
"                                                                                                                                                                                       \n" 
"                                                                                                                                                                                       \n" 
"map                                                                                                                                                                                    \n" 
"  remove: List(Transaction)#Transaction -> List(Transaction);                                                                                                                          \n" 
"var                                                                                                                                                                                    \n" 
"  LTI : List(Transaction);                                                                                                                                                             \n" 
"  t: Transaction;                                                                                                                                                                      \n" 
"eqn                                                                                                                                                                                    \n" 
"  (#LTI == 0) -> remove(LTI,t) = [];                                                                                                                                                   \n" 
"  (#LTI != 0) -> remove(LTI,t) = if ( head(LTI) == t , remove(tail(LTI),t) , [head(LTI)] ++ remove(tail(LTI),t));                                                                      \n" 
"                                                                                                                                                                                       \n" 
"act removeLocks : Node#Transaction;                                                                                                                                                    \n" 
"    rClearLock, sClearLock, cClearLock:Node#Transaction;                                                                                                                               \n" 
"                                                                                                                                                                                       \n" 
"proc NODE_LOCK_MANAGER(n:Node,                                                                                                                                                         \n" 
"  readLocks:List(Transaction),                                                                                                                                                         \n" 
"  writeLocks:List(Transaction),                                                                                                                                                        \n" 
"  releases:List(Transaction)                                                                                                                                                           \n" 
") =                                                                                                                                                                                    \n" 
"  sum t:Transaction . (                                                                                                                                                                \n" 
"    sum a:Action .                                                                                                                                                                     \n" 
"      rLock(n,t,a) . (                                                                                                                                                                 \n" 
"        (a == r) -> sLockResult(n,t,commit) . NODE_LOCK_MANAGER(n, readLocks ++ [t],writeLocks,releases)                                                                               \n" 
"        <> (                                                                                                                                                                           \n" 
"          NoLocks(n,t) .                                                                                                                                                               \n" 
"          sLockResult(n,t,commit) .                                                                                                                                                    \n" 
"          NODE_LOCK_MANAGER(n, remove(readLocks,t), remove(writeLocks,t) ++ [t], releases)                                                                                             \n" 
"        +                                                                                                                                                                              \n" 
"          NoConflictingLocks(n,t) .                                                                                                                                                    \n" 
"          sLockResult(n,t,commit) .                                                                                                                                                    \n" 
"          NODE_LOCK_MANAGER(n, remove(readLocks, t), remove(writeLocks,t) ++ [t], releases)                                                                                            \n" 
"        +                                                                                                                                                                              \n" 
"          (#remove(readLocks,t) != 0)                                                                                                                                                  \n" 
"          ->  ConflictingLocks(n,t) .                                                                                                                                                  \n" 
"              sRejectLock(n, head(remove(readLocks, t))) .                                                                                                                             \n" 
"              sLockResult(n,t,commit) .                                                                                                                                                \n" 
"              NODE_LOCK_MANAGER(n, remove(readLocks, t), remove(writeLocks,t) ++ [t], releases)                                                                                        \n" 
"          <>  delta                                                                                                                                                                    \n" 
"        )                                                                                                                                                                              \n" 
"      )                                                                                                                                                                                \n" 
"    +                                                                                                                                                                                  \n" 
"    sum r:Result .                                                                                                                                                                     \n" 
"      rLockDecision(n,t,r) .                                                                                                                                                           \n" 
"      NODE_LOCK_MANAGER(n, readLocks, writeLocks, releases ++ [t])                                                                                                                     \n" 
"    +                                                                                                                                                                                  \n" 
"	rClearLock(n,t) . NODE_LOCK_MANAGER(n,                                                                                                                                                \n" 
"	  remove(readLocks,t),                                                                                                                                                                \n" 
"	  remove(writeLocks,t),                                                                                                                                                               \n" 
"	  remove(releases,t)                                                                                                                                                                  \n" 
"	)                                                                                                                                                                                     \n" 
"	+                                                                                                                                                                                     \n" 
"    (t in writeLocks && t in releases) ->                                                                                                                                              \n" 
"      removeLocks(n,t) . NODE_LOCK_MANAGER(n,                                                                                                                                          \n" 
"        remove(readLocks,t),                                                                                                                                                           \n" 
"        remove(writeLocks,t),                                                                                                                                                          \n" 
"        remove(releases,t)                                                                                                                                                             \n" 
"      )                                                                                                                                                                                \n" 
"    <> delta                                                                                                                                                                           \n" 
"  );                                                                                                                                                                                   \n" 
"                                                                                                                                                                                       \n" 
"init                                                                                                                                                                                   \n" 
"  allow({                                                                                                                                                                              \n" 
"      cUQ,                                                                                                                                                                             \n" 
"      sUR,                                                                                                                                                                             \n" 
"      cSend,                                                                                                                                                                           \n" 
"      cReceive,                                                                                                                                                                        \n" 
"      cLockResultWS,                                                                                                                                                                   \n" 
"      cLock,                                                                                                                                                                           \n" 
"      cLockResult,                                                                                                                                                                     \n" 
"      cLockDecision,                                                                                                                                                                   \n" 
"      cRejectLock,                                                                                                                                                                     \n" 
"      NoLocks, NoConflictingLocks, ConflictingLocks, removeLocks, cClearLock                                                                                                           \n" 
"    }                                                                                                                                                                                  \n" 
"    ,comm({                                                                                                                                                                            \n" 
"      rUQ | sUQ -> cUQ,                                                                                                                                                                \n" 
"      rUR | sUR -> cUR,                                                                                                                                                                \n" 
"                                                                                                                                                                                       \n" 
"      rSend | sSend -> cSend,                                                                                                                                                          \n" 
"      rReceive | sReceive -> cReceive,                                                                                                                                                 \n" 
"      rLockResultWS | sLockResultWS -> cLockResultWS,                                                                                                                                  \n" 
"      rLock | sLock -> cLock,                                                                                                                                                          \n" 
"      rLockResult | sLockResult -> cLockResult,                                                                                                                                        \n" 
"      rLockDecision | sLockDecision -> cLockDecision,                                                                                                                                  \n" 
"      rRejectLock | sRejectLock -> cRejectLock,                                                                                                                                        \n" 
"	  rClearLock | sClearLock -> cClearLock                                                                                                                                               \n" 
"      },                                                                                                                                                                               \n" 
"      USER([t1,t2]) || NETWORK([],[]) || NODE(N1) || NODE(N2)                                                                                                                          \n" 
"    )                                                                                                                                                                                  \n" 
"  );                                                                                                                                                                                   \n" 
; 

void test_pbes()
{
  specification spec    = mcrl22lps(SPECIFICATION);
  state_formula formula = mcf2statefrm(FORMULA2, spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  pbes_expression e = p.equations().front().formula();

  BOOST_CHECK(!p.is_bes());
  BOOST_CHECK(!e.is_bes());

  try
  {
    p.load("non-existing file");
    BOOST_CHECK(false); // loading is expected to fail
  }
  catch (std::runtime_error e)
  {
  }

  try
  {
    aterm t = atermpp::make_term("f(x)");
    std::string filename = "write_to_named_text_file.pbes";
    atermpp::write_to_named_text_file(t, filename);
    p.load(filename);
    BOOST_CHECK(false); // loading is expected to fail
  }
  catch (std::runtime_error e)
  {
  }
  p.save("pbes_test_file.pbes");
  p.load("pbes_test_file.pbes");
}

void test_normalize()
{
  using namespace pbes_expr;

  pbes_expression x = propositional_variable_instantiation("x:X");
  pbes_expression y = propositional_variable_instantiation("y:Y");
  pbes_expression z = propositional_variable_instantiation("z:Z");
  pbes_expression f; 
  pbes_expression f1;
  pbes_expression f2;

  f = not_(not_(x));
  f1 = normalize(f);
  f2 = x;
  std::cout << "f  = " << f  << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f = imp(not_(x), y);
  f1 = normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << f  << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = not_(and_(not_(x), not_(y)));
  f1 = normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = imp(and_(not_(x), not_(y)), z);
  f1 = normalize(f);
  f2 = or_(or_(x, y), z);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  x = data_variable("x:X");
  y = data_variable("y:Y");
  z = data_variable("z:Z");

  f  = not_(x);
  f1 = normalize(f);
  f2 = data_expr::not_(x);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = imp(and_(x, y), z);
  f1 = normalize(f);
  f2 = or_(or_(data_expr::not_(x), data_expr::not_(y)), z);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  pbes_expression T = true_();
  pbes_expression F = false_();
  x = pbes_expression(gsMakePBESImp(T, F));
  y = normalize(x);
  std::cout << "x = " << x << std::endl;
  std::cout << "y = " << y << std::endl;

  data_variable_list ab;
  ab = push_front(ab, data_variable("s:S"));
  x = propositional_variable_instantiation("x:X");
  y = and_(x, imp(pbes_expression(gsMakePBESAnd(false_(), false_())), false_()));
  z = normalize(y);
  std::cout << "y = " << y << std::endl;
  std::cout << "z = " << z << std::endl;
}

void test_normalize2()
{
  // test case from Aad Mathijssen, 2/11/2008
  specification spec     = mcrl22lps("init tau + tau;");
  state_formula formula  = mcf2statefrm("nu X. [true]X", spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  p.normalize();
  std::cout << "HIEROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO" << std::endl;
}

// void test_xyz_generator()
// {
//   XYZ_identifier_generator generator(propositional_variable("X1(d:D)"));
//   identifier_string x;
//   x = generator(); BOOST_CHECK(std::string(x) == "X");
//   x = generator(); BOOST_CHECK(std::string(x) == "Y");
//   x = generator(); BOOST_CHECK(std::string(x) == "Z");
//   x = generator(); BOOST_CHECK(std::string(x) == "X0");
//   x = generator(); BOOST_CHECK(std::string(x) == "Y0");
//   x = generator(); BOOST_CHECK(std::string(x) == "Z0");
//   x = generator(); BOOST_CHECK(std::string(x) == "Y1"); // X1 should be skipped
// }

void test_free_variables()
{
  pbes<> p;
  try {
    p.load("abp_fv.pbes");
    atermpp::set< data_variable > freevars = p.free_variables();
    cout << freevars.size() << endl;
    BOOST_CHECK(freevars.size() == 20);
    for (atermpp::set< data_variable >::iterator i = freevars.begin(); i != freevars.end(); ++i)
    {
      cout << "<var>" << pp(*i) << endl;
    }
    freevars = p.free_variables();
    BOOST_CHECK(freevars.size() == 15);
  }
  catch (std::runtime_error e)
  {
    cout << e.what() << endl;
    BOOST_CHECK(false); // loading is expected to succeed
  }
}

void test_pbes_expression_builder()
{
  specification mpsu_spec = mcrl22lps(MPSU_SPECIFICATION);
  state_formula mpsu_formula = mcf2statefrm(MPSU_FORMULA, mpsu_spec);
  bool timed = false;
  pbes<> p = lps2pbes(mpsu_spec, mpsu_formula, timed);

  for (atermpp::vector<pbes_equation>::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
  {
    const pbes_expression& q = i->formula();
    pbes_expression_builder builder;
    pbes_expression q1 = builder.visit(q);
    BOOST_CHECK(q == q1);
  }
}

void test_quantifier_rename_builder()
{
  using namespace pbes_expr;
  namespace d = data_expr; 

  data_variable mN("m:N");
  data_variable nN("n:N");

  pbes_expression f = d::equal_to(mN, nN);
  pbes_expression g = d::not_equal_to(mN, nN);

  multiset_identifier_generator generator(make_list(identifier_string("n00"), identifier_string("n01")));

  pbes_expression p1 = 
  and_(
    forall(make_list(nN), exists(make_list(nN), f)),
    forall(make_list(mN), exists(make_list(mN, nN), g))
  );
  pbes_expression q1 = make_quantifier_rename_builder(generator).visit(p1); 
  std::cout << "p1 = " << pp(p1) << std::endl;
  std::cout << "q1 = " << pp(q1) << std::endl;

  pbes_expression p2 = 
  and_(
    forall(make_list(nN), exists(make_list(nN), p1)),
    forall(make_list(mN), exists(make_list(mN, nN), q1))
  );
  pbes_expression q2 = rename_quantifier_variables(p2, make_list(data_variable("n00:N"), data_variable("n01:N")));
  std::cout << "p2 = " << pp(p2) << std::endl;
  std::cout << "q2 = " << pp(q2) << std::endl;

  // BOOST_CHECK(false);
}

void test_complement_method_builder()
{
  using namespace pbes_expr;
  namespace d = data_expr;

  data_variable X("x:X");
  data_variable Y("y:Y");

  pbes_expression p = or_(and_(X,Y), and_(Y,X));
  pbes_expression q = and_(or_(d::not_(X), d::not_(Y)), or_(d::not_(Y),d::not_(X)));
  std::cout << "p             = " << pp(p) << std::endl;
  std::cout << "q             = " << pp(q) << std::endl;
  std::cout << "complement(p) = " << pp(complement(p)) << std::endl;
  BOOST_CHECK(complement(p) == q);
}

void test_pbes_expression()
{
  namespace p = pbes_expr;
  namespace d = data_expr;

  data_variable x1("x1:X");
  pbes_expression e = p::val(x1);
  data_expression x2 = accessors::val_arg(e);
  BOOST_CHECK(x1 == x2);
  
  pbes_expression v_expr = propositional_variable_instantiation("v:V");
  propositional_variable_instantiation v1 = v_expr;
  propositional_variable_instantiation v2(v_expr);
}

void test_trivial()
{
  specification spec    = mcrl22lps(ABP_SPECIFICATION);
  state_formula formula = mcf2statefrm(TRIVIAL_FORMULA, spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());
}

void test_lps2pbes()
{
  std::string SPECIFICATION;
  std::string FORMULA;
  specification spec;
  state_formula formula;
  pbes<> p;
  bool timed = false;

  SPECIFICATION =
  "act a;                                  \n"
  "proc X(n : Nat) = (n > 2) -> a. X(n+1); \n"
  "init X(3);                              \n"
  ; 
  FORMULA = "true => false";
  spec    = mcrl22lps(SPECIFICATION);
  formula = mcf2statefrm(FORMULA, spec);
  p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  SPECIFICATION =
  "act a : Nat;                           \n" 
  "proc X(n:Nat) = (n>2) -> a(n). X(n+1); \n"
  "init X(3);                             \n"
  ;
  FORMULA = "nu X. (X && forall m:Nat. [a(m)]false)";
  spec    = mcrl22lps(SPECIFICATION);
  formula = mcf2statefrm(FORMULA, spec);
  p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  SPECIFICATION =
  "act a;         \n"
  "proc X = a. X; \n"
  "init X;        \n"
  ; 
  FORMULA =
  "(                                 \n"
  "  ( mu A. [!a]A)                  \n"
  "||                                \n"
  "  ( mu B. exists t3:Pos . [!a]B ) \n"
  ")                                 \n"
  ;
  spec    = mcrl22lps(SPECIFICATION);
  formula = mcf2statefrm(FORMULA, spec);
  p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  specification model = mcrl22lps(MODEL);
  state_formula req1_1 = mcf2statefrm(REQ1_1, model);
  state_formula req1_2 = mcf2statefrm(REQ1_2, model);
  pbes<> p1 = lps2pbes(model, req1_1, timed);
  pbes<> p2 = lps2pbes(model, req1_2, timed);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  test_normalize(); 
  test_normalize2();
  test_lps2pbes();
  test_trivial();
  test_pbes();
  // test_xyz_generator();
  // test_free_variables();
  test_pbes_expression_builder();
  test_quantifier_rename_builder();
  test_complement_method_builder();
  test_pbes_expression();

  return 0;
}
