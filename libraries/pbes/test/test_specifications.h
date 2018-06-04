#ifndef LPS_TEST_SPECIFICATIONS_H
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
#define LPS_TEST_SPECIFICATIONS_H

#include <string>

const std::string SPEC1 =
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

#endif // LPS_TEST_SPECIFICATIONS_H
