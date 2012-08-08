// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./parser.cpp

#include <QFileInfo>

#include "wx.hpp" // precompiled headers

#include "parser.h"

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_fsm.h"


using namespace std;

// -- constructors and destructor -----------------------------------


Parser::Parser(QObject* parent) :
  QObject(parent)
{ }


// -- parsing functions ---------------------------------------------

void Parser::parseFile(QString filename, Graph* graph)
{
  using namespace mcrl2::lts;

  string line = "";
  mcrl2::lts::lts_fsm_t l;

  load_lts_as_fsm_file(filename.toStdString(),l);

  emit started(l.process_parameters().size() + l.num_states() + l.get_transitions().size());
  int progress = 0;

  const std::vector < std::pair < std::string, std::string > > process_parameters=l.process_parameters();
  std::vector < std::pair < std::string, std::string > >::const_iterator parameter=process_parameters.begin();
  for (size_t i = 0; parameter != process_parameters.end(); ++i, ++parameter)
  {
    std::vector< string > values = l.state_element_values(i);
    for (size_t j = 0; j < values.size(); j++)
    {
      if (values.at(j).empty())
        values.assign(j, "-");
    }

    graph->addAttrDiscr(
          parameter->first,
          parameter->second,
          graph->getSizeAttributes(),
          values);

    emit progressed(++progress);
  }

  for (unsigned int si= 0; si<l.num_states(); ++si)
  {
    vector< double > stateVector;
    for (unsigned int i = 0; i < process_parameters.size(); ++i)
    {
      stateVector.push_back(l.state_label(si)[i]);
    }
    graph->addNode(stateVector);
    emit progressed(++progress);
  }

  const std::vector<transition> &trans=l.get_transitions();
  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    graph->addEdge(
          detail::pp(l.action_label(r->label())),
          r->from(),
          r->to());
    emit progressed(++progress);
  }
}


void Parser::writeFSMFile(
    QString filename,
    Graph* graph)
{
  QFile file(filename);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    throw mcrl2::runtime_error("Error opening file for writing.");
  }

  QString line = "";
  int lineCnt = 0;
  size_t lineTot = graph->getSizeAttributes() + graph->getSizeNodes() + graph->getSizeEdges();

  emit started(lineTot);

  // write state variable description
  for (size_t i = 0; i < graph->getSizeAttributes(); ++i)
  {
    QString name = QString::fromStdString(graph->getAttribute(i)->getName());
    QString sizeOrigValues = QString::number(int(graph->getAttribute(i)->getSizeOrigValues()));
    QString type = QString::fromStdString(graph->getAttribute(i)->getType());

    line = QString("%1(%2) %3 ").arg(name, sizeOrigValues, type);

    for (size_t j = 0; j < graph->getAttribute(i)->getSizeCurValues(); ++j)
    {
      QString value = QString::fromStdString(graph->getAttribute(i)->getCurValue(j)->getValue());
      line.append(QString("\"%1\"").arg(value));
      if (j < graph->getAttribute(i)->getSizeCurValues()-1)
      {
        line.append(" ");
      }
    }

    line.append("\n");

    file.write(line.toAscii());
    emit progressed(++lineCnt);
  }

  // write state vectors
  file.write("---\n");

  for (size_t i = 0; i < graph->getSizeNodes(); ++i)
  {
    line = "";

    for (size_t j = 0; j < graph->getNode(i)->getSizeTuple(); ++j)
    {
      line.append(QString::number(int(graph->getNode(i)->getTupleVal(j))));

      if (j < graph->getNode(i)->getSizeTuple()-1)
      {
        line.append(" ");
      }
    }

    line.append("\n");

    file.write(line.toAscii());

    emit progressed(++lineCnt);
  }

  // write transitions
  file.write("---\n");

  for (size_t i = 0; i < graph->getSizeEdges(); ++i)
  {
    QString inNode = QString::number(int(graph->getEdge(i)->getInNode()->getIndex()+1));
    QString outNode = QString::number(int(graph->getEdge(i)->getOutNode()->getIndex()+1));
    QString label = QString::fromStdString(graph->getEdge(i)->getLabel());

    line = QString("%1 %2 \"%3\"\n").arg(inNode, outNode, label);
    file.write(line.toAscii());

    emit progressed(++lineCnt);
  }

  file.close();
}


void Parser::parseAttrConfig(
    QString filename,
    Graph* graph,
    map< size_t, size_t > &attrIdxFrTo,
    map< size_t, vector< string > > &attrCurDomains,
    map< size_t, map< size_t, size_t  > > &attrOrigToCurDomains)
{

  QDomDocument xml;

  QFile file(filename);
  if(!file.open( QFile::ReadOnly ))
  {
    throw mcrl2::runtime_error(QString("Could not open XML file: %1").arg(filename).toStdString());
  }

  QString errorMsg;
  if(!xml.setContent(&file, false, &errorMsg))
  {
    file.close();
    throw mcrl2::runtime_error(QString("Could not parse XML file: %1").arg(errorMsg).toStdString());
  }
  file.close();

  QDomElement root = xml.documentElement();
  if(root.tagName() != "Configuration")
  {
    throw mcrl2::runtime_error("XML contains no valid configuration");
  }

  QDomElement fileElement = root.firstChildElement("File");
  if(fileElement.isNull() || fileElement.text().isEmpty())
  {
    throw mcrl2::runtime_error("XML contains no valid filename");
  }

  QDomNode node = root.firstChild();
  while (!node.isNull()) {
    QDomElement e = node.toElement();

    if (e.tagName() == "Attribute") {
      parseAttribute(graph,
                     attrIdxFrTo,
                     attrCurDomains,
                     attrOrigToCurDomains,
                     node);
    }

    node = node.nextSibling();
  }

}


void Parser::writeAttrConfig(
    QString filename,
    Graph* graph)
{
  try
  {
    // new xml document
    wxXmlDocument  doc;
    wxXmlNode*     conf;
    wxXmlNode*     file;
    wxXmlNode*     attr;
    wxXmlNode*     name;
    wxXmlNode*     type;
    wxXmlNode*     card;
    wxXmlNode*     domn;
    wxXmlNode*     valu;
    wxXmlNode*     map;
    wxXmlNode*     pos;

    // document declaration
    doc.SetVersion(wxT("1.0"));

    // configuration
    conf = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Configuration"));
    doc.SetRoot(conf);

    // file name
    file = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("File"));
    conf -> AddChild(file);
    new wxXmlNode(file, wxXML_TEXT_NODE, wxEmptyString, wxString(graph->getFileName().c_str(), wxConvUTF8));

    // attributes
    for (size_t i = 0; i < graph->getSizeAttributes(); ++i)
    {
      attr = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Attribute"));
      conf -> AddChild(attr);

      // name
      name = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Name"));
      attr -> AddChild(name);
      new wxXmlNode(name, wxXML_TEXT_NODE, wxEmptyString, wxString(graph->getAttribute(i)->getName().c_str(), wxConvUTF8));

      // type
      type = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Type"));
      attr -> AddChild(type);
      new wxXmlNode(type , wxXML_TEXT_NODE, wxEmptyString, wxString(graph->getAttribute(i)->getType().c_str(), wxConvUTF8));

      // cardinality
      card = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("OriginalCardinality"));
      attr -> AddChild(card);
      new wxXmlNode(card , wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::intToStr((int) graph->getAttribute(i)->getSizeOrigValues()).c_str(), wxConvUTF8));

      // current domain
      domn = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("CurrentDomain"));
      attr -> AddChild(domn);
      {
        for (size_t j = 0; j < graph->getAttribute(i)->getSizeCurValues(); ++j)
        {
          // value
          valu = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
          domn -> AddChild(valu);
          new wxXmlNode(valu , wxXML_TEXT_NODE, wxEmptyString, wxString(graph->getAttribute(i)->getCurValue(j)->getValue().c_str(), wxConvUTF8));
        }
      }

      // mapping from original to current domain
      // current domain
      map = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("OriginalToCurrent")) ;
      attr -> AddChild(map);
      {
        for (size_t j = 0; j < graph->getAttribute(i)->getSizeOrigValues(); ++j)
        {
          // value
          pos = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("CurrentPosition"));
          map -> AddChild(pos);
          new wxXmlNode(pos , wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::intToStr((int) graph->getAttribute(i)->mapToValue(j)->getIndex()).c_str(), wxConvUTF8));
        }
      }
    }

    doc.Save(wxString(filename.toStdString().c_str(), wxConvUTF8));

    conf = 0;
    file = 0;
    attr = 0;
    name = 0;
    type = 0;
    card = 0;
    domn = 0;
    valu = 0;
    map  = 0;
    pos  = 0;
  }
  catch (...)
  {
    throw mcrl2::runtime_error(
          "Error saving attribute configuration.");
  }
}


void Parser::parseDiagram(
    QString filename,
    Graph* graph,
    Diagram* diagram)
{
  QDomDocument xml;

  QFile file(filename);
  if(!file.open( QFile::ReadOnly ))
  {
    throw mcrl2::runtime_error(QString("Could not open XML file: %1").arg(filename).toStdString());
  }

  QString errorMsg;
  if(!xml.setContent(&file, false, &errorMsg))
  {
    file.close();
    throw mcrl2::runtime_error(QString("Could not parse XML file: %1").arg(errorMsg).toStdString());
  }
  file.close();

  QDomElement root = xml.documentElement();
  if(root.tagName() != "Diagram")
  {
    throw mcrl2::runtime_error("XML contains no valid diagram");
  }

  QDomElement fileElement = root.firstChildElement("File");
  if(fileElement.isNull() || fileElement.text().isEmpty())
  {
    throw mcrl2::runtime_error("XML contains no valid filename");
  }

  QDomNode node = root.firstChild();
  while (!node.isNull()) {
    QDomElement e = node.toElement();

    if (e.tagName() == "Shape") {
      parseShape(graph, diagram, node);
    }

    node = node.nextSibling();
  }

}


void Parser::writeDiagram(
    QString filename,
    Graph* graph,
    Diagram* diagram)
{
  try
  {
    // new xml document
    wxXmlDocument     doc;
    wxXmlNode*     dgrm;
    wxXmlNode*     file;
    wxXmlNode*     shpe;
    wxXmlNode*     prop;
    wxXmlNode*     subp;
    Attribute*        attr;

    // document declaration
    doc.SetVersion(wxT("1.0"));

    // configuration
    dgrm = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Diagram"));
    doc.SetRoot(dgrm);

    // file name
    file = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("File"));
    dgrm -> AddChild(file);

    new wxXmlNode(file, wxXML_TEXT_NODE, wxEmptyString, wxString(graph->getFileName().c_str(), wxConvUTF8));

    // shapes
    for (size_t i = 0; i < diagram->getSizeShapes(); ++i)
    {
      shpe = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Shape"));
      dgrm -> AddChild(shpe);

      // coordinates
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("XCenter"));
      shpe -> AddChild(prop);
      new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getXCtr()).c_str(), wxConvUTF8));

      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("YCenter"));
      shpe -> AddChild(prop);
      new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getYCtr()).c_str(), wxConvUTF8));

      // distance from center
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("XDistanceFromCenter"));
      shpe -> AddChild(prop);
      new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getXDFC()).c_str(), wxConvUTF8));

      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("YDistanceFromCenter"));
      shpe -> AddChild(prop);
      new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getYDFC()).c_str(), wxConvUTF8));

      // hinge
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("XHinge"));
      shpe -> AddChild(prop);
      new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getXHinge()).c_str(), wxConvUTF8));

      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("YHinge"));
      shpe -> AddChild(prop);
      new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getYHinge()).c_str(), wxConvUTF8));

      // angle center
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("AngleCenter"));
      shpe -> AddChild(prop);
      new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getAngleCtr()).c_str(), wxConvUTF8));

      // type
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Type"));
      shpe -> AddChild(prop);
      if (diagram->getShape(i)->getType() == Shape::TYPE_LINE)
      {
        new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString,  wxT("TYPE_LINE"));
      }
      else if (diagram->getShape(i)->getType() == Shape::TYPE_RECT)
      {
        new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString,  wxT("TYPE_RECT"));
      }
      else if (diagram->getShape(i)->getType() == Shape::TYPE_ELLIPSE)
      {
        new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString,  wxT("TYPE_ELLIPSE"));
      }
      else if (diagram->getShape(i)->getType() == Shape::TYPE_ARROW)
      {
        new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString,  wxT("TYPE_ARROW"));
      }
      else if (diagram->getShape(i)->getType() == Shape::TYPE_DARROW)
      {
        new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString,  wxT("TYPE_DARROW"));
      }

      // line width
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("LineWidth"));
      shpe -> AddChild(prop);
      new wxXmlNode(prop, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getLineWidth()).c_str(), wxConvUTF8));

      // color line
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("LineColor"));
      shpe -> AddChild(prop);

      QColor col = diagram->getShape(i)->getLineColor();
      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Red"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.redF()).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Green"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.greenF()).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Blue"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.blueF()).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Alpha"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.alphaF()).c_str(), wxConvUTF8));

      // color fill
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("FillColor"));
      shpe -> AddChild(prop);

      col = diagram->getShape(i)->getFillColor();
      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Red"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.redF()).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Green"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.greenF()).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Blue"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.blueF()).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Alpha"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.alphaF()).c_str(), wxConvUTF8));

      // X center DOF
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("XCenterDOF"));
      shpe -> AddChild(prop);

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Attribute"));
      prop -> AddChild(subp);
      attr = diagram->getShape(i)->getDOFXCtr()->getAttribute();
      if (attr != 0)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.alphaF()).c_str(), wxConvUTF8));
      }
      attr = 0;

      {
        for (size_t j = 0; j < diagram->getShape(i)->getDOFXCtr()->getSizeValues(); ++j)
        {
          subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
          prop -> AddChild(subp);
          new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getDOFXCtr()->getValue(j)).c_str(), wxConvUTF8));
        }
      }

      // Y center DOF
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("YCenterDOF"));
      shpe -> AddChild(prop);

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Attribute"));
      prop -> AddChild(subp);

      attr = diagram->getShape(i)->getDOFYCtr()->getAttribute();
      if (attr != 0)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = 0;

      {
        for (size_t j = 0; j < diagram->getShape(i)->getDOFYCtr()->getSizeValues(); ++j)
        {
          subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
          prop -> AddChild(subp);
          new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getDOFYCtr()->getValue(j)).c_str(), wxConvUTF8));
        }
      }

      // width DOF
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("WidthDOF"));
      shpe -> AddChild(prop);

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Attribute"));
      prop -> AddChild(subp);
      attr = diagram->getShape(i)->getDOFWth()->getAttribute();
      if (attr != 0)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = 0;

      {
        for (size_t j = 0; j < diagram->getShape(i)->getDOFWth()->getSizeValues(); ++j)
        {
          subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
          prop -> AddChild(subp);
          new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getDOFWth()->getValue(j)).c_str(), wxConvUTF8));
        }
      }

      // height DOF
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("HeightDOF"));
      shpe -> AddChild(prop);

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Attribute"));
      prop -> AddChild(subp);

      attr = diagram->getShape(i)->getDOFHgt()->getAttribute();
      if (attr != 0)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = 0;

      {
        for (size_t j = 0; j < diagram->getShape(i)->getDOFHgt()->getSizeValues(); ++j)
        {
          subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
          prop -> AddChild(subp);
          new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getDOFHgt()->getValue(j)).c_str(), wxConvUTF8));
        }
      }

      // angle DOF (relative to hinge)
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("AngleDOF"));
      shpe -> AddChild(prop);

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Attribute"));
      prop -> AddChild(subp);

      attr = diagram->getShape(i)->getDOFAgl()->getAttribute();
      if (attr != 0)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = 0;

      {
        for (size_t j = 0; j < diagram->getShape(i)->getDOFAgl()->getSizeValues(); ++j)
        {
          subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
          prop -> AddChild(subp);
          new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getDOFAgl()->getValue(j)).c_str(), wxConvUTF8));
        }
      }

      // color DOF
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("ColorDOF"));
      shpe -> AddChild(prop);

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Attribute"));
      prop -> AddChild(subp);
      attr = diagram->getShape(i)->getDOFCol()->getAttribute();
      if (attr != 0)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = 0;

      {
        for (size_t j = 0; j < diagram->getShape(i)->getDOFCol()->getSizeValues(); ++j)
        {
          subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
          prop -> AddChild(subp);
          new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getDOFCol()->getValue(j)).c_str(), wxConvUTF8));
        }
      }

      vector< double > yValsCol;
      diagram->getShape(i)->getDOFColYValues(yValsCol);
      {
        for (size_t j = 0; j < yValsCol.size(); ++j)
        {
          subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("AuxilaryValue"));
          prop -> AddChild(subp);
          new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(yValsCol[j]).c_str(), wxConvUTF8));
        }
      }

      // opacity DOF
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("OpacityDOF"));
      shpe -> AddChild(prop);

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Attribute"));
      prop -> AddChild(subp);

      attr = diagram->getShape(i)->getDOFOpa()->getAttribute();
      if (attr != 0)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = 0;

      {
        for (size_t j = 0; j < diagram->getShape(i)->getDOFOpa()->getSizeValues(); ++j)
        {
          subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
          prop -> AddChild(subp);
          new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(diagram->getShape(i)->getDOFOpa()->getValue(j)).c_str(), wxConvUTF8));
        }
      }

      vector< double > yValsOpa;
      diagram->getShape(i)->getDOFOpaYValues(yValsOpa);
      {
        for (size_t j = 0; j < yValsOpa.size(); ++j)
        {
          subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("AuxilaryValue"));
          prop -> AddChild(subp);
          new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(yValsOpa[j]).c_str(), wxConvUTF8));
        }
      }
    }

    doc.Save(wxString(filename.toStdString().c_str(), wxConvUTF8));

    dgrm = 0;
    file = 0;
    shpe = 0;
    prop = 0;
    subp = 0;
    attr = 0;
  }
  catch (...)
  {
    throw mcrl2::runtime_error(
          "Error saving diagram.");
  }
}


// -- private utility functions -------------------------------------

QMap<QString, QDomElement> Parser::findElements(QDomElement root, QStringList tagNames)
{
  QMap<QString, QDomElement> result;
  for (int i = 0; i < tagNames.size(); i++)
  {
    QDomElement tagElement = root.firstChildElement(tagNames[i]);
    if (tagElement.isNull())
    {
      throw mcrl2::runtime_error(QString("%1 value not found.").arg(tagNames[i]).toStdString());
    }
    result.insert(tagNames[i], tagElement);
  }
  return result;
}

QMap<QString, QString> Parser::findStringValues(QDomElement root, QStringList tagNames)
{
  QMap<QString, QDomElement> elements = findElements(root, tagNames);
  QMap<QString, QString> result;
  for (int i = 0; i < tagNames.size(); i++)
  {
    QDomElement tagElement = elements[tagNames[i]];
    if (tagElement.text().isEmpty())
    {
      throw mcrl2::runtime_error(QString("%1 value is empty.").arg(tagNames[i]).toStdString());
    }
    result.insert(tagNames[i], tagElement.text());
  }
  return result;
}

QMap<QString, double> Parser::findDoubleValues(QDomElement root, QStringList tagNames)
{
  QMap<QString, QString> values = findStringValues(root, tagNames);
  QMap<QString, double> result;

  for (int i = 0; i < tagNames.size(); i++)
  {
    bool ok;
    double value = values[tagNames[i]].toDouble(&ok);
    if (!ok)
    {
      throw mcrl2::runtime_error(QString("%1 contains no valid double (%2).").arg(tagNames[i], values[tagNames[i]]).toStdString());
    }
    result.insert(tagNames[i], value);
  }
  return result;
}

QMap<QString, float> Parser::findFloatValues(QDomElement root, QStringList tagNames)
{
  QMap<QString, QString> values = findStringValues(root, tagNames);
  QMap<QString, float> result;

  for (int i = 0; i < tagNames.size(); i++)
  {
    bool ok;
    float value = values[tagNames[i]].toFloat(&ok);
    if (!ok)
    {
      throw mcrl2::runtime_error(QString("%1 contains no valid float (%2).").arg(tagNames[i], values[tagNames[i]]).toStdString());
    }
    result.insert(tagNames[i], value);
  }
  return result;
}

QMap<QString, int> Parser::findIntValues(QDomElement root, QStringList tagNames)
{
  QMap<QString, QString> values = findStringValues(root, tagNames);
  QMap<QString, int> result;

  for (int i = 0; i < tagNames.size(); i++)
  {
    bool ok;
    int value = values[tagNames[i]].toInt(&ok);
    if (!ok)
    {
      throw mcrl2::runtime_error(QString("%1 contains no valid integer (%2).").arg(tagNames[i], values[tagNames[i]]).toStdString());
    }
    result.insert(tagNames[i], value);
  }
  return result;
}


void Parser::parseAttribute(
    Graph* graph,
    map< size_t , size_t > &attrIdxFrTo,
    map< size_t, vector< string > > &attrCurDomains,
    map< size_t, map< size_t, size_t  > > &attrOrigToCurDomains,
    QDomNode AttributeNode)
{
  try
  {
    // Attribute properties
    QStringList stringProperties = QStringList() << "Name" <<
                                                    "Type";
    QMap<QString, QString> stringPropertyValues = findStringValues(AttributeNode.toElement(), stringProperties);

    int originalCardinality = findIntValues(AttributeNode.toElement(), QStringList("OriginalCardinality")).value("OriginalCardinality");

    QStringList listProperties = QStringList() << "CurrentDomain" <<
                                                  "OriginalToCurrent";

    QMap<QString, QDomElement> listPropertyElements = findElements(AttributeNode.toElement(), listProperties);

    Attribute* attribute = graph->getAttribute(stringPropertyValues["Name"].toStdString());
    if (attribute == 0)
    {
      throw mcrl2::runtime_error(QString("Invalid attribute name (%1).").arg(stringPropertyValues["Name"]).toStdString());
    }

    if (QString::fromStdString(attribute->getType()) != stringPropertyValues["Type"])
    {
      throw mcrl2::runtime_error(QString("Types do not match (%1 vs %2).").arg(QString::fromStdString(attribute->getType()), stringPropertyValues["Type"]).toStdString());
    }

    if (attribute->getSizeOrigValues() != originalCardinality)
    {
      throw mcrl2::runtime_error(QString("Cardinalities do not match (%1 vs %2).").arg(attribute->getSizeOrigValues()).arg(originalCardinality).toStdString());
    }

    // update attribute mapping
    attrIdxFrTo.insert(pair< size_t, size_t >(attribute->getIndex(), attrIdxFrTo.size()));

    vector< string > domain;
    QDomElement valueElement = listPropertyElements["CurrentDomain"].firstChildElement("Value");
    for (int i = 0; !valueElement.isNull(); i++, valueElement = valueElement.nextSiblingElement("Value"))
    {
      QString value = valueElement.text();
      if (value.isEmpty())
      {
        throw mcrl2::runtime_error("Domain value is empty.");
      }
      domain.push_back(value.toStdString());
    }

    // update domain
    attrCurDomains.insert(pair< size_t, vector< string > >(attribute->getIndex(), domain));

    map< size_t, size_t > origToCur;
    QDomElement positionElement = listPropertyElements["OriginalToCurrent"].firstChildElement("CurrentPosition");
    int i;
    for (i = 0; !positionElement.isNull(); i++, positionElement = positionElement.nextSiblingElement("CurrentPosition"))
    {
      bool ok;
      int value = positionElement.text().toInt(&ok);
      if (!ok)
      {
        throw mcrl2::runtime_error(QString("Current Position contains no valid integer (%1).").arg(positionElement.text()).toStdString());
      }
      if (value >= int(domain.size()))
      {
        throw mcrl2::runtime_error(QString("Current Position is larger than current domain (%1).").arg(positionElement.text()).toStdString());
      }
      if (i >= originalCardinality)
      {
        throw mcrl2::runtime_error("Too many Current Positions found.");
      }
      origToCur.insert(pair< int, int >(i, value));
    }
    if (i < originalCardinality)
    {
      throw mcrl2::runtime_error("Too few Current Positions found.");
    }

    // update mapping
    attrOrigToCurDomains.insert(pair< size_t , map< size_t, size_t > >(attribute->getIndex(), origToCur));

  }
  catch (const mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(QString("Invalid attribute.\n%1").arg(e.what()).toStdString());
  }
}


void Parser::parseShape(
    Graph* graph,
    Diagram* diagram,
    QDomNode shapeNode)
{
  try
  {
    // Shape properties
    QStringList doubleProperties = QStringList() << "XCenter" <<
                                                    "YCenter" <<
                                                    "XDistanceFromCenter" <<
                                                    "YDistanceFromCenter" <<
                                                    "XHinge" <<
                                                    "YHinge" <<
                                                    "AngleCenter" <<
                                                    "LineWidth";
    QMap<QString, double> doublePropertyValues = findDoubleValues(shapeNode.toElement(), doubleProperties);

    QStringList colorProperties = QStringList() << "LineColor" <<
                                                   "FillColor";
    QMap<QString, QDomElement> colorPropertyElements = findElements(shapeNode.toElement(), colorProperties);
    QMap<QString, QColor> colorPropertyValues;

    QStringList colorParts = QStringList() << "Red" <<
                                              "Green" <<
                                              "Blue" <<
                                              "Alpha";

    // Construct QColor objects for each Color property
    for (int i = 0; i < colorProperties.size(); i++)
    {
      try
      {
        QMap<QString, float> colorPartValues = findFloatValues(colorPropertyElements[colorProperties[i]], colorParts);
        colorPropertyValues.insert(colorProperties[i], QColor::fromRgbF(colorPartValues["Red"],
                                                                        colorPartValues["Green"],
                                                                        colorPartValues["Blue"],
                                                                        colorPartValues["Alpha"]));
      }
      catch (const mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(QString("Invalid %1 value.\n%2").arg(colorProperties[i], e.what()).toStdString());
      }
    }

    QMap<QString, int> types;
    types.insert("TYPE_LINE",     Shape::TYPE_LINE);
    types.insert("TYPE_RECT",     Shape::TYPE_RECT);
    types.insert("TYPE_ELLIPSE",  Shape::TYPE_ELLIPSE);
    types.insert("TYPE_ARROW",    Shape::TYPE_ARROW);
    types.insert("TYPE_DARROW",   Shape::TYPE_DARROW);
    types.insert("TYPE_NOTE",     Shape::TYPE_NOTE);

    QString typeValue = findStringValues(shapeNode.toElement(), QStringList("Type")).value("Type");
    if (!types.contains(typeValue))
    {
      throw mcrl2::runtime_error(QString("Invalid type (%1).").arg(typeValue).toStdString());
    }
    int type = types[typeValue];

    // Create shape
    Shape* shape = new Shape(
          diagram,
          diagram->getSizeShapes(),
          doublePropertyValues["XCenter"],   doublePropertyValues["YCenter"],
          doublePropertyValues["XDistanceFromCenter"],   doublePropertyValues["YDistanceFromCenter"],
          doublePropertyValues["AngleCenter"], type);
    shape->setHinge(doublePropertyValues["XHinge"], doublePropertyValues["YHinge"]);
    shape->setLineWidth(doublePropertyValues["LineWidth"]);
    shape->setLineColor(colorPropertyValues["LineColor"]);
    shape->setFillColor(colorPropertyValues["FillColor"]);

    QMap<QString, DOF*> dofs;
    dofs.insert("XCenterDOF", shape->getDOFXCtr());
    dofs.insert("YCenterDOF", shape->getDOFYCtr());
    dofs.insert("WidthDOF",   shape->getDOFWth());
    dofs.insert("HeightDOF",  shape->getDOFHgt());
    dofs.insert("AngleDOF",   shape->getDOFAgl());
    dofs.insert("ColorDOF",   shape->getDOFCol());
    dofs.insert("OpacityDOF", shape->getDOFOpa());

    QStringList dofTags = dofs.keys();
    QMap<QString, QDomElement> dofElements = findElements(shapeNode.toElement(), dofTags);

    for (int i = 0; i < dofTags.size(); i++)
    {
      try
      {
        QDomElement dofElement = dofElements[dofTags[i]];
        DOF* dof = dofs[dofTags[i]];
        QMap<QString, QDomElement> attributeElements = findElements(dofElement, QStringList("Attribute"));
        QDomElement attributeElement = attributeElements["Attribute"];
        if (!attributeElement.text().isEmpty())
        {
          Attribute* attribute = graph->getAttribute(attributeElement.text().toStdString());
          if (attribute != 0)
          {
            dof->setAttribute(attribute);
          }
        }
        QDomElement valueElement = dofElement.firstChildElement("Value");
        for (int j = 0; !valueElement.isNull(); j++, valueElement = valueElement.nextSiblingElement("Value"))
        {
          bool ok;
          double value = valueElement.text().toDouble(&ok);
          if (!ok)
          {
            throw mcrl2::runtime_error(QString("Value contains no valid double (%1).").arg(valueElement.text()).toStdString());
          }
          switch(j)
          {
            case 0:
              dof->setMin(value);
              break;
            case 1:
              dof->setMax(value);
              break;
            default:
              dof->addValue(value);
              break;
          }
        }
        if (dofTags[i] == "ColorDOF")
        {
          QDomElement valueElement = dofElement.firstChildElement("AuxilaryValue");
          for (int j = 0; !valueElement.isNull(); j++, valueElement = valueElement.nextSiblingElement("AuxilaryValue"))
          {
            bool ok;
            double value = valueElement.text().toDouble(&ok);
            if (!ok)
            {
              throw mcrl2::runtime_error(QString("Auxilary Value contains no valid double (%1).").arg(valueElement.text()).toStdString());
            }
            switch(j)
            {
              case 0:
              case 1:
                shape->setDOFColYValue(j, value);
                break;
              default:
                shape->addDOFColYValue(value);
                break;
            }
          }
        }
        if (dofTags[i] == "OpacityDOF")
        {
          QDomElement valueElement = dofElement.firstChildElement("AuxilaryValue");
          for (int j = 0; !valueElement.isNull(); j++, valueElement = valueElement.nextSiblingElement("AuxilaryValue"))
          {
            bool ok;
            double value = valueElement.text().toDouble(&ok);
            if (!ok)
            {
              throw mcrl2::runtime_error(QString("Auxilary Value contains no valid double (%1).").arg(valueElement.text()).toStdString());
            }
            switch(j)
            {
              case 0:
              case 1:
                shape->setDOFOpaYValue(j, value);
                break;
              default:
                shape->addDOFOpaYValue(value);
                break;
            }
          }
        }
      }
      catch (const mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(QString("Invalid %1 value.\n%2").arg(dofTags[i], e.what()).toStdString());
      }
    }

    diagram->addShape(shape);
  }
  catch (const mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(QString("Invalid shape.\n%1").arg(e.what()).toStdString());
  }
}

// -- end -----------------------------------------------------------
