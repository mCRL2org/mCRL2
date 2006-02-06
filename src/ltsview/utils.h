#ifndef UTILS_H
#define UTILS_H
#include <math.h>

namespace Utils
{
  const double PI = 3.14159265359;

  enum RankStyle { Iterative, Cyclic };

  struct RGB_Color    /* The name is not RGBColor because
                         this leads to a naming conflict with
                         the built-in name RGBColor on 
                         MacOsX (Jan Friso Groote, 27-1-2006) */
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
    RGB_Color  backgroundColor;
    float     backpointerCurve;
//    int	      branchRotation;
//    int	      branchScale;
    int	      branchSpread;
    float     clusterHeight;
    RGB_Color downEdgeColor;
    RGB_Color interpolateColor1;
    RGB_Color interpolateColor2;
    bool      levelDividers;
    bool      longInterpolation;
    RGB_Color markedColor;
    float     nodeSize;
    int	      outerBranchTilt;
    int	      quality;
    RGB_Color stateColor;
    int	      transparency;
    RGB_Color upEdgeColor;
  };

  bool operator==( VisSettings, VisSettings );
  bool operator!=( VisSettings, VisSettings );
  bool operator==( RGB_Color, RGB_Color );
  bool operator!=( RGB_Color, RGB_Color );
  
  template< class T > const T& maximum( const T& a, const T& b )
  {
    return ( a < b ) ? b : a;
  }

  template< class T > const T& minimum( const T& a, const T& b )
  {
    return ( a < b ) ? a : b;
  }

  int roundToInt( double f );
}
#endif
