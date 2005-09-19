#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "lintool.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "lintool.h"
#include "gslinearise2.h"
#include "libgsparse.h"

void LineariseTool::GetOption(gsToolOption &opt)
{
}


void LineariseTool::SetOption(gsToolOption &opt)
{
}

int LineariseTool::GetState()
{
}


wxImageList *LineariseTool::GetStateIcons()
{
}


sType &LineariseTool::GetArgumentType()
{
}


sType &LineariseTool::GetResultType()
{
}


string LineariseTool::GetDescription(gsObject *obj)
{
	return "Linearise";
}


void LineariseTool::Configure(wxWindow *parent)
{
}


void LineariseTool::About(wxWindow *parent)
{
}

void LineariseTool::LoadFromFile(std::istream &i)
{
}

void LineariseTool::SaveToFile(std::ostream &o)
{
}

void* LineariseTool::Entry()
{
	FILE *SpecStream, *OutFile;

	if ( (SpecStream = fopen(obj_in->GetFilename().c_str(),"r")) == NULL )
	{
		perror("fopen");
		return 0;
	}
	SetAvailable(false);
	if ( (OutFile = fopen(obj_out->GetFilename().c_str(),"w")) == NULL )
	{
		perror("fopen");
		fclose(SpecStream);
		return 0;
	}

	ATermAppl Spec = gsParseSpecification(SpecStream,true);
	Spec = gsLinearise2(Spec,true);
	ATwriteToTextFile((ATerm) Spec,OutFile);

	fclose(OutFile);
	SetModified();
	SetAvailable(true);
	fclose(SpecStream);

	return 0;
}
