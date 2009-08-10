// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file export_xml.h
/// \brief XML exporter.

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
