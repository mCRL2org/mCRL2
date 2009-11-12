// Author(s): Carst Tankink and Ali Deniz Aladagli
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file export_latex.h
/// \brief LaTeX exporter.

#ifndef LATEX_EXPORT_H
#define LATEX_EXPORT_H

#include "exporter.h"
class LTSGraph3d;

class ExporterLatex: public Exporter
{
  public:
    ExporterLatex(Graph* g, LTSGraph3d* app);
    ~ExporterLatex() {};
    bool export_to(wxString _filename);

  private:
    LTSGraph3d* owner;
    std::string tikz_code;

    void drawBezier(Transition* tr);
    void drawSelfLoop(Transition* tr);
};



#endif //LATEX_EXPORT_H
