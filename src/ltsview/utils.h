#ifndef UTILS_H
#define UTILS_H
#include <math.h>
#include <vector>
#include <algorithm>
//using namespace std;

namespace Utils {
  const double PI = 3.14159265359;
  const double tau = 1.50;

  enum RankStyle {ITERATIVE,CYCLIC};
  enum MarkStyle {NO_MARKS,MARK_DEADLOCKS,MARK_STATES,MARK_TRANSITIONS};
  enum VisStyle	 {CONES,TUBES,ATOMIUM};
  enum DFSState  {DFS_WHITE,DFS_GREY,DFS_BLACK};

  struct RGB_Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
  }; 

  const RGB_Color RGB_WHITE = {255,255,255};
  const RGB_Color RGB_BLUE  = {0,0,255};
  const RGB_Color RGB_RED   = {255,0,0};
  const RGB_Color RGB_YELLOW= {255,255,0};

  struct HSV_Color {
    int h;
    int s;
    int v;
  };

  struct Point3D {
    float x;
    float y;
    float z;
  };

  struct VisSettings {
    float     ellipsoidThreshold;
    float     nodeSize;
    int              branchRotation;
    int              innerBranchTilt;
    int              outerBranchTilt;
    int              quality;
    unsigned char alpha;
    bool      longInterpolation;
    RGB_Color downEdgeColor;
    RGB_Color interpolateColor1;
    RGB_Color interpolateColor2;
    RGB_Color markedColor;
    RGB_Color stateColor;
    RGB_Color upEdgeColor;
//    float     backpointerCurve;
//    int            branchScale;
//    int            branchSpread;
//    float     clusterHeight;
//    bool      levelDividers;
  };
  
  struct MarkRule {
    int		            paramIndex;
    bool	            isActivated;
    bool        	    isNegated;
    std::vector< bool >     valueSet;
  };
  
  struct Slot {
    int   occupying;  //INV: 0 <= occupying 
    int   under_consideration; 
    // Number of slot we are placing. Position this slot will be placed in is 
    // P = 2 * PI s / S - 1/2 * total_size + under_consideration * total_size /
    //     (n + 1)) fmod 2 * PI
    float total_size;
  };

  struct Vect {
    float x;
    float y;
  };

  int round_to_int(double f);
  float deg_to_rad(float deg);
  float rad_to_deg(float rad);
  
  bool operator==(RGB_Color c1,RGB_Color c2);
  bool operator!=(RGB_Color c1,RGB_Color c2);
  HSV_Color operator+(HSV_Color c1,HSV_Color c2);
  HSV_Color RGB_to_HSV(RGB_Color c);
  RGB_Color HSV_to_RGB(HSV_Color c);
  HSV_Color interpolate(HSV_Color hsv1,HSV_Color hsv2,float r,bool l);

  Point3D operator+(Point3D p1,Point3D p2);
  Point3D operator-(Point3D p1,Point3D p2);
  Point3D operator*(float s,Point3D p);
  Vect operator+(Vect v1,Vect v2);
  float length(Point3D p);
  void normalize(Point3D &p);
  float dot_product(Point3D p1,Point3D p2);
  Point3D cross_product(Point3D p1,Point3D p2);
  float vec_to_ang(Vect v); //RET: atan2f(Vect.y, Vect.x)
  Vect ang_to_vec(float phi); //RET: V, such that V.x = cos(phi) /\ V.y = sin(phi)
  float vec_length(Vect v); //RET: || v ||

}
#endif
