#include "visualizer.h"

bool Distance_desc::operator()(const Primitive* p1, const Primitive* p2) const
{
  return ( p1->distance > p2->distance );
}

VisSettings Visualizer::defaultVisSettings =
{
  0.6f, { 120, 120, 120 }, 0.3f, /*100, 1.2f,*/ RGB_WHITE, RGB_WHITE,
  { 0, 0, 255 }, /*false,*/ false, { 255, 0, 0 }, 0.1f, 30, 12, RGB_WHITE,
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
  refreshStates = false;
  displayStates = false;
  statesDisplayList = 0;
}

Visualizer::~Visualizer()
{
  for ( unsigned int i = 0 ; i < primitives.size() ; ++i )
  {
    glDeleteLists( primitives[i]->displayList, 1 );
    delete primitives[i];
  }
  primitives.clear();
  glDeleteLists( statesDisplayList, 1 );
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
  return clusterHeight*( lts->getNumberOfRanks()-1 ) / 2.0f;
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
  refreshStates = true;
}

void Visualizer::setRankStyle( RankStyle rs )
{
  rankStyle = rs;
  refreshPrimitives = true;
  refreshStates = true;
}

void Visualizer::setLTS( LTS* l )
{
  lts = l;
  refreshPrimitives = true;
  refreshStates = true;
}

// returns whether the bounding cylinder needs to be recalculated
bool Visualizer::setVisSettings( VisSettings vs )
{
  VisSettings oldSettings = visSettings;
  visSettings = vs;
  
  if ( oldSettings.stateColor != vs.stateColor ||
       fabs( oldSettings.nodeSize - vs.nodeSize ) > 0.01f )
  {
    refreshStates = true;
  }
  
  if ( markStyle == NO_MARKS && 
      ( oldSettings.interpolateColor1 != vs.interpolateColor1 ||
        oldSettings.interpolateColor2 != vs.interpolateColor2 ||
        oldSettings.longInterpolation != vs.longInterpolation ) )
  {
    refreshPrimitives = true;
  }
  
  if ( oldSettings.quality != vs.quality ||
       fabs( oldSettings.alpha - vs.alpha ) > 0.01f )
  {
    refreshPrimitives = true;
  }

  if ( markStyle != NO_MARKS && oldSettings.markedColor != vs.markedColor )
  {
    refreshStates = true;
    refreshPrimitives = true;
  }

  if ( oldSettings.outerBranchTilt != vs.outerBranchTilt )
  {
    refreshStates = true;
    refreshPrimitives = true;
    return true;
  }
  return false;
}

void Visualizer::toggleDisplayStates()
{
  displayStates = !displayStates;
}

void Visualizer::computeClusterHeight()
{
  // compute the cluster height that results in a picture with a "nice" aspect
  // ratio
  float ratio = lts->getInitialState()->getCluster()->getSize() / (
      lts->getNumberOfRanks() - 1 );
  clusterHeight = max(1,roundToInt(40.0f * ratio)) / 10.0f;
  
  refreshPrimitives = true;
  refreshStates = true;
}

void Visualizer::drawLTS( Point3D viewpoint )
{
  if ( lts == NULL ) return;

  if ( displayStates && refreshStates )
  {
    glDeleteLists( statesDisplayList, 1 );
    statesDisplayList = glGenLists( 1 );
    glNewList( statesDisplayList, GL_COMPILE );
      switch ( markStyle )
      {
	case MARK_STATES:
	  drawStatesMarkStates( lts->getInitialState()->getCluster() );
	  break;
	case MARK_DEADLOCKS:
	  drawStatesMarkDeadlocks( lts->getInitialState()->getCluster() );
	  break;
	default:
	  setColor( visSettings.stateColor, 1.0f );
	  drawStates( lts->getInitialState()->getCluster() );
	  break;
      }
    glEndList();
    refreshStates = false;
  }
  
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

      switch ( markStyle )
      {
	case MARK_DEADLOCKS:
	  drawSubtreeMarkDeadlocks( lts->getInitialState()->getCluster(),
	      true );
	  break;
	  
	case MARK_STATES:
	  drawSubtreeMarkStates( lts->getInitialState()->getCluster(), true );
	  break;

	case MARK_TRANSITIONS:
	  drawSubtreeMarkTransitions( lts->getInitialState()->getCluster(),
	      true );
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
	  
	  drawSubtree( lts->getInitialState()->getCluster(), true, hsv1,
	      delta_col );
	  break;
      }
    glPopMatrix();
    refreshPrimitives = false;
  }

  // first draw the opaque objects in the scene (if required)
  if ( displayStates )
  {
    glCallList( statesDisplayList );
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

void Visualizer::computeBoundsInfo()
{
  boundingCylH = 0.0f;
  boundingCylW = 0.0f;
  sin_obt = float( sin( visSettings.outerBranchTilt * PI / 180.0 ) );
  cos_obt = float( cos( visSettings.outerBranchTilt * PI / 180.0 ) );
  computeSubtreeBounds( lts->getInitialState()->getCluster(), boundingCylW,
      boundingCylH );
}


// traverses the structure like a draw procedure but does not call any OpenGL
// commands, so nothing is actually drawn to the canvas. Used when we only want
// to compute the bounding cylinder of the structure.
void Visualizer::computeSubtreeBounds( Cluster* root, float &boundWidth, float
    &boundHeight )
{
  if ( !root->hasDescendants() )
  {
    boundWidth = root->getTopRadius();
    boundHeight = 2.0f * root->getTopRadius();
  }
  else
  {
    vector< Cluster* > descendants;
    root->getDescendants( descendants );
    vector< Cluster* >::iterator descit;
    for ( descit = descendants.begin() ; descit != descendants.end() ; ++descit )
    {
      Cluster* desc = *descit;
      
      if ( desc->getPosition() < -0.9f )
      {
	// descendant is centered
	float descWidth = 0.0f;
	float descHeight = 0.0f;
	computeSubtreeBounds( desc, descWidth, descHeight );
	boundWidth = max( boundWidth, descWidth );
	boundHeight = max( boundHeight, descHeight );
      }
      else
      {
	float descWidth = 0.0f;
	float descHeight = 0.0f;
	computeSubtreeBounds( desc, descWidth, descHeight );
	boundWidth = max( boundWidth, root->getBaseRadius() + descHeight *
	    sin_obt + descWidth * cos_obt );
	boundHeight = max( boundHeight, descHeight * cos_obt + descWidth *
	    sin_obt );
      }
    }
    boundWidth = max( boundWidth, root->getTopRadius() );
    boundHeight += clusterHeight;
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on interpolation settings
void Visualizer::drawSubtree( Cluster* root, bool topClosed, HSV_Color col,
    HSV_Color delta_col )
{
  if ( !root->hasDescendants() )
  {
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      setColor( HSVtoRGB( col ), visSettings.alpha );
      glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
      /*Point3D b1 = { 0,0,1 };
      Point3D b2 = { 2,0,1 };
      Point3D b3 = { 2,0,2 };
      drawTube( root->getTopRadius(), root->getTopRadius(), HSVtoRGB( col ),
	  HSVtoRGB( col ), b1, b2, b3 )*/;
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
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
      drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	  HSVtoRGB(col), HSVtoRGB(desccol), topClosed, descendants.size() > 1 );
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * clusterHeight );
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
	glTranslatef( 0.0f, 0.0f, clusterHeight );
	
	drawSubtree( desc, false, desccol, delta_col );
	
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	glTranslatef( 0.0f, 0.0f, clusterHeight );
	glRotatef( -desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, 0.0f );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

	drawSubtree( desc, true, desccol, delta_col );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, 0.0f );
	glRotatef( desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
    }
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on marked states
void Visualizer::drawSubtreeMarkStates( Cluster* root, bool topClosed )
{
  if ( !root->hasDescendants() )
  {
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      if ( root->hasMarkedState() )
	setColor( visSettings.markedColor, visSettings.alpha );
      else
	setColor( RGB_WHITE, visSettings.alpha );
      glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
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
      if ( root->hasMarkedState() )
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    visSettings.markedColor, RGB_WHITE, topClosed, descendants.size() >
	    1 );
      }
      else
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    RGB_WHITE, RGB_WHITE, topClosed, descendants.size() > 1 );
      }
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * clusterHeight );
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
	glTranslatef( 0.0f, 0.0f, clusterHeight );
	
	drawSubtreeMarkStates( desc, false );
	
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	glTranslatef( 0.0f, 0.0f, clusterHeight );
	glRotatef( -desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, 0.0f );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

	drawSubtreeMarkStates( desc, true );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, 0.0f );
	glRotatef( desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
    }
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on deadlocks
void Visualizer::drawSubtreeMarkDeadlocks( Cluster* root, bool topClosed )
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
	setColor( visSettings.markedColor, visSettings.alpha );
      else
	setColor( RGB_WHITE, visSettings.alpha );
      glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
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
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    visSettings.markedColor, RGB_WHITE, topClosed, descendants.size() >
	    1 );
      }
      else
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    RGB_WHITE, RGB_WHITE, topClosed, descendants.size() > 1 );
      }
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * clusterHeight );
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
	glTranslatef( 0.0f, 0.0f, clusterHeight );

	drawSubtreeMarkDeadlocks( desc, false );

	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	glTranslatef( 0.0f, 0.0f, clusterHeight );
	glRotatef( -desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, 0.0f );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

	drawSubtreeMarkDeadlocks( desc, true );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, 0.0f );
	glRotatef( desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
    }
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on marked transitions
void Visualizer::drawSubtreeMarkTransitions( Cluster* root, bool topClosed )
{
  if ( !root->hasDescendants() )
  {
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      if ( root->hasMarkedTransition() )
	setColor( visSettings.markedColor, visSettings.alpha );
      else
	setColor( RGB_WHITE, visSettings.alpha );
      glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
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
      if ( root->hasMarkedTransition() )
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    visSettings.markedColor, RGB_WHITE, topClosed, descendants.size() >
	    1);
      }
      else
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    RGB_WHITE, RGB_WHITE, topClosed, descendants.size() > 1 );
      }
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * clusterHeight );
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
	glTranslatef( 0.0f, 0.0f, clusterHeight );
	drawSubtreeMarkTransitions( desc, false );
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	glTranslatef( 0.0f, 0.0f, clusterHeight );
	glRotatef( -desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, 0.0f );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	
	drawSubtreeMarkTransitions( desc, true );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, 0.0f );
	glRotatef( desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
    }
  }
}

void Visualizer::drawStates( Cluster* root )
{
  vector< State* > c_ss;
  root->getStates( c_ss );
  for ( vector< State* >::iterator s_it = c_ss.begin() ; s_it != c_ss.end() ;
	++s_it )
  {
    if ( (**s_it).getPosition() < -0.9f )
      glutSolidSphere( visSettings.nodeSize, 4, 4 );
    else
    {
      glRotatef( -(**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getTopRadius(), 0.0f, 0.0f );
      glutSolidSphere( visSettings.nodeSize, 4, 4 );
      glTranslatef( -root->getTopRadius(), 0.0f, 0.0f );
      glRotatef( (**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
    }
  }

  vector< Cluster* > descs;
  root->getDescendants( descs );
  for ( vector< Cluster* >::iterator descit = descs.begin() ;
	descit != descs.end() ; ++descit )
  {
    if ( (**descit).getPosition() < -0.9f )
    {
      // descendant is centered
      glTranslatef( 0.0f, 0.0f, clusterHeight );
      drawStates( *descit );
      glTranslatef( 0.0f, 0.0f, -clusterHeight );
    }
    else
    {
      glTranslatef( 0.0f, 0.0f, clusterHeight );
      glRotatef( -(**descit).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getBaseRadius(), 0.0f, 0.0f );
      glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

      drawStates( *descit );
      
      glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
      glTranslatef( -root->getBaseRadius(), 0.0f, 0.0f );
      glRotatef( (**descit).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( 0.0f, 0.0f, -clusterHeight );
    }
  }
}

void Visualizer::drawStatesMarkStates( Cluster* root )
{
  vector< State* > c_ss;
  root->getStates( c_ss );
  for ( vector< State* >::iterator s_it = c_ss.begin() ; s_it != c_ss.end() ;
	++s_it )
  {
    if ( (**s_it).isMarked() )
      setColor( visSettings.markedColor, 1.0f );
    else
      setColor( RGB_WHITE, 1.0f );

    if ( (**s_it).getPosition() < -0.9f )
      glutSolidSphere( visSettings.nodeSize, 4, 4 );
    else
    {
      glRotatef( -(**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getTopRadius(), 0.0f, 0.0f );
      glutSolidSphere( visSettings.nodeSize, 4, 4 );
      glTranslatef( -root->getTopRadius(), 0.0f, 0.0f );
      glRotatef( (**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
    }
  }

  vector< Cluster* > descs;
  root->getDescendants( descs );
  for ( vector< Cluster* >::iterator descit = descs.begin() ;
	descit != descs.end() ; ++descit )
  {
    if ( (**descit).getPosition() < -0.9f )
    {
      // descendant is centered
      glTranslatef( 0.0f, 0.0f, clusterHeight );
      drawStatesMarkStates( *descit );
      glTranslatef( 0.0f, 0.0f, -clusterHeight );
    }
    else
    {
      glTranslatef( 0.0f, 0.0f, clusterHeight );
      glRotatef( -(**descit).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getBaseRadius(), 0.0f, 0.0f );
      glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

      drawStatesMarkStates( *descit );
      
      glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
      glTranslatef( -root->getBaseRadius(), 0.0f, 0.0f );
      glRotatef( (**descit).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( 0.0f, 0.0f, -clusterHeight );
    }
  }
}

void Visualizer::drawStatesMarkDeadlocks( Cluster* root )
{
  vector< State* > c_ss;
  root->getStates( c_ss );
  for ( vector< State* >::iterator s_it = c_ss.begin() ; s_it != c_ss.end() ;
	++s_it )
  {
    if ( (**s_it).isDeadlock() )
      setColor( visSettings.markedColor, 1.0f );
    else
      setColor( RGB_WHITE, 1.0f );

    if ( (**s_it).getPosition() < -0.9f )
      glutSolidSphere( visSettings.nodeSize, 4, 4 );
    else
    {
      glRotatef( -(**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getTopRadius(), 0.0f, 0.0f );
      glutSolidSphere( visSettings.nodeSize, 4, 4 );
      glTranslatef( -root->getTopRadius(), 0.0f, 0.0f );
      glRotatef( (**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
    }
  }

  vector< Cluster* > descs;
  root->getDescendants( descs );
  for ( vector< Cluster* >::iterator descit = descs.begin() ;
	descit != descs.end() ; ++descit )
  {
    if ( (**descit).getPosition() < -0.9f )
    {
      glTranslatef( 0.0f, 0.0f, clusterHeight );
      
      drawStatesMarkDeadlocks( *descit );
      
      glTranslatef( 0.0f, 0.0f, -clusterHeight );
    }
    else
    {
      glTranslatef( 0.0f, 0.0f, clusterHeight );
      glRotatef( -(**descit).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getBaseRadius(), 0.0f, 0.0f );
      glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

      drawStatesMarkDeadlocks( *descit );
      
      glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
      glTranslatef( -root->getBaseRadius(), 0.0f, 0.0f );
      glRotatef( (**descit).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( 0.0f, 0.0f, -clusterHeight );
    }
  }
}

void Visualizer::setColor( RGB_Color c, float alpha )
{
  GLfloat fc[] = { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, alpha };
  glColor4fv( fc );
}

// draws a cylinder around z-axis with given base radius, top radius, height,
// color at base, color at top, closed base if baseclosed and closed top if
// topclosed
void Visualizer::drawCylinder( float baserad, float toprad, RGB_Color basecol,
      RGB_Color topcol, bool baseclosed, bool topclosed )
{
  int slices = visSettings.quality;
  float nxg = clusterHeight;
  float nzg = baserad - toprad;
  float r = sqrt( nxg*nxg + nzg*nzg );
  nxg = nxg / r;
  nzg = nzg / r;

  vector< float > ctab;
  vector< float > stab;
  vector< float > nx;
  vector< float > ny;
  
  float delta = 2.0f * PI / slices;
  float alf = 0.0f;
  for ( int i=0 ; i <= slices ; ++i )
  {
    float cos_alf = cos(alf);
    float sin_alf = sin(alf);
    ctab.push_back( cos_alf );
    stab.push_back( sin_alf );
    nx.push_back( cos_alf * nxg );
    ny.push_back( sin_alf * nxg );
    alf += delta;
  }

  float c1r = basecol.r / 255.0f;
  float c1g = basecol.g / 255.0f;
  float c1b = basecol.b / 255.0f;
  float c2r = topcol.r / 255.0f;
  float c2g = topcol.g / 255.0f;
  float c2b = topcol.b / 255.0f;

  if ( baseclosed )
  {
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0.0, 0.0, -1.0 );
    glColor4f( c1r, c1g, c1b, visSettings.alpha );
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
    glColor4f( c2r, c2g, c2b, visSettings.alpha );
    glVertex3f( toprad * ctab[j], toprad * stab[j], clusterHeight );
    glColor4f( c1r, c1g, c1b, visSettings.alpha );
    glVertex3f( baserad * ctab[j], baserad * stab[j], 0.0f );
  }
  glEnd();

  if ( topclosed )
  {
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0.0, 0.0, 1.0 );
    glColor4f( c2r, c2g, c2b, visSettings.alpha );
    glVertex3f( 0.0, 0.0, clusterHeight );
    for ( int j = 0 ; j <= slices ; ++j )
    {
      glVertex3f( toprad * ctab[j], toprad * stab[j], clusterHeight );
    }
    glEnd();
  }
}

void Visualizer::drawHemisphere( float r, RGB_Color col )
{
  int n = visSettings.quality;
  int i,j;
  
  // rotate around the z-axis first to ensure that one vertex of the base of the
  // hemisphere is always at angle 0 in the (x,y)-plane (i.e. in point (r,0,0)).
  // This way, the hemisphere will always fit nicely onto other objects in the
  // scene (like cylinders) no matter what the value of n is.
  float rot = 0.0f;
  while ( rot <= 90.0f ) rot += 360.0f / (float)n;
  rot -= 360.0f / (float)n;
  glRotatef( 90 - rot, 0.0f, 0.0f, 1.0f );

  // precompute the cosines and sines that are needed during drawing and put
  // them in vectors (this is done for efficiency: every (co)sine used is
  // now computed exactly once).
  vector< float > cos_theta1;
  vector< float > sin_theta1;
  for ( j = 0 ; j <= n / 2 ; j++ )
  {
    float theta1 = j * 2.0f * PI / n - 0.5f * PI;
    cos_theta1.push_back( cos( theta1 ) );
    sin_theta1.push_back( sin( theta1 ) );
  }
  
  vector< float > cos_theta2;
  vector< float > sin_theta2;
  for ( i = 0 ; i <= n ; i++ )
  {
    cos_theta2.push_back( cos( i * PI / n ) );
    sin_theta2.push_back( sin( i * PI / n ) );
  }
   
  float ex,ey,ez,px,py,pz;
  glColor4f( col.r / 255.0f, col.g / 255.0f, col.b / 255.0f, visSettings.alpha );

  // draw the hemisphere by drawing rings that stand upright on top of the
  // (x,y)-plane
  for ( j = 0 ; j < n / 2 ; j++ )
  {
    glBegin(GL_QUAD_STRIP);
    for ( i = 0 ; i <= n ; i++ )
    {
      ex = cos_theta1[j] * cos_theta2[i];
      ey = sin_theta1[j];
      ez = cos_theta1[j] * sin_theta2[i];
      px = r * ex;
      py = r * ey;
      pz = r * ez;

      glNormal3f( ex, ey, ez );
      glVertex3f( px, py, pz );

      ex = cos_theta1[j+1] * cos_theta2[i];
      ey = sin_theta1[j+1];
      ez = cos_theta1[j+1] * sin_theta2[i];
      px = r * ex;
      py = r * ey;
      pz = r * ez;

      glNormal3f( ex, ey, ez );
      glVertex3f( px, py, pz );
    }
    glEnd();
  }
  
  // if n is odd, then we have to draw one more polygon, namely the one that is
  // enclosed by the outermost circle of the last ring and the (x,y)-plane
  if ( n % 2 == 1 )
  {
    j = n / 2;
    glBegin(GL_POLYGON );
    for ( i = n ; i >= 0 ; i-- )
    {
      ex = cos_theta1[j] * cos_theta2[i];
      ey = sin_theta1[j];
      ez = cos_theta1[j] * sin_theta2[i];
      px = r * ex;
      py = r * ey;
      pz = r * ez;
      glNormal3f( ex, ey, ez );
      glVertex3f( px, py, pz );
    }
    glEnd();
  }
}

// draws a tube around the first half of a cubic Bezier curve of which begin
// point b0 is in the origin. The coordinates of the other points are given as
// parameters, as are the base radius, top radius, base color and top color.
// Coordinates of b1, b2 and b3 are assumed to be relative to the current origin
// (not world coordinates).
void Visualizer::drawTube( float baserad, float toprad, RGB_Color basecol,
      RGB_Color topcol, Point3D b1, Point3D b2, Point3D b3 )
{
  int N = visSettings.quality / 2 + visSettings.quality % 2;

  vector< Point3D > curve;
  vector< Point3D > curve_der;
  float d_t = 0.5f / N;
  float t  = 0.0f;
  float it = 1.0f;
  
  for ( int i = 0 ; i <= N ; i++ )
  {
    float fac1 = 3*t*it*it;
    float fac2 = 3*t*t*it;
    float fac3 = t*t*t;
    
    Point3D bt =
      { fac1*b1.x + fac2*b2.x + fac3*b3.x,
	fac1*b1.y + fac2*b2.y + fac3*b3.y,
	fac1*b1.z + fac2*b2.z + fac3*b3.z };
    curve.push_back( bt );
    t  += d_t;
    it -= d_t;
  }

  t  = 0.0f;
  it = 1.0f;
  for ( int i = 0 ; i <= N ; i++ )
  {
    float fac1 = 3*it*it;
    float fac2 = 6*t*it;
    float fac3 = 3*t*t;
    
    // compute the derivative of the Bezier curve in t
    Point3D bt_der =
      { fac1*b1.x + fac2*(b2.x-b1.x) + fac3*(b3.x-b2.x),
        fac1*b1.y + fac2*(b2.y-b1.y) + fac3*(b3.y-b2.y),
        fac1*b1.z + fac2*(b2.z-b1.z) + fac3*(b3.z-b2.z) };
    
    // normalise the vector
    float length = sqrt( bt_der.x*bt_der.x + bt_der.y*bt_der.y +
	bt_der.z*bt_der.z );
    bt_der.x = bt_der.x / length;
    bt_der.y = bt_der.y / length;
    bt_der.z = bt_der.z / length;
	       
    curve_der.push_back( bt_der );
    t  += d_t;
    it -= d_t;
  }

  vector< float > ctab;
  vector< float > stab;
  float delta_ang = 2.0f * PI / visSettings.quality;
  float ang = 0.0f;
  for ( int j = 0 ; j <= visSettings.quality ; j++ )
  {
    ctab.push_back( cos( ang ) );
    stab.push_back( sin( ang ) );
    ang += delta_ang;
  }
    
  for ( int i = 0 ; i < N ; i++ )
  {
    // rotate so that z-axis points in direction of curve, i.e.: rotate around
    // the vector perpendicular to the plane spanned by the z-axis (0,0,1) and
    // the direction vector curve_der[i]. This perpendicular vector is computed
    // by taking the cross product of the two vectors. The angle over which we
    // rotate is the angle between the two vectors, which (as both vectors are
    // normalised) equals the arccos of the dot product of both vectors.
    float rot_ang1 = acos( curve_der[i].z ) * 180.0f / PI;
    float rot_ang2 = acos( curve_der[i+1].z ) * 180.0f / PI;
    
    vector< Point3D > normals1;
    vector< Point3D > normals2;
    vector< Point3D > vertices1;
    vector< Point3D > vertices2;
    GLfloat M[16];
    
    for ( int j = 0 ; j <= visSettings.quality ; j++ )
    {
      glPushMatrix();
      glLoadIdentity();
      glRotatef( rot_ang2, -curve_der[i+1].y, curve_der[i+1].x, 0.0f );
      glTranslatef( ctab[j], stab[j], 0.0f );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
      Point3D p1 = { M[12], M[13], M[14] };
      normals2.push_back( p1 );
      glPopMatrix();
      
      glPushMatrix();
      glLoadIdentity();
      glTranslatef( curve[i+1].x, curve[i+1].y, curve[i+1].z );
      glRotatef( rot_ang2, -curve_der[i+1].y, curve_der[i+1].x, 0.0f );
      glTranslatef( baserad*ctab[j], baserad*stab[j], 0.0f );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
      Point3D p2 = { M[12], M[13], M[14] };
      vertices2.push_back( p2 );
      glPopMatrix();
      
      glPushMatrix();
      glLoadIdentity();
      glRotatef( rot_ang1, -curve_der[i].y, curve_der[i].x, 0.0f );
      glTranslatef( ctab[j], stab[j], 0.0f );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
      Point3D p3 = { M[12], M[13], M[14] };
      normals1.push_back( p3 );
      glPopMatrix();
      
      glPushMatrix();
      glLoadIdentity();
      glTranslatef( curve[i].x, curve[i].y, curve[i].z );
      glRotatef( rot_ang1, -curve_der[i].y, curve_der[i].x, 0.0f );
      glTranslatef( baserad*ctab[j], baserad*stab[j], 0.0f );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
      Point3D p4 = { M[12], M[13], M[14] };
      cerr << M[12] << "," << M[13] << "," << M[14] << endl;
      vertices1.push_back( p4 );
      glPopMatrix();
    }
    
    glColor4f( 0,0,0,1 );
    glBegin( GL_LINE_STRIP );
    for ( int j = 0 ; j <= visSettings.quality ; j++ )
    {
      glNormal3f( normals2[j].x, normals2[j].y, normals2[j].z );
      glVertex3f( vertices2[j].x, vertices2[j].y, vertices2[j].z );
      glNormal3f( normals1[j].x, normals1[j].y, normals1[j].z );
      glVertex3f( vertices1[j].x, vertices1[j].y, vertices1[j].z );
    }
    glEnd();
  }
  
  glColor4f( 0,0,0,1 );
  glBegin( GL_LINE_STRIP );
  for ( int i = 0 ; i < N ; i++ )
  {
    glVertex3f( curve[i].x, curve[i].y, curve[i].z );
  }
  glEnd();
}
