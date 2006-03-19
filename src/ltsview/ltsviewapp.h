#ifndef LTSVIEWAPP_H
#define LTSVIEWAPP_H
#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/filename.h>
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif
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

using namespace std;
using namespace Utils;

class LTSViewApp : public wxApp, public Mediator
{
  private:
    int		      currentJobNr;
    FileLoader*	      fileLoader;
    GLCanvas*	      glCanvas;
    vector< string >  jobNames;
    LTS*	      lts;
    MainFrame*	      mainFrame;
    int		      numberOfJobs;
    Visualizer*	      visualizer;

    void applyRanking( RankStyle rs );

  public:
    void	  activateMarkRule( const int index, const bool activate );
    void	  addMarkRule();
    void	  applyDefaultSettings();
    void	  applyMarkStyle( MarkStyle ms );
    void	  applySettings();
    void	  drawLTS( Point3D viewpoint );
    void	  editMarkRule( const int index );
    RGB_Color	  getBackgroundColor() const;
    float	  getHalfStructureHeight() const;
    void	  markAction( string label );
    virtual bool  OnInit();
    void	  openFile( string fileName );
    void	  removeMarkRule( const int index );
    void	  setMatchAnyMarkRule( bool b );
    void	  setRankStyle( RankStyle rs );
    void	  unmarkAction( string label );
};

DECLARE_APP( LTSViewApp )

#endif
