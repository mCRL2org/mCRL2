// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file exporter.h
/// \brief Superclass for exporters

#ifndef EXPORTER_H
#define EXPORTER_H

#include <wx/wx.h>
#include "graph.h"
#include <wx/string.h>

class Exporter
{
  public:

    inline Exporter(Graph* g) : graph(g)
    { }

    inline virtual ~Exporter()
    {};

    virtual bool export_to(wxString _filename) = 0;
  protected:
    Graph* graph;
};

#endif //EXPORTER_H
