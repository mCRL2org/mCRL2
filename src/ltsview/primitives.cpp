#include "primitives.h"
#include "utils.h"

using namespace Utils;

/* -------- P_Sphere -------------------------------------------------------- */

P_Sphere::P_Sphere() {
  disp_list = 0;
}

P_Sphere::~P_Sphere() {
  glDeleteLists(disp_list,1);
}

void P_Sphere::draw() {
  glCallList(disp_list);
}

void P_Sphere::reshape(int N,float *coss,float *sins) {
  int i,j,k;
  GLfloat vertices[3*((N-1)*N+2)];
  vertices[0] = 0;
  vertices[1] = 0;
  vertices[2] = -1;
  k = 3;
  for (j=1; j<N; ++j) {
    for (i=0; i<2*N; i+=2) {
      vertices[k]   = sins[j] * coss[i];
      vertices[k+1] = sins[j] * sins[i];
      vertices[k+2] = -coss[j];
      k += 3;
    }
  }
  vertices[k]   = 0;
  vertices[k+1] = 0;
  vertices[k+2] = 1;

  int M = N+2;
  GLuint is_bot[M];
  is_bot[0] = 0;
  is_bot[1] = 1;
  for (i=N; i>=1; --i) {
    is_bot[M-i] = i;
  }
  
  GLuint is_mid[(N-2)*(2*N+2)];
  i = 0;
  for (j=N+1; j<=(N-2)*N+1; j+=N) {
    for (k=0; k<N; ++k) {
      is_mid[i] = j + k;
      is_mid[i+1] = is_mid[i] - N;
      i += 2;
    }
    is_mid[i] = j;
    is_mid[i+1] = is_mid[i] - N;
    i += 2;
  }
  
  GLuint is_top[M];
  j = (N-2)*N;
  is_top[0] = j + N + 1;
  i = 1;
  for (k=j+1; k<=j+N; ++k) {
    is_top[i] = k;
    ++i;
  }
  is_top[i] = j + 1;
  
  if (disp_list == 0) {
    disp_list = glGenLists(1);
  }
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3,GL_FLOAT,0,vertices);
  glNormalPointer(GL_FLOAT,0,vertices);
  glNewList(disp_list,GL_COMPILE);
    glDrawElements(GL_TRIANGLE_FAN,M,GL_UNSIGNED_INT,is_bot);
    glDrawElements(GL_TRIANGLE_FAN,M,GL_UNSIGNED_INT,is_top);
    M += N;
    for (int i=0; i<N-2; ++i) {
      glDrawElements(GL_QUAD_STRIP,M,GL_UNSIGNED_INT,is_mid + i*M);
    }
  glEndList();
}

/* -------- P_SimpleSphere -------------------------------------------------- */

P_SimpleSphere::P_SimpleSphere() {
  GLfloat S = GLfloat(sin(PI/4));
  GLfloat C = GLfloat(cos(PI/4));
  GLfloat vertices[] = { 0,0,-1,
    S,0,-C, 0,-S,-C, -S,0,-C, 0,S,-C,
    1,0, 0, 0,-1, 0, -1,0, 0, 0,1, 0,
    S,0, C, 0,-S, C, -S,0, C, 0,S, C,
    0,0,1 };

  GLuint is_bot[] = { 0,1,4,3,2,1 };
  GLuint is_mid1[] = { 5,1,6,2,7,3,8,4,5,1 };
  GLuint is_mid2[] = { 9,5,10,6,11,7,12,8,9,5 };
  GLuint is_top[] = { 13,9,10,11,12,9 };

  disp_list = glGenLists(1);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3,GL_FLOAT,0,vertices);
  glNormalPointer(GL_FLOAT,0,vertices);
  glNewList(disp_list,GL_COMPILE);
    glDrawElements(GL_TRIANGLE_FAN,6,GL_UNSIGNED_INT,is_bot);
    glDrawElements(GL_QUAD_STRIP,10,GL_UNSIGNED_INT,is_mid1);
    glDrawElements(GL_QUAD_STRIP,10,GL_UNSIGNED_INT,is_mid2);
    glDrawElements(GL_TRIANGLE_FAN,6,GL_UNSIGNED_INT,is_top);
  glEndList();
}

P_SimpleSphere::~P_SimpleSphere() {
  glDeleteLists(disp_list,1);
}

void P_SimpleSphere::draw() {
  glCallList(disp_list);
}

void P_SimpleSphere::reshape(int N,float *coss,float *sins) {
}

/* -------- P_Hemisphere ---------------------------------------------------- */

P_Hemisphere::P_Hemisphere() {
  disp_list = 0;
}

P_Hemisphere::~P_Hemisphere() {
  glDeleteLists(disp_list,1);
}

void P_Hemisphere::draw() {
  glCallList(disp_list);
}

void P_Hemisphere::reshape(int N,float *coss,float *sins) {
  int Ndiv2 = N/2;
  int i,j,k;
  GLfloat vertices[3*(N*Ndiv2+1)];
  k = 0;
  for (j=0; j<Ndiv2; ++j) {
    for (i=0; i<2*N; i+=2) {
      vertices[k]   = coss[j] * coss[i];
      vertices[k+1] = coss[j] * sins[i];
      vertices[k+2] = sins[j];
      k += 3;
    }
  }
  vertices[k]   = 0;
  vertices[k+1] = 0;
  vertices[k+2] = 1;

  GLuint is_mid[N*N+N];
  i = 0;
  for (j=N; j<N+Ndiv2-1; ++j) {
    for (k=0; k<N; ++k) {
      is_mid[i] = j + k;
      is_mid[i+1] = is_mid[i] - N;
      i += 2;
    }
    is_mid[i] = j;
    is_mid[i+1] = is_mid[i] - N;
    i += 2;
  }

  GLuint is_top[N+2];
  is_top[0] = N*Ndiv2;
  i = 1;
  j = (Ndiv2-1)*N;
  for (int k=j; k<j+N; ++k) {
    is_top[i] = k;
    ++i;
  }
  is_top[i] = j;
  
  if (disp_list == 0) {
    disp_list = glGenLists(1);
  }
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3,GL_FLOAT,0,vertices);
  glNormalPointer(GL_FLOAT,0,vertices);
  int M = 2*N + 2;
  glNewList(disp_list,GL_COMPILE);
    glDrawElements(GL_TRIANGLE_FAN,N+2,GL_UNSIGNED_INT,is_top);
    for (int i=0; i<Ndiv2-1; ++i) {
      glDrawElements(GL_QUAD_STRIP,M,GL_UNSIGNED_INT,is_mid + i*M);
    }
  glEndList();
}

/* -------- P_Disc ------------------------------------------------------ */

P_Disc::P_Disc() {
  disp_list = 0;
}

P_Disc::~P_Disc() {
  glDeleteLists(disp_list,1);
}

void P_Disc::draw() {
  glCallList(disp_list);
}

void P_Disc::reshape(int N,float *coss,float *sins) {
  GLfloat vertices[3*N];
  int i,j;
  j = 0;
  for (i=0; i<2*N; i+=2) {
    vertices[j] = GLfloat(coss[i]);
    vertices[j+1] = GLfloat(sins[i]);
    vertices[j+2] = 0;
    j += 3;
  }
  
  if (disp_list == 0) {
    disp_list = glGenLists(1);
  }
  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3,GL_FLOAT,0,vertices);
  glNewList(disp_list,GL_COMPILE);
    glNormal3f(0.0f,0.0f,1.0f);
    glDrawArrays(GL_POLYGON,0,N);
  glEndList();
}

/* -------- P_Ring ------------------------------------------------------ */

P_Ring::P_Ring(float r) {
  disp_list = 0;
  r_top = r;
}

P_Ring::~P_Ring() {
  glDeleteLists(disp_list,1);
}

void P_Ring::draw() {
  glCallList(disp_list);
}

void P_Ring::reshape(int N,float *coss,float *sins) {
  int N3 = 3*N;
  int i,j,k;
  GLfloat vertices[2*N3];
  k = 0;
  j = N3;
  for (i=0; i<2*N; i+=2) {
    vertices[k] = coss[i];
    vertices[k+1] = sins[i];
    vertices[k+2] = -0.5f;
    k += 3;
    vertices[j] = r_top*coss[i];
    vertices[j+1] = r_top*sins[i];
    vertices[j+2] = 0.5f;
    j += 3;
  }

  GLfloat normals[2*N3];
  float z = 1-r_top;
  memcpy(normals,vertices,N3*sizeof(float));
  for (k=2; k<N3; k+=3) {
    normals[k] = z;
  }
  memcpy(normals+N3,normals,N3*sizeof(float));

  GLuint is[2*N+2];
  k = 0;
  for (i=0; i<N; ++i) {
    is[k] = i + N;
    ++k;
    is[k] = i;
    ++k;
  }
  is[k] = N;
  is[k+1] = 0;

  if (disp_list == 0) {
    disp_list = glGenLists(1);
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3,GL_FLOAT,0,vertices);
  glNormalPointer(GL_FLOAT,0,normals);
  glNewList(disp_list,GL_COMPILE);
    glDrawElements(GL_QUAD_STRIP,2*N+2,GL_UNSIGNED_INT,is);
  glEndList();
}

float P_Ring::getTopRadius() {
  return r_top;
}

/* -------- P_Cone ------------------------------------------------------ */

P_Cone::P_Cone(P_Ring *a_ring,P_Disc *a_disc,unsigned char tb) {
  ring = a_ring;
  disc = a_disc;
  top_bot = tb;
}

P_Cone::~P_Cone() {
}

void P_Cone::draw() {
  if (top_bot & BOT_BIT) {
    glPushMatrix();
      glTranslatef(0.0f,0.0f,-0.5f);
      glRotatef(180,1.0f,0.0f,0.0f);
      disc->draw();
    glPopMatrix();
  }
  if (top_bot & TOP_BIT) {
    glPushMatrix();
      glTranslatef(0.0f,0.0f,0.5f);
      glScalef(ring->getTopRadius(),ring->getTopRadius(),1.0f);
      disc->draw();
    glPopMatrix();
  }
  ring->draw();
}

void P_Cone::reshape(int /*N*/,float* /*coss*/,float* /*sins*/) {
}
