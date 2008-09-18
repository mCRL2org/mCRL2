#ifndef XML_EXPORT_H
#define XML_EXPORT_H

#include "exporter.h"

class ExporterXML: public Exporter
{
  public:
    ExporterXML(Graph* g);
    ~ExporterXML();

    bool export_to(wxString _filename);

  private:


};

#endif // XML_EXPORT_H
