// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualizer.cpp
/// \brief Visualization functions.

#include "wx.hpp" // precompiled headers

#include "visualizer.h"
#include <sstream>
#include <cmath>
#include "ids.h"

Visualizer::Visualizer(LTSGraph* app)
{
  owner = app;
  width = 0;
  height = 0;
  pixelSize = 1;
  radius = 10;
  showHandles = false;
  fr = NULL;
}

Visualizer::~Visualizer()
{
}

void Visualizer::visualize(double _width, double _height, double _pixelSize, 
                           bool inSelectMode)
{
  width = _width;
  height = _height;
  pixelSize = _pixelSize;
 
  if (!fr)
  {
    fr = new mcrl2::utilities::font_renderer();
  } 

  drawStates(inSelectMode);
}

void Visualizer::drawStates(bool inSelectMode)
{

  glClear(GL_COLOR_BUFFER_BIT);
  
  Graph* graph = owner->getGraph();

  
  if (graph)
  {
    for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
    {
      State* s = graph->getState(i);

      for(size_t j = 0; j < s->getNumberOfTransitions(); ++j)
      {
        Transition* t = s->getTransition(j);
        drawTransition(t, j, inSelectMode);
      }

      for(size_t j = 0; j < s->getNumberOfSelfLoops(); ++j)
      {
        Transition* t = s->getSelfLoop(j);
        drawSelfLoop(t, j);
      }  
    }

    for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
    {
      State* s = graph->getState(i);

      
      //glPushMatrix();
      
      drawState(s);
      
      //glPopMatrix();
    }
  }
}

void Visualizer::setWidth(double _width)
{
  width = _width;
}

void Visualizer::setHeight(double _height)
{
 height = _height;
}


void Visualizer::drawState(State* s)
{
  double x = s->getX();
  double y = s->getY();
  double rad =  radius * pixelSize;

  x = (x / 2000.0) * (width - rad * 2);
  y = (y / 2000.0) * (height - rad * 2);
  float t = 0.0;
  int prec = 50;  // TODO: Make parameterisable
  float step = 1.0 / prec;

  
  if(s->isLocked())
  {
    // Grey out a locked state
    glColor3ub(170, 170, 170);
  }

  else
  {
    wxColour c = s->getColour();
    
    glColor3ub(c.Red(), c.Green(), c.Blue());  
  }

  
  glPushName(IDS::STATE);
  glPushName(s->getValue());


  glBegin(GL_TRIANGLE_FAN);
    glVertex2d(x, y);
    while ( t <= 1.0 )
    { 
      double phi = 2 * M_PI * t;
      double nx = x + rad * cos(phi);
      double ny = y + rad * sin(phi);

      glVertex2d(nx, ny); 

      t += step;
    }
  glEnd();
  
  t = 0.0f;
  // Draw border of states
  if(s->isSelected())
  {
    glColor3ub(255, 0, 0);  // TODO: Param
  }
  else if(s->isInitialState())
  {
    glColor3ub(0, 255, 0);
  }
  else
  {
    glColor3ub(0, 0, 0);
  }

  glBegin(GL_LINE_STRIP);
    while (t <= 1.0)
    {
      double phi = 2 * M_PI * t;
      double nx = x + rad * cos(phi);
      double ny = y + rad * sin(phi);

      glVertex2d(nx, ny);

      t += step;
    }
  glEnd();


  // Draw label
  std::stringstream labelstr;
  labelstr << s->getValue();
  
  fr->draw_text(labelstr.str(), x, y, (rad - 2 * pixelSize) / 24.0f,  
                mcrl2::utilities::al_center, mcrl2::utilities::al_top);
  
  glPopName();
  glPopName();
}

void Visualizer::drawTransition(Transition* tr, size_t trid, bool selecting)
{
  State* from = tr->getFrom();
  State* to = tr->getTo();

  double rad = radius * pixelSize;

  double xFrom = from->getX();
  double yFrom = from->getY();
  double xTo = to->getX();
  double yTo = to->getY();


  double seBx = xFrom - xTo;
  double seBy = yFrom - yTo;
  double normalise = 4.0;
  double xVirtual, yVirtual;
  double xControl, yControl;

  tr->getControl(xVirtual, yVirtual);
  

  // (xVirtual, yVirtual) is a "virtual" control point, which should lie on the 
  // curve (For non-selfloops). 
  // We need to calculate a spline control point such that the 2nd order
  // Bezier curve has its apex on (xControl, yControl) = V.
  //
  // A (2nd order) Bezier spline is defined by: 
  // P(t) =  t    *    t  * S 
  //      + 2t    * (1-t) * C 
  //      + (1-t) * (1-t) * E
  // Where S is the start point of the curve, E the endpoint and C the actual
  // control point. 
  // For V, we would like P(0.5) = .25 * S + .5 * C + .25 * E
  // Now, since V, S and E are known, we can compute C as:
  // C = 2 * V - .5 (S + E)
  // As these operations are scalar multiplications and vector additions, these 
  // can be done per component (x, y).
  xControl = 2.0 * xVirtual - .5 * (xFrom + xTo);
  yControl = 2.0 * yVirtual - .5 * (yFrom + yTo);

  seBx /= normalise;
  seBy /= normalise;

  xFrom = (xFrom / 2000.0) * (width - rad * 2);
  yFrom = (yFrom / 2000.0) * (height - rad * 2);

  xTo =   (xTo / 2000.0) * (width - rad * 2);
  yTo =   (yTo / 2000.0) * (height - rad * 2);

  xControl = (xControl / 2000.0) * (width - rad * 2);
  yControl = (yControl / 2000.0) * (height - rad * 2);

  xVirtual = (xVirtual / 2000.0) * (width - rad * 2);
  yVirtual = (yVirtual / 2000.0) * (height - rad * 2);



  // Draw a Bezier curve through the control points
  GLdouble ctrlPts[3][2] = {{xFrom, yFrom},
                            {xControl, yControl},
                            {xTo, yTo}};
  
  double t, it, b0, b1, b2, x, y;
  int N = 50; // TODO: Parameterisable
  glColor3ub(0, 0, 0); // TODO: Parameterisable
  
  
  glBegin(GL_LINE_STRIP); 
    for(int k  = 0; k < N; ++k)
    {
      t = static_cast<float>(k) / (N - 1);
      it = 1.0f - t;
      b0 =      t *  t;
      b1 = 2 *  t * it;
      b2 =     it * it;

      x = b0 * ctrlPts[0][0] + 
          b1 * ctrlPts[1][0] +
          b2 * ctrlPts[2][0];
      y = b0 * ctrlPts[0][1] +
          b1 * ctrlPts[1][1] +
          b2 * ctrlPts[2][1];

      glVertex2d(x, y);
    }
  glEnd(); 
  
 
  // Transitions are uniquely identified by their from state and the identifier
  // within this state

  if(showHandles)
  {
    glPushName(IDS::TRANSITION);
    glPushName(from->getValue());
    glPushName(trid);

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
      glVertex2d(xVirtual , yVirtual - .015);
      glVertex2d(xVirtual - .015, yVirtual);
      glVertex2d(xVirtual, yVirtual + .015);
      glVertex2d(xVirtual + .015, yVirtual);
    glEnd();
     
    if(tr->isSelected())
    {
      glColor3ub(255, 0, 0);
    }
    else
    {
      glColor3ub(0, 0, 0);
    }
    
    glBegin(GL_LINE_LOOP);
      glVertex2d(xVirtual , yVirtual - .015);
      glVertex2d(xVirtual - .015, yVirtual);
      glVertex2d(xVirtual, yVirtual + .015);
      glVertex2d(xVirtual + .015, yVirtual); 
    glEnd();
   
    glPopName();
    glPopName();
    glPopName();  
  }

  // Draw label near the control point (for the moment, fixed above the control 
  // point
  double labelX, labelY;
  tr->getLabelPos(labelX, labelY);
  labelX = (labelX / 2000.0) * (width - rad * 2);
  labelY = (labelY / 2000.0) * (height - rad * 2);
  if(selecting) {
    glPushName(IDS::LABEL);
    glPushName(from->getValue());
    glPushName(trid);
    fr->draw_bounding_box(tr->getLabel(), labelX, labelY + .025,
                  8 * pixelSize / 20.0f, 
                  mcrl2::utilities::al_center, mcrl2::utilities::al_top, false);
    glPopName();
    glPopName();
    glPopName();
  }
  
  if(tr->isSelected())
  {
    glColor3ub(255, 0, 0);
  }
  else
  {
    glColor3ub(0, 0, 0);
  }
    
  fr->draw_text(tr->getLabel(), labelX, labelY + .025, 
                8 * pixelSize / 20.0f, 
                mcrl2::utilities::al_center, mcrl2::utilities::al_top);
  
  glColor3ub(0, 0, 0); 

  float ang = atan2(yControl - yTo, xControl - xTo) 
              * 180.0f / M_PI;
  
  glTranslatef(xTo, yTo, 0.0f);
  glRotatef(90 + ang, 0.0, 0.0, 1.0f);
  glTranslatef(0.0f, -rad * 2, 0.0f);
  drawArrowHead(rad);
  glTranslatef(0.0f,  rad * 2, 0.0f);
  glRotatef(-90 - ang, 0.0f, 0.0f, 1.0f);
  glTranslatef(-xTo, -yTo, 0.0f);
}

void Visualizer::drawArrowHead(double baseLength)
{
  glBegin(GL_TRIANGLES);
    glVertex2d(-baseLength / 2, 0);
    glVertex2d( baseLength / 2, 0);
    glVertex2d( 0,  baseLength);
  glEnd();

}

void Visualizer::drawSelfLoop(Transition* tr, size_t j)
{
  // We are drawing a self loop, so t.to == t.from
  State* s = tr->getFrom();

  double rad = radius * pixelSize;
  double beta = .25 * M_PI;
  
  double xState, yState;
  double xVirtual, yVirtual;
  
  double alpha = tr->getControlAlpha();
  double dist = tr->getControlDist();

  xState = s->getX();
  yState = s->getY();
  
  xVirtual = xState + cos(alpha) * dist * 200.0f;
  yVirtual = yState + sin(alpha) * dist * 200.0f;
  
 
  // Calculate control points of the curve
  // TODO: Explain
  double gamma = alpha + beta;
  double delta = alpha - beta;
  
  double xFactor;
  double yFactor;
  double cosGamma = cos(gamma);
  double cosDelta = cos(delta);
  double sinGamma = sin(gamma);
  double sinDelta = sin(delta);

  if(fabs(cosGamma + cosDelta) > 0.01)
  {
    xFactor = (8 *(xVirtual - xState)) / (3 * (cosGamma + cosDelta));
  }
  double xControl1 = xState + xFactor * cosGamma;
  double xControl2 = xState + xFactor * cosDelta;
  double yControl1;
  double yControl2;
  
  if(fabs(sinGamma + sinDelta) <= 0.01)
  {
    float additive = tan(beta) * (xControl1 - xState);
    yControl1 = yState + additive;
    yControl2 = yState - additive;
  }
  else
  {
    yFactor = (8 * (yVirtual - yState)) / (3 * (sinGamma + sinDelta));
    yControl1 = yState + yFactor * sinGamma;
    yControl2 = yState + yFactor * sinDelta;
    
    if(fabs(cosGamma + cosDelta) <= .01)
    {
      float additive = tan(beta) * (yControl1 - yState);
      xControl1 = xState - additive;
      xControl2 = xState + additive;
    }
  }
  

    
  // Normalize points for drawing on glContext
  xState = (xState / 2000.0f) * (width - rad * 2.0);
  yState = (yState / 2000.0f) * (height - rad * 2.0);

  xVirtual = (xVirtual / 2000.0f) * (width - rad * 2.0);
  yVirtual = (yVirtual / 2000.0f) * (height - rad * 2.0);
   
  xControl1 = (xControl1 / 2000.0f) * (width - rad * 2.0);
  yControl1 = (yControl1 / 2000.0f) * (height - rad * 2.0);

  xControl2 = (xControl2 / 2000.0f) * (width - rad * 2.0);
  yControl2 = (yControl2 / 2000.0f) * (height -rad * 2.0);


  // Draw cubic Bezier curve through the control points
  GLdouble ctrlPts[3][2] = {{xState, yState},
                            {xControl1, yControl1},
                            {xControl2, yControl2}};

  
  double t, it, b0, b1, b2, b3, x, y;
  int N = 50; // TODO: Parameterisable
  glColor3ub(0, 0, 0); // TODO: Parameterisable
  
  
  glBegin(GL_LINE_STRIP); 
    for(int k  = 0; k < N; ++k)
    {
      t = static_cast<float>(k) / (N - 1);
      it = 1.0f - t;

      b0 =      t *  t *  t;
      b1 = 3 *  t *  t * it;
      b2 = 3 *  t * it * it;
      b3 =     it * it * it;


      x = b0 * ctrlPts[0][0] + 
          b1 * ctrlPts[1][0] +
          b2 * ctrlPts[2][0] +
          b3 * ctrlPts[0][0];


      y = b0 * ctrlPts[0][1] +
          b1 * ctrlPts[1][1] +
          b2 * ctrlPts[2][1] +
          b3 * ctrlPts[0][1];

      glVertex2d(x, y);
    }
  glEnd(); 
  
  if(showHandles)
  {
    glPushName(IDS::SELF_LOOP);
    glPushName(s->getValue());
    glPushName(j);
    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
      glVertex2d(xVirtual , yVirtual - .015);
      glVertex2d(xVirtual - .015, yVirtual);
      glVertex2d(xVirtual, yVirtual + .015);
      glVertex2d(xVirtual + .015, yVirtual);
    glEnd();
    
    glColor3ub(0, 0, 0);
    glBegin(GL_LINE_LOOP);
      glVertex2d(xVirtual , yVirtual - .015);
      glVertex2d(xVirtual - .015, yVirtual);
      glVertex2d(xVirtual, yVirtual + .015);
      glVertex2d(xVirtual + .015, yVirtual);
    glEnd();

    glPopName();
    glPopName();
    glPopName();
  } 
  // Draw label.
  fr->draw_text(tr->getLabel(), xVirtual, yVirtual + .025, 
                8 * pixelSize / 20.0f, 
                mcrl2::utilities::al_center, mcrl2::utilities::al_top);
 

  glColor3ub(0, 0, 0); 

  float ang = atan2(yControl2 - yState, xControl2 - xState) 
              * 180.0f / M_PI;

  glTranslatef(xState, yState, 0.0f);
  glRotatef(90 + ang, 0.0, 0.0, 1.0f);
  glTranslatef(0.0f, -rad * 2, 0.0f);
  drawArrowHead(rad);
  glTranslatef(0.0f,  rad * 2, 0.0f);
  glRotatef(-90 - ang, 0.0f, 0.0f, 1.0f);
  glTranslatef(-xState, -yState, 0.0f);


}

void Visualizer::setPixelSize(double ps)
{
  pixelSize = ps;
}

void Visualizer::initFontRenderer()
{
  if (!fr)
  {
    fr = new mcrl2::utilities::font_renderer();
  }
}

void Visualizer::setRadius(int radius)
{
  this->radius = radius;
}

int Visualizer::getRadius() const {
  return radius;
}

void Visualizer::setCurves(bool value)
{
  showHandles = value;
}
