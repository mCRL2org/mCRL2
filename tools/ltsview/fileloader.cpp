// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file
/// \brief Implementation of the FileLoader for loading FSM files

#include <fstream>
#include <sstream>
#include "fileloader.h"
#include "ltsview_fsmparser.hpp"
#include "ltsview_fsmlexer.h"

namespace ltsview {

FileLoader::FileLoader(LTS* _lts)
  : lts(_lts) 
{
}

bool FileLoader::parse_file(const std::string& _filename)
{
  filename = _filename;
  std::ifstream in(filename.c_str());
  
  ltsview::LTSViewFSMLexer scanner(&in);
  lexer = &scanner;

  ltsview::LTSViewFSMParser parser(*this);
  return (parser.parse() == 0);
}

void FileLoader::error(const class location &l, const std::string &m)
{
  std::ostringstream oss;
  oss << "Parse error at " << l << ": " << m;
  throw oss.str();
}

void FileLoader::error(const std::string &m)
{
  throw "Parse error: " + m;
}

}
