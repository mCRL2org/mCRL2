// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xmlimporter.h
/// \brief XML importer declaration.

#ifndef XML_IMPORTER_H
#define XML_IMPORTER_H

#include "importer.h"

class XMLImporter: public Importer
{
  public:
    XMLImporter() {};
    ~XMLImporter() {};
    
    Graph* importFile(std::string file);
};

#endif // XML_IMPORTER_H
