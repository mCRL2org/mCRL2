#include "visualizer.h"

VisSettings Visualizer::defaultVisSettings =
{
  { 120, 120, 120 }, 0.3f, 100, 1.2f, RGB_WHITE, RGB_WHITE,
  { 0, 0, 255 }, false, false, { 255, 0, 0 }, 0.1f, 30, 12, RGB_WHITE, 40, 
  { 0, 0, 255 }
};

Visualizer::Visualizer( Mediator* owner )
{
  lts = NULL;
  boundingCylH = 0.0f;
  boundingCylW = 0.0f;
  mediator = owner;
  
  // set the visualization settings to default values
  visSettings = defaultVisSettings;
  
  rankStyle = ITERATIVE;
  refreshPrimitives = false;
}

Visualizer::~Visualizer()
{
  for ( unsigned int i = 0 ; i < primitives.size() ; ++i )
  {
    glDeleteLists( primitives[i]->displayList, 1 );
    delete primitives[i];
  }
  primitives.clear();
}

RankStyle Visualizer::getRankStyle() const
{
  return rankStyle;
}

VisSettings Visualizer::getDefaultVisSettings() const
{
  return defaultVisSettings;
}

VisSettings Visualizer::getVisSettings() const
{
  return visSettings;
}

float Visualizer::getHalfStructureHeight() const
{
  if ( lts == NULL ) return 0.0f;
  return visSettings.clusterHeight*( lts->getNumberOfRanks()-1 ) / 2.0f;
}

float Visualizer::getBoundingCylinderHeight() const
{
  return boundingCylH;
}

float Visualizer::getBoundingCylinderWidth() const
{
  return boundingCylW;
}

RGB_Color Visualizer::getBackgroundColor() const
{
  return visSettings.backgroundColor;
}

void Visualizer::setMarkStyle( MarkStyle ms )
{
  markStyle = ms;
  refreshPrimitives = true;
}

void Visualizer::setRankStyle( RankStyle rs )
{
  rankStyle = rs;
  refreshPrimitives = true;
}

void Visualizer::setLTS( LTS* l )
{
  lts = l;
  refreshPrimitives = true;
}

void Visualizer::setVisSettings( VisSettings vs )
{
  if ( visSettings != vs )
  {
    visSettings = vs;
    refreshPrimitives = true;
  }
}

void Visualizer::positionClusters()
{
  // iterate over the ranks in reverse order (bottom-up)
  for ( int rank = lts->getNumberOfRanks() - 1 ; rank >= 0 ; --rank )
  {
    // iterate over the clusters in this rank
    vector< Cluster* > clusters;
    lts->getClustersAtRank( (unsigned int) rank, clusters );
    vector< Cluster* >::iterator clusit;
    
    for ( clusit = clusters.begin() ; clusit != clusters.end() ; ++clusit )
    {
      // compute the size of this cluster and the positions of its descendants
      (**clusit).computeSizeAndDescendantPositions();
    }
  }
  // position the initial state's cluster
  lts->getInitialState()->getCluster()->setPosition( -1 );

  // compute the cluster height that results in a picture with a "nice" aspect
  // ratio
  float ratio = lts->getInitialState()->getCluster()->getSize() / (
      lts->getNumberOfRanks() - 1 );
  defaultVisSettings.clusterHeight = max(1,roundToInt(40.0f * ratio)) / 10.0f;
  visSettings.clusterHeight = defaultVisSettings.clusterHeight;
  
  refreshPrimitives = true;
}

void Visualizer::drawLTS( Point3D viewpoint )
{
  if ( lts == NULL ) return;
  
  if ( refreshPrimitives )
  {
    // refresh necessary
    // delete all primitives
    for ( unsigned int i = 0 ; i < primitives.size() ; ++i )
    {
      glDeleteLists( primitives[i]->displayList, 1 );
      delete primitives[i];
    }
    primitives.clear();
    
    glPushMatrix();
      glLoadIdentity();
      boundingCylH = 0.0f;
      boundingCylW = 0.0f;
      sin_obt = float( sin( visSettings.outerBranchTilt * PI / 180.0 ) );
      cos_obt = float( cos( visSettings.outerBranchTilt * PI / 180.0 ) );

      switch ( markStyle )
      {
	case MARK_DEADLOCKS:
	  drawSubtreeMarkDeadlocks( lts->getInitialState()->getCluster(),
	      true, boundingCylW, boundingCylH );
	  break;
	  
	case MARK_STATES:
	  drawSubtreeMarkStates( lts->getInitialState()->getCluster(), true,
	      boundingCylW, boundingCylH );
	  break;

	case NO_MARKS:
	default:
	  // coloring based on interpolation settings, so compute delta_col,
	  // i.e. the color difference between clusters at consecutive levels in
	  // the structure.
	  RGB_Color rgb1 = visSettings.interpolateColor1;
	  RGB_Color rgb2 = visSettings.interpolateColor2;
	  HSV_Color hsv1 = RGBtoHSV( rgb1 );
	  HSV_Color hsv2 = RGBtoHSV( rgb2 );
	  if ( rgb1.r == rgb1.g && rgb1.g == rgb1.b ) hsv1.h = hsv2.h;
	  if ( rgb2.r == rgb2.g && rgb2.g == rgb2.b ) hsv2.h = hsv1.h;
	  float delta_h1 = hsv2.h - hsv1.h;
	  float delta_h2 = ((delta_h1 < 0.0f) ? 1.0f : -1.0f) * (360.0f - fabs(delta_h1));
	  
	  delta_h1 /= (lts->getNumberOfRanks() - 1);
	  delta_h2 /= (lts->getNumberOfRanks() - 1);
	  
	  HSV_Color delta_col;
	  if ( visSettings.longInterpolation )
	    delta_col.h = ( fabs(delta_h1) < fabs(delta_h2) ) ? delta_h2 : delta_h1;
	  else
	    delta_col.h = ( fabs(delta_h1) < fabs(delta_h2) ) ? delta_h1 : delta_h2;
	  delta_col.s = (hsv2.s - hsv1.s) / (lts->getNumberOfRanks() - 1);
	  delta_col.v = (hsv2.v - hsv1.v) / (lts->getNumberOfRanks() - 1);
	  
	  drawSubtree( lts->getInitialState()->getCluster(), true,
	      boundingCylW, boundingCylH, hsv1, delta_col );
	  break;
      }
    glPopMatrix();
    refreshPrimitives = false;
  }
  
  // compute distance of every primitive to viewpoint
  for ( unsigned int i = 0 ; i < primitives.size() ; ++i )
  {
    Point3D d = 
    {
      primitives[i]->worldCoordinate.x - viewpoint.x,
      primitives[i]->worldCoordinate.y - viewpoint.y,
      primitives[i]->worldCoordinate.z - viewpoint.z
    };
    primitives[i]->distance = sqrt( d.x*d.x + d.y*d.y + d.z*d.z );
  }

  // sort primitives descending based on distance to viewpoint
  sort( primitives.begin(), primitives.end(), Distance_desc() ); 
  
  // draw primitives in sorted order
  for ( unsigned int i = 0 ; i < primitives.size() ; ++i )
  {
    glCallList( primitives[i]->displayList );
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on interpolation settings
void Visualizer::drawSubtree( Cluster* root, bool topClosed, float &boundWidth,
    float &boundHeight, HSV_Color col, HSV_Color delta_col )
{
  if ( !root->hasDescendants() )
  {
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      GLUtils::setColor( HSVtoRGB( col ), visSettings.transparency );
      glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );

    boundWidth = root->getTopRadius();
    boundHeight = 2.0f * root->getTopRadius();
  }
  else
  {
    HSV_Color desccol = 
      { col.h + delta_col.h, col.s + delta_col.s, col.v + delta_col.v };
    if ( desccol.h < 0.0f ) desccol.h += 360.0f;
    else if ( desccol.h >= 360.0f ) desccol.h -= 360.0f;
    
    vector< Cluster* > descendants;
    root->getDescendants( descendants );
    
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	  visSettings.clusterHeight, visSettings.quality, HSVtoRGB( col ),
	  HSVtoRGB( desccol ), visSettings.transparency, topClosed,
	  ( descendants.size() > 1 ) );
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * visSettings.clusterHeight );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );

    vector< Cluster* >::iterator descit;
    for ( descit = descendants.begin() ; descit != descendants.end() ; ++descit )
    {
      Cluster* desc = *descit;
      
      if ( desc->getPosition() < -0.9f )
      {
	// descendant is centered
	glTranslatef( 0.0f, 0.0f, visSettings.clusterHeight );
	
	float descWidth = 0.0f;
	float descHeight = 0.0f;
	
	drawSubtree( desc, false, descWidth, descHeight, desccol, delta_col );
	
	boundWidth = max( boundWidth, descWidth );
	boundHeight = max( boundHeight, descHeight );
	
	glTranslatef( 0.0f, 0.0f, -visSettings.clusterHeight );
      }
      else
      {
	float rad = root->getBaseRadius() * visSettings.branchSpread / 100.0f;
	
	glTranslatef( 0.0f, 0.0f, visSettings.clusterHeight );
	glRotatef( -desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( rad, 0.0f, 0.0f );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

	float descWidth = 0.0f;
	float descHeight = 0.0f;
	
	drawSubtree( desc, true, descWidth, descHeight, desccol, delta_col );
	
	boundWidth = max( boundWidth, rad + descHeight * sin_obt + descWidth *
	    cos_obt );
	boundHeight = max( boundHeight, descHeight * cos_obt + descWidth *
	    sin_obt );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -rad, 0.0f, 0.0f );
	glRotatef( desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( 0.0f, 0.0f, -visSettings.clusterHeight );
      }
    }
    boundWidth = max( boundWidth, root->getTopRadius() );
    boundHeight += visSettings.clusterHeight;
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on marked states
void Visualizer::drawSubtreeMarkStates( Cluster* root, bool topClosed, float
    &boundWidth, float &boundHeight )
{
  if ( !root->hasDescendants() )
  {
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      if ( root->isMarked() )
	GLUtils::setColor( visSettings.markedColor, visSettings.transparency );
      else
	GLUtils::setColor( RGB_WHITE, visSettings.transparency );
      glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );

    boundWidth = root->getTopRadius();
    boundHeight = 2.0f * root->getTopRadius();
  }
  else
  {
    vector< Cluster* > descendants;
    root->getDescendants( descendants );
    
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      if ( root->isMarked() )
      {
	GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	  visSettings.clusterHeight, visSettings.quality, visSettings.markedColor,
	  RGB_WHITE, visSettings.transparency, topClosed, descendants.size() > 1
	  );
      }
      else
      {
	GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	  visSettings.clusterHeight, visSettings.quality, RGB_WHITE, RGB_WHITE,
	  visSettings.transparency, topClosed, descendants.size() > 1 );
      }
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * visSettings.clusterHeight );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
    
    vector< Cluster* >::iterator descit;
    for ( descit = descendants.begin() ; descit != descendants.end() ; ++descit )
    {
      Cluster* desc = *descit;
      
      if ( desc->getPosition() < -0.9f )
      {
	// descendant is centered
	glTranslatef( 0.0f, 0.0f, visSettings.clusterHeight );
	
	float descWidth = 0.0f;
	float descHeight = 0.0f;
	
	drawSubtreeMarkStates( desc, false, descWidth, descHeight );
	
	boundWidth = max( boundWidth, descWidth );
	boundHeight = max( boundHeight, descHeight );
	
	glTranslatef( 0.0f, 0.0f, -visSettings.clusterHeight );
      }
      else
      {
	float rad = root->getBaseRadius() * visSettings.branchSpread / 100.0f;
	
	glTranslatef( 0.0f, 0.0f, visSettings.clusterHeight );
	glRotatef( -desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( rad, 0.0f, 0.0f );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

	float descWidth = 0.0f;
	float descHeight = 0.0f;
	
	drawSubtreeMarkStates( desc, true, descWidth, descHeight );
	
	boundWidth = max( boundWidth, rad + descHeight * sin_obt + descWidth *
	    cos_obt );
	boundHeight = max( boundHeight, descHeight * cos_obt + descWidth *
	    sin_obt );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -rad, 0.0f, 0.0f );
	glRotatef( desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( 0.0f, 0.0f, -visSettings.clusterHeight );
      }
    }
    boundWidth = max( boundWidth, root->getTopRadius() );
    boundHeight += visSettings.clusterHeight;
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on deadlocks
void Visualizer::drawSubtreeMarkDeadlocks( Cluster* root, bool topClosed, float
    &boundWidth, float &boundHeight )
{
  if ( !root->hasDescendants() )
  {
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      if ( root->hasDeadlock() )
	GLUtils::setColor( visSettings.markedColor, visSettings.transparency );
      else
	GLUtils::setColor( RGB_WHITE, visSettings.transparency );
      glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );

    boundWidth = root->getTopRadius();
    boundHeight = 2.0f * root->getTopRadius();
  }
  else
  {
    vector< Cluster* > descendants;
    root->getDescendants( descendants );
    
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      if ( root->hasDeadlock() )
      {
	GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	  visSettings.clusterHeight, visSettings.quality, visSettings.markedColor,
	  RGB_WHITE, visSettings.transparency, topClosed, descendants.size() > 1
	  );
      }
      else
      {
	GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	  visSettings.clusterHeight, visSettings.quality, RGB_WHITE, RGB_WHITE,
	  visSettings.transparency, topClosed, descendants.size() > 1 );
      }
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * visSettings.clusterHeight );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
      
    vector< Cluster* >::iterator descit;
    for ( descit = descendants.begin() ; descit != descendants.end() ; ++descit )
    {
      Cluster* desc = *descit;
      
      if ( desc->getPosition() < -0.9f )
      {
	// descendant is centered
	glTranslatef( 0.0f, 0.0f, visSettings.clusterHeight );
	
	float descWidth = 0.0f;
	float descHeight = 0.0f;
	
	drawSubtreeMarkDeadlocks( desc, false, descWidth, descHeight );
	
	boundWidth = max( boundWidth, descWidth );
	boundHeight = max( boundHeight, descHeight );
	
	glTranslatef( 0.0f, 0.0f, -visSettings.clusterHeight );
      }
      else
      {
	float rad = root->getBaseRadius() * visSettings.branchSpread / 100.0f;
	
	glTranslatef( 0.0f, 0.0f, visSettings.clusterHeight );
	glRotatef( -desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( rad, 0.0f, 0.0f );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

	float descWidth = 0.0f;
	float descHeight = 0.0f;
	
	drawSubtreeMarkDeadlocks( desc, true, descWidth, descHeight );
	
	boundWidth = max( boundWidth, rad + descHeight * sin_obt + descWidth *
	    cos_obt );
	boundHeight = max( boundHeight, descHeight * cos_obt + descWidth *
	    sin_obt );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -rad, 0.0f, 0.0f );
	glRotatef( desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( 0.0f, 0.0f, -visSettings.clusterHeight );
      }
    }
    boundWidth = max( boundWidth, root->getTopRadius() );
    boundHeight += visSettings.clusterHeight;
  }
}
