#ifndef UTILS_H
#define UTILS_H
#include <math.h>
#include <vector>
#include <algorithm>
using namespace std;

namespace Utils
{
  const double PI = 3.14159265359;

  enum RankStyle { ITERATIVE, CYCLIC };
  enum MarkStyle { NO_MARKS, MARK_DEADLOCKS, MARK_STATES, MARK_TRANSITIONS };

  struct RGB_Color
  {
    float r;
    float g;
    float b;
  }; 

  const RGB_Color RGB_WHITE = { 1.0f, 1.0f, 1.0f };
  const RGB_Color RGB_BLUE  = { 0.0f, 0.0f, 1.0f };
  const RGB_Color RGB_RED   = { 1.0f, 0.0f, 0.0f };

  struct HSV_Color
  {
    float h;
    float s;
    float v;
  };

  struct Point3D
  {
    float x;
    float y;
    float z;
  };

  struct VisSettings
  {
    float     alpha;
//    float     backpointerCurve;
    int	      branchRotation;
//    int	      branchScale;
//    int	      branchSpread;
//    float     clusterHeight;
    RGB_Color downEdgeColor;
    int	      innerBranchTilt;
    RGB_Color interpolateColor1;
    RGB_Color interpolateColor2;
//    bool      levelDividers;
    bool      longInterpolation;
    RGB_Color markedColor;
    bool      newStyle;
    float     nodeSize;
    int	      outerBranchTilt;
    int	      quality;
    RGB_Color stateColor;
    RGB_Color upEdgeColor;
  };

  struct MarkRule
  {
    int		    paramIndex;
    bool	    isActivated;
    bool	    isNegated;
    vector< bool >  valueSet;
  };
  
  bool operator==( RGB_Color c1, RGB_Color c2 );
  bool operator!=( RGB_Color c1, RGB_Color c2 );
  Point3D operator+( Point3D p1, Point3D p2 );
  Point3D operator-( Point3D p1, Point3D p2 );
  Point3D operator*( float s, Point3D p );
  float length( Point3D p );
  HSV_Color operator+( HSV_Color c1, HSV_Color c2 );
  HSV_Color RGB_to_HSV( RGB_Color c );
  RGB_Color HSV_to_RGB( HSV_Color c );
  int round_to_int( double f );
  float deg_to_rad( float deg );
}
#endif
