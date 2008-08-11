#ifndef ALGO_DIALOG_H
#define ALGO_DIALOG_H

#include <wx/dialog.h>

#include "gltsgraph.h"

class AlgoDialog : public wxDialog 
{
  public:
    AlgoDialog(GLTSGraph* owner, wxWindow* parent);
  
  private:
    GLTSGraph* app;

};

#endif //ALGO_DIALOG_H
