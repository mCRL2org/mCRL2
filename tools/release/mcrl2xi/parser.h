// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file parser.h
  @author R. Boudewijns

  A parser object that moves itself to the aTerm Thread upon construction

*/

#ifndef MCRL2XI_PARSER_H
#define MCRL2XI_PARSER_H

#include <QObject>
#include <QThread>

class Parser : public QObject
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor
     */
    explicit Parser(QThread *atermThread);

  signals:
    /**
     * @brief Signal to indicate that the algorithm finished
     */
    void finished();
    /**
     * @brief Signal to indicate that the parsing failed
     */
    void parseError(QString error);
    
  public slots:
    /**
     * @brief Slot to start the parsing the given @e specification
     * @param specification The specification to be parsed
     */
    void parse(QString specification);
};

#endif // MCRL2XI_PARSER_H
