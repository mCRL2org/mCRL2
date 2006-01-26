#include "utils.h"

bool Utils::operator==( VisSettings vs1, VisSettings vs2 )
{
  return  vs1.backgroundColor	== vs2.backgroundColor
      &&  fabs( vs1.backpointerCurve - vs2.backpointerCurve ) < 0.1
//      &&  vs1.branchRotation	== vs2.branchRotation
//      &&  vs1.branchScale	== vs2.branchScale
      &&  vs1.branchSpread	== vs2.branchSpread
      &&  fabs( vs1.clusterHeight - vs2.clusterHeight ) < 0.1
      &&  vs1.downEdgeColor	== vs2.downEdgeColor
      &&  vs1.interpolateColor1 == vs2.interpolateColor1
      &&  vs1.interpolateColor2 == vs2.interpolateColor2
      &&  vs1.levelDividers	== vs2.levelDividers
      &&  vs1.longInterpolation	== vs2.longInterpolation
      &&  vs1.markedColor	== vs2.markedColor
      &&  fabs( vs1.nodeSize - vs2.nodeSize ) < 0.1
      &&  vs1.outerBranchTilt	== vs2.outerBranchTilt
      &&  vs1.quality	  	== vs2.quality
      &&  vs1.stateColor	== vs2.stateColor
      &&  vs1.transparency	== vs2.transparency
      &&  vs1.upEdgeColor	== vs2.upEdgeColor;
}

bool Utils::operator!=( VisSettings vs1, VisSettings vs2 )
{
  return !( vs1 == vs2 );
}

bool Utils::operator==( RGBColor c1, RGBColor c2 )
{
  return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

bool Utils::operator!=( RGBColor c1, RGBColor c2 )
{
  return !( c1 == c2 );
}

int Utils::round( double f )
{
  double intpart;
  double fracpart = modf( f, &intpart );
  int result = static_cast< int >(intpart) + ( (fracpart > 0.5) ? 1 : 0 );
  return result;
}
