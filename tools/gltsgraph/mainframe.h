#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/frame.h>

#ifndef LTSGRAPH_H
  #include "ltsgraph.h"
#else
  class LTSGraph; // Forward declaration
#endif

#ifndef GLCANVAS_H
  #include "glcanvas.h"
#else
  class GLCanvas;
#endif

#ifndef ALGO_DIALOG_H
  #include "algodialog.h"
#else
  class AlgoDialog;
#endif

#include "infodialog.h"

class MainFrame : public wxFrame
{
  public:
    MainFrame(LTSGraph *owner);
    GLCanvas* getGLCanvas(); 
    
    void setLTSInfo(int is, int ns, int nt, int nl);

  private:
    LTSGraph *app;
    GLCanvas *glCanvas;
    AlgoDialog *algoDlg;
    InfoDialog *infoDlg;

    void setupMenuBar();
    void setupMainArea();


    void onOpen(wxCommandEvent& event);
    void onAlgo(wxCommandEvent& event);
    void onInfo(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H
