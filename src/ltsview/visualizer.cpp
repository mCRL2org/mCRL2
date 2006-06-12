#include "visualizer.h"

bool Distance_desc::operator()(const Primitive* p1, const Primitive* p2) const
{
  return ( p1->distance > p2->distance );
}

VisSettings Visualizer::defaultVisSettings =
{
  0.6f, { 120, 120, 120 }, 111, /*100, 1.2f,*/ RGB_WHITE, 0, RGB_WHITE,
  { 0, 0, 255 }, /*false,*/ false, { 255, 0, 0 }, true, 0.1f, 30, 12, RGB_WHITE,
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
  displayWireframe = false;
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

  if ( oldSettings.innerBranchTilt != vs.innerBranchTilt ||
       oldSettings.newStyle != vs.newStyle )
  {
    refreshPrimitives = true;
  }

  if ( oldSettings.branchRotation != vs.branchRotation ) 
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

void Visualizer::toggleDisplayWireframe()
{
  displayWireframe = !displayWireframe;
}

void Visualizer::computeClusterHeight()
{
  // compute the cluster height that results in a picture with a "nice" aspect
  // ratio
  float ratio = lts->getInitialState()->getCluster()->getSize() / (
      lts->getNumberOfRanks() - 1 );
  clusterHeight = max(4,roundToInt(40.0f * ratio)) / 10.0f;
  
  refreshPrimitives = true;
  refreshStates = true;
}

void Visualizer::drawLTS( Point3D viewpoint )
{
  if ( lts == NULL ) return;

  if ( displayWireframe )
    glPolygonMode( GL_FRONT, GL_LINE );
  else
    glPolygonMode( GL_FRONT, GL_FILL );

  if ( displayStates && refreshStates )
  {
    glDeleteLists( statesDisplayList, 1 );
    statesDisplayList = glGenLists( 1 );
    glNewList( statesDisplayList, GL_COMPILE );
      switch ( markStyle )
      {
	case MARK_STATES:
	  drawStatesMarkStates( lts->getInitialState()->getCluster(), 0 );
	  break;
	case MARK_DEADLOCKS:
	  drawStatesMarkDeadlocks( lts->getInitialState()->getCluster(), 0 );
	  break;
	default:
	  setColor( visSettings.stateColor, 1.0f );
	  drawStates( lts->getInitialState()->getCluster(), 0 );
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
      if ( visSettings.newStyle )
      {
	sin_ibt = float( sin( degToRad( visSettings.innerBranchTilt ) ) );
	cos_ibt = float( cos( degToRad( visSettings.innerBranchTilt ) ) );
	sin_obt = float( sin( degToRad( visSettings.outerBranchTilt ) ) );
	cos_obt = float( cos( degToRad( visSettings.outerBranchTilt ) ) );
	switch ( markStyle )
	{
	  case MARK_DEADLOCKS:
	    drawSubtreeNewMarkDeadlocks( lts->getInitialState()->getCluster(),
		0);
	    break;
	    
	  case MARK_STATES:
	    drawSubtreeNewMarkStates( lts->getInitialState()->getCluster(), 0);
	    break;

	  case MARK_TRANSITIONS:
	    drawSubtreeNewMarkTransitions( lts->getInitialState()->getCluster(),
		0);
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
	    
	    if ( visSettings.longInterpolation )
	      delta_col.h = ( fabs(delta_h1) < fabs(delta_h2) ) ? delta_h2 : delta_h1;
	    else
	      delta_col.h = ( fabs(delta_h1) < fabs(delta_h2) ) ? delta_h1 : delta_h2;
	    delta_col.s = (hsv2.s - hsv1.s) / (lts->getNumberOfRanks() - 1);
	    delta_col.v = (hsv2.v - hsv1.v) / (lts->getNumberOfRanks() - 1);
	    
	    drawSubtreeNew( lts->getInitialState()->getCluster(), hsv1, 0 );
	    break;
	}
      }
      else
      {
	switch ( markStyle )
	{
	  case MARK_DEADLOCKS:
	    drawSubtreeMarkDeadlocks( lts->getInitialState()->getCluster(),
		true, 0 );
	    break;
	    
	  case MARK_STATES:
	    drawSubtreeMarkStates( lts->getInitialState()->getCluster(), true, 0 );
	    break;

	  case MARK_TRANSITIONS:
	    drawSubtreeMarkTransitions( lts->getInitialState()->getCluster(),
		true, 0 );
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
	    
	    if ( visSettings.longInterpolation )
	      delta_col.h = ( fabs(delta_h1) < fabs(delta_h2) ) ? delta_h2 : delta_h1;
	    else
	      delta_col.h = ( fabs(delta_h1) < fabs(delta_h2) ) ? delta_h1 : delta_h2;
	    delta_col.s = (hsv2.s - hsv1.s) / (lts->getNumberOfRanks() - 1);
	    delta_col.v = (hsv2.v - hsv1.v) / (lts->getNumberOfRanks() - 1);
	    
	    drawSubtree( lts->getInitialState()->getCluster(), true, hsv1, 0 );
	    break;
	}
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
  sin_obt = float( sin( degToRad( visSettings.outerBranchTilt ) ) );
  cos_obt = float( cos( degToRad( visSettings.outerBranchTilt ) ) );
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
    for ( vector< Cluster* >::iterator descit = descendants.begin() ; descit !=
	descendants.end() ; ++descit )
    {
      if ( (**descit).getPosition() < -0.9f )
      {
	// descendant is centered
	float descWidth = 0.0f;
	float descHeight = 0.0f;
	computeSubtreeBounds( *descit, descWidth, descHeight );
	boundWidth = max( boundWidth, descWidth );
	boundHeight = max( boundHeight, descHeight );
      }
      else
      {
	float descWidth = 0.0f;
	float descHeight = 0.0f;
	computeSubtreeBounds( *descit, descWidth, descHeight );
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
void Visualizer::drawSubtree( Cluster* root, bool topClosed, HSV_Color col, int
    rot )
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
      drawSphere( root->getTopRadius() );
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
    
    int desc_rot = rot + visSettings.branchRotation;
    if ( desc_rot < 0 ) desc_rot += 360;
    else if ( desc_rot >= 360 ) desc_rot -= 360;
    
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      drawCylinder( root->getTopRadius(), root->getBaseRadius(), clusterHeight,
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

    for ( vector< Cluster* >::iterator descit = descendants.begin() ; descit !=
	descendants.end() ; ++descit )
    {
      if ( (**descit).getPosition() < -0.9f )
      {
	glTranslatef( 0.0f, 0.0f, clusterHeight );
	
	drawSubtree( *descit, false, desccol, (descendants.size()>1)?desc_rot:rot );
	
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	glRotatef( -(**descit).getPosition()-rot, 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

	drawSubtree( *descit, true, desccol, desc_rot );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
	glRotatef( (**descit).getPosition()+rot, 0.0f, 0.0f, 1.0f );
      }
    }
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on marked states
void Visualizer::drawSubtreeMarkStates( Cluster* root, bool topClosed, int rot )
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
      drawSphere( root->getTopRadius() );
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
    
    int desc_rot = rot + visSettings.branchRotation;
    if ( desc_rot < 0 ) desc_rot += 360;
    else if ( desc_rot >= 360 ) desc_rot -= 360;
    
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      if ( root->hasMarkedState() )
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    clusterHeight, visSettings.markedColor, RGB_WHITE, topClosed,
	    descendants.size() > 1 );
      }
      else
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    clusterHeight, RGB_WHITE, RGB_WHITE, topClosed, descendants.size() >
	    1 );
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
    
    for ( vector< Cluster* >::iterator descit = descendants.begin() ; descit !=
	descendants.end() ; ++descit )
    {
      if ( (**descit).getPosition() < -0.9f )
      {
	glTranslatef( 0.0f, 0.0f, clusterHeight );
	
	drawSubtreeMarkStates( *descit, false, (descendants.size() > 1) ?
	    desc_rot : rot );
	
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	glRotatef( -(**descit).getPosition()-rot, 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

	drawSubtreeMarkStates( *descit, true, desc_rot );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
	glRotatef( (**descit).getPosition()+rot, 0.0f, 0.0f, 1.0f );
      }
    }
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on deadlocks
void Visualizer::drawSubtreeMarkDeadlocks( Cluster* root, bool topClosed, int
    rot )
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
      drawSphere( root->getTopRadius() );
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
    
    int desc_rot = rot + visSettings.branchRotation;
    if ( desc_rot < 0 ) desc_rot += 360;
    else if ( desc_rot >= 360 ) desc_rot -= 360;
    
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      if ( root->hasDeadlock() )
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    clusterHeight, visSettings.markedColor, RGB_WHITE, topClosed,
	    descendants.size() > 1 );
      }
      else
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    clusterHeight, RGB_WHITE, RGB_WHITE, topClosed, descendants.size() >
	    1 );
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
      
    for ( vector< Cluster* >::iterator descit = descendants.begin() ; descit !=
	descendants.end() ; ++descit )
    {
      if ( (**descit).getPosition() < -0.9f )
      {
	glTranslatef( 0.0f, 0.0f, clusterHeight );

	drawSubtreeMarkDeadlocks( *descit, false, (descendants.size() > 1) ?
	    desc_rot : rot );

	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	glRotatef( -(**descit).getPosition()-rot, 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

	drawSubtreeMarkDeadlocks( *descit, true, desc_rot );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
	glRotatef( (**descit).getPosition()+rot, 0.0f, 0.0f, 1.0f );
      }
    }
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on marked transitions
void Visualizer::drawSubtreeMarkTransitions( Cluster* root, bool topClosed, int
    rot )
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
      drawSphere( root->getTopRadius() );
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
    
    int desc_rot = rot + visSettings.branchRotation;
    if ( desc_rot < 0 ) desc_rot += 360;
    else if ( desc_rot >= 360 ) desc_rot -= 360;
    
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      if ( root->hasMarkedTransition() )
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    clusterHeight, visSettings.markedColor, RGB_WHITE, topClosed,
	    descendants.size() > 1);
      }
      else
      {
	drawCylinder( root->getTopRadius(), root->getBaseRadius(),
	    clusterHeight, RGB_WHITE, RGB_WHITE, topClosed, descendants.size() >
	    1 );
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
    
    for ( vector< Cluster* >::iterator descit = descendants.begin() ; descit !=
	descendants.end() ; ++descit )
    {
      if ( (**descit).getPosition() < -0.9f )
      {
	// descendant is centered
	glTranslatef( 0.0f, 0.0f, clusterHeight );
	drawSubtreeMarkTransitions( *descit, false, (descendants.size() > 1) ?
	    desc_rot : rot );
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	glRotatef( -(**descit).getPosition()-rot, 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	
	drawSubtreeMarkTransitions( *descit, true, desc_rot );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
	glRotatef( (**descit).getPosition()+rot, 0.0f, 0.0f, 1.0f );
      }
    }
  }
}

// draw the subtree with cluster *root as the root of the tree in the NEW style
// apply coloring based on interpolation settings
void Visualizer::drawSubtreeNew( Cluster* root, HSV_Color col, int rot )
{
  if ( root == lts->getInitialState()->getCluster() )
  {
    glRotatef( 180.0f, 1.0f, 0.0f, 0.0f );
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      setColor( HSVtoRGB( col ), visSettings.alpha );
      drawHemisphere( root->getTopRadius() ); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * root->getTopRadius() );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
    glRotatef( -180.0f, 1.0f, 0.0f, 0.0f );
  }
  
  if ( !root->hasDescendants() )
  {
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      setColor( HSVtoRGB( col ), visSettings.alpha );
      drawHemisphere( root->getTopRadius() ); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * root->getTopRadius() );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
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

    int desc_rot = rot + visSettings.branchRotation;
    if ( desc_rot < 0 ) desc_rot += 360;
    else if ( desc_rot >= 360 ) desc_rot -= 360;
    
    for ( int i = 0 ; i < root->getNumberOfDescendants() ; i++ )
    {
      Cluster* desc = root->getDescendant( i );
      if ( desc->getPosition() < -0.9f )
      {
	Point3D b1 = { 0.0f, 0.0f, 0.5f*clusterHeight };
	Point3D b2 = { 0.0f, 0.0f, 0.5f*clusterHeight };
	Point3D b3 = { 0.0f, 0.0f, clusterHeight };
	Point3D center;
	GLfloat M[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, M );
	GLuint displist = glGenLists( 1 );
	glNewList( displist, GL_COMPILE );
	  glPushMatrix();
	  glMultMatrixf( M );
	  drawTubeInterpolate( root->getTopRadius(), desc->getTopRadius(),
	      HSVtoRGB(col), HSVtoRGB(desccol), b1, b2, b3, center );
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

	glTranslatef( 0.0f, 0.0f, clusterHeight );
	
	drawSubtreeNew( desc, desccol, (root->getNumberOfDescendants() > 1) ?
	    desc_rot : rot );
	
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	float M1[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	
	myRotatef( degToRad( -desc->getPosition()-rot ), 0.0f, 0.0f, 1.0f, M1 );
	myTranslatef( 0.5f * clusterHeight * sin_ibt, 0.0f, 0.5f * clusterHeight
	    * cos_ibt, M1 );
	Point3D b1 = { M1[3], M1[7], M1[11] };
	
	myTranslatef( -0.5f * clusterHeight * sin_ibt, 0.0f, -0.5f *
	    clusterHeight * cos_ibt, M1 );
	myTranslatef( root->getBaseRadius(), 0.0f, clusterHeight, M1 );
	Point3D b3 = { M1[3], M1[7], M1[11] };
	
	myTranslatef( -0.5f * clusterHeight * sin_obt, 0.0f, -0.5f *
	    clusterHeight * cos_obt, M1 );
	Point3D b2 = { M1[3], M1[7], M1[11] };
	
	Point3D center;

	GLfloat M[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, M );
	GLuint displist = glGenLists( 1 );
	glNewList( displist, GL_COMPILE );
	  glPushMatrix();
	  glMultMatrixf( M );
	  drawTubeInterpolate( root->getTopRadius(), desc->getTopRadius(),
	      HSVtoRGB(col), HSVtoRGB(desccol), b1, b2, b3, center );
	  glPopMatrix();
	glEndList();

	Primitive* p = new Primitive;
	glPushMatrix();
	  glTranslatef( center.x, center.y, center.z );
	  glGetFloatv( GL_MODELVIEW_MATRIX, M );
	glPopMatrix();
	p->worldCoordinate.x = M[12];
	p->worldCoordinate.y = M[13];
	p->worldCoordinate.z = M[14];
	p->displayList = displist;
	primitives.push_back( p );

	glRotatef( -desc->getPosition()-rot, 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
  
	drawSubtreeNew( desc, desccol, desc_rot );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
	glRotatef( desc->getPosition()+rot, 0.0f, 0.0f, 1.0f );
      }
    }
  }
}

// draws the subtree with cluster *root as the root of the tree in the NEW style
// applies coloring based on marked states
void Visualizer::drawSubtreeNewMarkStates( Cluster* root, int rot )
{
  RGB_Color color;
  if ( root->hasMarkedState() )
    color = visSettings.markedColor;
  else
    color = RGB_WHITE;
    
  if ( root == lts->getInitialState()->getCluster() )
  {
    glRotatef( 180.0f, 1.0f, 0.0f, 0.0f );
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      setColor( color, visSettings.alpha );
      drawHemisphere( root->getTopRadius() ); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * root->getTopRadius() );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
    glRotatef( -180.0f, 1.0f, 0.0f, 0.0f );
  }
  
  if ( !root->hasDescendants() )
  {
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      setColor( color, visSettings.alpha );
      drawHemisphere( root->getTopRadius() ); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * root->getTopRadius() );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
  }
  else
  {
    int desc_rot = rot + visSettings.branchRotation;
    if ( desc_rot < 0 ) desc_rot += 360;
    else if ( desc_rot >= 360 ) desc_rot -= 360;
    
    for ( int i = 0 ; i < root->getNumberOfDescendants() ; i++ )
    {
      Cluster* desc = root->getDescendant( i );
      if ( desc->getPosition() < -0.9f )
      {
	Point3D b1 = { 0.0f, 0.0f, 0.5f*clusterHeight };
	Point3D b2 = { 0.0f, 0.0f, 0.5f*clusterHeight };
	Point3D b3 = { 0.0f, 0.0f, clusterHeight };
	Point3D center;
	GLfloat M[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, M );
	GLuint displist = glGenLists( 1 );
	glNewList( displist, GL_COMPILE );
	  glPushMatrix();
	  glMultMatrixf( M );
	  drawTubeSplit( root->getTopRadius(), desc->getTopRadius(),
	      color, (desc->hasMarkedState()) ?  visSettings.markedColor :
	      RGB_WHITE, b1, b2, b3, center );
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

	glTranslatef( 0.0f, 0.0f, clusterHeight );
	
	drawSubtreeNewMarkStates( desc, (root->getNumberOfDescendants() > 1) ?
	    desc_rot : rot );
	
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	float M1[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	
	myRotatef( degToRad( -desc->getPosition()-rot ), 0.0f, 0.0f, 1.0f, M1 );
	myTranslatef( 0.5f * clusterHeight * sin_ibt, 0.0f, 0.5f * clusterHeight
	    * cos_ibt, M1 );
	Point3D b1 = { M1[3], M1[7], M1[11] };
	
	myTranslatef( -0.5f * clusterHeight * sin_ibt, 0.0f, -0.5f *
	    clusterHeight * cos_ibt, M1 );
	myTranslatef( root->getBaseRadius(), 0.0f, clusterHeight, M1 );
	Point3D b3 = { M1[3], M1[7], M1[11] };
	
	myTranslatef( -0.5f * clusterHeight * sin_obt, 0.0f, -0.5f *
	    clusterHeight * cos_obt, M1 );
	Point3D b2 = { M1[3], M1[7], M1[11] };
	
	Point3D center;

	GLfloat M[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, M );
	GLuint displist = glGenLists( 1 );
	glNewList( displist, GL_COMPILE );
	  glPushMatrix();
	  glMultMatrixf( M );
	  drawTubeSplit( root->getTopRadius(), desc->getTopRadius(),
	      color, (desc->hasMarkedState()) ? visSettings.markedColor :
	      RGB_WHITE, b1, b2, b3, center );
	  glPopMatrix();
	glEndList();

	Primitive* p = new Primitive;
	glPushMatrix();
	  glTranslatef( center.x, center.y, center.z );
	  glGetFloatv( GL_MODELVIEW_MATRIX, M );
	glPopMatrix();
	p->worldCoordinate.x = M[12];
	p->worldCoordinate.y = M[13];
	p->worldCoordinate.z = M[14];
	p->displayList = displist;
	primitives.push_back( p );

	glRotatef( -desc->getPosition()-rot, 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
  
	drawSubtreeNewMarkStates( desc, desc_rot );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
	glRotatef( desc->getPosition()+rot, 0.0f, 0.0f, 1.0f );
      }
    }
  }
}

// draws the subtree with cluster *root as the root of the tree in the NEW style
// applies coloring based on deadlocks
void Visualizer::drawSubtreeNewMarkDeadlocks( Cluster* root, int rot )
{
  RGB_Color color;
  if ( root->hasDeadlock() )
  {
    color = visSettings.markedColor;
  }
  else
    color = RGB_WHITE;
    
  if ( root == lts->getInitialState()->getCluster() )
  {
    glRotatef( 180.0f, 1.0f, 0.0f, 0.0f );
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      setColor( color, visSettings.alpha );
      drawHemisphere( root->getTopRadius() ); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * root->getTopRadius() );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
    glRotatef( -180.0f, 1.0f, 0.0f, 0.0f );
  }
  
  if ( !root->hasDescendants() )
  {
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      setColor( color, visSettings.alpha );
      drawHemisphere( root->getTopRadius() ); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * root->getTopRadius() );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
  }
  else
  {
    int desc_rot = rot + visSettings.branchRotation;
    if ( desc_rot < 0 ) desc_rot += 360;
    else if ( desc_rot >= 360 ) desc_rot -= 360;
    
    for ( int i = 0 ; i < root->getNumberOfDescendants() ; i++ )
    {
      Cluster* desc = root->getDescendant( i );
      if ( desc->getPosition() < -0.9f )
      {
	Point3D b1 = { 0.0f, 0.0f, 0.5f*clusterHeight };
	Point3D b2 = { 0.0f, 0.0f, 0.5f*clusterHeight };
	Point3D b3 = { 0.0f, 0.0f, clusterHeight };
	Point3D center;
	GLfloat M[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, M );
	GLuint displist = glGenLists( 1 );
	glNewList( displist, GL_COMPILE );
	  glPushMatrix();
	  glMultMatrixf( M );
	  drawTubeSplit( root->getTopRadius(), desc->getTopRadius(), color,
	      (desc->hasDeadlock()) ? visSettings.markedColor : RGB_WHITE, b1,
	      b2, b3, center );
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

	glTranslatef( 0.0f, 0.0f, clusterHeight );
	
	drawSubtreeNewMarkDeadlocks( desc, (root->getNumberOfDescendants() > 1)
	    ?  desc_rot : rot );
	
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	float M1[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	
	myRotatef( degToRad( -desc->getPosition()-rot ), 0.0f, 0.0f, 1.0f, M1 );
	myTranslatef( 0.5f * clusterHeight * sin_ibt, 0.0f, 0.5f * clusterHeight
	    * cos_ibt, M1 );
	Point3D b1 = { M1[3], M1[7], M1[11] };
	
	myTranslatef( -0.5f * clusterHeight * sin_ibt, 0.0f, -0.5f *
	    clusterHeight * cos_ibt, M1 );
	myTranslatef( root->getBaseRadius(), 0.0f, clusterHeight, M1 );
	Point3D b3 = { M1[3], M1[7], M1[11] };
	
	myTranslatef( -0.5f * clusterHeight * sin_obt, 0.0f, -0.5f *
	    clusterHeight * cos_obt, M1 );
	Point3D b2 = { M1[3], M1[7], M1[11] };
	
	Point3D center;

	GLfloat M[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, M );
	GLuint displist = glGenLists( 1 );
	glNewList( displist, GL_COMPILE );
	  glPushMatrix();
	  glMultMatrixf( M );
	  drawTubeSplit( root->getTopRadius(), desc->getTopRadius(), color,
	      (desc->hasDeadlock()) ? visSettings.markedColor : RGB_WHITE, b1,
	      b2, b3, center );
	  glPopMatrix();
	glEndList();

	Primitive* p = new Primitive;
	glPushMatrix();
	  glTranslatef( center.x, center.y, center.z );
	  glGetFloatv( GL_MODELVIEW_MATRIX, M );
	glPopMatrix();
	p->worldCoordinate.x = M[12];
	p->worldCoordinate.y = M[13];
	p->worldCoordinate.z = M[14];
	p->displayList = displist;
	primitives.push_back( p );

	glRotatef( -desc->getPosition()-rot, 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
  
	drawSubtreeNewMarkDeadlocks( desc, desc_rot );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
	glRotatef( desc->getPosition()+rot, 0.0f, 0.0f, 1.0f );
      }
    }
  }
}

// draws the subtree with cluster *root as the root of the tree in the NEW style
// applies coloring based on marked transitions
void Visualizer::drawSubtreeNewMarkTransitions( Cluster* root, int rot )
{
  RGB_Color color;
  if ( root->hasMarkedTransition() )
  {
    color = visSettings.markedColor;
  }
  else
    color = RGB_WHITE;
    
  if ( root == lts->getInitialState()->getCluster() )
  {
    glRotatef( 180.0f, 1.0f, 0.0f, 0.0f );
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      setColor( color, visSettings.alpha );
      drawHemisphere( root->getTopRadius() ); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * root->getTopRadius() );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
    glRotatef( -180.0f, 1.0f, 0.0f, 0.0f );
  }
  
  if ( !root->hasDescendants() )
  {
    GLfloat M[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, M );
    GLuint displist = glGenLists( 1 );
    glNewList( displist, GL_COMPILE );
      glPushMatrix();
      glMultMatrixf( M );
      setColor( color, visSettings.alpha );
      drawHemisphere( root->getTopRadius() ); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 0.5f * root->getTopRadius() );
      glGetFloatv( GL_MODELVIEW_MATRIX, M );
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back( p );
  }
  else
  {
    int desc_rot = rot + visSettings.branchRotation;
    if ( desc_rot < 0 ) desc_rot += 360;
    else if ( desc_rot >= 360 ) desc_rot -= 360;
    
    for ( int i = 0 ; i < root->getNumberOfDescendants() ; i++ )
    {
      Cluster* desc = root->getDescendant( i );
      if ( desc->getPosition() < -0.9f )
      {
	Point3D b1 = { 0.0f, 0.0f, 0.5f*clusterHeight };
	Point3D b2 = { 0.0f, 0.0f, 0.5f*clusterHeight };
	Point3D b3 = { 0.0f, 0.0f, clusterHeight };
	Point3D center;
	GLfloat M[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, M );
	GLuint displist = glGenLists( 1 );
	glNewList( displist, GL_COMPILE );
	  glPushMatrix();
	  glMultMatrixf( M );
	  drawTubeSplit( root->getTopRadius(), desc->getTopRadius(), color,
	      (desc->hasMarkedTransition()) ?  visSettings.markedColor :
	      RGB_WHITE, b1, b2, b3, center );
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

	glTranslatef( 0.0f, 0.0f, clusterHeight );
	
	drawSubtreeNewMarkTransitions( desc, (root->getNumberOfDescendants() >
	      1) ?  desc_rot : rot );
	
	glTranslatef( 0.0f, 0.0f, -clusterHeight );
      }
      else
      {
	float M1[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	
	myRotatef( degToRad( -desc->getPosition()-rot ), 0.0f, 0.0f, 1.0f, M1 );
	myTranslatef( 0.5f * clusterHeight * sin_ibt, 0.0f, 0.5f * clusterHeight
	    * cos_ibt, M1 );
	Point3D b1 = { M1[3], M1[7], M1[11] };
	
	myTranslatef( -0.5f * clusterHeight * sin_ibt, 0.0f, -0.5f *
	    clusterHeight * cos_ibt, M1 );
	myTranslatef( root->getBaseRadius(), 0.0f, clusterHeight, M1 );
	Point3D b3 = { M1[3], M1[7], M1[11] };
	
	myTranslatef( -0.5f * clusterHeight * sin_obt, 0.0f, -0.5f *
	    clusterHeight * cos_obt, M1 );
	Point3D b2 = { M1[3], M1[7], M1[11] };
	
	Point3D center;

	GLfloat M[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, M );
	GLuint displist = glGenLists( 1 );
	glNewList( displist, GL_COMPILE );
	  glPushMatrix();
	  glMultMatrixf( M );
	  drawTubeSplit( root->getTopRadius(), desc->getTopRadius(),
	      color, (desc->hasMarkedTransition()) ? visSettings.markedColor :
	      RGB_WHITE, b1, b2, b3, center );
	  glPopMatrix();
	glEndList();

	Primitive* p = new Primitive;
	glPushMatrix();
	  glTranslatef( center.x, center.y, center.z );
	  glGetFloatv( GL_MODELVIEW_MATRIX, M );
	glPopMatrix();
	p->worldCoordinate.x = M[12];
	p->worldCoordinate.y = M[13];
	p->worldCoordinate.z = M[14];
	p->displayList = displist;
	primitives.push_back( p );

	glRotatef( -desc->getPosition()-rot, 0.0f, 0.0f, 1.0f );
	glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
	glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
  
	drawSubtreeNewMarkTransitions( desc, desc_rot );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
	glRotatef( desc->getPosition()+rot, 0.0f, 0.0f, 1.0f );
      }
    }
  }
}

void Visualizer::drawStates( Cluster* root, int rot )
{
  vector< State* > c_ss;
  root->getStates( c_ss );
  for ( vector< State* >::iterator s_it = c_ss.begin() ; s_it != c_ss.end() ;
	++s_it )
  {
    if ( (**s_it).getPosition() < -0.9f )
      drawSphere( visSettings.nodeSize, 4 );
    else
    {
      glRotatef( -(**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getTopRadius(), 0.0f, 0.0f );
      drawSphere( visSettings.nodeSize, 4 );
      glTranslatef( -root->getTopRadius(), 0.0f, 0.0f );
      glRotatef( (**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
    }
  }

  int desc_rot = rot + visSettings.branchRotation;
  if ( desc_rot < 0 ) desc_rot += 360;
  else if ( desc_rot >= 360 ) desc_rot -= 360;
  
  for ( int i = 0 ; i < root->getNumberOfDescendants() ; i++ )
  {
    Cluster* desc = root->getDescendant( i );
    if ( desc->getPosition() < -0.9f )
    {
      // descendant is centered
      glTranslatef( 0.0f, 0.0f, clusterHeight );
      drawStates( desc, (root->getNumberOfDescendants() > 1) ? desc_rot : rot );
      glTranslatef( 0.0f, 0.0f, -clusterHeight );
    }
    else
    {
      glRotatef( -desc->getPosition()-rot, 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
      glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

      drawStates( desc, desc_rot );
      
      glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
      glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
      glRotatef( desc->getPosition()+rot, 0.0f, 0.0f, 1.0f );
    }
  }
}

void Visualizer::drawStatesMarkStates( Cluster* root, int rot )
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
      drawSphere( visSettings.nodeSize, 4 );
    else
    {
      glRotatef( -(**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getTopRadius(), 0.0f, 0.0f );
      drawSphere( visSettings.nodeSize, 4 );
      glTranslatef( -root->getTopRadius(), 0.0f, 0.0f );
      glRotatef( (**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
    }
  }

  int desc_rot = rot + visSettings.branchRotation;
  if ( desc_rot < 0 ) desc_rot += 360;
  else if ( desc_rot >= 360 ) desc_rot -= 360;
  
  for ( int i = 0 ; i < root->getNumberOfDescendants() ; i++ ) 
  {
    Cluster* desc = root->getDescendant( i );
    if ( desc->getPosition() < -0.9f )
    {
      // descendant is centered
      glTranslatef( 0.0f, 0.0f, clusterHeight );
      drawStatesMarkStates( desc, (root->getNumberOfDescendants() > 1) ?
	  desc_rot : rot );
      glTranslatef( 0.0f, 0.0f, -clusterHeight );
    }
    else
    {
      glRotatef( -desc->getPosition()-rot, 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
      glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

      drawStatesMarkStates( desc, desc_rot );
      
      glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
      glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
      glRotatef( desc->getPosition()+rot, 0.0f, 0.0f, 1.0f );
    }
  }
}

void Visualizer::drawStatesMarkDeadlocks( Cluster* root, int rot )
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
      drawSphere( visSettings.nodeSize, 4 );
    else
    {
      glRotatef( -(**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getTopRadius(), 0.0f, 0.0f );
      drawSphere( visSettings.nodeSize, 4 );
      glTranslatef( -root->getTopRadius(), 0.0f, 0.0f );
      glRotatef( (**s_it).getPosition(), 0.0f, 0.0f, 1.0f );
    }
  }

  int desc_rot = rot + visSettings.branchRotation;
  if ( desc_rot < 0 ) desc_rot += 360;
  else if ( desc_rot >= 360 ) desc_rot -= 360;
  
  for ( int i = 0 ; i < root->getNumberOfDescendants() ; i++ )
  {
    Cluster* desc = root->getDescendant( i );
    if ( desc->getPosition() < -0.9f )
    {
      glTranslatef( 0.0f, 0.0f, clusterHeight );
      
      drawStatesMarkDeadlocks( desc, (root->getNumberOfDescendants() > 1) ?
	  desc_rot : rot );
      
      glTranslatef( 0.0f, 0.0f, -clusterHeight );
    }
    else
    {
      glRotatef( -desc->getPosition()-rot, 0.0f, 0.0f, 1.0f );
      glTranslatef( root->getBaseRadius(), 0.0f, clusterHeight );
      glRotatef( visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );

      drawStatesMarkDeadlocks( desc, desc_rot );
      
      glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
      glTranslatef( -root->getBaseRadius(), 0.0f, -clusterHeight );
      glRotatef( desc->getPosition()+rot, 0.0f, 0.0f, 1.0f );
    }
  }
}

void Visualizer::setColor( RGB_Color c, float alpha )
{
  glColor4f( c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, alpha );
}

// draws a cylinder around z-axis with given base radius, top radius, height,
// color at base, color at top, closed base if baseclosed and closed top if
// topclosed
void Visualizer::drawCylinder( float baserad, float toprad, float height,
    RGB_Color basecol, RGB_Color topcol, bool baseclosed, bool topclosed )
{
  int slices = visSettings.quality;
  float nxg = height;
  float nzg = baserad - toprad;
  float r = sqrt( nxg*nxg + nzg*nzg );
  nxg = nxg / r;
  nzg = nzg / r;

  vector< float > ctab( slices+1 );
  vector< float > stab( slices+1 );
  vector< float > nx( slices+1 );
  vector< float > ny( slices+1 );
  
  float delta = 2.0f * PI / slices;
  float alf = 0.0f;
  for ( int i=0 ; i <= slices ; ++i )
  {
    float cos_alf = cos(alf);
    float sin_alf = sin(alf);
    ctab[i] = cos_alf;
    stab[i] = sin_alf;
    nx[i] = cos_alf * nxg;
    ny[i] = sin_alf * nxg;
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
    glVertex3f( toprad * ctab[j], toprad * stab[j], height );
    glColor4f( c1r, c1g, c1b, visSettings.alpha );
    glVertex3f( baserad * ctab[j], baserad * stab[j], 0.0f );
  }
  glEnd();

  if ( topclosed )
  {
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0.0, 0.0, 1.0 );
    glColor4f( c2r, c2g, c2b, visSettings.alpha );
    glVertex3f( 0.0, 0.0, height );
    for ( int j = 0 ; j <= slices ; ++j )
    {
      glVertex3f( toprad * ctab[j], toprad * stab[j], height );
    }
    glEnd();
  }
}

// draws a cylinder around z-axis with given base radius, top radius, height,
// color at base, color at top and interpolates between the given colors over
// the length of the cylinder
void Visualizer::drawCylinderInterpolate( float baserad, float toprad, float
    height, RGB_Color basecol, RGB_Color topcol )
{
  int slices = visSettings.quality;
  float nxg = height;
  float nzg = baserad - toprad;
  float r = sqrt( nxg*nxg + nzg*nzg );
  nxg = nxg / r;
  nzg = nzg / r;

  vector< float > ctab( slices+1 );
  vector< float > stab( slices+1 );
  vector< float > nx( slices+1 );
  vector< float > ny( slices+1 );
  
  float delta = 2.0f * PI / slices;
  float alf = 0.0f;
  for ( int i=0 ; i <= slices ; ++i )
  {
    float cos_alf = cos(alf);
    float sin_alf = sin(alf);
    ctab[i] = cos_alf;
    stab[i] = sin_alf;
    nx[i] = cos_alf * nxg;
    ny[i] = sin_alf * nxg;
    alf += delta;
  }

  float c1r = basecol.r / 255.0f;
  float c1g = basecol.g / 255.0f;
  float c1b = basecol.b / 255.0f;
  float c2r = topcol.r / 255.0f;
  float c2g = topcol.g / 255.0f;
  float c2b = topcol.b / 255.0f;

  glBegin( GL_QUAD_STRIP );
  for ( int j = 0 ; j <= slices ; ++j )
  {
    glNormal3f( nx[j], ny[j], nzg );
    glColor4f( c2r, c2g, c2b, visSettings.alpha );
    glVertex3f( toprad * ctab[j], toprad * stab[j], height );
    glColor4f( c1r, c1g, c1b, visSettings.alpha );
    glVertex3f( baserad * ctab[j], baserad * stab[j], 0.0f );
  }
  glEnd();
}

// As drawCylinderInterpolate, but the color of the bottom half of the cylinder
// is basecol and that of the top half is topcol.
void Visualizer::drawCylinderSplit( float baserad, float toprad, float
    height, RGB_Color basecol, RGB_Color topcol )
{
  int slices = visSettings.quality;
  float nxg = height;
  float nzg = baserad - toprad;
  float r = sqrt( nxg*nxg + nzg*nzg );
  nxg = nxg / r;
  nzg = nzg / r;

  vector< float > ctab( slices+1 );
  vector< float > stab( slices+1 );
  vector< float > nx( slices+1 );
  vector< float > ny( slices+1 );
  
  float delta = 2.0f * PI / slices;
  float alf = 0.0f;
  for ( int i=0 ; i <= slices ; ++i )
  {
    float cos_alf = cos(alf);
    float sin_alf = sin(alf);
    ctab[i] = cos_alf;
    stab[i] = sin_alf;
    nx[i] = cos_alf * nxg;
    ny[i] = sin_alf * nxg;
    alf += delta;
  }

  float demirad = 0.5f * ( toprad + baserad );
  glColor4f( basecol.r/255.0f, basecol.g/255.0f, basecol.b/255.0f,
      visSettings.alpha );
  glBegin( GL_QUAD_STRIP );
  for ( int j = 0 ; j <= slices ; ++j )
  {
    glNormal3f( nx[j], ny[j], nzg );
    glVertex3f( demirad * ctab[j], demirad * stab[j], 0.5f * height );
    glVertex3f( baserad * ctab[j], baserad * stab[j], 0.0f );
  }
  glEnd();

  glColor4f( topcol.r/255.0f, topcol.g/255.0f, topcol.b/255.0f,
      visSettings.alpha );
  glBegin( GL_QUAD_STRIP );
  for ( int j = 0 ; j <= slices ; ++j )
  {
    glNormal3f( nx[j], ny[j], nzg );
    glVertex3f( toprad * ctab[j], toprad * stab[j], height );
    glVertex3f( demirad * ctab[j], demirad * stab[j], 0.5f * height );
  }
  glEnd();
}

void Visualizer::drawHemisphere( float r )
{
  int n = visSettings.quality;
  
  // precompute the cosines and sines that are needed during drawing
  vector< float > cos_theta1( n+1 );
  vector< float > sin_theta1( n+1 );
  float delta = 0.5f * PI / n;
  float theta = 0.0f;
  for ( int j = 0 ; j <= n ; j++ )
  {
    cos_theta1[j] = cos( theta );
    sin_theta1[j] = sin( theta );
    theta += delta;
  }
  
  vector< float > cos_theta2( n+1 );
  vector< float > sin_theta2( n+1 );
  delta = 2.0f * PI / n;
  theta = 0.0f;
  for ( int i = 0 ; i <= n ; i++ )
  {
    cos_theta2[i] = cos( theta );
    sin_theta2[i] = sin( theta );
    theta += delta;
  }
   
  float ex,ey,ez,px,py,pz;

  // draw the hemisphere by drawing rings around the z-axis
  for ( int j = 0 ; j < n ; j++ )
  {
    glBegin(GL_QUAD_STRIP);
    for ( int i = 0 ; i <= n ; i++ )
    {
      ex = cos_theta1[j+1] * cos_theta2[i];
      ey = cos_theta1[j+1] * sin_theta2[i];
      ez = sin_theta1[j+1];
      px = r * ex;
      py = r * ey;
      pz = r * ez;

      glNormal3f( ex, ey, ez );
      glVertex3f( px, py, pz );
      
      ex = cos_theta1[j] * cos_theta2[i];
      ey = cos_theta1[j] * sin_theta2[i];
      ez = sin_theta1[j];
      px = r * ex;
      py = r * ey;
      pz = r * ez;

      glNormal3f( ex, ey, ez );
      glVertex3f( px, py, pz );
    }
    glEnd();
  }
}

void Visualizer::drawSphere( float r, int n )
{
  if ( n <= 0 ) n = visSettings.quality;
  
  // precompute the cosines and sines that are needed during drawing
  vector< float > cos_theta1( n+1 );
  vector< float > sin_theta1( n+1 );
  float delta = PI / n;
  float theta = -0.5f * PI;
  for ( int j = 0 ; j <= n ; j++ )
  {
    cos_theta1[j] = cos( theta );
    sin_theta1[j] = sin( theta );
    theta += delta;
  }
  
  vector< float > cos_theta2( n+1 );
  vector< float > sin_theta2( n+1 );
  delta = 2.0f * PI / n;
  theta = 0.0f;
  for ( int i = 0 ; i <= n ; i++ )
  {
    cos_theta2[i] = cos( theta );
    sin_theta2[i] = sin( theta );
    theta += delta;
  }
   
  float ex,ey,ez,px,py,pz;

  // draw the sphere by drawing rings around the z-axis
  for ( int j = 0 ; j < n ; j++ )
  {
    glBegin(GL_QUAD_STRIP);
    for ( int i = 0 ; i <= n ; i++ )
    {
      ex = cos_theta1[j+1] * cos_theta2[i];
      ey = cos_theta1[j+1] * sin_theta2[i];
      ez = sin_theta1[j+1];
      px = r * ex;
      py = r * ey;
      pz = r * ez;

      glNormal3f( ex, ey, ez );
      glVertex3f( px, py, pz );
      
      ex = cos_theta1[j] * cos_theta2[i];
      ey = cos_theta1[j] * sin_theta2[i];
      ez = sin_theta1[j];
      px = r * ex;
      py = r * ey;
      pz = r * ez;

      glNormal3f( ex, ey, ez );
      glVertex3f( px, py, pz );
    }
    glEnd();
  }
}

// Draws a tube around a cubic Bezier curve of which begin point b0 is in the
// origin. The coordinates of the other points are given as parameters, as are
// the base radius, top radius, base color and top color. The center of the tube
// will be stored in the center parameter. Coordinates of b1, b2, b3 and center
// are relative to the current origin (not world coordinates).
// The color of the tube is interpolated between basecol and topcol over the
// length of the tube.
void Visualizer::drawTubeInterpolate( float baserad, float toprad, RGB_Color basecol,
      RGB_Color topcol, Point3D b1, Point3D b2, Point3D b3, Point3D &center )
{
  int N = visSettings.quality + visSettings.quality % 2;

  // compute the coordinates of the center of the tube (i.e. at t = 0.5)
  center.x = 0.375f*b1.x + 0.375f*b2.x + 0.125f*b3.x;
  center.y = 0.375f*b1.y + 0.375f*b2.y + 0.125f*b3.y;
  center.z = 0.375f*b1.z + 0.375f*b2.z + 0.125f*b3.z;
  
  vector< Point3D > curve( N+1 );	// stores the curve's vertex coordinates
  vector< Point3D > curve_der( N+1 );	// stores the derivatives in those vertices
  vector< float > color_r( N+1 );	// stores the tube's colors (red)
  vector< float > color_g( N+1 );	// stores the tube's colors (green)
  vector< float > color_b( N+1 );	// stores the tube's colors (blue)
  vector< float > radius( N+1 );	// stores the tube's radii

  float basecol_fr = basecol.r / 255.0f;
  float basecol_fg = basecol.g / 255.0f;
  float basecol_fb = basecol.b / 255.0f;
  float topcol_fr = topcol.r / 255.0f;
  float topcol_fg = topcol.g / 255.0f;
  float topcol_fb = topcol.b / 255.0f;
  
  bool odd_segs = (visSettings.quality % 2 == 1);
  float delta_t = 1.0f / visSettings.quality;
  // precompute the coordinates, derivatives, colors and radii
  for ( int i = 0 ; i <= N ; i++ )
  {
    // compute t and its "inverse", 1-t
    float t = i * delta_t;
    if ( odd_segs )
    {
      if ( i > N / 2 )	      t -= delta_t;
      else if ( i == N / 2 )  t = 0.5f;
    }
    float it = 1.0f - t;
    
    // compute the coordinates of b(t)
    float fac1 = 3*t*it*it;
    float fac2 = 3*t*t*it;
    float fac3 = t*t*t;
    Point3D bt =
      { fac1*b1.x + fac2*b2.x + fac3*b3.x,
	fac1*b1.y + fac2*b2.y + fac3*b3.y,
	fac1*b1.z + fac2*b2.z + fac3*b3.z };
    curve[i] = bt;
    
    // compute the derivative in b(t) as a normalised direction vector
    fac1 = 3*it*it;
    fac2 = 6*t*it;
    fac3 = 3*t*t;
    Point3D bt_der =
      { fac1*b1.x + fac2*(b2.x-b1.x) + fac3*(b3.x-b2.x),
        fac1*b1.y + fac2*(b2.y-b1.y) + fac3*(b3.y-b2.y),
        fac1*b1.z + fac2*(b2.z-b1.z) + fac3*(b3.z-b2.z) };
    // normalise the vector
    float len = sqrt( bt_der.x*bt_der.x +
		      bt_der.y*bt_der.y +
		      bt_der.z*bt_der.z );
    if ( len != 0 )
    {
      bt_der.x = bt_der.x / len;
      bt_der.y = bt_der.y / len;
      bt_der.z = bt_der.z / len;
    }
    curve_der[i] = bt_der;
    
    // compute the color of the tube in b(t)
    color_r[i] = it*basecol_fr + t*topcol_fr;
    color_g[i] = it*basecol_fg + t*topcol_fg;
    color_b[i] = it*basecol_fb + t*topcol_fb;

    // compute the radius of the tube in b(t)
    radius[i] = it*baserad + t*toprad;
  }

  // precompute sine and cosine functions
  vector< float > ctab( visSettings.quality + 1 );
  vector< float > stab( visSettings.quality + 1 );
  float delta_ang = 2.0f * PI / visSettings.quality;
  float ang = 0.0f;
  for ( int j = 0 ; j <= visSettings.quality ; j++ )
  {
    ctab[j] = cos( ang );
    stab[j] = sin( ang );
    ang += delta_ang;
  }

  float M[16];
  float rot_ang1;
  float rot_ang2;

  // draw the rings
  for ( int i = 0 ; i < N ; i++ )
  {
    // compute the angle over which we have to rotate to align the z-axis with
    // vertex i's direction vector (i.e. derivative) (note that we use the fact
    // that the derivative vector is normalised)
    rot_ang1 = acos( curve_der[i].z );
    rot_ang2 = acos( curve_der[i+1].z );
    
    glBegin( GL_QUAD_STRIP );
    for ( int j = 0 ; j <= visSettings.quality ; j++ )
    {
      // compute the normal vector for vertex (i+1,j)
      for ( int k = 0 ; k < 16 ; k++ ) M[k] = (k % 5 == 0) ? 1 : 0;
      myRotatef( rot_ang2, -curve_der[i+1].y, curve_der[i+1].x, 0.0f, M );
      myTranslatef( ctab[j], stab[j], 0.0f, M );
      glNormal3f( M[3], M[7], M[11] );

      // compute the coordinates of vertex (i+1,j)
      for ( int k = 0 ; k < 16 ; k++ ) M[k] = (k % 5 == 0) ? 1 : 0;
      myTranslatef( curve[i+1].x, curve[i+1].y, curve[i+1].z, M );
      myRotatef( rot_ang2, -curve_der[i+1].y, curve_der[i+1].x, 0.0f, M );
      myTranslatef( radius[i+1]*ctab[j], radius[i+1]*stab[j], 0.0f, M );
      glColor4f( color_r[i+1], color_g[i+1], color_b[i+1], visSettings.alpha );
      glVertex3f( M[3], M[7], M[11] );
      
      // compute the normal vector for vertex (i,j)
      for ( int k = 0 ; k < 16 ; k++ ) M[k] = (k % 5 == 0) ? 1 : 0;
      myRotatef( rot_ang1, -curve_der[i].y, curve_der[i].x, 0.0f, M );
      myTranslatef( ctab[j], stab[j], 0.0f, M );
      glNormal3f( M[3], M[7], M[11] );
      
      // compute the coordinates of vertex (i,j)
      for ( int k = 0 ; k < 16 ; k++ ) M[k] = (k % 5 == 0) ? 1 : 0;
      myTranslatef( curve[i].x, curve[i].y, curve[i].z, M );
      myRotatef( rot_ang1, -curve_der[i].y, curve_der[i].x, 0.0f, M );
      myTranslatef( radius[i]*ctab[j], radius[i]*stab[j], 0.0f, M );
      glColor4f( color_r[i], color_g[i], color_b[i], visSettings.alpha );
      glVertex3f( M[3], M[7], M[11] );
    }
    glEnd();
  }
}

// As drawTubeInterpolate, but the color of the first half of the tube is
// basecol and that of the second half is topcol.
void Visualizer::drawTubeSplit( float baserad, float toprad, RGB_Color basecol,
      RGB_Color topcol, Point3D b1, Point3D b2, Point3D b3, Point3D &center )
{
  int N = visSettings.quality + visSettings.quality % 2;

  // compute the coordinates of the center of the tube (i.e. at t = 0.5)
  center.x = 0.375f*b1.x + 0.375f*b2.x + 0.125f*b3.x;
  center.y = 0.375f*b1.y + 0.375f*b2.y + 0.125f*b3.y;
  center.z = 0.375f*b1.z + 0.375f*b2.z + 0.125f*b3.z;
  
  vector< Point3D > curve( N+1 );	// stores the curve's vertex coordinates
  vector< Point3D > curve_der( N+1 );	// stores the derivatives in those vertices
  vector< float > radius( N+1 );	// stores the tube's radii

  bool odd_segs = (visSettings.quality % 2 == 1);
  float delta_t = 1.0f / visSettings.quality;
  // precompute the coordinates, derivatives, colors and radii
  for ( int i = 0 ; i <= N ; i++ )
  {
    // compute t and its "inverse", 1-t
    float t = i * delta_t;
    if ( odd_segs )
    {
      if ( i > N / 2 )	      t -= delta_t;
      else if ( i == N / 2 )  t = 0.5f;
    }
    float it = 1.0f - t;
    
    // compute the coordinates of b(t)
    float fac1 = 3*t*it*it;
    float fac2 = 3*t*t*it;
    float fac3 = t*t*t;
    Point3D bt =
      { fac1*b1.x + fac2*b2.x + fac3*b3.x,
	fac1*b1.y + fac2*b2.y + fac3*b3.y,
	fac1*b1.z + fac2*b2.z + fac3*b3.z };
    curve[i] = bt;
    
    // compute the derivative in b(t) as a normalised direction vector
    fac1 = 3*it*it;
    fac2 = 6*t*it;
    fac3 = 3*t*t;
    Point3D bt_der =
      { fac1*b1.x + fac2*(b2.x-b1.x) + fac3*(b3.x-b2.x),
        fac1*b1.y + fac2*(b2.y-b1.y) + fac3*(b3.y-b2.y),
        fac1*b1.z + fac2*(b2.z-b1.z) + fac3*(b3.z-b2.z) };
    // normalise the vector
    float len = sqrt( bt_der.x*bt_der.x +
		      bt_der.y*bt_der.y +
		      bt_der.z*bt_der.z );
    if ( len != 0 )
    {
      bt_der.x = bt_der.x / len;
      bt_der.y = bt_der.y / len;
      bt_der.z = bt_der.z / len;
    }
    curve_der[i] = bt_der;
    
    // compute the radius of the tube in b(t)
    radius[i] = it*baserad + t*toprad;
  }

  // precompute sine and cosine functions
  vector< float > ctab( visSettings.quality + 1 );
  vector< float > stab( visSettings.quality + 1 );
  float delta_ang = 2.0f * PI / visSettings.quality;
  float ang = 0.0f;
  for ( int j = 0 ; j <= visSettings.quality ; j++ )
  {
    ctab[j] = cos( ang );
    stab[j] = sin( ang );
    ang += delta_ang;
  }

  float M[16];
  float rot_ang1;
  float rot_ang2;

  // draw the rings
  glColor4f( basecol.r/255.0f, basecol.g/255.0f, basecol.b/255.0f,
      visSettings.alpha );
  for ( int i = 0 ; i < N ; i++ )
  {
    // compute the angle over which we have to rotate to align the z-axis with
    // vertex i's direction vector (i.e. derivative) (note that we use the fact
    // that the derivative vector is normalised)
    rot_ang1 = acos( curve_der[i].z );
    rot_ang2 = acos( curve_der[i+1].z );
    
    glBegin( GL_QUAD_STRIP );
    if ( i == N / 2 )
    {
      glColor4f( topcol.r/255.0f, topcol.g/255.0f, topcol.b/255.0f,
	  visSettings.alpha );
    }
    for ( int j = 0 ; j <= visSettings.quality ; j++ )
    {
      // compute the normal vector for vertex (i+1,j)
      for ( int k = 0 ; k < 16 ; k++ ) M[k] = (k % 5 == 0) ? 1 : 0;
      myRotatef( rot_ang2, -curve_der[i+1].y, curve_der[i+1].x, 0.0f, M );
      myTranslatef( ctab[j], stab[j], 0.0f, M );
      glNormal3f( M[3], M[7], M[11] );

      // compute the coordinates of vertex (i+1,j)
      for ( int k = 0 ; k < 16 ; k++ ) M[k] = (k % 5 == 0) ? 1 : 0;
      myTranslatef( curve[i+1].x, curve[i+1].y, curve[i+1].z, M );
      myRotatef( rot_ang2, -curve_der[i+1].y, curve_der[i+1].x, 0.0f, M );
      myTranslatef( radius[i+1]*ctab[j], radius[i+1]*stab[j], 0.0f, M );
      glVertex3f( M[3], M[7], M[11] );
      
      // compute the normal vector for vertex (i,j)
      for ( int k = 0 ; k < 16 ; k++ ) M[k] = (k % 5 == 0) ? 1 : 0;
      myRotatef( rot_ang1, -curve_der[i].y, curve_der[i].x, 0.0f, M );
      myTranslatef( ctab[j], stab[j], 0.0f, M );
      glNormal3f( M[3], M[7], M[11] );
      
      // compute the coordinates of vertex (i,j)
      for ( int k = 0 ; k < 16 ; k++ ) M[k] = (k % 5 == 0) ? 1 : 0;
      myTranslatef( curve[i].x, curve[i].y, curve[i].z, M );
      myRotatef( rot_ang1, -curve_der[i].y, curve_der[i].x, 0.0f, M );
      myTranslatef( radius[i]*ctab[j], radius[i]*stab[j], 0.0f, M );
      glVertex3f( M[3], M[7], M[11] );
    }
    glEnd();
  }
}

// multiplies the rotation matrix with M and stores the result in M
// theta is the angle in radians(!) over which we rotate, [ax,ay,az] is the
// vector around which we rotate.
void Visualizer::myRotatef( float theta, float ax, float ay, float az, float M[] )
{
  float len = sqrt( ax*ax + ay*ay + az*az );
  float x = ax;
  float y = ay;
  float z = az;
  if ( len != 0 )
  {
    x /= len;
    y /= len;
    z /= len;
  }

  float c = cos( theta );
  float s = sin( theta );

  // compute the rotation matrix for this angle and vector
  // note that R is actually a 4x4 matrix; the fourth column and fourth row are
  // as in the identity matrix: [0,0,0,1]
  float R[9] = { x*x*(1-c)+c,   x*y*(1-c)-z*s, x*z*(1-c)+y*s,
		 y*x*(1-c)+z*s, y*y*(1-c)+c,   y*z*(1-c)-x*s,
		 x*z*(1-c)-y*s, y*z*(1-c)+x*s, z*z*(1-c)+c };
  
  M[0] = M[0]*R[0] + M[1]*R[3] + M[2]*R[6];
  M[1] = M[0]*R[1] + M[1]*R[4] + M[2]*R[7];
  M[2] = M[0]*R[2] + M[1]*R[5] + M[2]*R[8];
  M[4] = M[4]*R[0] + M[5]*R[3] + M[6]*R[6];
  M[5] = M[4]*R[1] + M[5]*R[4] + M[6]*R[7];
  M[6] = M[4]*R[2] + M[5]*R[5] + M[6]*R[8];
  M[8] = M[8]*R[0] + M[9]*R[3] + M[10]*R[6];
  M[9] = M[8]*R[1] + M[9]*R[4] + M[10]*R[7];
  M[10] = M[8]*R[2] + M[9]*R[5] + M[10]*R[8];
  M[12] = M[12]*R[0] + M[13]*R[3] + M[14]*R[6];
  M[13] = M[12]*R[1] + M[13]*R[4] + M[14]*R[7];
  M[14] = M[12]*R[2] + M[13]*R[5] + M[14]*R[8];
}

// multiplies the translation matrix with matrix M and stores the result in M
// [tx,ty,tz] is the vector over which we translate
void Visualizer::myTranslatef( float tx, float ty, float tz, float M[] )
{
  M[3] = M[0]*tx + M[1]*ty + M[2]*tz + M[3];
  M[7] = M[4]*tx + M[5]*ty + M[6]*tz + M[7];
  M[11] = M[8]*tx + M[9]*ty + M[10]*tz + M[11];
  M[15] = M[12]*tx + M[13]*ty + M[14]*tz + M[15];
}
