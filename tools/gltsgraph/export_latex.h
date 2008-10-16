//  Copyright 2007 Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./export_latex.h

#ifndef LATEX_EXPORT_H
#define LATEX_EXPORT_H

#include "exporter.h"

class ExporterLatex: public Exporter
{
  public:
    ExporterLatex(Graph* g);
    ~ExporterLatex() {};
    bool export_to(wxString _filename);

  private:
    std::string tikz_code;
    
    void drawBezier(Transition* tr);
    void drawSelfLoop(Transition* tr);
};



#endif //LATEX_EXPORT_H
