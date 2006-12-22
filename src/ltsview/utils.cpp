#include "utils.h"
#include <iostream>
  
bool Utils::operator==(RGB_Color c1,RGB_Color c2) {
  return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

bool Utils::operator!=(RGB_Color c1,RGB_Color c2) {
  return !(c1 == c2);
}

Utils::Point3D Utils::operator+(Point3D p1,Point3D p2) {
  Point3D result = { p1.x+p2.x, p1.y+p2.y, p1.z+p2.z };
  return result;
}

Utils::Point3D Utils::operator-(Point3D p1,Point3D p2) {
  Point3D result = { p1.x-p2.x, p1.y-p2.y, p1.z-p2.z };
  return result;
}

Utils::Point3D Utils::operator*(float s,Point3D p) {
  Point3D result = { s*p.x, s*p.y, s*p.z };
  return result;
}

Utils::Vect Utils::operator+(Vect v1, Vect v2) {
  Vect result;
  result.x = v1.x + v2.x;
  result.y = v1.y + v2.y;
  return result;
}
float Utils::length(Point3D p) {
  return sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
}

void Utils::normalize(Point3D &p) {
  float len = length( p );
  if (len != 0.0f) {
    p = (1.0f / len) * p;
  }
}

float Utils::dot_product(Point3D p1,Point3D p2) {
  return (p1.x*p2.x + p1.y*p2.y + p1.z*p2.z);
}

Utils::Point3D Utils::cross_product(Point3D p1,Point3D p2) {
  Point3D result = { p1.y*p2.z - p1.z*p2.y,
                     p1.z*p2.x - p1.x*p2.z,
                     p1.x*p2.y - p1.y*p2.x };
  return result;
}

Utils::HSV_Color Utils::operator+(HSV_Color c1,HSV_Color c2) {
  HSV_Color result = { c1.h+c2.h, c1.s+c2.s, c1.v+c2.v };
  while (result.h < 0)    result.h += 360;
  while (result.h >= 360) result.h -= 360;
  if (result.s < 0)       result.s = 0;
  if (result.s > 100)     result.s = 100;
  if (result.v < 0)       result.v = 0;
  if (result.v > 100)     result.v = 100;
  return result;
}

Utils::HSV_Color Utils::interpolate(HSV_Color hsv1,HSV_Color hsv2,float r,bool l) {
  HSV_Color result;
  int delta_h1 = hsv2.h - hsv1.h;
  int delta_h2 = 360 - abs(delta_h1);
  if (delta_h1 >= 0) {
    delta_h2 = delta_h1 - 360;
  }
  else {
    delta_h2 = delta_h1 + 360;
  }
  if (l) {
    if (abs(delta_h1) < abs(delta_h2)) {
      result.h = round_to_int(hsv1.h + r*delta_h2);
    }
    else {
      result.h = round_to_int(hsv1.h + r*delta_h1);
    }
  }
  else {
    if (fabs(delta_h1) < fabs(delta_h2)) {
      result.h = round_to_int(hsv1.h + r*delta_h1);
    }
    else {
      result.h = round_to_int(hsv1.h + r*delta_h2);
    }
  }
  while (result.h < 0) {
    result.h += 360;
  } 
  while (result.h >= 360) {
    result.h -= 360;
  }
  result.s = round_to_int(hsv1.s + r*(hsv2.s-hsv1.s));
  result.v = round_to_int(hsv1.v + r*(hsv2.v-hsv1.v));
  return result;
}

float Utils::deg_to_rad(float deg) {
  return deg * Utils::PI / 180.0f;
}

float Utils::rad_to_deg(float rad) {
  return rad * 180.0f / Utils::PI;
}

Utils::HSV_Color Utils::RGB_to_HSV(RGB_Color c) {
  unsigned char MIN = std::min(c.r,std::min(c.g,c.b));
  unsigned char MAX = std::max(c.r,std::max(c.g,c.b));
  
  HSV_Color result;
  if (MAX == MIN) {
    result.h = 0;
  } 
  else if (MAX == c.r) {
    if (c.g >= c.b) {
      result.h = round_to_int(60.0 * double(c.g-c.b)/double(MAX-MIN));
    }
    else {
      result.h = round_to_int(60.0 * double(c.g-c.b)/double(MAX-MIN)) + 360;
    }
  }
  else if (MAX == c.g) {
    result.h = round_to_int(60.0 * double(c.b-c.r)/double(MAX-MIN)) + 120;
  }
  else {
    result.h = round_to_int(60.0 * double(c.r-c.g)/double(MAX-MIN)) + 240;
  }

  if (MAX == 0) {
    result.s = 0;
  } else {
    result.s = round_to_int(100 * (1.0 - double(MIN)/double(MAX)));
  }
  result.v = round_to_int(double(MAX)/2.55);
  return result;
}

Utils::RGB_Color Utils::HSV_to_RGB(HSV_Color c) {
  int hi = (c.h/60) % 6;
  double H = double(c.h);
  double S = double(c.s) / 100;
  double V = double(c.v) / 100;
  double f = H / 60.0 - hi;
  double r,g,b;
  switch (hi) {
    case 0:
      r = V;
      g = V*(1-(1-f)*S);
      b = V*(1-S);
      break;
    case 1:
      r = V*(1-f*S);
      g = V;
      b = V*(1-S);
      break;
    case 2:
      r = V*(1-S);
      g = V;
      b = V*(1-(1-f)*S);
      break;
    case 3:
      r = V*(1-S);
      g = V*(1-f*S);
      b = V;
      break;
    case 4:
      r = V*(1-(1-f)*S);
      g = V*(1-S);
      b = V;
      break;
    case 5:
      r = V;
      g = V*(1-S);
      b = V*(1-f*S);
      break;
    default:
      r = 0;
      g = 0;
      b = 0;
      break;
  }
  RGB_Color result = {
    static_cast<unsigned char>(r*255),
    static_cast<unsigned char>(g*255),
    static_cast<unsigned char>(b*255) };
  return result;
}

int Utils::round_to_int(double f) {
  double intpart;
  modf(f + 0.5,&intpart);
  return static_cast< int > (intpart);
}

float Utils::vec_to_ang(Utils::Vect v) {
  return atan2(v.y, v.x);
}

Utils::Vect Utils::ang_to_vec( float phi) {
  Vect v;
  if (rad_to_deg(phi) < -0.9f) {
    v.x = 0;
    v.y = 0;
  }
  else {
    v.x = cos(phi);
    v.y = sin(phi);
  }

  return v;
}

float Utils::vec_length( Vect v) {
  return sqrt(v.x*v.x + v.y *  v.y);
}
