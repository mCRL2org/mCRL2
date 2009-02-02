// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsimporter.h
/// \brief Declaration of LTS importer

#ifndef LTS_IMPORTER_H
#define LTS_IMPORTER_H

#include "importer.h"
#include <string>

class LTSImporter: public Importer
{
  public:
    Graph* importFile(std::string fn);
  
};

#endif //LTS_IMPORTER_H
