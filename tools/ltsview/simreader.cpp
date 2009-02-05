// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simreader.cpp
/// \brief Source file for simReader class

#include "wx.hpp" // precompiled headers

#include "simreader.h"

simReader::simReader(Simulation* s)
  : sim(s)
{
  if (s != NULL) {
    connection = sim->connect(boost::bind(&simReader::refresh, this));
    chooseConnection = sim->connectSel(boost::bind(&simReader::selChange,
                                       this));
  }
}

simReader::~simReader()
{
  sim->disconnect(connection);
}

void simReader::setSim(Simulation* s)
{
  // Disconnect from old sim
  sim->disconnect(connection);
  sim->disconnect(chooseConnection);

  // Connect to new sim

  sim = s;
  connection = sim->connect(boost::bind(&simReader::refresh, this));
  chooseConnection = sim->connectSel(boost::bind(&simReader::selChange,
                                       this));
}
