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

//using namespace std;
//using namespace Utils;

struct Primitive
{
  GLuint  displayList;
  float   distance;
  Utils::Point3D worldCoordinate;
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
    std::vector< float >	  cos_theta1;
    std::vector< float >	  cos_theta2;
    std::vector< float >	  cos_theta1_s;
    std::vector< float >	  cos_theta2_s;
    static Utils::VisSettings	  defaultVisSettings;
    Utils::HSV_Color		  delta_col;
    bool		  displayStates;
    bool		  displayWireframe;
    LTS*		  lts;
    Utils::MarkStyle		  markStyle;
    Mediator*		  mediator;
    std::vector< Primitive* >  primitives;
    Utils::RankStyle		  rankStyle;
    bool		  refreshPrimitives;
    bool		  refreshStates;
    float		  sin_ibt;
    float		  sin_obt;
    std::vector< float >	  sin_theta1;
    std::vector< float >	  sin_theta2;
    std::vector< float >	  sin_theta1_s;
    std::vector< float >	  sin_theta2_s;
    GLuint		  statesDisplayList;
    Utils::Point3D		  viewpoint;
    Utils::VisSettings		  visSettings;
    Utils::VisStyle		  visStyle;

    void computeDeltaCol(Utils::HSV_Color &hsv1);
    void computeSubtreeBounds(Cluster* root,float &boundWidth,
                              float &boundHeight);
    void drawCylinder(float baserad,float toprad,float height,Utils::RGB_Color basecol,
                      Utils::RGB_Color topcol,bool interpolate,bool baseclosed,
                      bool topclosed);
    void drawEllipsoid(float d,float h);
    void drawHemisphere(float rad);
    void drawSphere(float rad);
    void drawSphereState();
    void drawStates(Cluster* root,int rot);
    void drawStatesMark(Cluster* root,int rot);
    void drawSubtreeA(Cluster* root,int rot);
    void drawSubtreeAMark(Cluster* root,int rot);
    void drawSubtreeC(Cluster* root, bool topClosed, Utils::HSV_Color col,int rot);
    void drawSubtreeCMark(Cluster* root,bool topClosed,int rot);
    void drawSubtreeO(Cluster* root, Utils::HSV_Color col,int rot);
    void drawSubtreeOMark(Cluster* root,int rot);
    void drawTube(float baserad,float toprad, Utils::RGB_Color basecol, Utils::RGB_Color topcol,
    							bool interpolate,
                                                        Utils::Point3D b1, Utils::Point3D b2,Utils::Point3D b3,
    							Utils::Point3D &center);
    bool isMarked(Cluster* c);
    bool isMarked(State* s);
    void updateGeometryTables();
  
  public:
    Visualizer( Mediator* owner );
    ~Visualizer();
    
    void	computeBoundsInfo();
    void	computeClusterHeight();
    void	drawLTS( Utils::Point3D viewpoint );
    float	getBoundingCylinderHeight() const;
    float	getBoundingCylinderWidth() const;
    Utils::VisSettings getDefaultVisSettings() const;
    float	getHalfStructureHeight() const;
    Utils::RankStyle	getRankStyle() const;
    Utils::VisSettings getVisSettings() const;
    Utils::VisStyle	getVisStyle() const;
    void	setLTS( LTS* l );
    void	setMarkStyle( Utils::MarkStyle ms );
    void	setRankStyle( Utils::RankStyle rs );
    bool	setVisSettings( Utils::VisSettings vs );
    void	setVisStyle( Utils::VisStyle vs );
    void	toggleDisplayStates();
    void	toggleDisplayWireframe();
};
#endif
