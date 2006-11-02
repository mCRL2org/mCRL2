#ifndef LTSVIEWAPP_H
#define LTSVIEWAPP_H
#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include <string>
#include <iostream>
#include "aterm1.h"
#include "mediator.h"
#include "mainframe.h"
#include "fileloader.h"
#include "glcanvas.h"
#include "visualizer.h"
#include "markstateruledialog.h"
#include "utils.h"

//using namespace std;
//using namespace Utils;

class LTSViewApp : public wxApp, public Mediator
{
  private:
    int		      currentJobNr;
    GLCanvas*	      glCanvas;
    std::vector< std::string >  jobNames;
    LTS*	      lts;
    MainFrame*	      mainFrame;
    int		      numberOfJobs;
    Visualizer*	      visualizer;

    void applyRanking( Utils::RankStyle rs );

  public:
    void	  activateMarkRule( const int index, const bool activate );
    void	  addMarkRule();
    void	  applyDefaultSettings();
    void	  applyMarkStyle( Utils::MarkStyle ms );
    void	  applySettings();
    void	  drawLTS( Utils::Point3D viewpoint );
    void	  editMarkRule( const int index );
    float	  getHalfStructureHeight() const;
    void	  markAction( std::string label );
    void	  notifyRenderingFinished();
    void	  notifyRenderingStarted();
    virtual int   OnExit();
    virtual bool  OnInit();
    void	  openFile( std::string fileName );
    void	  removeMarkRule( const int index );
    void	  setMatchAnyMarkRule( bool b );
    void	  setRankStyle( Utils::RankStyle rs );
    void	  setVisStyle( Utils::VisStyle vs );
    void	  toggleDisplayStates();
    void          toggleDisplayTransitions();
    void          toggleDisplayBackpointers();
    void	  toggleDisplayWireframe();
    void	  unmarkAction( std::string label );
};

DECLARE_APP( LTSViewApp )

#endif
