// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "parser.h"
#include "parsing.h"

Parser::Parser(QThread *atermThread)
{
  moveToThread(atermThread);
}

void Parser::parse(QString specification)
{
  try
  {
    mcrl2xi_qt::parseMcrl2Specification(specification.toStdString());
  }
  catch (const mcrl2::runtime_error& e)
  {
    QString err = QString::fromStdString(e.what());
    emit(parseError(err));
  }
  emit(finished());
}

