// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./timeseries.cpp

#include "timeseries.h"

#include <iostream>
using namespace std;


// -- static variables ----------------------------------------------


static const QColor colMrk(186, 227, 255);
static const int itvAnim = 350;


// -- constructors and destructor -----------------------------------


TimeSeries::TimeSeries(
  QWidget *parent,
  Mediator* m,
  Settings* s,
  Graph* g)
  : Visualizer(parent, g),
    Colleague(m),
    settings(s)
{
  critSect = false;

  ySpacePxl     = 6.0;
  minPixPerNode = 4.0;
  actPixPerNode = 4.0;
  wdwStartIdx   = 0;

  shiftStartIdx = -1;
  dragStartIdx  = -1;

  mouseOverIdx = -1;

  dragDir = DRAG_DIR_NULL;

  animIdxDgrm = -1;
  animFrame = itemsMarked.end();

  connect(&m_animationTimer, SIGNAL(timeout()), this, SLOT(animate()));

  connect(&settings->backgroundColor, SIGNAL(changed(QColor)), this, SLOT(update()));
  connect(&settings->textColor, SIGNAL(changed(QColor)), this, SLOT(update()));
  connect(&settings->textSize, SIGNAL(changed(int)), this, SLOT(update()));
}


TimeSeries::~TimeSeries()
{
  clearDiagram();
  clearAttributes();
}


// -- get functions -------------------------------------------------


void TimeSeries::getIdcsClstMarked(set< size_t > &idcs)
{
  idcs.clear();
  set< size_t >::iterator it;
  for (it = itemsMarked.begin(); it != itemsMarked.end(); ++it)
  {
    idcs.insert(graph->getNode(*it)->getCluster()->getIndex());
  }
}


void TimeSeries::getIdcsClstMarked(
  set< size_t > &idcs ,
  QColor& col)
{
  getIdcsClstMarked(idcs);
  col = colMrk;
}


void TimeSeries::getIdxMseOver(
  size_t& idxLeaf,
  set< size_t > &idcsBndl,
  QColor& colLeaf)
{
  idxLeaf = NON_EXISTING;
  idcsBndl.clear();
  if (mouseOverIdx != NON_EXISTING && mouseOverIdx < graph->getSizeNodes())
  {
    Node* node = graph->getNode(mouseOverIdx);
    idxLeaf = node->getCluster()->getIndex();
    for (size_t i = 0; i < node->getSizeInEdges(); ++i)
    {
      idcsBndl.insert(node->getInEdge(i)->getBundle()->getIndex());
    }
    node = 0;
  }
  colLeaf = VisUtils::coolBlue;
}


void TimeSeries::getCurrIdxDgrm(
  size_t& idxLeaf,
  set< size_t > &idcsBndl,
  QColor& colLeaf)
{
  idxLeaf = NON_EXISTING;
  idcsBndl.clear();
  if (currIdxDgrm != NON_EXISTING && currIdxDgrm < graph->getSizeNodes())
  {
    Node* node = graph->getNode(currIdxDgrm);
    idxLeaf = node->getCluster()->getIndex();
    for (size_t i = 0; i < node->getSizeInEdges(); ++i)
    {
      idcsBndl.insert(node->getInEdge(i)->getBundle()->getIndex());
    }
    node = 0;
  }
  colLeaf = VisUtils::coolBlue;
}


void TimeSeries::getAnimIdxDgrm(
  size_t& idxLeaf,
  set< size_t > &idcsBndl,
  QColor& colLeaf)
{
  if (animFrame != itemsMarked.end())
  {
    Node* nodeFr;
    Node* nodeTo;
    Edge* edgeIn;

    idxLeaf = NON_EXISTING;
    idcsBndl.clear();

    if (*animFrame != NON_EXISTING && *animFrame < graph->getSizeNodes())
    {
      nodeTo = graph->getNode(*animFrame);
      set< size_t >::iterator it = itemsMarked.begin();
      if (nodeTo->getIndex() == *it)
      {
        it = itemsMarked.end();
      }
      else
      {
        it = animFrame;
      }
      nodeFr = graph->getNode(*(--it));

      idxLeaf = nodeTo->getCluster()->getIndex();
      for (size_t i = 0; i < nodeTo->getSizeInEdges(); ++i)
      {
        edgeIn = nodeTo->getInEdge(i);
        if (edgeIn->getInNode() == nodeFr)
        {
          idcsBndl.insert(edgeIn->getBundle()->getIndex());
        }
      }
    }
    colLeaf = VisUtils::coolBlue;

    nodeFr = 0;
    nodeTo = 0;
    edgeIn = 0;
  }
}


void TimeSeries::getAttrIdcs(vector< size_t > &idcs)
{
  idcs.clear();
  for (size_t i = 0; i < attributes.size(); ++i)
  {
    idcs.push_back(attributes[i]->getIndex());
  }
}


// -- set functions -------------------------------------------------


void TimeSeries::setDiagram(Diagram* dgrm)
{
  clearDiagram();
  diagram = dgrm;

  dataChanged = true;
}


void TimeSeries::initAttributes(const vector< size_t > attrIdcs)
{
  // clear existing attributes
  clearAttributes();

  // init new attributes
  for (size_t i = 0; i < attrIdcs.size(); ++i)
  {
    attributes.push_back(graph->getAttribute(attrIdcs[i]));
  }

  dataChanged = true;
}


void TimeSeries::clearData()
{
  wdwStartIdx = 0;
}


void TimeSeries::markItems(Cluster* frame)
{
  if (frame->getSizeNodes() > 0)
  {
    // get index of animation frame
    size_t prevAnimIdx;
    if (animFrame != itemsMarked.end())
    {
      prevAnimIdx = *animFrame;
    }
    else
    {
      prevAnimIdx = NON_EXISTING;
    }

    // update marked items
    itemsMarked.clear();
    for (size_t i = 0; i < frame->getSizeNodes(); ++i)
    {
      itemsMarked.insert(frame->getNode(i)->getIndex());
    }

    // update animation frame
    animFrame = itemsMarked.find(prevAnimIdx);
    if (animFrame == itemsMarked.end())
    {
      animFrame = itemsMarked.begin();
    }
  }
}


void TimeSeries::markItems(QList<Cluster*> frames)
{
  for (int i = 0; i < frames.size(); i++)
  {
    markItems(frames[i]);
  }
}


// -- visualization functions  --------------------------------------


void TimeSeries::visualize(const bool& inSelectMode)
{
  if (critSect != true)
  {
    // have textures been generated
    if (texCharOK != true)
    {
      genCharTex();
    }

    // check if positions are ok
    if (geomChanged == true)
    {
      calcSettingsGeomBased();
    }
    // check if data are ok
    if (dataChanged == true)
    {
      calcSettingsDataBased();
    }

    clear();

    // visualize
    if (inSelectMode == true)
    {
      QSizeF size = worldSize();

      GLint hits = 0;
      GLuint selectBuf[512];
      startSelectMode(
        hits,
        selectBuf,
        0.0125,
        0.0125);

      glPushName(ID_CANVAS);
      VisUtils::fillRect(-0.5*size.width(), 0.5*size.width(), 0.5*size.height(), -0.5*size.height());

      if (!m_animationTimer.isActive())
      {
        drawMarkedItems(inSelectMode);
        drawSlider(inSelectMode);
      }
      if (dragStatus != DRAG_STATUS_ITMS)
      {
        drawDiagrams(inSelectMode);
      }

      glPopName();

      finishSelectMode(
        hits,
        selectBuf);
    }
    else
    {
      drawAxes(inSelectMode);
      drawMarkedItems(inSelectMode);
      drawSlider(inSelectMode);
      drawScale(inSelectMode);
      drawAttrVals(inSelectMode);
      //drawAxes( inSelectMode );
      drawLabels(inSelectMode);
      drawMouseOver(inSelectMode);
      drawDiagrams(inSelectMode);
    }
  }
}


// -- event handlers ------------------------------------------------


void TimeSeries::handleMouseEvent(QMouseEvent* e)
{
  Visualizer::handleMouseEvent(e);

  // redraw in select mode
  updateGL(true);
  // redraw in render mode
  updateGL();

  if (e->type() == QEvent::MouseMove)
  {
    m_lastMousePos = e->pos();
  }
}


void TimeSeries::handleWheelEvent(QWheelEvent *e)
{
  Visualizer::handleWheelEvent(e);

  if (!m_animationTimer.isActive())
  {
    mouseOverIdx = -1;

    // zoom out
    double pix  = pixelSize();
    double dist = posSliderBotRgt.x - posSliderTopLft.x;

    double diff = actPixPerNode;
    actPixPerNode = (dist/pix)/(double)(nodesWdwScale-nodesItvSlider);
    diff -= actPixPerNode;

    if (e->delta() > 0)
    {
      if (actPixPerNode > minPixPerNode)
      {
        actPixPerNode = minPixPerNode;
        if (diff > 0)
        {
          wdwStartIdx += (int)(0.5*(nodesItvSlider-1));
        }
      }
      else if (actPixPerNode < 0)
      {
        actPixPerNode = minPixPerNode;
      }
      else
      {
        wdwStartIdx += (int)(0.5*nodesItvSlider);
      }
    } else {
      if (actPixPerNode < itvSliderPerNode)
      {
        actPixPerNode = itvSliderPerNode;
        wdwStartIdx -= (int)(0.5*itvSliderPerNode);
      }
      else
      {
        // update position
        wdwStartIdx -= (int)(0.5*nodesItvSlider);
        if (wdwStartIdx + nodesWdwScale+nodesItvSlider > graph->getSizeNodes()-1)
        {
          wdwStartIdx = (graph->getSizeNodes()-1) - (nodesWdwScale+nodesItvSlider);
        }
        if (wdwStartIdx == NON_EXISTING)
        {
          wdwStartIdx = 0;
        }
      }
    }

    geomChanged = true;

    // redraw in render mode
    updateGL();
  }
}





void TimeSeries::handleMouseLeaveEvent()
{
  Visualizer::handleMouseLeaveEvent();

  // reset mouse roll-over
  mouseOverIdx = -1;
  mediator->handleMarkFrameClust(this);

  // redraw in render mode
  updateGL();
}


void TimeSeries::handleKeyEvent(QKeyEvent *e)
{
  Visualizer::handleKeyEvent(e);

  if (e->type() == QEvent::KeyPress)
  {
    if (e->key() == Qt::Key_Right)
    {
      // move to right
      if ((wdwStartIdx + 1 + nodesWdwScale) <= (graph->getSizeNodes()-1))
      {
        wdwStartIdx += 1;
      }
      else if ((wdwStartIdx + 1 + nodesWdwScale) > (graph->getSizeNodes()-1))
      {
        wdwStartIdx = (graph->getSizeNodes()-1) - nodesWdwScale;
      }
    }
    else if (e->key() == Qt::Key_Left)
    {
      // move to left
      if ((wdwStartIdx + 1) == NON_EXISTING)
      {
        wdwStartIdx = 0;
      }
      else if ((wdwStartIdx - 1) != NON_EXISTING)
      {
        wdwStartIdx -= 1;
      }
    }
    else if (e->key() == Qt::Key_Home)
    {
      // move to beginning
      wdwStartIdx = 0;
    }
    else if (e->key() == Qt::Key_End)
    {
      // move to end
      wdwStartIdx = (graph->getSizeNodes()-1) - nodesWdwScale;
    }
    else if (e->key() == Qt::Key_PageUp || (e->key() == Qt::Key_9 && (e->modifiers() & Qt::KeypadModifier)))
    {
      // move one window toward beginning
      if (wdwStartIdx < nodesWdwScale)
      {
        wdwStartIdx = 0;
      }
      else
      {
        wdwStartIdx -= nodesWdwScale;
      }

    }
    else if (e->key() == Qt::Key_PageDown || (e->key() == Qt::Key_3 && (e->modifiers() & Qt::KeypadModifier)))
    {
      // move one window toward end
      if ((wdwStartIdx + 2*nodesWdwScale) <= (graph->getSizeNodes()-1))
      {
        wdwStartIdx += nodesWdwScale;
      }
      else
      {
        wdwStartIdx = (graph->getSizeNodes()-1) - nodesWdwScale;
      }
    }
    else if (e->key() == Qt::Key_Escape)
    {
      if (m_animationTimer.isActive())
      {
        m_animationTimer.stop();
      }
      else
      {
        itemsMarked.clear();
      }
    }

    // redraw in render mode
    updateGL();
  }
  else
  {
    if (e->key() == Qt::Key_Shift)
    {
      shiftStartIdx = -1;
    }
  }
}


// -- utility functions -----------------------------------------


void TimeSeries::calcSettingsGeomBased()
{
  critSect = true;

  // update flag
  geomChanged = false;
  // calculate positions
  calcPositions();

  critSect = false;
}


void TimeSeries::calcSettingsDataBased()
{
  critSect = true;

  // update flag
  dataChanged = false;
  // calculate positions
  calcPositions();

  critSect = false;
}


void TimeSeries::calcPositions()
{
  double yItv = 0.0;

  double prevItvWdwPerNode = itvWdwPerNode;
  double prevScaleLft      = posScaleTopLft.x;

  // calc general info
  double pix = pixelSize();
  QSizeF size = worldSize();
  double xLft = -0.5*size.width();
  double xRgt =  0.5*size.width();
  double yTop =  0.5*size.height();
  double yBot = -0.5*size.height();

  // calc positions of slider at top
  posSliderTopLft.x = xLft + 5.0*pix;
  posSliderTopLft.y = yTop - 6.0*pix;
  posSliderBotRgt.x = xRgt - 5.0*pix;
  posSliderBotRgt.y = yTop - 6.0*pix - 6.0*ySpacePxl*pix;
  // calc intervals of slider
  if (graph->getSizeNodes() > 0)
  {
    nodesItvSlider = 1;
    double distPx = (posSliderBotRgt.x - posSliderTopLft.x)/pix;
    itvSlider = 0.0;

    while (itvSlider == 0.0)
    {
      double fact = (double)(graph->getSizeNodes())/(double)nodesItvSlider;
      double itvPx = distPx/fact;

      if (itvPx >= 5)
      {
        itvSlider = itvPx*pix;
      }
      else
      {
        nodesItvSlider *= 10;
      }
    }

    itvSliderPerNode = (posSliderBotRgt.x - posSliderTopLft.x)/(double)(graph->getSizeNodes());
  }
  else
  {
    itvSlider = size.width();
  }

  // calc size of visible window
  if (graph->getSizeNodes() > 0)
  {
    double distPx = (posSliderBotRgt.x - posSliderTopLft.x)/pix;

    nodesWdwScale = int (distPx/actPixPerNode);
    if (graph->getSizeNodes() < nodesWdwScale)
    {
      nodesWdwScale = graph->getSizeNodes();
    }

    itvWdwPerNode = (posSliderBotRgt.x - posSliderTopLft.x)/(double)nodesWdwScale;
  }
  else
  {
    nodesWdwScale = 0;
  }

  // calc intervals of scale
  if (graph->getSizeNodes() > 0)
  {
    nodesItvScale = 1;
    double distPx = (posScaleBotRgt.x - posScaleTopLft.x)/pix;

    double itvPx = (double)distPx/(double)(nodesWdwScale/nodesItvScale);
    while (itvPx <= 3)
    {
      nodesItvScale *= 10;
      itvPx = (double)distPx/(double)(nodesWdwScale/nodesItvScale);
    }
  }
  else
  {
    nodesItvScale = int (size.width());
  }

  // calc positions of scale at bottom
  posScaleTopLft.x = xLft + 5.0*pix;
  posScaleTopLft.y = yBot + 3.5*ySpacePxl*pix + 6.0*pix;
  posScaleBotRgt.x = xRgt - 5.0*pix;
  posScaleBotRgt.y = yBot + 6.0*pix;

  // calc positions of attribute grids
  posAxesTopLft.clear();
  posAxesBotRgt.clear();

  if (attributes.size() > 0)
  {
    double yDist = size.height()
                   - (6.0*ySpacePxl + 6.0)*pix // slider at top
                   - (3.5*ySpacePxl + 6.0)*pix // scale at bottom
                   - 2.0*pix;                  // spacing
    yItv = yDist/(double)attributes.size();
  }

  Position2D pos;
  for (size_t i = 0; i < attributes.size(); ++i)
  {
    pos.x = posScaleTopLft.x;
    pos.y = yTop
            - (6.0*ySpacePxl + 6.0)*pix // slider at top
            - 0.5*ySpacePxl*pix         // space between attrs
            - i*yItv                    // top of interval
            - 3.0*pix;                  // bit of a hack
    posAxesTopLft.push_back(pos);

    pos.x = posScaleBotRgt.x;
    pos.y = yTop
            - (6.0*ySpacePxl + 6.0)*pix // slider at top
            - (i+1)*yItv;               // bot of interval
    posAxesBotRgt.push_back(pos);
  }

  // calc positions of values
  posValues.clear();
  Attribute* attr;
  Node* node;
  for (size_t i = 0; i < attributes.size(); ++i)
  {
    attr = attributes[i];
    vector< Position2D > v;

    for (size_t j = 0; j< graph->getSizeNodes(); ++j)
    {
      node = graph->getNode(j);

      double alphaHgt;
      if (attr->getSizeCurValues() == 1)
      {
        alphaHgt = 1.0;
      }
      else
        alphaHgt = (double)attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex()
                   /
                   (double)(attr->getSizeCurValues()-1);
     pos.x = posAxesTopLft[i].x + j*itvWdwPerNode;
     pos.y = posAxesBotRgt[i].y + alphaHgt*(yItv - 0.5*ySpacePxl*pix - 3.0*pix);
     v.push_back(pos);
    }
  }

  // diagram scale factor to draw 120 x 120 pix diagram
  scaleDgrm = 120.0*(pix/2.0);

  // update positions of diagrams
  map< size_t, Position2D >::iterator it;
  for (it = showDgrm.begin(); it != showDgrm.end(); ++it)
  {
    double prevCorrIdx = (it->second.x-prevScaleLft)/prevItvWdwPerNode;
    it->second.x = posScaleTopLft.x + prevCorrIdx*itvWdwPerNode;
  }

  // clear memory
  attr = 0;
  node = 0;
}


void TimeSeries::clearDiagram()
{
  // association
  diagram = 0;
}


void TimeSeries::clearAttributes()
{
  // association
  attributes.clear();
}


// -- utility event handlers ----------------------------------------


void TimeSeries::animate()
{
  ++animFrame;
  if (animFrame == itemsMarked.end())
  {
    animFrame = itemsMarked.begin();
  }

  //mediator->handleAnimFrameBundl( this );
  mediator->handleAnimFrameClust(this);

  updateGL();
  repaint();
}


void TimeSeries::handleRwndDiagram(const int& dgrmIdx)
{
  animFrame = itemsMarked.begin();

  map< size_t, Position2D >::iterator it;
  it = showDgrm.find(dgrmIdx);

  size_t idx = *animFrame;
  Position2D pos = it->second;

  showDgrm.erase(it);
  showDgrm.insert(pair< size_t, Position2D >(idx, pos));

  animIdxDgrm = idx;
  currIdxDgrm = idx;

  mediator->handleAnimFrameClust(this);

  updateGL();
  update();
}


void TimeSeries::handlePrevDiagram(const int& /*dgrmIdx*/)
{
  if (m_animationTimer.isActive())
  {
    m_animationTimer.stop();
  }

  if (animFrame == itemsMarked.begin())
  {
    animFrame = --itemsMarked.end();
  }
  else
  {
    --animFrame;
  }

  if (*animFrame != animIdxDgrm)
  {
    map< size_t, Position2D >::iterator it;
    it = showDgrm.find(animIdxDgrm);
    if (it == showDgrm.end())
    {
      it = --showDgrm.end();
    }

    size_t idx = *animFrame;
    Position2D pos = it->second;

    showDgrm.erase(it);
    showDgrm.insert(pair< size_t, Position2D >(idx, pos));

    mediator->handleAnimFrameClust(this);

    animIdxDgrm = idx;
    currIdxDgrm = idx;
  }
}


void TimeSeries::handlePlayDiagram(const size_t& dgrmIdx)
{
  if (dgrmIdx == animIdxDgrm)
  {
    if (m_animationTimer.isActive())
    {
      m_animationTimer.stop();

      if (*animFrame != animIdxDgrm)
      {
        map< size_t, Position2D >::iterator it;
        it = showDgrm.find(animIdxDgrm);

        size_t idx = *animFrame;
        Position2D pos = it->second;

        showDgrm.erase(it);
        showDgrm.insert(pair< size_t, Position2D >(idx, pos));

        animIdxDgrm = idx;
        currIdxDgrm = idx;
      }
    }
    else
    {
      m_animationTimer.start(itvAnim);
    }
  }
  else
  {
    animIdxDgrm = dgrmIdx;
    animFrame = itemsMarked.begin();
    m_animationTimer.start(itvAnim);
  }
}


void TimeSeries::handleNextDiagram(const int& dgrmIdx)
{
  if (m_animationTimer.isActive())
  {
    m_animationTimer.stop();
  }

  if (animFrame == itemsMarked.end())
  {
    animFrame = itemsMarked.find(dgrmIdx);
  }

  ++animFrame;
  if (animFrame == itemsMarked.end())
  {
    animFrame = itemsMarked.begin();
  }

  if (*animFrame != animIdxDgrm)
  {
    map< size_t, Position2D >::iterator it;
    it = showDgrm.find(animIdxDgrm);
    if (it == showDgrm.end())
    {
      it = showDgrm.begin();
    }

    size_t idx = *animFrame;
    Position2D pos = it->second;

    showDgrm.erase(it);
    showDgrm.insert(pair< size_t, Position2D >(idx, pos));

    mediator->handleAnimFrameClust(this);

    animIdxDgrm = idx;
    currIdxDgrm = idx;
  }
}


// -- hit detection -------------------------------------------------


void TimeSeries::route()
{
  Cluster cluster;
  QList<Attribute*> attributes;

  cluster.addNode(graph->getNode(currIdxDgrm));
  for (size_t i = 0; i < graph->getSizeAttributes(); i++)
  {
    attributes += graph->getAttribute(i);
  }

  emit routingCluster(&cluster, QList<Cluster *>(), attributes);
}


void TimeSeries::handleHits(const vector< int > &ids)
{
  if (ids.size() > 1)
  {
    // mouse button down
    if (m_mouseDrag)
    {
      mouseOverIdx = -1;
      if (m_lastMouseEvent.buttons() == Qt::LeftButton)
      {
        if (dragStatus == DRAG_STATUS_SLDR)
        {
          handleDragSliderHdl();
        }
        else if (dragStatus == DRAG_STATUS_SLDR_LFT)
        {
          handleDragSliderHdlLft();
        }
        else if (dragStatus == DRAG_STATUS_SLDR_RGT)
        {
          handleDragSliderHdlRgt();
        }
        else if (dragStatus == DRAG_STATUS_ITMS)
        {
          if (ids.size() > 2)
          {
            handleDragItems(ids[2]);
          }
        }
        else if (dragStatus == DRAG_STATUS_DGRM)
        {
          if (ids.size() > 2)
          {
            handleDragDiagram(ids[2]);
          }
        }
      }
    }    

    if (m_lastMouseEvent.type() != QEvent::MouseMove) //Implies press, release or double click
    {
      dragDistNodes = 0.0;
      dragStatus = DRAG_STATUS_NONE;

      if (m_lastMouseEvent.type() == QEvent::MouseButtonPress && m_lastMouseEvent.button() == Qt::LeftButton &&
          ids[1] == ID_DIAGRAM && ids.size() > 2)
      {
        if (ids.size() == 4)
        {
          if (ids[3] == ID_DIAGRAM_CLSE)
          {
            handleShowDiagram(ids[2]);
          }
          else if (ids[3] == ID_DIAGRAM_MORE)
          {
            currIdxDgrm = ids[2];
            route();
          }
          else if (ids[3] == ID_DIAGRAM_RWND)
          {
            handleRwndDiagram(ids[2]);
          }
          else if (ids[3] == ID_DIAGRAM_PREV)
          {
            handlePrevDiagram(ids[2]);
          }
          else if (ids[3] == ID_DIAGRAM_PLAY)
          {
            handlePlayDiagram(ids[2]);
          }
          else if (ids[3] == ID_DIAGRAM_NEXT)
          {
            handleNextDiagram(ids[2]);
          }
        }
        else
        {
          dragStatus = DRAG_STATUS_DGRM;
        }

        mouseOverIdx = NON_EXISTING;
        currIdxDgrm = ids[2];

        if (currIdxDgrm != NON_EXISTING && !m_animationTimer.isActive())
        {
          Cluster* frame = new Cluster();
          vector< Attribute* > attrs;

          frame->addNode(graph->getNode(currIdxDgrm));

          for (size_t i = 0; i < graph->getSizeAttributes(); ++i)
          {
            //if ( graph->getAttribute( i )->getSizeCurValues() > 0 )
            attrs.push_back(graph->getAttribute(i));
          }

          mediator->handleShowFrame(frame, attrs, VisUtils::coolBlue);
          mediator->handleMarkFrameClust(this);

          delete frame;
          frame = 0;
        }
      }
      else if (m_lastMouseEvent.type() == QEvent::MouseButtonPress && m_lastMouseEvent.button() == Qt::LeftButton &&
               ids[1] == ID_SLIDER)
      {
        if (ids.size() == 3)
        {
          if (ids[2] == ID_SLIDER_HDL)
          {
            dragStatus = DRAG_STATUS_SLDR;
          }
          else if (ids[2] == ID_SLIDER_HDL_LFT)
          {
            dragStatus = DRAG_STATUS_SLDR_LFT;
          }
          else if (ids[2] == ID_SLIDER_HDL_RGT)
          {
            dragStatus = DRAG_STATUS_SLDR_RGT;
          }
        }
        else
        {
          dragStatus = DRAG_STATUS_SLDR;
          handleHitSlider();
        }
      }
      else if (m_lastMouseEvent.button() == Qt::LeftButton &&
               ids[1] == ID_ITEMS && ids.size() > 2 && attributes.size() > 0)
      {
        if (m_lastMouseEvent.type() == QEvent::MouseButtonPress)
        {
          handleHitItems(ids[2]);
          dragStatus = DRAG_STATUS_ITMS;
        }
        else if (m_lastMouseEvent.type() == QEvent::MouseButtonDblClick)
        {
          handleShowDiagram(ids[2]);
        }
        //mouseOverIdx = ids[2];
        //currIdxDgrm = -1;

        //mediator->handleMarkFrameClust( this );
        //mediator->handleUnshowFrame();
      }
      else if (m_lastMouseEvent.type() == QEvent::MouseButtonPress && m_lastMouseEvent.button() == Qt::RightButton &&
               ids[1] == ID_DIAGRAM)
      {
        route();
      }
      else
      {
        if (currIdxDgrm != NON_EXISTING)
        {
          currIdxDgrm = -1;
          mediator->handleUnshowFrame();
          mediator->handleMarkFrameClust(this);
        }

        if (mouseOverIdx != NON_EXISTING)
        {
          mouseOverIdx = NON_EXISTING;
        }

        mediator->handleMarkFrameClust(this);
        mediator->handleUnshowFrame();
      }
    }
  }
  else
  {
    currIdxDgrm  = -1;
    mouseOverIdx = -1;
  }
}


void TimeSeries::processHits(
  GLint hits,
  GLuint buffer[])
{
  GLuint* ptr;
  ptr = (GLuint*) buffer;

  if (hits > 0)
  {
    // if necassary, advance to closest hit
    if (hits > 1)
    {
      for (int i = 0; i < (hits-1); ++i)
      {
        int number = *ptr;
        ++ptr; // number;
        ++ptr; // z1
        ++ptr; // z2
        for (int j = 0; j < number; ++j)
        {
          ++ptr;  // names
        }
      }
    }

    // last hit
    int number = *ptr;
    ++ptr; // number
    ++ptr; // z1
    ++ptr; // z2

    vector< int > ids;
    for (int i = 0; i < number; ++i)
    {
      ids.push_back(*ptr);
      ++ptr;
    }

    handleHits(ids);
  }
  else
  {
    setToolTip(QString());
  }

  ptr = 0;
}


// -- utility drawing functions -------------------------------------


void TimeSeries::clear()
{
  VisUtils::clear(settings->backgroundColor.value());
}


void TimeSeries::drawSlider(const bool& inSelectMode)
{
  if (inSelectMode == true)
  {
    double pix = pixelSize();

    glPushName(ID_SLIDER);
    VisUtils::fillRect(
      posSliderTopLft.x, posSliderBotRgt.x,
      posSliderTopLft.y, posSliderBotRgt.y);

    glPushName(ID_SLIDER_HDL);
    VisUtils::fillRect(
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
      posSliderTopLft.y - 2.0*pix,
      posSliderTopLft.y - 2.0*ySpacePxl*pix + 2.0*pix);
    glPopName();

    glPushName(ID_SLIDER_HDL_LFT);
    VisUtils::fillTriangle(
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode - 5*pix,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix);
    glPopName();

    glPushName(ID_SLIDER_HDL_RGT);
    VisUtils::fillTriangle(
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode + 5*pix,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix);
    glPopName();

    glPopName();
  }
  else
  {
    double pix = pixelSize();

    // draw marked items on slider
    VisUtils::setColor(colMrk);
    if (itvSliderPerNode < pix)
    {
      set< size_t >::iterator it;
      for (it = itemsMarked.begin(); it != itemsMarked.end(); ++it)
      {
        VisUtils::drawLine(
          posSliderTopLft.x + (*it)*itvSliderPerNode,
          posSliderTopLft.x + (*it)*itvSliderPerNode,
          posSliderTopLft.y - 2.0*ySpacePxl*pix,
          posSliderTopLft.y - 4.0*ySpacePxl*pix);
      }
    }
    else
    {
      set< size_t >::iterator it;
      for (it = itemsMarked.begin(); it != itemsMarked.end(); ++it)
      {
        VisUtils::fillRect(
          posSliderTopLft.x + (*it)*itvSliderPerNode,
          posSliderTopLft.x + (*it + 1)*itvSliderPerNode,
          posSliderTopLft.y - 2.0*ySpacePxl*pix,
          posSliderTopLft.y - 4.0*ySpacePxl*pix);
      }
    }

    // draw positions of diagrams
    VisUtils::setColor(VisUtils::coolBlue);
    map< size_t, Position2D >::iterator it;
    for (it = showDgrm.begin(); it != showDgrm.end(); ++it)
    {
      VisUtils::drawLine(
        posSliderTopLft.x + it->first*itvSliderPerNode,
        posSliderTopLft.x + it->first*itvSliderPerNode,
        posSliderTopLft.y - 2.0*ySpacePxl*pix,
        posSliderTopLft.y - 4.0*ySpacePxl*pix);
    }

    // draw slider outlines
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
      posSliderTopLft.x, posSliderBotRgt.x,
      posSliderTopLft.y, posSliderTopLft.y);

    VisUtils::setColor(Qt::white);
    VisUtils::drawLine(
      posSliderTopLft.x,
      posSliderBotRgt.x,
      posSliderBotRgt.y - 0.5*ySpacePxl*pix + 1.0*pix,
      posSliderBotRgt.y - 0.5*ySpacePxl*pix + 1.0*pix);
    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::drawLine(
      posSliderTopLft.x,
      posSliderBotRgt.x,
      posSliderBotRgt.y - 0.5*ySpacePxl*pix,
      posSliderBotRgt.y - 0.5*ySpacePxl*pix);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::drawLine(
      posSliderTopLft.x,
      posSliderBotRgt.x,
      posSliderBotRgt.y - 0.5*ySpacePxl*pix - 1.0*pix,
      posSliderBotRgt.y - 0.5*ySpacePxl*pix - 1.0*pix);

    // draw slider scale
    double pos = posSliderTopLft.x;
    int ctr = 0;
    while (pos <= posSliderBotRgt.x)
    {
      if (ctr%5 == 0)
      {
        if (ctr%10 == 0)
        {
          VisUtils::setColor(VisUtils::mediumGray);
          VisUtils::drawLine(
            pos,
            pos,
            posSliderTopLft.y - 2.0*ySpacePxl*pix,
            posSliderTopLft.y - 4.0*ySpacePxl*pix);

          VisUtils::setColor(settings->textColor.value());
          VisUtils::drawLabelCenter(
            texCharId,
            pos,
            posSliderTopLft.y - 5.0*ySpacePxl*pix,
            settings->textSize.value()*pix/CHARHEIGHT,
            Utils::intToStr(ctr*nodesItvSlider));
        }
        else
        {
          VisUtils::setColor(VisUtils::mediumGray);
          VisUtils::drawLine(
            pos,
            pos,
            posSliderTopLft.y - 2.0*ySpacePxl*pix,
            posSliderTopLft.y - 4.0*ySpacePxl*pix + 4.0*pix);
        }
      }
      else
      {
        VisUtils::setColor(VisUtils::mediumGray);
        VisUtils::drawLine(
          pos,
          pos,
          posSliderTopLft.y - 2.0*ySpacePxl*pix,
          posSliderTopLft.y - 4.0*ySpacePxl*pix + 8.0*pix);
      }

      pos += itvSlider;
      ctr += 1;
    }

    // draw slider
    VisUtils::fillRect(
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
      posSliderTopLft.y - 2.0*pix,
      posSliderTopLft.y - 2.0*ySpacePxl*pix + 2.0*pix,
      VisUtils::lightCoolGreen, VisUtils::lightCoolGreen, VisUtils::coolGreen, VisUtils::coolGreen);

    // draw slider handles
    VisUtils::setColor(VisUtils::darkGray);
    VisUtils::fillTriangle(
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode - 5*pix,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix);
    VisUtils::fillTriangle(
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode + 5*pix,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix);

    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::enableLineAntiAlias();
    VisUtils::drawTriangle(
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode - 5*pix,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + wdwStartIdx*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix);
    VisUtils::drawTriangle(
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode + 5*pix,
      posSliderTopLft.y - 2.0*ySpacePxl*pix - 10*pix,
      posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode,
      posSliderTopLft.y - 2.0*ySpacePxl*pix);
    VisUtils::disableLineAntiAlias();
  }
}


void TimeSeries::drawScale(const bool& inSelectMode)
{
  if (inSelectMode == true)
    {}
  else
  {
    double pix = pixelSize();
    size_t    beg = 0;
    for (size_t i = 0; i < nodesWdwScale; ++i)
    {
      beg = wdwStartIdx+i;
      if (beg%nodesItvScale == 0)
      {
        break;
      }
    }

    for (size_t i = beg; i <= wdwStartIdx+nodesWdwScale; i += nodesItvScale)
    {
      if ((i/nodesItvScale)%5 == 0)
      {
        if ((i/nodesItvScale)%10 == 0)
        {
          VisUtils::setColor(VisUtils::mediumGray);
          VisUtils::drawLine(
            posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
            posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
            posScaleTopLft.y,
            posScaleTopLft.y - 2.0*ySpacePxl*pix);
          VisUtils::setColor(settings->textColor.value());
          VisUtils::drawLabelCenter(
            texCharId,
            posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
            posScaleTopLft.y - 3.0*ySpacePxl*pix,
            settings->textSize.value()*pix/CHARHEIGHT,
            Utils::size_tToStr(i));
        }
        else
        {
          VisUtils::setColor(VisUtils::mediumGray);
          VisUtils::drawLine(
            posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
            posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
            posScaleTopLft.y,
            posScaleTopLft.y - 2.0*ySpacePxl*pix + 4.0*pix);
        }
      }
      else
      {
        VisUtils::setColor(VisUtils::mediumGray);
        VisUtils::drawLine(
          posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
          posScaleTopLft.x + (i-wdwStartIdx)*itvWdwPerNode,
          posScaleTopLft.y,
          posScaleTopLft.y - 2.0*ySpacePxl*pix + 8.0*pix);
      }
    }
  }
}


void TimeSeries::drawMarkedItems(const bool& inSelectMode)
{
  if (inSelectMode == true)
  {
    double pix = pixelSize();

    glPushName(ID_ITEMS);
    for (size_t i = 0; i < nodesWdwScale; ++i)
    {
      glPushName((GLuint)(wdwStartIdx + i));
      VisUtils::fillRect(
        posScaleTopLft.x + i*itvWdwPerNode,
        posScaleTopLft.x + (i+1)*itvWdwPerNode,
        posSliderBotRgt.y - 0.5*ySpacePxl*pix,
        posScaleTopLft.y - 2.0*ySpacePxl*pix);
      glPopName();
    }
    glPopName();
  }
  else
  {
    double pix = pixelSize();

    // draw selected items
    VisUtils::setColor(colMrk);

    set< size_t >::iterator it;
    for (it = itemsMarked.begin(); it != itemsMarked.end(); ++it)
    {
      if (wdwStartIdx <= (*it) && (*it) <= (wdwStartIdx + nodesWdwScale))
        VisUtils::fillRect(
          posScaleTopLft.x - wdwStartIdx*itvWdwPerNode + (*it)*itvWdwPerNode,
          posScaleTopLft.x - wdwStartIdx*itvWdwPerNode + (*it + 1)*itvWdwPerNode,
          posSliderBotRgt.y - 0.5*ySpacePxl*pix,
          posScaleTopLft.y - 2.0*ySpacePxl*pix);
    }
  }
}


void TimeSeries::drawAxes(const bool& inSelectMode)
{
  if (inSelectMode == true)
    {}
  else
  {
    for (size_t i = 0; i < posAxesTopLft.size(); ++i)
    {
      VisUtils::fillRect(
        posAxesTopLft[i].x,
        posAxesBotRgt[i].x,
        posAxesTopLft[i].y,
        posAxesBotRgt[i].y,
        VisUtils::lightGray,
        VisUtils::lightGray,
        VisUtils::lightLightGray,
        VisUtils::lightLightGray);
    }
  }
}


void TimeSeries::drawAttrVals(const bool& inSelectMode)
{
  if (inSelectMode == true)
    {}
  else
  {
    // draw bars
    for (size_t i = 0; i < posValues.size(); ++i)
    {
      for (size_t j = 0; j < nodesWdwScale; ++j)
      {
        VisUtils::setColor(VisUtils::coolGreen);
        VisUtils::fillRect(
          posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode,
          posValues[i][wdwStartIdx+j].x - wdwStartIdx*itvWdwPerNode + itvWdwPerNode,
          posValues[i][wdwStartIdx+j].y,
          posAxesBotRgt[i].y);
      }
    }

    // draw line above or below bars
    VisUtils::enableLineAntiAlias();
    for (size_t i = 0; i < posValues.size(); ++i)
    {
      glBegin(GL_LINE_STRIP);
      for (size_t j = 0; j < nodesWdwScale; ++j)
      {
        VisUtils::setColor(VisUtils::mediumGray);
        glVertex2f(
          posValues[i][wdwStartIdx + j].x - wdwStartIdx*itvWdwPerNode,
          posValues[i][wdwStartIdx + j].y);
        glVertex2f(
          posValues[i][wdwStartIdx + j].x - wdwStartIdx*itvWdwPerNode + itvWdwPerNode,
          posValues[i][wdwStartIdx + j].y);
      }
      glEnd();
    }
    VisUtils::disableLineAntiAlias();
  }
}


void TimeSeries::drawDiagrams(const bool& inSelectMode)
{
  if (inSelectMode == true)
  {
    glPushName(ID_DIAGRAM);
    if (m_animationTimer.isActive() && animIdxDgrm != NON_EXISTING)
    {
      Position2D posDgrm;
      map< size_t, Position2D >::iterator it;
      vector< double >     vals;
      vector< Attribute* > attrs;

      it = showDgrm.find(animIdxDgrm);
      posDgrm = it->second;
      posDgrm.x -= wdwStartIdx*itvWdwPerNode;

      glPushMatrix();
      glTranslatef(posDgrm.x, posDgrm.y, 0.0);
      glScalef(scaleDgrm, scaleDgrm, scaleDgrm);

      glPushName((GLuint) it->first);
      VisUtils::fillRect(- 1.0, 1.0, 1.0, -1.0);

      glPushName(ID_DIAGRAM_RWND);
      VisUtils::fillRect(0.2, 0.36, -0.8, -0.98);
      glPopName();

      glPushName(ID_DIAGRAM_PREV);
      VisUtils::fillRect(0.4, 0.56, -0.8, -0.98);
      glPopName();

      glPushName(ID_DIAGRAM_PLAY);
      VisUtils::fillRect(0.6, 0.76, -0.8, -0.98);
      glPopName();

      glPushName(ID_DIAGRAM_NEXT);
      VisUtils::fillRect(0.8, 0.96, -0.8, -0.98);
      glPopName();

      glPopName();
      glPopMatrix();
    }
    else
    {
      Position2D posDgrm;
      vector< double >     vals;
      vector< Attribute* > attrs;

      map< size_t, Position2D >::iterator it;
      for (it = showDgrm.begin(); it != showDgrm.end(); ++it)
      {
        posDgrm = it->second;
        posDgrm.x -= wdwStartIdx*itvWdwPerNode;

        glPushMatrix();
        glTranslatef(posDgrm.x, posDgrm.y, 0.0);
        glScalef(scaleDgrm, scaleDgrm, scaleDgrm);

        glPushName((GLuint) it->first);
        VisUtils::fillRect(- 1.0, 1.0, 1.0, -1.0);

        glPushName(ID_DIAGRAM_CLSE);
        VisUtils::fillRect(0.8, 0.96, 0.96, 0.8);
        glPopName();

        glPushName(ID_DIAGRAM_MORE);
        VisUtils::fillRect(-0.98, -0.8, -0.8, -0.98);
        glPopName();

        if (it->first == currIdxDgrm && itemsMarked.size() > 1)
        {
          glPushName(ID_DIAGRAM_RWND);
          VisUtils::fillRect(0.2, 0.36, -0.8, -0.98);
          glPopName();

          glPushName(ID_DIAGRAM_PREV);
          VisUtils::fillRect(0.4, 0.56, -0.8, -0.98);
          glPopName();

          glPushName(ID_DIAGRAM_PLAY);
          VisUtils::fillRect(0.6, 0.76, -0.8, -0.98);
          glPopName();

          glPushName(ID_DIAGRAM_NEXT);
          VisUtils::fillRect(0.8, 0.96, -0.8, -0.98);
          glPopName();
        }

        glPopName();
        glPopMatrix();
      }
    }
    glPopName();
  }
  else
  {
    double     pix = pixelSize();

    if (m_animationTimer.isActive() && animIdxDgrm != NON_EXISTING)
    {
      Position2D posPvot, posDgrm;
      map< size_t, Position2D >::iterator it;
      vector< double >     vals;
      vector< Attribute* > attrs;

      it = showDgrm.find(animIdxDgrm);
      posDgrm = it->second;
      posDgrm.x -= wdwStartIdx*itvWdwPerNode;
      posPvot.x = posScaleTopLft.x + (*animFrame /*+ 0.5*/ - wdwStartIdx)*itvWdwPerNode;
      posPvot.y = posScaleTopLft.y /*+ 0.5*ySpacePxl*pix*/;

      double aglDeg = Utils::calcAngleDg(posDgrm.x-posPvot.x, posDgrm.y-posPvot.y);
      double dist   = Utils::dist(posPvot.x, posPvot.y, posDgrm.x, posDgrm.y);

      // draw vertical line
      VisUtils::setColor(VisUtils::coolBlue);
      VisUtils::drawLine(
        posPvot.x,
        posPvot.x,
        posSliderBotRgt.y - 0.5*ySpacePxl*pix,
        posScaleTopLft.y - 2.0*ySpacePxl*pix);

      // draw connector
      VisUtils::setColor(VisUtils::coolBlue);
      glPushMatrix();
      glTranslatef(posPvot.x, posPvot.y, 0.0);
      glRotatef(aglDeg-90.0, 0.0, 0.0, 1.0);
      VisUtils::enableLineAntiAlias();
      VisUtils::fillTriangle(0.0, 0.0, -pix, dist, pix, dist);
      VisUtils::drawTriangle(0.0, 0.0, -pix, dist, pix, dist);
      VisUtils::disableLineAntiAlias();
      glPopMatrix();

      glPushMatrix();
      glTranslatef(posDgrm.x, posDgrm.y, 0.0);
      glScalef(scaleDgrm, scaleDgrm, scaleDgrm);

      // drop shadow
      VisUtils::setColor(VisUtils::coolBlue);
      VisUtils::fillRect(
        -1.0 + 4.0*pix/scaleDgrm,
        1.0 + 4.0*pix/scaleDgrm,
        1.0 - 4.0*pix/scaleDgrm,
        -1.0 - 4.0*pix/scaleDgrm);
      // diagram
      for (size_t i = 0; i < graph->getSizeAttributes(); ++i)
      {
        Attribute* attr = graph->getAttribute(i);
        Node* node = graph->getNode(*animFrame);
        if (attr->getSizeCurValues() > 0)
        {
          attrs.push_back(attr);
          vals.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
        }
        else
        {
          double val = node->getTupleVal(attr->getIndex());
          attrs.push_back(attr);
          vals.push_back(val);
        }
        attr = 0;
        node = 0;
      }
      diagram->visualize(
        inSelectMode,
        pixelSize(),
        attrs,
        vals);
      attrs.clear();
      vals.clear();

      VisUtils::enableLineAntiAlias();

      VisUtils::setColor(VisUtils::coolBlue);
      VisUtils::fillRwndIcon(0.2, 0.36, -0.8, -0.98);
      VisUtils::setColor(VisUtils::lightLightGray);
      VisUtils::drawRwndIcon(0.2, 0.36, -0.8, -0.98);

      VisUtils::setColor(VisUtils::coolBlue);
      VisUtils::fillPrevIcon(0.4, 0.56, -0.8, -0.98);
      VisUtils::setColor(VisUtils::lightLightGray);
      VisUtils::drawPrevIcon(0.4, 0.56, -0.8, -0.98);

      VisUtils::setColor(VisUtils::coolBlue);
      VisUtils::fillPauseIcon(0.6, 0.76, -0.8, -0.98);
      VisUtils::setColor(VisUtils::lightLightGray);
      VisUtils::drawPauseIcon(0.6, 0.76, -0.8, -0.98);

      VisUtils::setColor(VisUtils::coolBlue);
      VisUtils::fillNextIcon(0.8, 0.96, -0.8, -0.98);
      VisUtils::setColor(VisUtils::lightLightGray);
      VisUtils::drawNextIcon(0.8, 0.96, -0.8, -0.98);

      VisUtils::disableLineAntiAlias();
      glPopMatrix();
    }
    else
    {
      Position2D posPvot, posDgrm;
      vector< double >     vals;
      vector< Attribute* > attrs;

      map< size_t, Position2D >::iterator it;
      for (it = showDgrm.begin(); it != showDgrm.end(); ++it)
      {
        posPvot.x = posScaleTopLft.x + (it->first /*+ 0.5*/ - wdwStartIdx)*itvWdwPerNode;
        posPvot.y = posScaleTopLft.y /*+ 0.5*ySpacePxl*pix*/;
        posDgrm = it->second;
        posDgrm.x -= wdwStartIdx*itvWdwPerNode;

        double aglDeg = Utils::calcAngleDg(posDgrm.x-posPvot.x, posDgrm.y-posPvot.y);
        double dist   = Utils::dist(posPvot.x, posPvot.y, posDgrm.x, posDgrm.y);

        // draw vertical line
        VisUtils::setColor(VisUtils::coolBlue);
        VisUtils::drawLine(
          posPvot.x,
          posPvot.x,
          posSliderBotRgt.y - 0.5*ySpacePxl*pix,
          posScaleTopLft.y - 2.0*ySpacePxl*pix);

        // draw connector
        VisUtils::setColor(VisUtils::coolBlue);
        glPushMatrix();
        glTranslatef(posPvot.x, posPvot.y, 0.0);
        glRotatef(aglDeg-90.0, 0.0, 0.0, 1.0);
        VisUtils::enableLineAntiAlias();
        VisUtils::fillTriangle(0.0, 0.0, -pix, dist, pix, dist);
        VisUtils::drawTriangle(0.0, 0.0, -pix, dist, pix, dist);
        VisUtils::disableLineAntiAlias();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(posDgrm.x, posDgrm.y, 0.0);
        glScalef(scaleDgrm, scaleDgrm, scaleDgrm);

        // drop shadow
        VisUtils::setColor(VisUtils::coolBlue);
        VisUtils::fillRect(
          -1.0 + 4.0*pix/scaleDgrm,
          1.0 + 4.0*pix/scaleDgrm,
          1.0 - 4.0*pix/scaleDgrm,
          -1.0 - 4.0*pix/scaleDgrm);
        // diagram
        Attribute* attr;
        Node* node;
        for (size_t i = 0; i < graph->getSizeAttributes(); ++i)
        {
          attr = graph->getAttribute(i);
          node = graph->getNode(it->first);
          if (attr->getSizeCurValues() > 0)
          {
            attrs.push_back(attr);
            vals.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
          }
          else
          {
            double val = node->getTupleVal(attr->getIndex());
            attrs.push_back(attr);
            vals.push_back(val);
          }
        }
        attr = 0;
        node = 0;

        diagram->visualize(
          inSelectMode,
          pixelSize(),
          attrs,
          vals);
        attrs.clear();
        vals.clear();

        VisUtils::enableLineAntiAlias();

        // close icon
        VisUtils::setColor(VisUtils::coolBlue);
        VisUtils::fillCloseIcon(0.8, 0.96, 0.96, 0.8);
        VisUtils::setColor(VisUtils::lightLightGray);
        VisUtils::drawCloseIcon(0.8, 0.96, 0.96, 0.8);
        // more icon
        VisUtils::setColor(VisUtils::coolBlue);
        VisUtils::fillMoreIcon(-0.98, -0.8, -0.8, -0.98);
        VisUtils::setColor(VisUtils::lightLightGray);
        VisUtils::drawMoreIcon(-0.98, -0.8, -0.8, -0.98);

        if (it->first == currIdxDgrm && itemsMarked.size() > 1)
        {
          VisUtils::setColor(VisUtils::coolBlue);
          VisUtils::fillRwndIcon(0.2, 0.36, -0.8, -0.98);
          VisUtils::setColor(VisUtils::lightLightGray);
          VisUtils::drawRwndIcon(0.2, 0.36, -0.8, -0.98);

          VisUtils::setColor(VisUtils::coolBlue);
          VisUtils::fillPrevIcon(0.4, 0.56, -0.8, -0.98);
          VisUtils::setColor(VisUtils::lightLightGray);
          VisUtils::drawPrevIcon(0.4, 0.56, -0.8, -0.98);

          VisUtils::setColor(VisUtils::coolBlue);
          VisUtils::fillPlayIcon(0.6, 0.76, -0.8, -0.98);
          VisUtils::setColor(VisUtils::lightLightGray);
          VisUtils::drawPlayIcon(0.6, 0.76, -0.8, -0.98);

          VisUtils::setColor(VisUtils::coolBlue);
          VisUtils::fillNextIcon(0.8, 0.96, -0.8, -0.98);
          VisUtils::setColor(VisUtils::lightLightGray);
          VisUtils::drawNextIcon(0.8, 0.96, -0.8, -0.98);
        }

        VisUtils::disableLineAntiAlias();
        glPopMatrix();
      }
    }
  }
}


void TimeSeries::drawMouseOver(const bool& inSelectMode)
{
  if (inSelectMode != true)
  {
    if (mouseOverIdx != NON_EXISTING && attributes.size() > 0)
    {
      double pix = pixelSize();
      Position2D pos1, pos2;
      vector< string > lbls;
      vector< Position2D > posTopLft;
      vector< Position2D > posBotRgt;
      size_t maxLbl = 0;

      pos1.x = posScaleTopLft.x + (mouseOverIdx - wdwStartIdx)*itvWdwPerNode;
      pos1.y = posAxesTopLft[0].y;
      pos2.x = pos1.x;
      pos2.y = posAxesBotRgt[posAxesBotRgt.size()-1].y;

      double txtScaling = settings->textSize.value()*pix/CHARHEIGHT;

      VisUtils::setColor(VisUtils::coolBlue);
      VisUtils::drawLine(pos1.x, pos2.x, pos1.y, pos2.y);

      for (size_t i = 0; i < attributes.size(); ++i)
      {
        string lbl;
        Attribute* attr = attributes[i];
        Node* node = graph->getNode(mouseOverIdx);

        lbl = attr->mapToValue(node->getTupleVal(attr->getIndex()))->getValue();

        attr = 0;
        node = 0;
        lbls.push_back(lbl);

        if (lbl.size() > maxLbl)
        {
          maxLbl = lbl.size();
        }

        pos1.x = posScaleTopLft.x + (mouseOverIdx - wdwStartIdx)*itvWdwPerNode;
        pos1.y = posAxesTopLft[i].y /*+ 0.5*txtScaling*CHARHEIGHT*/;
        pos2.x = pos1.x + (lbl.size()+1)*txtScaling*CHARWIDTH;
        pos2.y = pos1.y - 1.0*txtScaling*CHARHEIGHT;
        posTopLft.push_back(pos1);
        posBotRgt.push_back(pos2);
      }

      if (pos1.x + maxLbl*txtScaling*CHARWIDTH > posScaleBotRgt.x)
      {
        for (size_t i = 0; i < posTopLft.size(); ++i)
        {
          double diff = posBotRgt[i].x-posTopLft[i].x;
          posTopLft[i].x -= diff;
          posBotRgt[i].x -= diff;
        }
      }

      for (size_t i = 0; i < posTopLft.size(); ++i)
      {
        VisUtils::setColor(Qt::white);
        VisUtils::fillRect(
          posTopLft[i].x,
          posBotRgt[i].x,
          posTopLft[i].y,
          posBotRgt[i].y);
        VisUtils::setColor(VisUtils::coolBlue);
        VisUtils::drawRect(
          posTopLft[i].x,
          posBotRgt[i].x,
          posTopLft[i].y,
          posBotRgt[i].y);

        VisUtils::setColor(settings->textColor.value());
        VisUtils::drawLabel(
          texCharId,
          posTopLft[i].x + 0.5*txtScaling*CHARWIDTH,
          posTopLft[i].y - 0.5*txtScaling*CHARHEIGHT,
          settings->textSize.value()*pix/CHARHEIGHT,
          lbls[i]);
      }
    }
  }
}


void TimeSeries::drawLabels(const bool& inSelectMode)
{
  if (inSelectMode == true)
    {}
  else
  {
    double pix = pixelSize();
    double txtScaling = settings->textSize.value()*pix/CHARHEIGHT;

    for (size_t i = 0; i < posAxesTopLft.size(); ++i)
    {
      string lblTop, lblBot;
      lblTop = Utils::dblToStr(0);
      lblBot = Utils::dblToStr(0);

      // min
      VisUtils::setColor(Qt::white);
      VisUtils::drawLabel(
        texCharId,
        posAxesTopLft[i].x + 2.0*pix,
        posAxesBotRgt[i].y + 0.5*txtScaling*CHARHEIGHT + 1.0*pix,
        settings->textSize.value()*pix/CHARHEIGHT,
        lblBot);
      VisUtils::setColor(settings->textColor.value());
      VisUtils::drawLabel(
        texCharId,
        posAxesTopLft[i].x + 1.0*pix,
        posAxesBotRgt[i].y + 0.5*txtScaling*CHARHEIGHT + 2.0*pix,
        settings->textSize.value()*pix/CHARHEIGHT,
        lblBot);

      // max
      VisUtils::setColor(Qt::white);
      VisUtils::drawLabel(
        texCharId,
        posAxesTopLft[i].x + 2.0*pix,
        posAxesTopLft[i].y - 0.5*txtScaling*CHARHEIGHT - 2.0*pix,
        settings->textSize.value()*pix/CHARHEIGHT,
        lblTop);
      VisUtils::setColor(settings->textColor.value());
      VisUtils::drawLabel(
        texCharId,
        posAxesTopLft[i].x + 1.0*pix,
        posAxesTopLft[i].y - 0.5*txtScaling*CHARHEIGHT - 1.0*pix,
        settings->textSize.value()*pix/CHARHEIGHT,
        lblTop);

      // attribute
      VisUtils::setColor(Qt::white);
      VisUtils::drawLabelCenter(
        texCharId,
        posAxesTopLft[i].x + 0.5*(posAxesBotRgt[i].x - posAxesTopLft[i].x) + 1.0*pix,
        posAxesTopLft[i].y - 0.5*txtScaling*CHARHEIGHT - 2.0*pix,
        settings->textSize.value()*pix/CHARHEIGHT,
        attributes[i]->name().toStdString());
      VisUtils::setColor(settings->textColor.value());
      VisUtils::drawLabelCenter(
        texCharId,
        posAxesTopLft[i].x + 0.5*(posAxesBotRgt[i].x - posAxesTopLft[i].x),
        posAxesTopLft[i].y - 0.5*txtScaling*CHARHEIGHT - 1.0*pix,
        settings->textSize.value()*pix/CHARHEIGHT,
        attributes[i]->name().toStdString());
    }
  }
}


void TimeSeries::handleHitSlider()
{
  double distWorld = worldCoordinate(m_lastMouseEvent.posF()).x() - (posSliderTopLft.x + wdwStartIdx*itvSliderPerNode + 0.5*nodesWdwScale*itvSliderPerNode);

  dragDistNodes = distWorld/itvSliderPerNode;

  if (distWorld < 0)
  {
    // move to left
    if (((double)wdwStartIdx + dragDistNodes) < 0)
    {
      wdwStartIdx   = 0;
    }
    else if (((double)wdwStartIdx + dragDistNodes) >= 0)
    {
      wdwStartIdx += (size_t)dragDistNodes;
    }
  }
  else if (distWorld > 0)
  {
    // move to right
    if ((wdwStartIdx + (int)dragDistNodes + nodesWdwScale) <= (graph->getSizeNodes()/*-1*/))
    {
      wdwStartIdx += (int)dragDistNodes;
    }
    else if ((wdwStartIdx + (int)dragDistNodes + nodesWdwScale) > (graph->getSizeNodes()/*-1*/))
    {
      wdwStartIdx = (graph->getSizeNodes()/*-1*/) - nodesWdwScale;
    }
  }

  dragDistNodes = 0.0;
}


void TimeSeries::handleDragSliderHdl()
{
//    draggingSlider = true;
  dragStatus = DRAG_STATUS_SLDR;

  double distWorld = worldCoordinate(m_lastMouseEvent.posF()).x() - worldCoordinate(m_lastMousePos).x();
  dragDistNodes += (distWorld/itvSliderPerNode);

  if (dragDistNodes < -1)
  {
    // move to left
    if (((double)wdwStartIdx + dragDistNodes) < 0)
    {
      wdwStartIdx   = 0;
      dragDistNodes = 0.0;
    }
    else if (((double)wdwStartIdx + dragDistNodes) >= 0)
    {
      wdwStartIdx += (size_t)dragDistNodes;
      dragDistNodes -= (size_t)dragDistNodes;
    }
  }
  else if (dragDistNodes > 1)
  {
    // move to right
    if ((wdwStartIdx + (int)dragDistNodes + nodesWdwScale) <= (graph->getSizeNodes()/*-1*/))
    {
      wdwStartIdx += (int)dragDistNodes;
      dragDistNodes -= (int)dragDistNodes;
    }
    else if ((wdwStartIdx + (int)dragDistNodes + nodesWdwScale) > (graph->getSizeNodes()/*-1*/))
    {
      wdwStartIdx = (graph->getSizeNodes()/*-1*/) - nodesWdwScale;
      dragDistNodes = 0.0;
    }
  }
}


void TimeSeries::handleDragSliderHdlLft()
{
  double pix  = pixelSize();
  double xHdl = posSliderTopLft.x + wdwStartIdx*itvSliderPerNode;

  double distWorld  = Utils::maxx(posSliderTopLft.x, worldCoordinate(m_lastMouseEvent.posF()).x())-xHdl;
  double distNodes  = Utils::rndToNearestMult((distWorld/itvSliderPerNode), 1.0);

  double distWindow = posSliderBotRgt.x - posSliderTopLft.x;
  double pixWindow  = distWindow/pix;

  double tempPixPerNode = pixWindow/(double)(nodesWdwScale-distNodes);
  if (tempPixPerNode < minPixPerNode)
  {
    // update pixels per node
    actPixPerNode  = tempPixPerNode;
    wdwStartIdx   += int (distNodes);

    geomChanged    = true;
  }
  else
  {
    dragStatus = -1;
  }
}


void TimeSeries::handleDragSliderHdlRgt()
{
  double pix  = pixelSize();
  double xHdl = posSliderTopLft.x + (wdwStartIdx + nodesWdwScale)*itvSliderPerNode;

  double distWorld  = Utils::minn(posSliderTopLft.x, worldCoordinate(m_lastMouseEvent.posF()).x())-xHdl;
  double distNodes  = distWorld/itvSliderPerNode;

  double distWindow = posSliderBotRgt.x - posSliderTopLft.x;
  double pixWindow  = distWindow/pix;

  double tempPixPerNode = pixWindow/(double)(nodesWdwScale+distNodes);
  if (tempPixPerNode < minPixPerNode)
  {
    // update pixels per node
    actPixPerNode  = tempPixPerNode;
    geomChanged = true;
  }
  else
  {
    dragStatus = -1;
  }
}


void TimeSeries::handleHitItems(const int& idx)
{
  if (shiftStartIdx < 0)
  {
    shiftStartIdx = idx;
  }

  // calc index of animation frame
  size_t prevAnimIdx;
  if (animFrame != itemsMarked.end())
  {
    prevAnimIdx = *animFrame;
  }
  else
  {
    prevAnimIdx = NON_EXISTING;
  }

  // shift key
  if (m_lastKeyCode == Qt::Key_Shift)
  {
    int begIdx, endIdx;

    // calc range of marked items
    if (shiftStartIdx <= idx)
    {
      begIdx = shiftStartIdx;
      endIdx = idx;
    }
    else if (idx < shiftStartIdx)
    {
      begIdx = idx;
      endIdx = shiftStartIdx;
    }


    // update marked items
    itemsMarked.clear();
    for (int i = begIdx; i <= endIdx; ++i)
    {
      itemsMarked.insert(i);
    }
  }
  // control key
  else if (m_lastKeyCode == Qt::Key_Control)
  {
    // update marked items
    set< size_t >::iterator it;
    it = itemsMarked.find(idx);
    if (it == itemsMarked.end())
    {
      itemsMarked.insert(idx);
    }
    else
    {
      itemsMarked.erase(it);
    }
    shiftStartIdx = idx;
  }
  // no key
  else
  {
    // update marked items
    itemsMarked.clear();
    itemsMarked.insert(idx);
    shiftStartIdx = idx;
  }

  // update animation frame
  animFrame = itemsMarked.find(prevAnimIdx);
  if (animFrame == itemsMarked.end())
  {
    animFrame = itemsMarked.begin();
  }

  dragStartIdx = idx;

  mediator->handleMarkFrameClust(this);
}


void TimeSeries::handleDragItems(const int& idx)
{
  if (dragStartIdx < 0)
  {
    dragStartIdx = idx;
  }

  // calc index of animation frame
  size_t prevAnimIdx;
  if (animFrame != itemsMarked.end())
  {
    prevAnimIdx = *animFrame;
  }
  else
  {
    prevAnimIdx = NON_EXISTING;
  }

  dragStatus = DRAG_STATUS_ITMS;

  if (dragStartIdx != idx)
  {
    // sort indices
    bool flag = false;

    // shift key
    if (m_lastKeyCode == Qt::Key_Shift)
    {
      /*
      for ( int i = begIdx; i <= endIdx; ++i )
          itemsMarked.insert( i );
      */
    }
    else if (m_lastKeyCode == Qt::Key_Control)
    {
      int begIdx, endIdx;
      bool incr;

      if (dragStartIdx <= idx)
      {
        incr = true;
        begIdx = dragStartIdx;
        endIdx = idx;
        if (dragDir != DRAG_DIR_RGT)
        {
          flag = true;
        }
        dragDir = DRAG_DIR_RGT;
      }
      else
      {
        incr = false;
        begIdx = idx;
        endIdx = dragStartIdx;
        if (dragDir != DRAG_DIR_LFT)
        {
          flag = true;
        }
        dragDir = DRAG_DIR_LFT;
      }

      set< size_t >::iterator it;
      if (incr == true)
      {
        if (flag == true)
        {
          it = itemsMarked.find(begIdx);
          if (it != itemsMarked.end())
          {
            itemsMarked.erase(begIdx);
          }
          flag = false;
        }

        for (int i = begIdx+1; i <= endIdx; ++i)
        {
          it = itemsMarked.find(i);
          if (it == itemsMarked.end())
          {
            itemsMarked.insert(i);
          }
          else
          {
            itemsMarked.erase(i);
          }
        }

        dragStartIdx = endIdx;
      }
      else
      {
        if (flag == true)
        {
          it = itemsMarked.find(endIdx);
          if (it != itemsMarked.end())
          {
            itemsMarked.erase(endIdx);
          }
          flag = false;
        }

        for (int i = begIdx; i < endIdx; ++i)
        {
          it = itemsMarked.find(i);
          if (it == itemsMarked.end())
          {
            itemsMarked.insert(i);
          }
          else
          {
            itemsMarked.erase(i);
          }
        }

        dragStartIdx = begIdx;
      }
    }
    else
    {
      int begIdx, endIdx;
      if (dragStartIdx <= idx)
      {
        begIdx = dragStartIdx;
        endIdx = idx;
      }
      else
      {
        begIdx = idx;
        endIdx = dragStartIdx;
      }

      itemsMarked.clear();
      for (int i = begIdx; i <= endIdx; ++i)
      {
        itemsMarked.insert(i);
      }
    }
  }

  // update animation frame
  animFrame = itemsMarked.find(prevAnimIdx);
  if (animFrame == itemsMarked.end())
  {
    animFrame = itemsMarked.begin();
  }

  mediator->handleMarkFrameClust(this);
}


void TimeSeries::handleShowDiagram(const int& dgrmIdx)
{
  map< size_t, Position2D >::iterator it;

  it = showDgrm.find(dgrmIdx);
  // diagram doesn't exist, add it
  if (it == showDgrm.end())
  {
    Position2D pos;
    pos.x = posScaleTopLft.x + (dgrmIdx+0.5)*itvWdwPerNode;
    pos.y = 0.0;
    showDgrm.insert(pair< int, Position2D >(dgrmIdx, pos));
  }
  // diagram exists, remove it
  else
  {
    showDgrm.erase(it);
  }
}


void TimeSeries::handleDragDiagram(const int& dgrmIdx)
{
  dragStatus = DRAG_STATUS_DGRM;

  map< size_t, Position2D >::iterator it;
  it = showDgrm.find(dgrmIdx);
  if (it != showDgrm.end())
  {
    QPointF delta = worldCoordinate(m_lastMouseEvent.posF()) - worldCoordinate(m_lastMousePos);
    it->second.x += delta.x();
    it->second.y += delta.y();
  }
}
