#ifndef UTILS_H
#define UTILS_H

namespace Utils {
  struct Vect {
    double x;
    double y;
  };

  Vect operator+(Vect v1, Vect v2);
  Vect operator-(Vect v1, Vect v2);
  Vect operator*(double s, Vect v);
  Vect operator/(Vect v, double s);
  
  float vecLength(Vect v);
  float angDiff(Vect v1, Vect v2);
  float signum(float f);
}
#endif
