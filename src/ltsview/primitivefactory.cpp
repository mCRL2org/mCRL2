#include "primitivefactory.h"
#include <math.h>
#include "utils.h"

using namespace Utils;

/* Primitive Factory -------------------------------------------------------- */

PrimitiveFactory::PrimitiveFactory(int accrcy) {
  accuracy = accrcy;
  disc = -1;
  sphere = -1;
  hemisphere = -1;
  simple_sphere = -1;
  
  cos_theta = NULL;
  sin_theta = NULL;
  coneDB = new ConeDB(6,25);
  update_geom_tables();
}

PrimitiveFactory::~PrimitiveFactory() {
  delete coneDB;
  for (unsigned int i=0; i<primitives.size(); ++i) {
    delete primitives[i];
  }
  primitives.clear();
  if (cos_theta != NULL) {
    free(cos_theta);
    cos_theta = NULL;
  }
  if (sin_theta != NULL) {
    free(sin_theta);
    sin_theta = NULL;
  }
}

void PrimitiveFactory::drawPrimitive(int p) {
  primitives[p]->draw();
}

void PrimitiveFactory::drawSimpleSphere() {
  make_simple_sphere();
  primitives[simple_sphere]->draw();
}

int PrimitiveFactory::makeCone(float r,bool topClosed,bool bottomClosed) {
  int result = make_ring(r);
  unsigned char top_bot = 0;
  if (bottomClosed) {
    top_bot |= BOT_BIT;
  }
  if (topClosed) {
    top_bot |= TOP_BIT;
  }
  if (top_bot != 0) {
    P_Ring *ring = dynamic_cast<P_Ring*>(primitives[result]);
    int key = map_cone_radius(r);
    result = coneDB->findCone(key,top_bot);
    if (result == -1) {
      make_disc();
      P_Cone *p = new P_Cone(ring,dynamic_cast<P_Disc*>(primitives[disc]),
          top_bot);
      result = primitives.size();
      coneDB->addCone(key,top_bot,result);
      primitives.push_back(p);
    }
  }
  return result;
}

int PrimitiveFactory::makeHemisphere() {
  if (hemisphere==-1) {
    P_Hemisphere *p = new P_Hemisphere();
    p->reshape(accuracy,cos_theta,sin_theta);
    hemisphere = primitives.size();
    primitives.push_back(p);
  }
  return hemisphere;
}

int PrimitiveFactory::makeSphere() {
  if (sphere==-1) {
    P_Sphere *p = new P_Sphere();
    p->reshape(accuracy,cos_theta,sin_theta);
    sphere = primitives.size();
    primitives.push_back(p);
  }
  return sphere;
}
/*
int PrimitiveFactory::makeTube() {
  return -1;
}
*/
void PrimitiveFactory::setAccuracy(int accrcy) {
  if (accuracy!=accrcy) {
    accuracy = accrcy;
    update_geom_tables();
    update_primitives();
  }
}
  
void PrimitiveFactory::update_geom_tables() {
  cos_theta = (float*)realloc(cos_theta,2*accuracy*sizeof(float));
  sin_theta = (float*)realloc(sin_theta,2*accuracy*sizeof(float));
  
  float d_theta = PI / accuracy;
  float theta = 0.0f;
  for (int i=0; i<2*accuracy; ++i) {
    cos_theta[i] = cos(theta);
    sin_theta[i] = sin(theta);
    theta += d_theta;
  }
}

void PrimitiveFactory::update_primitives() {
  for (unsigned int i=0; i<primitives.size(); ++i) {
    primitives[i]->reshape(accuracy,cos_theta,sin_theta);
  }
}

int PrimitiveFactory::make_ring(float r) {
  int key = map_cone_radius(r);
  int result = coneDB->findCone(key,0);
  if (result == -1) {
    P_Ring *p = new P_Ring(r);
    p->reshape(accuracy,cos_theta,sin_theta);
    result = primitives.size();
    primitives.push_back(p);
    coneDB->addCone(key,0,result);
  }
  return result;
}

void PrimitiveFactory::make_disc() {
  if (disc == -1) {
    P_Disc *p = new P_Disc();
    p->reshape(accuracy,cos_theta,sin_theta);
    disc = primitives.size();
    primitives.push_back(p);
  }
}

void PrimitiveFactory::make_simple_sphere() {
  if (simple_sphere == -1) {
    P_SimpleSphere *p = new P_SimpleSphere();
    simple_sphere = primitives.size();
    primitives.push_back(p);
  }
}

int PrimitiveFactory::map_cone_radius(float r) {
  return round_to_int(r * 100.0);
}
