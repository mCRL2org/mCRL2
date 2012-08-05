// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./parser.h

#ifndef PARSER_H
#define PARSER_H

#include <QtCore>
#include <QtGui>
#include <QDomDocument>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "diagram.h"
#include "dof.h"
#include "graph.h"
#include <wx/xml/xml.h>
#include "utils.h"

template <class T>
inline std::string to_string(const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

class Mediator;

class Parser : public QObject
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    Parser(QObject* parent = 0);

    // fsm files
    void parseFile(
      QString filename,
      Graph* graph);
    void writeFSMFile(
      QString filename,
      Graph* graph);

    // dgc files
    void parseAttrConfig(
      QString filename,
      Graph* graph,
      std::map< size_t, size_t > &attrIdxFrTo,
      std::map< size_t, std::vector< std::string > > &attrCurDomains,
      std::map< size_t, std::map< size_t, size_t  > > &attrOrigToCurDomains);
    void writeAttrConfig(
      QString filename,
      Graph* graph);

    // dgd files
    void parseDiagram(
      QString filename,
      Graph* graph,
      Diagram* diagram);
    void writeDiagram(
      QString filename,
      Graph* graph,
      Diagram* diagram);

  signals:
    void started(int steps);
    void progressed(int steps);

  private:
    // -- private utility functions ---------------------------------

    QMap<QString, QDomElement> findElements(QDomElement root, QStringList tagNames);
    QMap<QString, QString> findStringValues(QDomElement root, QStringList tagNames);
    QMap<QString, double> findDoubleValues(QDomElement root, QStringList tagNames);
    QMap<QString, float> findFloatValues(QDomElement root, QStringList tagNames);
    QMap<QString, int> findIntValues(QDomElement root, QStringList tagNames);

    // dgc files
    void parseAttrConfig(
      Graph* graph,
      std::map< size_t, size_t > &attrIdxFrTo,
      std::map< size_t, std::vector< std::string > > &attrCurDomains,
      std::map< size_t, std::map< size_t, size_t  > > &attrOrigToCurDomains,
      wxXmlNode* curNode);
    void parseAttr(
      Graph* graph,
      std::map< size_t, size_t > &attrIdxFrTo,
      std::map< size_t , std::vector< std::string > > &attrCurDomains,
      std::map< size_t, std::map< size_t, size_t  > > &attrOrigToCurDomains,
      wxXmlNode* curNode);

    // dgd files
    void parseShape(
      Graph* graph,
      Diagram* diagram,
      QDomNode shapeNode);
};

#endif

// -- end -----------------------------------------------------------
