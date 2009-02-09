// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearization_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <mcrl2/lps/mcrl22lps.h>

using namespace mcrl2::lps;

// Parameter i should be removed
const std::string case_1(
  "act a;\n\n"
  "proc X(i: Nat) = a.X(i);\n\n"
  "init X(2);\n");

// Parameter j should be removed
const std::string case_2(
  "act a: Nat;\n\n"
  "proc X(i,j: Nat) = a(i). X(i,j);\n\n"
  "init X(0,1);\n");

// Parameter j should be removed
const std::string case_3(
  "act a;\n\n"
  "proc X(i,j: Nat)   = (i == 5) -> a. X(i,j);\n\n"
  "init X(0,1);\n");

// Parameter j should be removed
const std::string case_4(
  "act a;\n\n"
  "proc X(i,j: Nat) = a@i.X(i,j);\n\n"
  "init X(0,4);\n");

// Nothing should be removed
const std::string case_5(
  "act a: Nat;\n"
  "act b;\n\n"
  "proc X(i,j,k: Nat) =  a(i).X(k,j,k) +\n"
  "                         b.X(j,j,k);\n\n"
  "init X(1,2,3);");

// Nothing should be removed
const std::string case_6(
  "act act1, act2, act3: Nat;\n\n"
  "proc X(i: Nat)   = (i <  5) -> act1(i).X(i+1) +\n"
  "                   (i == 5) -> act3(i).Y(i, i);\n"
  "     Y(i,j: Nat) = act2(j).Y(i,j+1);\n\n"
  "init X(0);\n");

const std::string case_7(
  "act act1, act2, act3: Nat;\n\n"
  "proc X(i,z,j: Nat)   = (i <  5) -> act1(i)@z.X(i+1,z, j) +\n"
  "                       (i == 5) -> act3(i).X(i, j, 4);\n\n"
  "init X(0,5, 1);\n"
);

const std::string case_8(
   "act a;\n"
   "init sum t:Nat. a@t;\n"
);

void test_multiple_linearization_calls()
{
  specification spec;
  spec = mcrl22lps(case_1);
  spec = mcrl22lps(case_2);
  spec = mcrl22lps(case_3);
  spec = mcrl22lps(case_4);
  spec = mcrl22lps(case_5);
  spec = mcrl22lps(case_6);
  spec = mcrl22lps(case_7);
  spec = mcrl22lps(case_8);
}

void test_struct()
{
  std::string text =
  "sort D = struct d1(Nat)?is_d1 | d2(arg2:Nat)?is_d2;\n"
  "                                                   \n"
  "init true->delta;                                  \n"
  ;
  specification spec = mcrl22lps(text);
}

void test_large_specification()
{
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
  "      cUR,                                                                                                                                                                             \n"
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

  specification model = mcrl22lps(MODEL);
}

int test_main(int argc, char** argv )
{
  MCRL2_ATERM_INIT(argc, argv)

  test_struct();
  test_multiple_linearization_calls();
  test_large_specification();

  return 0;
}
