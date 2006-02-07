#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "mediator.h"
#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <GLUT/glut.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glut.h>
#endif
#include "utils.h"
#include "glutils.h"
#include "lts.h"

using namespace Utils;

class Visualizer
{
  private:
    static VisSettings defaultVisSettings;
    GLuint	    displayList;
    LTS*	    lts;
    Mediator*	    mediator;
    RankStyle	    rankStyle;
    bool	    refreshDisplayList;
    float	    structHeight;
    float	    structWidth;
    VisSettings	    visSettings;

    void drawSubtree( Cluster* root, HSV_Color col, HSV_Color delta_col, bool
	topClosed, float &boundWidth, float &boundHeight );
  
  public:
    Visualizer( Mediator* owner );
    ~Visualizer();
    
    void	drawLTS();
    float	getStructureHeight() const;
    float	getStructureWidth() const;
    RankStyle	getRankStyle() const;
    VisSettings getDefaultVisSettings() const;
    VisSettings getVisSettings() const;
    void	positionClusters();
    void	setLTS( LTS* l );
    void	setRankStyle( RankStyle rs );
    void	setVisSettings( VisSettings vs );
};
#endif
