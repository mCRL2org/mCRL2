#include "utils.h"
  
bool Utils::operator==( RGB_Color c1, RGB_Color c2 )
{
  return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

bool Utils::operator!=( RGB_Color c1, RGB_Color c2 )
{
  return !( c1 == c2 );
}

Utils::Point3D Utils::operator+( Point3D p1, Point3D p2 )
{
  Point3D result = { p1.x + p2.x, p1.y + p2.y, p1.z + p2.z };
  return result;
}

Utils::Point3D Utils::operator-( Point3D p1, Point3D p2 )
{
  Point3D result = { p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
  return result;
}

Utils::Point3D Utils::operator*( float s, Point3D p )
{
  Point3D result = { s*p.x, s*p.y, s*p.z };
  return result;
}

float Utils::length( Point3D p )
{
  return sqrt( p.x*p.x + p.y*p.y + p.z*p.z );
}

Utils::HSV_Color Utils::operator+( HSV_Color c1, HSV_Color c2 )
{
  HSV_Color result = { c1.h + c2.h, c1.s + c2.s, c1.v + c2.v };
  if ( result.h < 0.0f ) result.h += 360.0f;
  else if ( result.h >= 360.0f ) result.h -= 360.0f;
  return result;
}

float Utils::deg_to_rad( float deg )
{
  return deg * Utils::PI / 180.0f;
}

float Utils::rad_to_deg( float rad )
{
  return rad * 180.0f / Utils::PI;
}

Utils::HSV_Color Utils::RGB_to_HSV( RGB_Color c )
{
  float* minval;
  float* maxval;
  if ( c.r < c.g )
  {
    maxval = (c.b < c.g) ? &c.g : &c.b;
    minval = (c.r < c.b) ? &c.r : &c.b;
  }
  else
  {
    maxval = (c.b < c.r) ? &c.r : &c.b;
    minval = (c.g < c.b) ? &c.g : &c.b;
  }
  
  HSV_Color result;
  float diff = *maxval - *minval;
  float min_uc = min( c.r, min( c.g, c.b ) );
  float max_uc = max( c.r, max( c.g, c.b ) );
  if ( max_uc == min_uc )
    result.h = 0.0f;
  else if ( maxval == &c.r )
    result.h = 60.0f * (c.g-c.b) / diff;
  else if ( maxval == &c.g )
    result.h = 60.0f * (c.b-c.r) / diff + 120.0f;
  else
    result.h = 60.0f * (c.r-c.g) / diff + 240.0f;

  if ( max_uc == 0.0f ) 
    result.s = 0.0f;
  else
    result.s = diff / *maxval;
  result.v = *maxval;
  return result;
}

Utils::RGB_Color Utils::HSV_to_RGB( HSV_Color c )
{
  int hi = int( c.h / 60.0f ) % 6;
  float f = c.h / 60.0f - hi;
  float p = c.v * ( 1 - c.s );
  float q = c.v * ( 1 - f * c.s );
  float t = c.v * ( 1 - (1 - f) * c.s );
  RGB_Color result;
  switch ( hi )
  {
    case 0:
      result.r = c.v;
      result.g = t;
      result.b = p;
      break;
    case 1:
      result.r = q;
      result.g = c.v;
      result.b = p;
      break;
    case 2:
      result.r = p;
      result.g = c.v;
      result.b = t;
      break;
    case 3:
      result.r = p;
      result.g = q;
      result.b = c.v;
      break;
    case 4:
      result.r = t;
      result.g = p;
      result.b = c.v;
      break;
    case 5:
      result.r = c.v;
      result.g = p;
      result.b = q;
      break;
    default:
      break;
  }
  return result;
}

int Utils::round_to_int( double f )
{
  double intpart;
  modf( f + 0.5, &intpart );
  return static_cast< int > (intpart);
}
