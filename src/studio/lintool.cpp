#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "lintool.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "lintool.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libprint_cxx.h"
#include "gslexer.h"
#include "gstypecheck.h"
#include "gsdataimpl.h"
#include "lin_alt.h"

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

	gsEnableConstructorFunctions();
        //parse specification
        gsVerboseMsg("parsing...\n");
        ATermAppl Spec = gsParse(SpecStream);
	fclose(SpecStream);
        if (Spec == NULL) {
          gsErrorMsg("parsing failed\n");
          return 0;
        }
        //type check specification
        gsVerboseMsg("type checking...\n");
        Spec = gsTypeCheck(Spec);
        if (Spec == NULL) {
          gsErrorMsg("type checking failed\n");
          return 0;
        }
        //implement standard data types and type constructors
        gsVerboseMsg("implementing standard data types and type constructors...\n");
        Spec = gsImplementData(Spec);
        if (Spec == NULL) {
          gsErrorMsg("data implementation failed\n");
          return 0;
        }
        t_lin_options lin_options;
	Spec = linearise_alt(Spec, lin_options);
	ATwriteToTextFile((ATerm) Spec,OutFile);

	fclose(OutFile);
	SetModified();
	SetAvailable(true);
	fclose(SpecStream);

	return 0;
}
