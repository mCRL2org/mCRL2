#include "wx.hpp" // precompiled headers

#include "algodialog.h"
#include <wx/sizer.h>
#include <wx/notebook.h>

AlgoDialog::AlgoDialog(GLTSGraph* owner, wxWindow* parent)
  : wxDialog(parent, wxID_ANY, wxT("Layout optimization"), wxDefaultPosition, 
             wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)

{
  app = owner;
  
  size_t nrAlgos = app->getNumberOfAlgorithms();
  
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

  if (nrAlgos > 1)
  {
    wxNotebook* nb = new wxNotebook(
                          this, wxID_ANY, wxDefaultPosition,
                          wxDefaultSize, wxNB_TOP);

    for (size_t i = 0; i < nrAlgos; ++i)
    {
      LayoutAlgorithm* algo = app->getAlgorithm(i);
      wxPanel* algoWindow = new wxPanel(nb, wxID_ANY);
      
      algo->setupPane(algoWindow);

      //TODO: Get algorithm name
      nb->AddPage(algoWindow, wxT("Spring embedder"));       
    }
    sizer->Add(nb, 0, wxEXPAND|wxALL, 5);
  }

  else
  {
    LayoutAlgorithm* algo = app->getAlgorithm(0);

    wxPanel* algoWindow = new wxPanel(this, wxID_ANY);
    
    algo->setupPane(algoWindow);
    PushEventHandler(algo);

    sizer->Add(algoWindow, 0, wxEXPAND|wxALL, 5);
  }

    
  SetSizer(sizer);
  Fit();
  Layout();

}
