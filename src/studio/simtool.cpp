#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "simtool.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "simtool.h"


void SimulatorTool::GetOption(gsToolOption &opt)
{
}


void SimulatorTool::SetOption(gsToolOption &opt)
{
}


void SimulatorTool::Execute(gsObject *in, gsObject *out, wxWindow *parent)
{
	win = new XSimMain(parent,-1,wxT("mCRL2 Simulator"));
	win->LoadFile(in->GetFilename());
	if ( win->GetState() )
	{
		win->Show();
	}
}


bool SimulatorTool::Executing()
{
	return win != NULL;
}


int SimulatorTool::GetState()
{
}


wxImageList *SimulatorTool::GetStateIcons()
{
}


sType &SimulatorTool::GetArgumentType()
{
}


sType &SimulatorTool::GetResultType()
{
}


string SimulatorTool::GetDescription(gsObject *obj)
{
	return "Simulate";
}


void SimulatorTool::Configure(wxWindow *parent)
{
}


void SimulatorTool::About(wxWindow *parent)
{
}

void SimulatorTool::LoadFromFile(std::istream &i)
{
}

void SimulatorTool::SaveToFile(std::ostream &o)
{
}
