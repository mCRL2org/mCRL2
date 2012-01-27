// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simbase.cpp

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA) && !defined(__clang__)
#pragma implementation "simbase.h"
#endif

#include <list>
#include "mcrl2/lps/simbase.h"

using namespace std;

SimulatorViewDLLInterface::~SimulatorViewDLLInterface()
{
  if (simdll != NULL)
  {
    simdll->Remove(this);
  }
}

void SimulatorViewDLLInterface::Registered(SimulatorInterface* Simulator)
{
  if (simdll != NULL)
  {
    simdll->SetSimulator(this,Simulator);
  }
}

void SimulatorViewDLLInterface::Unregistered()
{
  if (simdll != NULL)
  {
    simdll->ClearSimulator(this);
  }
}

void SimulatorViewDLLInterface::SetSimViewsDLL(SimViewsDLL* dll)
{
  simdll = dll;
}

SimViewsDLL::~SimViewsDLL()
{
  list<SimulatorInterface*>::iterator j = sims.begin();
  list<SimulatorViewDLLInterface*>::iterator i = views.begin();
  for (; i != views.end(); i++, j++)
  {
    (*i)->SetSimViewsDLL(NULL);
    if ((*j) != NULL)
    {
      (*j)->Unregister(*i);
    }
    delete *i;
  }
}

void SimViewsDLL::Add(SimulatorViewDLLInterface* View, SimulatorInterface* Simulator, bool Register)
{
  views.push_back(View);
  sims.push_back(Simulator);
  if (Register)
  {
    Simulator->Register(View);
  }
}

void SimViewsDLL::Remove(SimulatorViewDLLInterface* View, bool Unregister)
{
  list<SimulatorInterface*>::iterator j = sims.begin();
  list<SimulatorViewDLLInterface*>::iterator i = views.begin();
  for (; i != views.end(); i++, j++)
  {
    if ((*i) == View)
    {
      if (Unregister && ((*j) != NULL))
      {
        (*i)->SetSimViewsDLL(NULL);
        (*j)->Unregister(*i);
      }
      views.erase(i);
      sims.erase(j);
      break;
    }
  }
}

void SimViewsDLL::SetSimulator(SimulatorViewDLLInterface* View, SimulatorInterface* Simulator)
{
  list<SimulatorInterface*>::iterator j = sims.begin();
  list<SimulatorViewDLLInterface*>::iterator i = views.begin();
  for (; i != views.end(); i++, j++)
  {
    if ((*i) == View)
    {
      *j = Simulator;
      break;
    }
  }
}

void SimViewsDLL::ClearSimulator(SimulatorViewDLLInterface* View)
{
  SetSimulator(View,NULL);
}
