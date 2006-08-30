/* 
   SVC tools -- the SVC (Systems Validation Centre) tool set

   Copyright (C) 2000  Stichting Mathematisch Centrum, Amsterdam,
                       The  Netherlands

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


   $Id: bsim.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "liblts.h"
#include "detail/bsim.h"

using namespace std;

int traceLevel = 0, optimal = 0, classes = 0; 

namespace mcrl2
{
namespace lts
{

bool bisimulation_reduce(lts &l, bool branching, bool add_class_to_state, vector<string> *tau_actions)
{
  if ( add_class_to_state )
  {
    if ( !l.has_state_info() )
    {
      gsErrorMsg("LTS has no state information; cannot add equivalence class information\n");
      return false;
    } else if ( (ATgetType(l.state_value(0)) != AT_APPL) || strcmp(ATgetName(ATgetAFun((ATermAppl) l.state_value(0))),"STATE") )
    {
      gsErrorMsg("unsuitable state information; can only add equivalence class information to mCRL2 states\n");
      return false;
    }
  }

  set_tau_actions(tau_actions);
  int initState = ReadData(l); 
  
  if ( branching )
  {
    SCC();
    ReduceBranching();  
  } else {
    Reduce();
  }

  if ( add_class_to_state )
  {
    WriteDataAddParam(l,branching?ATtrue:ATfalse);
  } else {
    WriteData(l,initState,branching?ATtrue:ATfalse);
  }

  return true;
}

bool bisimulation_compare(lts &l1, lts &l2, bool branching, vector<string> *tau_actions)
{
  int init1, init2;
  set_tau_actions(tau_actions);
  ReadCompareData(l1, &init1, l2, &init2);
  
  bool equal = false;
  if ( branching )
  {
    equal = CompareBranching(init1, init2) == ATtrue;
  } else {
    equal = Compare(init1, init2) == ATtrue;
  }
  return equal;
}

}
}
