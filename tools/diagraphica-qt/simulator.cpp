// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./simulator.cpp

#include "wx.hpp" // precompiled headers

#include "simulator.h"

using namespace std;

// -- static variables ----------------------------------------------


QColor  Simulator::m_clearColor             = Qt::white;
QColor  Simulator::m_textColor              = Qt::black;
int     Simulator::m_textSize               = 12;
QColor  Simulator::m_selectColor            = VisUtils::coolGreen;

int     Simulator::m_blendType              = VisUtils::BLEND_HARD;

int     Simulator::m_labelHeight            = 40;
int     Simulator::m_timerInterval          = 10;
double  Simulator::m_animationPixelsPerMS   = 1.0;



// -- constructors and destructor -----------------------------------


Simulator::Simulator(
  Mediator* m,
  Graph* g,
  GLCanvas* c,
  QObject* parent)
  : Visualizer(m, g, c),
    QObject(parent)
{
  m_diagram   = NULL;
  m_currentFrame = NULL;

  m_currentSelection = -1;
  m_currentSelectionIndex = -1;

  m_lastSelection     = -1;
  m_lastSelectionIndexPrevious = -1;
  m_lastSelectionIndexNext = -1;

  m_previousBundleFocusIndex  = -1;
  m_nextBundleFocusIndex  = -1;

  connect(&m_animationTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
  m_animationTimer.start(m_timerInterval);
}


Simulator::~Simulator()
{
  graph = NULL;

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
    if (m_currentFrame != NULL)
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
  canvas->Refresh();
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

  double distPix = Utils::dist(xTo, yTo, xFr, yFr) / canvas->getPixelSize();
  m_totalAnimationTime = distPix/m_animationPixelsPerMS;
  m_totalBlendTime = 0.0;

  m_currentAnimationPhase = ANIM_POS;

  // clear previous data
  clearFrames();
  clearBundles();

  m_lastSelection     = ID_FRAME_CURR;
  m_lastSelectionIndexPrevious = -1;
  m_lastSelectionIndexNext = -1;

  m_animationTimer.start(m_timerInterval);
  canvas->disableMouseMotion();
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


void Simulator::handleSendDgrmSglToExnr()
{
  if (m_currentSelection == ID_FRAME_PREV)
  {
    if (0 <= m_currentSelectionIndex && static_cast <size_t>(m_currentSelectionIndex) < m_previousFrames.size())
      mediator->addToExaminer(
        m_previousFrames[m_currentSelectionIndex],
        m_attributes);
  }
  else if (m_currentSelection == ID_FRAME_CURR)
  {
    if (m_currentSelectionIndex == 0)
      mediator->addToExaminer(
        m_currentFrame,
        m_attributes);
  }
  else if (m_currentSelection == ID_FRAME_NEXT)
  {
    if (0 <= m_currentSelectionIndex && static_cast <size_t>(m_currentSelectionIndex) < m_nextFrames.size())
      mediator->addToExaminer(
        m_nextFrames[m_currentSelectionIndex],
        m_attributes);
  }
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
      double wth, hgt;
      canvas->getSize(wth, hgt);

      GLint hits = 0;
      GLuint selectBuf[512];
      startSelectMode(
        hits,
        selectBuf,
        2.0,
        2.0);

      glPushName(ID_CANVAS);
      VisUtils::fillRect(-0.5*wth, 0.5*wth, 0.5*hgt, -0.5*hgt);

      drawBdlLblGridPrev(inSelectMode);
      drawBdlLblGridNext(inSelectMode);
      drawBundlesPrev(inSelectMode);
      drawBundlesNext(inSelectMode);
      drawFrameCurr(inSelectMode);
      drawFramesPrev(inSelectMode);
      drawFramesNext(inSelectMode);
      if (m_previousFrames.size() > 0 || m_currentFrame != NULL || m_nextFrames.size() > 0)
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
      if (m_previousFrames.size() > 0 || m_currentFrame != NULL || m_nextFrames.size() > 0)
      {
        drawControls(inSelectMode);
      }
    }
  }
}


// -- event handlers ------------------------------------------------


void Simulator::handleMouseLftDownEvent(
  const int& x,
  const int& y)
{
  Visualizer::handleMouseLftDownEvent(x, y);

  // redraw in select mode
  visualize(true);
  // redraw in render mode
  visualize(false);
}


void Simulator::handleMouseLftUpEvent(
  const int& x,
  const int& y)
{
  Visualizer::handleMouseLftUpEvent(x, y);

  // redraw in select mode
  visualize(true);
  // redraw in render mode
  visualize(false);
}


void Simulator::handleMouseLftDClickEvent(
  const int& x,
  const int& y)
{
  Visualizer::handleMouseLftDClickEvent(x, y);

  // redraw in select mode
  visualize(true);
  // redraw in render mode
  visualize(false);
}


void Simulator::handleMouseRgtDownEvent(
  const int& x,
  const int& y)
{
  Visualizer::handleMouseRgtDownEvent(x, y);

  // redraw in select mode
  visualize(true);
  // redraw in render mode
  visualize(false);
}


void Simulator::handleMouseRgtUpEvent(
  const int& x,
  const int& y)
{
  Visualizer::handleMouseRgtUpEvent(x, y);

  // redraw in select mode
  visualize(true);
  // redraw in render mode
  visualize(false);
}


void Simulator::handleMouseMotionEvent(
  const int& x,
  const int& y)
{
  Visualizer::handleMouseMotionEvent(x, y);

  // redraw in select mode
  visualize(true);
  // redraw in render mode
  visualize(false);
}


void Simulator::handleMouseLeaveEvent()
{
  Visualizer::initMouse();

  if (!showMenu)
  {
    m_currentSelection = -1;
    m_currentSelectionIndex = -1;

    m_previousBundleFocusIndex = -1;
    m_nextBundleFocusIndex = -1;

    mediator->handleUnmarkFrameClusts(this);
    mediator->handleUnshowFrame();
  }
  else
  {
    showMenu = false;
  }

  // redraw in render mode
  visualize(false);
}


void Simulator::handleKeyDownEvent(const int& keyCode)
{
  if (m_animationTimer.isActive())
  {
    Visualizer::handleKeyDownEvent(keyCode);

    if (keyCodeDown == WXK_UP  || keyCodeDown == WXK_NUMPAD_UP)
    {
      handleKeyUp();
    }
    else if (keyCodeDown == WXK_RIGHT || keyCodeDown == WXK_NUMPAD_RIGHT)
    {
      handleKeyRgt();
    }
    else if (keyCodeDown == WXK_DOWN || keyCodeDown == WXK_NUMPAD_DOWN)
    {
      handleKeyDwn();
    }
    else if (keyCodeDown == WXK_LEFT || keyCodeDown == WXK_NUMPAD_LEFT)
    {
      handleKeyLft();
    }
    else if (keyCodeDown == WXK_ESCAPE)
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
      mediator->handleUnshowFrame();
    }

    markFrameClusts();

    // redraw in render mode
    visualize(false);
  }
}

/*
void Simulator::handleMarkFrameClust(
    DiagramChooser* dc,
    const int &idx )
{
    if ( dc == chsrCurr )
        mediator->handleMarkCurrFrameClust( idx );
}
*/

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
  temp = NULL;
  tempPrev.clear();
  tempNext.clear();
  delete nodesPrev;
  nodesPrev = NULL;
  delete nodesNext;
  nodesNext = NULL;
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
  clst = NULL;
  edge = NULL;
  currNodes.clear();
  bdls.clear();
  bdl = NULL;

  lbls.clear();
  bdlLbls = NULL;
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

  // get canvas info & calc intervals
  double wthCvs, hgtCvs;
  canvas->getSize(wthCvs, hgtCvs);
  double pix = canvas->getPixelSize();
  double itvHori = wthCvs/6;
  double itvVert = (hgtCvs-itvHori)/Utils::maxx(1, Utils::maxx(m_previousFrames.size(), m_nextFrames.size()));
  m_horizontalFrameScale = 0.5*itvHori;
  m_verticalFrameScale = Utils::minn(m_horizontalFrameScale, 0.45*itvVert);

  // calc new positions
  pos.x = 0;
  pos.y = 0;
  m_currentFramePosition = pos;

  pos.x = -0.5*wthCvs + 0.5*itvHori + 4.0*pix;
  pos.y = 0.5*m_previousFrames.size()*itvVert - 0.5*itvVert;
  {
    for (size_t i = 0; i < m_previousFrames.size(); ++i)
    {
      m_previousFramePositions.push_back(pos);
      pos.y -= itvVert;
    }
  }

  pos.x = 0.5*wthCvs - 0.5*itvHori - 4.0*pix;
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

  // get canvas info & calc intervals
  double wthCvs, hgtCvs;
  canvas->getSize(wthCvs, hgtCvs);
  double pix = canvas->getPixelSize();
  double itvHori = wthCvs/6;
  double itvVert = (hgtCvs-itvHori)/Utils::maxx(1, Utils::maxx(m_previousFrames.size(), m_nextFrames.size()));

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
        posTopLft.y =  0.5*hgtCvs - m_labelHeight*pix;
        posBotRgt.x =  posTopLft.x;
        posBotRgt.y = -0.5*hgtCvs + m_labelHeight*pix;

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
        posTopLft.y =  0.5*hgtCvs - m_labelHeight*pix;
        posBotRgt.x = posTopLft.x;
        posBotRgt.y = -0.5*hgtCvs + m_labelHeight*pix;

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
      /*
      mediator->handleUnmarkFrameClusts();
      */
      mediator->handleMarkFrameClust(this);

      mediator->handleShowFrame(
        m_previousFrames[m_currentSelectionIndex],
        m_attributes,
        m_selectColor);
    }
  }
  else if (m_currentSelection == ID_FRAME_CURR)
  {
    if (m_currentFrame != NULL)
    {
      /*
      mediator->handleUnmarkFrameClusts();
      */
      mediator->handleMarkFrameClust(this);

      mediator->handleShowFrame(
        m_currentFrame,
        m_attributes,
        m_selectColor);
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

      mediator->handleShowFrame(
        m_nextFrames[m_currentSelectionIndex],
        m_attributes,
        m_selectColor);
    }
  }
}


void Simulator::clearAttributes()
{
  m_attributes.clear();
}


void Simulator::clearDiagram()
{
  m_diagram = NULL;
}


void Simulator::clearFrames()
{
  if (m_currentFrame != NULL)
  {
    delete m_currentFrame;
    m_currentFrame = NULL;
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
        mediator->handleUnshowFrame();
      }

      m_previousBundleFocusIndex = -1;
      m_nextBundleFocusIndex = -1;
    }
  }
  else if (ids.size() > 1)
  {
    if (ids[1] == ID_ICON_CLEAR &&
        (mouseSide == MSE_SIDE_LFT && mouseButton == MSE_BUTTON_DOWN))
    {
      if (m_previousFrames.size() > 0 || m_currentFrame != NULL || m_nextFrames.size() > 0)
      {
        mediator->handleClearSim(this);
      }
    }
    else if (ids[1] == ID_ICON_UP &&
             (mouseSide == MSE_SIDE_LFT && mouseButton == MSE_BUTTON_DOWN))
    {
      handleKeyUp();
    }
    else if (ids[1] == ID_ICON_NEXT &&
             (mouseSide == MSE_SIDE_LFT && mouseButton == MSE_BUTTON_DOWN))
    {
      handleKeyRgt();
    }
    else if (ids[1] == ID_ICON_DOWN &&
             (mouseSide == MSE_SIDE_LFT && mouseButton == MSE_BUTTON_DOWN))
    {
      handleKeyDwn();
    }
    else if (ids[1] == ID_ICON_PREV &&
             (mouseSide == MSE_SIDE_LFT && mouseButton == MSE_BUTTON_DOWN))
    {
      handleKeyLft();
    }
    else if (ids[1] == ID_FRAME_CURR)
    {
      m_currentSelection     = ID_FRAME_CURR;
      m_lastSelection = m_currentSelection;
      m_currentSelectionIndex     = ids[2];

      if (ids.size() > 3 &&
          (mouseSide = MSE_SIDE_LFT && mouseButton == MSE_BUTTON_DOWN))
      {
        if (ids[3] == ID_DIAGRAM_MORE)
        {
          showMenu = true;
          mediator->handleSendDgrm(this, false, false, false, true, false);

          // no mouseup event is generated reset manually
          mouseButton = MSE_BUTTON_UP;
          mouseSide   = MSE_SIDE_LFT;
          if (mouseClick != MSE_CLICK_DOUBLE)
          {
            mouseClick  = MSE_CLICK_SINGLE;
          }
          mouseDrag   = MSE_DRAG_FALSE;
        }
      }
      else if (mouseSide == MSE_SIDE_RGT && mouseButton == MSE_BUTTON_DOWN)
      {
        showMenu = true;
        mediator->handleSendDgrm(this, false, false, false, true, false);

        // no mouseup event is generated reset manually
        mouseButton = MSE_BUTTON_UP;
        mouseSide   = MSE_SIDE_RGT;
        if (mouseClick != MSE_CLICK_DOUBLE)
        {
          mouseClick  = MSE_CLICK_SINGLE;
        }
        mouseDrag   = MSE_DRAG_FALSE;
      }
    }
    else if (ids[1] == ID_FRAME_PREV)
    {
      m_currentSelection         = ID_FRAME_PREV;
      m_lastSelection     = m_currentSelection;
      m_currentSelectionIndex         = ids[2];
      m_lastSelectionIndexPrevious = m_currentSelectionIndex;

      if (ids.size() > 3 &&
          (mouseSide = MSE_SIDE_LFT && mouseButton == MSE_BUTTON_DOWN))
      {
        if (ids[3] == ID_DIAGRAM_MORE)
        {
          showMenu = true;
          mediator->handleSendDgrm(this, false, false, false, true, false);

          // no mouseup event is generated reset manually
          mouseButton = MSE_BUTTON_UP;
          mouseSide   = MSE_SIDE_LFT;
          if (mouseClick != MSE_CLICK_DOUBLE)
          {
            mouseClick  = MSE_CLICK_SINGLE;
          }
          mouseDrag   = MSE_DRAG_FALSE;
        }
      }
      else if (mouseSide == MSE_SIDE_RGT && mouseButton == MSE_BUTTON_DOWN)
      {
        showMenu = true;
        mediator->handleSendDgrm(this, false, false, false, true, false);

        // no mouseup event is generated reset manually
        mouseButton = MSE_BUTTON_UP;
        mouseSide   = MSE_SIDE_RGT;
        if (mouseClick != MSE_CLICK_DOUBLE)
        {
          mouseClick  = MSE_CLICK_SINGLE;
        }
        mouseDrag   = MSE_DRAG_FALSE;
      }
    }
    else if (ids[1] == ID_FRAME_NEXT)
    {
      m_currentSelection         = ID_FRAME_NEXT;
      m_lastSelection     = m_currentSelection;
      m_currentSelectionIndex         = ids[2];
      m_lastSelectionIndexNext = m_currentSelectionIndex;

      if (ids.size() > 3 &&
          (mouseSide = MSE_SIDE_LFT && mouseButton == MSE_BUTTON_DOWN))
      {
        if (ids[3] == ID_DIAGRAM_MORE)
        {
          showMenu = true;
          mediator->handleSendDgrm(this, false, false, false, true, false);

          // no mouseup event is generated reset manually
          mouseButton = MSE_BUTTON_UP;
          mouseSide   = MSE_SIDE_LFT;
          if (mouseClick != MSE_CLICK_DOUBLE)
          {
            mouseClick  = MSE_CLICK_SINGLE;
          }
          mouseDrag   = MSE_DRAG_FALSE;
        }
      }
      else if (mouseSide == MSE_SIDE_RGT && mouseButton == MSE_BUTTON_DOWN)
      {
        showMenu = true;
        mediator->handleSendDgrm(this, false, false, false, true, false);

        // no mouseup event is generated reset manually
        mouseButton = MSE_BUTTON_UP;
        mouseSide   = MSE_SIDE_RGT;
        if (mouseClick != MSE_CLICK_DOUBLE)
        {
          mouseClick  = MSE_CLICK_SINGLE;
        }
        mouseDrag   = MSE_DRAG_FALSE;
      }
    }
    else if (ids[1] == ID_BUNDLE_LBL)
    {
      m_previousBundleFocusIndex = ids[2];
      m_nextBundleFocusIndex = ids[2];
    }

    if (mouseButton == MSE_BUTTON_DOWN && mouseClick == MSE_CLICK_DOUBLE)
    {
      if (ids[1] == ID_FRAME_PREV)
        updateFrameCurr(
          new Cluster(*m_previousFrames[ids[2]]),
          m_previousFramePositions[ids[2]]);
      else if (ids[1] == ID_FRAME_NEXT)
        updateFrameCurr(
          new Cluster(*m_nextFrames[ids[2]]),
          m_nextFramePositions[ids[2]]);
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
    canvas->clearToolTip();
  }

  ptr = NULL;
}


// -- utility drawing functions ---------------------------------


void Simulator::clear()
{
  VisUtils::clear(m_clearColor);
}


QColor Simulator::calcColor(size_t iter, size_t numr)
{
  return VisUtils::qualPair(iter, numr);
}


void Simulator::drawFrameCurr(const bool& inSelectMode)
{
  if (inSelectMode)
  {
    if (m_currentFrame != NULL)
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
    double pix = canvas->getPixelSize();
    vector< double > valsFrame;

    if (m_currentFrame != NULL)
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
      attr = NULL;
      node = NULL;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      if (m_currentSelection == ID_FRAME_CURR)
      {
        VisUtils::setColor(m_selectColor);
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
        canvas,
        m_attributes,
        valsFrame);

      if (m_currentSelection == ID_FRAME_CURR)
      {
        VisUtils::setColor(m_selectColor);
        VisUtils::enableLineAntiAlias();
        VisUtils::setColor(m_selectColor);
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
    double pix = canvas->getPixelSize();
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
          attr = NULL;
          node = NULL;

          m_diagram->visualize(
            inSelectMode,
            canvas,
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
        attr = NULL;
        node = NULL;

        glPushMatrix();
        glTranslatef(
          m_previousFramePositions[m_currentSelectionIndex].x,
          m_previousFramePositions[m_currentSelectionIndex].y,
          0.0);
        glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

        VisUtils::setColor(m_selectColor);
        VisUtils::fillRect(
          -1.0+4*pix/m_horizontalFrameScale,  1.0+4*pix/m_horizontalFrameScale,
          1.0-4*pix/m_horizontalFrameScale, -1.0-4*pix/m_horizontalFrameScale);
        m_diagram->visualize(
          inSelectMode,
          canvas,
          m_attributes,
          valsFrame);

        VisUtils::enableLineAntiAlias();
        VisUtils::setColor(m_selectColor);
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
    double pix = canvas->getPixelSize();
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
          attr = NULL;
          node = NULL;

          m_diagram->visualize(
            inSelectMode,
            canvas,
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
        attr = NULL;
        node = NULL;

        glPushMatrix();
        glTranslatef(
          m_nextFramePositions[m_currentSelectionIndex].x,
          m_nextFramePositions[m_currentSelectionIndex].y,
          0.0);
        glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

        VisUtils::setColor(m_selectColor);
        VisUtils::fillRect(
          -1.0+4*pix/m_horizontalFrameScale,  1.0+4*pix/m_horizontalFrameScale,
          1.0-4*pix/m_horizontalFrameScale, -1.0-4*pix/m_horizontalFrameScale);
        m_diagram->visualize(
          inSelectMode,
          canvas,
          m_attributes,
          valsFrame);

        VisUtils::enableLineAntiAlias();
        VisUtils::setColor(m_selectColor);
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
    double pix = canvas->getPixelSize();;
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
        0.0, (lbl.size()+1)*CHARWIDTH*(m_textSize*pix/CHARHEIGHT),
        0.5*m_textSize*pix, -0.5*m_textSize*pix);

      glPopMatrix();

      glPushMatrix();
      glTranslatef(
        m_previousBundleLabelPositionBR[i].x,
        m_previousBundleLabelPositionBR[i].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(m_textSize*pix/CHARHEIGHT)),  0.0,
        0.5*m_textSize*pix,                                     -0.5*m_textSize*pix);

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
    double pix = canvas->getPixelSize();
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

        double txt = m_textSize;

        glPushMatrix();
        glTranslatef(
          m_previousBundleLabelPositionTL[i].x,
          m_previousBundleLabelPositionTL[i].y,
          0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);

        VisUtils::setColor(m_textColor);
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

        VisUtils::setColor(m_textColor);
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

      double txt = m_textSize;
      txt += 1;

      QColor colLne = calcColor(m_bundlesPreviousByLabel[idxHiLite]->getParent()->getIndex(), m_bundlesByLabel.size() - 1);

      glPushMatrix();
      glTranslatef(
        m_previousBundleLabelPositionTL[idxHiLite].x,
        m_previousBundleLabelPositionTL[idxHiLite].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(m_clearColor);
      VisUtils::fillRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(m_textColor);
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
      VisUtils::setColor(m_clearColor);
      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(m_textColor);
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
    double pix = canvas->getPixelSize();;

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
        0.0, (lbl.size()+1)*CHARWIDTH*(m_textSize*pix/CHARHEIGHT),
        0.5*m_textSize*pix, -0.5*m_textSize*pix);

      glPopMatrix();

      glPushMatrix();
      glTranslatef(
        m_nextBundleLabelPositionBR[i].x,
        m_nextBundleLabelPositionBR[i].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(m_textSize*pix/CHARHEIGHT)), 0.0,
        0.5*m_textSize*pix,                                    -0.5*m_textSize*pix);

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
    double pix = canvas->getPixelSize();;
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

        double txt = m_textSize;

        glPushMatrix();
        glTranslatef(
          m_nextBundleLabelPositionTL[i].x,
          m_nextBundleLabelPositionTL[i].y,
          0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);

        VisUtils::setColor(m_textColor);
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

        VisUtils::setColor(m_textColor);
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

      double txt = m_textSize;
      txt += 1;

      QColor colLne = calcColor(m_bundlesNextByLabel[idxHiLite]->getParent()->getIndex(), m_bundlesByLabel.size());

      glPushMatrix();
      glTranslatef(
        m_nextBundleLabelPositionTL[idxHiLite].x,
        m_nextBundleLabelPositionTL[idxHiLite].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(m_clearColor);
      VisUtils::fillRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(m_textColor);
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
      VisUtils::setColor(m_clearColor);
      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(m_textColor);
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
    double pix = canvas->getPixelSize();

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
    double pix = canvas->getPixelSize();
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
    double pix = canvas->getPixelSize();
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
    double pix = canvas->getPixelSize();
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
  double wth, hgt;
  canvas->getSize(wth, hgt);
  double pix = canvas->getPixelSize();

  double itvSml = 6.0*pix;
  double itvLrg = 9.0*pix;

  if (inSelectMode)
  {
    // clear icon
    double x = 0.5*wth - itvSml - pix;
    double y = 0.5*hgt - itvSml - pix;
    glPushName(ID_ICON_CLEAR);
    VisUtils::fillRect(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    glPopName();

    // up arrow
    x =  0.0;
    y = -0.5*hgt + 3.0*itvLrg + 2.0*pix + 4*pix;
    glPushName(ID_ICON_UP);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    glPopName();

    // right arrow
    x =  0.0 + 2*itvLrg + 4.0*pix;
    y = -0.5*hgt + 1.0*itvLrg + 2.0*pix;
    glPushName(ID_ICON_NEXT);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    glPopName();

    // down arrow
    x =  0.0;
    y = -0.5*hgt + 1.0*itvLrg + 2.0*pix;
    glPushName(ID_ICON_DOWN);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    glPopName();

    // left arrow
    x =  0.0 - 2.0*itvLrg - 4.0*pix;
    y = -0.5*hgt + 1.0*itvLrg + 2.0*pix;
    glPushName(ID_ICON_PREV);
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    glPopName();
  }
  else
  {
    VisUtils::enableLineAntiAlias();

    // clear icon
    double x = 0.5*wth - itvSml - pix;
    double y = 0.5*hgt - itvSml - pix;
    VisUtils::setColor(Qt::white);
    VisUtils::fillClearIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColor(VisUtils::darkGray);
    VisUtils::drawClearIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    // up arrow
    x =  0.0;
    y = -0.5*hgt + 3.0*itvLrg + 2.0*pix + 4*pix;
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
    y = -0.5*hgt + 1.0*itvLrg + 2.0*pix;
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
    y = -0.5*hgt + 1.0*itvLrg + 2.0*pix;
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
    y = -0.5*hgt + 1.0*itvLrg + 2.0*pix;
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

  if (m_animationOldFrame != NULL)
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
      attr = NULL;
      node = NULL;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      m_diagram->visualize(
        false,
        canvas,
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
      attr = NULL;
      node = NULL;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      m_diagram->visualize(
        false,
        canvas,
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
      attr = NULL;
      node = NULL;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      m_diagram->visualize(
        false,
        canvas,
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
      attr = NULL;
      node = NULL;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(m_horizontalFrameScale, m_horizontalFrameScale, m_horizontalFrameScale);

      m_diagram->visualize(
        false,
        canvas,
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

      if (m_blendType != VisUtils::BLEND_HARD)
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
        canvas->enableMouseMotion();
        m_currentAnimationPhase = ANIM_NONE;

        // update new data
        m_currentFrame = m_animationOldFrame;

        m_animationOldFrame = NULL;
        delete m_animationNewFrame;
        m_animationNewFrame = NULL;

        initFramesPrevNext();
        initBundles();
        sortFramesPrevNext();

        // init & visualize
        dataChanged = true;
        canvas->Refresh();
      }
    }
    else if (m_currentAnimationPhase == ANIM_BLEND)
    {
      m_animationTimer.stop();
      canvas->enableMouseMotion();
      m_currentAnimationPhase = ANIM_NONE;

      // update new data
      m_currentFrame = m_animationOldFrame;

      m_animationOldFrame = NULL;
      delete m_animationNewFrame;
      m_animationNewFrame = NULL;

      initFramesPrevNext();
      initBundles();
      sortFramesPrevNext();

      // init & visualize
      dataChanged = true;
      canvas->Refresh();
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
      if (m_blendType == VisUtils::BLEND_LINEAR)
      {
        m_animationOldFrameOpacity = (1-a)*0.0 + a*1.0;
        m_animationNewFrameOpacity = (1-a)*1.0 + a*0.0;
      }
      else if (m_blendType == VisUtils::BLEND_CONCAVE)
      {
        m_animationOldFrameOpacity = (a*1.0) * (a*1.0);
        m_animationNewFrameOpacity = 1.0 - m_animationOldFrameOpacity;
      }
      else if (m_blendType == VisUtils::BLEND_CONVEX)
      {
        m_animationOldFrameOpacity = sin(a*(PI/2.0));
        m_animationNewFrameOpacity = 1.0 - m_animationOldFrameOpacity;
      }
      else if (m_blendType == VisUtils::BLEND_OSCILLATE)
      {
        m_animationOldFrameOpacity = (-1.0*(cos(a*3.0*PI)/2.0) + 0.5);
        m_animationNewFrameOpacity = 1.0 - m_animationOldFrameOpacity;
      }
    }

    m_totalBlendTime += m_timerInterval;

    canvas->Refresh();
    canvas->Update();
  }
}

// -- end -----------------------------------------------------------
