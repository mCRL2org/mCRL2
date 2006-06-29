#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <vector>
#include <math.h>
#include "mediator.h"
#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
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
    float		  cos_ibt;
    float		  cos_obt;
    vector< float >	  cos_theta1;
    vector< float >	  cos_theta2;
    vector< float >	  cos_theta1_s;
    vector< float >	  cos_theta2_s;
    static VisSettings	  defaultVisSettings;
    HSV_Color		  delta_col;
    bool		  displayStates;
    bool		  displayWireframe;
    LTS*		  lts;
    MarkStyle		  markStyle;
    Mediator*		  mediator;
    vector< Primitive* >  primitives;
    RankStyle		  rankStyle;
    bool		  refreshPrimitives;
    bool		  refreshStates;
    float		  sin_ibt;
    float		  sin_obt;
    vector< float >	  sin_theta1;
    vector< float >	  sin_theta2;
    vector< float >	  sin_theta1_s;
    vector< float >	  sin_theta2_s;
    GLuint		  statesDisplayList;
    Point3D		  viewpoint;
    VisSettings		  visSettings;
    VisStyle		  visStyle;

    void computeSubtreeBounds( Cluster* root, float &boundWidth, float
	&boundHeight );
    void drawCylinder( float baserad, float toprad, float height );
    void drawCylinder( float baserad, float toprad, float height, RGB_Color
	basecol, RGB_Color topcol, bool baseclosed, bool topclosed );
    void drawCylinderSplit( float baserad, float toprad, float height, RGB_Color
	basecol, RGB_Color topcol );
    void drawHemisphere( float rad );
    void drawSphere( float rad );
    void drawSphereState();
    void drawStates( Cluster* root, int rot );
    void drawStatesMarkStates( Cluster* root, int rot );
    void drawStatesMarkDeadlocks( Cluster* root, int rot );
    void drawSubtreeA( Cluster* root, int rot );
    void drawSubtreeAMarkDeadlocks( Cluster* root, int rot );
    void drawSubtreeAMarkStates( Cluster* root, int rot );
    void drawSubtreeAMarkTransitions( Cluster* root, int rot );
    void drawSubtreeC( Cluster* root, bool topClosed, HSV_Color col, int rot );
    void drawSubtreeCMarkDeadlocks( Cluster* root, bool topClosed, int rot );
    void drawSubtreeCMarkStates( Cluster* root, bool topClosed, int rot );
    void drawSubtreeCMarkTransitions( Cluster* root, bool topClosed, int rot );
    void drawSubtreeO( Cluster* root, HSV_Color col, int rot );
    void drawSubtreeOMarkDeadlocks( Cluster* root, int rot );
    void drawSubtreeOMarkStates( Cluster* root, int rot );
    void drawSubtreeOMarkTransitions( Cluster* root, int rot );
    void drawTubeInterpolate( float baserad, float toprad, RGB_Color basecol,
	RGB_Color topcol, Point3D b1, Point3D b2, Point3D b3, Point3D &center );
    void drawTubeSplit( float baserad, float toprad, RGB_Color basecol,
	RGB_Color topcol, Point3D b1, Point3D b2, Point3D b3, Point3D &center );
    void myRotatef( float theta, float ax, float ay, float az, float M[] );
    void myTranslatef( float tx, float ty, float tz, float M[] );
    void updateGeometryTables();
  
  public:
    Visualizer( Mediator* owner );
    ~Visualizer();
    
    void	computeBoundsInfo();
    void	computeClusterHeight();
    void	drawLTS( Point3D viewpoint );
    float	getBoundingCylinderHeight() const;
    float	getBoundingCylinderWidth() const;
    VisSettings getDefaultVisSettings() const;
    float	getHalfStructureHeight() const;
    RankStyle	getRankStyle() const;
    VisSettings getVisSettings() const;
    VisStyle	getVisStyle() const;
    void	setLTS( LTS* l );
    void	setMarkStyle( MarkStyle ms );
    void	setRankStyle( RankStyle rs );
    bool	setVisSettings( VisSettings vs );
    void	setVisStyle( VisStyle vs );
    void	toggleDisplayStates();
    void	toggleDisplayWireframe();
};
#endif
