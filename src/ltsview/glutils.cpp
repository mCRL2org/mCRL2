#include "glutils.h"

void GLUtils::setColor( RGBColor c, int transp )
{
  GLfloat fc[] = { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, (100-transp) / 100.0f };
  glColor4fv( fc );
}

void GLUtils::coloredCylinder( float baserad, float toprad, float height,
      int slices, int stacks, RGBColor basecol, RGBColor topcol, int transp,
      bool baseclosed, bool topclosed )
{
  float nxg = height;
  float nzg = baserad - toprad;
  float r = sqrt( nxg*nxg + nzg*nzg );
  float alpha = (100-transp) / 100.0f;
  nxg = nxg / r;
  nzg = nzg / r;

  vector< float > ctab;
  vector< float > stab;
  vector< float > nx;
  vector< float > ny;
  
  for ( int i = 0 ; i <= slices ; ++i )
  {
    float alf = 2.0 * i * PI / slices;
    ctab.push_back( cos( alf ) );
    stab.push_back( sin( alf ) );

    nx.push_back( ctab[i] * nxg );
    ny.push_back( stab[i] * nxg );
  }

  float a2 = 0.0f;
  float r2 = baserad;
  float z2 = 0.0f;
  float c2r = basecol.r / 255.0f;
  float c2g = basecol.g / 255.0f;
  float c2b = basecol.b / 255.0f;
  float delta_r = ( topcol.r / 255.0f - c2r ) / stacks;
  float delta_g = ( topcol.g / 255.0f - c2g ) / stacks;
  float delta_b = ( topcol.b / 255.0f - c2b ) / stacks;
  
  float a1;
  float r1;
  float z1;
  float c1r;
  float c1g;
  float c1b;

  if ( baseclosed )
  {
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0.0, 0.0, -1.0 );
    glColor4f( c2r, c2g, c2b, alpha );
    glVertex3f( 0.0, 0.0, 0.0 );
    for ( int j = slices ; j >= 0 ; --j )
    {
      glVertex3f( r2 * ctab[j], r2 * stab[j], z2 );
    }
    glEnd();
  }
  
  for ( int i = 0 ; i < stacks ; ++i )
  {
    a1 = a2;
    r1 = r2;
    z1 = z2;
    c1r = c2r;
    c1g = c2g;
    c1b = c2b;

    a2 = i / float(stacks - 1);
    r2 = (1.0f - a2) * baserad + a2 * toprad;
    z2 = a2 * height;
    c2r += delta_r;
    c2g += delta_g;
    c2b += delta_b;
    
    glBegin( GL_QUAD_STRIP );
    for ( int j = slices ; j >= 0 ; --j )
    {
      glNormal3f( nx[j], ny[j], nzg );
      glColor4f( c1r, c1g, c1b, alpha );
      glVertex3f( r1 * ctab[j], r1 * stab[j], z1 );
      glColor4f( c2r, c2g, c2b, alpha );
      glVertex3f( r2 * ctab[j], r2 * stab[j], z2 );
    }
    glEnd();
  }
  
  if ( topclosed )
  {
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0.0, 0.0, 1.0 );
    glColor4f( c2r, c2g, c2b, alpha );
    glVertex3f( 0.0, 0.0, height );
    for ( int j = 0 ; j <= slices ; ++j )
    {
      glVertex3f( r2 * ctab[j], r2 * stab[j], z2 );
    }
    glEnd();
  }
}

void GLUtils::coloredSphere( float rad, int slices, int stacks, RGBColor color,
    int transp )
{
  /*
  vector< float > sin_tab;
  vector< float > cos_tab;
  for ( int i = 0 ; i <= slices ; ++i )
  {
    float angle = 2.0f * i * PI / slices;
    sin_tab.push_back( sin( angle ) );
    cos_tab.push_back( cos( angle ) );
  }

  vector< float > rad_tab;
  float factor = PI / ( 2 * rad );
  for ( int i = 0 ; i <= stacks ; ++i )
  {
    rad_tab.push_back( rad * sin( i * factor ) );
  }
  
  
  for ( int i = 0 ; i < stacks ; ++i )
  {
    
  }*/
}
