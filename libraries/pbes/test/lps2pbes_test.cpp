// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2pbes_test.cpp
/// \brief Add your file description here.

// Test program for timed lps2pbes.

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/core/text_utility.h"
#include "test_specifications.h"

using namespace std;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::modal;
using namespace mcrl2::modal::detail;
using namespace mcrl2::pbes_system;
namespace fs = boost::filesystem;

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

const std::string TRIVIAL_FORMULA  = "[true*]<true*>true";

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

void test_directory(int argc, char** argv)
{
  BOOST_CHECK(argc > 1);

  // The dummy file test.test is used to extract the full path of the test directory.
  fs::path dummy_path = fs::system_complete( fs::path( argv[1], fs::native ) );
  fs::path dir = dummy_path.branch_path();
  BOOST_CHECK(fs::is_directory(dir));

  fs::directory_iterator end_iter;
  for ( fs::directory_iterator dir_itr(dir); dir_itr != end_iter; ++dir_itr )
  {
    if ( fs::is_regular( dir_itr->status() ) )
    {
      std::string filename = dir_itr->path().file_string();
      if (boost::ends_with(filename, std::string(".form")))
      {
        std::string timed_result_file   = filename.substr(0, filename.find_last_of('.') + 1) + "expected_timed_result";
        std::string untimed_result_file = filename.substr(0, filename.find_last_of('.') + 1) + "expected_untimed_result";
        std::string formula = core::read_text(filename);
        if (fs::exists(timed_result_file))
        {
          try {
            pbes<> result = lps2pbes(SPEC1, formula, true);
            pbes<> expected_result;
            expected_result.load(timed_result_file);
            bool cmp = (result == expected_result);
            if (!cmp)
              cerr << "ERROR: test " << timed_result_file << " failed!" << endl;
            BOOST_CHECK(cmp);
          }
          catch (std::runtime_error e)
          {
            cerr << e.what() << endl;
          }
        }
        if (fs::exists(untimed_result_file))
        {
          try {
            pbes<> result = lps2pbes(SPEC1, formula, false);
            BOOST_CHECK(result.is_well_typed());
            pbes<> expected_result;
            expected_result.load(untimed_result_file);
            bool cmp = (result == expected_result);
            if (!cmp)
              cerr << "ERROR: test " << untimed_result_file << " failed!" << endl;
            BOOST_CHECK(cmp);
          }
          catch (std::runtime_error e)
          {
            cerr << e.what() << endl;
          }
        }
      }
    }
  }
}

void test_formulas()
{
  std::string SPEC =
  "act a:Nat;                             \n"
  "map smaller: Nat#Nat -> Bool;          \n"
  "var x,y : Nat;                         \n"
  "eqn smaller(x,y) = x < y;              \n"
  "proc P(n:Nat) = sum m: Nat. a(m). P(m);\n"
  "init P(0);                             \n"
  ;

  std::vector<string> formulas;
  formulas.push_back("delay@11");
  formulas.push_back("exists m:Nat. <a(m)>true");
  formulas.push_back("exists p:Nat. <a(p)>true");
  formulas.push_back("forall m:Nat. [a(m)]false");
  formulas.push_back("nu X(n:Nat = 1). [forall m:Nat. a(m)](val(n < 10)  && X(n+2))");
  formulas.push_back("mu X(n:Nat = 1). [forall m:Nat. a(m)](val(smaller(n,10) ) && X(n+2))");
  formulas.push_back("<exists m:Nat. a(m)>true");
  formulas.push_back("<a(2)>[a(0)]false");
  formulas.push_back("<a(2)>true");
  formulas.push_back("[forall m:Nat. a(m)]false");
  formulas.push_back("[a(0)]<a(1)>true");
  formulas.push_back("[a(1)]false");
  formulas.push_back("!true");
  formulas.push_back("yaled@10");
  
  for (std::vector<string>::iterator i = formulas.begin(); i != formulas.end(); ++i)
  {
    std::cout << "<formula>" << *i << std::flush;
    pbes<> result1 = lps2pbes(SPEC, *i, false);   
    std::cout << " <timed>" << std::flush;
    pbes<> result2 = lps2pbes(SPEC, *i, true);   
    std::cout << " <untimed>" << std::endl;
  }
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  //test_lps2pbes();
  //test_trivial();
  //test_directory();
  test_formulas();

  return 0;
}
