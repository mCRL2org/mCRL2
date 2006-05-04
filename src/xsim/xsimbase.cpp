#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsimbase.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <list>
#include "xsimbase.h"

using namespace std;

SimulatorViewDLLInterface::~SimulatorViewDLLInterface()
{
	if ( xsimdll != NULL )
	{
		xsimdll->Remove(this);
	}
}

void SimulatorViewDLLInterface::Registered(SimulatorInterface *Simulator)
{
	if ( xsimdll != NULL )
	{
		xsimdll->SetSimulator(this,Simulator);
	}
}

void SimulatorViewDLLInterface::Unregistered()
{
	if ( xsimdll != NULL )
	{
		xsimdll->ClearSimulator(this);
	}
}

void SimulatorViewDLLInterface::SetXSimViewsDLL(XSimViewsDLL *dll)
{
	xsimdll = dll;
}

XSimViewsDLL::XSimViewsDLL()
{
}

XSimViewsDLL::~XSimViewsDLL()
{
	list<SimulatorInterface *>::iterator j = sims.begin();
	list<SimulatorViewDLLInterface *>::iterator i = views.begin();
	for (; i != views.end(); i++, j++)
	{
		(*i)->SetXSimViewsDLL(NULL);
		if ( (*j) != NULL )
		{
			(*j)->Unregister(*i);
		}
		delete *i;
	}
}

void XSimViewsDLL::Add(SimulatorViewDLLInterface *View, SimulatorInterface *Simulator, bool Register)
{
	views.push_back(View);
	sims.push_back(Simulator);
	if ( Register )
	{
		Simulator->Register(View);
	}
}

void XSimViewsDLL::Remove(SimulatorViewDLLInterface *View, bool Unregister)
{
	list<SimulatorInterface *>::iterator j = sims.begin();
	list<SimulatorViewDLLInterface *>::iterator i = views.begin();
	for (; i != views.end(); i++, j++)
	{
		*i;
		if ( (*i) == View )
		{
			if ( Unregister && ((*j) != NULL) )
			{
				(*i)->SetXSimViewsDLL(NULL);
				(*j)->Unregister(*i);
			}
			views.erase(i);
			sims.erase(j);
			break;
		}
	}
}

void XSimViewsDLL::SetSimulator(SimulatorViewDLLInterface *View, SimulatorInterface *Simulator)
{
	list<SimulatorInterface *>::iterator j = sims.begin();
	list<SimulatorViewDLLInterface *>::iterator i = views.begin();
	for (; i != views.end(); i++, j++)
	{
		if ( (*i) == View )
		{
			*j = Simulator;
			break;
		}
	}
}

void XSimViewsDLL::ClearSimulator(SimulatorViewDLLInterface *View)
{
	SetSimulator(View,NULL);
}
