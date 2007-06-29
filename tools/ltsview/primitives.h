// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file primitives.h
/// \brief Add your file description here.

#ifndef PRIMITIVES_H
#define PRIMITIVES_H

extern "C" {
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
#else
# if defined(_WIN32_) || defined(_MSC_VER)
#  include <windows.h>
# endif
# include <GL/gl.h>
# include <GL/glu.h>
#endif
}

/* Abstract base class */

class Primitive {
  public:
    virtual ~Primitive() {}
    virtual void draw() = 0;
    virtual void reshape(int N,float *coss,float *sins) = 0;
};

/* Concrete derived classes */

class P_Sphere : public Primitive {
  public:
    P_Sphere();
    ~P_Sphere();
    void draw();
    void reshape(int N,float *coss,float *sins);
  private:
    GLuint disp_list;
};

class P_SimpleSphere : public Primitive {
  public:
    P_SimpleSphere();
    ~P_SimpleSphere();
    void draw();
    void reshape(int N,float *coss,float *sins);
  private:
    GLuint disp_list;
};

class P_Hemisphere : public Primitive {
  public:
    P_Hemisphere();
    ~P_Hemisphere();
    void draw();
    void reshape(int N,float *coss,float *sins);
  private:
    GLuint disp_list;
};

class P_Disc : public Primitive {
  public:
    P_Disc();
    ~P_Disc();
    void draw();
    void reshape(int N,float *coss,float *sins);
  private:
    GLuint disp_list;
};

class P_Ring : public Primitive {
  public:
    P_Ring(float r);
    ~P_Ring();
    void draw();
    void reshape(int N,float *coss,float *sins);
    float getTopRadius();
  private:
    GLuint disp_list;
    float r_top;
};

// a truncated cone is a ring with a top surface, a bottom surface or both
class P_TruncatedCone : public Primitive {
  public:
    P_TruncatedCone(P_Ring *a_ring,P_Disc *a_disc,bool t,bool b);
    ~P_TruncatedCone();
    void draw();
    void reshape(int N,float *coss,float *sins);
  private:
    P_Ring *ring;
    P_Disc *disc;
    bool top;
		bool bot;
};

// an oblique cone is a cone of which the apex is not necessarily located at a
// right angle to the center of the base.
// creating a P_ObliqueCone with parameter x produces a cone with the base in
// the (x,y)-plane, base radius 1 and apex located at (x,0,1)
class P_ObliqueCone : public Primitive {
  public:
    P_ObliqueCone(float a,float r,float s);
    ~P_ObliqueCone();
    void draw();
    void reshape(int N,float *coss,float *sins);
		void reshape(int N,float *coss,float *sins,float obt);
  private:
		GLuint disp_list;
		float alpha;
		float radius;
		float sign;
};
#endif
