// Author(s): Simona Orzan
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./pbes_simple.h



#define MAXSIZE 1000

typedef struct {
  char op[MAXSIZE]; 
  int arg1[MAXSIZE];
  int arg2[MAXSIZE];
  int nops;
} spbes, *t_pbes_simple;


t_pbes_simple ps;

int pos;


void parsePBES( std::string fileName );
