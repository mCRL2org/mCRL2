// Author(s): Carst Tankink and Ali Deniz Aladagli
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file export_svg.h
/// \brief SVG exporter.

#ifndef SVG_EXPORT_H
#define SVG_EXPORT_H

#include "exporter.h"

class LTSGraph3d;

class ExporterSVG : public Exporter
{
  public:
    ExporterSVG(Graph* g, LTSGraph3d* app);
    ~ExporterSVG();
    bool export_to(wxString _filename);

  private:
    std::string svg_code;
    LTSGraph3d* owner;

    void drawBezier(Transition* tr);
    void drawSelfLoop(Transition* tr);
};

#endif //svg_export_h

