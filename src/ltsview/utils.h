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

  struct MarkRule
  {
    int		    paramIndex;
    vector< bool >  valueSet;
  };
  
  bool operator==( VisSettings, VisSettings );
  bool operator!=( VisSettings, VisSettings );
  bool operator==( RGB_Color, RGB_Color );
  bool operator!=( RGB_Color, RGB_Color );
  
  HSV_Color RGBtoHSV( RGB_Color c );
  RGB_Color HSVtoRGB( HSV_Color c );
  int roundToInt( double f );
}
#endif
