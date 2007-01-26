#ifndef LTSVIEWAPP_H
#define LTSVIEWAPP_H
#include <wx/wx.h>
#include <string>
#include "mediator.h"
#include "mainframe.h"
#include "glcanvas.h"
#include "visualizer.h"
#include "utils.h"

class LTSViewApp : public wxApp, public Mediator {
  private:
    LTS *lts;
    GLCanvas *glCanvas;
    MainFrame *mainFrame;
    Settings *settings;
    Visualizer *visualizer;
    Utils::RankStyle rankStyle;
    void applyRanking(Utils::RankStyle rs);

  public:
    void	  activateMarkRule(const int index,const bool activate);
    void	  addMarkRule();
    void	  applyMarkStyle(Utils::MarkStyle ms);
    void	  editMarkRule(const int index);
    void	  markAction(std::string label);
    void	  notifyRenderingFinished();
    void	  notifyRenderingStarted();
    virtual int  OnExit();
    virtual bool OnInit();
    void	  openFile(std::string fileName);
    void	  removeMarkRule(const int index);
    void	  setMatchAnyMarkRule(bool b);
    void	  setRankStyle(Utils::RankStyle rs);
    void	  setVisStyle(Utils::VisStyle vs);
    void	  unmarkAction(std::string label);
};

DECLARE_APP(LTSViewApp)

#endif
