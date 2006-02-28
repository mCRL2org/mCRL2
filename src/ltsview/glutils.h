#ifndef GLUTILS_H
#define GLUTILS_H
#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
#endif
#include <vector>
#include <math.h>
#include "utils.h"

using namespace std;
using namespace Utils;

namespace GLUtils
{
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

  void setColor( RGB_Color c, int transp );
  void coloredCylinder( float baserad, float toprad, float height, 
      int slices, RGB_Color basecol, RGB_Color topcol, int transp, bool
      baseclosed, bool topclosed );
}
#endif
