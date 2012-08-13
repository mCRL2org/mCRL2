// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./mediator.h

// This is an ABSTRACT CLASS with a number of PURE VIRUAL FUNCTIONS.
// This serves as an INTERFACE that that should be implemented by
// subclasses. This allows for the implementation of a MEDIATOR
// design pattern where an instance of a subclass serves as the
// mediator or controller.

#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#ifndef NON_EXISTING
#define NON_EXISTING (size_t)(-1)
#endif

class Attribute;
class Cluster;
class Colleague;

class Mediator
{
  public:
    virtual ~Mediator() {}

    virtual void initProgress(
      const std::string& title,
      const std::string& msg,
      const size_t& max) = 0;
    virtual void updateProgress(const size_t& val) = 0;
    virtual void closeProgress() = 0;

    virtual QColor getColor(QColor col) = 0;

    virtual void handleAttributeCluster(const std::vector< size_t > &indcs) = 0;

    virtual void handleNote(const size_t& shapeId, const std::string& msg) = 0;
    virtual void handleEditModeDOF(Colleague* c) = 0;

    virtual void handleEditShape(
      const bool& cut,
      const bool& copy,
      const bool& paste,
      const bool& clear,
      const bool& bringToFront,
      const bool& sendToBack,
      const bool& bringForward,
      const bool& sendBackward,
      const bool& editDOF,
      const int&  checkedItem) = 0;

    virtual void handleEditDOF(
      const std::vector< size_t > &degsOfFrdmIds,
      const std::vector< std::string > &degsOfFrdm,
      const std::vector< size_t > &attrIndcs,
      const size_t& selIdx) = 0;
    virtual void handleDOFSel(const size_t& DOFIdx) = 0;

    virtual void setDOFColorSelected() = 0;
    virtual void setDOFOpacitySelected() = 0;

    virtual void handleSetDOFTextStatus(
      const size_t& DOFIdx,
      const int& status) = 0;
    virtual size_t handleGetDOFTextStatus(const size_t& DOFIdx) = 0;

    virtual void handleDOFColActivate() = 0;
    virtual void handleDOFColDeactivate() = 0;
    virtual void handleDOFColAdd(
      const double& hue,
      const double& y) = 0;
    virtual void handleDOFColUpdate(
      const size_t& idx,
      const double& hue,
      const double& y) = 0;
    virtual void handleDOFColClear(
      const size_t& idx) = 0;
    virtual void handleDOFColSetValuesEdt(
      const std::vector< double > &hue,
      const std::vector< double > &y) = 0;

    virtual void handleDOFOpaActivate() = 0;
    virtual void handleDOFOpaDeactivate() = 0;
    virtual void handleDOFOpaAdd(
      const double& hue,
      const double& y) = 0;
    virtual void handleDOFOpaUpdate(
      const size_t& idx,
      const double& hue,
      const double& y) = 0;
    virtual void handleDOFOpaClear(
      const size_t& idx) = 0;
    virtual void handleDOFOpaSetValuesEdt(
      const std::vector< double > &hue,
      const std::vector< double > &y) = 0;

    virtual void handleLinkDOFAttr(
      const size_t DOFIdx,
      const size_t attrIdx) = 0;
    virtual void handleUnlinkDOFAttr(const size_t DOFIdx) = 0;
    virtual void handleDOFFrameDestroy() = 0;
    virtual void handleDOFDeselect() = 0;

    virtual void markTimeSeries(
      Colleague* sender,
      Cluster* currFrame) = 0;
    virtual void markTimeSeries(
      Colleague* sender,
      const std::vector< Cluster* > frames) = 0;

    virtual void handleAnimFrameClust(Colleague* sender) = 0;

    virtual void handleMarkFrameClust(Colleague* sender) = 0;
    virtual void handleUnmarkFrameClusts(Colleague* sender) = 0;
};

#endif
