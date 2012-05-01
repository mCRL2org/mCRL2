// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parser.h
/// \brief A parser object that moves itself to the aTerm Thread upon construction

#ifndef MCRL2XI_PARSER_H
#define MCRL2XI_PARSER_H

#include <QObject>

class Parser : public QObject
{
    Q_OBJECT
  public:
    // Constuctor
    explicit Parser();

  signals:
    // Signal to indicate that the parsing is done
    void parsed();
    
  public slots:
    // Slot to start the parsing of the specification
    void parse(QString specification);
    
  private:
};

#endif // MCRL2XI_PARSER_H
