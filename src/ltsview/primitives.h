#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#define BOT_BIT 1
#define TOP_BIT 2

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

// a cone is a ring with a top surface, a bottom surface or both
class P_Cone : public Primitive {
  public:
    P_Cone(P_Ring *a_ring,P_Disc *a_disc,unsigned char tb);
    ~P_Cone();
    void draw();
    void reshape(int N,float *coss,float *sins);
  private:
    P_Ring *ring;
    P_Disc *disc;
    unsigned char top_bot;
};
#endif
