// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./simulator.cpp

#include "simulator.h"

using namespace std;

static const int labelHeight = 40;
static const int timerInterval = 10;
static const double animationPixelsPerMS =  1.0;



// -- constructors and destructor -----------------------------------


Simulator::Simulator(
  QWidget *parent,
  Mediator* m,
  Settings* s,
  Graph* g)
  : Visualizer(parent, g),
    Colleague(m),
    m_settings(s)
{
  m_diagram   = 0;
  m_currentFrame = 0;

  m_currentSelection = -1;
  m_currentSelectionIndex = -1;

  m_lastSelection     = -1;
  m_lastSelectionIndexPrevious = -1;
  m_lastSelectionIndexNext = -1;

  m_previousBundleFocusIndex  = -1;
  m_nextBundleFocusIndex  = -1;

  connect(&m_animationTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
  m_animationTimer.start(timerInterval);

  connect(&m_settings->backgroundColor, SIGNAL(changed(QColor)), this, SLOT(update()));
  connect(&m_settings->textColor, SIGNAL(changed(QColor)), this, SLOT(update()));
  connect(&m_settings->textSize, SIGNAL(changed(int)), this, SLOT(update()));
  connect(&m_settings->blendType, SIGNAL(changed(int)), this, SLOT(update()));
}


Simulator::~Simulator()
{
  graph = 0;

  clearDiagram();
  clearFrames();
  clearBundles();

}


// -- get functions ---------------------------------------------


size_t Simulator::SelectedClusterIndex()
{
  size_t result = NON_EXISTING;

  if (m_currentSelection == ID_FRAME_PREV)
  {
    if (0 <= m_currentSelectionIndex && static_cast <size_t>(m_currentSelectionIndex) < m_previousFrames.size())
    {
      result = m_previousFrames[m_currentSelectionIndex]->getNode(0)->getCluster()->getIndex();
    }
  }
  else if (m_currentSelection == ID_FRAME_CURR)
  {
    if (m_currentFrame != 0)
    {
      result = m_currentFrame->getNode(0)->getCluster()->getIndex();
    }
  }
  else if (m_currentSelection == ID_FRAME_NEXT)
  {
    if (0 <= m_currentSelectionIndex && static_cast <size_t>(m_currentSelectionIndex) < m_nextFrames.size())
    {
      result = m_nextFrames[m_currentSelectionIndex]->getNode(0)->getCluster()->getIndex();
    }
  }

  return result;
}


// -- set functions ---------------------------------------------


void Simulator::setDiagram(Diagram* dgrm)
{
  clearDiagram();
  m_diagram = dgrm;

  dataChanged = true;
}


void Simulator::initFrameCurr(
  Cluster* frame,
  const vector< Attribute* > &attrs)
{
  // clear previous data
  clearAttributes();
  clearFrames();
  clearBundles();

  m_lastSelection     = ID_FRAME_CURR;
  m_lastSelectionIndexPrevious = -1;
  m_lastSelectionIndexNext = -1;

  // update new data
  m_attributes = attrs;
  m_currentFrame = new Cluster(*frame);
  initFramesPrevNext();
  initBundles();
  sortFramesPrevNext();

  // init & visualize
  dataChanged = true;
  update();
}


void Simulator::updateFrameCurr(
  Cluster* frame,
  const Position2D& pos)
{
  // init animation data
  m_animationOldFrame    = frame;
  m_animationStartPosition = pos;

  m_animationNewFrame    = new Cluster(*m_currentFrame);
  m_animationEndPosition = m_currentFramePosition;

  m_animationCurrentPosition = m_animationStartPosition;

  // calc animation parameters
  double xTo, yTo, xFr, yFr;
  xTo = m_animationEndPosition.x;
  yTo = m_animationEndPosition.y;
  xFr = m_animationStartPosition.x;
  yFr = m_animationStartPosition.y;

  double distPix = Utils::dist(xTo, yTo, xFr, yFr) / pixelSize();
  m_totalAnimationTime = distPix/animationPixelsPerMS;
  m_totalBlendTime = 0.0;

  m_currentAnimationPhase = ANIM_POS;

  // clear previous data
  clearFrames();
  clearBundles();

  m_lastSelection     = ID_FRAME_CURR;
  m_lastSelectionIndexPrevious = -1;
  m_lastSelectionIndexNext = -1;

  m_animationTimer.start(timerInterval);
  setMouseTracking(false);
}


void Simulator::clearData()
{
  m_currentSelection  = -1;
  m_currentSelectionIndex  = -1;

  m_lastSelection     = -1;
  m_lastSelectionIndexPrevious = -1;
  m_lastSelectionIndexPrevious = -1;

  m_previousBundleFocusIndex  = -1;
  m_nextBundleFocusIndex  = -1;

  clearAttributes();
  clearFrames();
  clearBundles();
}


// -- visualization functions  --------------------------------------


void Simulator::visualize(const bool& inSelectMode)
{
  // have textures been generated
  if (!texCharOK)
  {
    genCharTex();
  }

  clear();

  // check if positions are ok
  if (geomChanged)
  {
    calcSettingsGeomBased();
  }
  if (dataChanged)
  {
    calcSettingsDataBased();
  }

  if (inSelectMode)
  {
    if (!m_animationTimer.isActive())
    {
      QSizeF size = worldSize();

      GLint hits = 0;
      GLuint selectBuf[512];
      startSelectMode(
        hits,
        selectBuf,
        2.0,
        2.0);

      glPushName(ID_CANVAS);
      VisUtils::fillRect(-0.5*size.width(), 0.5*size.width(), 0.5*size.height(), -0.5*size.height());

      drawBdlLblGridPrev(inSelectMode);
      drawBdlLblGridNext(inSelectMode);
      drawBundlesPrev(inSelectMode);
      drawBundlesNext(inSelectMode);
      drawFrameCurr(inSelectMode);
      drawFramesPrev(inSelectMode);
      drawFramesNext(inSelectMode);
      if (m_previousFrames.size() > 0 || m_currentFrame != 0 || m_nextFrames.size() > 0)
      {
        drawControls(inSelectMode);
      }

      glPopName();

      finishSelectMode(
        hits,
        selectBuf);
    }
  }
  else
  {
    if (m_animationTimer.isActive())
    {
      animate();
      drawControls(inSelectMode);
    }
    else
    {
      drawBdlLblGridPrev(inSelectMode);
      drawBdlLblGridNext(inSelectMode);
      drawBundlesPrev(inSelectMode);
      drawBundlesNext(inSelectMode);
      drawFrameCurr(inSelectMode);
      drawFramesPrev(inSelectMode);
      drawFramesNext(inSelectMode);
      if (m_previousFrames.size() > 0 || m_currentFrame != 0 || m_nextFrames.size() > 0)
      {
        drawControls(inSelectMode);
      }
    }
  }
}


// -- event handlers ------------------------------------------------


void Simulator::handleMouseEvent(QMouseEvent* e)
{
  Visualizer::handleMouseEvent(e);

  // redraw in select mode
  updateGL(true);
  // redraw in render mode
  updateGL();
}


void Simulator::handleMouseLeaveEvent()
{
  Visualizer::handleMouseLeaveEvent();

  if (!showMenu)
  {
    m_currentSelection = -1;
    m_currentSelectionIndex = -1;

    m_previousBundleFocusIndex = -1;
    m_nextBundleFocusIndex = -1;

    mediator->handleUnmarkFrameClusts(this);
    emit hoverCluster(0);
  }
  else
  {
    showMenu = false;
  }

  // redraw in render mode
  updateGL();
}


void Simulator::handleKeyEvent(QKeyEvent* e)
{
  Visualizer::handleKeyEvent(e);

  if (!m_animationTimer.isActive() && e->type() == QEvent::KeyPress)
  {
    if (e->key() == Qt::Key_Up)
    {
      handleKeyUp();
    }
    else if (e->key() == Qt::Key_Right)
    {
      handleKeyRgt();
    }
    else if (e->key() == Qt::Key_Down)
    {
      handleKeyDwn();
    }
    else if (e->key() == Qt::Key_Left)
    {
      handleKeyLft();
    }
    else if (e->key() == Qt::Key_Escape)
    {
      m_lastSelection = m_currentSelection;

      if (m_currentSelection == ID_FRAME_PREV)
      {
        m_lastSelectionIndexPrevious = m_currentSelectionIndex;
      }
      else if (m_currentSelection == ID_FRAME_NEXT)
      {
        m_lastSelectionIndexNext = m_currentSelectionIndex;
      }

      m_currentSelection = -1;
      m_currentSelectionIndex = -1;

      mediator->handleUnmarkFrameClusts(this);
      emit hoverCluster(0);
    }

    markFrameClusts();

    // redraw in render mode
    updateGL();
  }
}

// -- utility functions ---------------------------------------------

void Simulator::initFramesPrevNext()
{
  Node*        temp;
  set< Node* > tempPrev;
  set< Node* > tempNext;
  Cluster*     nodesPrev;
  Cluster*     nodesNext;

  // get nodes leading to & from current frame
  for (size_t i = 0; i < m_currentFrame->getSizeNodes(); ++i)
  {
    temp = m_currentFrame->getNode(i);

    // incoming nodes
    {
      for (size_t j = 0; j < temp->getSizeInEdges(); ++j)
      {
        tempPrev.insert(temp->getInEdge(j)->getInNode());
      }
    }

    // outgoing nodes
    {
      for (size_t j = 0; j < temp->getSizeOutEdges(); ++j)
      {
        tempNext.insert(temp->getOutEdge(j)->getOutNode());
      }
    }
  }

  // update clusters of incoming & outgoing nodes
  nodesPrev = new Cluster();
  nodesNext = new Cluster();
  set< Node* >::iterator it;
  {
    for (it = tempPrev.begin(); it != tempPrev.end(); ++it)
    {
      nodesPrev->addNode(*it);
    }
  }
  {
    for (it = tempNext.begin(); it != tempNext.end(); ++it)
    {
      nodesNext->addNode(*it);
    }
  }

  // calculate prev & next frames
  graph->calcAttrCombn(
    nodesPrev,
    m_attributes,
    m_previousFrames);
  graph->calcAttrCombn(
    nodesNext,
    m_attributes,
    m_nextFrames);

  // clear memory
  temp = 0;
  tempPrev.clear();
  tempNext.clear();
  delete nodesPrev;
  nodesPrev = 0;
  delete nodesNext;
  nodesNext = 0;
}


void Simulator::initBundles()
{
  Node*    node;
  Cluster* clst;
  Edge*    edge;
  set< Node* >           currNodes;
  map< string, Bundle* > bdls;
  Bundle*                bdl;
  map< string, Bundle* > lbls;
  Bundle*                bdlLbls;

  // get nodes in current frame
  {
    for (size_t i = 0; i < m_currentFrame->getSizeNodes(); ++i)
    {
      currNodes.insert(m_currentFrame->getNode(i));
    }
  }

  // get all edges from previous frames to current frame
  lbls.clear();
  {
    for (size_t i = 0; i < m_previousFrames.size(); ++i)
    {
      bdls.clear();

      clst = m_previousFrames[i];
      for (size_t j = 0; j < clst->getSizeNodes(); ++j)
      {
        node = clst->getNode(j);
        for (size_t k = 0; k < node->getSizeOutEdges(); ++k)
        {
          edge = node->getOutEdge(k);

          if (currNodes.find(edge->getOutNode()) != currNodes.end())
          {
            map< string, Bundle* >::iterator pos;

            pos = lbls.find(edge->getLabel());
            if (pos == lbls.end())
            {
              bdlLbls = new Bundle();

              lbls.insert(pair< string, Bundle* >(edge->getLabel(), bdlLbls));
            }
            else
            {
              bdlLbls = pos->second;
            }

            // bundles
            pos = bdls.find(edge->getLabel());
            if (pos == bdls.end())
            {
              bdl = new Bundle(m_bundles.size());
              m_bundles.push_back(bdl);

              bdls.insert(pair< string, Bundle* >(edge->getLabel(), bdl));

              clst->addOutBundle(bdl);
              m_currentFrame->addInBundle(bdl);

              bdl->setInCluster(clst);
              bdl->setOutCluster(m_currentFrame);

              bdl->setParent(bdlLbls);
              bdlLbls->addChild(bdl);
            }
            else
            {
              bdl = pos->second;
            }
            bdl->addEdge(edge);
          }
        }
      }
    }
  }

  map< string, Bundle* >::iterator it;
  for (it = lbls.begin(); it != lbls.end(); ++it)
  {
    bdlLbls = it->second;
    bdlLbls->setIndex(m_bundlesPreviousByLabel.size());
    m_bundlesPreviousByLabel.push_back(bdlLbls);
  }

  // get all edges from current frame to next frames
  lbls.clear();
  {
    for (size_t i = 0; i < m_nextFrames.size(); ++i)
    {
      bdls.clear();
      clst = m_nextFrames[i];
      for (size_t j = 0; j < clst->getSizeNodes(); ++j)
      {
        node = clst->getNode(j);
        for (size_t k = 0; k < node->getSizeInEdges(); ++k)
        {
          edge = node->getInEdge(k);

          if (currNodes.find(edge->getInNode()) != currNodes.end())
          {
            map< string, Bundle* >::iterator pos;

            pos = lbls.find(edge->getLabel());
            if (pos == lbls.end())
            {
              bdlLbls = new Bundle();

              lbls.insert(pair< string, Bundle* >(edge->getLabel(), bdlLbls));
            }
            else
            {
              bdlLbls = pos->second;
            }

            pos = bdls.find(edge->getLabel());
            if (pos == bdls.end())
            {
              bdl = new Bundle();
              m_bundles.push_back(bdl);

              bdls.insert(pair< string, Bundle* >(edge->getLabel(), bdl));

              m_currentFrame->addOutBundle(bdl);
              clst->addInBundle(bdl);

              bdl->setInCluster(m_currentFrame);
              bdl->setOutCluster(clst);

              bdl->setParent(bdlLbls);
              bdlLbls->addChild(bdl);
            }
            else
            {
              bdl = pos->second;
            }
            bdl->addEdge(edge);
          }
        }
      }
    }
  }

  for (it = lbls.begin(); it != lbls.end(); ++it)
  {
    bdlLbls = it->second;
    bdlLbls->setIndex(m_bundlesNextByLabel.size());
    m_bundlesNextByLabel.push_back(bdlLbls);
  }

  lbls.clear();
  {
    for (size_t i = 0; i < m_bundlesPreviousByLabel.size(); ++i)
    {
      bdl = m_bundlesPreviousByLabel[i];

      map< string, Bundle* >::iterator pos;
      pos = lbls.find(bdl->getChild(0)->getEdge(0)->getLabel());

      if (pos == lbls.end())
      {
        bdlLbls = new Bundle();
        lbls.insert(pair< string, Bundle* >(
                      bdl->getChild(0)->getEdge(0)->getLabel(),
                      bdlLbls));
      }
      else
      {
        bdlLbls = pos->second;
      }

      bdl->setParent(bdlLbls);
      bdlLbls->addChild(bdl);
    }
  }

  {
    for (size_t i = 0; i < m_bundlesNextByLabel.size(); ++i)
    {
      bdl = m_bundlesNextByLabel[i];

      map< string, Bundle* >::iterator pos;
      pos = lbls.find(bdl->getChild(0)->getEdge(0)->getLabel());

      if (pos == lbls.end())
      {
        bdlLbls = new Bundle();
        lbls.insert(pair< string, Bundle* >(
                      bdl->getChild(0)->getEdge(0)->getLabel(),
                      bdlLbls));
      }
      else
      {
        bdlLbls = pos->second;
      }

      bdl->setParent(bdlLbls);
      bdlLbls->addChild(bdl);
    }
  }

  for (it = lbls.begin(); it != lbls.end(); ++it)
  {
    bdlLbls = it->second;
    bdlLbls->setIndex(m_bundlesByLabel.size());
    m_bundlesByLabel.push_back(bdlLbls);
  }

  // clear memory
  clst = 0;
  edge = 0;
  currNodes.clear();
  bdls.clear();
  bdl = 0;

  lbls.clear();
  bdlLbls = 0;
}


void Simulator::sortFramesPrevNext()
{
  multimap< int, Cluster* > sorted;

  // sort previous frames
  {
    for (size_t i = 0; i < m_previousFrames.size(); ++i)
    {
      int key = 0;
      for (size_t j = 0; j < m_previousFrames[i]->getSizeOutBundles(); ++j)
      {
        key += (int)pow(10.0, (int) m_previousFrames[i]->getOutBundle(j)->getParent()->getIndex());
      }

      sorted.insert(pair< int, Cluster* >(key, m_previousFrames[i]));
    }
  }

  m_previousFrames.clear();
  multimap< int, Cluster* >::iterator it;
  for (it = sorted.begin(); it != sorted.end(); ++it)
  {
    m_previousFrames.push_back(it->second);
  }
  sorted.clear();

  // sort previous frames
  for (size_t i = 0; i < m_nextFrames.size(); ++i)
  {
    int key = 0;
    for (size_t j = 0; j < m_nextFrames[i]->getSizeInBundles(); ++j)
    {
      key += (int)pow(10.0, (int) m_nextFrames[i]->getInBundle(j)->getParent()->getIndex());
    }

    sorted.insert(pair< int, Cluster* >(key, m_nextFrames[i]));
  }

  m_nextFrames.clear();
  for (it = sorted.begin(); it != sorted.end(); ++it)
  {
    m_nextFrames.push_back(it->second);
  }
  sorted.clear();
}


void Simulator::calcSettingsGeomBased()
{
  // update flag
  geomChanged = false;

  calcPosFrames();
  calcPosBundles();
}


void Simulator::calcSettingsDataBased()
{
  // update flag
  dataChanged = false;

  calcPosFrames();
  calcPosBundles();
}


void Simulator::calcPosFrames()
{
  Position2D pos;

  // clear previous positions
  m_previousFramePositions.clear();
  m_nextFramePositions.clear();

  QSizeF size = worldSize();
  double pix = pixelSize();
  double itvHori = size.width()/6;
  double itvVert = (size.height()-itvHori)/Utils::maxx(1, Utils::maxx(m_previousFrames.size(), m_nextFrames.size()));
  m_horizontalFrameScale = 0.5*itvHori;
  m_verticalFrameScale = Utils::minn(m_horizontalFrameScale, 0.45*itvVert);

  // calc new positions
  pos.x = 0;
  pos.y = 0;
  m_currentFramePosition = pos;

  pos.x = -0.5*size.width() + 0.5*itvHori + 4.0*pix;
  pos.y = 0.5*m_previousFrames.size()*itvVert - 0.5*itvVert;
  {
    for (size_t i = 0; i < m_previousFrames.size(); ++i)
    {
      m_previousFramePositions.push_back(pos);
      pos.y -= itvVert;
    }
  }

  pos.x = 0.5*size.width() - 0.5*itvHori - 4.0*pix;
  pos.y = 0.5*m_nextFrames.size()*itvVert - 0.5*itvVert;
  {
    for (size_t i = 0; i < m_nextFrames.size(); ++i)
    {
      m_nextFramePositions.push_back(pos);
      pos.y -= itvVert;
    }
  }
}


void Simulator::calcPosBundles()
{
  Position2D posTopLft, posBotRgt;

  // clear previous positions
  m_previousBundleLabelPositionTL.clear();
  m_previousBundleLabelPositionBR.clear();
  m_nextBundleLabelPositionTL.clear();
  m_nextBundleLabelPositionBR.clear();

  m_previousBundlePositionTL.clear();
  m_previousBundlePositionBR.clear();
  m_nextBundlePositionTL.clear();
  m_nextBundlePositionBR.clear();

  QSizeF size = worldSize();
  double pix = pixelSize();
  double itvHori = size.width()/6;
  double itvVert = (size.height()-itvHori)/Utils::maxx(1, Utils::maxx(m_previousFrames.size(), m_nextFrames.size()));

  // calc new positions
  if (m_previousFramePositions.size() > 0 && m_bundlesPreviousByLabel.size() > 0)
  {
    // grid prev
    if (m_previousFramePositions.size() >= m_nextFramePositions.size())
    {
      posTopLft.y = m_previousFramePositions[0].y + 1.0*m_verticalFrameScale + 0.125*itvVert;
      posBotRgt.y = m_previousFramePositions[m_previousFramePositions.size()-1].y - 1.0*m_verticalFrameScale - 0.125*itvVert;
    }
    else
    {
      posTopLft.y = m_nextFramePositions[0].y + 1.0*m_verticalFrameScale + 0.125*itvVert;
      posBotRgt.y = m_nextFramePositions[m_nextFramePositions.size()-1].y - 1.0*m_verticalFrameScale - 0.125*itvVert;
    }

    double itvGrid = (1.5*itvHori)/(m_bundlesPreviousByLabel.size()+1);

    {
      for (size_t i = 0; i < m_bundlesPreviousByLabel.size(); ++i)
      {
        posTopLft.x = -2.0*itvHori + (i+1)*itvGrid;
        posTopLft.y =  0.5*size.height() - labelHeight*pix;
        posBotRgt.x =  posTopLft.x;
        posBotRgt.y = -0.5*size.height() + labelHeight*pix;

        m_previousBundleLabelPositionTL.push_back(posTopLft);
        m_previousBundleLabelPositionBR.push_back(posBotRgt);
      }
    }

    // bundles prev
    {
      for (size_t i = 0; i < m_previousFrames.size(); ++i)
      {
        vector< Position2D > v;

        m_previousBundlePositionTL.push_back(v);
        m_previousBundlePositionBR.push_back(v);

        // incoming bundles
        double itv = 2.0/m_bundlesPreviousByLabel.size();
        {
          for (size_t j = 0; j < m_previousFrames[i]->getSizeOutBundles(); ++j)
          {
            ///*
            posTopLft.x = m_previousFramePositions[i].x + 1.0*m_verticalFrameScale + 3.0*pix;
            posTopLft.y = m_previousFramePositions[i].y
                          + 1.0*m_verticalFrameScale
                          - 0.5*itv*m_verticalFrameScale
                          - m_previousFrames[i]->getOutBundle(j)->getParent()->getIndex()*itv*m_verticalFrameScale;

            posBotRgt.x = m_previousBundleLabelPositionTL[ m_previousFrames[i]->getOutBundle(j)->getParent()->getIndex() ].x;
            posBotRgt.y = posTopLft.y;
            //*/
            /*
            posTopLft.x = posBdlLblGridPrevTopLft[ framesPrev[i]->getOutBundle(j)->getParent()->getIndex() ].x - 1.0*pix;
            posTopLft.y = posBotRgt.y = posFramesPrev[i].y
                + 1.0*scaleDgrmVert
                - 0.5*itv*scaleDgrmVert
                - framesPrev[i]->getOutBundle(j)->getParent()->getIndex()*itv*scaleDgrmVert;

            posBotRgt.x = posFrameCurr.x - 1.0*scaleDgrmHori - 3.0*pix;
            posBotRgt.y = posTopLft.y;
            */
            m_previousBundlePositionTL[i].push_back(posTopLft);
            m_previousBundlePositionBR[i].push_back(posBotRgt);
          }
        }
      }
    }
  }

  if (m_nextFramePositions.size() > 0 && m_bundlesNextByLabel.size() > 0)
  {
    // grid prev
    if (m_previousFramePositions.size() >= m_nextFramePositions.size())
    {
      posTopLft.y = m_previousFramePositions[0].y + 1.0*m_verticalFrameScale + 0.125*itvVert;
      posBotRgt.y = m_previousFramePositions[m_previousFramePositions.size()-1].y - 1.0*m_verticalFrameScale - 0.125*itvVert;
    }
    else
    {
      posTopLft.y = m_nextFramePositions[0].y + 1.0*m_verticalFrameScale + 0.125*itvVert;
      posBotRgt.y = m_nextFramePositions[m_nextFramePositions.size()-1].y - 1.0*m_verticalFrameScale - 0.125*itvVert;
    }

    // grid next
    double itvGrid = (1.5*itvHori)/(m_bundlesNextByLabel.size()+1);

    {
      for (size_t i = 0; i < m_bundlesNextByLabel.size(); ++i)
      {
        posTopLft.x = 2.0*itvHori - (m_bundlesNextByLabel.size()-i)*itvGrid;
        posTopLft.y =  0.5*size.height() - labelHeight*pix;
        posBotRgt.x = posTopLft.x;
        posBotRgt.y = -0.5*size.height() + labelHeight*pix;

        m_nextBundleLabelPositionTL.push_back(posTopLft);
        m_nextBundleLabelPositionBR.push_back(posBotRgt);
      }
    }

    // bundles next
    {
      for (size_t i = 0; i < m_nextFrames.size(); ++i)
      {
        vector< Position2D > v;

        m_nextBundlePositionTL.push_back(v);
        m_nextBundlePositionBR.push_back(v);

        // outgoing bundles
        double itv = 2.0/m_bundlesNextByLabel.size();
        {
          for (size_t j = 0; j < m_nextFrames[i]->getSizeInBundles(); ++j)
          {
            posTopLft.x = m_nextBundleLabelPositionTL[ m_nextFrames[i]->getInBundle(j)->getParent()->getIndex() ].x + 1.0*pix;
            posTopLft.y = posBotRgt.y = m_nextFramePositions[i].y
                                        + 1.0*m_verticalFrameScale
                                        - 0.5*itv*m_verticalFrameScale
                                        - m_nextFrames[i]->getInBundle(j)->getParent()->getIndex()*itv*m_verticalFrameScale;

            posBotRgt.x = m_nextFramePositions[i].x - 1.0*m_verticalFrameScale - 3.0*pix;
            posBotRgt.y = posTopLft.y;

            m_nextBundlePositionTL[i].push_back(posTopLft);
            m_nextBundlePositionBR[i].push_back(posBotRgt);
          }
        }
      }
    }
  }
}


void Simulator::handleKeyUp()
{
  if (m_currentSelection == ID_FRAME_PREV)
  {
    if (0 < m_currentSelectionIndex)
    {
      --m_currentSelectionIndex;
    }

    m_lastSelectionIndexPrevious = m_currentSelectionIndex;
  }
  else if (m_currentSelection == ID_FRAME_NEXT)
  {
    if (0 < m_currentSelectionIndex)
    {
      --m_currentSelectionIndex;
    }

    m_lastSelectionIndexNext = m_currentSelectionIndex;
  }
}


void Simulator::handleKeyRgt()
{
  if (m_currentSelection == -1)
  {
    if (m_lastSelection != -1)
    {
      m_currentSelection = m_lastSelection;

      if (m_currentSelection == ID_FRAME_PREV)
      {
        if (0 <= m_lastSelectionIndexPrevious && static_cast <size_t>(m_lastSelectionIndexPrevious) < m_previousFrames.size())
        {
          m_currentSelectionIndex = m_lastSelectionIndexPrevious;
        }
        else
        {
          m_currentSelectionIndex = 0;
        }
      }
      else if (m_currentSelection == ID_FRAME_CURR)
      {
        m_currentSelectionIndex = 0;
      }
      else if (m_currentSelection == ID_FRAME_NEXT)
      {
        if (0 <= m_lastSelectionIndexNext && static_cast <size_t>(m_lastSelectionIndexNext) < m_nextFrames.size())
        {
          m_currentSelectionIndex = m_lastSelectionIndexNext;
        }
        else
        {
          m_currentSelectionIndex = 0;
        }
      }
    }
    else
    {
      m_currentSelection = ID_FRAME_CURR;
      m_currentSelectionIndex = 0;
    }
  }
  else if (m_currentSelection == ID_FRAME_PREV)
  {
    m_currentSelection = ID_FRAME_CURR;
    m_currentSelectionIndex = 0;
  }
  else if (m_currentSelection == ID_FRAME_CURR)
  {
    if (m_nextFrames.size() > 0)
    {
      m_currentSelection = ID_FRAME_NEXT;

      if (0 <= m_lastSelectionIndexNext && static_cast <size_t>(m_lastSelectionIndexNext) < m_nextFrames.size())
      {
        m_currentSelectionIndex = m_lastSelectionIndexNext;
      }
      else
      {
        m_currentSelectionIndex = 0;
      }

      m_lastSelectionIndexNext = m_currentSelectionIndex;
    }
  }
  else if (m_currentSelection == ID_FRAME_NEXT)
  {
    updateFrameCurr(
      new Cluster(*m_nextFrames[m_currentSelectionIndex]),
      m_nextFramePositions[m_currentSelectionIndex]);

    m_currentSelection = ID_FRAME_CURR;
    m_currentSelectionIndex = 0;
  }
}


void Simulator::handleKeyDwn()
{
  if (m_currentSelection == ID_FRAME_PREV)
  {
    if (static_cast <size_t>(m_currentSelectionIndex) < m_previousFrames.size()-1)
    {
      ++m_currentSelectionIndex;
    }

    m_lastSelectionIndexPrevious = m_currentSelectionIndex;
  }
  else if (m_currentSelection == ID_FRAME_NEXT)
  {
    if (static_cast <size_t>(m_currentSelectionIndex) < m_nextFrames.size()-1)
    {
      ++m_currentSelectionIndex;
    }

    m_lastSelectionIndexNext = m_currentSelectionIndex;
  }
}


void Simulator::handleKeyLft()
{
  if (m_currentSelection < 0)
  {
    if (m_lastSelection >= 0)
    {
      m_currentSelection = m_lastSelection;

      if (m_currentSelection == ID_FRAME_PREV)
      {
        if (0 <= m_lastSelectionIndexPrevious && static_cast <size_t>(m_lastSelectionIndexPrevious) < m_previousFrames.size())
        {
          m_currentSelectionIndex = m_lastSelectionIndexPrevious;
        }
        else
        {
          m_currentSelectionIndex = 0;
        }
      }
      else if (m_currentSelection == ID_FRAME_CURR)
      {
        m_currentSelectionIndex = 0;
      }
      else if (m_currentSelection == ID_FRAME_NEXT)
      {
        if (0 <= m_lastSelectionIndexNext && static_cast <size_t>(m_lastSelectionIndexNext) < m_nextFrames.size())
        {
          m_currentSelectionIndex = m_lastSelectionIndexNext;
        }
        else
        {
          m_currentSelectionIndex = 0;
        }
      }
    }
    else
    {
      m_currentSelection = ID_FRAME_CURR;
      m_currentSelectionIndex = 0;
    }
  }
  else if (m_currentSelection == ID_FRAME_PREV)
  {
    updateFrameCurr(
      new Cluster(*m_previousFrames[m_currentSelectionIndex]),
      m_previousFramePositions[m_currentSelectionIndex]);

    m_currentSelection = ID_FRAME_CURR;
    m_currentSelectionIndex = 0;
  }
  else if (m_currentSelection == ID_FRAME_CURR)
  {
    if (m_previousFrames.size() > 0)
    {
      m_currentSelection = ID_FRAME_PREV;

      if (0 <= m_lastSelectionIndexPrevious && static_cast <size_t>(m_lastSelectionIndexPrevious) < m_previousFrames.size())
      {
        m_currentSelectionIndex = m_lastSelectionIndexPrevious;
      }
      else
      {
        m_currentSelectionIndex = 0;
      }

      m_lastSelectionIndexPrevious = m_currentSelectionIndex;
    }
  }
  else if (m_currentSelection == ID_FRAME_NEXT)
  {
    m_currentSelection = ID_FRAME_CURR;
    m_currentSelectionIndex = 0;
  }
}


void Simulator::markFrameClusts()
{
  if (m_currentSelection == ID_FRAME_PREV)
  {
    if (0 <= m_currentSelectionIndex && static_cast <size_t>(m_currentSelectionIndex) < m_previousFrames.size())
    {
      mediator->handleMarkFrameClust(this);
      emit hoverCluster(m_previousFrames[m_currentSelectionIndex], QVector<Attribute *>::fromStdVector(m_attributes).toList());
    }
  }
  else if (m_currentSelection == ID_FRAME_CURR)
  {
    if (m_currentFrame != 0)
    {
      mediator->handleMarkFrameClust(this);
      emit hoverCluster(m_currentFrame, QVector<Attribute *>::fromStdVector(m_attributes).toList());
    }
  }
  else if (m_currentSelection == ID_FRAME_NEXT)
  {
    if (0 <= m_currentSelectionIndex && static_cast <size_t>(m_currentSelectionIndex) < m_nextFrames.size())
    {
      /*
      mediator->handleUnmarkFrameClusts();
      */
      mediator->handleMarkFrameClust(this);
      emit hoverCluster(m_previousFrames[m_currentSelectionIndex], QVector<Attribute *>::fromStdVector(m_attributes).toList());
    }
  }
}


void Simulator::clearAttributes()
{
  m_attributes.clear();
}


void Simulator::clearDiagram()
{
  m_diagram = 0;
}


void Simulator::clearFrames()
{
  if (m_currentFrame != 0)
  {
    delete m_currentFrame;
    m_currentFrame = 0;
  }

  {
    for (size_t i = 0; i < m_previousFrames.size(); ++i)
    {
      delete m_previousFrames[i];
    }
  }
  m_previousFrames.clear();

  {
    for (size_t i = 0; i < m_nextFrames.size(); ++i)
    {
      delete m_nextFrames[i];
    }
  }
  m_nextFrames.clear();

  m_previousFramePositions.clear();
  m_nextFramePositions.clear();
}


void Simulator::clearBundles()
{
  {
    for (size_t i = 0; i < m_bundles.size(); ++i)
    {
      delete m_bundles[i];
    }
    m_bundles.clear();
  }

  {
    for (size_t i = 0; i < m_bundlesPreviousByLabel.size(); ++i)
    {
      delete m_bundlesPreviousByLabel[i];
    }
  }
  m_bundlesPreviousByLabel.clear();

  {
    for (size_t i = 0; i < m_bundlesNextByLabel.size(); ++i)
    {
      delete m_bundlesNextByLabel[i];
    }
  }
  m_bundlesNextByLabel.clear();

  {
    for (size_t i = 0; i < m_bundlesByLabel.size(); ++i)
    {
      delete m_bundlesByLabel[i];
    }
  }
  m_bundlesByLabel.clear();

  m_previousBundleLabelPositionTL.clear();
  m_previousBundleLabelPositionBR.clear();
  m_nextBundleLabelPositionTL.clear();
  m_nextBundleLabelPositionBR.clear();

  m_previousBundlePositionTL.clear();
  m_previousBundlePositionBR.clear();
  m_nextBundlePositionTL.clear();
  m_nextBundlePositionBR.clear();
}


// -- hit detection ---------------------------------------------


void Simulator::handleHits(const vector< int > &ids)
{
  if (ids.size() == 1)
  {
    if (ids[0] == ID_CANVAS)
    {
      if (m_currentSelection != -1 || m_currentSelectionIndex != -1)
      {
        m_currentSelection = -1;
        m_currentSelectionIndex = -1;

        mediator->handleUnmarkFrameClusts(this);
        emit hoverCluster(0);
      }

      m_previousBundleFocusIndex = -1;
      m_nextBundleFocusIndex = -1;
    }
  }
  else if (ids.size() > 1)
  {
    if (m_lastMouseEvent.button() == Qt::LeftButton)
    {
      if (m_lastMouseEvent.type() == QEvent::MouseButtonPress)
      {
        if (ids[1] == ID_ICON_CLEAR && (m_previousFrames.size() > 0 || m_currentFrame != 0 || m_nextFrames.size() > 0))
        {
          mediator->handleClearSim(this);
        }
        else if (ids[1] == ID_ICON_UP)
        {
          handleKeyUp();
        }
        else if (ids[1] == ID_ICON_NEXT)
        {
          handleKeyRgt();
        }
        else if (ids[1] == ID_ICON_DOWN)
        {
          handleKeyDwn();
        }
        else if (ids[1] == ID_ICON_PREV)
        {
          handleKeyLft();
        }
      }
      else if (m_lastMouseEvent.type() == QEvent::MouseButtonDblClick)
      {
        if (ids[1] == ID_FRAME_PREV)
        {
          updateFrameCurr(new Cluster(*m_previousFrames[ids[2]]), m_previousFramePositions[ids[2]]);
        }
        else if (ids[1] == ID_FRAME_NEXT)
        {
          updateFrameCurr(new Cluster(*m_nextFrames[ids[2]]), m_nextFramePositions[ids[2]]);
        }
      }
    }

    if (ids[1] == ID_FRAME_CURR || ids[1] == ID_FRAME_PREV || ids[1] == ID_FRAME_NEXT)
    {
      if (m_lastMouseEvent.type() == QEvent::MouseButtonPress &&
          ((m_lastMouseEvent.button() == Qt::LeftButton && ids.size() > 3 && ids[3] == ID_DIAGRAM_MORE) ||
          m_lastMouseEvent.button() == Qt::RightButton))
      {
        m_currentSelection      = ids[1];

        if (ids[1] == ID_FRAME_PREV) m_lastSelectionIndexPrevious = m_currentSelectionIndex;
        if (ids[1] == ID_FRAME_NEXT) m_lastSelectionIndexNext = m_currentSelectionIndex;

        m_lastSelection         = m_currentSelection;
        m_currentSelectionIndex = ids[2];

        Cluster *frame = m_currentSelection == ID_FRAME_PREV ? m_previousFrames[m_currentSelectionIndex] :
                         m_currentSelection == ID_FRAME_NEXT ? m_nextFrames[m_currentSelectionIndex] :
                         m_currentFrame;
        emit routingCluster(frame, QList<Cluster *>(), QVector<Attribute *>::fromStdVector(m_attributes).toList());
      }
    }
    else if (ids[1] == ID_BUNDLE_LBL)
    {
      m_previousBundleFocusIndex = ids[2];
      m_nextBundleFocusIndex = ids[2];
    }

    markFrameClusts();
  }
}


void Simulator::processHits(
  GLint hits,
  GLuint buffer[])
{
  GLuint* ptr;
  vector< int > ids;

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


// -- utility drawing functions ---------------------------------


void Simulator::clear()
{
  VisUtils::clear(m_settings->backgroundColor.value());
}


QColor Simulator::calcColor(size_t iter, size_t numr)
{
  return VisUtils::qualPair(iter, numr);
}


void Simulator::drawFrameCurr(const bool& inSelectMode)
{
  if (inSelectMode)
  {
    if (m_currentFrame != 0)
    {
      double x = m_currentFramePosition.x;
      double y = m_currentFramePosition.y;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      glPushName(ID_FRAME_CURR);

      glPushName(0);
      VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);

      if (m_currentSelection == ID_FRAME_CURR && m_currentSelectionIndex == 0)
      {
        glPushName(ID_DIAGRAM_MORE);
        VisUtils::fillRect(-0.98, -0.8, -0.8, -0.98);
        glPopName();
      }

      glPopName();

      glPopName();

      glPopMatrix();
    }
  }
  else
  {
    double pix = pixelSize();
    vector< double > valsFrame;

    if (m_currentFrame != 0)
    {
      double x = m_currentFramePosition.x;
      double y = m_currentFramePosition.y;
      /*
      for ( int j = 0; j < attributes.size(); ++j )
          valsFrame.push_back(
              attributes[j]->mapToValue(
                  frameCurr->getNode(0)->getTupleVal(
                      attributes[j]->getIndex() ) )->getIndex() );
      */
      Attribute* attr;
      Node* node;
      for (size_t j = 0; j < m_attributes.size(); ++j)
      {
        attr = m_attributes[j];
        node = m_currentFrame->getNode(0);
        if (attr->getSizeCurValues() > 0)
        {
          valsFrame.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
        }
        else
        {
          double val = node->getTupleVal(attr->getIndex());
          valsFrame.push_back(val);
        }
      }
      attr = 0;
      node = 0;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      if (m_currentSelection == ID_FRAME_CURR)
      {
        VisUtils::setColor(SelectColor());
        VisUtils::fillRect(
          -1.0+4*pix/m_horizontalFrameScale,  1.0+4*pix/m_horizontalFrameScale,
          1.0-4*pix/m_horizontalFrameScale, -1.0-4*pix/m_horizontalFrameScale);
      }
      else
      {
        VisUtils::setColor(VisUtils::mediumGray);
        VisUtils::fillRect(
          -1.0+3*pix/m_horizontalFrameScale,  1.0+3*pix/m_horizontalFrameScale,
          1.0-3*pix/m_horizontalFrameScale, -1.0-3*pix/m_horizontalFrameScale);
      }
      m_diagram->visualize(
        inSelectMode,
        pixelSize(),
        m_attributes,
        valsFrame);

      if (m_currentSelection == ID_FRAME_CURR)
      {
        VisUtils::setColor(SelectColor());
        VisUtils::enableLineAntiAlias();
        VisUtils::setColor(SelectColor());
        VisUtils::fillMoreIcon(-0.98, -0.8, -0.8, -0.98);
        VisUtils::setColor(VisUtils::lightLightGray);
        VisUtils::drawMoreIcon(-0.98, -0.8, -0.8, -0.98);
        VisUtils::disableLineAntiAlias();
      }

      glPopMatrix();
    }

    valsFrame.clear();
  }
}


void Simulator::drawFramesPrev(const bool& inSelectMode)
{
  if (inSelectMode)
  {
    glPushName(ID_FRAME_PREV);
    for (size_t i = 0; i < m_previousFramePositions.size(); ++i)
    {
      double x = m_previousFramePositions[i].x;
      double y = m_previousFramePositions[i].y;

      glPushMatrix();
      glTranslatef(x, y, 0.0);

      if (m_currentSelection == ID_FRAME_PREV &&  static_cast <size_t>(m_currentSelectionIndex) == i)
      {
        glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);
      }
      else
      {
        glScalef(m_verticalFrameScale, m_verticalFrameScale, m_verticalFrameScale);
      }

      glPushName((GLuint) i);
      VisUtils::fillRect(
        -1.0,  1.0,
        1.0, -1.0);

      if (m_currentSelection == ID_FRAME_PREV &&  static_cast <size_t>(m_currentSelectionIndex) == i)
      {
        glPushName(ID_DIAGRAM_MORE);
        VisUtils::fillRect(-0.98, -0.8, -0.8, -0.98);
        glPopName();
      }

      glPopName();

      glPopMatrix();
    }
    glPopName();
  }
  else
  {
    double pix = pixelSize();
    vector< double > valsFrame;

    for (int i = 0; i < (int) m_previousFramePositions.size(); ++i)
    {
      if (m_currentSelection != ID_FRAME_PREV ||  i != m_currentSelectionIndex)
      {
        double x = m_previousFramePositions[i].x;
        double y = m_previousFramePositions[i].y;

        glPushMatrix();
        glTranslatef(x, y, 0.0);
        glScalef(m_verticalFrameScale, m_verticalFrameScale, m_verticalFrameScale);

        VisUtils::setColor(VisUtils::mediumGray);
        VisUtils::fillRect(
          -1.0+3*pix/m_verticalFrameScale,  1.0+3*pix/m_verticalFrameScale,
          1.0-3*pix/m_verticalFrameScale, -1.0-3*pix/m_verticalFrameScale);

        if (2.0*m_verticalFrameScale > 30.0*pix)
        {
          /*
          for ( int j = 0; j < attributes.size(); ++j )
              valsFrame.push_back(
                  attributes[j]->mapToValue(
                      framesPrev[i]->getNode(0)->getTupleVal(
                          attributes[j]->getIndex() ) )->getIndex() );
          */
          Attribute* attr;
          Node* node;
          for (size_t j = 0; j < m_attributes.size(); ++j)
          {
            attr = m_attributes[j];
            node = m_previousFrames[i]->getNode(0);
            if (attr->getSizeCurValues() > 0)
            {
              valsFrame.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
            }
            else
            {
              double val = node->getTupleVal(attr->getIndex());
              valsFrame.push_back(val);
            }
          }
          attr = 0;
          node = 0;

          m_diagram->visualize(
            inSelectMode,
            pixelSize(),
            m_attributes,
            valsFrame);
        }
        else
        {
          VisUtils::setColor(Qt::white);
          VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);
          VisUtils::setColor(VisUtils::mediumGray);
          VisUtils::drawRect(-1.0, 1.0, 1.0, -1.0);
        }

        glPopMatrix();

        valsFrame.clear();
      }
    }

    if (m_currentSelection == ID_FRAME_PREV)
    {
      if (0 <= m_currentSelectionIndex &&  static_cast <size_t>(m_currentSelectionIndex) < m_previousFramePositions.size())
      {
        /*
        for ( int j = 0; j < attributes.size(); ++j )
            valsFrame.push_back(
                attributes[j]->mapToValue(
                    framesPrev[focusFrameIdx]->getNode(0)->getTupleVal(
                        attributes[j]->getIndex() ) )->getIndex() );
        */
        Attribute* attr;
        Node* node;
        for (size_t j = 0; j < m_attributes.size(); ++j)
        {
          attr = m_attributes[j];
          node = m_previousFrames[m_currentSelectionIndex]->getNode(0);
          if (attr->getSizeCurValues() > 0)
          {
            valsFrame.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
          }
          else
          {
            double val = node->getTupleVal(attr->getIndex());
            valsFrame.push_back(val);
          }
        }
        attr = 0;
        node = 0;

        glPushMatrix();
        glTranslatef(
          m_previousFramePositions[m_currentSelectionIndex].x,
          m_previousFramePositions[m_currentSelectionIndex].y,
          0.0);
        glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

        VisUtils::setColor(SelectColor());
        VisUtils::fillRect(
          -1.0+4*pix/m_horizontalFrameScale,  1.0+4*pix/m_horizontalFrameScale,
          1.0-4*pix/m_horizontalFrameScale, -1.0-4*pix/m_horizontalFrameScale);
        m_diagram->visualize(
          inSelectMode,
          pixelSize(),
          m_attributes,
          valsFrame);

        VisUtils::enableLineAntiAlias();
        VisUtils::setColor(SelectColor());
        VisUtils::fillMoreIcon(-0.98, -0.8, -0.8, -0.98);
        VisUtils::setColor(VisUtils::lightLightGray);
        VisUtils::drawMoreIcon(-0.98, -0.8, -0.8, -0.98);
        VisUtils::disableLineAntiAlias();

        glPopMatrix();

        valsFrame.clear();
      }
    }
  }
}


void Simulator::drawFramesNext(const bool& inSelectMode)
{
  if (inSelectMode)
  {
    glPushName(ID_FRAME_NEXT);
    for (size_t i = 0; i < m_nextFramePositions.size(); ++i)
    {
      double x = m_nextFramePositions[i].x;
      double y = m_nextFramePositions[i].y;

      glPushMatrix();
      glTranslatef(x, y, 0.0);

      if (m_currentSelection == ID_FRAME_NEXT &&  static_cast <size_t>(m_currentSelectionIndex) == i)
      {
        glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);
      }
      else
      {
        glScalef(m_verticalFrameScale, m_verticalFrameScale, m_verticalFrameScale);
      }

      glPushName((GLuint) i);
      VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);

      if (m_currentSelection == ID_FRAME_NEXT &&  static_cast <size_t>(m_currentSelectionIndex) == i)
      {
        glPushName(ID_DIAGRAM_MORE);
        VisUtils::fillRect(-0.98, -0.8, -0.8, -0.98);
        glPopName();
      }

      glPopName();

      glPopMatrix();
    }
    glPopName();
  }
  else
  {
    double pix = pixelSize();
    vector< double > valsFrame;

    for (size_t i = 0; i < m_nextFramePositions.size(); ++i)
    {
      if (m_currentSelection != ID_FRAME_NEXT || i !=  static_cast <size_t>(m_currentSelectionIndex))
      {
        double x = m_nextFramePositions[i].x;
        double y = m_nextFramePositions[i].y;

        glPushMatrix();
        glTranslatef(x, y, 0.0);
        glScalef(m_verticalFrameScale, m_verticalFrameScale, m_verticalFrameScale);

        VisUtils::setColor(VisUtils::mediumGray);
        VisUtils::fillRect(
          -1.0+3*pix/m_verticalFrameScale,  1.0+3*pix/m_verticalFrameScale,
          1.0-3*pix/m_verticalFrameScale, -1.0-3*pix/m_verticalFrameScale);

        if (2.0*m_verticalFrameScale > 30.0*pix)
        {
          /*
          for ( int j = 0; j < attributes.size(); ++j )
              valsFrame.push_back(
                  attributes[j]->mapToValue(
                      framesNext[i]->getNode(0)->getTupleVal(
                          attributes[j]->getIndex() ) )->getIndex() );
          */
          Attribute* attr;
          Node* node;
          for (size_t j = 0; j < m_attributes.size(); ++j)
          {
            attr = m_attributes[j];
            node = m_nextFrames[i]->getNode(0);
            if (attr->getSizeCurValues() > 0)
            {
              valsFrame.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
            }
            else
            {
              double val = node->getTupleVal(attr->getIndex());
              valsFrame.push_back(val);
            }
          }
          attr = 0;
          node = 0;

          m_diagram->visualize(
            inSelectMode,
            pixelSize(),
            m_attributes,
            valsFrame);
        }
        else
        {
          VisUtils::setColor(Qt::white);
          VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);
          VisUtils::setColor(VisUtils::mediumGray);
          VisUtils::drawRect(-1.0, 1.0, 1.0, -1.0);
        }

        glPopMatrix();

        valsFrame.clear();
      }
    }

    if (m_currentSelection == ID_FRAME_NEXT)
    {
      if (0 <= m_currentSelectionIndex &&  static_cast <size_t>(m_currentSelectionIndex) < m_nextFramePositions.size())
      {
        /*
        for ( int j = 0; j < attributes.size(); ++j )
            valsFrame.push_back(
                attributes[j]->mapToValue(
                    framesNext[focusFrameIdx]->getNode(0)->getTupleVal(
                        attributes[j]->getIndex() ) )->getIndex() );
        */
        Attribute* attr;
        Node* node;
        for (size_t j = 0; j < m_attributes.size(); ++j)
        {
          attr = m_attributes[j];
          node = m_nextFrames[m_currentSelectionIndex]->getNode(0);
          if (attr->getSizeCurValues() > 0)
          {
            valsFrame.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
          }
          else
          {
            double val = node->getTupleVal(attr->getIndex());
            valsFrame.push_back(val);
          }
        }
        attr = 0;
        node = 0;

        glPushMatrix();
        glTranslatef(
          m_nextFramePositions[m_currentSelectionIndex].x,
          m_nextFramePositions[m_currentSelectionIndex].y,
          0.0);
        glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

        VisUtils::setColor(SelectColor());
        VisUtils::fillRect(
          -1.0+4*pix/m_horizontalFrameScale,  1.0+4*pix/m_horizontalFrameScale,
          1.0-4*pix/m_horizontalFrameScale, -1.0-4*pix/m_horizontalFrameScale);
        m_diagram->visualize(
          inSelectMode,
          pixelSize(),
          m_attributes,
          valsFrame);

        VisUtils::enableLineAntiAlias();
        VisUtils::setColor(SelectColor());
        VisUtils::fillMoreIcon(-0.98, -0.8, -0.8, -0.98);
        VisUtils::setColor(VisUtils::lightLightGray);
        VisUtils::drawMoreIcon(-0.98, -0.8, -0.8, -0.98);
        VisUtils::disableLineAntiAlias();

        glPopMatrix();

        valsFrame.clear();
      }
    }
  }
}


void Simulator::drawBdlLblGridPrev(const bool& inSelectMode)
{
  if (inSelectMode)
  {
    double pix = pixelSize();;
    string lbl;

    glPushName(ID_BUNDLE_LBL);
    for (size_t i = 0; i < m_previousBundleLabelPositionTL.size(); ++i)
    {
      lbl = m_bundlesPreviousByLabel[i]->getChild(0)->getEdge(0)->getLabel();

      glPushName((GLuint) m_bundlesPreviousByLabel[i]->getParent()->getIndex());

      glPushMatrix();
      glTranslatef(
        m_previousBundleLabelPositionTL[i].x,
        m_previousBundleLabelPositionTL[i].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::fillRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(m_settings->textSize.value()*pix/CHARHEIGHT),
        0.5*m_settings->textSize.value()*pix, -0.5*m_settings->textSize.value()*pix);

      glPopMatrix();

      glPushMatrix();
      glTranslatef(
        m_previousBundleLabelPositionBR[i].x,
        m_previousBundleLabelPositionBR[i].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(m_settings->textSize.value()*pix/CHARHEIGHT)),  0.0,
        0.5*m_settings->textSize.value()*pix,                                     -0.5*m_settings->textSize.value()*pix);

      glPopMatrix();

      VisUtils::drawLine(
        m_previousBundleLabelPositionTL[i].x,
        m_previousBundleLabelPositionBR[i].x,
        m_previousBundleLabelPositionTL[i].y,
        m_previousBundleLabelPositionBR[i].y);

      glPopName();
    }
    glPopName();
  }
  else
  {
    double pix = pixelSize();
    size_t idxHiLite = NON_EXISTING;

    for (size_t i = 0; i < m_previousBundleLabelPositionTL.size(); ++i)
    {
      if (m_bundlesPreviousByLabel[i]->getParent()->getIndex() == m_previousBundleFocusIndex)
      {
        idxHiLite = i;
      }
      else
      {
        string lbl = m_bundlesPreviousByLabel[i]->getChild(0)->getEdge(0)->getLabel();

        double txt = m_settings->textSize.value();

        glPushMatrix();
        glTranslatef(
          m_previousBundleLabelPositionTL[i].x,
          m_previousBundleLabelPositionTL[i].y,
          0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);

        VisUtils::setColor(m_settings->textColor.value());
        VisUtils::drawLabel(
          texCharId,
          0.0 + 3*pix,
          0.0,
          txt*pix/CHARHEIGHT,
          lbl);

        glPopMatrix();

        glPushMatrix();
        glTranslatef(
          m_previousBundleLabelPositionBR[i].x,
          m_previousBundleLabelPositionBR[i].y,
          0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);

        VisUtils::setColor(m_settings->textColor.value());
        VisUtils::drawLabelLeft(
          texCharId,
          0.0 - 3*pix,
          0.0,
          txt*pix/CHARHEIGHT,
          lbl);

        glPopMatrix();

        VisUtils::setColor(VisUtils::lightGray);
        VisUtils::drawLine(
          m_previousBundleLabelPositionTL[i].x,
          m_previousBundleLabelPositionBR[i].x,
          m_previousBundleLabelPositionTL[i].y,
          m_previousBundleLabelPositionBR[i].y);
      }
    }

    if (idxHiLite != NON_EXISTING &&  static_cast <size_t>(idxHiLite) < m_previousBundleLabelPositionTL.size())
    {
      string lbl = m_bundlesPreviousByLabel[idxHiLite]->getChild(0)->getEdge(0)->getLabel();

      double txt = m_settings->textSize.value();
      txt += 1;

      QColor colLne = calcColor(m_bundlesPreviousByLabel[idxHiLite]->getParent()->getIndex(), m_bundlesByLabel.size() - 1);

      glPushMatrix();
      glTranslatef(
        m_previousBundleLabelPositionTL[idxHiLite].x,
        m_previousBundleLabelPositionTL[idxHiLite].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(m_settings->backgroundColor.value());
      VisUtils::fillRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(m_settings->textColor.value());
      VisUtils::drawLabel(
        texCharId,
        0.0 + 3*pix,
        0.0,
        txt*pix/CHARHEIGHT,
        lbl);

      glPopMatrix();

      glPushMatrix();
      glTranslatef(
        m_previousBundleLabelPositionBR[idxHiLite].x,
        m_previousBundleLabelPositionBR[idxHiLite].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(m_settings->backgroundColor.value());
      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(m_settings->textColor.value());
      VisUtils::drawLabelLeft(
        texCharId,
        0.0 - 3*pix,
        0.0,
        txt*pix/CHARHEIGHT,
        lbl);

      glPopMatrix();

      VisUtils::setColor(colLne);
      VisUtils::drawLine(
        m_previousBundleLabelPositionTL[idxHiLite].x,
        m_previousBundleLabelPositionBR[idxHiLite].x,
        m_previousBundleLabelPositionTL[idxHiLite].y,
        m_previousBundleLabelPositionBR[idxHiLite].y);
    }
  }
}


void Simulator::drawBdlLblGridNext(const bool& inSelectMode)
{
  if (inSelectMode)
  {
    double pix = pixelSize();;

    glPushName(ID_BUNDLE_LBL);
    for (size_t i = 0; i < m_nextBundleLabelPositionTL.size(); ++i)
    {
      string lbl = m_bundlesNextByLabel[i]->getChild(0)->getEdge(0)->getLabel();

      glPushName((GLuint) m_bundlesNextByLabel[i]->getParent()->getIndex());

      glPushMatrix();
      glTranslatef(
        m_nextBundleLabelPositionTL[i].x,
        m_nextBundleLabelPositionTL[i].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::fillRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(m_settings->textSize.value()*pix/CHARHEIGHT),
        0.5*m_settings->textSize.value()*pix, -0.5*m_settings->textSize.value()*pix);

      glPopMatrix();

      glPushMatrix();
      glTranslatef(
        m_nextBundleLabelPositionBR[i].x,
        m_nextBundleLabelPositionBR[i].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(m_settings->textSize.value()*pix/CHARHEIGHT)), 0.0,
        0.5*m_settings->textSize.value()*pix,                                    -0.5*m_settings->textSize.value()*pix);

      glPopMatrix();

      VisUtils::drawLine(
        m_nextBundleLabelPositionTL[i].x,
        m_nextBundleLabelPositionBR[i].x,
        m_nextBundleLabelPositionTL[i].y,
        m_nextBundleLabelPositionBR[i].y);

      glPopName();
    }
    glPopName();
  }
  else
  {
    double pix = pixelSize();;
    size_t idxHiLite = NON_EXISTING;

    for (size_t i = 0; i < m_nextBundleLabelPositionTL.size(); ++i)
    {
      if (m_bundlesNextByLabel[i]->getParent()->getIndex() == m_nextBundleFocusIndex)
      {
        idxHiLite = i;
      }
      else
      {
        string lbl = m_bundlesNextByLabel[i]->getChild(0)->getEdge(0)->getLabel();

        double txt = m_settings->textSize.value();

        glPushMatrix();
        glTranslatef(
          m_nextBundleLabelPositionTL[i].x,
          m_nextBundleLabelPositionTL[i].y,
          0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);

        VisUtils::setColor(m_settings->textColor.value());
        VisUtils::drawLabel(
          texCharId,
          0.0 + 3*pix,
          0.0,
          txt*pix/CHARHEIGHT,
          lbl);

        glPopMatrix();

        glPushMatrix();
        glTranslatef(
          m_nextBundleLabelPositionBR[i].x,
          m_nextBundleLabelPositionBR[i].y,
          0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);

        VisUtils::setColor(m_settings->textColor.value());
        VisUtils::drawLabelLeft(
          texCharId,
          0.0 - 3*pix,
          0.0,
          txt*pix/CHARHEIGHT,
          lbl);

        glPopMatrix();

        VisUtils::setColor(VisUtils::lightGray);
        VisUtils::drawLine(
          m_nextBundleLabelPositionTL[i].x,
          m_nextBundleLabelPositionBR[i].x,
          m_nextBundleLabelPositionTL[i].y,
          m_nextBundleLabelPositionBR[i].y);
      }
    }

    if (idxHiLite != NON_EXISTING &&  static_cast <size_t>(idxHiLite) < m_nextBundleLabelPositionTL.size())
    {
      string lbl = m_bundlesNextByLabel[idxHiLite]->getChild(0)->getEdge(0)->getLabel();

      double txt = m_settings->textSize.value();
      txt += 1;

      QColor colLne = calcColor(m_bundlesNextByLabel[idxHiLite]->getParent()->getIndex(), m_bundlesByLabel.size());

      glPushMatrix();
      glTranslatef(
        m_nextBundleLabelPositionTL[idxHiLite].x,
        m_nextBundleLabelPositionTL[idxHiLite].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(m_settings->backgroundColor.value());
      VisUtils::fillRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(m_settings->textColor.value());
      VisUtils::drawLabel(
        texCharId,
        0.0 + 3*pix,
        0.0,
        txt*pix/CHARHEIGHT,
        lbl);

      glPopMatrix();

      glPushMatrix();
      glTranslatef(
        m_nextBundleLabelPositionBR[idxHiLite].x,
        m_nextBundleLabelPositionBR[idxHiLite].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(m_settings->backgroundColor.value());
      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(m_settings->textColor.value());
      VisUtils::drawLabelLeft(
        texCharId,
        0.0 - 3*pix,
        0.0,
        txt*pix/CHARHEIGHT,
        lbl);

      glPopMatrix();

      VisUtils::setColor(colLne);
      VisUtils::drawLine(
        m_nextBundleLabelPositionTL[idxHiLite].x,
        m_nextBundleLabelPositionBR[idxHiLite].x,
        m_nextBundleLabelPositionTL[idxHiLite].y,
        m_nextBundleLabelPositionBR[idxHiLite].y);
    }
  }
}


void Simulator::drawBundlesPrev(const bool& inSelectMode)
{
  if (inSelectMode)
  {
    double pix = pixelSize();

    glPushName(ID_BUNDLE_LBL);
    for (size_t i = 0; i < m_previousBundlePositionTL.size(); ++i)
    {
      for (size_t j = 0; j < m_previousBundlePositionBR[i].size(); ++j)
      {
        glPushName((GLuint) m_previousFrames[i]->getOutBundle(j)->getParent()->getParent()->getIndex());
        // arrow interval
        double arrowItv = 3;

        // draw
        VisUtils::fillArrow(
          m_previousBundlePositionTL[i][j].x, m_previousBundlePositionBR[i][j].x,
          m_previousBundlePositionTL[i][j].y, m_previousBundlePositionBR[i][j].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix);

        glPopName();
      }
    }
    glPopName();
  }
  else
  {
    double pix = pixelSize();
    size_t idxHiLite = NON_EXISTING;

    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::enableLineAntiAlias();

    for (size_t i = 0; i < m_previousBundlePositionTL.size(); ++i)
    {
      idxHiLite = -1;
      for (size_t j = 0; j < m_previousBundlePositionBR[i].size(); ++j)
      {
        // fade color
        if (m_previousFrames[i]->getOutBundle(j)->getParent()->getParent()->getIndex() == m_previousBundleFocusIndex)
        {
          idxHiLite = j;
        }
        else
        {
          // arrow interval
          double arrowItv = 3;

          QColor colFill = calcColor(m_previousFrames[i]->getOutBundle(j)->getParent()->getParent()->getIndex(), m_bundlesByLabel.size());
          // draw
          VisUtils::fillArrow(
            m_previousBundlePositionTL[i][j].x, m_previousBundlePositionBR[i][j].x,
            m_previousBundlePositionTL[i][j].y, m_previousBundlePositionBR[i][j].y,
            arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
            alpha(colFill, 0.2), colFill);
          VisUtils::drawArrow(
            m_previousBundlePositionTL[i][j].x, m_previousBundlePositionBR[i][j].x,
            m_previousBundlePositionTL[i][j].y, m_previousBundlePositionBR[i][j].y,
            arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
            alpha(VisUtils::mediumGray, 0.2), VisUtils::mediumGray);
        }
      }

      if (idxHiLite != NON_EXISTING &&  static_cast <size_t>(idxHiLite) < m_previousBundlePositionBR[i].size())
      {
        // fill color
        QColor colFill = calcColor(m_previousFrames[i]->getOutBundle(idxHiLite)->getParent()->getParent()->getIndex(), m_bundlesByLabel.size());
        // arrow interva
        double arrowItv = 3;
        arrowItv += 1;

        // draw
        VisUtils::fillArrow(
          m_previousBundlePositionTL[i][idxHiLite].x, m_previousBundlePositionBR[i][idxHiLite].x,
          m_previousBundlePositionTL[i][idxHiLite].y, m_previousBundlePositionBR[i][idxHiLite].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
          colFill, colFill);
        VisUtils::drawArrow(
          m_previousBundlePositionTL[i][idxHiLite].x, m_previousBundlePositionBR[i][idxHiLite].x,
          m_previousBundlePositionTL[i][idxHiLite].y, m_previousBundlePositionBR[i][idxHiLite].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
          VisUtils::mediumGray, VisUtils::mediumGray);
      }
    }

    VisUtils::disableLineAntiAlias();
  }
}


void Simulator::drawBundlesNext(const bool& inSelectMode)
{
  if (inSelectMode)
  {
    double pix = pixelSize();
    double arrowItv;

    glPushName(ID_BUNDLE_LBL);
    for (size_t i = 0; i < m_nextBundlePositionTL.size(); ++i)
    {
      for (size_t j = 0; j < m_nextBundlePositionBR[i].size(); ++j)
      {
        glPushName((GLuint) m_nextFrames[i]->getInBundle(j)->getParent()->getParent()->getIndex());
        // arrow interval
        arrowItv = 3;

        // draw
        VisUtils::fillArrow(
          m_nextBundlePositionTL[i][j].x, m_nextBundlePositionBR[i][j].x,
          m_nextBundlePositionTL[i][j].y, m_nextBundlePositionBR[i][j].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix);

        glPopName();
      }
    }
    glPopName();
  }
  else
  {
    double pix = pixelSize();
    size_t idxHiLite = NON_EXISTING;

    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::enableLineAntiAlias();

    for (size_t i = 0; i < m_nextBundlePositionTL.size(); ++i)
    {
      idxHiLite = -1;

      for (size_t j = 0; j < m_nextBundlePositionBR[i].size(); ++j)
      {
        // fade color
        if (m_nextFrames[i]->getInBundle(j)->getParent()->getParent()->getIndex() == m_nextBundleFocusIndex)
        {
          idxHiLite = j;
        }
        else
        {
          // fill color
          QColor colFill = calcColor(m_nextFrames[i]->getInBundle(j)->getParent()->getParent()->getIndex(), m_bundlesByLabel.size());

          // arrow interva
          double arrowItv = 3;
          if (m_nextFrames[i]->getInBundle(j)->getParent()->getParent()->getIndex() == m_nextBundleFocusIndex)
          {
            arrowItv += 1;
          }

          // draw
          VisUtils::fillArrow(
            m_nextBundlePositionTL[i][j].x-pix, m_nextBundlePositionBR[i][j].x,
            m_nextBundlePositionTL[i][j].y, m_nextBundlePositionBR[i][j].y,
            arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
            alpha(colFill, 0.2), colFill);
          VisUtils::drawArrow(
            m_nextBundlePositionTL[i][j].x-pix, m_nextBundlePositionBR[i][j].x,
            m_nextBundlePositionTL[i][j].y, m_nextBundlePositionBR[i][j].y,
            arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
            alpha(VisUtils::mediumGray, 0.2), VisUtils::mediumGray);
        }
      }

      if (idxHiLite != NON_EXISTING &&  static_cast <size_t>(idxHiLite) < m_nextBundlePositionBR[i].size())
      {
        // fill color
        QColor colFill = calcColor(m_nextFrames[i]->getInBundle(idxHiLite)->getParent()->getParent()->getIndex(), m_bundlesByLabel.size());

        // arrow interva
        double arrowItv = 3;
        arrowItv += 1;

        // draw
        VisUtils::fillArrow(
          m_nextBundlePositionTL[i][idxHiLite].x-pix, m_nextBundlePositionBR[i][idxHiLite].x,
          m_nextBundlePositionTL[i][idxHiLite].y, m_nextBundlePositionBR[i][idxHiLite].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
          colFill, colFill);
        VisUtils::drawArrow(
          m_nextBundlePositionTL[i][idxHiLite].x-pix, m_nextBundlePositionBR[i][idxHiLite].x,
          m_nextBundlePositionTL[i][idxHiLite].y, m_nextBundlePositionBR[i][idxHiLite].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
          VisUtils::mediumGray, VisUtils::mediumGray);
      }
    }
    VisUtils::disableLineAntiAlias();
  }
}


void Simulator::drawControls(const bool& inSelectMode)
{
  QSizeF size = worldSize();
  double pix = pixelSize();

  double itvSml = 6.0*pix;
  double itvLrg = 9.0*pix;

  if (inSelectMode)
  {
    // clear icon
    double x = 0.5*size.width() - itvSml - pix;
    double y = 0.5*size.height() - itvSml - pix;
    glPushName(ID_ICON_CLEAR);
    VisUtils::fillRect(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    glPopName();

    // up arrow
    x =  0.0;
    y = -0.5*size.height() + 3.0*itvLrg + 2.0*pix + 4*pix;
    glPushName(ID_ICON_UP);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    glPopName();

    // right arrow
    x =  0.0 + 2*itvLrg + 4.0*pix;
    y = -0.5*size.height() + 1.0*itvLrg + 2.0*pix;
    glPushName(ID_ICON_NEXT);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    glPopName();

    // down arrow
    x =  0.0;
    y = -0.5*size.height() + 1.0*itvLrg + 2.0*pix;
    glPushName(ID_ICON_DOWN);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    glPopName();

    // left arrow
    x =  0.0 - 2.0*itvLrg - 4.0*pix;
    y = -0.5*size.height() + 1.0*itvLrg + 2.0*pix;
    glPushName(ID_ICON_PREV);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    glPopName();
  }
  else
  {
    VisUtils::enableLineAntiAlias();

    // clear icon
    double x = 0.5*size.width() - itvSml - pix;
    double y = 0.5*size.height() - itvSml - pix;
    VisUtils::setColor(Qt::white);
    VisUtils::fillClearIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColor(VisUtils::darkGray);
    VisUtils::drawClearIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    // up arrow
    x =  0.0;
    y = -0.5*size.height() + 3.0*itvLrg + 2.0*pix + 4*pix;
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::drawRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y+itvLrg+pix);
    VisUtils::drawLine(x-itvLrg-pix, x-itvLrg-pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::darkGray);
    VisUtils::drawLine(x-itvLrg-pix, x+itvLrg+pix, y-itvLrg-pix, y-itvLrg-pix);
    VisUtils::drawLine(x+itvLrg+pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::coolGreen);
    VisUtils::fillUpIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColor(VisUtils::lightLightGray);
    VisUtils::drawUpIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    // right arrow
    x =  0.0 + 2*itvLrg + 4.0*pix;
    y = -0.5*size.height() + 1.0*itvLrg + 2.0*pix;
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::drawRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y+itvLrg+pix);
    VisUtils::drawLine(x-itvLrg-pix, x-itvLrg-pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::darkGray);
    VisUtils::drawLine(x-itvLrg-pix, x+itvLrg+pix, y-itvLrg-pix, y-itvLrg-pix);
    VisUtils::drawLine(x+itvLrg+pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::coolGreen);
    VisUtils::fillNextIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColor(VisUtils::lightLightGray);
    VisUtils::drawNextIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    // down arrow
    x =  0.0;
    y = -0.5*size.height() + 1.0*itvLrg + 2.0*pix;
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::drawRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y+itvLrg+pix);
    VisUtils::drawLine(x-itvLrg-pix, x-itvLrg-pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::darkGray);
    VisUtils::drawLine(x-itvLrg-pix, x+itvLrg+pix, y-itvLrg-pix, y-itvLrg-pix);
    VisUtils::drawLine(x+itvLrg+pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::coolGreen);
    VisUtils::fillDownIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColor(VisUtils::lightLightGray);
    VisUtils::drawDownIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    // left arrow
    x =  0.0 - 2.0*itvLrg - 4.0*pix;
    y = -0.5*size.height() + 1.0*itvLrg + 2.0*pix;
    VisUtils::setColor(Qt::white);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::drawRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y+itvLrg+pix);
    VisUtils::drawLine(x-itvLrg-pix, x-itvLrg-pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::darkGray);
    VisUtils::drawLine(x-itvLrg-pix, x+itvLrg+pix, y-itvLrg-pix, y-itvLrg-pix);
    VisUtils::drawLine(x+itvLrg+pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColor(VisUtils::coolGreen);
    VisUtils::fillPrevIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColor(VisUtils::lightLightGray);
    VisUtils::drawPrevIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    VisUtils::disableLineAntiAlias();
  }
}


void Simulator::animate()
{
  vector< double > valsFrame;

  if (m_animationOldFrame != 0)
  {
    if (m_currentAnimationPhase == ANIM_POS)
    {
      // 'new' current frame
      double x = m_animationCurrentPosition.x;
      double y = m_animationCurrentPosition.y;
      /*
      {
      for ( int j = 0; j < attributes.size(); ++j )
          valsFrame.push_back(
              attributes[j]->mapToValue(
                  keyFrameFr->getNode(0)->getTupleVal(
                      attributes[j]->getIndex() ) )->getIndex() );
      }
      */
      Attribute* attr;
      Node* node;
      for (size_t j = 0; j < m_attributes.size(); ++j)
      {
        attr = m_attributes[j];
        node = m_animationOldFrame->getNode(0);
        if (attr->getSizeCurValues() > 0)
        {
          valsFrame.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
        }
        else
        {
          double val = node->getTupleVal(attr->getIndex());
          valsFrame.push_back(val);
        }
      }
      attr = 0;
      node = 0;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      m_diagram->visualize(
        false,
        pixelSize(),
        m_attributes,
        valsFrame);

      glPopMatrix();

      // 'old' current frame
      x = m_animationEndPosition.x;
      y = m_animationEndPosition.y;

      valsFrame.clear();
      /*
      {
      for ( int j = 0; j < attributes.size(); ++j )
          valsFrame.push_back(
              attributes[j]->mapToValue(
                  keyFrameTo->getNode(0)->getTupleVal(
                      attributes[j]->getIndex() ) )->getIndex() );
      }
      */
      for (size_t j = 0; j < m_attributes.size(); ++j)
      {
        attr = m_attributes[j];
        node = m_animationNewFrame->getNode(0);
        if (attr->getSizeCurValues() > 0)
        {
          valsFrame.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
        }
        else
        {
          double val = node->getTupleVal(attr->getIndex());
          valsFrame.push_back(val);
        }
      }
      attr = 0;
      node = 0;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      m_diagram->visualize(
        false,
        pixelSize(),
        m_attributes,
        valsFrame);

      glPopMatrix();
    }
    else if (m_currentAnimationPhase == ANIM_BLEND)
    {
      // 'new' current frame
      double x = m_animationCurrentPosition.x;
      double y = m_animationCurrentPosition.y;
      /*
      {
      for ( int j = 0; j < attributes.size(); ++j )
          valsFrame.push_back(
              attributes[j]->mapToValue(
                  keyFrameFr->getNode(0)->getTupleVal(
                      attributes[j]->getIndex() ) )->getIndex() );
      }
      */
      Attribute* attr;
      Node* node;
      for (size_t j = 0; j < m_attributes.size(); ++j)
      {
        attr = m_attributes[j];
        node = m_animationOldFrame->getNode(0);
        if (attr->getSizeCurValues() > 0)
        {
          valsFrame.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
        }
        else
        {
          double val = node->getTupleVal(attr->getIndex());
          valsFrame.push_back(val);
        }
      }
      attr = 0;
      node = 0;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      m_diagram->visualize(
        false,
        pixelSize(),
        m_attributes,
        valsFrame);

      glPopMatrix();

      // 'old' current frame
      x = m_animationEndPosition.x;
      y = m_animationEndPosition.y;

      valsFrame.clear();
      /*
      {
      for ( int j = 0; j < attributes.size(); ++j )
          valsFrame.push_back(
              attributes[j]->mapToValue(
                  keyFrameTo->getNode(0)->getTupleVal(
                      attributes[j]->getIndex() ) )->getIndex() );
      }
      */
      for (size_t j = 0; j < m_attributes.size(); ++j)
      {
        attr = m_attributes[j];
        node = m_animationNewFrame->getNode(0);
        if (attr->getSizeCurValues() > 0)
        {
          valsFrame.push_back(attr->mapToValue(node->getTupleVal(attr->getIndex()))->getIndex());
        }
        else
        {
          double val = node->getTupleVal(attr->getIndex());
          valsFrame.push_back(val);
        }
      }
      attr = 0;
      node = 0;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      m_diagram->visualize(
        false,
        pixelSize(),
        m_attributes,
        valsFrame,
        m_animationNewFrameOpacity);

      glPopMatrix();
    }
  }

  valsFrame.clear();
}


// -- utility event handlers ------------------------------------


void Simulator::onTimer()
{
  if (m_totalBlendTime >= m_totalAnimationTime)
  {
    if (m_currentAnimationPhase == ANIM_POS)
    {
      m_animationCurrentPosition.x = m_animationEndPosition.x;
      m_animationCurrentPosition.y = m_animationEndPosition.y;

      if (m_settings->blendType.value() != VisUtils::BLEND_HARD)
      {
        m_currentAnimationPhase = ANIM_BLEND;

        m_totalAnimationTime = 900;
        m_totalBlendTime = 0;

        m_animationOldFrameOpacity = 1.0;
        m_animationNewFrameOpacity = 0.0;
      }
      else
      {
        m_animationTimer.stop();
        setMouseTracking(true);
        m_currentAnimationPhase = ANIM_NONE;

        // update new data
        m_currentFrame = m_animationOldFrame;

        m_animationOldFrame = 0;
        delete m_animationNewFrame;
        m_animationNewFrame = 0;

        initFramesPrevNext();
        initBundles();
        sortFramesPrevNext();

        // init & visualize
        dataChanged = true;
        update();
      }
    }
    else if (m_currentAnimationPhase == ANIM_BLEND)
    {
      m_animationTimer.stop();
      setMouseTracking(true);
      m_currentAnimationPhase = ANIM_NONE;

      // update new data
      m_currentFrame = m_animationOldFrame;

      m_animationOldFrame = 0;
      delete m_animationNewFrame;
      m_animationNewFrame = 0;

      initFramesPrevNext();
      initBundles();
      sortFramesPrevNext();

      // init & visualize
      dataChanged = true;
      update();
    }
  }
  else
  {
    double a = m_totalBlendTime/m_totalAnimationTime;

    if (m_currentAnimationPhase == ANIM_POS)
    {
      m_animationCurrentPosition.x = (1-a)*m_animationStartPosition.x + a*m_animationEndPosition.x;
      m_animationCurrentPosition.y = (1-a)*m_animationStartPosition.y + a*m_animationEndPosition.y;
    }
    else if (m_currentAnimationPhase == ANIM_BLEND)
    {
      if (m_settings->blendType.value() == VisUtils::BLEND_LINEAR)
      {
        m_animationOldFrameOpacity = (1-a)*0.0 + a*1.0;
        m_animationNewFrameOpacity = (1-a)*1.0 + a*0.0;
      }
      else if (m_settings->blendType.value() == VisUtils::BLEND_CONCAVE)
      {
        m_animationOldFrameOpacity = (a*1.0) * (a*1.0);
        m_animationNewFrameOpacity = 1.0 - m_animationOldFrameOpacity;
      }
      else if (m_settings->blendType.value() == VisUtils::BLEND_CONVEX)
      {
        m_animationOldFrameOpacity = sin(a*(PI/2.0));
        m_animationNewFrameOpacity = 1.0 - m_animationOldFrameOpacity;
      }
      else if (m_settings->blendType.value() == VisUtils::BLEND_OSCILLATE)
      {
        m_animationOldFrameOpacity = (-1.0*(cos(a*3.0*PI)/2.0) + 0.5);
        m_animationNewFrameOpacity = 1.0 - m_animationOldFrameOpacity;
      }
    }

    m_totalBlendTime += timerInterval;
    update();
  }
}

// -- end -----------------------------------------------------------
