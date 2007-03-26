#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "visualizer.h"

using namespace std;
using namespace Utils;

bool Distance_greater::operator()(const Cluster *c1, const Cluster *c2) const {
  Point3D d1 = c1->getCoordinates() - viewpoint;
  Point3D d2 = c2->getCoordinates() - viewpoint;
  return (dot_product(d1,d1) > dot_product(d2,d2));
}

Visualizer::Visualizer(Mediator* owner,Settings* ss) {
  lts = NULL;
  mediator = owner;
  settings = ss;
  settings->subscribe(BranchRotation,this);
  settings->subscribe(OuterBranchTilt,this);
  /*settings->subscribe(Alpha,this);
  settings->subscribe(InnerBranchTilt,this);
  settings->subscribe(NodeSize,this);
  settings->subscribe(LongInterpolation,this);
  settings->subscribe(DownEdgeColor,this);
  settings->subscribe(InterpolateColor1,this);
  settings->subscribe(InterpolateColor2,this);
  settings->subscribe(MarkedColor,this);
  settings->subscribe(StateColor,this);
  settings->subscribe(UpEdgeColor,this);*/
  
  primitiveFactory = new PrimitiveFactory(settings);
  sin_ibt = float(sin(deg_to_rad(settings->getInt(InnerBranchTilt))));
  cos_ibt = float(cos(deg_to_rad(settings->getInt(InnerBranchTilt))));
  sin_obt = float(sin(deg_to_rad(settings->getInt(OuterBranchTilt))));
  cos_obt = float(cos(deg_to_rad(settings->getInt(OuterBranchTilt))));
  
  visStyle = CONES;
}

Visualizer::~Visualizer() {
  delete primitiveFactory;
}

float Visualizer::getHalfStructureHeight() const {
  if (lts == NULL) return 0.0f;
  return clusterHeight*(lts->getNumberOfRanks()-1) / 2.0f;
}

void Visualizer::setLTS(LTS* l) {
  lts = l;
  fillClusters();
}

Utils::VisStyle Visualizer::getVisStyle() const {
  return visStyle;
}

void Visualizer::setMarkStyle(Utils::MarkStyle ms) {
  markStyle = ms;
}

void Visualizer::setVisStyle(Utils::VisStyle vs) {
  visStyle = vs;
}

void Visualizer::notify(SettingID s) {
  switch (s) {
    case OuterBranchTilt:
      sin_obt = float(sin(deg_to_rad(settings->getInt(OuterBranchTilt))));
      cos_obt = float(cos(deg_to_rad(settings->getInt(OuterBranchTilt))));
      /* fall through */
    case BranchRotation:
      if (lts != NULL) {
        updateClusterMatrices(lts->getInitialState()->getCluster(),0);
      }
      break;
    default:
      break;
  }
}

void Visualizer::computeBoundsInfo(float &bcw,float &bch) {
  bcw = 0.0f;
  bch = 0.0f;
  if (lts != NULL) {
    computeSubtreeBounds(lts->getInitialState()->getCluster(),bcw,bch);
  }
}

void Visualizer::computeSubtreeBounds(Cluster* root,float &bw,float &bh) {
// compute the bounding cylinder of the structure.
  if (!root->hasDescendants()) {
    bw = root->getTopRadius();
    bh = 2.0f * root->getTopRadius();
  }
  else {
    Cluster *desc;
    int i;
    for (i=0; i<root->getNumberOfDescendants(); ++i) {
      desc = root->getDescendant(i);
      if (desc->getPosition() < -0.9f) {
      	// descendant is centered
      	float dw = 0.0f;
      	float dh = 0.0f;
      	computeSubtreeBounds(desc,dw,dh);
      	bw = max(bw,dw);
      	bh = max(bh,dh);
      }
      else {
      	float dw = 0.0f;
      	float dh = 0.0f;
      	computeSubtreeBounds(desc,dw,dh);
      	bw = max(bw,root->getBaseRadius() + dh*sin_obt + dw*cos_obt);
      	bh = max(bh,dh*cos_obt + dw*sin_obt);
      }
    }
    bw = max(bw,root->getTopRadius());
    bh += clusterHeight;
  }
}

// ------------- STRUCTURE -----------------------------------------------------

void Visualizer::drawStructure() {
  if (lts == NULL) return;

  unsigned char alpha = settings->getUByte(Alpha);
  if (markStyle == NO_MARKS) {
    RGB_Color ic1 = settings->getRGB(InterpolateColor1);
    RGB_Color ic2 = settings->getRGB(InterpolateColor2);
    HSV_Color hsv1 = RGB_to_HSV(ic1);
    HSV_Color hsv2 = RGB_to_HSV(ic2);
    if (ic1.r == ic1.g  &&  ic1.g == ic1.b) {
      hsv1.h = hsv2.h;
    }
    if (ic2.r == ic2.g  &&  ic2.g == ic2.b) {
      hsv2.h = hsv1.h;
    }
    RGB_Color col;
    for (unsigned int i=0; i<clusters.size(); ++i) {
      // set color
      col = HSV_to_RGB(interpolate(hsv1,hsv2,
            float(clusters[i]->getRank()) / float(lts->getNumberOfRanks()-1),
            settings->getBool(LongInterpolation)));
      glColor4ub(col.r,col.g,col.b,alpha);
      glPushMatrix();
        glMultMatrixf(clusters[i]->getMatrix());
        primitiveFactory->drawPrimitive(clusters[i]->getPrimitive());
      glPopMatrix();
    }
  }
  else {
    RGB_Color mc = settings->getRGB(MarkedColor);
    for (unsigned int i=0; i<clusters.size(); ++i) {
      if (isMarked(clusters[i])) {
        glColor4ub(mc.r,mc.g,mc.b,alpha);
      }
      else {
        glColor4ub(255,255,255,alpha);
      }
      glPushMatrix();
        glMultMatrixf(clusters[i]->getMatrix());
        primitiveFactory->drawPrimitive(clusters[i]->getPrimitive());
      glPopMatrix();
    }
  }
}

void Visualizer::fillClusters() {
  // compute the cluster height that results in a picture with a "nice" aspect
  // ratio
  float ratio = lts->getInitialState()->getCluster()->getSize() /
    (lts->getNumberOfRanks()-1);
  clusterHeight = max(4,round_to_int(40.0f * ratio)) / 10.0f;

  clusters.clear();
  clusters.reserve(lts->getNumberOfClusters());
  initClusterData(lts->getInitialState()->getCluster(),true,0);
}

void Visualizer::initClusterData(Cluster *root,bool topClosed,int rot) {
  if (!root->hasDescendants()) {
    float r = root->getTopRadius();
    glPushMatrix();
      glScalef(r,r,r);
      glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat*)root->getMatrix());
    glPopMatrix();
    root->setPrimitive(primitiveFactory->makeSphere());
  }
  else {
    int desc_rot = rot + settings->getInt(BranchRotation);
    if (desc_rot < 0) desc_rot += 360;
    else if (desc_rot >= 360) desc_rot -= 360;

    glTranslatef(0.0f,0.0f,clusterHeight);
    for (int i=0; i<root->getNumberOfDescendants(); ++i) {
      Cluster* desc = root->getDescendant(i);
      if (desc->getPosition() < -0.9f) {
      	initClusterData(desc,false,(root->getNumberOfDescendants()>1)?desc_rot:rot);
      }
      else {
      	glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      	glTranslatef(root->getBaseRadius(),0.0f,0.0f);
      	glRotatef(settings->getInt(OuterBranchTilt),0.0f,1.0f,0.0f);
       	initClusterData(desc,true,desc_rot);
      	glRotatef(-settings->getInt(OuterBranchTilt),0.0f,1.0f,0.0f);
      	glTranslatef(-root->getBaseRadius(),0.0f,0.0f);
      	glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
      }
    }
    glTranslatef(0.0f,0.0f,-clusterHeight);
    
    float r = root->getBaseRadius() / root->getTopRadius();
    if (r > 1.0f) {
      glPushMatrix();
        glTranslatef(0.0f,0.0f,0.5f*clusterHeight);
        glRotatef(180.0f,1.0f,0.0f,0.0f);
        glScalef(root->getBaseRadius(),root->getBaseRadius(),clusterHeight);
        glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat*)root->getMatrix());
      glPopMatrix();
      r = 1.0f / r;
      root->setPrimitive(primitiveFactory->makeCone(r,topClosed,
            root->getNumberOfDescendants() > 1));
    }
    else {
      glPushMatrix();
        glTranslatef(0.0f,0.0f,0.5f*clusterHeight);
        glScalef(root->getTopRadius(),root->getTopRadius(),clusterHeight);
        glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat*)root->getMatrix());
      glPopMatrix();
      root->setPrimitive(primitiveFactory->makeCone(r,
        root->getNumberOfDescendants() > 1,topClosed));
    }
  }
  clusters.push_back(root);
}

void Visualizer::updateClusterPrimitives() {
}

void Visualizer::updateClusterMatrices(Cluster *root,int rot) {
  if (!root->hasDescendants()) {
    float r = root->getTopRadius();
    glPushMatrix();
      glScalef(r,r,r);
      glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat*)root->getMatrix());
    glPopMatrix();
  }
  else {
    int desc_rot = rot + settings->getInt(BranchRotation);
    if (desc_rot < 0) desc_rot += 360;
    else if (desc_rot >= 360) desc_rot -= 360;

    glTranslatef(0.0f,0.0f,clusterHeight);
    for (int i=0; i<root->getNumberOfDescendants(); ++i) {
      Cluster* desc = root->getDescendant(i);
      if (desc->getPosition() < -0.9f) {
      	updateClusterMatrices(desc,
            (root->getNumberOfDescendants()>1)?desc_rot:rot);
      }
      else {
      	glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      	glTranslatef(root->getBaseRadius(),0.0f,0.0f);
      	glRotatef(settings->getInt(OuterBranchTilt),0.0f,1.0f,0.0f);
       	updateClusterMatrices(desc,desc_rot);
      	glRotatef(-settings->getInt(OuterBranchTilt),0.0f,1.0f,0.0f);
      	glTranslatef(-root->getBaseRadius(),0.0f,0.0f);
      	glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
      }
    }
    glTranslatef(0.0f,0.0f,-clusterHeight);
    
    float r = root->getBaseRadius() / root->getTopRadius();
    glPushMatrix();
      glTranslatef(0.0f,0.0f,0.5f*clusterHeight);
      if (r > 1.0f) {
        glScalef(root->getBaseRadius(),-root->getBaseRadius(),-clusterHeight);
      }
      else {
        glScalef(root->getTopRadius(),root->getTopRadius(),clusterHeight);
      }
      glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat*)root->getMatrix());
    glPopMatrix();
  }
}

void Visualizer::sortClusters(Point3D viewpoint) {
  stable_sort(clusters.begin(),clusters.end(),Distance_greater(viewpoint));
}

bool Visualizer::isMarked(Cluster* c) {
  return ((markStyle == MARK_STATES && c->hasMarkedState()) || 
          (markStyle == MARK_DEADLOCKS && c->hasDeadlock()) ||
          (markStyle == MARK_TRANSITIONS && c->hasMarkedTransition()));
}

// ------------- STATES --------------------------------------------------------

void Visualizer::drawStates() {
  if (lts == NULL) return;

  if (markStyle == NO_MARKS) {
    RGB_Color sc = settings->getRGB(StateColor);
    glColor4ub(sc.r,sc.g,sc.b,255);
    drawStates(lts->getInitialState()->getCluster(),0);
  }
  else {
    drawStatesMark(lts->getInitialState()->getCluster(),0);
  }
}

void Visualizer::clearDFSStates(State* root) {
  root->DFSclear();
  for(int i=0; i!=root->getNumberOfOutTransitions(); ++i) {
    Transition* outTransition = root->getOutTransitioni(i);
    if (!outTransition->isBackpointer()) {
      State* endState = outTransition->getEndState();
      if (endState->getVisitState() != DFS_WHITE) {
        clearDFSStates(endState);
      }
    }
  }
}

void Visualizer::computeStateAbsPos(State* root, int rot, Point3D initVect) {
// Does a DFS on the states to calculate their `absolute' position, taking the 
// position of the initial state as (0,0,0). 
// Secondly, it also assigns the incoming and outgoing control points for the 
// back pointers of each state. 
  root->DFSvisit();
  Cluster* startCluster = root->getCluster();

  float M[16];
  float N[16]; // For the backpointers
  glGetFloatv(GL_MODELVIEW_MATRIX, M);

  if (root->getRank() == 0) {
    // Root is the initial state of the system.
    Point3D initPos = {0, 0, 0};
    root->setPositionAbs(initPos);
    initVect.x = M[12];
    initVect.y = M[14];
    initVect.z = M[13];
  }

  if(root->getPosition() < -0.9f) {
    // The outgoing vector of the state lies clusterHeight above the state.
    glTranslatef(0.0f, 0.0f, 2 * clusterHeight);
    glGetFloatv(GL_MODELVIEW_MATRIX, N);
    Point3D outgoingVect = { N[12] - initVect.x,
                             N[14] - initVect.y,
                           - N[13] + initVect.z };
    root->setOutgoingControl( outgoingVect );
    glTranslatef(0.0f, 0.0f, -2 * clusterHeight);

    Point3D rootPos = { M[12] - initVect.x, 
                        M[14] - initVect.y, 
                      - M[13] + initVect.z};    
    root->setPositionAbs(rootPos);

    // The incoming vector of the state lies clusterHeight beneath the state.
    glTranslatef(0.0f, 0.0f, -2 * clusterHeight);
    glGetFloatv(GL_MODELVIEW_MATRIX, N);
    Point3D incomingVect = { N[12] - initVect.x,
                             N[14] - initVect.y,
                           - N[13] + initVect.z };
    root->setIncomingControl( incomingVect );
    glTranslatef(0.0f, 0.0f, 2 * clusterHeight);
  }
  else {
    // The outgoing vector of the state points out of the cluster, in the 
    // direction the state itself is positioned. Furhtermore, it points 
    // clusterHeight up.
    glRotatef(-root->getPosition(), 0.0f, 0.0f, 1.0f);

    glTranslatef(startCluster->getTopRadius() * 3, 0.0f, -clusterHeight);
    glGetFloatv(GL_MODELVIEW_MATRIX, N);
    Point3D outgoingVect = { N[12] - initVect.x,
                             N[14] - initVect.y,
                            -N[13] + initVect.z };
    root->setOutgoingControl(outgoingVect);
    glTranslatef(-startCluster->getTopRadius() * 2, 0.0f, clusterHeight);
    
    glGetFloatv(GL_MODELVIEW_MATRIX, M);
    Point3D rootPos = { M[12] - initVect.x, 
                        M[14] - initVect.y, 
                      - M[13] + initVect.z};                    
    root->setPositionAbs(rootPos);

    glTranslatef( startCluster->getTopRadius() * 2, 0.0f, clusterHeight);
    glGetFloatv(GL_MODELVIEW_MATRIX, N);
    Point3D incomingVect = { N[12] - initVect.x,
                             N[14] - initVect.y,
                            -N[13] + initVect.z };
    root->setIncomingControl(incomingVect);
    glTranslatef(-startCluster->getTopRadius() * 3, 0.0f, -clusterHeight);
    glRotatef(root->getPosition(), 0.0f, 0.0f, 1.0f);
  }

  for(int i = 0; i != root->getNumberOfOutTransitions(); ++i) {
    Transition* outTransition = root->getOutTransitioni(i);
    State* endState = outTransition->getEndState();

    if (endState->getVisitState() == DFS_WHITE &&
        !outTransition->isBackpointer()) {

      int desc_rot = rot + settings->getInt(BranchRotation);
      if (desc_rot < 0) {
        desc_rot += 360;
      }
      else if (desc_rot >= 360) {
        desc_rot -=360;
      }

      Cluster* endCluster = endState->getCluster();

      if (endState->getRank() != root->getRank()) {
        
        if (endCluster->getPosition() < -0.9f) {
          //endCluster is centered, only descen)d
          glTranslatef(0.0f, 0.0f, clusterHeight);
          computeStateAbsPos(endState, 
            (startCluster->getNumberOfDescendants()>1)?desc_rot: rot,
            initVect);
          glTranslatef(0.0f, 0.0f, -clusterHeight);
        }
        else {
          glRotatef(-endCluster->getPosition() - rot, 0.0f, 0.0f, 1.0f);
          glTranslatef(startCluster->getBaseRadius(), 0.0f, clusterHeight);
          glRotatef(settings->getInt(OuterBranchTilt), 0.0f, 1.0f, 0.0f);
          computeStateAbsPos(endState, desc_rot, initVect);
          glRotatef(-settings->getInt(OuterBranchTilt), 0.0f, 1.0f, 0.0f);
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
  State *s;
  float ns = settings->getFloat(NodeSize);
  for (int i = 0; i < root->getNumberOfStates(); ++i) {
    s = root->getState(i);
    if (s->getPosition() < -0.9f) {
      //drawSphereState();

      glPushMatrix();
        glScalef(ns,ns,ns);
        primitiveFactory->drawSimpleSphere();
      glPopMatrix();
    }
    else {
      glPushMatrix();
        glRotatef(-s->getPosition(),0.0f,0.0f,1.0f);
        glTranslatef(root->getTopRadius(),0.0f,0.0f);
        glScalef(ns,ns,ns);
        primitiveFactory->drawSimpleSphere();
      glPopMatrix();
    }
  }

  int desc_rot = rot + settings->getInt(BranchRotation);
  if (desc_rot < 0) desc_rot += 360;
  else if (desc_rot >= 360) desc_rot -= 360;
  Cluster *desc;
  for (int i=0; i<root->getNumberOfDescendants(); ++i) {
    desc = root->getDescendant(i);
    if (desc->getPosition() < -0.9f) {
      // descendant is centered
      glTranslatef(0.0f,0.0f,clusterHeight);
      drawStates(desc,(root->getNumberOfDescendants()>1)?desc_rot:rot);
      glTranslatef(0.0f,0.0f,-clusterHeight);
    }
    else {
      glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      glTranslatef(root->getBaseRadius(),0.0f,clusterHeight);
      glRotatef(settings->getInt(OuterBranchTilt), 0.0f, 1.0f, 0.0f);
      drawStates(desc,desc_rot);
      glRotatef(-settings->getInt(OuterBranchTilt), 0.0f, 1.0f, 0.0f);
      glTranslatef(-root->getBaseRadius(),0.0f,-clusterHeight);
      glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
    }
  }
}

bool Visualizer::isMarked(State* s) {
  return ((markStyle == MARK_STATES && s->isMarked()) || 
          (markStyle == MARK_DEADLOCKS && s->isDeadlock())/* ||
          (markStyle == MARK_TRANSITIONS && s->hasMarkedTransition())*/);
}

void Visualizer::drawStatesMark(Cluster* root,int rot) {
  State *s;
  for (int i=0; i < root->getNumberOfStates(); ++i) {
    s = root->getState(i);
    if (isMarked(s)) {
      RGB_Color mc = settings->getRGB(MarkedColor);
      glColor4ub(mc.r,mc.g,mc.b,255);
    }
    else {
      glColor4ub(255,255,255,255);
    }
    float ns = settings->getFloat(NodeSize);
    if (s->getPosition() < -0.9f) {
      //drawSphereState();
      glPushMatrix();
        glScalef(ns,ns,ns);
        primitiveFactory->drawSimpleSphere();
      glPopMatrix();
    }
    else {
      glPushMatrix();
        glRotatef(-s->getPosition(),0.0f,0.0f,1.0f);
        glTranslatef(root->getTopRadius(),0.0f,0.0f);
        glScalef(ns,ns,ns);
        primitiveFactory->drawSimpleSphere();
      glPopMatrix();
    }
  }

  int desc_rot = rot + settings->getInt(BranchRotation);
  if (desc_rot < 0) desc_rot += 360;
  else if (desc_rot >= 360) desc_rot -= 360;
  Cluster *desc;
  for (int i=0; i<root->getNumberOfDescendants(); ++i) {
    desc = root->getDescendant(i);
    if (desc->getPosition() < -0.9f) {
      // descendant is centered
      glTranslatef(0.0f,0.0f,clusterHeight);
      drawStatesMark(desc,(root->getNumberOfDescendants()>1)?desc_rot:rot);
      glTranslatef(0.0f,0.0f,-clusterHeight);
    }
    else {
      glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      glTranslatef(root->getBaseRadius(),0.0f,clusterHeight);
      glRotatef(settings->getInt(OuterBranchTilt), 0.0f, 1.0f, 0.0f);
      drawStatesMark(desc,desc_rot);
      glRotatef(-settings->getInt(OuterBranchTilt), 0.0f, 1.0f, 0.0f);
      glTranslatef(-root->getBaseRadius(),0.0f,-clusterHeight);
      glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
    }
  }
}

// ------------- TRANSITIONS ---------------------------------------------------

void Visualizer::drawTransitions(bool draw_fp,bool draw_bp) {
  if (lts == NULL) return;
  if (!draw_fp && !draw_bp) return;

  clearDFSStates(lts->getInitialState());
  Point3D init = {0, 0, 0};
  glPushMatrix();
    glLoadIdentity();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    computeStateAbsPos(lts->getInitialState(), 0, init);
  glPopMatrix();
  clearDFSStates(lts->getInitialState());
        
  drawTransitions(lts->getInitialState(),draw_fp,draw_bp);
}

void Visualizer::drawTransitions(State* root,bool disp_fp,bool disp_bp) {
  root->DFSvisit();
  RGB_Color up_col = settings->getRGB(UpEdgeColor);
  RGB_Color dn_col = settings->getRGB(DownEdgeColor);
  for(int i = 0; i != root->getNumberOfOutTransitions(); ++i) {
    Transition* outTransition = root->getOutTransitioni(i);

    State* endState = outTransition->getEndState();

    // Draw transition from root to endState
    if (outTransition->isBackpointer() && disp_bp) {
      
      glColor4ub(up_col.r, up_col.g, up_col.b, 255);
      drawBackPointer(root, endState);
    }
    else if (!outTransition->isBackpointer() && disp_fp) {
      glColor4ub(dn_col.r, dn_col.g, dn_col.b, 255);
      drawForwardPointer(root, endState);
    }
    
    // If we haven't visited endState before, do so now.
    if (endState->getVisitState() == DFS_WHITE && 
        !outTransition->isBackpointer()) {

      // Move to the next state
      drawTransitions(endState,disp_fp,disp_bp);
    }
  }

  // Finalize this node
  root->DFSfinish();
}

void Visualizer::drawForwardPointer(State* startState, State* endState) {
  Point3D startPoint = startState->getPositionAbs();
  Point3D endPoint = endState->getPositionAbs();

  glBegin(GL_LINES);
    glVertex3f(startPoint.x, startPoint.y, startPoint.z);
    glVertex3f(endPoint.x, endPoint.y, endPoint.z);
  glEnd();
}

void Visualizer::drawBackPointer(State* startState, State* endState) {
  Point3D startPoint = startState->getPositionAbs();
  Point3D startControl = startState->getOutgoingControl();
  Point3D endControl = endState->getIncomingControl();
  Point3D endPoint = endState->getPositionAbs();


  if ( (startState->getPosition() < -0.9f) && (endState->getPosition() < -0.9f))
  {
    startControl.x = startPoint.x * 1.25;
    endControl.x = startControl.x;
  }

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

      glVertex3f(x, y, z);
    }
  /*
    glVertex3f(startPoint.x, startPoint.y, startPoint.z);
    glVertex3f(startControl.x, startControl.y, startControl.z);
    glVertex3f(endControl.x, endControl.y, endControl.z);
    glVertex3f(endPoint.x, endPoint.y, endPoint.z);
  */
  glEnd();
}
