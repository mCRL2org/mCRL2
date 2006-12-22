#ifndef PRIMITIVEFACTORY_H
#define PRIMITIVEFACTORY_H
#include <vector>
#include "primitives.h"
#include "conedb.h"

class PrimitiveFactory {
  public:
    PrimitiveFactory(int accrcy);
    ~PrimitiveFactory();
    void drawPrimitive(int p);
    void drawSimpleSphere();
    int  makeCone(float r,bool topClosed,bool bottomClosed);
    int  makeHemisphere();
    int  makeSphere();
    //int  makeTube();
    void setAccuracy(int accrcy);
  private:
    std::vector<Primitive*> primitives;
    ConeDB  *coneDB;
    int     accuracy;
    int     disc;
    int     simple_sphere;
    int     sphere;
    int     hemisphere;
    float   *cos_theta;
    float   *sin_theta;

    int  make_ring(float r);
    void make_disc();
    void make_simple_sphere();
    int  map_cone_radius(float r);
    void update_geom_tables();
    void update_primitives();
};
#endif
