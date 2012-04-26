// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "parser.h"
#include "parsing.h"

#include "mcrl2/utilities/atermthread.h"
#include "mcrl2/process/parse.h"

Parser::Parser()
{
  moveToThread(mcrl2::utilities::qt::get_aterm_thread());
}

void Parser::parse(QString specification)
{
  mcrl2xi_qt::parse_mcrl2_specification(specification.toStdString());
  emit parsed();
}

