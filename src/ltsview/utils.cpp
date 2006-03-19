#include "utils.h"

bool Utils::operator==( VisSettings vs1, VisSettings vs2 )
{
  return  vs1.backgroundColor	== vs2.backgroundColor
      &&  fabs( vs1.backpointerCurve - vs2.backpointerCurve ) < 0.01
//      &&  vs1.branchRotation	== vs2.branchRotation
//      &&  vs1.branchScale	== vs2.branchScale
//      &&  vs1.branchSpread	== vs2.branchSpread
//      &&  fabs( vs1.clusterHeight - vs2.clusterHeight ) < 0.01
      &&  vs1.downEdgeColor	== vs2.downEdgeColor
      &&  vs1.interpolateColor1 == vs2.interpolateColor1
      &&  vs1.interpolateColor2 == vs2.interpolateColor2
//      &&  vs1.levelDividers	== vs2.levelDividers
      &&  vs1.longInterpolation	== vs2.longInterpolation
      &&  vs1.markedColor	== vs2.markedColor
      &&  fabs( vs1.nodeSize - vs2.nodeSize ) < 0.01
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

bool Utils::operator==( RGB_Color c1, RGB_Color c2 )
{
  return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

bool Utils::operator!=( RGB_Color c1, RGB_Color c2 )
{
  return !( c1 == c2 );
}

Utils::HSV_Color Utils::RGBtoHSV( RGB_Color c )
{
  float r = c.r / 255.0f;
  float g = c.g / 255.0f;
  float b = c.b / 255.0f;
  float* minval;
  float* maxval;
  if ( r < g )
  {
    maxval = (b < g) ? &g : &b;
    minval = (r < b) ? &r : &b;
  }
  else
  {
    maxval = (b < r) ? &r : &b;
    minval = (g < b) ? &g : &b;
  }
  
  HSV_Color result;
  float diff = *maxval - *minval;
  unsigned char min_uc = min( c.r, min( c.g, c.b ) );
  unsigned char max_uc = max( c.r, max( c.g, c.b ) );
  if ( max_uc == min_uc )
    result.h = 0.0f;
  else if ( maxval == &r )
    result.h = 60.0f * (g-b) / diff;
  else if ( maxval == &g )
    result.h = 60.0f * (b-r) / diff + 120.0f;
  else
    result.h = 60.0f * (r-g) / diff + 240.0f;

  if ( max_uc == 0 ) 
    result.s = 0.0f;
  else
    result.s = diff / *maxval;
  result.v = *maxval;
  return result;
}

Utils::RGB_Color Utils::HSVtoRGB( HSV_Color c )
{
  int hi = int( c.h / 60.0f ) % 6;
  float f = c.h / 60.0f - hi;
  float p = c.v * ( 1 - c.s );
  float q = c.v * ( 1 - f * c.s );
  float t = c.v * ( 1 - (1 - f) * c.s );
  float fr = 0.0f;
  float fg = 0.0f;
  float fb = 0.0f;
  switch ( hi )
  {
    case 0:
      fr = c.v;
      fg = t;
      fb = p;
      break;
    case 1:
      fr = q;
      fg = c.v;
      fb = p;
      break;
    case 2:
      fr = p;
      fg = c.v;
      fb = t;
      break;
    case 3:
      fr = p;
      fg = q;
      fb = c.v;
      break;
    case 4:
      fr = t;
      fg = p;
      fb = c.v;
      break;
    case 5:
      fr = c.v;
      fg = p;
      fb = q;
      break;
    default:
      break;
  }
  RGB_Color result;
  result.r = (unsigned char) roundToInt( fr * 255.0f );
  result.g = (unsigned char) roundToInt( fg * 255.0f );
  result.b = (unsigned char) roundToInt( fb * 255.0f );
  return result;
}

int Utils::roundToInt( double f )
{
  double intpart;
  modf( f + 0.5, &intpart );
  return static_cast< int > ( intpart );
}
