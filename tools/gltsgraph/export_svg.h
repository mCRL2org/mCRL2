#ifndef SVG_EXPORT_H
#define SVG_EXPORT_H

#include "exporter.h"

class ExporterSVG : public Exporter
{
  public:
    ExporterSVG(Graph* g);
    ~ExporterSVG();
    bool export_to(wxString _filename);

  private:
    std::string svg_code;

    void drawBezier(Transition* tr);
    void drawSelfLoop(Transition* tr);
};

#endif //svg_export_h

