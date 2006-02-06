#include "visualizer.h"

VisSettings Visualizer::defaultVisSettings =
{
  { 190, 190, 190 }, 0.3f, 100, 1.2f, { 255, 255, 255 }, { 235, 235, 235 },
  { 0, 0, 255 }, false, true, { 255, 0, 0 }, 0.1f, 30, 12, { 255, 255, 255 }, 0, 
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
  
  rankStyle = Iterative;
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

	RGB_Color delta_col;
	//if ( visSettings.longInterpolation )
	delta_col.r = ( visSettings.interpolateColor2.r - 
	    visSettings.interpolateColor1.r ) / lts->getNumberOfRanks();
	delta_col.g = ( visSettings.interpolateColor2.g - 
	    visSettings.interpolateColor1.g ) / lts->getNumberOfRanks();
	delta_col.b = ( visSettings.interpolateColor2.b - 
	    visSettings.interpolateColor1.b ) / lts->getNumberOfRanks();
	
	structHeight = 0.0f;
	structWidth = 0.0f;

	drawSubtree( lts->getInitialState()->getCluster(),
	    visSettings.interpolateColor1, delta_col, true, structWidth,
	    structHeight );

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

// draws the subtree with cluster *root (being the root of the tree)
void Visualizer::drawSubtree( Cluster* root, RGB_Color col, RGB_Color delta_col,
    bool topClosed, float &boundWidth, float &boundHeight )
{
  if ( !root->hasDescendants() )
  {
    GLUtils::setColor( col, visSettings.transparency );
    glutSolidSphere( root->getTopRadius(), visSettings.quality, visSettings.quality );
    boundWidth = root->getTopRadius();
    boundHeight = 2.0f * root->getTopRadius();
  }
  else
  {
    RGB_Color desccol = 
      { col.r + delta_col.r, col.g + delta_col.g, col.b + delta_col.b };
    
    vector< Cluster* > descendants;
    root->getDescendants( descendants );
    
    GLUtils::coloredCylinder( root->getTopRadius(), root->getBaseRadius(),
	visSettings.clusterHeight, visSettings.quality, visSettings.quality,
	col, desccol, visSettings.transparency, topClosed,
	descendants.size() > 1 );
      
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
	
	boundWidth = maximum( boundWidth, descWidth );
	boundHeight = maximum( boundHeight, descHeight );
	
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
	boundWidth = maximum( boundWidth, rad + descHeight * sin_a + descWidth *
	    cos_a );
	boundHeight = maximum( boundHeight, descHeight * cos_a + descWidth *
	    sin_a );
	
	glRotatef( -visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f );
	glTranslatef( -rad, 0.0f, 0.0f );
	glRotatef( desc->getPosition(), 0.0f, 0.0f, 1.0f );
	glTranslatef( 0.0f, 0.0f, -visSettings.clusterHeight );
      }
    }
    boundWidth = maximum( boundWidth, root->getTopRadius() );
    boundHeight += visSettings.clusterHeight;
  }
}
