// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file primitivefactory.cpp
/// \brief Source file for PrimitiveFactory class

#include "primitivefactory.h"
#include "conedb.h"
#include "mathutils.h"
#include "primitives.h"
#include <cmath>
#include <stdlib.h>

using namespace MathUtils;

/* Primitive Factory -------------------------------------------------------- */

PrimitiveFactory::PrimitiveFactory(Settings* ss)
{
  disc = -1;
  sphere = -1;
  hemisphere = -1;
  simple_sphere = -1;
  settings = ss;
  connect(&settings->quality, SIGNAL(changed(int)), this, SLOT(qualityChanged()));
  connect(&settings->branchTilt, SIGNAL(changed(int)), this, SLOT(branchTiltChanged()));

  cos_theta = nullptr;
  sin_theta = nullptr;
  coneDB = new ConeDB();
  update_geom_tables();
}

PrimitiveFactory::~PrimitiveFactory()
{
  delete coneDB;
  for (auto & primitive : primitives)
  {
    delete primitive;
  }
  primitives.clear();
  if (cos_theta != nullptr)
  {
    free(cos_theta);
    cos_theta = nullptr;
  }
  if (sin_theta != nullptr)
  {
    free(sin_theta);
    sin_theta = nullptr;
  }
}

void PrimitiveFactory::drawPrimitive(int p)
{
  primitives[p]->draw();
}

void PrimitiveFactory::drawSimpleSphere()
{
  make_simple_sphere();
  primitives[simple_sphere]->draw();
}

int PrimitiveFactory::makeTruncatedCone(float r,bool topClosed,bool bottomClosed)
{
  int result = make_ring(r);
  if (topClosed || bottomClosed)
  {
    auto* ring = dynamic_cast<P_Ring*>(primitives[result]);
    result = coneDB->findTruncatedCone(r,topClosed,bottomClosed);
    if (result == -1)
    {
      make_disc();
      auto* p = new P_TruncatedCone(ring,
          dynamic_cast<P_Disc*>(primitives[disc]),topClosed,bottomClosed);
      result = static_cast<int>(primitives.size());
      coneDB->addTruncatedCone(r,topClosed,bottomClosed,result);
      primitives.push_back(p);
    }
  }
  return result;
}

int PrimitiveFactory::makeObliqueCone(float a,float r,float s)
{
  int result = coneDB->findObliqueCone(a,r,s);
  if (result == -1)
  {
    auto* p = new P_ObliqueCone(a,r,s);
    p->reshape(settings->quality.value(),cos_theta,sin_theta,deg_to_rad(float(settings->branchTilt.value())));
    result = static_cast<int>(primitives.size());
    primitives.push_back(p);
    oblq_cones.push_back(p);
    coneDB->addObliqueCone(a,r,s,result);
  }
  return result;
}

int PrimitiveFactory::makeHemisphere()
{
  if (hemisphere == -1)
  {
    auto* p = new P_Hemisphere();
    p->reshape(settings->quality.value(),cos_theta,sin_theta);
    hemisphere = static_cast<int>(primitives.size());
    primitives.push_back(p);
  }
  return hemisphere;
}

int PrimitiveFactory::makeSphere()
{
  if (sphere == -1)
  {
    auto* p = new P_Sphere();
    p->reshape(settings->quality.value(),cos_theta,sin_theta);
    sphere = static_cast<int>(primitives.size());
    primitives.push_back(p);
  }
  return sphere;
}

void PrimitiveFactory::update_geom_tables()
{
  int qlt = settings->quality.value();
  cos_theta = static_cast<float*>(realloc(cos_theta,2*qlt*sizeof(float)));
  sin_theta = static_cast<float*>(realloc(sin_theta,2*qlt*sizeof(float)));

  float d_theta = static_cast<float>(PI) / qlt;
  float theta = 0.0f;
  for (int i = 0; i < 2*qlt; ++i)
  {
    cos_theta[i] = std::cos(theta);
    sin_theta[i] = std::sin(theta);
    theta += d_theta;
  }
}

void PrimitiveFactory::update_primitives()
{
  int qlt = settings->quality.value();
  for (auto & primitive : primitives)
  {
    primitive->reshape(qlt,cos_theta,sin_theta);
  }
}

void PrimitiveFactory::update_oblique_cones()
{
  int qlt = settings->quality.value();
  float obt = deg_to_rad(float(settings->branchTilt.value()));
  for (auto & oblq_cone : oblq_cones)
  {
    oblq_cone->reshape(qlt,cos_theta,sin_theta,obt);
  }
}

int PrimitiveFactory::make_ring(float r)
{
  int result = coneDB->findTruncatedCone(r,false,false);
  if (result == -1)
  {
    auto* p = new P_Ring(r);
    p->reshape(settings->quality.value(),cos_theta,sin_theta);
    result = static_cast<int>(primitives.size());
    primitives.push_back(p);
    coneDB->addTruncatedCone(r,false,false,result);
  }
  return result;
}

void PrimitiveFactory::make_disc()
{
  if (disc == -1)
  {
    auto* p = new P_Disc();
    p->reshape(settings->quality.value(),cos_theta,sin_theta);
    disc = static_cast<int>(primitives.size());
    primitives.push_back(p);
  }
}

void PrimitiveFactory::make_simple_sphere()
{
  if (simple_sphere == -1)
  {
    auto* p = new P_SimpleSphere();
    simple_sphere = static_cast<int>(primitives.size());
    primitives.push_back(p);
  }
}

void PrimitiveFactory::qualityChanged()
{
  update_geom_tables();
  update_primitives();
  update_oblique_cones();
}

void PrimitiveFactory::branchTiltChanged()
{
  update_oblique_cones();
}
