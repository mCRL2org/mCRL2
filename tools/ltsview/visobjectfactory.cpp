// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visobjectfactory.cpp
/// \brief Source file for VisObjectFactory class

#include "wx.hpp" // precompiled headers

#include "visobjectfactory.h"
#include <algorithm>
#include <cstdlib>
#include "primitivefactory.h"

extern "C" {
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
#else
# if defined(_WIN32_) || defined(_MSC_VER)
#  include <windows.h>
#  undef __in_range // For STLport
# endif
# include <GL/gl.h>
# include <GL/glu.h>
#endif
}

using namespace std;
using namespace Utils;

class VisObject
{
  public:
    VisObject();
    ~VisObject();
    float* getMatrixP() const;
    RGB_Color getColor() const; //TODO This doesn't seem to be used;
    Point3D getCoordinates() const;
    int getPrimitive() const;
    void setColor(Utils::RGB_Color c);
    void setTextureColours(std::vector<Utils::RGB_Color>& colours);
    void setPrimitive(int p);
    void draw(PrimitiveFactory *pf,unsigned char alpha);
    void drawWithTexture(PrimitiveFactory *pf, unsigned char alpha);
    void addIdentifier(int id);
  private:
    float* matrix;
    RGB_Color color;
    GLuint texName;
    int numColours;
    int primitive;
    vector<int> identifiers;
};

VisObject::VisObject() {
	matrix = (float*)malloc(16*sizeof(float));
	color.r = 150;
	color.g = 150;
	color.b = 150;
        numColours = 0;

        // Generate texture alias for this object.
        glGenTextures(1, &texName);
}

VisObject::~VisObject() {
  glDeleteTextures(1, &texName);
  free(matrix);
}

float* VisObject::getMatrixP() const {
	return matrix;
}

RGB_Color VisObject::getColor() const {
	return color;
}

int VisObject::getPrimitive() const {
	return primitive;
}

Point3D VisObject::getCoordinates() const {
  Point3D result = { matrix[12],matrix[13],matrix[14] };
  return result;
}

void VisObject::setColor(RGB_Color c) {
	color = c;
}

void VisObject::setTextureColours(vector<Utils::RGB_Color>& colours)
{

  if (colours.size() > 0)
  {
    numColours = 1;

    // numColours := smallest power of 2 s.t. colours.size <= numColours,
    // since taking an NP2 greatly reduces performance greatly.
    while ( static_cast<unsigned int>(numColours) < colours.size())
    {
      numColours = numColours << 1;
    }

    GLubyte* texture = (GLubyte*)malloc(4*numColours*sizeof(GLubyte));

    for(int i = 0; i < numColours; ++i)
    {
      int j = i % colours.size();
      texture[4*i]   = colours[j].r;
      texture[4*i+1] = colours[j].g;
      texture[4*i+2] = colours[j].b;
      texture[4*i+3] = 255; // alpha value
    }

    glBindTexture(GL_TEXTURE_1D, texName);
    // GL_TEXTURE_1D is now an alias for texName
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, numColours, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texture);
    free(texture);
  }
  else
  {
    numColours = 0;
  }
}

void VisObject::setPrimitive(int p) {
	primitive = p;
}

void VisObject::draw(PrimitiveFactory *pf,unsigned char alpha) {
	glColor4ub(color.r,color.g,color.b,alpha);
	glPushMatrix();
	glMultMatrixf(matrix);
  for(size_t i = 0; i < identifiers.size(); ++i) {
    glPushName(identifiers[i]);
  }
	pf->drawPrimitive(primitive);
  for(size_t i = 0; i < identifiers.size(); ++i) {
    glPopName();
  }
	glPopMatrix();
}

void VisObject::drawWithTexture(PrimitiveFactory *pf, unsigned char alpha)
{
  if (numColours > 0)
  {

    // Recall all settings stored in texName
    glBindTexture(GL_TEXTURE_1D, texName);

    glEnable(GL_TEXTURE_1D);
  }

  draw(pf, alpha);

  if (numColours > 0)
  {
    glDisable(GL_TEXTURE_1D);
  }
}

void VisObject::addIdentifier(int id)
{
  identifiers.push_back(id);
}
/* -------------------- Distance -------------------------------------------- */

class Distance {
  private:
    Point3D viewpoint;
  public:
    explicit Distance(const Point3D vp) : viewpoint(vp) {}
    bool operator()(const VisObject* o1,const VisObject* o2) const;
};

bool Distance::operator()(const VisObject *o1, const VisObject *o2) const {
  Point3D d1 = o1->getCoordinates() - viewpoint;
  Point3D d2 = o2->getCoordinates() - viewpoint;
  return (dot_product(d1,d1) > dot_product(d2,d2));
}

/* -------------------- VisObjectFactory ------------------------------------ */

VisObjectFactory::VisObjectFactory() {
}

VisObjectFactory::~VisObjectFactory() {
	clear();
}

void VisObjectFactory::sortObjects(Point3D viewpoint) {
	stable_sort(objects_sorted.begin(),objects_sorted.end(),Distance(viewpoint));
}

void VisObjectFactory::drawObjects(PrimitiveFactory *pf,unsigned char alpha,
                                   bool texture) {
  if (texture)
  {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    //glEnable(GL_TEXTURE_1D);
    for (unsigned int i = 0; i < objects_sorted.size(); ++i)
    {
      objects_sorted[i]->drawWithTexture(pf,alpha);
    }
    //glDisable(GL_TEXTURE_1D);
  }
  else
  {
    for (unsigned int i = 0; i < objects_sorted.size(); ++i)
    {
      objects_sorted[i]->drawWithTexture(pf, alpha);
    }
  }
}

void VisObjectFactory::clear() {
	for (unsigned int i = 0; i < objects.size(); ++i) {
		delete objects[i];
	}
	objects.clear();
	objects_sorted.clear();
}

int VisObjectFactory::makeObject(int primitive, vector<int> &ids) {
	VisObject *vo = new VisObject();
	glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat*)vo->getMatrixP());
	vo->setPrimitive(primitive);

  for(size_t i = 0; i < ids.size(); ++i) {
    vo->addIdentifier(ids[i]);
  }

	objects.push_back(vo);
	objects_sorted.push_back(vo);
	return objects.size()-1;
}

void VisObjectFactory::updateObjectMatrix(int obj) {
	glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat*)objects[obj]->getMatrixP());
}

void VisObjectFactory::updateObjectColor(int obj,RGB_Color color) {
	objects[obj]->setColor(color);
}

void VisObjectFactory::updateObjectTexture(int obj, vector<RGB_Color> &colours)
{
  objects[obj]->setTextureColours(colours);
}
