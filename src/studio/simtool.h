#ifndef __simtool_H__
#define __simtool_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "simtool.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

using namespace std;

#include "../xsimmain.h"
#include "tool.h"

class SimulatorTool: public gsTool
{
public:
    // constructors and destructors
   
    // gsTool methods
    virtual void GetOption(gsToolOption &opt);
    virtual void SetOption(gsToolOption &opt);
    virtual void Execute(gsObject *in, gsObject *out, wxWindow *parent);
    virtual bool Executing();
    virtual int GetState();
    virtual wxImageList *GetStateIcons();
    virtual sType &GetArgumentType();
    virtual sType &GetResultType();
    virtual string GetDescription(gsObject *obj);
    virtual void Configure(wxWindow *parent);
    virtual void About(wxWindow *parent);

    virtual void LoadFromFile(std::istream &i); 
    virtual void SaveToFile(std::ostream &o); 

private:
    XSimMain *win;
};

#endif
