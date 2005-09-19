#ifndef __lintool_H__
#define __lintool_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "lintool.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

using namespace std;

#include <wx/thread.h>
#include "object.h"
#include "tool.h"

class LineariseTool: public gsTool
{
public:
    // constructors and destructors
   
    // gsTool methods
    virtual void GetOption(gsToolOption &opt);
    virtual void SetOption(gsToolOption &opt);
    virtual int GetState();
    virtual wxImageList *GetStateIcons();
    virtual sType &GetArgumentType();
    virtual sType &GetResultType();
    virtual string GetDescription(gsObject *obj);
    virtual void Configure(wxWindow *parent);
    virtual void About(wxWindow *parent);

    virtual void LoadFromFile(std::istream &i); 
    virtual void SaveToFile(std::ostream &o); 

    virtual ExitCode Entry();
};

#endif
