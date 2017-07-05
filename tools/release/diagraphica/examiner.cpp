// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./examiner.cpp

#include "examiner.h"

#include <QMessageBox>
#include <QMenu>
#include <iostream>
using namespace std;


static const int hgtHstPix = 80;

// -- constructors and destructor -----------------------------------


Examiner::Examiner(
  QWidget *parent,
  Settings* s,
  Graph* g)
  : Visualizer(parent, g),
    settings(s)
{
  diagram = 0;
  frame = 0;
  colFrm = VisUtils::mediumGray;

  focusFrameIdx = -1;
  offset = 0;

  connect(&settings->backgroundColor, SIGNAL(changed(QColor)), this, SLOT(update()));
  connect(&settings->textColor, SIGNAL(changed(QColor)), this, SLOT(update()));
  connect(&settings->textSize, SIGNAL(changed(int)), this, SLOT(update()));
  
  connect(m_graph, SIGNAL(deletedAttribute()), this, SLOT(clearData()));
}


Examiner::~Examiner()
{
  // association
  diagram = 0;
  attributes.clear();

  // composition
  delete frame;
  frame = 0;

  // composition
  {
    for (std::size_t i = 0; i < framesHist.size(); ++i)
    {
      delete framesHist[i];
    }
  }
  framesHist.clear();

  // association
  {
    for (std::size_t i = 0; i < attrsHist.size(); ++i)
    {
      attrsHist[i].clear();
    }
  }
  attrsHist.clear();
}


// -- get functions -------------------------------------------------


std::size_t Examiner::selectedClusterIndex()
{
  std::size_t result = NON_EXISTING;
  if (focusFrameIdx < framesHist.size())
  {
    result = framesHist[focusFrameIdx]->getNode(0)->getCluster()->getIndex();
  }
  return result;
}


// -- set functions -------------------------------------------------


void Examiner::setFrame(
  Cluster* frme,
  const vector< Attribute*> &attrs,
  QColor col)
{
  delete frame;
  frame = new Cluster(*frme);

  attributes = attrs;
  colFrm = col;

  update();
  emit selectionChanged();
}


void Examiner::clrFrame()
{
  delete frame;
  frame = 0;

  if (focusFrameIdx < framesHist.size())
  {
    setFrame(framesHist[focusFrameIdx], attrsHist[focusFrameIdx], VisUtils::coolRed);
  }
  else
  {
    attributes.clear();
    colFrm = VisUtils::mediumGray;
  }

  update();
  emit selectionChanged();
}


void Examiner::addFrameHist(
  Cluster* frme,
  const vector< Attribute* > &attrs)
{
  // update flag
  dataChanged = true;

  framesHist.push_back(new Cluster(*frme));

  vector< Attribute* > v;
  attrsHist.push_back(attrs);

  update();
}


void Examiner::addFrameHist(
  QList<Cluster*> frames,
  const std::vector< Attribute* > &attrs)
{
  for (int i = 0; i < frames.size(); ++i)
  {
    addFrameHist(frames[i], attrs);
  }
}



void Examiner::clrFrameHistCur()
{
  // update flag
  dataChanged = true;

  if (focusFrameIdx < framesHist.size())
  {
    // composition
    delete framesHist[focusFrameIdx];
    framesHist.erase(framesHist.begin() + focusFrameIdx);

    // association
    attrsHist.erase(attrsHist.begin() + focusFrameIdx);

    // update focus
    focusFrameIdx = -1;

    clrFrame();
  }

  update();
}


void Examiner::clearData()
{
  clearAttributes();
  clearFrames();
  update();
}


// -- visualization functions  --------------------------------------


void Examiner::visualize(const bool& inSelectMode)
{
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
    GLint hits = 0;
    GLuint selectBuf[512];
    startSelectMode(
      hits,
      selectBuf,
      2.0,
      2.0);

    if (diagram != 0)
    {
      drawFrame(inSelectMode);

      if (framesHist.size() > 0)
      {
        drawFramesHist(inSelectMode);
        drawControls(inSelectMode);
      }
    }

    finishSelectMode(
      hits,
      selectBuf);
  }
  else
  {
    if (diagram != 0)
    {
      drawFrame(inSelectMode);

      if (framesHist.size() > 0)
      {
        drawFramesHist(inSelectMode);
        drawControls(inSelectMode);
      }
    }
  }
}


// -- event handlers ------------------------------------------------


void Examiner::handleSizeEvent()
{
  Visualizer::handleSizeEvent();

  double bdr     = 10;
  double pix = pixelSize();

  if (posFramesHist.size() > 0)
  {
    // update offset if necessary
    if ((posFramesHist[posFramesHist.size()-1].x + offset*pix + scaleFramesHist*1.0) < (0.5*worldSize().width() - bdr*pix)  &&
        (offset < 0))
    {
      offset += ((0.5*worldSize().width() - bdr*pix) - (posFramesHist[posFramesHist.size()-1].x + scaleFramesHist*1.0))/pix;

      if (offset > 0)
      {
        offset = 0;
      }
    }
  }
}


void Examiner::handleMouseEvent(QMouseEvent* e)
{
  Visualizer::handleMouseEvent(e);

  // redraw in select mode
  updateGL(true);
  // redraw in render mode
  updateGL();
}


void Examiner::handleKeyEvent(QKeyEvent* e)
{
  Visualizer::handleKeyEvent(e);

  if (e->type() == QEvent::KeyPress)
  {
    if (e->key() == Qt::Key_Right)
    {
      handleIconRgt();
    }
    else if (e->key() == Qt::Key_Left)
    {
      handleIconLft();
    }

    // redraw in render mode
    updateGL();
  }
}


// -- utility functions ---------------------------------------------


void Examiner::calcSettingsGeomBased()
{
  // update flag
  geomChanged = false;

  calcPosFrame();
  calcPosFramesHist();
}


void Examiner::calcSettingsDataBased()
{
  // update flag
  dataChanged = false;

  calcPosFrame();
  calcPosFramesHist();
}


void Examiner::calcPosFrame()
{
  double itvHist = hgtHstPix;
  double bdr     = 10;

  double pix = pixelSize();

  // position & scaling
  posFrame.x = 0.0;
  if (framesHist.size() > 0)
  {
    // frame
    posFrame.y = 0.0 + 0.5*itvHist*pix;
    scaleFrame = Utils::minn((worldSize().width() - 2.0*bdr*pix), (worldSize().height() - itvHist*pix - 2.0*bdr*pix))/2.0;
  }
  else
  {
    posFrame.y = 0.0;
    scaleFrame = Utils::minn((worldSize().width() - 2.0*bdr*pix), (worldSize().height() - 2.0*bdr*pix))/2.0;
  }
}


void Examiner::calcPosFramesHist()
{
  double itvHist = hgtHstPix;
  double bdr     = 10;

  double pix = pixelSize();

  vsblHistIdxLft = 0;
  vsblHistIdxRgt = framesHist.size()-1;

  if (framesHist.size() > 0)
  {
    // frames history
    posFramesHist.clear();
    Position2D pos;
    pos.y = -0.5*worldSize().height() + 0.5*itvHist*pix;
    for (std::size_t i = 0; i < framesHist.size(); ++i)
    {
      pos.x = -0.5*worldSize().width() + bdr*pix + 0.5*itvHist*pix + i*itvHist*pix + offset*pix;
      posFramesHist.push_back(pos);

      if (pos.x + bdr*pix < -0.5*worldSize().width())
      {
        ++vsblHistIdxLft;
      }
      if (0.5*worldSize().width() < pos.x - bdr*pix)
      {
        --vsblHistIdxRgt;
      }
    }

    scaleFramesHist = 0.40*itvHist*pix;
  }
}


void Examiner::clearAttributes()
{
  // association
  attributes.clear();
}


void Examiner::clearDiagram()
{
  // association
  diagram = 0;
}


void Examiner::clearFrames()
{
  // composition
  delete frame;
  frame = 0;
}


// -- hit detection -------------------------------------------------


void Examiner::handleHits(const vector< int > &ids)
{
  if (ids.size() > 0)
  {
    if (m_lastMouseEvent.type() == QEvent::MouseButtonPress && m_lastMouseEvent.button() == Qt::LeftButton)
    {
      if (ids[0] == ID_FRAME)
      {
        if (ids.size() == 2 && ids[1] == ID_ICON_MORE)
        {
          emit routingCluster(frame, QVector<Cluster *>::fromStdVector(framesHist).toList(), QVector<Attribute *>::fromStdVector(attributes).toList());
        }
      }
      else if (ids[0] == ID_FRAME_HIST)
      {
        if (focusFrameIdx == static_cast <std::size_t>(ids[1]))
        {
          focusFrameIdx = -1;
          clrFrame();
        }
        else
        {
          focusFrameIdx = ids[1];

          setFrame(framesHist[focusFrameIdx], attrsHist[focusFrameIdx], VisUtils::coolRed);
        }
      }
      else if (ids[ids.size()-1] == ID_ICON_CLR)
      {
        if(QMessageBox::question(this, "Confirm examiner clear", "Are you sure you want to clear the examiner history?", QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
        {
          dataChanged = true;

          for (std::size_t i = 0; i < framesHist.size(); ++i)
          {
            delete framesHist[i];
          }
          framesHist.clear();
          attrsHist.clear();

          focusFrameIdx = -1;
          offset = 0;

          update();
        }
      }
      else if (ids[ids.size()-1] == ID_ICON_RWND)
      {
        handleIconRwnd();
      }
      else if (ids[ids.size()-1] == ID_ICON_LFT)
      {
        handleIconLft();
      }
      else if (ids[ids.size()-1] == ID_ICON_RGT)
      {
        handleIconRgt();
      }

    }
    else if (m_lastMouseEvent.type() == QEvent::MouseButtonPress && m_lastMouseEvent.button() == Qt::RightButton)
    {
      if (ids[0] == ID_FRAME)
      {
        emit routingCluster(frame, QVector<Cluster *>::fromStdVector(framesHist).toList(), QVector<Attribute *>::fromStdVector(attributes).toList());
      }
      else if (ids[0] == ID_FRAME_HIST)
      {
        focusFrameIdx = ids[1];

        setFrame(framesHist[focusFrameIdx], attrsHist[focusFrameIdx], VisUtils::coolRed);

        QMenu *menu = new QMenu();
        connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater()));

        QAction *deleteCluster = menu->addAction("Delete");
        connect(deleteCluster, SIGNAL(triggered()), this, SLOT(clrFrameHistCur()));

        menu->popup(QCursor::pos());
      }
    }
  }
}


void Examiner::handleIconRwnd()
{
  if (framesHist.size() > 0)
  {
    focusFrameIdx = 0;
    offset        = 0;
    geomChanged   = true;

    setFrame(framesHist[focusFrameIdx], attrsHist[focusFrameIdx], VisUtils::coolRed);
  }
  else
  {
    offset      = 0;
    geomChanged = true;
  }
}


void Examiner::handleIconLft()
{
  double bdr = 12;

  double pix = pixelSize();

  if (framesHist.size() > 0)
  {
    if (focusFrameIdx < framesHist.size())
    {
      if (focusFrameIdx != 0)
      {
        focusFrameIdx -= 1;
      }

      double dLft = (-0.5*worldSize().width() + bdr*pix) - (posFramesHist[focusFrameIdx].x - scaleFramesHist*1.0);
      double dRgt = (posFramesHist[focusFrameIdx].x + scaleFramesHist*1.0 + 4*pix) - (0.5*worldSize().width() - bdr*pix);
      if (dRgt > 0)
      {
        offset -= dRgt/pix;
      }
      else if (dLft > 0)
      {
        offset += dLft/pix;
      }

      setFrame(framesHist[focusFrameIdx], attrsHist[focusFrameIdx], VisUtils::coolRed);

      geomChanged = true;
    }
    else
    {
      if ((posFramesHist[0].x - scaleFramesHist*1.0) < (-0.5*worldSize().width() + bdr*pix))
      {
        offset += 10;
      }
      geomChanged = true;
    }
  }
  else
  {
    offset = 0;
    geomChanged = true;
  }
}


void Examiner::handleIconRgt()
{
  double bdr = 12;

  double pix = pixelSize();

  if (framesHist.size() > 0)
  {
    if (focusFrameIdx < framesHist.size())
    {
      if (focusFrameIdx < framesHist.size()-1)
      {
        focusFrameIdx += 1;
      }

      double dLft = (-0.5*worldSize().width() + bdr*pix) - (posFramesHist[focusFrameIdx].x - scaleFramesHist*1.0);
      double dRgt = (posFramesHist[focusFrameIdx].x + scaleFramesHist*1.0 + 4*pix) - (0.5*worldSize().width() - bdr*pix);
      if (dRgt > 0)
      {
        offset -= dRgt/pix;
      }
      else if (dLft > 0)
      {
        offset += dLft/pix;
      }

      setFrame(framesHist[focusFrameIdx], attrsHist[focusFrameIdx], VisUtils::coolRed);

      geomChanged = true;
    }
    else
    {
      if ((posFramesHist[posFramesHist.size()-1].x + scaleFramesHist*1.0) > (0.5*worldSize().width() - bdr*pix))
      {
        offset -= 10;
      }
      geomChanged = true;
    }
  }
  else
  {
    offset = 0;
    geomChanged = true;
  }
}


void Examiner::processHits(
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


// -- utility drawing functions -------------------------------------


void Examiner::clear()
{
  VisUtils::clear(settings->backgroundColor.value());
}


void Examiner::drawFrame(const bool& inSelectMode)
{
  double pix = pixelSize();

  if (inSelectMode == true)
  {
    glPushMatrix();
    glTranslatef(posFrame.x, posFrame.y, 0.0);
    glScalef(scaleFrame, scaleFrame, scaleFrame);

    glPushName(ID_FRAME);
    VisUtils::fillRect(- 1.0, 1.0, 1.0, -1.0);
    glPushName(ID_ICON_MORE);
    VisUtils::fillRect(-0.98, -0.98+14*pix, -0.98+14*pix, -0.98);
    glPopName();
    glPopName();

    glPopMatrix();
  }
  else
  {
    glPushMatrix();
    glTranslatef(posFrame.x, posFrame.y, 0.0);
    glScalef(scaleFrame, scaleFrame, scaleFrame);

    VisUtils::setColor(colFrm);
    VisUtils::fillRect(
      -1.0 + 4*pix/scaleFrame,  1.0 + 4*pix/scaleFrame,
      1.0 - 4*pix/scaleFrame, -1.0 - 4*pix/scaleFrame);

    vector< double > valsFrame;
    /*
    for ( int i = 0; i < attributes.size(); ++i )
        valsFrame.push_back(
           attributes[i]->mapToValue(
                frame->getNode(0)->getTupleVal(
                    attributes[i]->getIndex() ) )->getIndex() );
    */
    Attribute* attr;
    Node* node;
    for (std::size_t i = 0; i < attributes.size(); ++i)
    {
      attr = attributes[i];
      node = frame->getNode(0);
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

    diagram->visualize(
      false,
      pixelSize(),
      attributes,
      valsFrame);

    VisUtils::enableLineAntiAlias();
    VisUtils::setColor(colFrm);
    VisUtils::fillMoreIcon(-0.98, -0.98+14*pix, -0.98+14*pix, -0.98);
    VisUtils::setColor(VisUtils::lightLightGray);
    VisUtils::drawMoreIcon(-0.98, -0.98+14*pix, -0.98+14*pix, -0.98);
    VisUtils::disableLineAntiAlias();

    glPopMatrix();
  }
}


void Examiner::drawFramesHist(const bool& inSelectMode)
{
  if (inSelectMode == true)
  {
    glPushName(ID_FRAME_HIST);
    //for ( int i = 0; i < framesHist.size(); ++i )
    for (std::size_t i = vsblHistIdxLft; i <= vsblHistIdxRgt; ++i)
    {
      glPushMatrix();
      glTranslatef(posFramesHist[i].x, posFramesHist[i].y, 0.0);
      glScalef(scaleFramesHist, scaleFramesHist, scaleFramesHist);

      glPushName((GLuint) i);
      VisUtils::fillRect(
        -1.0,  1.0,
        1.0, -1.0);
      glPopName();

      glPopMatrix();
    }
    glPopName();
  }
  else
  {
    double pix = pixelSize();
    vector< double > valsFrame;

    //for ( int i = 0; i < framesHist.size(); ++i )
    for (std::size_t i = vsblHistIdxLft; i <= vsblHistIdxRgt; ++i)
    {
      valsFrame.clear();
      /*
      for ( int j = 0; j < attrsHist[i].size(); ++j )
          valsFrame.push_back(
              attrsHist[i][j]->mapToValue(
                  framesHist[i]->getNode(0)->getTupleVal(
                      attrsHist[i][j]->getIndex() ) )->getIndex() );
      */
      Attribute* attr;
      Node* node;
      for (std::size_t j = 0; j < attrsHist[i].size(); ++j)
      {
        attr = attrsHist[i][j];
        node = framesHist[i]->getNode(0);
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
      glTranslatef(posFramesHist[i].x, posFramesHist[i].y, 0.0);
      glScalef(scaleFramesHist, scaleFramesHist, scaleFramesHist);

      if (i == focusFrameIdx)
      {
        VisUtils::setColor(VisUtils::coolRed);
        VisUtils::fillRect(
          -1.0 + 4*pix/scaleFramesHist,  1.0+4*pix/scaleFramesHist,
          1.0 - 4*pix/scaleFramesHist, -1.0-4*pix/scaleFramesHist);
      }
      else
      {
        VisUtils::setColor(VisUtils::mediumGray);
        VisUtils::fillRect(
          -1.0 + 3*pix/scaleFramesHist,  1.0+3*pix/scaleFramesHist,
          1.0 - 3*pix/scaleFramesHist, -1.0-3*pix/scaleFramesHist);
      }

      diagram->visualize(
        false,
        pixelSize(),
        attrsHist[i],
        valsFrame);

      glPopMatrix();
    }
  }
}


void Examiner::drawControls(const bool& inSelectMode)
{
  double itvHist = hgtHstPix;

  double pix = pixelSize();

  if (inSelectMode == true)
  {
    // clear icon
    double itvSml = 6.0*pix;
    double x = 0.5*worldSize().width() - itvSml - pix;
    double y = 0.5*worldSize().height() - itvSml - pix;
    glPushName(ID_ICON_CLR);
    VisUtils::fillRect(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    glPopName();

    // rewind
    glPushName(ID_ICON_RWND);
    glPushMatrix();
    glTranslatef(-18.0*pix, -0.5*worldSize().height() + itvHist*pix, 0.0);
    VisUtils::fillRect(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    glPopMatrix();
    glPopName();

    // left
    glPushName(ID_ICON_LFT);
    glPushMatrix();
    glTranslatef(-6*pix, -0.5*worldSize().height() + itvHist*pix, 0.0);
    VisUtils::fillRect(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    glPopMatrix();
    glPopName();

    // left
    glPushName(ID_ICON_LFT);
    glPushMatrix();
    glTranslatef(-0.5*worldSize().width()+6*pix, -0.5*worldSize().height() + 0.5*itvHist*pix, 0.0);
    VisUtils::fillRect(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    glPopMatrix();
    glPopName();

    // play
    glPushName(ID_ICON_PLAY);
    glPushMatrix();
    glTranslatef(6*pix, -0.5*worldSize().height() + itvHist*pix, 0.0);
    VisUtils::fillRect(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    glPopMatrix();
    glPopName();

    // right
    glPushName(ID_ICON_RGT);
    glPushMatrix();
    glTranslatef(18*pix, -0.5*worldSize().height() + itvHist*pix, 0.0);
    VisUtils::fillRect(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    glPopMatrix();
    glPopName();

    // right
    glPushName(ID_ICON_RGT);
    glPushMatrix();
    glTranslatef(0.5*worldSize().width()-6*pix, -0.5*worldSize().height() + 0.5*itvHist*pix, 0.0);
    VisUtils::fillRect(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    glPopMatrix();
    glPopName();
  }
  else
  {
    // clear icon
    double itvSml = 6.0*pix;
    double x = 0.5*worldSize().width() - itvSml - pix;
    double y = 0.5*worldSize().height() - itvSml - pix;
    double bdr = 10;
    double dLft = 0;
    double dRgt = 0;

    if (posFramesHist.size() > 1)
    {
      dLft = (-0.5*worldSize().width() + bdr*pix) - (posFramesHist[0].x - scaleFramesHist*1.0);
      dRgt = (posFramesHist[posFramesHist.size()-1].x + scaleFramesHist*1.0) - (0.5*worldSize().width() - bdr*pix);
    }
    else
    {
      if (0 < focusFrameIdx && focusFrameIdx < posFramesHist.size())
      {
        dLft = 1;
      }

      if (focusFrameIdx < posFramesHist.size()-1)
      {
        dRgt = 1;
      }
    }

    VisUtils::enableLineAntiAlias();
    VisUtils::setColor(Qt::white);
    VisUtils::fillClearIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::setColor(VisUtils::darkGray);
    VisUtils::drawClearIcon(x-itvSml, x+itvSml, y+itvSml, y-itvSml);
    VisUtils::disableLineAntiAlias();

    // border
    VisUtils::setColor(settings->backgroundColor.value());
    VisUtils::fillRect(
      -0.5*worldSize().width(),               -0.5*worldSize().width() + 12.0*pix,
      -0.5*worldSize().height() + itvHist*pix, -0.5*worldSize().height());
    VisUtils::fillRect(
      0.5*worldSize().width() - 12.0*pix,      0.5*worldSize().width(),
      -0.5*worldSize().height() + itvHist*pix, -0.5*worldSize().height());

    // lines
    VisUtils::setColor(VisUtils::lightGray);
    VisUtils::drawLine(
      -0.5*worldSize().width() + 6.0*pix,     -26.0*pix,
      -0.5*worldSize().height() + itvHist*pix, -0.5*worldSize().height() + itvHist*pix);
    /*
    VisUtils::drawLine(
        -0.5*worldSize().width() + 6.0*pix,     -0.5*worldSize().width() + 6.0*pix,
        -0.5*worldSize().height() + itvHist*pix, -0.5*worldSize().height() + 0.5*itvHist*pix + 8.0*pix );
    */
    VisUtils::drawLine(
      26.0*pix,                0.5*worldSize().width() - 6.0*pix,
      -0.5*worldSize().height() + itvHist*pix, -0.5*worldSize().height() + itvHist*pix);
    /*
    VisUtils::drawLine(
        0.5*worldSize().width() - 6.0*pix,       0.5*worldSize().width() - 6.0*pix,
        -0.5*worldSize().height() + itvHist*pix, -0.5*worldSize().height() + 0.5*itvHist*pix + 8.0*pix );
    */

    // rewind
    glPushMatrix();
    glTranslatef(-18.0*pix, -0.5*worldSize().height() + itvHist*pix, 0.0);

    VisUtils::enableLineAntiAlias();
    if (dLft > 0)
    {
      VisUtils::setColor(VisUtils::coolRed);
    }
    else
    {
      VisUtils::setColor(VisUtils::mediumGray);
    }
    VisUtils::fillRwndIcon(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    VisUtils::setColor(VisUtils::lightLightGray);
    VisUtils::drawRwndIcon(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    VisUtils::disableLineAntiAlias();

    glPopMatrix();

    // left
    glPushMatrix();
    glTranslatef(-6*pix, -0.5*worldSize().height() + itvHist*pix, 0.0);

    VisUtils::enableLineAntiAlias();
    if (dLft > 0)
    {
      VisUtils::setColor(VisUtils::coolRed);
    }
    else
    {
      VisUtils::setColor(VisUtils::mediumGray);
    }
    VisUtils::fillPrevIcon(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    VisUtils::setColor(VisUtils::lightLightGray);
    VisUtils::drawPrevIcon(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    VisUtils::disableLineAntiAlias();

    glPopMatrix();

    // left
    if (dLft > 0)
    {
      glPushMatrix();
      glTranslatef(-0.5*worldSize().width()+6*pix, -0.5*worldSize().height() + 0.5*itvHist*pix, 0.0);
      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(VisUtils::coolRed);
      VisUtils::fillPrevIcon(
        -5.0*pix,  5.0*pix,
        5.0*pix, -5.0*pix);
      VisUtils::setColor(VisUtils::lightLightGray);
      VisUtils::drawPrevIcon(
        -5.0*pix,  5.0*pix,
        5.0*pix, -5.0*pix);
      VisUtils::disableLineAntiAlias();
      glPopMatrix();
    }

    // play
    glPushMatrix();
    glTranslatef(6*pix, -0.5*worldSize().height() + itvHist*pix, 0.0);

    VisUtils::enableLineAntiAlias();
    //VisUtils::setColor(VisUtils::coolRed);
    VisUtils::setColor(VisUtils::mediumGray);
    VisUtils::fillPlayIcon(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    VisUtils::setColor(VisUtils::lightLightGray);
    VisUtils::drawPlayIcon(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    VisUtils::disableLineAntiAlias();

    glPopMatrix();

    // right
    glPushMatrix();
    glTranslatef(18*pix, -0.5*worldSize().height() + itvHist*pix, 0.0);

    VisUtils::enableLineAntiAlias();
    if (dRgt > 0)
    {
      VisUtils::setColor(VisUtils::coolRed);
    }
    else
    {
      VisUtils::setColor(VisUtils::mediumGray);
    }
    VisUtils::fillNextIcon(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    VisUtils::setColor(VisUtils::lightLightGray);
    VisUtils::drawNextIcon(
      -5.0*pix,  5.0*pix,
      5.0*pix, -5.0*pix);
    VisUtils::disableLineAntiAlias();

    glPopMatrix();

    // right
    if (dRgt > 0)
    {
      glPushMatrix();
      glTranslatef(0.5*worldSize().width()-6*pix, -0.5*worldSize().height() + 0.5*itvHist*pix, 0.0);
      VisUtils::enableLineAntiAlias();
      VisUtils::setColor(VisUtils::coolRed);
      VisUtils::fillNextIcon(
        -5.0*pix,  5.0*pix,
        5.0*pix, -5.0*pix);
      VisUtils::setColor(VisUtils::lightLightGray);
      VisUtils::drawNextIcon(
        -5.0*pix,  5.0*pix,
        5.0*pix, -5.0*pix);
      VisUtils::disableLineAntiAlias();
      glPopMatrix();
    }
  }
}
