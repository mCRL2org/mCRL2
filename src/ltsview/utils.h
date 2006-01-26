#ifndef UTILS_H
#define UTILS_H
#include <math.h>

namespace Utils
{
  const double PI = 3.14159265359;

  enum RankStyle { Iterative, Cyclic };

  struct RGBColor
  {
    unsigned char r;
    unsigned char g;
    unsigned char b;
  };

  struct Point3D
  {
    float x;
    float y;
    float z;
  };

  struct VisSettings
  {
    RGBColor  backgroundColor;
    float     backpointerCurve;
//    int	      branchRotation;
//    int	      branchScale;
    int	      branchSpread;
    float     clusterHeight;
    RGBColor  downEdgeColor;
    RGBColor  interpolateColor1;
    RGBColor  interpolateColor2;
    bool      levelDividers;
    bool      longInterpolation;
    RGBColor  markedColor;
    float     nodeSize;
    int	      outerBranchTilt;
    int	      quality;
    RGBColor  stateColor;
    int	      transparency;
    RGBColor  upEdgeColor;
  };

  bool operator==( VisSettings, VisSettings );
  bool operator!=( VisSettings, VisSettings );
  bool operator==( RGBColor, RGBColor );
  bool operator!=( RGBColor, RGBColor );
  
  template< class T > const T& maximum( const T& a, const T& b )
  {
    return ( a < b ) ? b : a;
  }

  template< class T > const T& minimum( const T& a, const T& b )
  {
    return ( a < b ) ? a : b;
  }

  int round( double f );
}
#endif
