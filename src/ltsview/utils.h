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
    unsigned char r;
    unsigned char g;
    unsigned char b;
  }; 

  const RGB_Color RGB_WHITE = { 255, 255, 255 };

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
    RGB_Color backgroundColor;
//    float     backpointerCurve;
    float     branchRotation;
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
  
  bool operator==( RGB_Color, RGB_Color );
  bool operator!=( RGB_Color, RGB_Color );
  
  HSV_Color RGBtoHSV( RGB_Color c );
  RGB_Color HSVtoRGB( HSV_Color c );
  int roundToInt( double f );
  float degToRad( float deg );
}
#endif
