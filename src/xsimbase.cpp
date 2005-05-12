#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "xsimbase.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <list>
#include "xsimbase.h"

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
		if ( (*j) != NULL )
		{
			(*i)->SetXSimViewsDLL(NULL);
			(*j)->Unregister(*i);
		}
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
