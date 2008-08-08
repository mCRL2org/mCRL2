#ifndef ALGO_DIALOG_H
#define ALGO_DIALOG_H

#include <wx/dialog.h>

#include "ltsgraph.h"

class AlgoDialog : public wxDialog 
{
  public:
    AlgoDialog(LTSGraph* owner, wxWindow* parent);
  
  private:
    LTSGraph* app;

};

#endif //ALGO_DIALOG_H
