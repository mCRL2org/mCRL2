// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualizer.cpp
/// \brief Implements the visualizer

#include "wx.hpp" // precompiled headers

#include "visualizer.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "cluster.h"
#include "lts.h"
#include "mediator.h"
#include "primitivefactory.h"
#include "settings.h"
#include "state.h"
#include "transition.h"
#include "visobjectfactory.h"

extern "C" {
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
#else
# if defined(_WIN32_) || defined(_MSC_VER)
#  include <windows.h>
#  undef __in_range // For STLport
# endif
# include <GL/gl.h>
# include <GL/glu.h>
#endif
}

using namespace std;
using namespace Utils;

#define SELECT_BLEND 0.3f

Visualizer::Visualizer(Mediator* owner,Settings* ss) {
  lts = NULL;
  mediator = owner;
  settings = ss;
  settings->subscribe(BranchRotation,this);
  settings->subscribe(BranchTilt,this);
  settings->subscribe(ClusterHeight,this);
  settings->subscribe(InterpolateColor1,this);
  settings->subscribe(InterpolateColor2,this);
  settings->subscribe(LongInterpolation,this);
  settings->subscribe(MarkedColor,this);
  settings->subscribe(Quality,this);

  visObjectFactory = new VisObjectFactory();
  primitiveFactory = new PrimitiveFactory(settings);
  sin_obt = float(sin(deg_to_rad(settings->getInt(BranchTilt))));
  cos_obt = float(cos(deg_to_rad(settings->getInt(BranchTilt))));

  visStyle = CONES;
  update_matrices = false;
  update_abs = true;
  update_colors = false;
  create_objects = false;
}

Visualizer::~Visualizer() {
  delete visObjectFactory;
  delete primitiveFactory;
}

float Visualizer::getHalfStructureHeight() const {
  if (lts == NULL) {
    return 0.0f;
  }
  return settings->getFloat(ClusterHeight)*(lts->getNumRanks()-1) / 2.0f;
}

void Visualizer::setLTS(LTS* l,bool compute_ratio) {
  lts = l;
  if (compute_ratio) {
    float ratio = lts->getInitialState()->getCluster()->getBCRadius() /
      lts->getInitialState()->getCluster()->getBCHeight();
    settings->setFloat(ClusterHeight,
        max(4,round_to_int(40.0f * ratio)) / 10.0f);
  }
  update_abs = true;
  traverseTree(true);
}

Utils::VisStyle Visualizer::getVisStyle() const {
  return visStyle;
}

void Visualizer::notifyMarkStyleChanged() {
  update_colors = true;
}

void Visualizer::setVisStyle(Utils::VisStyle vs) {
  if (visStyle != vs) {
    visStyle = vs;
    traverseTree(true);
  }
}

void Visualizer::notify(SettingID s) {
  switch (s) {
    case BranchTilt:
      sin_obt = float(sin(deg_to_rad(settings->getInt(BranchTilt))));
      cos_obt = float(cos(deg_to_rad(settings->getInt(BranchTilt))));
      update_matrices = true;
      update_abs = true;
      break;
    case BranchRotation:
    case ClusterHeight:
    case Quality:
      update_matrices = true;
      update_abs = true;
      break;
    case InterpolateColor1:
    case InterpolateColor2:
    case LongInterpolation:
      if (mediator->getMarkStyle() == NO_MARKS) {
        update_colors = true;
      }
      break;
    case MarkedColor:
      if (mediator->getMarkStyle() != NO_MARKS) {
        update_colors = true;
      }
      break;
    case Selection:
      update_colors = true;
      break;
    default:
      break;
  }
}

void Visualizer::computeBoundsInfo(float &bcw,float &bch) {
  bcw = 0.0f;
  bch = 0.0f;
  if (lts != NULL) {
    computeSubtreeBounds(lts->getInitialState()->getCluster(),
                          bcw, bch);
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
    for (i = 0; i < root->getNumDescendants(); ++i) {
      desc = root->getDescendant(i);
      if (desc != NULL)
      {
        if (desc->isCentered()) {
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
    }
    bw = max(bw,root->getTopRadius());
    bh += settings->getFloat(ClusterHeight);
  }
}

// ------------- STRUCTURE -----------------------------------------------------

void Visualizer::drawStructure() {
  if (lts == NULL) {
    return;
  }
  if (update_matrices) {
    traverseTree(false);
    update_matrices = false;
  }
  if (update_colors) {
    updateColors();
    update_colors = false;
  }
  visObjectFactory->drawObjects(primitiveFactory,settings->getUByte(Alpha),
                                mediator->getMatchStyle() == MATCH_MULTI);
}

void Visualizer::traverseTree(bool co) {
  if (lts == NULL) {
    return;
  }
  create_objects = co;
  if (co) {
    visObjectFactory->clear();
    update_colors = true;
  }
  glPushMatrix();
  glLoadIdentity();
  switch (visStyle) {
    case CONES:
      traverseTreeC(lts->getInitialState()->getCluster(), true, 0);
      break;
    case TUBES:
      traverseTreeT(lts->getInitialState()->getCluster(), true, 0);
      break;
    default:
      break;
  }
  glPopMatrix();
}

void Visualizer::traverseTreeC(Cluster *root,bool topClosed,int rot) {
  if (!root->hasDescendants())
  {
    float r = root->getTopRadius();
    glPushMatrix();
    glScalef(r,r,r);
    vector<int> ids;
    ids.push_back(root->getRank());
    ids.push_back(root->getPositionInRank());
    if (create_objects)
    {
      root->setVisObject(visObjectFactory->makeObject(
            primitiveFactory->makeSphere(),ids));
    }
    else
    {
      visObjectFactory->updateObjectMatrix(root->getVisObject());
    }
    glPopMatrix();
  }
  else
  {
    int drot = rot + settings->getInt(BranchRotation);
    if (drot >= 360)
    {
      drot -= 360;
    }
    if (create_objects)
    {
      root->clearBranchVisObjects();
    }

    glTranslatef(0.0f,0.0f,settings->getFloat(ClusterHeight));
    for (int i = 0; i < root->getNumDescendants(); ++i)
    {
      Cluster* desc = root->getDescendant(i);
      if (desc != NULL)
      {
        if (desc->isCentered())
        {
          if (root->getNumDescendants() > 1)
          {
            traverseTreeC(desc,false,drot);
          }
          else
          {
            traverseTreeC(desc,false,rot);
          }
        }
        else
        { 
          float delta = distance_circle_to_poly(
              deg_to_rad(desc->getPosition()+rot),
              root->getBaseRadius());
          glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
          glTranslatef(root->getBaseRadius() - delta,0.0f,0.0f);
          glRotatef(settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
          traverseTreeC(desc,true,drot);
          glRotatef(-settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
          glTranslatef(-root->getBaseRadius() + delta,0.0f,0.0f);
          glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
        }
      }
    }
    glTranslatef(0.0f,0.0f,-settings->getFloat(ClusterHeight));

    float r = root->getBaseRadius() / root->getTopRadius();
    glPushMatrix();
    glTranslatef(0.0f,0.0f,0.5f*settings->getFloat(ClusterHeight));
    if (r > 1.0f) {
      r = 1.0f / r;
      glRotatef(180.0f,1.0f,0.0f,0.0f);
      glScalef(root->getBaseRadius(),root->getBaseRadius(),
            settings->getFloat(ClusterHeight));

      vector<int> ids;
      ids.push_back(root->getRank());
      ids.push_back(root->getPositionInRank());

      if (create_objects)
      {
        root->setVisObject(visObjectFactory->makeObject(
              primitiveFactory->makeTruncatedCone(r,topClosed,
                root->getNumDescendants() > 1 || root->hasSeveredDescendants()),
              ids));
      }
      else
      {
        visObjectFactory->updateObjectMatrix(root->getVisObject());
      }
      glPopName();
      glPopName();
    }
    else
    {
      glScalef(root->getTopRadius(),root->getTopRadius(),
          settings->getFloat(ClusterHeight));

      vector<int> ids;
      ids.push_back(root->getRank());
      ids.push_back(root->getPositionInRank());

      if (create_objects)
      {
        root->setVisObject(visObjectFactory->makeObject(
              primitiveFactory->makeTruncatedCone(r,
                root->getNumDescendants() > 1 || root->hasSeveredDescendants(),
                topClosed), ids));
      }
      else
      {
        visObjectFactory->updateObjectMatrix(root->getVisObject());
      }
      glPopName();
      glPopName();
    }
    glPopMatrix();
  }
}

void Visualizer::traverseTreeT(Cluster *root, bool topClosed, int rot) {
  if (!root->hasDescendants()) {
    // root has no descendants; so draw it as a hemispheroid
    glPushMatrix();
    glScalef(root->getTopRadius(),root->getTopRadius(),
        min(root->getTopRadius(),settings->getFloat(ClusterHeight)));
    if (create_objects) {
      vector<int> ids;
      ids.push_back(root->getRank());
      ids.push_back(root->getPositionInRank());
      if (root == lts->getInitialState()->getCluster()) {
        // exception: draw root as a sphere if it is the initial cluster
        root->setVisObject(visObjectFactory->makeObject(
              primitiveFactory->makeSphere(), ids));
      } else {
        root->setVisObject(visObjectFactory->makeObject(
              primitiveFactory->makeHemisphere(),ids));
      }
    } else {
      visObjectFactory->updateObjectMatrix(root->getVisObject());
    }
    glPopMatrix();
  } else {
    int drot = rot + settings->getInt(BranchRotation);
    if (drot >= 360) {
      drot -= 360;
    }
    float baserad = 0.0f;
    if (create_objects) {
      root->clearBranchVisObjects();
    }

    for (int i = 0; i < root->getNumDescendants(); ++i) {
      Cluster* desc = root->getDescendant(i);
      if (desc != NULL)
      {
        if (desc->isCentered()) {
          if (create_objects) {
            root->addBranchVisObject(-1);
          }
          baserad = desc->getTopRadius();
          glTranslatef(0.0f,0.0f,settings->getFloat(ClusterHeight));
          if (root->getNumDescendants() > 1) {
            traverseTreeT(desc,false,drot);
          } else {
            traverseTreeT(desc,false,rot);
          }
          glTranslatef(0.0f,0.0f,-settings->getFloat(ClusterHeight));
        } else {
          // make the connecting cone
          float d_rad = root->getBaseRadius() - root->getTopRadius();
          float sz = sqrt(settings->getFloat(ClusterHeight) *
              settings->getFloat(ClusterHeight) + d_rad * d_rad);
          float alpha, sign;
          if (d_rad < 0.0f) {
            sign = -1.0f;
            alpha = atan(settings->getFloat(ClusterHeight) / -d_rad);
          } else {
            sign = 1.0f;
            if (d_rad > 0.0f) {
              alpha = atan(settings->getFloat(ClusterHeight) / d_rad);
            } else {
              alpha = 0.5f * PI;
            }
          }
          glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
          glPushMatrix();
          glTranslatef(root->getTopRadius(),0.0f,0.0f);
          glRotatef(sign*(90.0f-rad_to_deg(alpha)),0.0f,1.0f,0.0f);
          glScalef(sz,sz,sz);
          if (create_objects) {
            vector<int> ids;
            ids.push_back(root->getRank());
            ids.push_back(root->getPositionInRank());
            root->addBranchVisObject(visObjectFactory->makeObject(
                  primitiveFactory->makeObliqueCone(alpha,
                    desc->getTopRadius()/sz,sign),ids));
          } else {
            visObjectFactory->updateObjectMatrix(root->getBranchVisObject(i));
          }
          glPopMatrix();

          // recurse into the subtree
          glTranslatef(root->getBaseRadius(),0.0f,
              settings->getFloat(ClusterHeight));
          glRotatef(settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
          traverseTreeT(desc,false,drot);
          glRotatef(-settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
          glTranslatef(-root->getBaseRadius(),0.0f,
              -settings->getFloat(ClusterHeight));
          glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
        }
      }
    }

    if (baserad <= 0.0f && !root->hasSeveredDescendants()) {
      // root has no centered descendant, so draw it as a hemispheroid
      glPushMatrix();
      glScalef(root->getTopRadius(),root->getTopRadius(),
            min(root->getTopRadius(),settings->getFloat(ClusterHeight)));
      if (create_objects) {
        vector<int> ids;
        ids.push_back(root->getRank());
        ids.push_back(root->getPositionInRank());
        if (root == lts->getInitialState()->getCluster()) {
          // exception: draw root as a sphere if it is the initial cluster
          root->setVisObject(visObjectFactory->makeObject(
                primitiveFactory->makeSphere(), ids));
        } else {
          root->setVisObject(visObjectFactory->makeObject(
                primitiveFactory->makeHemisphere(), ids));
        }
      } else {
        visObjectFactory->updateObjectMatrix(root->getVisObject());
      }
      glPopMatrix();
    } else {
      // root has centered descendant; so draw it as a truncated cone
      float r = baserad / root->getTopRadius();
      if (root->hasSeveredDescendants()) {
        r = 1.0f;
      }
      glPushMatrix();
      glTranslatef(0.0f,0.0f,0.5f*settings->getFloat(ClusterHeight));
      if (r > 1.0f) {
        r = 1.0f / r;
        glRotatef(180.0f,1.0f,0.0f,0.0f);
        glScalef(baserad,baserad,settings->getFloat(ClusterHeight));
        if (create_objects) {
          vector<int> ids;
          ids.push_back(root->getRank());
          ids.push_back(root->getPositionInRank());
          root->setVisObject(visObjectFactory->makeObject(
                primitiveFactory->makeTruncatedCone(r,topClosed,
                  root->hasSeveredDescendants()), ids));
        } else {
          visObjectFactory->updateObjectMatrix(root->getVisObject());
        }
      } else {
        glScalef(root->getTopRadius(),root->getTopRadius(),
            settings->getFloat(ClusterHeight));
        if (create_objects) {
          vector<int> ids;
          ids.push_back(root->getRank());
          ids.push_back(root->getPositionInRank());
          root->setVisObject(visObjectFactory->makeObject(
                primitiveFactory->makeTruncatedCone(r,
                  root->hasSeveredDescendants(),topClosed),ids));
        } else {
          visObjectFactory->updateObjectMatrix(root->getVisObject());
        }
      }
      glPopMatrix();
    }
  }
}

float Visualizer::distance_circle_to_poly(float angle,float radius)
{
  float alpha = 2.0f * PI / settings->getInt(Quality);
  float beta = int(angle / alpha) * alpha - angle;
  return radius * abs(sin(alpha) * (cos(beta) - 1.0f) + sin(beta) *
      (cos(alpha) - 1.0f)) / sqrt(2.0f - 2.0f * cos(alpha)) ;
}

float Visualizer::compute_cone_scale_x(float phi,float r,float x) {
  float f = r/x * sin(phi);
  return r * cos(phi) / sqrt(1.0f - f*f);
}

void Visualizer::updateColors()
{
  Cluster *cl;
  RGB_Color c;
  if (mediator->getMarkStyle() == NO_MARKS)
  {
    Interpolater ipr(settings->getRGB(InterpolateColor1),
        settings->getRGB(InterpolateColor2),
        lts->getMaxRanks(),
        settings->getBool(LongInterpolation));
    for (Cluster_iterator ci = lts->getClusterIterator(); !ci.is_end();
        ++ci)
    {
      cl = *ci;
      c = ipr.getColor(cl->getRank());
      if (cl->isSelected())
      {
        c = blend_RGB(c, RGB_ORANGE, SELECT_BLEND);
      }
      // set color of cluster cl
      visObjectFactory->updateObjectColor(cl->getVisObject(),c);
      // and its branches
      for (int i = 0; i < cl->getNumBranchVisObjects(); ++i)
      {
        if (cl->getBranchVisObject(i) != -1)
        {
          visObjectFactory->updateObjectColor(
              cl->getBranchVisObject(i),c);
        }
      }
    }
  }
  else // mediator->getMarkStyle() != NO_MARKS
  {
    for (Cluster_iterator ci = lts->getClusterIterator(); !ci.is_end();
        ++ci)
    {
      cl = *ci;
      vector<RGB_Color> rule_colours;

      c = RGB_WHITE;
      if (mediator->isMarked(cl))
      {
        if (mediator->getMarkStyle() == MARK_STATES &&
            mediator->getMatchStyle() == MATCH_MULTI)
        {
          vector<int> cluster_rules;
          cl->getMatchedRules(cluster_rules);
          for(size_t i = 0; i <  cluster_rules.size(); ++i)
          {
            rule_colours.push_back(
                    mediator->getMarkRuleColor(cluster_rules[i]));
          }
        }
        else
        {
          c = settings->getRGB(MarkedColor);
        }
      }

      if (cl->isSelected())
      {
        c = blend_RGB(c, RGB_ORANGE, SELECT_BLEND);
      }

      visObjectFactory->updateObjectColor(cl->getVisObject(),c);
      visObjectFactory->updateObjectTexture(cl->getVisObject(),
          rule_colours);

      for (int i = 0; i < cl->getNumBranchVisObjects(); ++i)
      {
        if (cl->getBranchVisObject(i) != -1)
        {
          visObjectFactory->updateObjectColor(
              cl->getBranchVisObject(i),c);
          visObjectFactory->updateObjectTexture(
              cl->getBranchVisObject(i),rule_colours);
        }
      }
    }
  }
}

void Visualizer::sortClusters(Point3D viewpoint) {
  visObjectFactory->sortObjects(viewpoint);
}

// ------------- STATES --------------------------------------------------------

void Visualizer::drawStates(bool simulating) {
  if (lts == NULL) {
    return;
  }

  // Compute absolute positions of nodes, if necessary
  computeAbsPos();
  drawStates(lts->getInitialState()->getCluster(), simulating);
}

void Visualizer::drawSimStates(vector<State*> historicStates,
                               State* currState, Transition* chosenTrans)
{
  if (lts == NULL) {
    return;
  }

  // Compute absolute positions of nodes, if necessary
  computeAbsPos();

  float ns = settings->getFloat(StateSize);
  RGB_Color c;
  Point3D p;

  set<State*> drawnStates;

  // Draw the current state.
  //RGB_Color hisStateColor = settings->getRGB(SimStateColor);
  if (lts->getZoomLevel() == currState->getZoomLevel())
  {
    if(mediator->isMarked(currState))
    {
      c = settings->getRGB(MarkedColor);
    }
    else
    {
      c = settings->getRGB(SimCurrColor);
    }

    if (currState->isSelected())
    {
      c = blend_RGB(c,RGB_ORANGE,SELECT_BLEND);
    }

    glColor4ub(c.r, c.g, c.b, 255);
    Point3D p = currState->getPositionAbs();

    glPushName(STATE);
    glPushName(currState->getID());

    glPushMatrix();
    glTranslatef(p.x, p.y, p.z);

    // Make the current state a bit larger, to make it easier to find it in the
    // simulation
    glScalef(1.5 *ns, 1.5* ns, 1.5 * ns);
    primitiveFactory->drawSimpleSphere();
    glPopMatrix();

    glPopName();
    glPopName();
    drawnStates.insert(currState);
  }

  // Draw the future states
  State* endState;
  for (int i = 0; i < currState->getNumOutTransitions(); ++i)
  {
    endState = currState->getOutTransition(i)->getEndState();

    if (lts->getZoomLevel() == endState->getZoomLevel()
        && drawnStates.find(endState) == drawnStates.end())
    {
      glPushName(SIMSTATE);

      glPushName(endState->getID());

      if (mediator->isMarked(endState))
      {
        c = settings->getRGB(MarkedColor);
      }
      else if (currState->getOutTransition(i) == chosenTrans)
      {
        c = settings->getRGB(SimSelColor);
      }
      else
      {
        c = settings->getRGB(SimPosColor);
      }

      if (endState->isSelected())
      {
        c = blend_RGB(c, RGB_ORANGE, SELECT_BLEND);
      }

      glColor4ub(c.r, c.g, c.b, 255);
      p = endState->getPositionAbs();
      glPushMatrix();
      glTranslatef(p.x, p.y, p.z);
      if (currState->getOutTransition(i) == chosenTrans)
      {
        glScalef(1.5 * ns, 1.5 * ns, 1.5* ns);
      }
      else
      {
        glScalef(ns, ns, ns);
      }
      primitiveFactory->drawSimpleSphere();
      glPopMatrix();

      glPopName();
      glPopName();
      drawnStates.insert(endState);
    }
  }

  // Draw previous states of the simulation, in the colour specified in the
  // settings (default: white)
  State* s;
  for (size_t i = 0; i < historicStates.size() - 1; ++i) {
    s = historicStates[i];

    if(lts->getZoomLevel() == s->getZoomLevel()
        && drawnStates.find(s) == drawnStates.end())
    {
      if (mediator->isMarked(s))
      {
        c = settings->getRGB(MarkedColor);
      }
      else
      {
        c = settings->getRGB(SimPrevColor);
      }

      if (s->isSelected())
      {
        c = blend_RGB(c, RGB_ORANGE, SELECT_BLEND);
      }

      glColor4ub(c.r, c.g, c.b, 255);

      glPushName(STATE);
      glPushName(s->getID());

      p = s->getPositionAbs();
      glPushMatrix();
      glTranslatef(p.x, p.y, p.z);
      glScalef(ns, ns, ns);
      primitiveFactory->drawSimpleSphere();
      glPopMatrix();

      glPopName();
      glPopName();
      drawnStates.insert(s);
    }
  }
}

void Visualizer::computeAbsPos()
{
  if (update_abs) {
    glPushMatrix();
      glLoadIdentity();
      computeStateAbsPos(lts->getInitialState()->getCluster(),0);
    glPopMatrix();
    update_abs = false;
  }
}

void Visualizer::computeStateAbsPos(Cluster* root, int rot)
{
// Does a DFS on the clusters to calculate the `absolute' positions of their
// states, taking the position of the initial state as (0,0,0).
// Secondly, it also assigns the incoming and outgoing control points for the
// back pointers to each state.

  if (root->getState(0)->getZoomLevel() == lts->getZoomLevel())
  {
    // compute position of each state in this cluster
    for (int i = 0; i < root->getNumStates(); ++i) {
      State* s = root->getState(i);
      if (s->isCentered()) {
        float M[16];

        // store the position of this state
        glGetFloatv(GL_MODELVIEW_MATRIX, M);
        Point3D p1 = { M[12], M[13], M[14]};
        s->setPositionAbs(p1);

        // The outgoing vector of the state lies settings->getFloat(ClusterHeight) above the state.
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, -2 * settings->getFloat(ClusterHeight));
        glGetFloatv(GL_MODELVIEW_MATRIX, M);
        Point3D p2 = { M[12], M[13], M[14]};
        s->setIncomingControl(p2);

        // The incoming vector of the state lies settings->getFloat(ClusterHeight) beneath the state.
        glTranslatef(0.0f, 0.0f, 4 * settings->getFloat(ClusterHeight));
        glGetFloatv(GL_MODELVIEW_MATRIX, M);
        Point3D p3 = { M[12], M[13], M[14]};
        s->setOutgoingControl(p3);

        glPopMatrix();
      }
      else {
        float M[16];
        glPushMatrix();
        glRotatef(-s->getPositionAngle(), 0.0f, 0.0f, 1.0f);

        glTranslatef(s->getPositionRadius(), 0.0f, 0.0f);
        glGetFloatv(GL_MODELVIEW_MATRIX, M);
        Point3D p1 = { M[12], M[13], M[14]};
        s->setPositionAbs(p1);
        glTranslatef(-s->getPositionRadius(), 0.0f, 0.0f);

        // The outgoing vector of the state points out of the cluster, in the
        // direction the state itself is positioned. Furthermore, it points
        // settings->getFloat(ClusterHeight) up.
        glTranslatef(root->getTopRadius() * 3, 0.0f, -settings->getFloat(ClusterHeight));
        glGetFloatv(GL_MODELVIEW_MATRIX, M);
        Point3D p2 = { M[12], M[13], M[14]};
        s->setIncomingControl(p2);

        glTranslatef(0.0f, 0.0f, 2 * settings->getFloat(ClusterHeight));
        glGetFloatv(GL_MODELVIEW_MATRIX, M);
        Point3D p3 = { M[12], M[13], M[14]};
        s->setOutgoingControl(p3);
        glPopMatrix();
      }

      float M[16];
      glPushMatrix();
      glRotatef(-s->getPositionAngle(), 0.0f, 0.0f, 1.0f);
      glTranslatef(s->getPositionRadius(), 0.0f, 0.0f);

      glTranslatef(settings->getFloat(StateSize) * 5.0f,
                   settings->getFloat(StateSize) * 5.0f,
                   0.0f);
      glGetFloatv(GL_MODELVIEW_MATRIX, M);
      Point3D p = { M[12], M[13], M[14]};
      s->setLoopControl1(p);

      glTranslatef(0.0f,
                   -settings->getFloat(StateSize) * 10.0f,
                   0.0f);
      glGetFloatv(GL_MODELVIEW_MATRIX, M);
      p.x = M[12];
      p.y = M[13];
      p.z = M[14];
      s->setLoopControl2(p);

      glPopMatrix();
    }

    // recurse into the descendants
    int drot = rot + settings->getInt(BranchRotation);
    if (drot >= 360) {
      drot -= 360;
    }
    glTranslatef(0.0f,0.0f,settings->getFloat(ClusterHeight));

    for (int i = 0; i < root->getNumDescendants(); ++i) {
      Cluster* desc = root->getDescendant(i);
      if (desc != NULL)
      {
        if (desc->isCentered()) {
          computeStateAbsPos(desc,(root->getNumDescendants()>1)?drot:rot);
        }
        else {
          glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
          glTranslatef(root->getBaseRadius(),0.0f,0.0f);
          glRotatef(settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
          computeStateAbsPos(desc,drot);
          glRotatef(-settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
          glTranslatef(-root->getBaseRadius(),0.0f,0.0f);
          glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
        }
      }
    }

    glTranslatef(0.0f,0.0f,-settings->getFloat(ClusterHeight));
  }
}

void Visualizer::drawStates(Cluster* root, bool simulating) {
  if (lts->getZoomLevel() == root->getState(0)->getZoomLevel())
  {
    float ns = settings->getFloat(StateSize);
    for (int i = 0; i < root->getNumStates(); ++i) {
      State *s = root->getState(i);

      if(!(simulating && s->isSimulated()))
      {
        RGB_Color c;
        GLuint texName;

        if (!mediator->isMarked(s))
        {
          c = settings->getRGB(StateColor);
        }
        else
        {
          if (mediator->getMatchStyle() != MATCH_MULTI)
          {
            c = settings->getRGB(MarkedColor);
          }
          else
          {
            c = settings->getRGB(StateColor);
            vector< int > state_rules;
            s->getMatchedRules(state_rules);

            size_t n_colours = 1;
            size_t n_rules = state_rules.size();

            while (n_colours < n_rules)
            {
              n_colours = n_colours << 1;
            }


            GLubyte *texture = (GLubyte*)malloc(4 * n_colours *
                sizeof(GLubyte));

            for(unsigned int i = 0; i < n_colours; ++i)
            {
              int j = i % n_rules;

              RGB_Color c1 = mediator->getMarkRuleColor(state_rules[j]);
              texture[4*i]   = (GLubyte) c1.r;
              texture[4*i+1] = (GLubyte) c1.g;
              texture[4*i+2] = (GLubyte) c1.b;
              texture[4*i+3] = (GLubyte) 255;
            }
            // Bind the texture created above, set textures on. (From Red Book)

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glGenTextures(1, &texName);
            glBindTexture(GL_TEXTURE_1D, texName);

            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, n_colours, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, texture);

            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            glBindTexture(GL_TEXTURE_1D, texName);
            glEnable(GL_TEXTURE_1D);
            free(texture);
          }
        }

        if (s->isSelected()) {
          c = blend_RGB(c, RGB_ORANGE, SELECT_BLEND);
        }

        glColor4ub(c.r,c.g,c.b,255);
        Point3D p = s->getPositionAbs();

        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glScalef(ns,ns,ns);

        glPushName(s->getID());

        primitiveFactory->drawSimpleSphere();
        glPopName();
        glPopMatrix();

        if(mediator->isMarked(s))
        {
          glDisable(GL_TEXTURE_1D);

        }

        glDeleteTextures(1, &texName);
      }
    }
    Cluster *desc;
    for (int i = 0; i < root->getNumDescendants(); ++i) {
      desc = root->getDescendant(i);
      if (desc != NULL)
      {
        drawStates(desc, simulating);
      }
    }
  }
}

// ------------- FORCE DIRECTED ------------------------------------------------

void Visualizer::forceDirectedInit() {
  if (lts == NULL) return;
  resetVelocities(lts->getInitialState()->getCluster());
  // assign initial positions to the states
  // ensure that no two states have the same position
  forceDirectedInitPos(lts->getInitialState()->getCluster());
  update_abs = true;
  computeAbsPos();
}

void Visualizer::forceDirectedInitPos(Cluster* root) {
  root->positionStatesSpiral();
  for (int i = 0; i < root->getNumDescendants(); ++i) {
    if (root->getDescendant(i) != NULL) {
      forceDirectedInitPos(root->getDescendant(i));
    }
  }
}

void Visualizer::forceDirectedStep() {
  if (lts == NULL) return;
  // Step 1: set all state forces to 0
  resetForces(lts->getInitialState()->getCluster());
  // Step 2: compute resulting force on every state
  computeForces(lts->getInitialState()->getCluster());
  // Step 3: move every state according to computed force
  glPushMatrix();
  glLoadIdentity();
  applyForces(lts->getInitialState()->getCluster(),0);
  glPopMatrix();
}

void Visualizer::resetForces(Cluster* root) {
  for (int i = 0; i < root->getNumStates(); ++i) {
    root->getState(i)->resetForce();
  }
  // recurse to every descendant
  for (int i = 0; i < root->getNumDescendants(); ++i) {
    if (root->getDescendant(i) != NULL) {
      resetForces(root->getDescendant(i));
    }
  }
}

void Visualizer::resetVelocities(Cluster* root) {
  for (int i = 0; i < root->getNumStates(); ++i) {
    root->getState(i)->resetVelocity();
  }
  // recurse to every descendant
  for (int i = 0; i < root->getNumDescendants(); ++i) {
    if (root->getDescendant(i) != NULL) {
      resetVelocities(root->getDescendant(i));
    }
  }
}

void Visualizer::computeForces(Cluster* root) {
  for (int i = 0; i < root->getNumStates(); ++i) {
    State *s = root->getState(i);
    for (int j = 0; j < s->getNumOutTransitions(); ++j) {
      if (!s->getOutTransition(j)->isBackpointer()) {
        State *v = s->getOutTransition(j)->getEndState();
        // compute attracting force of s on v (and v.v.) using Hooke's law
        Point3D d = s->getPositionAbs() - v->getPositionAbs();
        float dl = length(d);
        if (dl > 0.0f) {
          Point3D force = (0.00001f * settings->getFloat(TransitionAttraction) *
            (dl - 0.4f*settings->getFloat(TransitionLength)) / dl) * d;
          force.x = truncate_float(force.x);
          force.y = truncate_float(force.y);
          force.z = truncate_float(force.z);
          v->addForce(force);
          s->addForce(-1.0f * force);

    /*cerr << "add force (" << force.x << "," << force.y << "," << force.z
      << ") to " << s->getID() << endl;
    cerr << "add force (" << -force.x << "," << -force.y << "," << -force.z
      << ") to " << v->getID() << endl;
    force = s->getForce();
    cerr << "force of " << s->getID() << " is now: (" << force.x << "," <<
      force.y << "," << force.z << ")" << endl;
    force = v->getForce();
    cerr << "force of " << v->getID() << " is now: (" << force.x << "," <<
      force.y << "," << force.z << ")" << endl;*/
        }
      }
    }
    for (int j = i+1; j < root->getNumStates(); ++j) {
      State *v = root->getState(j);
      // compute repulsing force of s on v (and v.v.) using Coulomb's law
      Point3D d = s->getPositionAbs() - v->getPositionAbs();
      float dl = length(d);
      if (dl > 0.0f) {
        Point3D force = (0.00001f * settings->getFloat(StateRepulsion) / (dl*dl*dl)) * d;
        force.x = truncate_float(force.x);
        force.y = truncate_float(force.y);
        force.z = truncate_float(force.z);
        s->addForce(force);
        v->addForce(-1.0f * force);
      }
    }
  }
  // recurse to every descendant
  for (int i = 0; i < root->getNumDescendants(); ++i) {
    if (root->getDescendant(i) != NULL) {
      computeForces(root->getDescendant(i));
    }
  }
}

void Visualizer::applyForces(Cluster* root,int rot) {
  if (root->getNumStates() > 1) {
    for (int i = 0; i < root->getNumStates(); ++i) {
      State *s = root->getState(i);
      if (s != lts->getInitialState()) {
        // "localize" the force on s: it has to become a vector in the current
        // coordinate system; we do this by multiplying it with the current GL
        // matrix
        float M[16];
        glGetFloatv(GL_MODELVIEW_MATRIX,M);
        Point3D force = s->getForce();
        // we're only interested in the (x,y) components, because the state has
        // to stay in the plane of its cluster
        Vect v = { truncate_float(M[0]*force.x + M[4]*force.y + M[8]*force.z),
                   truncate_float(M[1]*force.x + M[5]*force.y + M[9]*force.z) };
        // add force vector to s's velocity and get the new velocity vector
        // s will be translated over this vector
        v.x += s->getVelocity().x;
        v.y += s->getVelocity().y;
  //cerr << "add velocity (" << v.x << "," << v.y << ") to " << s->getID() << endl;
        v = 0.7f * v;
  /*cerr << "velocity of " << s->getID() << " is now: (" << v.x << "," <<  v.y << ")" << endl;
  cerr << "current location " << s->getID() << " (polar): (" <<
      s->getPositionAngle() << "," << s->getPositionRadius() << ")" << endl;
  cerr << "current location " << s->getID() << " (eucln): (" <<
      s->getPositionRadius()*cos(deg_to_rad(s->getPositionAngle())) << "," <<
      s->getPositionRadius()*sin(deg_to_rad(s->getPositionAngle())) << ")" << endl;*/

        Vect old_pos = {
          s->getPositionRadius()*cos(deg_to_rad(s->getPositionAngle())),
          s->getPositionRadius()*sin(deg_to_rad(s->getPositionAngle()))
        };
        // add the old position in local Euclidean coordinates if s is not centered
        if (!s->isCentered()) {
          v.x += old_pos.x;
          v.y += old_pos.y;
        }
        Vect new_pos;
        // normally, v would now be the new position of s in local Euclidean
        // coordinates, but we still have to make sure it is within the boundary
        // of the cluster
        if (!(v.x == 0.0f && v.y == 0.0f)) {
          float angle = truncate_float(vec_to_deg(v));
          // make sure we stay inside the boundary of the cluster
          float radius = truncate_float(min(vec_length(v),root->getTopRadius()));
          new_pos.x = radius * cos(deg_to_rad(angle));
          new_pos.y = radius * sin(deg_to_rad(angle));
  //cerr << "new location of " << s->getID() << " (polar): (" << angle << "," << radius << ")" << endl;
  //cerr << "new location of " << s->getID() << " (eucln): (" << new_pos.x << "," <<  new_pos.y << ")" << endl;
          // compute the new position of s in "world" coordinates (i.e. with the
          // initial state at (0,0,0))
          glPushMatrix();
            glRotatef(-angle,0.0f,0.0f,1.0f);
            glTranslatef(radius,0.0f,0.0f);
            glGetFloatv(GL_MODELVIEW_MATRIX,M);
            Point3D new_pos = { truncate_float(M[12]), truncate_float(M[13]),
              truncate_float(M[14]) };
          glPopMatrix();
          s->setPositionAngle(angle);
          s->setPositionRadius(radius);
          s->setPositionAbs(new_pos);
        } else {
          // s is centered
          s->center();
          new_pos.x = 0.0f;
          new_pos.y = 0.0f;
          glGetFloatv(GL_MODELVIEW_MATRIX,M);
          Point3D new_pos = { truncate_float(M[12]), truncate_float(M[13]),
            truncate_float(M[14]) };
          s->setPositionAbs(new_pos);
        }
        // state may have been moved over a vector different from the computed
        // velocity, so compute the actual velocity and store it
        v.x = truncate_float(new_pos.x - old_pos.x);
        v.y = truncate_float(new_pos.y - old_pos.y);
        s->setVelocity(v);
      }
    }
  }
  // recurse into the descendants
  int drot = rot + settings->getInt(BranchRotation);
  if (drot >= 360) {
    drot -= 360;
  }
  glTranslatef(0.0f,0.0f,settings->getFloat(ClusterHeight));

  for (int i = 0; i < root->getNumDescendants(); ++i) {
    Cluster* desc = root->getDescendant(i);
    if (desc != NULL)
    {
      if (desc->isCentered()) {
        applyForces(desc,(root->getNumDescendants()>1)?drot:rot);
      }
      else {
        glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
        glTranslatef(root->getBaseRadius(),0.0f,0.0f);
        glRotatef(settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
        applyForces(desc,drot);
        glRotatef(-settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
        glTranslatef(-root->getBaseRadius(),0.0f,0.0f);
        glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
      }
    }
  }

  glTranslatef(0.0f,0.0f,-settings->getFloat(ClusterHeight));
}

void Visualizer::resetStatePositions() {
  if (lts == NULL) return;
  lts->clearStatePositions();
  lts->positionStates();
  update_abs = true;
}

// ------------- TRANSITIONS ---------------------------------------------------

void Visualizer::drawTransitions(bool draw_fp,bool draw_bp) {
  if (lts == NULL) return;
  if (!draw_fp && !draw_bp) return;

  computeAbsPos();
  drawTransitions(lts->getInitialState()->getCluster(),draw_fp,draw_bp);
}

void Visualizer::drawTransitions(Cluster* root,bool disp_fp,bool disp_bp) {
  if (lts->getZoomLevel() == root->getState(0)->getZoomLevel())
  {
    for (int j = 0; j < root->getNumStates(); ++j) {
      State *s = root->getState(j);
      // draw the outgoing transitions of s
      for (int i = 0; i < s->getNumOutTransitions(); ++i) {
        Transition* outTransition = s->getOutTransition(i);
        State* endState = outTransition->getEndState();

        if (lts->getZoomLevel() == endState->getZoomLevel())
        {
          // Draw transition from root to endState
          if (disp_bp && outTransition->isBackpointer()) {
            if (mediator->isMarked(outTransition)) {
              RGB_Color c = settings->getRGB(MarkedColor);
              glColor4ub(c.r,c.g,c.b,255);
            } else {
              RGB_Color c = settings->getRGB(UpEdgeColor);
              glColor4ub(c.r,c.g,c.b,255);
            }
            drawBackPointer(s,endState);
          }
          if (disp_fp && !outTransition->isBackpointer()) {
            if (mediator->isMarked(outTransition)) {
              RGB_Color c = settings->getRGB(MarkedColor);
              glColor4ub(c.r,c.g,c.b,255);
            } else {
              RGB_Color c = settings->getRGB(DownEdgeColor);
              glColor4ub(c.r,c.g,c.b,255);
            }
            drawForwardPointer(s,endState);
          }
        }
      }
      // draw a loop if s has a loop
      if (disp_fp && s->getNumLoops() > 0) {
        bool hasMarkedLoop = false;
        for (int i = 0; i < s->getNumLoops() && !hasMarkedLoop; ++i) {
          if (mediator->isMarked(s->getLoop(i))) {
            hasMarkedLoop = true;
          }
        }
        if (hasMarkedLoop) {
          RGB_Color c = settings->getRGB(MarkedColor);
          glColor4ub(c.r,c.g,c.b,255);
        } else {
          RGB_Color c = settings->getRGB(DownEdgeColor);
          glColor4ub(c.r,c.g,c.b,255);
        }
        drawLoop(s);
      }
    }
    for (int i = 0; i < root->getNumDescendants(); ++i) {
      Cluster* desc = root->getDescendant(i);
      if (desc != NULL)
      {
        drawTransitions(desc,disp_fp,disp_bp);
      }
    }
  }
}

void Visualizer::drawSimTransitions(bool draw_fp, bool draw_bp,
                                 vector<Transition*> transHis,
                                 vector<Transition*> posTrans,
                                 Transition* chosenTrans)
{
  computeAbsPos();

  // Draw the historical transitions.
  for (size_t i = 0; i < transHis.size(); ++i) {
    Transition* currTrans = transHis[i];
    State* beginState = currTrans->getBeginState();
    State* endState = currTrans->getEndState();

    if (lts->getZoomLevel() == beginState->getZoomLevel() &&
        lts->getZoomLevel() == endState->getZoomLevel())
    {
      // Draw transition from beginState to endState
      if (currTrans->isBackpointer() && draw_bp) {
        if (mediator->isMarked(currTrans)) {
          RGB_Color c = settings->getRGB(MarkedColor);
          glColor4ub(c.r, c.g, c.b, 255);
        }
        else {
          RGB_Color transColor = settings->getRGB(SimPrevColor);
          glColor4ub(transColor.r, transColor.g, transColor.b, 255);
        }
        drawBackPointer(beginState, endState);
      }
      if (!currTrans->isBackpointer() && draw_fp) {
        if (mediator->isMarked(currTrans)) {
          RGB_Color c = settings->getRGB(MarkedColor);
          glColor4ub(c.r, c.g, c.b, 255);
        }
        else {
          RGB_Color transColor = settings->getRGB(SimPrevColor);
          glColor4ub(transColor.r, transColor.g, transColor.b, 255);
        }
        if (!currTrans->isSelfLoop())
        {
          drawForwardPointer(beginState, endState);
        }
        else
        {
          drawLoop(beginState);
        }
      }
    }
  }

  // Draw the possible transitions from the current state, as well as the state
  // they lead into
  for (size_t i = 0; i < posTrans.size(); ++i) {
    Transition* currTrans = posTrans[i];
    State* beginState = currTrans->getBeginState();
    State* endState = currTrans->getEndState();


    if (lts->getZoomLevel() == beginState->getZoomLevel() &&
        lts->getZoomLevel() == endState->getZoomLevel())
    {
      // Draw transition from beginState to endState
      if (currTrans->isBackpointer() && draw_bp) {
        if (currTrans == chosenTrans) {
          RGB_Color c = settings->getRGB(SimSelColor);
          glColor4ub(c.r, c.g, c.b, 255);
          glLineWidth(2.0);
        }
        else {
          RGB_Color c = settings->getRGB(SimPosColor);
          glColor4ub(c.r, c.g, c.b, 255);
        }
        if (mediator->isMarked(currTrans)) {
          RGB_Color c = settings->getRGB(MarkedColor);
          glColor4ub(c.r, c.g, c.b, 255);
        }
        drawBackPointer(beginState, endState);
        glLineWidth(1.0);
      }
      if (!currTrans->isBackpointer() && draw_fp) {
        if (currTrans == chosenTrans) {
          RGB_Color c  = settings->getRGB(SimSelColor);
          glColor4ub(c.r, c.g, c.b, 255);
          glLineWidth(2.0);
        }
        else {
          RGB_Color c = settings->getRGB(SimPosColor);
          glColor4ub(c.r, c.g, c.b, 255);
        }
        if (mediator->isMarked(currTrans)) {
          RGB_Color c = settings->getRGB(MarkedColor);
          glColor4ub(c.r, c.g, c.b, 255);
        }

        if (currTrans->isSelfLoop())
        {
          drawLoop(beginState);
        }
        else
        {
          drawForwardPointer(beginState, endState);
        }

        glLineWidth(1.0);
      }
    }
  }
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

  if (startState->isCentered() && endState->isCentered()) {
    startControl.x = startPoint.x * 1.25;
    endControl.x = startControl.x;
  }

  GLfloat ctrlPts [4][3] = { {startPoint.x, startPoint.y, startPoint.z},
                             {startControl.x, startControl.y, startControl.z},
                             {endControl.x, endControl.y, endControl.z},
                             {endPoint.x, endPoint.y, endPoint.z} };
  float t,it,b0,b1,b2,b3,x,y,z;
  int N = settings->getInt(Quality);
  glBegin(GL_LINE_STRIP);
    for (int k = 0; k < N; ++k) {
      t  = (float)k / (N-1);
      it = 1.0f - t;
      b0 =      t *  t *  t;
      b1 = 3 *  t *  t * it;
      b2 = 3 *  t * it * it;
      b3 =     it * it * it;

      x = b0 * ctrlPts[0][0] +
          b1 * ctrlPts[1][0] +
          b2 * ctrlPts[2][0] +
          b3 * ctrlPts[3][0];

      y = b0 * ctrlPts[0][1] +
          b1 * ctrlPts[1][1] +
          b2 * ctrlPts[2][1] +
          b3 * ctrlPts[3][1];

      z = b0 * ctrlPts[0][2] +
          b1 * ctrlPts[1][2] +
          b2 * ctrlPts[2][2] +
          b3 * ctrlPts[3][2];
      glVertex3f(x,y,z);
    }
  glEnd();
}

void Visualizer::drawLoop(State* state) {
  Point3D statePoint = state->getPositionAbs();
  Point3D startControl = state->getLoopControl1();
  Point3D endControl = state->getLoopControl2();

  float t,it,b0,b1,b2,b3,x,y,z, statesize;
  int N = settings->getInt(Quality);
  statesize = settings->getFloat(StateSize);
  glBegin(GL_LINE_STRIP);
    for (int k = 0; k < N; ++k) {
      t  = (float)k / (N-1);
      it = 1.0f - t;
      b0 =      t *  t *  t;
      b1 = 3 *  t *  t * it;
      b2 = 3 *  t * it * it;
      b3 =     it * it * it;

      x = b0 * statePoint.x  +
          b1 * startControl.x +
          b2 * endControl.x +
          b3 * statePoint.x;

      y = b0 * statePoint.y +
          b1 * startControl.y +
          b2 * endControl.y +
          b3 * statePoint.y;

      z = b0 * statePoint.z +
          b1 * startControl.z +
          b2 * endControl.z +
          b3 * statePoint.z;
      glVertex3f(x,y,z);
    }
  glEnd();
}

/*
bool Visualizer::isMarked(Transition* t) {
  return markStyle == MARK_TRANSITIONS && t->isMarked();
}*/

void Visualizer::exportToText(std::string filename)
{
  if (lts == NULL)
  {
    return ;
  }
  std::map< Cluster*, unsigned int > clus_id;
  unsigned int N = 0;
  float ch = settings->getFloat(ClusterHeight);
  Cluster_iterator ci(lts,false);
  std::ofstream file(filename.c_str());
  if (!file)
  {
    return ;
  }

  file << "Legend:\n";
  file << "  cone(x,y,z): a cone with top radius x, base radius y and height z\n";
  file << "  sphere(x):   a sphere with radius x\n";
  file << "  at angle(x): cluster is on the rim of the base of its parent cluster at angle x\n";
  file << "               If cluster is a cone, the center of its top side is placed there.\n";
  file << "               If cluster is a sphere, the center of the sphere is placed there.\n";
  file << "---------------------------------------------------------------------------------\n";

  for ( ; !ci.is_end(); ++ci)
  {
    file << "rank " << (*ci)->getRank() << ": cluster " << N << " ";
    if ((*ci)->hasDescendants())
    {
      file << "cone(" << (*ci)->getTopRadius() << "," << (*ci)->getBaseRadius()
      << "," << ch << ") ";
    }
    else
    {
      file << "sphere(" << (*ci)->getTopRadius() << ") ";
    }
    if ((*ci)->isCentered())
    {
      file << "centered";
    }
    else
    {
      file << "at angle(" << (*ci)->getPosition() << ")";
    }
    if ((*ci)->getAncestor() != NULL)
    {
      file << " below parent cluster " << clus_id[(*ci)->getAncestor()];
    }
    file << std::endl;
    clus_id[*ci] = N;
    ++N;
  }
}
