#include "glutils.h"

bool GLUtils::Distance_desc::operator()(const Primitive* p1, const Primitive* p2) const
{
  return ( p1->distance > p2->distance );
}

void GLUtils::setColor( RGB_Color c, int transp )
{
  GLfloat fc[] = { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, (100-transp) / 100.0f };
  glColor4fv( fc );
}

void GLUtils::coloredCylinder( float baserad, float toprad, float height,
      int slices, RGB_Color basecol, RGB_Color topcol, int transp,
      bool baseclosed, bool topclosed )
{
  float nxg = height;
  float nzg = baserad - toprad;
  float r = sqrt( nxg*nxg + nzg*nzg );
  nxg = nxg / r;
  nzg = nzg / r;

  vector< float > ctab;
  vector< float > stab;
  vector< float > nx;
  vector< float > ny;
  
  float PI2 = 2.0f * PI;
  float delta = PI2 / slices;
  for ( float alf = 0.0f ; alf <= PI2 ; alf += delta )
  {
    float cos_alf = cos(alf);
    float sin_alf = sin(alf);
    ctab.push_back( cos_alf );
    stab.push_back( sin_alf );
    nx.push_back( cos_alf * nxg );
    ny.push_back( sin_alf * nxg );
  }

  float c1r = basecol.r / 255.0f;
  float c1g = basecol.g / 255.0f;
  float c1b = basecol.b / 255.0f;
  float c2r = topcol.r / 255.0f;
  float c2g = topcol.g / 255.0f;
  float c2b = topcol.b / 255.0f;
  float alpha = (100-transp) / 100.0f;

  if ( baseclosed )
  {
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0.0, 0.0, -1.0 );
    glColor4f( c1r, c1g, c1b, alpha );
    glVertex3f( 0.0, 0.0, 0.0 );
    for ( int j = slices ; j >= 0 ; --j )
    {
      glVertex3f( baserad * ctab[j], baserad * stab[j], 0.0f );
    }
    glEnd();
  }
  
  glBegin( GL_QUAD_STRIP );
  for ( int j = 0 ; j <= slices ; ++j )
  {
    glNormal3f( nx[j], ny[j], nzg );
    glColor4f( c2r, c2g, c2b, alpha );
    glVertex3f( toprad * ctab[j], toprad * stab[j], height );
    glColor4f( c1r, c1g, c1b, alpha );
    glVertex3f( baserad * ctab[j], baserad * stab[j], 0.0f );
  }
  glEnd();

  if ( topclosed )
  {
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0.0, 0.0, 1.0 );
    glColor4f( c2r, c2g, c2b, alpha );
    glVertex3f( 0.0, 0.0, height );
    for ( int j = 0 ; j <= slices ; ++j )
    {
      glVertex3f( toprad * ctab[j], toprad * stab[j], height );
    }
    glEnd();
  }
}
