// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file primitivefactory.h
/// \brief Add your file description here.

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
    int makeHemisphere();
    int makeObliqueCone(float a,float r,float s);
    int makeSphere();
    int makeTruncatedCone(float r,bool topClosed,bool bottomClosed);
    //int  makeTube();
    void notify(SettingID s);
  private:
    std::vector<Primitive*> primitives;
    std::vector<P_ObliqueCone*> oblq_cones;
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
    void update_geom_tables();
    void update_oblique_cones();
    void update_primitives();
};
#endif
