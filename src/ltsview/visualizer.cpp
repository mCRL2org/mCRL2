#include "visualizer.h"

VisSettings Visualizer::defaultVisSettings =
{
  { 120, 120, 120 }, 0.3f, 100, 1.2f, RGB_WHITE, RGB_WHITE,
  { 0, 0, 255 }, false, false, { 255, 0, 0 }, 0.1f, 30, 12, RGB_WHITE, 0, 
  { 0, 0, 255 }
};

Visualizer::Visualizer( Mediator* owner )
{
  displayList = glGenLists( 1 );
  lts = NULL;
  structHeight = 0.0f;
  structWidth = 0.0f;
  mediator = owner;
  
  // set the visualization settings to default values
  visSettings = defaultVisSettings;
  
  rankStyle = ITERATIVE;
  refreshDisplayList = false;
}

Visualizer::~Visualizer()
{
  glDeleteLists( displayList, 1 );
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

float Visualizer::getStructureHeight() const
{
  return structHeight;
}

float Visualizer::getStructureWidth() const
{
  return structWidth;
}

void Visualizer::setMarkStyle( MarkStyle ms )
{
  markStyle = ms;
  refreshDisplayList = true;
}

void Visualizer::setRankStyle( RankStyle rs )
{
  rankStyle = rs;
  refreshDisplayList = true;
}

void Visualizer::setLTS( LTS* l )
{
  lts = l;
  refreshDisplayList = true;
}

void Visualizer::setVisSettings( VisSettings vs )
{
  if ( visSettings != vs )
  {
    visSettings = vs;
    refreshDisplayList = true;
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
  int ratio = roundToInt( 10.0 * lts->getInitialState()->getCluster()->getSize()
      / ( lts->getNumberOfRanks() - 1 ) );
  
  defaultVisSettings.clusterHeight = 0.4f * ratio ;
  visSettings.clusterHeight = defaultVisSettings.clusterHeight;
  
  refreshDisplayList = true;
}

void Visualizer::drawLTS()
{
  if ( lts == NULL ) return;
  if ( refreshDisplayList )
  {
    glDeleteLists( displayList, 1 );
    displayList = glGenLists( 1 );
    
    glNewList( displayList, GL_COMPILE );
      glPushMatrix();
	glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
	glTranslatef( 0.0f, 0.0f, -visSettings.clusterHeight*( lts->getNumberOfRanks()-1 ) / 2.0f );
      
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );

	structHeight = 0.0f;
	structWidth = 0.0f;

	switch ( markStyle )
	{
	  case MARK_DEADLOCKS:
	    drawSubtreeMarkDeadlocks( lts->getInitialState()->getCluster(),
		true, structWidth, structHeight );
	    break;
	    
	  case MARK_STATES:
	    drawSubtreeMarkStates( lts->getInitialState()->getCluster(), true,
		structWidth, structHeight );
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
	    
	    drawSubtree( lts->getInitialState()->getCluster(), hsv1, delta_col,
		true, structWidth, structHeight );
	    break;
	}

	glDisable( GL_COLOR_MATERIAL );
      
      glPopMatrix();
    glEndList();
    
    refreshDisplayList = false;
  }
  
  // clear the canvas with the background color
  glClearColor( visSettings.backgroundColor.r / 255.0,
      visSettings.backgroundColor.g / 255.0,
      visSettings.backgroundColor.b / 255.0, 1.0 );
  glClearDepth( 1.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  if ( visSettings.transparency > 0 )
  {
    // objects are transparent, so to avoid artifacting...
    glEnable( GL_CULL_FACE );
      // first render the back facing polygons...
      glCullFace( GL_FRONT );
      glCallList( displayList );
      // and then the front facing polygons
      glCullFace( GL_BACK );
      glCallList( displayList );
    glDisable( GL_CULL_FACE );
  }
  else
  {
    // no transparent objects, so render the whole scene in one pass
    glCallList( displayList );
  }
}

// draws the subtree with cluster *root as the root of the tree
// applies coloring based on interpolation settings
void Visualizer::drawSubtree( Cluster* root, HSV_Color col, HSV_Color delta_col,
    bool topClosed, float &boundWidth, float &boundHeight )
{
  if ( !root->hasDescendants() )
  {
    GLUtils::setColor( HSVtoRGB( col ), visSettings.transparency );
    glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
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
    
    GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	visSettings.clusterHeight, visSettings.quality, visSettings.quality,
	HSVtoRGB( col ), HSVtoRGB( desccol ), visSettings.transparency,
	topClosed, descendants.size() > 1 );
      
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
	
	drawSubtree( desc, desccol, delta_col, false, descWidth, descHeight );
	
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
	
	drawSubtree( desc, desccol, delta_col, true, descWidth, descHeight );
	
	float sin_a = float( sin( visSettings.outerBranchTilt * PI / 180.0 ) );
	float cos_a = float( cos( visSettings.outerBranchTilt * PI / 180.0 ) );
	boundWidth = max( boundWidth, rad + descHeight * sin_a + descWidth *
	    cos_a );
	boundHeight = max( boundHeight, descHeight * cos_a + descWidth *
	    sin_a );
	
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
    if ( root->isMarked() )
      GLUtils::setColor( visSettings.markedColor, visSettings.transparency );
    else
      GLUtils::setColor( RGB_WHITE, visSettings.transparency );
    
    glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
    boundWidth = root->getTopRadius();
    boundHeight = 2.0f * root->getTopRadius();
  }
  else
  {
    vector< Cluster* > descendants;
    root->getDescendants( descendants );
    
    if ( root->isMarked() )
    {
      GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	visSettings.clusterHeight, visSettings.quality, visSettings.quality,
	visSettings.markedColor, RGB_WHITE, visSettings.transparency, topClosed,
	descendants.size() > 1 );
    }
    else
    {
      GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	visSettings.clusterHeight, visSettings.quality, visSettings.quality,
	RGB_WHITE, RGB_WHITE, visSettings.transparency, topClosed,
	descendants.size() > 1 );
    }
    
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
	
	float sin_a = float( sin( visSettings.outerBranchTilt * PI / 180.0 ) );
	float cos_a = float( cos( visSettings.outerBranchTilt * PI / 180.0 ) );
	boundWidth = max( boundWidth, rad + descHeight * sin_a + descWidth *
	    cos_a );
	boundHeight = max( boundHeight, descHeight * cos_a + descWidth *
	    sin_a );
	
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
    if ( root->hasDeadlock() )
      GLUtils::setColor( visSettings.markedColor, visSettings.transparency );
    else
      GLUtils::setColor( RGB_WHITE, visSettings.transparency );
    
    glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
    boundWidth = root->getTopRadius();
    boundHeight = 2.0f * root->getTopRadius();
  }
  else
  {
    vector< Cluster* > descendants;
    root->getDescendants( descendants );
    
    if ( root->hasDeadlock() )
    {
      GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	visSettings.clusterHeight, visSettings.quality, visSettings.quality,
	visSettings.markedColor, RGB_WHITE, visSettings.transparency, topClosed,
	descendants.size() > 1 );
    }
    else
    {
      GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	visSettings.clusterHeight, visSettings.quality, visSettings.quality,
	RGB_WHITE, RGB_WHITE, visSettings.transparency, topClosed,
	descendants.size() > 1 );
    }

      
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
	
	float sin_a = float( sin( visSettings.outerBranchTilt * PI / 180.0 ) );
	float cos_a = float( cos( visSettings.outerBranchTilt * PI / 180.0 ) );
	boundWidth = max( boundWidth, rad + descHeight * sin_a + descWidth *
	    cos_a );
	boundHeight = max( boundHeight, descHeight * cos_a + descWidth *
	    sin_a );
	
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
