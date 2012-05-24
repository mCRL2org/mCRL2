// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mediator.h
/// \brief Header file for Mediator class

#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QColor>

#include <string>
#include "enums.h"

class State;
class Cluster;
class Transition;

class Mediator
{
  public:
    virtual ~Mediator() {}
    virtual void exportToText(std::string filename) = 0;
    virtual std::string getVersionString() = 0;
    virtual void notifyRenderingFinished() = 0;
    virtual void notifyRenderingStarted() = 0;
    virtual void openFile(std::string fileName) = 0;
    virtual void selectStateByID(const int id) = 0;
    virtual void selectCluster(const int rank, const int pos) = 0;
    virtual void deselect() = 0;
    virtual int getNumberOfObjects() = 0;
    virtual void zoomInBelow() = 0;
    virtual void zoomInAbove() = 0;
    virtual void zoomOut() = 0;

    // Loads a trace from path
    virtual void loadTrace(std::string const& path) = 0;
};

#endif
