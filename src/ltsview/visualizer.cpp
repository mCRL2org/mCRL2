#include <math.h>
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

void Visualizer::sortClusters(Point3D viewpoint) {
  stable_sort(clusters.begin(),clusters.end(),Distance_greater(viewpoint));
}

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

// ------------- STATES --------------------------------------------------------

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

void Visualizer::computeStateAbsPos(State* root,int rot,Point3D initVect) {
// Does a DFS on the states to calculate their `absolute' position, taking the 
// position of the initial state as (0,0,0). 
// Pre: True (?)
// Post: root->getPosAbs() = absolute position of root, taking the position of
//                           the initial state as (0, 0, 0)
  root->DFSvisit();
  Cluster* startCluster = root->getCluster();

  float M[16];
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

  int rankDiff = startState->getRank() - endState->getRank();
  Point3D startPoint = startState->getPositionAbs();

  Point3D startControl;
  startControl.x = startPoint.x * rankDiff * clusterHeight / 8;
  startControl.y = startPoint.y * rankDiff * clusterHeight / 8;
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

// ------------- PRIMITIVES ----------------------------------------------------

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

/* 
--------------------------- OLD METHODS' GRAVEYARD :) --------------------------

void Visualizer::drawLTS(Point3D viewpoint) {
  if (lts == NULL) return;

  if (displayStates && refreshStates) {
    glDeleteLists(statesDisplayList,1);
    statesDisplayList = glGenLists(1);
    glNewList(statesDisplayList,GL_COMPILE);
      if (markStyle == NO_MARKS) {
        glColor4ub(visSettings.stateColor.r,visSettings.stateColor.g,
                  visSettings.stateColor.b,255);
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
    for (unsigned int i=0; i<primitiveObjs.size(); ++i) {
      delete primitiveObjs[i];
    }
    primitiveObjs.clear();
    
    glPushMatrix();
      glLoadIdentity();
        // coloring based on interpolation settings
        HSV_Color hsv;
        computeDeltaCol(hsv);
        drawSubtreeC(lts->getInitialState()->getCluster(),true,hsv,0);
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
  for (unsigned int i=0; i<primitiveObjs.size(); ++i) {
    Point3D d = {
      primitiveObjs[i]->matrix[12]-viewpoint.x,
      primitiveObjs[i]->matrix[13]-viewpoint.y,
      primitiveObjs[i]->matrix[14]-viewpoint.z
    };
    primitives[i]->distance = sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
  }

  // sort primitives descending based on distance to viewpoint
  sort(primitiveObjs.begin(),primitiveObjs.end(),Distance_descObj()); 
  
  // draw primitives in sorted order
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  for (unsigned int i=0; i<primitiveObjs.size(); ++i) {
    glMultMatrixf(primitiveObjs[i]->matrix);
    glColor4ub(primitiveObjs[i]->color.r,primitiveObjs[i]->color.g,
        primitiveObjs[i]->color.b,visSettings.alpha);
    glVertexPointer(3,GL_FLOAT,0,primitiveObjs[i]->vertices);
    glNormalPointer(GL_FLOAT,0,primitiveObjs[i]->normals);
    glDrawArrays(GL_QUAD_STRIP,0,primitiveObjs[i]->size);
  }
}
*/
/*
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
    glColor4ub(visSettings.interpolateColor2.r,visSettings.interpolateColor2.g,
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
      glColor4ub(visSettings.markedColor.r,visSettings.markedColor.g,
                visSettings.markedColor.b,visSettings.alpha);
    }
    else {
      glColor4ub(255,255,255,visSettings.alpha);
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

void Visualizer::drawSubtreeT(Cluster* root,HSV_Color col,int rot) {
  if (root == lts->getInitialState()->getCluster()) {
    RGB_Color col_rgb = HSV_to_RGB(col);
    glRotatef(180.0f,1.0f,0.0f,0.0f);
    GLfloat M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    GLuint displist = glGenLists(1);
    glNewList(displist,GL_COMPILE);
      glPushMatrix();
      glMultMatrixf(M);
      glColor4ub(col_rgb.r,col_rgb.g,col_rgb.b,visSettings.alpha);
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
      glColor4ub(col_rgb.r,col_rgb.g,col_rgb.b,visSettings.alpha);
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
      	drawSubtreeT(desc,desccol,
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
        drawSubtreeT(desc,desccol,desc_rot);
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

void Visualizer::drawSubtreeTMark(Cluster* root,int rot) {
  RGB_Color color = (isMarked(root)) ? visSettings.markedColor : RGB_WHITE;
  if (root == lts->getInitialState()->getCluster()) {
    glRotatef(180.0f,1.0f,0.0f,0.0f);
    GLfloat M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    GLuint displist = glGenLists(1);
    glNewList(displist,GL_COMPILE);
      glPushMatrix();
      glMultMatrixf(M);
      glColor4ub(color.r,color.g,color.b,visSettings.alpha);
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
      glColor4ub(color.r,color.g,color.b,visSettings.alpha);
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
      	drawSubtreeTMark(desc,(root->getNumberOfDescendants()>1)?desc_rot:rot);
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
        drawSubtreeTMark(desc,desc_rot);
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
*/
/*
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
*/
/*
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
        glColor4ub(
            static_cast<unsigned char>(it1*basecol.r+t1*topcol.r),
            static_cast<unsigned char>(it1*basecol.g+t1*topcol.g),
	          static_cast<unsigned char>(it1*basecol.b+t1*topcol.b),
            visSettings.alpha);
        glVertex3f(mesh_vs[i+1][j].x,mesh_vs[i+1][j].y,mesh_vs[i+1][j].z);
        
        glNormal3f(mesh_ns[i][j].x,mesh_ns[i][j].y,mesh_ns[i][j].z);
        glColor4ub(
            static_cast<unsigned char>(it*basecol.r+t*topcol.r),
            static_cast<unsigned char>(it*basecol.g+t*topcol.g),
		        static_cast<unsigned char>(it*basecol.b+t*topcol.b),
            visSettings.alpha);
        glVertex3f(mesh_vs[i][j].x,mesh_vs[i][j].y,mesh_vs[i][j].z);
      }
      glEnd();
    }
  }
  else {
    glColor4ub(basecol.r,basecol.g,basecol.b,visSettings.alpha);
    for (int i=0; i<M; ++i) {
      if (i == M/2) {
	      glColor4ub(topcol.r,topcol.g,topcol.b,visSettings.alpha);
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

RankStyle Visualizer::getRankStyle() const {
  return rankStyle;
}

void Visualizer::setRankStyle(RankStyle rs) {
  rankStyle = rs;
  fillClusters();
}
*/
