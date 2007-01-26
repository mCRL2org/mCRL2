#ifndef PRIMITIVEFACTORY_H
#define PRIMITIVEFACTORY_H
#include <vector>
#include "primitives.h"
#include "conedb.h"
#include "settings.h"

class PrimitiveFactory: public Subscriber {
  public:
    PrimitiveFactory(Settings* ss);
    ~PrimitiveFactory();
    void drawPrimitive(int p);
    void drawSimpleSphere();
    int  makeCone(float r,bool topClosed,bool bottomClosed);
    int  makeHemisphere();
    int  makeSphere();
    //int  makeTube();
    void notify(SettingID s);
  private:
    std::vector<Primitive*> primitives;
    ConeDB  *coneDB;
    Settings *settings;
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
