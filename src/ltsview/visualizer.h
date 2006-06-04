#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <vector>
#include <math.h>
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
#include "lts.h"

using namespace std;
using namespace Utils;

struct Primitive
{
  GLuint  displayList;
  float   distance;
  Point3D worldCoordinate;
};

// class for primitive comparison based on distance
class Distance_desc
{
  public:
    bool operator()( const Primitive*, const Primitive* ) const;
};

class Visualizer
{
  private:
    float		  boundingCylH;
    float		  boundingCylW;
    float		  clusterHeight;
    float		  cos_obt;
    static VisSettings	  defaultVisSettings;
    bool		  displayStates;
    LTS*		  lts;
    MarkStyle		  markStyle;
    Mediator*		  mediator;
    vector< Primitive* >  primitives;
    RankStyle		  rankStyle;
    bool		  refreshPrimitives;
    bool		  refreshStates;
    float		  sin_obt;
    GLuint		  statesDisplayList;
    Point3D		  viewpoint;
    VisSettings		  visSettings;

    void computeSubtreeBounds( Cluster* root, float &boundWidth, float
	&boundHeight );
    void drawCylinder( float baserad, float toprad, RGB_Color basecol, RGB_Color
	topcol, bool baseclosed, bool topclosed );
    void drawHemisphere( float rad, RGB_Color col );
    void drawStates( Cluster* root );
    void drawStatesMarkStates( Cluster* root );
    void drawStatesMarkDeadlocks( Cluster* root );
    void drawSubtree( Cluster* root, bool topClosed, HSV_Color col, HSV_Color
	delta_col );
    void drawSubtreeMarkDeadlocks( Cluster* root, bool topClosed );
    void drawSubtreeMarkStates( Cluster* root, bool topClosed );
    void drawSubtreeMarkTransitions( Cluster* root, bool topClosed );
    void drawTube( float baserad, float toprad, RGB_Color basecol, RGB_Color
	topcol, Point3D b1, Point3D b2, Point3D b3 );
    void myRotatef( float theta, float ax, float ay, float az, float M[] );
    void myTranslatef( float tx, float ty, float tz, float M[] );
    void setColor( RGB_Color c, float alpha );
  
  public:
    Visualizer( Mediator* owner );
    ~Visualizer();
    
    void	computeBoundsInfo();
    void	computeClusterHeight();
    void	drawLTS( Point3D viewpoint );
    RGB_Color	getBackgroundColor() const;
    float	getBoundingCylinderHeight() const;
    float	getBoundingCylinderWidth() const;
    float	getHalfStructureHeight() const;
    RankStyle	getRankStyle() const;
    VisSettings getDefaultVisSettings() const;
    VisSettings getVisSettings() const;
    void	setLTS( LTS* l );
    void	setMarkStyle( MarkStyle ms );
    void	setRankStyle( RankStyle rs );
    bool	setVisSettings( VisSettings vs );
    void	toggleDisplayStates();
};
#endif
