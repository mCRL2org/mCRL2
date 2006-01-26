#ifndef GLUTILS_H
#define GLUTILS_H
#ifdef __APPLE__
    #include <OpenGL/gl.h>
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
  void setColor( RGBColor c, int transp );
  void coloredCylinder( float baserad, float toprad, float height, 
      int slices, int stacks, RGBColor basecol, RGBColor topcol, int transp,
      bool baseclosed, bool topclosed );
  void coloredSphere( float rad, int slices, int stacks, RGBColor color, int
      transp );
}
#endif
