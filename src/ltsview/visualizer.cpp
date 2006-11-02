#include "visualizer.h"
using namespace std;
using namespace Utils;

bool Distance_desc::operator()(const Primitive* p1, const Primitive* p2) const {
  return ( p1->distance > p2->distance );
}

VisSettings Visualizer::defaultVisSettings = {
  1.0f, 111, RGB_WHITE, 1.0f, 0, RGB_WHITE, RGB_BLUE, false, RGB_RED, 0.1f, 30,
  12, RGB_WHITE, RGB_BLUE
};

Visualizer::Visualizer( Mediator* owner ) {
  lts = NULL;
  boundingCylH = 0.0f;
  boundingCylW = 0.0f;
  mediator = owner;
  
  // set the visualization settings to default values
  visSettings = defaultVisSettings;
  updateGeometryTables();
  sin_ibt = float( sin( deg_to_rad( visSettings.innerBranchTilt ) ) );
  cos_ibt = float( cos( deg_to_rad( visSettings.innerBranchTilt ) ) );
  sin_obt = float( sin( deg_to_rad( visSettings.outerBranchTilt ) ) );
  cos_obt = float( cos( deg_to_rad( visSettings.outerBranchTilt ) ) );
  
  rankStyle = ITERATIVE;
  visStyle = CONES;
  refreshPrimitives = false;
  refreshStates = false;
  refreshTransitions = false;
  displayStates = false;
  displayTransitions = false;
  displayBackpointers = false;
  displayWireframe = false;
  statesDisplayList = 0;
  transDisplayList = 0;
  
  cos_theta1_s.assign( 5, 0.0f );
  sin_theta1_s.assign( 5, 0.0f );
  cos_theta2_s.assign( 5, 0.0f );
  sin_theta2_s.assign( 5, 0.0f );
  
  float delta_theta1 = 0.5f  * PI;
  float delta_theta2 = 0.25f * PI;
  float theta1 = 0.0f;
  float theta2 = -0.5f * PI;
  
  for ( int i = 0 ; i <= 4 ; i++ )
  {
    cos_theta1_s[i] = cos( theta1 );
    sin_theta1_s[i] = sin( theta1 );
    cos_theta2_s[i] = cos( theta2 );
    sin_theta2_s[i] = sin( theta2 );
    theta1 += delta_theta1;
    theta2 += delta_theta2;
  }
}

Visualizer::~Visualizer() {
  for ( unsigned int i = 0 ; i < primitives.size() ; ++i ) {
    glDeleteLists( primitives[i]->displayList, 1 );
    delete primitives[i];
  }
  primitives.clear();
  glDeleteLists( statesDisplayList, 1 );
  glDeleteLists( transDisplayList, 1);
}

RankStyle Visualizer::getRankStyle() const {
  return rankStyle;
}

VisStyle Visualizer::getVisStyle() const {
  return visStyle;
}

VisSettings Visualizer::getDefaultVisSettings() const {
  return defaultVisSettings;
}

VisSettings Visualizer::getVisSettings() const {
  return visSettings;
}

float Visualizer::getHalfStructureHeight() const {
  if ( lts == NULL ) return 0.0f;
  return clusterHeight*( lts->getNumberOfRanks()-1 ) / 2.0f;
}

float Visualizer::getBoundingCylinderHeight() const {
  return boundingCylH;
}

float Visualizer::getBoundingCylinderWidth() const {
  return boundingCylW;
}

void Visualizer::setMarkStyle( MarkStyle ms ) {
  markStyle = ms;
  refreshPrimitives = true;
  refreshStates = true;
  refreshTransitions = true;
}

void Visualizer::setVisStyle( VisStyle vs ) {
  visStyle = vs;
  refreshPrimitives = true;
  refreshStates = true;
  refreshTransitions = true;
}

void Visualizer::setRankStyle( RankStyle rs ) {
  rankStyle = rs;
  refreshPrimitives = true;
  refreshStates = true;
  refreshTransitions = true;
}

void Visualizer::setLTS( LTS* l ) {
  lts = l;
  refreshPrimitives = true;
  refreshStates = true;
  refreshTransitions = true;
}

// returns whether the bounding cylinder needs to be recalculated
bool Visualizer::setVisSettings( VisSettings vs ) {
  VisSettings oldSettings = visSettings;
  visSettings = vs;
  
  if ( oldSettings.stateColor != vs.stateColor ||
       fabs( oldSettings.nodeSize - vs.nodeSize ) > 0.01f ) {
    refreshStates = true;
  }

  if ( oldSettings.upEdgeColor != vs.upEdgeColor ||
       oldSettings.downEdgeColor != vs.downEdgeColor ) {
    refreshTransitions = true;
  }
  
  if ( fabs( oldSettings.alpha - vs.alpha ) > 0.01f ||
       fabs( oldSettings.ellipsoidThreshold - vs.ellipsoidThreshold ) > 0.01f ) {
    refreshPrimitives = true;
  }
  
  if ( markStyle == NO_MARKS && 
      ( oldSettings.interpolateColor1 != vs.interpolateColor1 ||
        oldSettings.interpolateColor2 != vs.interpolateColor2 ||
        oldSettings.longInterpolation != vs.longInterpolation ) ) {
    refreshPrimitives = true;
  }
  
  if ( oldSettings.quality != vs.quality ) {
    updateGeometryTables();
    refreshPrimitives = true;
  }

  if ( markStyle != NO_MARKS && oldSettings.markedColor != vs.markedColor ) {
    refreshStates = true;
    refreshPrimitives = true;
    refreshTransitions = true;
  }

  if ( oldSettings.innerBranchTilt != vs.innerBranchTilt ) {
    sin_ibt = float( sin( deg_to_rad( vs.innerBranchTilt ) ) );
    cos_ibt = float( cos( deg_to_rad( vs.innerBranchTilt ) ) );
    refreshPrimitives = true;
  }

  if ( oldSettings.branchRotation != vs.branchRotation ) {
    refreshStates = true;
    refreshPrimitives = true;
    refreshTransitions = true;
  }

  if ( oldSettings.outerBranchTilt != vs.outerBranchTilt ) {
    sin_obt = float( sin( deg_to_rad( vs.outerBranchTilt ) ) );
    cos_obt = float( cos( deg_to_rad( vs.outerBranchTilt ) ) );
    refreshStates = true;
    refreshPrimitives = true;
    refreshTransitions = true;
    return true;
  }
  return false;
}

void Visualizer::toggleDisplayStates() {
  displayStates = !displayStates;
}

void Visualizer::toggleDisplayTransitions() {
  displayTransitions = !displayTransitions;

  // Set refreshTransitions to true, just in case.
  if ( displayTransitions) 
  {
    refreshTransitions = true;
  }
}

void Visualizer::toggleDisplayBackpointers() {
  displayBackpointers = !displayBackpointers;

  // Set refreshTransitions to true, just in case.
  if ( displayBackpointers ) 
  {
    refreshTransitions = true;
  }
}

void Visualizer::toggleDisplayWireframe() {
  displayWireframe = !displayWireframe;
}

void Visualizer::computeClusterHeight() {
  // compute the cluster height that results in a picture with a "nice" aspect
  // ratio
  float ratio = lts->getInitialState()->getCluster()->getSize() / (
      lts->getNumberOfRanks() - 1 );
  clusterHeight = max(4,round_to_int(40.0f * ratio)) / 10.0f;
  
  refreshPrimitives = true;
  refreshStates = true;
  refreshTransitions = true;
}

void Visualizer::drawLTS(Point3D viewpoint) {
  if (lts == NULL) return;

  if (displayWireframe) {
    glPolygonMode(GL_FRONT,GL_LINE);
  }
  else {
    glPolygonMode(GL_FRONT,GL_FILL);
  }

  if (displayStates && refreshStates ) {
    glDeleteLists(statesDisplayList,1);
    statesDisplayList = glGenLists(1);
    glNewList(statesDisplayList,GL_COMPILE);
      if (markStyle == NO_MARKS) {
        glColor4f(visSettings.stateColor.r,visSettings.stateColor.g,
                  visSettings.stateColor.b,1.0f);
        drawStates(lts->getInitialState()->getCluster(),0);
      }
      else {
        drawStatesMark(lts->getInitialState()->getCluster(),0);
      }
    glEndList();

    refreshStates = false;
  }

  if ((displayTransitions || displayBackpointers) && refreshTransitions) {
    clearDFSStates(lts->getInitialState());
    Point3D init = {0, 0, 0};
    glPushMatrix();
    glLoadIdentity();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    computeStateAbsPos(lts->getInitialState(), 0, init);
    glPopMatrix();
    clearDFSStates(lts->getInitialState());
        
    glDeleteLists(transDisplayList, 1);
    transDisplayList = glGenLists(1);
    glNewList(transDisplayList, GL_COMPILE);
      drawTransitions(lts->getInitialState());
    glEndList();

    refreshTransitions = false;
  }

  if (refreshPrimitives) {
    // refresh necessary
    // delete all primitives
    for (unsigned int i=0; i<primitives.size(); ++i) {
      glDeleteLists(primitives[i]->displayList,1);
      delete primitives[i];
    }
    primitives.clear();
    
    glPushMatrix();
      glLoadIdentity();
      switch (visStyle) {
        
	      case ATOMIUM:
          if (markStyle == NO_MARKS) {
            drawSubtreeA(lts->getInitialState()->getCluster(),0);
          }
          else {
	          drawSubtreeAMark(lts->getInitialState()->getCluster(),0);
          }
	        break;
          
        case TUBES:
	        if (markStyle == NO_MARKS) {
            // coloring based on interpolation settings
            HSV_Color hsv;
            computeDeltaCol(hsv);
            drawSubtreeO(lts->getInitialState()->getCluster(),hsv,0);
          }
          else {
	          drawSubtreeOMark(lts->getInitialState()->getCluster(),0);
          }
          break;
        
        case CONES:
	      default:
	        if (markStyle == NO_MARKS) {
            // coloring based on interpolation settings
            HSV_Color hsv;
            computeDeltaCol(hsv);
            drawSubtreeC(lts->getInitialState()->getCluster(),true,hsv,0);
          }
          else {
            drawSubtreeCMark(lts->getInitialState()->getCluster(),true,0);
          }
	        break;
      }
    glPopMatrix();
    refreshPrimitives = false;
  }

  // first draw the opaque objects in the scene (if required)
  if (displayStates) {
    glCallList(statesDisplayList);
  }

  if (displayTransitions || displayBackpointers) {
    glCallList(transDisplayList);
  }
  
  // compute distance of every primitive to viewpoint
  for (unsigned int i=0; i<primitives.size(); ++i) {
    Point3D d = {
      primitives[i]->worldCoordinate.x-viewpoint.x,
      primitives[i]->worldCoordinate.y-viewpoint.y,
      primitives[i]->worldCoordinate.z-viewpoint.z
    };
    primitives[i]->distance = sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
  }

  // sort primitives descending based on distance to viewpoint
  sort(primitives.begin(),primitives.end(),Distance_desc()); 
  
  // draw primitives in sorted order
  for (unsigned int i=0; i<primitives.size(); ++i) {
    glCallList(primitives[i]->displayList);
  }
}

void Visualizer::computeBoundsInfo() {
  boundingCylH = 0.0f;
  boundingCylW = 0.0f;
  if (lts != NULL) {
    computeSubtreeBounds(lts->getInitialState()->getCluster(),boundingCylW,
                         boundingCylH);
  }
}

void Visualizer::updateGeometryTables() {
  cos_theta1.assign(visSettings.quality+1,0.0f);
  sin_theta1.assign(visSettings.quality+1,0.0f);
  cos_theta2.assign(visSettings.quality+1,0.0f);
  sin_theta2.assign(visSettings.quality+1,0.0f);
  
  float delta_theta1 = 2.0f*PI / visSettings.quality;
  float delta_theta2 = PI / visSettings.quality;
  float theta1 = 0.0f;
  float theta2 = -0.5f*PI;
  
  for (int i=0; i<=visSettings.quality; ++i) {
    cos_theta1[i] = cos(theta1);
    sin_theta1[i] = sin(theta1);
    cos_theta2[i] = cos(theta2);
    sin_theta2[i] = sin(theta2);
    theta1 += delta_theta1;
    theta2 += delta_theta2;
  }
}

// traverses the structure like a draw procedure but does not call any OpenGL
// commands, so nothing is actually drawn to the canvas. Used when we only want
// to compute the bounding cylinder of the structure.
void Visualizer::computeSubtreeBounds(Cluster* root,float &boundWidth,
                                      float &boundHeight) {
  if (!root->hasDescendants()) {
    boundWidth = root->getTopRadius();
    boundHeight = 2.0f * root->getTopRadius();
  }
  else {
    vector< Cluster* > descendants;
    root->getDescendants(descendants);
    for (vector< Cluster* >::iterator descit=descendants.begin(); 
      descit!=descendants.end(); ++descit) {
      if ((**descit).getPosition() < -0.9f) {
      	// descendant is centered
      	float descWidth = 0.0f;
      	float descHeight = 0.0f;
      	computeSubtreeBounds(*descit,descWidth,descHeight);
      	boundWidth = max(boundWidth,descWidth);
      	boundHeight = max(boundHeight,descHeight);
      }
      else {
      	float descWidth = 0.0f;
      	float descHeight = 0.0f;
      	computeSubtreeBounds(*descit,descWidth,descHeight);
      	boundWidth = max(boundWidth,root->getBaseRadius()+descHeight*sin_obt+
                         descWidth*cos_obt);
      	boundHeight = max(boundHeight,descHeight*cos_obt+descWidth*sin_obt);
      }
    }
    boundWidth = max(boundWidth,root->getTopRadius());
    boundHeight += clusterHeight;
  }
}

void Visualizer::computeDeltaCol(HSV_Color &hsv1) {
  RGB_Color rgb1 = visSettings.interpolateColor1;
  RGB_Color rgb2 = visSettings.interpolateColor2;
  hsv1 = RGB_to_HSV(rgb1);
  HSV_Color hsv2 = RGB_to_HSV(rgb2);
  if (rgb1.r==rgb1.g && rgb1.g==rgb1.b) hsv1.h = hsv2.h;
  if (rgb2.r==rgb2.g && rgb2.g==rgb2.b) hsv2.h = hsv1.h;
  float delta_h1 = hsv2.h-hsv1.h;
  float delta_h2 = ((delta_h1<0.0f)?1.0f:-1.0f) *
                   (360.0f-fabs(delta_h1));
  delta_h1 /= lts->getNumberOfRanks()-1;
  delta_h2 /= lts->getNumberOfRanks()-1;
  if (visSettings.longInterpolation) {
    delta_col.h = (fabs(delta_h1)<fabs(delta_h2))?delta_h2:delta_h1;
  }
  else {
    delta_col.h = (fabs(delta_h1)<fabs(delta_h2))?delta_h1:delta_h2;
  }
  delta_col.s = (hsv2.s-hsv1.s)/(lts->getNumberOfRanks()-1);
  delta_col.v = (hsv2.v-hsv1.v)/(lts->getNumberOfRanks()-1);
}

bool Visualizer::isMarked(Cluster* c) {
  return ((markStyle == MARK_STATES && c->hasMarkedState()) || 
          (markStyle == MARK_DEADLOCKS && c->hasDeadlock()) ||
          (markStyle == MARK_TRANSITIONS && c->hasMarkedTransition()));
}

bool Visualizer::isMarked(State* s) {
  return ((markStyle == MARK_STATES && s->isMarked()) || 
          (markStyle == MARK_DEADLOCKS && s->isDeadlock())/* ||
          (markStyle == MARK_TRANSITIONS && s->hasMarkedTransition())*/);
}

// ------------- ATOMIUM -------------------------------------------------------

void Visualizer::drawSubtreeA(Cluster* root,int rot) {
  int desc_rot = rot + visSettings.branchRotation;
  if (desc_rot < 0) desc_rot += 360;
  else if (desc_rot >= 360) desc_rot -= 360;
    
  for (int i=0; i<root->getNumberOfDescendants(); ++i) {
    Cluster* desc = root->getDescendant(i);
    if (desc->getPosition() < -0.9f) {
      glTranslatef(0.0f,0.0f,clusterHeight);
      drawSubtreeA(desc,(root->getNumberOfDescendants()>1)?desc_rot:rot);
      glTranslatef(0.0f,0.0f,-clusterHeight);
      
      GLfloat M[16];
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
      GLuint displist = glGenLists(1);
      glNewList(displist,GL_COMPILE);
      	glPushMatrix();
      	glMultMatrixf(M);
      	drawCylinder(0.1f,0.1f,clusterHeight,visSettings.interpolateColor1,
                     visSettings.interpolateColor1,true,false,false);
      	glPopMatrix();
      glEndList();

      Primitive* p = new Primitive;
      glPushMatrix();
      	glTranslatef(0.0f,0.0f,0.5f*clusterHeight);
      	glGetFloatv(GL_MODELVIEW_MATRIX,M);
      glPopMatrix();
      p->worldCoordinate.x = M[12];
      p->worldCoordinate.y = M[13];
      p->worldCoordinate.z = M[14];
      p->displayList = displist;
      primitives.push_back(p);
    }
    else {
      glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      glTranslatef(root->getBaseRadius(),0.0f,clusterHeight);
      glRotatef(visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      drawSubtreeA(desc,desc_rot);
      glRotatef(-visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      glTranslatef(-root->getBaseRadius(),0.0f,-clusterHeight);
      
      float theta = rad_to_deg(atan(root->getBaseRadius()/clusterHeight));
      glRotatef(theta,0.0f,1.0f,0.0f);

      float height = sqrt(clusterHeight*clusterHeight +
                          root->getBaseRadius()*root->getBaseRadius());
      GLfloat M[16];
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
      GLuint displist = glGenLists(1);
      glNewList(displist,GL_COMPILE);
      	glPushMatrix();
      	glMultMatrixf(M);
      	drawCylinder(0.1f,0.1f,height,visSettings.interpolateColor1,
                     visSettings.interpolateColor1,true,false,false);
      	glPopMatrix();
      glEndList();

      Primitive* p = new Primitive;
      glPushMatrix();
      	glTranslatef(0.0f,0.0f,0.5f*height);
      	glGetFloatv(GL_MODELVIEW_MATRIX,M);
      glPopMatrix();
      p->worldCoordinate.x = M[12];
      p->worldCoordinate.y = M[13];
      p->worldCoordinate.z = M[14];
      p->displayList = displist;
      primitives.push_back(p);
      
      glRotatef(-theta,0.0f,1.0f,0.0f);
      glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
    }
  }
  
  GLfloat M[16];
  glGetFloatv(GL_MODELVIEW_MATRIX,M);
  GLuint displist = glGenLists(1);
  glNewList(displist,GL_COMPILE);
    glPushMatrix();
    glMultMatrixf(M);
    glColor4f(visSettings.interpolateColor2.r,visSettings.interpolateColor2.g,
              visSettings.interpolateColor2.b,visSettings.alpha);
    
    float r = pow(root->getTopRadius()*root->getTopRadius()*0.1,0.33);
    float h = visSettings.ellipsoidThreshold*clusterHeight;
    if (r > h) {
      drawEllipsoid(sqrt(r*r*r/h),h);
    }
    else {
      drawSphere(r);
    }
    glPopMatrix();
  glEndList();

  Primitive* p = new Primitive;
  p->worldCoordinate.x = M[12];
  p->worldCoordinate.y = M[13];
  p->worldCoordinate.z = M[14];
  p->displayList = displist;
  primitives.push_back(p);
}

void Visualizer::drawSubtreeAMark(Cluster* root,int rot) {
  int desc_rot = rot + visSettings.branchRotation;
  if (desc_rot < 0) desc_rot += 360;
  else if (desc_rot >= 360) desc_rot -= 360;
    
  for (int i=0; i<root->getNumberOfDescendants(); ++i) {
    Cluster* desc = root->getDescendant(i);
    if (desc->getPosition() < -0.9f) {
      glTranslatef(0.0f,0.0f,clusterHeight);
      drawSubtreeAMark(desc,(root->getNumberOfDescendants()>1)?desc_rot:rot);
      glTranslatef(0.0f,0.0f,-clusterHeight);
      
      GLfloat M[16];
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
      GLuint displist = glGenLists(1);
      glNewList(displist,GL_COMPILE);
      	glPushMatrix();
      	glMultMatrixf(M);
      	drawCylinder(0.1f,0.1f,clusterHeight,RGB_WHITE,RGB_WHITE,true,false,
                     false);
      	glPopMatrix();
      glEndList();

      Primitive* p = new Primitive;
      glPushMatrix();
      	glTranslatef(0.0f,0.0f,0.5f*clusterHeight);
      	glGetFloatv(GL_MODELVIEW_MATRIX,M);
      glPopMatrix();
      p->worldCoordinate.x = M[12];
      p->worldCoordinate.y = M[13];
      p->worldCoordinate.z = M[14];
      p->displayList = displist;
      primitives.push_back(p);
    }
    else {
      glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      glTranslatef(root->getBaseRadius(),0.0f,clusterHeight);
      glRotatef(visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      drawSubtreeAMark(desc,desc_rot);
      glRotatef(-visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      glTranslatef(-root->getBaseRadius(),0.0f,-clusterHeight);

      float theta = rad_to_deg(atan(root->getBaseRadius()/clusterHeight));
      glRotatef(theta,0.0f,1.0f,0.0f);

      float height = sqrt(clusterHeight*clusterHeight +
	                        root->getBaseRadius()*root->getBaseRadius());
      GLfloat M[16];
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
      GLuint displist = glGenLists(1);
      glNewList(displist,GL_COMPILE);
      	glPushMatrix();
      	glMultMatrixf(M);
      	drawCylinder(0.1f,0.1f,height,RGB_WHITE,RGB_WHITE,true,false,false);
      	glPopMatrix();
      glEndList();

      Primitive* p = new Primitive;
      glPushMatrix();
      	glTranslatef(0.0f,0.0f,0.5f*height);
      	glGetFloatv(GL_MODELVIEW_MATRIX,M);
      glPopMatrix();
      p->worldCoordinate.x = M[12];
      p->worldCoordinate.y = M[13];
      p->worldCoordinate.z = M[14];
      p->displayList = displist;
      primitives.push_back(p);
      
      glRotatef(-theta,0.0f,1.0f,0.0f);
      glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
    }
  }
  
  GLfloat M[16];
  glGetFloatv(GL_MODELVIEW_MATRIX,M);
  GLuint displist = glGenLists(1);
  glNewList(displist,GL_COMPILE);
    glPushMatrix();
    glMultMatrixf(M);
    if (isMarked(root)) {
      glColor4f(visSettings.markedColor.r,visSettings.markedColor.g,
                visSettings.markedColor.b,visSettings.alpha);
    }
    else {
      glColor4f(1.0f,1.0f,1.0f,visSettings.alpha);
    }

    float r = pow(root->getTopRadius()*root->getTopRadius()*0.1,0.33);
    float h = visSettings.ellipsoidThreshold*clusterHeight;
    if (r > h) {
      drawEllipsoid(pow(r*r*r/h,0.33f),h);
    }
    else {
      drawSphere(r);
    }
    glPopMatrix();
  glEndList();

  Primitive* p = new Primitive;
  p->worldCoordinate.x = M[12];
  p->worldCoordinate.y = M[13];
  p->worldCoordinate.z = M[14];
  p->displayList = displist;
  primitives.push_back(p);
}

// ------------- TUBES -------------------------------------------------------

void Visualizer::drawSubtreeO(Cluster* root,HSV_Color col,int rot) {
  if (root == lts->getInitialState()->getCluster()) {
    RGB_Color col_rgb = HSV_to_RGB(col);
    glRotatef(180.0f,1.0f,0.0f,0.0f);
    GLfloat M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    GLuint displist = glGenLists(1);
    glNewList(displist,GL_COMPILE);
      glPushMatrix();
      glMultMatrixf(M);
      glColor4f(col_rgb.r,col_rgb.g,col_rgb.b,visSettings.alpha);
      drawHemisphere(root->getTopRadius()); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef(0.0f,0.0f,0.5f*root->getTopRadius());
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back(p);
    glRotatef(-180.0f,1.0f,0.0f,0.0f);
  }
  
  if (!root->hasDescendants()) {
    RGB_Color col_rgb = HSV_to_RGB(col);
    GLfloat M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    GLuint displist = glGenLists(1);
    glNewList(displist,GL_COMPILE);
      glPushMatrix();
      glMultMatrixf(M);
      glColor4f(col_rgb.r,col_rgb.g,col_rgb.b,visSettings.alpha);
      drawHemisphere(root->getTopRadius()); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef(0.0f,0.0f,0.5f*root->getTopRadius());
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back(p);
  }
  else {
    HSV_Color desccol = col + delta_col;

    int desc_rot = rot + visSettings.branchRotation;
    if (desc_rot < 0) desc_rot += 360;
    else if (desc_rot >= 360) desc_rot -= 360;
    
    for (int i=0; i<root->getNumberOfDescendants(); ++i) {
      Cluster* desc = root->getDescendant(i);
      if (desc->getPosition() < -0.9f) {
      	glTranslatef(0.0f,0.0f,clusterHeight);
      	drawSubtreeO(desc,desccol,
                     (root->getNumberOfDescendants()>1)?desc_rot:rot);
      	glTranslatef(0.0f,0.0f,-clusterHeight);
      	
      	GLfloat M[16];
      	glGetFloatv(GL_MODELVIEW_MATRIX,M);
      	GLuint displist = glGenLists(1);
      	glNewList(displist,GL_COMPILE);
      	  glPushMatrix();
      	  glMultMatrixf(M);
	  drawCylinder(root->getTopRadius(),desc->getTopRadius(),clusterHeight,
	               HSV_to_RGB(col),HSV_to_RGB(desccol),true,false,false);
      	  glPopMatrix();
      	glEndList();
      
      	Primitive* p = new Primitive;
      	glPushMatrix();
      	  glTranslatef(0.0f,0.0f,0.5f*clusterHeight);
      	  glGetFloatv(GL_MODELVIEW_MATRIX,M);
      	glPopMatrix();
      	p->worldCoordinate.x = M[12];
      	p->worldCoordinate.y = M[13];
      	p->worldCoordinate.z = M[14];
      	p->displayList = displist;
      	primitives.push_back(p);
      }
      else {
      	glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      	glTranslatef(root->getBaseRadius(),0.0f,clusterHeight);
      	glRotatef(visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
        drawSubtreeO(desc,desccol,desc_rot);
      	glRotatef(-visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      	glTranslatef(-root->getBaseRadius(),0.0f,-clusterHeight);
        
        Point3D b1 = {0.5f*clusterHeight*sin_ibt,0.0f,
                      0.5f*clusterHeight*cos_ibt};
        Point3D b2 = {root->getBaseRadius() - 0.5f*clusterHeight*sin_obt,0.0f,
                      clusterHeight - 0.5f*clusterHeight*cos_obt};
        Point3D b3 = {root->getBaseRadius(),0.0f,clusterHeight};
        Point3D center;
        GLfloat M[16];
      	glGetFloatv(GL_MODELVIEW_MATRIX,M);
      	GLuint displist = glGenLists(1);
      	glNewList(displist,GL_COMPILE);
      	  glPushMatrix();
      	  glMultMatrixf(M);
      	  drawTube(root->getTopRadius(),desc->getTopRadius(),HSV_to_RGB(col),
                   HSV_to_RGB(desccol),true,b1,b2,b3,center);
      	  glPopMatrix();
      	glEndList();
        
        Primitive* p = new Primitive;
      	glPushMatrix();
      	  glTranslatef(center.x,center.y,center.z);
      	  glGetFloatv(GL_MODELVIEW_MATRIX,M);
      	glPopMatrix();
      	p->worldCoordinate.x = M[12];
      	p->worldCoordinate.y = M[13];
      	p->worldCoordinate.z = M[14];
      	p->displayList = displist;
      	primitives.push_back(p);
        
        glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
      }
    }
  }
}

void Visualizer::drawSubtreeOMark(Cluster* root,int rot) {
  RGB_Color color = (isMarked(root)) ? visSettings.markedColor : RGB_WHITE;
  if (root == lts->getInitialState()->getCluster()) {
    glRotatef(180.0f,1.0f,0.0f,0.0f);
    GLfloat M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    GLuint displist = glGenLists(1);
    glNewList(displist,GL_COMPILE);
      glPushMatrix();
      glMultMatrixf(M);
      glColor4f(color.r,color.g,color.b,visSettings.alpha);
      drawHemisphere(root->getTopRadius()); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef(0.0f,0.0f,0.5f*root->getTopRadius());
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back(p);
    glRotatef(-180.0f,1.0f,0.0f,0.0f);
  }
  
  if (!root->hasDescendants()) {
    GLfloat M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    GLuint displist = glGenLists(1);
    glNewList(displist,GL_COMPILE);
      glPushMatrix();
      glMultMatrixf(M);
      glColor4f(color.r,color.g,color.b,visSettings.alpha);
      drawHemisphere(root->getTopRadius()); 
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef(0.0f,0.0f,0.5f*root->getTopRadius());
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back(p);
  }
  else {
    int desc_rot = rot + visSettings.branchRotation;
    if (desc_rot < 0) desc_rot += 360;
    else if (desc_rot >= 360) desc_rot -= 360;
    
    for (int i=0; i<root->getNumberOfDescendants(); ++i) {
      Cluster* desc = root->getDescendant(i);
      if (desc->getPosition() < -0.9f) {
      	glTranslatef(0.0f,0.0f,clusterHeight);
      	drawSubtreeOMark(desc,(root->getNumberOfDescendants()>1)?desc_rot:rot);
      	glTranslatef(0.0f,0.0f,-clusterHeight);
      
      	GLfloat M[16];
      	glGetFloatv(GL_MODELVIEW_MATRIX,M);
      	GLuint displist = glGenLists(1);
      	glNewList(displist,GL_COMPILE);
      	  glPushMatrix();
      	  glMultMatrixf(M);
	  drawCylinder(root->getTopRadius(),desc->getTopRadius(),clusterHeight,
	               color,(isMarked(desc))?visSettings.markedColor:RGB_WHITE,
		       false,false,false);
      	  glPopMatrix();
      	glEndList();
      
      	Primitive* p = new Primitive;
      	glPushMatrix();
      	  glTranslatef(0.0f,0.0f,0.5f*clusterHeight);
      	  glGetFloatv(GL_MODELVIEW_MATRIX,M);
      	glPopMatrix();
      	p->worldCoordinate.x = M[12];
      	p->worldCoordinate.y = M[13];
      	p->worldCoordinate.z = M[14];
      	p->displayList = displist;
      	primitives.push_back(p);
      }
      else {
      	glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      	glTranslatef(root->getBaseRadius(),0.0f,clusterHeight);
      	glRotatef(visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
        drawSubtreeOMark(desc,desc_rot);
      	glRotatef(-visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      	glTranslatef(-root->getBaseRadius(),0.0f,-clusterHeight);
      	
        Point3D b1 = {0.5f*clusterHeight*sin_ibt,0.0f,
                      0.5f*clusterHeight*cos_ibt};
        Point3D b2 = {root->getBaseRadius() - 0.5f*clusterHeight*sin_obt,0.0f,
                      clusterHeight - 0.5f*clusterHeight*cos_obt};
        Point3D b3 = {root->getBaseRadius(),0.0f,clusterHeight};
        Point3D center;
        GLfloat M[16];
      	glGetFloatv(GL_MODELVIEW_MATRIX,M);
      	GLuint displist = glGenLists(1);
      	glNewList(displist,GL_COMPILE);
      	  glPushMatrix();
      	  glMultMatrixf(M);
      	  drawTube(root->getTopRadius(),desc->getTopRadius(),color,
                   (isMarked(desc))?visSettings.markedColor:RGB_WHITE,false,b1,
                   b2,b3,center);
      	  glPopMatrix();
      	glEndList();
      
      	Primitive* p = new Primitive;
      	glPushMatrix();
      	  glTranslatef(center.x,center.y,center.z);
      	  glGetFloatv(GL_MODELVIEW_MATRIX,M);
      	glPopMatrix();
      	p->worldCoordinate.x = M[12];
      	p->worldCoordinate.y = M[13];
      	p->worldCoordinate.z = M[14];
      	p->displayList = displist;
      	primitives.push_back(p);
        
        glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
      }
    }
  }
}

// ------------- CONES ---------------------------------------------------------

void Visualizer::drawSubtreeC(Cluster* root,bool topClosed,HSV_Color col,
                              int rot) {
  if (!root->hasDescendants()) {
    RGB_Color col_rgb = HSV_to_RGB(col);
    GLfloat M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    GLuint displist = glGenLists(1);
    glNewList(displist,GL_COMPILE);
      glPushMatrix();
      glMultMatrixf(M);
      glColor4f(col_rgb.r,col_rgb.g,col_rgb.b,visSettings.alpha);
      drawSphere(root->getTopRadius());
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back(p);
  }
  else {
    HSV_Color desccol = col + delta_col;
    
    int desc_rot = rot + visSettings.branchRotation;
    if (desc_rot < 0) desc_rot += 360;
    else if (desc_rot >= 360) desc_rot -= 360;

    for (int i=0; i<root->getNumberOfDescendants(); ++i) {
      Cluster* desc = root->getDescendant(i);
      if (desc->getPosition() < -0.9f) {
      	glTranslatef(0.0f,0.0f,clusterHeight);
      	drawSubtreeC(desc,false,desccol,
                     (root->getNumberOfDescendants()>1)?desc_rot:rot);
      	glTranslatef(0.0f,0.0f,-clusterHeight);
      }
      else {
      	glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      	glTranslatef(root->getBaseRadius(),0.0f,clusterHeight);
      	glRotatef(visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
       	drawSubtreeC(desc,true,desccol,desc_rot);
       	glRotatef(-visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      	glTranslatef(-root->getBaseRadius(),0.0f,-clusterHeight);
      	glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
      }
    }
    
    GLfloat M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    GLuint displist = glGenLists(1);
    glNewList(displist,GL_COMPILE);
      glPushMatrix();
      glMultMatrixf(M);
      drawCylinder(root->getTopRadius(),root->getBaseRadius(),clusterHeight,
	                 HSV_to_RGB(col),HSV_to_RGB(desccol),true,topClosed,
                   root->getNumberOfDescendants()>1);
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef(0.0f,0.0f,0.5f*clusterHeight);
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back(p);
  }
}

void Visualizer::drawSubtreeCMark(Cluster* root,bool topClosed,int rot) {
  if (!root->hasDescendants()) {
    GLfloat M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    GLuint displist = glGenLists(1);
    glNewList(displist,GL_COMPILE);
      glPushMatrix();
      glMultMatrixf(M);
      if (isMarked(root)) {
        glColor4f(visSettings.markedColor.r,visSettings.markedColor.g,
                  visSettings.markedColor.b,visSettings.alpha);
      }
      else {
        glColor4f(1.0f,1.0f,1.0f,visSettings.alpha);
      }
      drawSphere(root->getTopRadius());
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back(p);
  }
  else {
    vector< Cluster* > descendants;
    root->getDescendants( descendants );
    
    int desc_rot = rot + visSettings.branchRotation;
    if (desc_rot < 0) desc_rot += 360;
    else if (desc_rot >= 360) desc_rot -= 360;
    
    GLfloat M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    GLuint displist = glGenLists(1);
    glNewList(displist,GL_COMPILE);
      glPushMatrix();
      glMultMatrixf(M);
      if (isMarked(root)) {
        drawCylinder(root->getTopRadius(),root->getBaseRadius(),clusterHeight,
                     visSettings.markedColor,RGB_WHITE,true,topClosed,
                     descendants.size()>1);
      }
      else {
	      drawCylinder(root->getTopRadius(),root->getBaseRadius(),clusterHeight,
                     RGB_WHITE,RGB_WHITE,true,topClosed,descendants.size()>1);
      }
      glPopMatrix();
    glEndList();

    Primitive* p = new Primitive;
    glPushMatrix();
      glTranslatef(0.0f,0.0f,0.5f*clusterHeight);
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
    glPopMatrix();
    p->worldCoordinate.x = M[12];
    p->worldCoordinate.y = M[13];
    p->worldCoordinate.z = M[14];
    p->displayList = displist;
    primitives.push_back(p);
    
    for (vector< Cluster* >::iterator descit=descendants.begin(); 
      descit!=descendants.end(); ++descit) {
      if ((**descit).getPosition() < -0.9f) {
	      glTranslatef(0.0f,0.0f,clusterHeight);
	      drawSubtreeCMark(*descit,false,(descendants.size()>1)?desc_rot:rot);
	      glTranslatef(0.0f,0.0f,-clusterHeight);
      }
      else {
      	glRotatef(-(**descit).getPosition()-rot,0.0f,0.0f,1.0f);
      	glTranslatef(root->getBaseRadius(),0.0f,clusterHeight);
      	glRotatef(visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
       	drawSubtreeCMark(*descit,true,desc_rot);
      	glRotatef(-visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      	glTranslatef(-root->getBaseRadius(),0.0f,-clusterHeight);
      	glRotatef((**descit).getPosition()+rot,0.0f,0.0f,1.0f);
      }
    }
  }
}

// ------------- STATES --------------------------------------------------------
void Visualizer::clearDFSStates(State* root)
{
  root->DFSclear();
  for( int i = 0; i != root->getNumberOfOutTransitions(); ++i)
  {
    Transition* outTransition = root->getOutTransitioni(i);
    
    if (!outTransition->isBackpointer()) {
      State* endState = outTransition->getEndState();
      clearDFSStates(endState);
    }
  }
}

void Visualizer::computeStateAbsPos( State* root, int rot, Point3D initVect)
// Does a DFS on the states to calculate their `absolute' position, taking the 
// position of the initial state as (0,0,0). 
// Pre: True (?)
// Post: root->getPosAbs() = absolute position of root, taking the position of
//                           the initial state as (0, 0, 0)
{
  root->DFSvisit();
  Cluster* startCluster = root->getCluster();

  float M[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, M);

  if (root->getRank() == 0) 
  {
    // Root is the initial state of the system.
    Point3D initPos = {0, 0, 0};
    root->setPositionAbs(initPos);
    initVect.x = M[12];
    initVect.y = M[14];
    initVect.z = M[13];

  }

  if( root->getPosition() < -0.9f) {
    Point3D rootPos = { M[12] - initVect.x, 
                        M[14] - initVect.y, 
                       - M[13] + initVect.z};    
    root->setPositionAbs(rootPos);
  }

  else {
    glRotatef(-root->getPosition(), 0.0f, 0.0f, 1.0f);
    glTranslatef(startCluster->getTopRadius(), 0.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, M);
    Point3D rootPos = { M[12] - initVect.x, 
                        M[14] - initVect.y, 
                        - M[13] + initVect.z};

                        
    root->setPositionAbs(rootPos);
    glTranslatef(-startCluster->getTopRadius(), 0.0f, 0.0f);
    glRotatef(root->getPosition(), 0.0f, 0.0f, 1.0f);
                        
  }

  for( int i = 0; i != root->getNumberOfOutTransitions(); ++i)
  {
    Transition* outTransition = root->getOutTransitioni(i);
    State* endState = outTransition->getEndState();

    if (endState->getVisitState() == DFS_WHITE &&
        !outTransition->isBackpointer()) {

      int desc_rot = rot + visSettings.branchRotation;
      if (desc_rot < 0) {
        desc_rot += 360;
      }
      else if (desc_rot >= 360) {
        desc_rot -=360;
      }

      Cluster* endCluster = endState->getCluster();

      if (endState->getRank() != root->getRank()) {
        
        if ( endCluster->getPosition() < -0.9f) {
          //endCluster is centered, only descend
          glTranslatef(0.0f, 0.0f, clusterHeight);
          computeStateAbsPos(endState, 
            (startCluster->getNumberOfDescendants()>1)?desc_rot: rot,
            initVect);
          glTranslatef(0.0f, 0.0f, -clusterHeight);
        }
        else {
          glRotatef(-endCluster->getPosition() - rot, 0.0f, 0.0f, 1.0f);
          glTranslatef(startCluster->getBaseRadius(), 0.0f, clusterHeight);
          glRotatef(visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f);
          computeStateAbsPos(endState, desc_rot, initVect);
          glRotatef(-visSettings.outerBranchTilt, 0.0f, 1.0f, 0.0f);
          glTranslatef(-startCluster->getBaseRadius(), 0.0f, -clusterHeight);
          glRotatef(endCluster->getPosition() + rot, 0.0f, 0.0f, 1.0f);
        }
      }
    }
  }

  // Finalize this node
  root->DFSfinish();
}
void Visualizer::drawStates(Cluster* root,int rot) {

  vector< State* > c_ss;
  root->getStates(c_ss);
  for (vector< State* >::iterator s_it=c_ss.begin(); s_it!=c_ss.end(); ++s_it) {
    if ((**s_it).getPosition() < -0.9f) {
      drawSphereState();
    }
    else {
      glRotatef(-(**s_it).getPosition(),0.0f,0.0f,1.0f);
      glTranslatef(root->getTopRadius(),0.0f,0.0f);
      drawSphereState();
      glTranslatef(-root->getTopRadius(),0.0f,0.0f);
      glRotatef((**s_it).getPosition(),0.0f,0.0f,1.0f);
      }
  }

  int desc_rot = rot + visSettings.branchRotation;
  if (desc_rot < 0) desc_rot += 360;
  else if (desc_rot >= 360) desc_rot -= 360;
  for (int i=0; i<root->getNumberOfDescendants(); ++i) {
    Cluster* desc = root->getDescendant(i);
    if (desc->getPosition() < -0.9f) {
      // descendant is centered
      glTranslatef(0.0f,0.0f,clusterHeight);
      drawStates(desc,(root->getNumberOfDescendants()>1)?desc_rot:rot);
      glTranslatef(0.0f,0.0f,-clusterHeight);
    }
    else {
      glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      glTranslatef(root->getBaseRadius(),0.0f,clusterHeight);
      glRotatef(visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      drawStates(desc,desc_rot);
      glRotatef(-visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      glTranslatef(-root->getBaseRadius(),0.0f,-clusterHeight);
      glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
    }
  }
}

void Visualizer::drawStatesMark(Cluster* root,int rot) {
  vector< State* > c_ss;
  root->getStates(c_ss);
  for (vector< State* >::iterator s_it=c_ss.begin(); s_it!=c_ss.end(); ++s_it) {
    if (isMarked(*s_it)) {
      glColor4f(visSettings.markedColor.r,visSettings.markedColor.g,
                visSettings.markedColor.b,1.0f);
    }
    else {
      glColor4f(1.0f,1.0f,1.0f,1.0f);
    }
    if ((**s_it).getPosition() < -0.9f) {
      drawSphereState();
    }
    else {
      glRotatef(-(**s_it).getPosition(),0.0f,0.0f,1.0f);
      glTranslatef(root->getTopRadius(),0.0f,0.0f);
      drawSphereState();
      glTranslatef(-root->getTopRadius(),0.0f,0.0f);
      glRotatef((**s_it).getPosition(),0.0f,0.0f,1.0f);
    }
  }

  int desc_rot = rot + visSettings.branchRotation;
  if (desc_rot < 0) desc_rot += 360;
  else if (desc_rot >= 360) desc_rot -= 360;
  for (int i=0; i<root->getNumberOfDescendants(); ++i) {
    Cluster* desc = root->getDescendant(i);
    if (desc->getPosition() < -0.9f) {
      // descendant is centered
      glTranslatef(0.0f,0.0f,clusterHeight);
      drawStatesMark(desc,(root->getNumberOfDescendants()>1)?desc_rot:rot);
      glTranslatef(0.0f,0.0f,-clusterHeight);
    }
    else {
      glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      glTranslatef(root->getBaseRadius(),0.0f,clusterHeight);
      glRotatef(visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      drawStatesMark(desc,desc_rot);
      glRotatef(-visSettings.outerBranchTilt,0.0f,1.0f,0.0f);
      glTranslatef(-root->getBaseRadius(),0.0f,-clusterHeight);
      glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
    }
  }
}
// ------------- TRANSITIONS ---------------------------------------------------


void Visualizer::drawTransitions(State* root)
{
  root->DFSvisit();

  for( int i = 0; i != root->getNumberOfOutTransitions(); ++i)
  {
    Transition* outTransition = root->getOutTransitioni(i);

    State* endState = outTransition->getEndState();

    // Draw transition from root to endState
    if (outTransition->isBackpointer() && displayBackpointers) {
      glColor4f(visSettings.upEdgeColor.r, visSettings.upEdgeColor.g,
                visSettings.upEdgeColor.b, 1.0f);
      drawBackPointer(root, endState);
    }
    else if (!outTransition->isBackpointer() && displayTransitions) {
      glColor4f(visSettings.downEdgeColor.r, visSettings.downEdgeColor.g,
                visSettings.downEdgeColor.b, 1.0f);
      drawForwardPointer(root, endState);
    }
    
    // If we haven't visited endState before, do so now.
    if (endState->getVisitState() == DFS_WHITE && 
        !outTransition->isBackpointer()) {

      // Move to the next state
      drawTransitions(endState);
    }
  }

  // Finalize this node
  root->DFSfinish();
}

void Visualizer::drawForwardPointer(State* startState, State* endState)
{

  Point3D startPoint = startState->getPositionAbs();
  Point3D endPoint = endState->getPositionAbs();

  glBegin(GL_LINES);
    glVertex3f(startPoint.x, startPoint.y, startPoint.z);
    glVertex3f(endPoint.x, endPoint.y, endPoint.z);
  glEnd();
}

void Visualizer::drawBackPointer(State* startState, State* endState)
{

  int rankDiff = startState->getRank() - endState->getRank();
  Point3D startPoint = startState->getPositionAbs();

  Point3D startControl;
  startControl.x = startPoint.x * rankDiff * clusterHeight / 2;
  startControl.y = startPoint.y * rankDiff * clusterHeight / 2;
  startControl.z = startPoint.z;

  Point3D endPoint   = endState->getPositionAbs();

  Point3D zUp = {0.0, 0.0, - endPoint.z * rankDiff * clusterHeight / 2};
  Point3D endControl;
  endControl.x = startControl.x;
  endControl.y = startControl.y;
  endControl.z = endPoint.z;

  endControl = endControl + zUp;

  GLfloat ctrlPts [4][3] = { {startPoint.x, startPoint.y, startPoint.z},
                             {startControl.x, startControl.y, startControl.z},
                             {endControl.x, endControl.y, endControl.z},
                             {endPoint.x, endPoint.y, endPoint.z} };
  
                             
  glBegin(GL_LINE_STRIP);
    for (GLint k = 0; k < 50; ++k) {
      float t  = (float)k / 49;
      float it = 1.0f - t;

      float b0 =      t *  t *  t;
      float b1 = 3 *  t *  t * it;
      float b2 = 3 *  t * it * it;
      float b3 =     it * it * it;

      float x = b0 * ctrlPts[0][0] +
                b1 * ctrlPts[1][0] + 
                b2 * ctrlPts[2][0] +
                b3 * ctrlPts[3][0];

      float y = b0 * ctrlPts[0][1] +
                b1 * ctrlPts[1][1] +
                b2 * ctrlPts[2][1] +
                b3 * ctrlPts[3][1];
                
      float z = b0 * ctrlPts[0][2] +
                b1 * ctrlPts[1][2] +
                b2 * ctrlPts[2][2] +
                b3 * ctrlPts[3][2];

      glVertex3f( x, y, z);
      

    }
  /*
    glVertex3f(startPoint.x, startPoint.y, startPoint.z);
    glVertex3f(startControl.x, startControl.y, startControl.z);
    glVertex3f(endControl.x, endControl.y, endControl.z);
    glVertex3f(endPoint.x, endPoint.y, endPoint.z);
  */
  glEnd();

  
}
// ------------- PRIMITIVES ----------------------------------------------------

// draws a cylinder around z-axis with given base radius, top radius, height,
// color at base, color at top, closed base if baseclosed and closed top if
// topclosed and interpolates between the given colors over the length of the
// cylinder
void Visualizer::drawCylinder(float baserad,float toprad,float height,
                              RGB_Color basecol,RGB_Color topcol,
                              bool interpolate,bool baseclosed,bool topclosed) {
  int M = visSettings.quality;
  float nxg = height;
  float nzg = baserad - toprad;
  float r = sqrt(nxg*nxg + nzg*nzg);
  nxg = nxg/r;
  nzg = nzg/r;

  if (baseclosed) {
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0,0.0,-1.0);
    glColor4f(basecol.r,basecol.g,basecol.b,visSettings.alpha);
    glVertex3f(0.0,0.0,0.0);
    for (int j=M; j>=0; --j) {
      glVertex3f(baserad*cos_theta1[j],baserad*sin_theta1[j],0.0f);
    }
    glEnd();
  }
  
  if (interpolate) {
    glBegin(GL_QUAD_STRIP);
    for (int j=0; j<=M; ++j) {
      glNormal3f(cos_theta1[j]*nxg,sin_theta1[j]*nxg,nzg);
      glColor4f(topcol.r,topcol.g,topcol.b,visSettings.alpha);
      glVertex3f(toprad*cos_theta1[j],toprad*sin_theta1[j],height);
      glColor4f(basecol.r,basecol.g,basecol.b,visSettings.alpha);
      glVertex3f(baserad*cos_theta1[j],baserad*sin_theta1[j],0.0f);
    }
    glEnd();
  }
  else {
    float demirad = 0.5f*(toprad+baserad);
    float demiheight = 0.5f*height;
    glColor4f(basecol.r,basecol.g,basecol.b,visSettings.alpha);
    glBegin(GL_QUAD_STRIP);
    for (int j=0; j<=M; ++j) {
      glNormal3f(nxg*cos_theta1[j],nxg*sin_theta1[j],nzg);
      glVertex3f(demirad*cos_theta1[j],demirad*sin_theta1[j],demiheight);
      glVertex3f(baserad*cos_theta1[j],baserad*sin_theta1[j],0.0f);
    }
    glEnd();
  
    glColor4f(topcol.r,topcol.g,topcol.b,visSettings.alpha);
    glBegin(GL_QUAD_STRIP);
    for (int j=0; j<=M; ++j) {
      glNormal3f(nxg*cos_theta1[j],nxg*sin_theta1[j],nzg);
      glVertex3f(toprad*cos_theta1[j],toprad*sin_theta1[j],height);
      glVertex3f(demirad*cos_theta1[j],demirad*sin_theta1[j],demiheight);
    }
    glEnd();
  }

  if (topclosed) {
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0,0.0,1.0);
    glColor4f(topcol.r,topcol.g,topcol.b,visSettings.alpha);
    glVertex3f(0.0,0.0,height);
    for (int j=0; j<=M; ++j) {
      glVertex3f(toprad*cos_theta1[j],toprad*sin_theta1[j],height);
    }
    glEnd();
  }
}

void Visualizer::drawHemisphere(float r) {
  int n = visSettings.quality;
  int ndiv2 = n/2;
  
  // precompute the cosines and sines that are needed during drawing
  vector< float > cos_theta3(ndiv2+1);
  vector< float > sin_theta3(ndiv2+1);
  float delta = 0.5f*PI/ndiv2;
  float theta = 0.0f;
  for (int j=0; j<=ndiv2; ++j) {
    cos_theta3[j] = cos(theta);
    sin_theta3[j] = sin(theta);
    theta += delta;
  }
  
  float ex,ey,ez,px,py,pz;
  // draw the hemisphere by drawing rings around the z-axis
  for (int j=0; j<ndiv2; ++j) {
    glBegin(GL_QUAD_STRIP);
    for (int i=0; i<=n; ++i) {
      ex = cos_theta3[j+1]*cos_theta1[i];
      ey = cos_theta3[j+1]*sin_theta1[i];
      ez = sin_theta3[j+1];
      px = r*ex;
      py = r*ey;
      pz = r*ez;
      glNormal3f(ex,ey,ez);
      glVertex3f(px,py,pz);
      
      ex = cos_theta3[j]*cos_theta1[i];
      ey = cos_theta3[j]*sin_theta1[i];
      ez = sin_theta3[j];
      px = r*ex;
      py = r*ey;
      pz = r*ez;
      glNormal3f(ex,ey,ez);
      glVertex3f(px,py,pz);
    }
    glEnd();
  }
}

void Visualizer::drawSphere(float r) {
  drawEllipsoid(r,r);
}

void Visualizer::drawEllipsoid(float d,float h) {
  float ex,ey,ez,px,py,pz;
  for (int j=0; j<visSettings.quality; ++j) {
    glBegin(GL_QUAD_STRIP);
    for (int i=0; i<=visSettings.quality; ++i) {
      ex = cos_theta2[j+1]*cos_theta1[i];
      ey = cos_theta2[j+1]*sin_theta1[i];
      ez = sin_theta2[j+1];
      px = d*ex;
      py = d*ey;
      pz = h*ez;
      glNormal3f(ex,ey,ez);
      glVertex3f(px,py,pz);
      
      ex = cos_theta2[j]*cos_theta1[i];
      ey = cos_theta2[j]*sin_theta1[i];
      ez = sin_theta2[j];
      px = d*ex;
      py = d*ey;
      pz = h*ez;
      glNormal3f(ex,ey,ez);
      glVertex3f(px,py,pz);
    }
    glEnd();
  }
}

void Visualizer::drawSphereState() {
  float ex,ey,ez,px,py,pz;
  float r = visSettings.nodeSize;
  for (int j=0; j<4; ++j) {
    glBegin(GL_QUAD_STRIP);
    for (int i=0; i<=4; ++i) {
      ex = cos_theta2_s[j+1]*cos_theta1_s[i];
      ey = cos_theta2_s[j+1]*sin_theta1_s[i];
      ez = sin_theta2_s[j+1];
      px = r*ex;
      py = r*ey;
      pz = r*ez;
      glNormal3f(ex,ey,ez);
      glVertex3f(px,py,pz);
      
      ex = cos_theta2_s[j]*cos_theta1_s[i];
      ey = cos_theta2_s[j]*sin_theta1_s[i];
      ez = sin_theta2_s[j];
      px = r*ex;
      py = r*ey;
      pz = r*ez;
      glNormal3f(ex,ey,ez);
      glVertex3f(px,py,pz);
    }
    glEnd();
  }
}

// Draws a tube around a cubic Bezier curve of which begin point b0 is in the
// origin. The coordinates of the other points are given as parameters, as are
// the base radius, top radius, base color and top color. The center of the tube
// will be stored in the center parameter. Coordinates of b1, b2, b3 and center
// are relative to the current origin (not world coordinates) and b1, b2 and b3
// are all assumed to be in the (x,z)-plane.
// If interpolate is true, the color of the tube is interpolated between basecol 
// and topcol over the length of the tube; otherwise the bottom half of the tube
// is basecol and the top half topcol.
void Visualizer::drawTube(float baserad,float toprad,RGB_Color basecol,
                          RGB_Color topcol,bool interpolate,Point3D b1,
                          Point3D b2,Point3D b3,Point3D &center) {
  int M = visSettings.quality;
  
  // compute the coordinates of the center of the tube (i.e. at t = 0.5)
  center = (0.375f*b1) + (0.375f*b2) + (0.125f*b3);
  
  // We know that the curve lies completely in the (x,z)-plane, so the Frenet 
  // frame can be computed easily:
  // T(i) is the tangent vector (first derivative of the curve)
  // N(i) is the principal normal vector and equals (-T(i).z, 0, T(i).x)
  // B(i) is the binormal vector and equals (0,1,0) for all i
  // Because the contour of the tube always lies in the local (N,B)-plane, we 
  // won't be needing the tangent vectors T(i) and only have to compute the N(i) 

  vector< Point3D > N(M+1); // Frenet frame principal normals
  
  float t,it;
  for (int i=0; i<=M; ++i) {
      t = float(i) / float(M);
      it = 1-t;
      
      // compute b'(t)
      Point3D dbt = (3*it*it)*b1 + (6*t*it)*(b2-b1) + (3*t*t)*(b3-b2);
      normalize(dbt);
      N[i].x = -dbt.z;
      N[i].y = 0;
      N[i].z = dbt.x;   
  }
  
  Point3D mesh_vs[M+1][M+1]; // vertices of the mesh
  
  float f1,f2,f3;
  for (int i=0; i<=M; ++i) {
    t = float(i) / float(M);
    it = 1-t;
    f1 = 3*t*it*it;
    f2 = 3*t*t*it;
    f3 = t*t*t;
    Point3D F0 = { it*baserad*N[i].x, -it*baserad, it*baserad*N[i].z };
    Point3D F1 = { t*toprad*N[i].x, -t*toprad, t*toprad*N[i].z };
    
    for (int j=0; j<=M; ++j) {
      mesh_vs[i][j].x = f1*b1.x + f2*b2.x + f3*b3.x
                      + F0.x*cos_theta1[j] + F1.x*cos_theta1[j];
      mesh_vs[i][j].y = f1*b1.y + f2*b2.y + f3*b3.y
                      + F0.y*sin_theta1[j] + F1.y*sin_theta1[j];
      mesh_vs[i][j].z = f1*b1.z + f2*b2.z + f3*b3.z
                      + F0.z*cos_theta1[j] + F1.z*cos_theta1[j];
    }
  }

  Point3D mesh_ns[M+1][M+1]; // normals of the mesh vertices

  for (int i=1; i<=M+1; ++i) {
    if (i == 1) {
      for (int j=0; j<M; ++j) {
	Point3D v1 = mesh_vs[i][j] - mesh_vs[i-1][j];
	Point3D v2 = mesh_vs[i-1][(j==0)?M-1:j-1] - mesh_vs[i-1][j];
	Point3D v4 = mesh_vs[i-1][j+1] - mesh_vs[i-1][j];
	Point3D n1 = cross_product(v1,v2);
	Point3D n4 = cross_product(v4,v1);
	mesh_ns[i-1][j].x = n1.x + n4.x;
	mesh_ns[i-1][j].y = n1.y + n4.y;
	mesh_ns[i-1][j].z = n1.z + n4.z;
	normalize(mesh_ns[i-1][j]);
      }
    }
    else if (i == M+1) {
      for (int j=0; j<M; ++j) {
	Point3D v2 = mesh_vs[i-1][(j==0)?M-1:j-1] - mesh_vs[i-1][j];
	Point3D v3 = mesh_vs[i-2][j] - mesh_vs[i-1][j];
	Point3D v4 = mesh_vs[i-1][j+1] - mesh_vs[i-1][j];
	Point3D n2 = cross_product(v2,v3);
	Point3D n3 = cross_product(v3,v4);
	mesh_ns[i-1][j].x = n2.x + n3.x;
	mesh_ns[i-1][j].y = n2.y + n3.y;
	mesh_ns[i-1][j].z = n2.z + n3.z;
	normalize(mesh_ns[i-1][j]);
      }
    }
    else {
      for (int j=0; j<M; ++j) {
	Point3D v1 = mesh_vs[i][j] - mesh_vs[i-1][j];
	Point3D v2 = mesh_vs[i-1][(j==0)?M-1:j-1] - mesh_vs[i-1][j];
	Point3D v3 = mesh_vs[i-2][j] - mesh_vs[i-1][j];
	Point3D v4 = mesh_vs[i-1][j+1] - mesh_vs[i-1][j];
	Point3D n1 = cross_product(v1,v2);
	Point3D n2 = cross_product(v2,v3);
	Point3D n3 = cross_product(v3,v4);
	Point3D n4 = cross_product(v4,v1);
	mesh_ns[i-1][j].x = n1.x + n2.x + n3.x + n4.x;
	mesh_ns[i-1][j].y = n1.y + n2.y + n3.y + n4.y;
	mesh_ns[i-1][j].z = n1.z + n2.z + n3.z + n4.z;
	normalize(mesh_ns[i-1][j]);
      }
    }
    mesh_ns[i-1][M] = mesh_ns[i-1][0];
  }
  
  float t1,it1;
  if (interpolate) {
    for (int i=0; i<M; ++i) {
      t1 = float(i+1) / float(M);
      it1 = 1-t1;
      t = float(i) / float(M);
      it = 1-t;
      glBegin(GL_QUAD_STRIP);
      for (int j=0; j<=M; ++j) {
        glNormal3f(mesh_ns[i+1][j].x,mesh_ns[i+1][j].y,mesh_ns[i+1][j].z);
        glColor4f(it1*basecol.r+t1*topcol.r,it1*basecol.g+t1*topcol.g,
		              it1*basecol.b+t1*topcol.b,visSettings.alpha);
        glVertex3f(mesh_vs[i+1][j].x,mesh_vs[i+1][j].y,mesh_vs[i+1][j].z);
        
        glNormal3f(mesh_ns[i][j].x,mesh_ns[i][j].y,mesh_ns[i][j].z);
        glColor4f(it*basecol.r+t*topcol.r,it*basecol.g+t*topcol.g,
		              it*basecol.b+t*topcol.b,visSettings.alpha);
        glVertex3f(mesh_vs[i][j].x,mesh_vs[i][j].y,mesh_vs[i][j].z);
      }
      glEnd();
    }
  }
  else {
    glColor4f(basecol.r,basecol.g,basecol.b,visSettings.alpha);
    for (int i=0; i<M; ++i) {
      if (i == M/2) {
	      glColor4f(topcol.r,topcol.g,topcol.b,visSettings.alpha);
      }
      glBegin(GL_QUAD_STRIP);
      for (int j=0; j<=M; ++j) {
        glNormal3f(mesh_ns[i+1][j].x,mesh_ns[i+1][j].y,mesh_ns[i+1][j].z);
        glVertex3f(mesh_vs[i+1][j].x,mesh_vs[i+1][j].y,mesh_vs[i+1][j].z);
        
        glNormal3f(mesh_ns[i][j].x,mesh_ns[i][j].y,mesh_ns[i][j].z);
        glVertex3f(mesh_vs[i][j].x,mesh_vs[i][j].y,mesh_vs[i][j].z);
      }
      glEnd();
    }
  }
}

