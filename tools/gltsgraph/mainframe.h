#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/frame.h>

#ifndef LTSGRAPH_H
  #include "gltsgraph.h"
#else
  class GLTSGraph; // Forward declaration
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

#ifndef SETTINGS_DIALOG_H
  #include "settingsdialog.h"
#else
  class SettingsDialog;
#endif

#include "infodialog.h"

class MainFrame : public wxFrame
{
  public:
    MainFrame(GLTSGraph *owner);
    GLCanvas* getGLCanvas(); 
    
    void setLTSInfo(int is, int ns, int nt, int nl);

  private:
    GLTSGraph *app;
    GLCanvas *glCanvas;
    AlgoDialog *algoDlg;
    SettingsDialog *settingsDlg;
    InfoDialog *infoDlg;

    void setupMenuBar();
    void setupMainArea();


    void onOpen(wxCommandEvent& event);
    void onQuit(wxCommandEvent& event);
    void onAlgo(wxCommandEvent& event);
    void onSettings(wxCommandEvent& event);
    void onInfo(wxCommandEvent& event);
    void onExport(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H
