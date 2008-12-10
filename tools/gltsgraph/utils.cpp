#include "utils.h"
#include <math.h>

namespace Utils
{

Utils::Vect operator+(Vect v1, Vect v2)
{
  Vect result;
  result.x = v1.x + v2.x;
  result.y = v1.y + v2.y;
  return result;
}

Utils::Vect operator-(Vect v1, Vect v2)
{
  Vect result = {v1.x - v2.x, v1.y - v2.y};
  return result;
}

Utils::Vect operator*(double s, Vect v)
{
  Vect result = {s * v.x, s * v.y};
  return result;
}

Utils::Vect operator/(Vect v, double s)
{
  Vect result = {v.x / s, v.y / s};
  return result;
}

float vecLength(Vect v)
{
  return static_cast< float >(sqrt(v.x * v.x + v.y * v.y));
}

float angDiff(Vect v1, Vect v2)
{
  float dotP = static_cast<float>(v1.x * v2.x + v1.y * v2.y);
  float lenP = static_cast<float>(vecLength(v1) * vecLength(v2));

  return acosf(dotP / lenP);

}

float signum(float v)
{
  if (v < 0)
  {
    return -1.0f;
  }
  else
  {
    return 1.0f;
  }
}


}
