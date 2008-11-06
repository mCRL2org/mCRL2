#ifndef SVG_EXPORT_H
#define SVG_EXPORT_H

#include "exporter.h"

class GLTSGraph;

class ExporterSVG : public Exporter
{
  public:
    ExporterSVG(Graph* g, GLTSGraph* app);
    ~ExporterSVG();
    bool export_to(wxString _filename);

  private:
    std::string svg_code;
    GLTSGraph* owner;

    void drawBezier(Transition* tr);
    void drawSelfLoop(Transition* tr);
};

#endif //svg_export_h

