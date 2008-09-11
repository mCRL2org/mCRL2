#ifndef EXPORTER_H
#define EXPORTER_H

#include "graph.h"
#include <wx/string.h>

class Exporter
{
  public:
    Exporter(Graph* g);
    virtual ~Exporter() {};

    virtual bool export_to(wxString _filename) = 0;
  protected:
    Graph* graph;
};

#endif //EXPORTER_H
