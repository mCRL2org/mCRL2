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


//ColorRGB Simulator::colClr = { 1.0, 1.0, 0.93, 1.0 };
ColorRGB Simulator::colClr = { 1.0, 1.0, 1.0, 1.0 };
ColorRGB Simulator::colTxt = { 0.0, 0.0, 0.0, 1.0 };
int Simulator::szeTxt = 12;
ColorRGB Simulator::colBdl = { 0.0, 0.0, 0.0, 0.3 };
int Simulator::itvLblPixVert = 40;

int Simulator::itvTmrMS = 10;
double Simulator::pixPerMS =  1.0;

int Simulator::blendType = VisUtils::BLEND_HARD;


// -- constructors and destructor -----------------------------------


Simulator::Simulator(
  Mediator* m,
  Graph* g,
  GLCanvas* c)
  : Visualizer(m, g, c)
{
  diagram   = NULL;
  frameCurr = NULL;

  focusDepthIdx = -1;
  focusFrameIdx = -1;

  focusDepthIdxLast     = -1;
  focusFrameIdxPrevLast = -1;
  focusFrameIdxNextLast = -1;

  fcsLblPrevIdx  = -1;
  fcsLblNextIdx  = -1;

  animating = false;

  timerAnim = new wxTimer();
  timerAnim->SetOwner(this, ID_TIMER);
}


Simulator::~Simulator()
{
  graph = NULL;

  clearDiagram();
  clearFrames();
  clearBundles();

  delete timerAnim;
  timerAnim = NULL;
}


// -- get functions ---------------------------------------------


ColorRGB Simulator::getColorClr()
{
  return colClr;
}


ColorRGB Simulator::getColorTxt()
{
  return colTxt;
}


int Simulator::getSizeTxt()
{
  return szeTxt;
}


ColorRGB Simulator::getColorBdl()
{
  return colBdl;
}


int Simulator::getBlendType()
{
  return blendType;
}


ColorRGB Simulator::getColorSel()
{
  ColorRGB col;
  VisUtils::mapColorCoolGreen(col);
  return col;
}


size_t Simulator::getIdxClstSel()
{
  size_t result = NON_EXISTING;

  if (focusDepthIdx == ID_FRAME_PREV)
  {
    if (0 <= focusFrameIdx && static_cast <size_t>(focusFrameIdx) < framesPrev.size())
    {
      result = framesPrev[focusFrameIdx]->getNode(0)->getCluster()->getIndex();
    }
  }
  else if (focusDepthIdx == ID_FRAME_CURR)
  {
    if (frameCurr != NULL)
    {
      result = frameCurr->getNode(0)->getCluster()->getIndex();
    }
  }
  else if (focusDepthIdx == ID_FRAME_NEXT)
  {
    if (0 <= focusFrameIdx && static_cast <size_t>(focusFrameIdx) < framesNext.size())
    {
      result = framesNext[focusFrameIdx]->getNode(0)->getCluster()->getIndex();
    }
  }

  return result;
}


// -- set functions ---------------------------------------------


void Simulator::setColorClr(const ColorRGB& col)
{
  colClr = col;
}


void Simulator::setColorTxt(const ColorRGB& col)
{
  colTxt = col;
}


void Simulator::setSizeTxt(const int& sze)
{
  szeTxt = sze;
}


void Simulator::setColorBdl(const ColorRGB& col)
{
  colBdl = col;
}


void Simulator::setBlendType(const int& type)
{
  blendType = type;
}


void Simulator::setDiagram(Diagram* dgrm)
{
  clearDiagram();
  diagram = dgrm;

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

  focusDepthIdxLast     = ID_FRAME_CURR;
  focusFrameIdxPrevLast = -1;
  focusFrameIdxNextLast = -1;

  // update new data
  initAttributes(attrs);
  frameCurr = new Cluster(*frame);
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
  keyFrameFr    = frame;
  posKeyFrameFr = pos;

  keyFrameTo    = new Cluster(*frameCurr);
  posKeyFrameTo = posFrameCurr;

  posTweenFrame = posKeyFrameFr;

  // calc animation parameters
  double xTo, yTo, xFr, yFr;
  xTo = posKeyFrameTo.x;
  yTo = posKeyFrameTo.y;
  xFr = posKeyFrameFr.x;
  yFr = posKeyFrameFr.y;

  double distPix = Utils::dist(xTo, yTo, xFr, yFr) / canvas->getPixelSize();
  timeTotalMS = distPix/pixPerMS;
  timeAlphaMS = 0.0;

  animPhase = ANIM_POS;

  // clear previous data
  clearFrames();
  clearBundles();

  focusDepthIdxLast     = ID_FRAME_CURR;
  focusFrameIdxPrevLast = -1;
  focusFrameIdxNextLast = -1;

  timerAnim->Start(itvTmrMS);
  canvas->disableMouseMotion();
}


void Simulator::clearData()
{
  focusDepthIdx  = -1;
  focusFrameIdx  = -1;

  focusDepthIdxLast     = -1;
  focusFrameIdxPrevLast = -1;
  focusFrameIdxPrevLast = -1;

  fcsLblPrevIdx  = -1;
  fcsLblNextIdx  = -1;

  clearAttributes();
  clearFrames();
  clearBundles();
}


void Simulator::handleSendDgrmSglToExnr()
{
  if (focusDepthIdx == ID_FRAME_PREV)
  {
    if (0 <= focusFrameIdx && static_cast <size_t>(focusFrameIdx) < framesPrev.size())
      mediator->addToExaminer(
        framesPrev[focusFrameIdx],
        attributes);
  }
  else if (focusDepthIdx == ID_FRAME_CURR)
  {
    if (focusFrameIdx == 0)
      mediator->addToExaminer(
        frameCurr,
        attributes);
  }
  else if (focusDepthIdx == ID_FRAME_NEXT)
  {
    if (0 <= focusFrameIdx && static_cast <size_t>(focusFrameIdx) < framesNext.size())
      mediator->addToExaminer(
        framesNext[focusFrameIdx],
        attributes);
  }
}


// -- visualization functions  --------------------------------------


void Simulator::visualize(const bool& inSelectMode)
{
  // have textures been generated
  if (texCharOK != true)
  {
    genCharTex();
  }

  clear();

  // check if positions are ok
  if (geomChanged == true)
  {
    calcSettingsGeomBased();
  }
  if (dataChanged == true)
  {
    calcSettingsDataBased();
  }

  if (inSelectMode == true)
  {
    if (timerAnim->IsRunning() != true)
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
      if (framesPrev.size() > 0 || frameCurr != NULL || framesNext.size() > 0)
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
    if (timerAnim->IsRunning() == true)
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
      if (framesPrev.size() > 0 || frameCurr != NULL || framesNext.size() > 0)
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

  if (showMenu != true)
  {
    focusDepthIdx = -1;
    focusFrameIdx = -1;

    fcsLblPrevIdx = -1;
    fcsLblNextIdx = -1;

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
  if (timerAnim->IsRunning() != true)
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
      focusDepthIdxLast = focusDepthIdx;

      if (focusDepthIdx == ID_FRAME_PREV)
      {
        focusFrameIdxPrevLast = focusFrameIdx;
      }
      else if (focusDepthIdx == ID_FRAME_NEXT)
      {
        focusFrameIdxNextLast = focusFrameIdx;
      }

      focusDepthIdx = -1;
      focusFrameIdx = -1;

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


void Simulator::initAttributes(const vector< Attribute* > &attrs)
{
  attributes = attrs;
}


void Simulator::initFramesPrevNext()
{
  Node*        temp;
  set< Node* > tempPrev;
  set< Node* > tempNext;
  Cluster*     nodesPrev;
  Cluster*     nodesNext;

  // get nodes leading to & from current frame
  for (size_t i = 0; i < frameCurr->getSizeNodes(); ++i)
  {
    temp = frameCurr->getNode(i);

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
    attributes,
    framesPrev);
  graph->calcAttrCombn(
    nodesNext,
    attributes,
    framesNext);

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
    for (size_t i = 0; i < frameCurr->getSizeNodes(); ++i)
    {
      currNodes.insert(frameCurr->getNode(i));
    }
  }

  // get all edges from previous frames to current frame
  lbls.clear();
  {
    for (size_t i = 0; i < framesPrev.size(); ++i)
    {
      bdls.clear();

      clst = framesPrev[i];
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
              bdl = new Bundle(bundles.size());
              bundles.push_back(bdl);

              bdls.insert(pair< string, Bundle* >(edge->getLabel(), bdl));

              clst->addOutBundle(bdl);
              frameCurr->addInBundle(bdl);

              bdl->setInCluster(clst);
              bdl->setOutCluster(frameCurr);

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
    bdlLbls->setIndex(bundlesPrevByLbl.size());
    bundlesPrevByLbl.push_back(bdlLbls);
  }

  // get all edges from current frame to next frames
  lbls.clear();
  {
    for (size_t i = 0; i < framesNext.size(); ++i)
    {
      bdls.clear();
      clst = framesNext[i];
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
              bundles.push_back(bdl);

              bdls.insert(pair< string, Bundle* >(edge->getLabel(), bdl));

              frameCurr->addOutBundle(bdl);
              clst->addInBundle(bdl);

              bdl->setInCluster(frameCurr);
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
    bdlLbls->setIndex(bundlesNextByLbl.size());
    bundlesNextByLbl.push_back(bdlLbls);
  }

  lbls.clear();
  {
    for (size_t i = 0; i < bundlesPrevByLbl.size(); ++i)
    {
      bdl = bundlesPrevByLbl[i];

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
    for (size_t i = 0; i < bundlesNextByLbl.size(); ++i)
    {
      bdl = bundlesNextByLbl[i];

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
    bdlLbls->setIndex(bundlesByLbl.size());
    bundlesByLbl.push_back(bdlLbls);
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
    for (size_t i = 0; i < framesPrev.size(); ++i)
    {
      int key = 0;
      for (size_t j = 0; j < framesPrev[i]->getSizeOutBundles(); ++j)
      {
        key += (int)pow(10.0, (int) framesPrev[i]->getOutBundle(j)->getParent()->getIndex());
      }

      sorted.insert(pair< int, Cluster* >(key, framesPrev[i]));
    }
  }

  framesPrev.clear();
  multimap< int, Cluster* >::iterator it;
  for (it = sorted.begin(); it != sorted.end(); ++it)
  {
    framesPrev.push_back(it->second);
  }
  sorted.clear();

  // sort previous frames
  for (size_t i = 0; i < framesNext.size(); ++i)
  {
    int key = 0;
    for (size_t j = 0; j < framesNext[i]->getSizeInBundles(); ++j)
    {
      key += (int)pow(10.0, (int) framesNext[i]->getInBundle(j)->getParent()->getIndex());
    }

    sorted.insert(pair< int, Cluster* >(key, framesNext[i]));
  }

  framesNext.clear();
  for (it = sorted.begin(); it != sorted.end(); ++it)
  {
    framesNext.push_back(it->second);
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
  posFramesPrev.clear();
  posFramesNext.clear();

  // get canvas info & calc intervals
  double wthCvs, hgtCvs;
  canvas->getSize(wthCvs, hgtCvs);
  double pix = canvas->getPixelSize();
  double itvHori = wthCvs/6;
  double itvVert = (hgtCvs-itvHori)/Utils::maxx(1, Utils::maxx(framesPrev.size(), framesNext.size()));
  scaleDgrmHori = 0.5*itvHori;
  scaleDgrmVert = Utils::minn(scaleDgrmHori, 0.45*itvVert);

  // calc new positions
  pos.x = 0;
  pos.y = 0;
  posFrameCurr = pos;

  pos.x = -0.5*wthCvs + 0.5*itvHori + 4.0*pix;
  pos.y = 0.5*framesPrev.size()*itvVert - 0.5*itvVert;
  {
    for (size_t i = 0; i < framesPrev.size(); ++i)
    {
      posFramesPrev.push_back(pos);
      pos.y -= itvVert;
    }
  }

  pos.x = 0.5*wthCvs - 0.5*itvHori - 4.0*pix;
  pos.y = 0.5*framesNext.size()*itvVert - 0.5*itvVert;
  {
    for (size_t i = 0; i < framesNext.size(); ++i)
    {
      posFramesNext.push_back(pos);
      pos.y -= itvVert;
    }
  }
}


void Simulator::calcPosBundles()
{
  Position2D posTopLft, posBotRgt;

  // clear previous positions
  posBdlLblGridPrevTopLft.clear();
  posBdlLblGridPrevBotRgt.clear();
  posBdlLblGridNextTopLft.clear();
  posBdlLblGridNextBotRgt.clear();

  posBundlesPrevTopLft.clear();
  posBundlesPrevBotRgt.clear();
  posBundlesNextTopLft.clear();
  posBundlesNextBotRgt.clear();

  // get canvas info & calc intervals
  double wthCvs, hgtCvs;
  canvas->getSize(wthCvs, hgtCvs);
  double pix = canvas->getPixelSize();
  double itvHori = wthCvs/6;
  double itvVert = (hgtCvs-itvHori)/Utils::maxx(1, Utils::maxx(framesPrev.size(), framesNext.size()));

  // calc new positions
  if (posFramesPrev.size() > 0 && bundlesPrevByLbl.size() > 0)
  {
    // grid prev
    if (posFramesPrev.size() >= posFramesNext.size())
    {
      posTopLft.y = posFramesPrev[0].y + 1.0*scaleDgrmVert + 0.125*itvVert;
      posBotRgt.y = posFramesPrev[posFramesPrev.size()-1].y - 1.0*scaleDgrmVert - 0.125*itvVert;
    }
    else
    {
      posTopLft.y = posFramesNext[0].y + 1.0*scaleDgrmVert + 0.125*itvVert;
      posBotRgt.y = posFramesNext[posFramesNext.size()-1].y - 1.0*scaleDgrmVert - 0.125*itvVert;
    }

    double itvGrid = (1.5*itvHori)/(bundlesPrevByLbl.size()+1);

    {
      for (size_t i = 0; i < bundlesPrevByLbl.size(); ++i)
      {
        posTopLft.x = -2.0*itvHori + (i+1)*itvGrid;
        posTopLft.y =  0.5*hgtCvs - itvLblPixVert*pix;
        posBotRgt.x =  posTopLft.x;
        posBotRgt.y = -0.5*hgtCvs + itvLblPixVert*pix;

        posBdlLblGridPrevTopLft.push_back(posTopLft);
        posBdlLblGridPrevBotRgt.push_back(posBotRgt);
      }
    }

    // bundles prev
    {
      for (size_t i = 0; i < framesPrev.size(); ++i)
      {
        vector< Position2D > v;

        posBundlesPrevTopLft.push_back(v);
        posBundlesPrevBotRgt.push_back(v);

        // incoming bundles
        double itv = 2.0/bundlesPrevByLbl.size();
        {
          for (size_t j = 0; j < framesPrev[i]->getSizeOutBundles(); ++j)
          {
            ///*
            posTopLft.x = posFramesPrev[i].x + 1.0*scaleDgrmVert + 3.0*pix;
            posTopLft.y = posFramesPrev[i].y
                          + 1.0*scaleDgrmVert
                          - 0.5*itv*scaleDgrmVert
                          - framesPrev[i]->getOutBundle(j)->getParent()->getIndex()*itv*scaleDgrmVert;

            posBotRgt.x = posBdlLblGridPrevTopLft[ framesPrev[i]->getOutBundle(j)->getParent()->getIndex() ].x;
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
            posBundlesPrevTopLft[i].push_back(posTopLft);
            posBundlesPrevBotRgt[i].push_back(posBotRgt);
          }
        }
      }
    }
  }

  if (posFramesNext.size() > 0 && bundlesNextByLbl.size() > 0)
  {
    // grid prev
    if (posFramesPrev.size() >= posFramesNext.size())
    {
      posTopLft.y = posFramesPrev[0].y + 1.0*scaleDgrmVert + 0.125*itvVert;
      posBotRgt.y = posFramesPrev[posFramesPrev.size()-1].y - 1.0*scaleDgrmVert - 0.125*itvVert;
    }
    else
    {
      posTopLft.y = posFramesNext[0].y + 1.0*scaleDgrmVert + 0.125*itvVert;
      posBotRgt.y = posFramesNext[posFramesNext.size()-1].y - 1.0*scaleDgrmVert - 0.125*itvVert;
    }

    // grid next
    double itvGrid = (1.5*itvHori)/(bundlesNextByLbl.size()+1);

    {
      for (size_t i = 0; i < bundlesNextByLbl.size(); ++i)
      {
        posTopLft.x = 2.0*itvHori - (bundlesNextByLbl.size()-i)*itvGrid;
        posTopLft.y =  0.5*hgtCvs - itvLblPixVert*pix;
        posBotRgt.x = posTopLft.x;
        posBotRgt.y = -0.5*hgtCvs + itvLblPixVert*pix;

        posBdlLblGridNextTopLft.push_back(posTopLft);
        posBdlLblGridNextBotRgt.push_back(posBotRgt);
      }
    }

    // bundles next
    {
      for (size_t i = 0; i < framesNext.size(); ++i)
      {
        vector< Position2D > v;

        posBundlesNextTopLft.push_back(v);
        posBundlesNextBotRgt.push_back(v);

        // outgoing bundles
        double itv = 2.0/bundlesNextByLbl.size();
        {
          for (size_t j = 0; j < framesNext[i]->getSizeInBundles(); ++j)
          {
            posTopLft.x = posBdlLblGridNextTopLft[ framesNext[i]->getInBundle(j)->getParent()->getIndex() ].x + 1.0*pix;
            posTopLft.y = posBotRgt.y = posFramesNext[i].y
                                        + 1.0*scaleDgrmVert
                                        - 0.5*itv*scaleDgrmVert
                                        - framesNext[i]->getInBundle(j)->getParent()->getIndex()*itv*scaleDgrmVert;

            posBotRgt.x = posFramesNext[i].x - 1.0*scaleDgrmVert - 3.0*pix;
            posBotRgt.y = posTopLft.y;

            posBundlesNextTopLft[i].push_back(posTopLft);
            posBundlesNextBotRgt[i].push_back(posBotRgt);
          }
        }
      }
    }
  }
}


void Simulator::handleKeyUp()
{
  if (focusDepthIdx == ID_FRAME_PREV)
  {
    if (0 < focusFrameIdx)
    {
      --focusFrameIdx;
    }

    focusFrameIdxPrevLast = focusFrameIdx;
  }
  else if (focusDepthIdx == ID_FRAME_NEXT)
  {
    if (0 < focusFrameIdx)
    {
      --focusFrameIdx;
    }

    focusFrameIdxNextLast = focusFrameIdx;
  }
}


void Simulator::handleKeyRgt()
{
  if (focusDepthIdx == -1)
  {
    if (focusDepthIdxLast != -1)
    {
      focusDepthIdx = focusDepthIdxLast;

      if (focusDepthIdx == ID_FRAME_PREV)
      {
        if (0 <= focusFrameIdxPrevLast && static_cast <size_t>(focusFrameIdxPrevLast) < framesPrev.size())
        {
          focusFrameIdx = focusFrameIdxPrevLast;
        }
        else
        {
          focusFrameIdx = 0;
        }
      }
      else if (focusDepthIdx == ID_FRAME_CURR)
      {
        focusFrameIdx = 0;
      }
      else if (focusDepthIdx == ID_FRAME_NEXT)
      {
        if (0 <= focusFrameIdxNextLast && static_cast <size_t>(focusFrameIdxNextLast) < framesNext.size())
        {
          focusFrameIdx = focusFrameIdxNextLast;
        }
        else
        {
          focusFrameIdx = 0;
        }
      }
    }
    else
    {
      focusDepthIdx = ID_FRAME_CURR;
      focusFrameIdx = 0;
    }
  }
  else if (focusDepthIdx == ID_FRAME_PREV)
  {
    focusDepthIdx = ID_FRAME_CURR;
    focusFrameIdx = 0;
  }
  else if (focusDepthIdx == ID_FRAME_CURR)
  {
    if (framesNext.size() > 0)
    {
      focusDepthIdx = ID_FRAME_NEXT;

      if (0 <= focusFrameIdxNextLast && static_cast <size_t>(focusFrameIdxNextLast) < framesNext.size())
      {
        focusFrameIdx = focusFrameIdxNextLast;
      }
      else
      {
        focusFrameIdx = 0;
      }

      focusFrameIdxNextLast = focusFrameIdx;
    }
  }
  else if (focusDepthIdx == ID_FRAME_NEXT)
  {
    updateFrameCurr(
      new Cluster(*framesNext[focusFrameIdx]),
      posFramesNext[focusFrameIdx]);

    focusDepthIdx = ID_FRAME_CURR;
    focusFrameIdx = 0;
  }
}


void Simulator::handleKeyDwn()
{
  if (focusDepthIdx == ID_FRAME_PREV)
  {
    if (static_cast <size_t>(focusFrameIdx) < framesPrev.size()-1)
    {
      ++focusFrameIdx;
    }

    focusFrameIdxPrevLast = focusFrameIdx;
  }
  else if (focusDepthIdx == ID_FRAME_NEXT)
  {
    if (static_cast <size_t>(focusFrameIdx) < framesNext.size()-1)
    {
      ++focusFrameIdx;
    }

    focusFrameIdxNextLast = focusFrameIdx;
  }
}


void Simulator::handleKeyLft()
{
  if (focusDepthIdx < 0)
  {
    if (focusDepthIdxLast >= 0)
    {
      focusDepthIdx = focusDepthIdxLast;

      if (focusDepthIdx == ID_FRAME_PREV)
      {
        if (0 <= focusFrameIdxPrevLast && static_cast <size_t>(focusFrameIdxPrevLast) < framesPrev.size())
        {
          focusFrameIdx = focusFrameIdxPrevLast;
        }
        else
        {
          focusFrameIdx = 0;
        }
      }
      else if (focusDepthIdx == ID_FRAME_CURR)
      {
        focusFrameIdx = 0;
      }
      else if (focusDepthIdx == ID_FRAME_NEXT)
      {
        if (0 <= focusFrameIdxNextLast && static_cast <size_t>(focusFrameIdxNextLast) < framesNext.size())
        {
          focusFrameIdx = focusFrameIdxNextLast;
        }
        else
        {
          focusFrameIdx = 0;
        }
      }
    }
    else
    {
      focusDepthIdx = ID_FRAME_CURR;
      focusFrameIdx = 0;
    }
  }
  else if (focusDepthIdx == ID_FRAME_PREV)
  {
    updateFrameCurr(
      new Cluster(*framesPrev[focusFrameIdx]),
      posFramesPrev[focusFrameIdx]);

    focusDepthIdx = ID_FRAME_CURR;
    focusFrameIdx = 0;
  }
  else if (focusDepthIdx == ID_FRAME_CURR)
  {
    if (framesPrev.size() > 0)
    {
      focusDepthIdx = ID_FRAME_PREV;

      if (0 <= focusFrameIdxPrevLast && static_cast <size_t>(focusFrameIdxPrevLast) < framesPrev.size())
      {
        focusFrameIdx = focusFrameIdxPrevLast;
      }
      else
      {
        focusFrameIdx = 0;
      }

      focusFrameIdxPrevLast = focusFrameIdx;
    }
  }
  else if (focusDepthIdx == ID_FRAME_NEXT)
  {
    focusDepthIdx = ID_FRAME_CURR;
    focusFrameIdx = 0;
  }
}


void Simulator::markFrameClusts()
{
  if (focusDepthIdx == ID_FRAME_PREV)
  {
    if (0 <= focusFrameIdx && static_cast <size_t>(focusFrameIdx) < framesPrev.size())
    {
      /*
      mediator->handleUnmarkFrameClusts();
      */
      mediator->handleMarkFrameClust(this);

      ColorRGB col;
      VisUtils::mapColorCoolGreen(col);
      mediator->handleShowFrame(
        framesPrev[focusFrameIdx],
        attributes,
        col);
    }
  }
  else if (focusDepthIdx == ID_FRAME_CURR)
  {
    if (frameCurr != NULL)
    {
      /*
      mediator->handleUnmarkFrameClusts();
      */
      mediator->handleMarkFrameClust(this);

      ColorRGB col;
      VisUtils::mapColorCoolGreen(col);
      mediator->handleShowFrame(
        frameCurr,
        attributes,
        col);
    }
  }
  else if (focusDepthIdx == ID_FRAME_NEXT)
  {
    if (0 <= focusFrameIdx && static_cast <size_t>(focusFrameIdx) < framesNext.size())
    {
      /*
      mediator->handleUnmarkFrameClusts();
      */
      mediator->handleMarkFrameClust(this);

      ColorRGB col;
      VisUtils::mapColorCoolGreen(col);
      mediator->handleShowFrame(
        framesNext[focusFrameIdx],
        attributes,
        col);
    }
  }
}


void Simulator::clearAttributes()
{
  attributes.clear();
}


void Simulator::clearDiagram()
{
  diagram = NULL;
}


void Simulator::clearFrames()
{
  if (frameCurr != NULL)
  {
    delete frameCurr;
    frameCurr = NULL;
  }

  {
    for (size_t i = 0; i < framesPrev.size(); ++i)
    {
      delete framesPrev[i];
    }
  }
  framesPrev.clear();

  {
    for (size_t i = 0; i < framesNext.size(); ++i)
    {
      delete framesNext[i];
    }
  }
  framesNext.clear();

  posFramesPrev.clear();
  posFramesNext.clear();
}


void Simulator::clearBundles()
{
  {
    for (size_t i = 0; i < bundles.size(); ++i)
    {
      delete bundles[i];
    }
    bundles.clear();
  }

  {
    for (size_t i = 0; i < bundlesPrevByLbl.size(); ++i)
    {
      delete bundlesPrevByLbl[i];
    }
  }
  bundlesPrevByLbl.clear();

  {
    for (size_t i = 0; i < bundlesNextByLbl.size(); ++i)
    {
      delete bundlesNextByLbl[i];
    }
  }
  bundlesNextByLbl.clear();

  {
    for (size_t i = 0; i < bundlesByLbl.size(); ++i)
    {
      delete bundlesByLbl[i];
    }
  }
  bundlesByLbl.clear();

  posBdlLblGridPrevTopLft.clear();
  posBdlLblGridPrevBotRgt.clear();
  posBdlLblGridNextTopLft.clear();
  posBdlLblGridNextBotRgt.clear();

  posBundlesPrevTopLft.clear();
  posBundlesPrevBotRgt.clear();
  posBundlesNextTopLft.clear();
  posBundlesNextBotRgt.clear();
}


// -- hit detection ---------------------------------------------


void Simulator::handleHits(const vector< int > &ids)
{
  if (ids.size() == 1)
  {
    if (ids[0] == ID_CANVAS)
    {
      if (focusDepthIdx != -1 || focusFrameIdx != -1)
      {
        focusDepthIdx = -1;
        focusFrameIdx = -1;

        mediator->handleUnmarkFrameClusts(this);
        mediator->handleUnshowFrame();
      }

      fcsLblPrevIdx = -1;
      fcsLblNextIdx = -1;
    }
  }
  else if (ids.size() > 1)
  {
    if (ids[1] == ID_ICON_CLEAR &&
        (mouseSide == MSE_SIDE_LFT && mouseButton == MSE_BUTTON_DOWN))
    {
      if (framesPrev.size() > 0 || frameCurr != NULL || framesNext.size() > 0)
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
      focusDepthIdx     = ID_FRAME_CURR;
      focusDepthIdxLast = focusDepthIdx;
      focusFrameIdx     = ids[2];

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
      focusDepthIdx         = ID_FRAME_PREV;
      focusDepthIdxLast     = focusDepthIdx;
      focusFrameIdx         = ids[2];
      focusFrameIdxPrevLast = focusFrameIdx;

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
      focusDepthIdx         = ID_FRAME_NEXT;
      focusDepthIdxLast     = focusDepthIdx;
      focusFrameIdx         = ids[2];
      focusFrameIdxNextLast = focusFrameIdx;

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
      fcsLblPrevIdx = ids[2];
      fcsLblNextIdx = ids[2];
    }

    if (mouseButton == MSE_BUTTON_DOWN && mouseClick == MSE_CLICK_DOUBLE)
    {
      if (ids[1] == ID_FRAME_PREV)
        updateFrameCurr(
          new Cluster(*framesPrev[ids[2]]),
          posFramesPrev[ids[2]]);
      else if (ids[1] == ID_FRAME_NEXT)
        updateFrameCurr(
          new Cluster(*framesNext[ids[2]]),
          posFramesNext[ids[2]]);
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
  VisUtils::clear(colClr);
}


void Simulator::calcColor(
  const size_t& iter,
  const size_t& numr,
  ColorRGB& col)
{
  VisUtils::mapColorQualPair(
    iter,
    numr,
    col);
  /*
  if ( colorMap == VisUtils::COL_MAP_QUAL_PAST_1 )
      VisUtils::mapColorQualPast1(
          iter,
          numr,
          col );
  else if ( colorMap == VisUtils::COL_MAP_QUAL_PAST_2 )
      VisUtils::mapColorQualPast2(
          iter,
          numr,
          col );
  else if ( colorMap == VisUtils::COL_MAP_QUAL_SET_1 )
      VisUtils::mapColorQualSet1(
          iter,
          numr,
          col );
  else if ( colorMap == VisUtils::COL_MAP_QUAL_SET_2 )
      VisUtils::mapColorQualSet2(
          iter,
          numr,
          col );
  else if ( colorMap == VisUtils::COL_MAP_QUAL_SET_3 )
      VisUtils::mapColorQualSet3(
          iter,
          numr,
          col );
  else if ( colorMap == VisUtils::COL_MAP_QUAL_PAIR )
      VisUtils::mapColorQualPair(
          iter,
          numr,
          col );
  else if ( colorMap == VisUtils::COL_MAP_QUAL_DARK )
      VisUtils::mapColorQualDark(
          iter,
          numr,
          col );
  else if ( colorMap == VisUtils::COL_MAP_QUAL_ACCENT )
      VisUtils::mapColorQualAccent(
          iter,
          numr,
          col );
  */
}


void Simulator::drawFrameCurr(const bool& inSelectMode)
{
  if (inSelectMode == true)
  {
    if (frameCurr != NULL)
    {
      double x = posFrameCurr.x;
      double y = posFrameCurr.y;

      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glScalef(scaleDgrmHori, scaleDgrmHori, scaleDgrmHori);

      glPushName(ID_FRAME_CURR);

      glPushName(0);
      VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);

      if (focusDepthIdx == ID_FRAME_CURR && focusFrameIdx == 0)
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

    if (frameCurr != NULL)
    {
      double x = posFrameCurr.x;
      double y = posFrameCurr.y;
      /*
      for ( int j = 0; j < attributes.size(); ++j )
          valsFrame.push_back(
              attributes[j]->mapToValue(
                  frameCurr->getNode(0)->getTupleVal(
                      attributes[j]->getIndex() ) )->getIndex() );
      */
      Attribute* attr;
      Node* node;
      for (size_t j = 0; j < attributes.size(); ++j)
      {
        attr = attributes[j];
        node = frameCurr->getNode(0);
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
      glScalef(scaleDgrmHori, scaleDgrmHori, scaleDgrmHori);

      if (focusDepthIdx == ID_FRAME_CURR)
      {
        VisUtils::setColorCoolGreen();
        VisUtils::fillRect(
          -1.0+4*pix/scaleDgrmHori,  1.0+4*pix/scaleDgrmHori,
          1.0-4*pix/scaleDgrmHori, -1.0-4*pix/scaleDgrmHori);
      }
      else
      {
        VisUtils::setColorMdGray();
        VisUtils::fillRect(
          -1.0+3*pix/scaleDgrmHori,  1.0+3*pix/scaleDgrmHori,
          1.0-3*pix/scaleDgrmHori, -1.0-3*pix/scaleDgrmHori);
      }
      diagram->visualize(
        inSelectMode,
        canvas,
        attributes,
        valsFrame,
        pix);

      if (focusDepthIdx == ID_FRAME_CURR)
      {
        VisUtils::setColorCoolGreen();
        VisUtils::enableLineAntiAlias();
        VisUtils::setColorCoolGreen();
        VisUtils::fillMoreIcon(-0.98, -0.8, -0.8, -0.98);
        VisUtils::setColorLtLtGray();
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
  if (inSelectMode == true)
  {
    glPushName(ID_FRAME_PREV);
    for (size_t i = 0; i < posFramesPrev.size(); ++i)
    {
      double x = posFramesPrev[i].x;
      double y = posFramesPrev[i].y;

      glPushMatrix();
      glTranslatef(x, y, 0.0);

      if (focusDepthIdx == ID_FRAME_PREV &&  static_cast <size_t>(focusFrameIdx) == i)
      {
        glScalef(scaleDgrmHori, scaleDgrmHori, scaleDgrmHori);
      }
      else
      {
        glScalef(scaleDgrmVert, scaleDgrmVert, scaleDgrmVert);
      }

      glPushName((GLuint) i);
      VisUtils::fillRect(
        -1.0,  1.0,
        1.0, -1.0);

      if (focusDepthIdx == ID_FRAME_PREV &&  static_cast <size_t>(focusFrameIdx) == i)
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

    for (int i = 0; i < (int) posFramesPrev.size(); ++i)
    {
      if (focusDepthIdx != ID_FRAME_PREV ||  i != focusFrameIdx)
      {
        double x = posFramesPrev[i].x;
        double y = posFramesPrev[i].y;

        glPushMatrix();
        glTranslatef(x, y, 0.0);
        glScalef(scaleDgrmVert, scaleDgrmVert, scaleDgrmVert);

        VisUtils::setColorMdGray();
        VisUtils::fillRect(
          -1.0+3*pix/scaleDgrmVert,  1.0+3*pix/scaleDgrmVert,
          1.0-3*pix/scaleDgrmVert, -1.0-3*pix/scaleDgrmVert);

        if (2.0*scaleDgrmVert > 30.0*pix)
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
          for (size_t j = 0; j < attributes.size(); ++j)
          {
            attr = attributes[j];
            node = framesPrev[i]->getNode(0);
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

          diagram->visualize(
            inSelectMode,
            canvas,
            attributes,
            valsFrame,
            pix);
        }
        else
        {
          VisUtils::setColorWhite();
          VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);
          VisUtils::setColorMdGray();
          VisUtils::drawRect(-1.0, 1.0, 1.0, -1.0);
        }

        glPopMatrix();

        valsFrame.clear();
      }
    }

    if (focusDepthIdx == ID_FRAME_PREV)
    {
      if (0 <= focusFrameIdx &&  static_cast <size_t>(focusFrameIdx) < posFramesPrev.size())
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
        for (size_t j = 0; j < attributes.size(); ++j)
        {
          attr = attributes[j];
          node = framesPrev[focusFrameIdx]->getNode(0);
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
          posFramesPrev[focusFrameIdx].x,
          posFramesPrev[focusFrameIdx].y,
          0.0);
        glScalef(scaleDgrmHori, scaleDgrmHori, scaleDgrmHori);

        VisUtils::setColorCoolGreen();
        VisUtils::fillRect(
          -1.0+4*pix/scaleDgrmHori,  1.0+4*pix/scaleDgrmHori,
          1.0-4*pix/scaleDgrmHori, -1.0-4*pix/scaleDgrmHori);
        diagram->visualize(
          inSelectMode,
          canvas,
          attributes,
          valsFrame);

        VisUtils::enableLineAntiAlias();
        VisUtils::setColorCoolGreen();
        VisUtils::fillMoreIcon(-0.98, -0.8, -0.8, -0.98);
        VisUtils::setColorLtLtGray();
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
  if (inSelectMode == true)
  {
    glPushName(ID_FRAME_NEXT);
    for (size_t i = 0; i < posFramesNext.size(); ++i)
    {
      double x = posFramesNext[i].x;
      double y = posFramesNext[i].y;

      glPushMatrix();
      glTranslatef(x, y, 0.0);

      if (focusDepthIdx == ID_FRAME_NEXT &&  static_cast <size_t>(focusFrameIdx) == i)
      {
        glScalef(scaleDgrmHori, scaleDgrmHori, scaleDgrmHori);
      }
      else
      {
        glScalef(scaleDgrmVert, scaleDgrmVert, scaleDgrmVert);
      }

      glPushName((GLuint) i);
      VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);

      if (focusDepthIdx == ID_FRAME_NEXT &&  static_cast <size_t>(focusFrameIdx) == i)
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

    for (size_t i = 0; i < posFramesNext.size(); ++i)
    {
      if (focusDepthIdx != ID_FRAME_NEXT || i !=  static_cast <size_t>(focusFrameIdx))
      {
        double x = posFramesNext[i].x;
        double y = posFramesNext[i].y;

        glPushMatrix();
        glTranslatef(x, y, 0.0);
        glScalef(scaleDgrmVert, scaleDgrmVert, scaleDgrmVert);

        VisUtils::setColorMdGray();
        VisUtils::fillRect(
          -1.0+3*pix/scaleDgrmVert,  1.0+3*pix/scaleDgrmVert,
          1.0-3*pix/scaleDgrmVert, -1.0-3*pix/scaleDgrmVert);

        if (2.0*scaleDgrmVert > 30.0*pix)
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
          for (size_t j = 0; j < attributes.size(); ++j)
          {
            attr = attributes[j];
            node = framesNext[i]->getNode(0);
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

          diagram->visualize(
            inSelectMode,
            canvas,
            attributes,
            valsFrame,
            pix);
        }
        else
        {
          VisUtils::setColorWhite();
          VisUtils::fillRect(-1.0, 1.0, 1.0, -1.0);
          VisUtils::setColorMdGray();
          VisUtils::drawRect(-1.0, 1.0, 1.0, -1.0);
        }

        glPopMatrix();

        valsFrame.clear();
      }
    }

    if (focusDepthIdx == ID_FRAME_NEXT)
    {
      if (0 <= focusFrameIdx &&  static_cast <size_t>(focusFrameIdx) < posFramesNext.size())
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
        for (size_t j = 0; j < attributes.size(); ++j)
        {
          attr = attributes[j];
          node = framesNext[focusFrameIdx]->getNode(0);
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
          posFramesNext[focusFrameIdx].x,
          posFramesNext[focusFrameIdx].y,
          0.0);
        glScalef(scaleDgrmHori, scaleDgrmHori, scaleDgrmHori);

        VisUtils::setColorCoolGreen();
        VisUtils::fillRect(
          -1.0+4*pix/scaleDgrmHori,  1.0+4*pix/scaleDgrmHori,
          1.0-4*pix/scaleDgrmHori, -1.0-4*pix/scaleDgrmHori);
        diagram->visualize(
          inSelectMode,
          canvas,
          attributes,
          valsFrame,
          pix);

        VisUtils::enableLineAntiAlias();
        VisUtils::setColorCoolGreen();
        VisUtils::fillMoreIcon(-0.98, -0.8, -0.8, -0.98);
        VisUtils::setColorLtLtGray();
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
  if (inSelectMode == true)
  {
    double pix = canvas->getPixelSize();;
    string lbl;

    glPushName(ID_BUNDLE_LBL);
    for (size_t i = 0; i < posBdlLblGridPrevTopLft.size(); ++i)
    {
      lbl = bundlesPrevByLbl[i]->getChild(0)->getEdge(0)->getLabel();

      glPushName((GLuint) bundlesPrevByLbl[i]->getParent()->getIndex());

      glPushMatrix();
      glTranslatef(
        posBdlLblGridPrevTopLft[i].x,
        posBdlLblGridPrevTopLft[i].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::fillRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(szeTxt*pix/CHARHEIGHT),
        0.5*szeTxt*pix, -0.5*szeTxt*pix);

      glPopMatrix();

      glPushMatrix();
      glTranslatef(
        posBdlLblGridPrevBotRgt[i].x,
        posBdlLblGridPrevBotRgt[i].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(szeTxt*pix/CHARHEIGHT)),  0.0,
        0.5*szeTxt*pix,                                     -0.5*szeTxt*pix);

      glPopMatrix();

      VisUtils::drawLine(
        posBdlLblGridPrevTopLft[i].x,
        posBdlLblGridPrevBotRgt[i].x,
        posBdlLblGridPrevTopLft[i].y,
        posBdlLblGridPrevBotRgt[i].y);

      glPopName();
    }
    glPopName();
  }
  else
  {
    ColorRGB colLne;
    double pix = canvas->getPixelSize();
    size_t idxHiLite = NON_EXISTING;

    VisUtils::mapColorLtGray(colLne);

    for (size_t i = 0; i < posBdlLblGridPrevTopLft.size(); ++i)
    {
      if (bundlesPrevByLbl[i]->getParent()->getIndex() == fcsLblPrevIdx)
      {
        idxHiLite = i;
      }
      else
      {
        string lbl = bundlesPrevByLbl[i]->getChild(0)->getEdge(0)->getLabel();

        double txt = szeTxt;

        glPushMatrix();
        glTranslatef(
          posBdlLblGridPrevTopLft[i].x,
          posBdlLblGridPrevTopLft[i].y,
          0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);

        VisUtils::setColor(colTxt);
        VisUtils::drawLabel(
          texCharId,
          0.0 + 3*pix,
          0.0,
          txt*pix/CHARHEIGHT,
          lbl);

        glPopMatrix();

        glPushMatrix();
        glTranslatef(
          posBdlLblGridPrevBotRgt[i].x,
          posBdlLblGridPrevBotRgt[i].y,
          0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);

        VisUtils::setColor(colTxt);
        VisUtils::drawLabelLeft(
          texCharId,
          0.0 - 3*pix,
          0.0,
          txt*pix/CHARHEIGHT,
          lbl);

        glPopMatrix();

        VisUtils::setColor(colLne);
        VisUtils::drawLine(
          posBdlLblGridPrevTopLft[i].x,
          posBdlLblGridPrevBotRgt[i].x,
          posBdlLblGridPrevTopLft[i].y,
          posBdlLblGridPrevBotRgt[i].y);
      }
    }

    if (idxHiLite != NON_EXISTING &&  static_cast <size_t>(idxHiLite) < posBdlLblGridPrevTopLft.size())
    {
      string lbl = bundlesPrevByLbl[idxHiLite]->getChild(0)->getEdge(0)->getLabel();

      double txt = szeTxt;
      txt += 1;

      calcColor(
        bundlesPrevByLbl[idxHiLite]->getParent()->getIndex(),
        bundlesByLbl.size()-1,
        colLne);

      glPushMatrix();
      glTranslatef(
        posBdlLblGridPrevTopLft[idxHiLite].x,
        posBdlLblGridPrevTopLft[idxHiLite].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(colClr);
      VisUtils::fillRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(colTxt);
      VisUtils::drawLabel(
        texCharId,
        0.0 + 3*pix,
        0.0,
        txt*pix/CHARHEIGHT,
        lbl);

      glPopMatrix();

      glPushMatrix();
      glTranslatef(
        posBdlLblGridPrevBotRgt[idxHiLite].x,
        posBdlLblGridPrevBotRgt[idxHiLite].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(colClr);
      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(colTxt);
      VisUtils::drawLabelLeft(
        texCharId,
        0.0 - 3*pix,
        0.0,
        txt*pix/CHARHEIGHT,
        lbl);

      glPopMatrix();

      VisUtils::setColor(colLne);
      VisUtils::drawLine(
        posBdlLblGridPrevTopLft[idxHiLite].x,
        posBdlLblGridPrevBotRgt[idxHiLite].x,
        posBdlLblGridPrevTopLft[idxHiLite].y,
        posBdlLblGridPrevBotRgt[idxHiLite].y);
    }
  }
}


void Simulator::drawBdlLblGridNext(const bool& inSelectMode)
{
  if (inSelectMode == true)
  {
    double pix = canvas->getPixelSize();;

    glPushName(ID_BUNDLE_LBL);
    for (size_t i = 0; i < posBdlLblGridNextTopLft.size(); ++i)
    {
      string lbl = bundlesNextByLbl[i]->getChild(0)->getEdge(0)->getLabel();

      glPushName((GLuint) bundlesNextByLbl[i]->getParent()->getIndex());

      glPushMatrix();
      glTranslatef(
        posBdlLblGridNextTopLft[i].x,
        posBdlLblGridNextTopLft[i].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::fillRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(szeTxt*pix/CHARHEIGHT),
        0.5*szeTxt*pix, -0.5*szeTxt*pix);

      glPopMatrix();

      glPushMatrix();
      glTranslatef(
        posBdlLblGridNextBotRgt[i].x,
        posBdlLblGridNextBotRgt[i].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(szeTxt*pix/CHARHEIGHT)), 0.0,
        0.5*szeTxt*pix,                                    -0.5*szeTxt*pix);

      glPopMatrix();

      VisUtils::drawLine(
        posBdlLblGridNextTopLft[i].x,
        posBdlLblGridNextBotRgt[i].x,
        posBdlLblGridNextTopLft[i].y,
        posBdlLblGridNextBotRgt[i].y);

      glPopName();
    }
    glPopName();
  }
  else
  {
    ColorRGB colLne;
    double pix = canvas->getPixelSize();;
    size_t idxHiLite = NON_EXISTING;

    VisUtils::mapColorLtGray(colLne);

    for (size_t i = 0; i < posBdlLblGridNextTopLft.size(); ++i)
    {
      if (bundlesNextByLbl[i]->getParent()->getIndex() == fcsLblNextIdx)
      {
        idxHiLite = i;
      }
      else
      {
        string lbl = bundlesNextByLbl[i]->getChild(0)->getEdge(0)->getLabel();

        double txt = szeTxt;

        glPushMatrix();
        glTranslatef(
          posBdlLblGridNextTopLft[i].x,
          posBdlLblGridNextTopLft[i].y,
          0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);

        VisUtils::setColor(colTxt);
        VisUtils::drawLabel(
          texCharId,
          0.0 + 3*pix,
          0.0,
          txt*pix/CHARHEIGHT,
          lbl);

        glPopMatrix();

        glPushMatrix();
        glTranslatef(
          posBdlLblGridNextBotRgt[i].x,
          posBdlLblGridNextBotRgt[i].y,
          0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);

        VisUtils::setColor(colTxt);
        VisUtils::drawLabelLeft(
          texCharId,
          0.0 - 3*pix,
          0.0,
          txt*pix/CHARHEIGHT,
          lbl);

        glPopMatrix();

        VisUtils::setColor(colLne);
        VisUtils::drawLine(
          posBdlLblGridNextTopLft[i].x,
          posBdlLblGridNextBotRgt[i].x,
          posBdlLblGridNextTopLft[i].y,
          posBdlLblGridNextBotRgt[i].y);
      }
    }

    if (idxHiLite != NON_EXISTING &&  static_cast <size_t>(idxHiLite) < posBdlLblGridNextTopLft.size())
    {
      string lbl = bundlesNextByLbl[idxHiLite]->getChild(0)->getEdge(0)->getLabel();

      double txt = szeTxt;
      txt += 1;

      calcColor(
        bundlesNextByLbl[idxHiLite]->getParent()->getIndex(),
        bundlesByLbl.size()-1,
        colLne);

      glPushMatrix();
      glTranslatef(
        posBdlLblGridNextTopLft[idxHiLite].x,
        posBdlLblGridNextTopLft[idxHiLite].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(colClr);
      VisUtils::fillRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        0.0, (lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT) + pix,
        0.5*txt*pix, -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(colTxt);
      VisUtils::drawLabel(
        texCharId,
        0.0 + 3*pix,
        0.0,
        txt*pix/CHARHEIGHT,
        lbl);

      glPopMatrix();

      glPushMatrix();
      glTranslatef(
        posBdlLblGridNextBotRgt[idxHiLite].x,
        posBdlLblGridNextBotRgt[idxHiLite].y,
        0.0);
      glRotatef(45.0, 0.0, 0.0, 1.0);

      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(colClr);
      VisUtils::fillRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::setColor(colLne);
      VisUtils::drawRect(
        -((lbl.size()+1)*CHARWIDTH*(txt*pix/CHARHEIGHT)), 0.0 + pix,
        0.5*txt*pix,                                   -0.5*txt*pix - pix);
      VisUtils::disableLineAntiAlias();

      VisUtils::setColor(colTxt);
      VisUtils::drawLabelLeft(
        texCharId,
        0.0 - 3*pix,
        0.0,
        txt*pix/CHARHEIGHT,
        lbl);

      glPopMatrix();

      VisUtils::setColor(colLne);
      VisUtils::drawLine(
        posBdlLblGridNextTopLft[idxHiLite].x,
        posBdlLblGridNextBotRgt[idxHiLite].x,
        posBdlLblGridNextTopLft[idxHiLite].y,
        posBdlLblGridNextBotRgt[idxHiLite].y);
    }
  }
}


void Simulator::drawBundlesPrev(const bool& inSelectMode)
{
  if (inSelectMode == true)
  {
    double pix = canvas->getPixelSize();

    glPushName(ID_BUNDLE_LBL);
    for (size_t i = 0; i < posBundlesPrevTopLft.size(); ++i)
    {
      for (size_t j = 0; j < posBundlesPrevBotRgt[i].size(); ++j)
      {
        glPushName((GLuint) framesPrev[i]->getOutBundle(j)->getParent()->getParent()->getIndex());
        // arrow interval
        double arrowItv = 3;

        // draw
        VisUtils::fillArrow(
          posBundlesPrevTopLft[i][j].x, posBundlesPrevBotRgt[i][j].x,
          posBundlesPrevTopLft[i][j].y, posBundlesPrevBotRgt[i][j].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix);

        glPopName();
      }
    }
    glPopName();
  }
  else
  {
    double pix = canvas->getPixelSize();
    ColorRGB colFill, colFade;
    ColorRGB colBrdrFill, colBrdrFade;
    size_t idxHiLite = NON_EXISTING;

    VisUtils::setColorLtGray();
    VisUtils::enableLineAntiAlias();
    //VisUtils::mapColorMdGray( colFill );

    for (size_t i = 0; i < posBundlesPrevTopLft.size(); ++i)
    {
      idxHiLite = -1;
      for (size_t j = 0; j < posBundlesPrevBotRgt[i].size(); ++j)
      {
        // fade color
        if (framesPrev[i]->getOutBundle(j)->getParent()->getParent()->getIndex() == fcsLblPrevIdx)
        {
          idxHiLite = j;
        }
        else
        {
          // fill color
          calcColor(
            framesPrev[i]->getOutBundle(j)->getParent()->getParent()->getIndex(),
            bundlesByLbl.size()-1,
            colFill);
          VisUtils::mapColorMdGray(colBrdrFill);

          // fade color
          colFade       = colFill;
          colFade.a     = 0.2;
          colBrdrFade   = colBrdrFill;
          colBrdrFade.a = 0.2;

          // arrow interval
          double arrowItv = 3;

          // draw
          VisUtils::fillArrow(
            posBundlesPrevTopLft[i][j].x, posBundlesPrevBotRgt[i][j].x,
            posBundlesPrevTopLft[i][j].y, posBundlesPrevBotRgt[i][j].y,
            arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
            colFade, colFill);
          VisUtils::drawArrow(
            posBundlesPrevTopLft[i][j].x, posBundlesPrevBotRgt[i][j].x,
            posBundlesPrevTopLft[i][j].y, posBundlesPrevBotRgt[i][j].y,
            arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
            colBrdrFade, colBrdrFill);
        }
      }

      if (idxHiLite != NON_EXISTING &&  static_cast <size_t>(idxHiLite) < posBundlesPrevBotRgt[i].size())
      {
        // fill color
        calcColor(
          framesPrev[i]->getOutBundle(idxHiLite)->getParent()->getParent()->getIndex(),
          bundlesByLbl.size()-1,
          colFill);
        VisUtils::mapColorMdGray(colBrdrFill);
        //colBrdrFill.a = 1.2*colFill.a;

        // fade color
        colFade       = colFill;
        //colBrdrFade   = colFill;
        colBrdrFade   = colBrdrFill;

        // arrow interva
        double arrowItv = 3;
        arrowItv += 1;

        // draw
        VisUtils::fillArrow(
          posBundlesPrevTopLft[i][idxHiLite].x, posBundlesPrevBotRgt[i][idxHiLite].x,
          posBundlesPrevTopLft[i][idxHiLite].y, posBundlesPrevBotRgt[i][idxHiLite].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
          colFade, colFill);
        VisUtils::drawArrow(
          posBundlesPrevTopLft[i][idxHiLite].x, posBundlesPrevBotRgt[i][idxHiLite].x,
          posBundlesPrevTopLft[i][idxHiLite].y, posBundlesPrevBotRgt[i][idxHiLite].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
          colBrdrFade, colBrdrFill);
      }
    }

    VisUtils::disableLineAntiAlias();
  }
}


void Simulator::drawBundlesNext(const bool& inSelectMode)
{
  if (inSelectMode == true)
  {
    double pix = canvas->getPixelSize();
    double arrowItv;

    glPushName(ID_BUNDLE_LBL);
    for (size_t i = 0; i < posBundlesNextTopLft.size(); ++i)
    {
      for (size_t j = 0; j < posBundlesNextBotRgt[i].size(); ++j)
      {
        glPushName((GLuint) framesNext[i]->getInBundle(j)->getParent()->getParent()->getIndex());
        // arrow interval
        arrowItv = 3;

        // draw
        VisUtils::fillArrow(
          posBundlesNextTopLft[i][j].x, posBundlesNextBotRgt[i][j].x,
          posBundlesNextTopLft[i][j].y, posBundlesNextBotRgt[i][j].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix);

        glPopName();
      }
    }
    glPopName();
  }
  else
  {
    double pix = canvas->getPixelSize();
    ColorRGB colFill, colFade;
    ColorRGB colBrdrFill, colBrdrFade;
    size_t idxHiLite = NON_EXISTING;

    VisUtils::setColorLtGray();
    VisUtils::enableLineAntiAlias();
    //VisUtils::mapColorMdGray( colFill );

    for (size_t i = 0; i < posBundlesNextTopLft.size(); ++i)
    {
      idxHiLite = -1;

      for (size_t j = 0; j < posBundlesNextBotRgt[i].size(); ++j)
      {
        // fade color
        if (framesNext[i]->getInBundle(j)->getParent()->getParent()->getIndex() == fcsLblNextIdx)
        {
          idxHiLite = j;
        }
        else
        {
          // fill color
          calcColor(
            framesNext[i]->getInBundle(j)->getParent()->getParent()->getIndex(),
            bundlesByLbl.size()-1,
            colFill);
          VisUtils::mapColorMdGray(colBrdrFill);
          VisUtils::mapColorMdGray(colBrdrFill);

          // fade color
          colFade       = colFill;
          colFade.a     = 0.2;
          colBrdrFade   = colBrdrFill;
          colBrdrFade.a = 0.2;

          // arrow interva
          double arrowItv = 3;
          if (framesNext[i]->getInBundle(j)->getParent()->getParent()->getIndex() == fcsLblNextIdx)
          {
            arrowItv += 1;
          }

          // draw
          VisUtils::fillArrow(
            posBundlesNextTopLft[i][j].x-pix, posBundlesNextBotRgt[i][j].x,
            posBundlesNextTopLft[i][j].y, posBundlesNextBotRgt[i][j].y,
            arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
            colFade, colFill);
          VisUtils::drawArrow(
            posBundlesNextTopLft[i][j].x-pix, posBundlesNextBotRgt[i][j].x,
            posBundlesNextTopLft[i][j].y, posBundlesNextBotRgt[i][j].y,
            arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
            colBrdrFade, colBrdrFill);
        }
      }

      if (idxHiLite != NON_EXISTING &&  static_cast <size_t>(idxHiLite) < posBundlesNextBotRgt[i].size())
      {
        // fill color
        calcColor(
          framesNext[i]->getInBundle(idxHiLite)->getParent()->getParent()->getIndex(),
          bundlesByLbl.size()-1,
          colFill);
        VisUtils::mapColorMdGray(colBrdrFill);
        //colBrdrFill.a = 1.2*colFill.a;

        // fade color
        colFade       = colFill;
        //colBrdrFade   = colFill;
        colBrdrFade   = colBrdrFill;

        // arrow interva
        double arrowItv = 3;
        arrowItv += 1;

        // draw
        VisUtils::fillArrow(
          posBundlesNextTopLft[i][idxHiLite].x-pix, posBundlesNextBotRgt[i][idxHiLite].x,
          posBundlesNextTopLft[i][idxHiLite].y, posBundlesNextBotRgt[i][idxHiLite].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
          colFade, colFill);
        VisUtils::drawArrow(
          posBundlesNextTopLft[i][idxHiLite].x-pix, posBundlesNextBotRgt[i][idxHiLite].x,
          posBundlesNextTopLft[i][idxHiLite].y, posBundlesNextBotRgt[i][idxHiLite].y,
          arrowItv*pix, 3.0*arrowItv*pix, 3.0*arrowItv*pix,
          colBrdrFade, colBrdrFill);
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

  if (inSelectMode == true)
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
    VisUtils::setColorWhite();
    VisUtils::fillClearIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColorDkGray();
    VisUtils::drawClearIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    // up arrow
    x =  0.0;
    y = -0.5*hgt + 3.0*itvLrg + 2.0*pix + 4*pix;
    VisUtils::setColorWhite();
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorLtGray();
    VisUtils::drawRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y+itvLrg+pix);
    VisUtils::drawLine(x-itvLrg-pix, x-itvLrg-pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorDkGray();
    VisUtils::drawLine(x-itvLrg-pix, x+itvLrg+pix, y-itvLrg-pix, y-itvLrg-pix);
    VisUtils::drawLine(x+itvLrg+pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorCoolGreen();
    VisUtils::fillUpIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColorLtLtGray();
    VisUtils::drawUpIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    // right arrow
    x =  0.0 + 2*itvLrg + 4.0*pix;
    y = -0.5*hgt + 1.0*itvLrg + 2.0*pix;
    VisUtils::setColorWhite();
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorLtGray();
    VisUtils::drawRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y+itvLrg+pix);
    VisUtils::drawLine(x-itvLrg-pix, x-itvLrg-pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorDkGray();
    VisUtils::drawLine(x-itvLrg-pix, x+itvLrg+pix, y-itvLrg-pix, y-itvLrg-pix);
    VisUtils::drawLine(x+itvLrg+pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorCoolGreen();
    VisUtils::fillNextIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColorLtLtGray();
    VisUtils::drawNextIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    // down arrow
    x =  0.0;
    y = -0.5*hgt + 1.0*itvLrg + 2.0*pix;
    VisUtils::setColorWhite();
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorLtGray();
    VisUtils::drawRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y+itvLrg+pix);
    VisUtils::drawLine(x-itvLrg-pix, x-itvLrg-pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorDkGray();
    VisUtils::drawLine(x-itvLrg-pix, x+itvLrg+pix, y-itvLrg-pix, y-itvLrg-pix);
    VisUtils::drawLine(x+itvLrg+pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorCoolGreen();
    VisUtils::fillDownIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColorLtLtGray();
    VisUtils::drawDownIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    // left arrow
    x =  0.0 - 2.0*itvLrg - 4.0*pix;
    y = -0.5*hgt + 1.0*itvLrg + 2.0*pix;
    VisUtils::setColorWhite();
    VisUtils::fillRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorLtGray();
    VisUtils::drawRect(x-itvLrg-pix, x+itvLrg+pix, y+itvLrg+pix, y+itvLrg+pix);
    VisUtils::drawLine(x-itvLrg-pix, x-itvLrg-pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorDkGray();
    VisUtils::drawLine(x-itvLrg-pix, x+itvLrg+pix, y-itvLrg-pix, y-itvLrg-pix);
    VisUtils::drawLine(x+itvLrg+pix, x+itvLrg+pix, y+itvLrg+pix, y-itvLrg-pix);
    VisUtils::setColorCoolGreen();
    VisUtils::fillPrevIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColorLtLtGray();
    VisUtils::drawPrevIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);

    VisUtils::disableLineAntiAlias();
  }
}


void Simulator::animate()
{
  vector< double > valsFrame;

  if (keyFrameFr != NULL)
  {
    if (animPhase == ANIM_POS)
    {
      // 'new' current frame
      double x = posTweenFrame.x;
      double y = posTweenFrame.y;
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
      for (size_t j = 0; j < attributes.size(); ++j)
      {
        attr = attributes[j];
        node = keyFrameFr->getNode(0);
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
      glScalef(scaleDgrmHori, scaleDgrmHori, scaleDgrmHori);

      diagram->visualize(
        false,
        canvas,
        attributes,
        valsFrame);

      glPopMatrix();

      // 'old' current frame
      x = posKeyFrameTo.x;
      y = posKeyFrameTo.y;

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
      for (size_t j = 0; j < attributes.size(); ++j)
      {
        attr = attributes[j];
        node = keyFrameTo->getNode(0);
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
      glScalef(scaleDgrmHori, scaleDgrmHori, scaleDgrmHori);

      diagram->visualize(
        false,
        canvas,
        attributes,
        valsFrame);

      glPopMatrix();
    }
    else if (animPhase == ANIM_BLEND)
    {
      // 'new' current frame
      double x = posTweenFrame.x;
      double y = posTweenFrame.y;
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
      for (size_t j = 0; j < attributes.size(); ++j)
      {
        attr = attributes[j];
        node = keyFrameFr->getNode(0);
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
      glScalef(scaleDgrmHori, scaleDgrmHori, scaleDgrmHori);

      diagram->visualize(
        false,
        canvas,
        /*1.0-alphaKeyFrameFr,*/
        attributes,
        valsFrame);

      glPopMatrix();

      // 'old' current frame
      x = posKeyFrameTo.x;
      y = posKeyFrameTo.y;

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
      for (size_t j = 0; j < attributes.size(); ++j)
      {
        attr = attributes[j];
        node = keyFrameTo->getNode(0);
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
      glScalef(scaleDgrmHori, scaleDgrmHori, scaleDgrmHori);

      diagram->visualize(
        false,
        canvas,
        1.0-opacityKeyFrameTo,
        attributes,
        valsFrame);

      glPopMatrix();
    }
  }

  valsFrame.clear();
}


// -- utility event handlers ------------------------------------


void Simulator::onTimer(wxTimerEvent& /*e*/)
{
  if (timeAlphaMS >= timeTotalMS)
  {
    if (animPhase == ANIM_POS)
    {
      posTweenFrame.x = posKeyFrameTo.x;
      posTweenFrame.y = posKeyFrameTo.y;

      if (blendType != VisUtils::BLEND_HARD)
      {
        animPhase = ANIM_BLEND;

        timeTotalMS = 900;
        timeAlphaMS = 0;

        opacityKeyFrameFr = 1.0;
        opacityKeyFrameTo = 0.0;
      }
      else
      {
        timerAnim->Stop();
        canvas->enableMouseMotion();
        animPhase = ANIM_NONE;

        // update new data
        frameCurr = keyFrameFr;

        keyFrameFr = NULL;
        delete keyFrameTo;
        keyFrameTo = NULL;

        initFramesPrevNext();
        initBundles();
        sortFramesPrevNext();

        // init & visualize
        dataChanged = true;
        canvas->Refresh();
      }
    }
    else if (animPhase == ANIM_BLEND)
    {
      timerAnim->Stop();
      canvas->enableMouseMotion();
      animPhase = ANIM_NONE;

      // update new data
      frameCurr = keyFrameFr;

      keyFrameFr = NULL;
      delete keyFrameTo;
      keyFrameTo = NULL;

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
    double a = timeAlphaMS/timeTotalMS;

    if (animPhase == ANIM_POS)
    {
      posTweenFrame.x = (1-a)*posKeyFrameFr.x + a*posKeyFrameTo.x;
      posTweenFrame.y = (1-a)*posKeyFrameFr.y + a*posKeyFrameTo.y;
    }
    else if (animPhase == ANIM_BLEND)
    {
      if (blendType == VisUtils::BLEND_LINEAR)
      {
        opacityKeyFrameFr = (1-a)*0.0 + a*1.0;
        opacityKeyFrameTo = (1-a)*1.0 + a*0.0;
      }
      else if (blendType == VisUtils::BLEND_CONCAVE)
      {
        opacityKeyFrameFr = (a*1.0) * (a*1.0);
        opacityKeyFrameTo = 1.0 - opacityKeyFrameFr;
      }
      else if (blendType == VisUtils::BLEND_CONVEX)
      {
        opacityKeyFrameFr = sin(a*(PI/2.0));
        opacityKeyFrameTo = 1.0 - opacityKeyFrameFr;
      }
      else if (blendType == VisUtils::BLEND_OSCILLATE)
      {
        opacityKeyFrameFr = (-1.0*(cos(a*3.0*PI)/2.0) + 0.5);
        opacityKeyFrameTo = 1.0 - opacityKeyFrameFr;
      }
    }

    timeAlphaMS += itvTmrMS;

    canvas->Refresh();
    canvas->Update();
  }
}


// -- implement event table -----------------------------------------


BEGIN_EVENT_TABLE(Simulator, wxEvtHandler)
  EVT_TIMER(ID_TIMER, Simulator::onTimer)
END_EVENT_TABLE()


// -- end -----------------------------------------------------------
