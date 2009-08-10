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
  showTransLabels = true;
  showStateLabels = true;
  showStateVector = false;
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
  depth = (width + height) / 2;
  pixelSize = _pixelSize;

  if (!fr)
  {
    fr = new mcrl2::utilities::wx::font_renderer();
  }

  drawStates(inSelectMode);
}

void Visualizer::drawStates(bool inSelectMode)
{

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  Graph* graph = owner->getGraph();


  if (graph)
  {
    for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
    {
      State* s = graph->getState(i);
	  drawState(s);

      for(size_t j = 0; j < s->getNumberOfTransitions(); ++j)
      {
        Transition* t = s->getTransition(j);
        drawTransition(t, j, inSelectMode);
      }

      for(size_t j = 0; j < s->getNumberOfSelfLoops(); ++j)
      {
        Transition* t = s->getSelfLoop(j);
        drawSelfLoop(t, j, inSelectMode);
      }
    }
  }
}

void Visualizer::setWidth(double _width)
{
  width = _width;
  depth = (width + height) / 2;
}

void Visualizer::setHeight(double _height)
{
  height = _height;
  depth = (width + height) / 2;
}


void Visualizer::drawState(State* s)
{
  quadratic=gluNewQuadric();							
  gluQuadricNormals(quadratic, GLU_SMOOTH);		

  double x = s->getX();
  double y = s->getY();
  double z = s->getZ();
  double rad =  radius * pixelSize;

  x = (x / 2000.0) * (width - rad * 2);
  y = (y / 2000.0) * (height - rad * 2);
  z = (z / 2000.0) * (depth - rad * 2);
  float t = 0.0;
  int prec = 50;  // TODO: Make parameterisable
  float step = 1.0 / prec;


  // Draw border of states
  if(s->isSelected())
  {
    glColor4ub(255, 0, 0,255);  // TODO: Param
  }
  else if(s->isInitialState())
  {
    glColor4ub(0, 255, 0,255);
  }
  else
  {
    glColor4ub(0, 0, 0,255);
  }

  glPushMatrix();
  glTranslatef(x, y, z);
  gluPartialDisk(quadratic,rad,rad*1.1,16,16,0,360);	// A Disk Like The One Before
  glRotatef(90.0f,0.0f,1.0f,0.0f);
  gluPartialDisk(quadratic,rad,rad*1.1,16,16,0,360);	// A Disk Like The One Before
  glRotatef(-90.0f,0.0f,1.0f,0.0f);
  glRotatef(90.0f,1.0f,0.0f,0.0f);
  gluPartialDisk(quadratic,rad,rad*1.1,16,16,0,360);	// A Disk Like The One Before
  glPopMatrix();
  
  if(s->isLocked())
  {
    // Grey out a locked state
    glColor4ub(125, 125, 125,0);
  }

  else
  {
    wxColour c = s->getColour();

    glColor4ub(c.Red(), c.Green(), c.Blue(),0);
  }


  glPushName(IDS::STATE);
  glPushName(s->getValue());

  glPushMatrix();
  glTranslatef(x, y, z);
  gluSphere(quadratic,rad,16,16);
  glPopMatrix();
  gluDeleteQuadric(quadratic);

  

  // Draw label

  std::stringstream labelstr;
  labelstr << s->getValue();

  if(showStateLabels) {
    fr->draw_text(labelstr.str(), x, y, z, (rad - 2 * pixelSize) / 24.0f,
                mcrl2::utilities::wx::al_center, mcrl2::utilities::wx::al_top);
  }

  glPopName();
  glPopName();

  // Draw state vector
  bool showStateVector = s->getShowStateVector();
  if(showStateVector) {
    std::stringstream vectorstr;
   std::map<std::string, std::string>::iterator it;
   std::map<std::string, std::string> stateParameters = s->getParameters();

    for(it = stateParameters.begin(); it != stateParameters.end(); ++it) {
     vectorstr << it->first <<": " << it->second << "\n";
    } 

    fr->draw_wrapped_text(vectorstr.str(), z, x + rad * 2, width,  
          height - y, y, (rad - 2 * pixelSize) / 24.0f,
          mcrl2::utilities::wx::al_left, mcrl2::utilities::wx::al_bottom);

  }

}

void Visualizer::drawTransition(Transition* tr, size_t trid, bool selecting)
{
  State* from = tr->getFrom();
  State* to = tr->getTo();

  double rad = radius * pixelSize;

  double xFrom = from->getX();
  double yFrom = from->getY();
  double zFrom = from->getZ();
  double xTo = to->getX();
  double yTo = to->getY();
  double zTo = to->getZ();


  double seBx = xFrom - xTo;
  double seBy = yFrom - yTo;
  double seBz = zFrom - zTo;
  double normalise = 4.0;
  double xVirtual, yVirtual, zVirtual;
  double xControl, yControl, zControl;

  tr->getControl(xVirtual, yVirtual, zVirtual);


  // (xVirtual, yVirtual, zVirtual) is a "virtual" control point, which should lie on the
  // curve (For non-selfloops).
  // We need to calculate a spline control point such that the 2nd order
  // Bezier curve has its apex on (xControl, yControl, zVirtual) = V.
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
  // can be done per component (x, y, z).
  xControl = 2.0 * xVirtual - .5 * (xFrom + xTo);
  yControl = 2.0 * yVirtual - .5 * (yFrom + yTo);
  zControl = 2.0 * zVirtual - .5 * (zFrom + zTo);

  seBx /= normalise;
  seBy /= normalise;
  seBz /= normalise;

  xFrom = (xFrom / 2000.0) * (width - rad * 2);
  yFrom = (yFrom / 2000.0) * (height - rad * 2);
  zFrom = (zFrom / 2000.0) * (depth - rad * 2);

  xTo =   (xTo / 2000.0) * (width - rad * 2);
  yTo =   (yTo / 2000.0) * (height - rad * 2);
  zTo =   (zTo / 2000.0) * (depth - rad * 2);

  xControl = (xControl / 2000.0) * (width - rad * 2);
  yControl = (yControl / 2000.0) * (height - rad * 2);
  zControl = (zControl / 2000.0) * (depth - rad * 2);

  xVirtual = (xVirtual / 2000.0) * (width - rad * 2);
  yVirtual = (yVirtual / 2000.0) * (height - rad * 2);
  zVirtual = (zVirtual / 2000.0) * (depth - rad * 2);





  // Draw a Bezier curve through the control points
  GLdouble ctrlPts[3][3] = {{xFrom, yFrom, zFrom},
                            {xControl, yControl, zControl},
                            {xTo, yTo, zTo}};

  double t, it, b0, b1, b2, x, y, z;
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
      z = b0 * ctrlPts[0][2] +
          b1 * ctrlPts[1][2] +
          b2 * ctrlPts[2][2];

      glVertex3d(x, y, z);
    }
  glEnd();


  // Transitions are uniquely identified by their from state and the identifier
  // within this state

  if(showHandles)
  {
    glPushName(IDS::TRANSITION);
    glPushName(from->getValue());
    glPushName(trid);

	glColor3ub(255,255,255);
	glBegin(GL_QUADS);

      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual + 0.00725);
      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual + 0.00725);
      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual + 0.00725);
      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual + 0.00725);

      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual + 0.00725);
      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual + 0.00725);

      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual - 0.00725);
      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual + 0.00725);
      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual + 0.00725);

      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual + 0.00725);
      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual + 0.00725);

      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual - 0.00725);
      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual + 0.00725);
      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual + 0.00725);

      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual - 0.00725);
      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual - 0.00725);

    glEnd();

    if(tr->isSelected())
    {
      glColor3ub(255, 0, 0);
    }
    else
    {
      glColor3ub(0, 0, 0);
    }

    glBegin(GL_LINE_STRIP);

      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual + 0.00725);
      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual + 0.00725);
      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual + 0.00725);
      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual + 0.00725);

    glEnd();
    glBegin(GL_LINE_STRIP);

      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual + 0.00725);
      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual + 0.00725);

    glEnd();
    glBegin(GL_LINE_STRIP);

      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual - 0.00725);
      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual + 0.00725);
      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual + 0.00725);

    glEnd();
    glBegin(GL_LINE_STRIP);

      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual + 0.00725);
      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual + 0.00725);

    glEnd();
    glBegin(GL_LINE_STRIP);

      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual - 0.00725);
      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual + 0.00725);
      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual + 0.00725);

    glEnd();
    glBegin(GL_LINE_STRIP);

      glVertex3d(xVirtual - 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual + 0.00725, zVirtual - 0.00725);
      glVertex3d(xVirtual + 0.00725, yVirtual, zVirtual - 0.00725);
      glVertex3d(xVirtual, yVirtual - 0.00725, zVirtual - 0.00725);

    glEnd();

    glPopName();
    glPopName();
    glPopName();
  }



  if(showTransLabels) {
    // Draw label near the control point
    // point
    double labelX, labelY, labelZ;
    tr->getLabelPos(labelX, labelY, labelZ);
    labelX = (labelX / 2000.0) * (width - rad * 2);
    labelY = (labelY / 2000.0) * (height - rad * 2);
	labelZ = (labelZ / 2000.0) * (depth - rad * 2);

    if(tr->isSelected())
    {
      glColor4ub(255, 0, 0, 255);
    }
    else
    {
      glColor4ub(0, 0, 0, 255);
    }
					
	glPushMatrix();
	glTranslatef(labelX, labelY, labelZ);
	double tX, tY, tZ;
	owner->getCanvasRots(tX, tY, tZ);
	glRotatef(tZ,0.0f,0.0f,-1.0f);
	glRotatef(tY,-1.0f,0.0f,0.0f);
	glRotatef(tX,0.0f,-1.0f,0.0f);

    if(selecting) {
      glPushName(IDS::LABEL);
      glPushName(from->getValue());
      glPushName(trid);
      fr->draw_bounding_box(tr->getLabel(), 0, .025, 0,
				  8 * pixelSize / 20.0f,
                  mcrl2::utilities::wx::al_center, mcrl2::utilities::wx::al_top, false);
      glPopName();
      glPopName();
      glPopName();
    }
    else {
      fr->draw_text(tr->getLabel(), 0, 0.025, 0,
                8 * pixelSize / 20.0f,
                mcrl2::utilities::wx::al_center, mcrl2::utilities::wx::al_top);
    }
	glPopMatrix();
					

  }

  glColor3ub(0, 0, 0);

  double xDif = xTo - xControl;
  double yDif = yTo - yControl;
  double zDif = zTo - zControl;

  float tanXZ = atan2(xDif, zDif) * 180.0f / M_PI;
  float angYxz = atan2(yDif, sqrt(xDif * xDif + zDif * zDif)) * 180.0f / M_PI;

  glPushMatrix();
  glTranslatef(xTo, yTo, zTo);
  glRotatef(tanXZ, 0.0f, 1.0f, 0.0f);
  glRotatef(angYxz, -1.0f, 0.0f, 0.0f);
  glTranslatef(0.0f, 0.0f, -2 * rad);
  drawArrowHead(rad);
  glPopMatrix();
}

void Visualizer::drawArrowHead(double baseLength)
{
  quadratic = gluNewQuadric();							
  gluQuadricNormals(quadratic, GLU_SMOOTH);
  gluCylinder(quadratic,baseLength/5,0.0f,baseLength,64,64);
  gluDeleteQuadric(quadratic);
}

void Visualizer::drawSelfLoop(Transition* tr, size_t j, bool selecting)
{
  // We are drawing a self loop, so t.to == t.from
  State* s = tr->getFrom();

  double rad = radius * pixelSize;
  double delta = .25 * M_PI;

  double xState, yState, zState;
  double xVirtual, yVirtual, zVirtual;

  double alpha = tr->getControlAlpha();
  double beta = tr->getControlBeta();
  double gamma = tr->getControlGamma();
  double dist = tr->getControlDist();

  xState = s->getX();
  yState = s->getY();
  zState = s->getZ();

  xVirtual = xState + sin(alpha) * dist * 200.0f;
  yVirtual = yState + sin(beta) * dist * 200.0f;
  zVirtual = zState + sin(gamma) * dist * 200.0f;;


  // Calculate control points of the curve
  // TODO: Explain
  double epsilon = alpha + beta;
  double zeta = alpha - beta;

  double xFactor = 1;
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
    double yFactor = (8 * (yVirtual - yState)) / (3 * (sinGamma + sinDelta));
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
  GLdouble ctrlPts[3][3] = {{xState, yState},
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

      glVertex3d(x, y, zState);
    }
  glEnd();



  if(showHandles)
  {
    glPushName(IDS::SELF_LOOP);
    glPushName(s->getValue());
    glPushName(j);
    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
      glVertex3d(xVirtual , yVirtual - .015, zState);
      glVertex3d(xVirtual - .015, yVirtual, zState);
      glVertex3d(xVirtual, yVirtual + .015, zState);
      glVertex3d(xVirtual + .015, yVirtual, zState);
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
      glVertex3d(xVirtual , yVirtual - .015, zState);
      glVertex3d(xVirtual - .015, yVirtual, zState);
      glVertex3d(xVirtual, yVirtual + .015, zState);
      glVertex3d(xVirtual + .015, yVirtual, zState);
    glEnd();

    glPopName();
    glPopName();
    glPopName();
  }


  // Draw label.
  if(showTransLabels) {
    double labelX, labelY, labelZ;
    tr->getLabelPos(labelX, labelY, labelZ);
    labelX = (labelX / 2000.0) * (width - rad * 2);
    labelY = (labelY / 2000.0) * (height - rad * 2);
	labelZ = (labelZ / 2000.0) * (depth - rad * 2);

    if(selecting) {
      glPushName(IDS::SELF_LABEL);
      glPushName(s->getValue());
      glPushName(j);
      fr->draw_bounding_box(tr->getLabel(), labelX, labelY + .025, labelZ,
                  8 * pixelSize / 20.0f,
                  mcrl2::utilities::wx::al_center, mcrl2::utilities::wx::al_top, false);
      glPopName();
      glPopName();
      glPopName();
    }
    else {
      fr->draw_text(tr->getLabel(), labelX, labelY + .025, labelZ,
                8 * pixelSize / 20.0f,
                mcrl2::utilities::wx::al_center, mcrl2::utilities::wx::al_top);
    }
  }


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
    fr = new mcrl2::utilities::wx::font_renderer();
  }
}

void Visualizer::setRadius(int radius)
{
  this->radius = radius;
}

int Visualizer::getRadius() const 
{
  return radius;
}

void Visualizer::setCurves(bool value)
{
  showHandles = value;
}

void Visualizer::setTransLabels(bool value) 
{
  showTransLabels = value;
}

void Visualizer::setStateLabels(bool value) 
{
  showStateLabels = value;
}
