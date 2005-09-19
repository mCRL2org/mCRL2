#ifndef __mcrl2studio_H__
#define __mcrl2studio_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "mcrl2studio.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class Studio: public wxApp
{
public:
    Studio();
    
    virtual bool OnInit();
    virtual int OnExit();
};

#endif
