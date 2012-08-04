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
{
  delims = "() \"";
}


// -- parsing functions ---------------------------------------------

void Parser::parseFile(QString filename, Graph* graph)
// Parse the file identified by 'fileName' by calling:
//  - Parser::parseStateVarDescr()
//  - Parser::parseStates()
//  - Parser::parseTransitions()
// Also, report to 'mediator' on the current progress: number of bytes
// already read.
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
  wxXmlDocument doc;
  if (doc.Load(wxString(filename.toStdString().c_str(), wxConvUTF8)) == true)
  {
    wxXmlNode* curNode    = NULL;

    try
    {

      curNode = doc.GetRoot();

      if (curNode != NULL)
      {
        attrIdxFrTo.clear();
        parseAttrConfig(
          graph,
          attrIdxFrTo,
          attrCurDomains,
          attrOrigToCurDomains,
          curNode);
      }

      curNode    = NULL;
    }
    catch (const mcrl2::runtime_error& e)
    {
      curNode = NULL;

      throw mcrl2::runtime_error(string("Error loading attribute configuration.\n") + string(e.what()));
    }
  }
  else
  {
    throw mcrl2::runtime_error("Error opening attribute configuration file.");
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

    conf = NULL;
    file = NULL;
    attr = NULL;
    name = NULL;
    type = NULL;
    card = NULL;
    domn = NULL;
    valu = NULL;
    map  = NULL;
    pos  = NULL;
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
  Diagram* dgrmOld,
  Diagram* dgrmNew)
{
  wxXmlDocument doc;

  if (doc.Load(wxString(filename.toStdString().c_str(), wxConvUTF8)))
  {
    wxXmlNode* curNode    = NULL;

    try
    {

      curNode = doc.GetRoot();

      if (curNode != NULL)
      {
        parseDiagram(
          graph,
          dgrmOld,
          dgrmNew,
          curNode);
      }

      curNode    = NULL;
    }
    catch (const mcrl2::runtime_error& e)
    {
      curNode = NULL;

      throw mcrl2::runtime_error(string("Error loading diagram.\n") + string(e.what()));
    }
  }
  else
  {
    throw mcrl2::runtime_error("Error opening diagram file.");
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
      if (attr != NULL)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.alphaF()).c_str(), wxConvUTF8));
      }
      attr = NULL;

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
      if (attr != NULL)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = NULL;

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
      if (attr != NULL)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = NULL;

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
      if (attr != NULL)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = NULL;

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
      if (attr != NULL)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = NULL;

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
      if (attr != NULL)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = NULL;

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
      if (attr != NULL)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(attr->getName().c_str(), wxConvUTF8));
      }
      attr = NULL;

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

    dgrm = NULL;
    file = NULL;
    shpe = NULL;
    prop = NULL;
    subp = NULL;
    attr = NULL;
  }
  catch (...)
  {
    throw mcrl2::runtime_error(
      "Error saving diagram.");
  }
}


// -- private utility functions -------------------------------------

void Parser::parseAttrConfig(
  Graph* graph,
  map< size_t , size_t > &attrIdxFrTo,
  map< size_t, vector< string > > &attrCurDomains,
  map< size_t, map< size_t, size_t  > > &attrOrigToCurDomains,
  wxXmlNode* curNode)
{
  if (curNode != NULL && curNode->GetName() != wxEmptyString)
  {
    // file
    if (curNode->GetName() == wxT("File"))
    {
      /*
      // the code below checks for matching file names
      if ( curNode->GetChildren()->GetName() != NULL )
      {
          if ( strcmp( curNode->GetChildren()->GetName(),
               graph->getFileName().c_str() ) != 0 )
          {
              throw mcrl2::runtime_error( "File names do not match." );
          }
      }
      else
      {
          throw mcrl2::runtime_error( "No file name specified." );
      }
      */

      // the code below does not check for matching file names
      if (curNode->GetNodeContent() == wxEmptyString)
      {
        throw mcrl2::runtime_error("No file name specified.");
      }
    }
    // shape
    else if (curNode->GetName() == wxT("Attribute"))
    {
      try
      {
        parseAttr(
          graph,
          attrIdxFrTo,
          attrCurDomains,
          attrOrigToCurDomains,
          curNode);
      }
      catch (const mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(string("Error parsing attribute.\n") + string(e.what()));
      }
    }
    // other
    else
    {
      wxXmlNode* nxtNode;
      for (nxtNode = curNode->GetChildren();
           nxtNode != NULL;
           nxtNode = nxtNode->GetNext())
      {
        parseAttrConfig(
          graph,
          attrIdxFrTo,
          attrCurDomains,
          attrOrigToCurDomains,
          nxtNode);
      }
      nxtNode = NULL;
    }
  }
}


void Parser::parseAttr(
  Graph* graph,
  map< size_t , size_t > &attrIdxFrTo,
  map< size_t, vector< string > > &attrCurDomains,
  map< size_t, map< size_t, size_t  > > &attrOrigToCurDomains,
  wxXmlNode* curNode)
{
  wxXmlNode* prop = NULL;
  wxXmlNode* subp = NULL;

  Attribute* attr;

  // name
  prop = curNode->GetChildren();
  if (prop != NULL &&
      prop->GetName() == wxT("Name") &&
      prop->GetNodeContent() != wxEmptyString)
  {
    attr = graph->getAttribute(std::string(prop->GetNodeContent().mb_str()));
    if (attr == NULL)
    {
      prop = NULL;
      throw mcrl2::runtime_error("Missing attribute.");
    }
  }
  else
  {
    prop = NULL;
    subp = NULL;
    throw mcrl2::runtime_error("Missing attribute.");
  }

  // type
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("Type") &&
      prop->GetNodeContent() != wxEmptyString)
  {
    if (strcmp(prop->GetNodeContent().mb_str(), attr->getType().c_str()) != 0)
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("Types do not match.");
    }
  }
  else
  {
    prop = NULL;
    subp = NULL;
    throw mcrl2::runtime_error("Missing type.");
  }

  // card
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("OriginalCardinality") &&
      prop->GetNodeContent() != wxEmptyString)
  {
    if (strcmp(prop->GetNodeContent().mb_str(), Utils::size_tToStr(attr->getSizeOrigValues()).c_str()) != 0)
    {
      prop = NULL;
      throw mcrl2::runtime_error("Cardinalities do not match.");
    }
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing cardinality.");
  }

  // update attribute mapping
  attrIdxFrTo.insert(pair< size_t, size_t >(attr->getIndex(), attrIdxFrTo.size()));

  // current domain
  vector< string > domain;
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("CurrentDomain"))
  {
    for (subp = prop->GetChildren(); subp != NULL; subp = subp->GetNext())
    {
      if (subp->GetName() == wxT("Value"))
      {
        wxString s = subp->GetNodeContent();

        if (s != wxEmptyString)
        {
          domain.push_back(std::string(s.mb_str()));
        }
        else
        {
          prop = NULL;
          throw mcrl2::runtime_error("Missing domain value.");
        }
      }
      else
      {
        prop = NULL;
        throw mcrl2::runtime_error("Domain incorrectly specified.");
      }
    }
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing domain.");
  }

  // update domain
  attrCurDomains.insert(pair< size_t, vector< string > >(attr->getIndex(), domain));

  // mapping from orig to current domain values
  map< size_t, size_t > origToCur;
  int valCnt = 0;
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("OriginalToCurrent"))
  {
    for (subp = prop->GetChildren(); subp != NULL; subp = subp->GetNext())
    {
      if (subp->GetName() == wxT("CurrentPosition"))
      {
        wxString s = subp->GetNodeContent();

        if (s != wxEmptyString)
        {
          origToCur.insert(
            pair< int, int >(
              valCnt, Utils::strToInt(std::string(s.mb_str()))));
          ++valCnt;
        }
        else
        {
          prop = NULL;
          subp = NULL;
          throw mcrl2::runtime_error("Missing mapping from original to current domain.");
        }
      }
      else
      {
        prop = NULL;
        subp = NULL;
        throw mcrl2::runtime_error("Mapping from original to current domain incorrectly specified.");
      }
    }
  }
  else
  {
    prop = NULL;
    subp = NULL;
    throw mcrl2::runtime_error("Missing mapping from original to current domain.");
  }

  // update mapping
  attrOrigToCurDomains.insert(pair< size_t , map< size_t, size_t > >(attr->getIndex(), origToCur));

  attr = NULL;
}


void Parser::parseDiagram(
  Graph* graph,
  Diagram* dgrmOld,
  Diagram* dgrmNew,
  wxXmlNode* curNode)
{
  if (curNode != NULL && curNode->GetName() != wxEmptyString)
  {
    // file
    if (curNode->GetName() == wxT("File"))
    {
      /*
      // the code below checks for matching file names
      if ( curNode->GetChildren()->GetName() != NULL )
      {
          if ( strcmp( curNode->GetChildren()->GetName(),
               graph->getFileName().c_str() ) != 0 )
          {
              throw mcrl2::runtime_error( "File names do not match." );
          }
      }
      else
      {
          throw mcrl2::runtime_error( "No file name specified." );
      }
      */

      // the code below does not check for matchin file names
      if (curNode->GetChildren()->GetName() == wxEmptyString)
      {
        throw mcrl2::runtime_error("No file name specified.");
      }
    }
    // shape
    else if (curNode->GetName() == wxT("Shape"))
    {
      try
      {
        parseShape(graph, dgrmOld, dgrmNew, curNode);
      }
      catch (const mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(string("Error parsing shape.\n") + string(e.what()));
      }
    }
    // other
    else
    {
      wxXmlNode* nxtNode = curNode->GetChildren();
      while (nxtNode)
      {
        parseDiagram(graph, dgrmOld, dgrmNew, nxtNode);
        nxtNode = nxtNode->GetNext();
      }
      nxtNode = NULL;
    }
  }
}


void Parser::parseShape(
  Graph* graph,
  Diagram* /*dgrmOld*/,
  Diagram* dgrmNew,
  wxXmlNode* curNode)
{
  wxXmlNode* prop = NULL;
  wxXmlNode* subp = NULL;

  double xCtr, yCtr;
  double xDFC, yDFC;
  double aglCtr;

  int      type;
  double   lineWth;
  QColor lineCol;
  QColor fillCol;

  // x center
  prop = curNode->GetChildren();
  if (prop != NULL &&
      prop->GetName() == wxT("XCenter") &&
      prop->GetNodeContent() != wxEmptyString
     )
  {
    xCtr = Utils::strToDbl(std::string(prop->GetNodeContent().mb_str()));
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing x-coordinate.");
  }

  // y center
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("YCenter") &&
      prop->GetNodeContent() != wxEmptyString
     )
  {
    yCtr = Utils::strToDbl(std::string(prop->GetNodeContent().mb_str()));
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing y-coordinate.");
  }

  // x distance from center
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("XDistanceFromCenter") &&
      prop->GetNodeContent() != wxEmptyString
     )
  {
    xDFC = Utils::strToDbl(std::string(prop->GetNodeContent().mb_str()));
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing x distance from center.");
  }

  // y distance from center
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("YDistanceFromCenter") &&
      prop->GetNodeContent() != wxEmptyString
     )
  {
    yDFC = Utils::strToDbl(std::string(prop->GetNodeContent().mb_str()));
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing y distance from center.");
  }

  // x hinge
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("XHinge") &&
      prop->GetNodeContent() != wxEmptyString)
  {
// Strange, xHge is not used (Jeroen Keiren 4 June 2009)
    /*
          xHge = Utils::strToDbl( std::string(prop->GetNodeContent().mb_str() ));
    */
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing x hinge.");
  }


  // y hinge
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("YHinge") &&
      prop->GetNodeContent() != wxEmptyString)
  {
// Strange, yHge is not used (Jeroen Keiren 4 June 2009)
    /*
            yHge = Utils::strToDbl( std::string(prop->GetNodeContent().mb_str() ));
    */
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing x hinge.");
  }
  // angle center
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("AngleCenter") &&
      prop->GetNodeContent() != wxEmptyString)

  {
    aglCtr = Utils::strToDbl(std::string(prop->GetNodeContent().mb_str()));
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing angle.");
  }

  // type
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("Type") &&
      prop->GetNodeContent() != wxEmptyString)

  {
    if (prop->GetNodeContent() == wxT("TYPE_LINE"))
    {
      type = Shape::TYPE_LINE;
    }
    else if (prop->GetNodeContent() == wxT("TYPE_RECT"))
    {
      type = Shape::TYPE_RECT;
    }
    else if (prop->GetNodeContent() == wxT("TYPE_ELLIPSE"))
    {
      type = Shape::TYPE_ELLIPSE;
    }
    else if (prop->GetNodeContent() == wxT("TYPE_ARROW"))
    {
      type = Shape::TYPE_ARROW;
    }
    else if (prop->GetNodeContent() == wxT("TYPE_DARROW"))
    {
      type = Shape::TYPE_DARROW;
    }
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing type.");
  }

  // line width
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("LineWidth") &&
      prop->GetNodeContent() != wxEmptyString)

  {
    lineWth = Utils::strToDbl(std::string(prop->GetNodeContent().mb_str()));
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing line width.");
  }

  // line color
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("LineColor"))
  {
    double red, green, blue, alpha;
    // red
    subp = prop->GetChildren();
    if (subp != NULL &&
        subp->GetName() == wxT("Red") &&
        subp->GetNodeContent() != wxEmptyString)
    {
      red = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
    }
    else
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("LineColor: Missing red channel.");
    }

    // green
    subp = subp->GetNext();
    if (subp != NULL &&
        subp->GetName() == wxT("Green") &&
        subp->GetNodeContent() != wxEmptyString)
    {
      green = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
    }
    else
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("LineColor: Missing green channel.");
    }

    // blue
    subp = subp->GetNext();
    if (subp != NULL &&
        subp->GetName() == wxT("Blue") &&
        subp->GetNodeContent() != wxEmptyString)
    {
      blue = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
    }
    else
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("LineColor: Missing blue channel.");
    }

    // alpha
    subp = subp->GetNext();
    if (subp != NULL &&
        subp->GetName() == wxT("Alpha") &&
        subp->GetNodeContent() != wxEmptyString)
    {
      alpha = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
    }
    else
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("LineColor: Missing alpha channel.");
    }

    lineCol = QColor::fromRgbF(red, green, blue, alpha);
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing line color.");
  }

  // fill color
  prop = prop->GetNext();
  if (prop != NULL &&
      prop->GetName() == wxT("FillColor"))
  {
    double red, green, blue, alpha;
    // red
    subp = prop->GetChildren();
    if (subp != NULL &&
        subp->GetName() == wxT("Red") &&
        subp->GetNodeContent() != wxEmptyString)
    {
      red = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
    }
    else
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("FillColor: Missing red channel.");
    }

    // green
    subp = subp->GetNext();
    if (subp != NULL &&
        subp->GetName() == wxT("Green") &&
        subp->GetNodeContent() != wxEmptyString)
    {
      green = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
    }
    else
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("FillColor: Missing green channel.");
    }

    // blue
    subp = subp->GetNext();
    if (subp != NULL &&
        subp->GetName() == wxT("Blue") &&
        subp->GetNodeContent() != wxEmptyString)
    {
      blue = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
    }
    else
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("FillColor: Missing blue channel.");
    }

    // alpha
    subp = subp->GetNext();
    if (subp != NULL &&
        subp->GetName() == wxT("Alpha") &&
        subp->GetNodeContent() != wxEmptyString)
    {
      alpha = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
    }
    else
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("FillColor:Missing alpha channel.");
    }

    fillCol = QColor::fromRgbF(red, green, blue, alpha);
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing fill color.");
  }

  // init shape
  Shape* s = new Shape(
    dgrmNew,
    dgrmNew->getSizeShapes(),
    xCtr,   yCtr,
    xDFC,   yDFC,
    aglCtr, type);
  s->setLineWidth(lineWth);
  s->setLineColor(lineCol);
  s->setFillColor(fillCol);

  // DOF's
  Attribute* attr;
  DOF*       dof;

  int  cntVal;
  int  cntAuxCol;
  int  cntAuxOpa;

  prop = prop->GetNext();
  while (prop)
  {
    attr = NULL;
    dof  = NULL;

    // dof
    if (prop->GetName() == wxT("XCenterDOF"))
    {
      dof = s->getDOFXCtr();
    }
    else if (prop->GetName() == wxT("YCenterDOF"))
    {
      dof = s->getDOFYCtr();
    }
    else if (prop->GetName() == wxT("WidthDOF"))
    {
      dof = s->getDOFWth();
    }
    else if (prop->GetName() == wxT("HeightDOF"))
    {
      dof = s->getDOFHgt();
    }
    else if (prop->GetName() == wxT("AngleDOF"))
    {
      dof = s->getDOFAgl();
    }
    else if (prop->GetName() == wxT("ColorDOF"))
    {
      dof = s->getDOFCol();
    }
    else if (prop->GetName() == wxT("OpacityDOF"))
    {
      dof = s->getDOFOpa();
    }

    if (dof != NULL)
    {

      subp = prop->GetChildren();

      cntVal    = 0;
      cntAuxCol = 0;
      cntAuxOpa = 0;

      while (subp)
      {

        // Atribute
        if (subp->GetName() == wxT("Attribute") &&
            subp->GetNodeContent() != wxEmptyString)
        {
          attr = graph->getAttribute(std::string(subp->GetNodeContent().mb_str()));
          if (attr != NULL)
          {
            dof->setAttribute(attr);
          }
        }

        if (subp->GetName() == wxT("Value") &&
            subp->GetNodeContent() != wxEmptyString)
        {
          ++cntVal;
          // reset min
          if (cntVal == 1)
          {
            dof->setMin(Utils::strToDbl(std::string(subp->GetNodeContent().mb_str())));
          }
          // reset max
          else if (cntVal == 2)
          {
            dof->setMax(Utils::strToDbl(std::string(subp->GetNodeContent().mb_str())));
          }
          // add additional values
          else
          {
            dof->addValue(Utils::strToDbl(std::string(subp->GetNodeContent().mb_str())));
          }
        }

        if (subp->GetName() == wxT("AuxilaryValue") &&
            prop->GetName() == wxT("ColorDOF") &&
            subp->GetNodeContent() != wxEmptyString)
        {
          ++cntAuxCol;
          // reset min
          if (cntAuxCol == 1)
          {
            s->setDOFColYValue(0, Utils::strToDbl(std::string(subp->GetNodeContent().mb_str())));
          }
          // reset max
          else if (cntAuxCol == 2)
          {
            s->setDOFColYValue(1, Utils::strToDbl(std::string(subp->GetNodeContent().mb_str())));
          }
          // add additional values
          else
          {
            s->addDOFColYValue(Utils::strToDbl(std::string(subp->GetNodeContent().mb_str())));
          }
        }

        if (subp->GetName() == wxT("AuxilaryValue") &&
            prop->GetName() == wxT("OpacityDOF") &&
            subp->GetNodeContent() != wxEmptyString)
        {
          ++cntAuxOpa;
          // reset min
          if (cntAuxOpa == 1)
          {
            s->setDOFOpaYValue(0, Utils::strToDbl(std::string(subp->GetNodeContent().mb_str())));
          }
          // reset max
          else if (cntAuxOpa == 2)
          {
            s->setDOFOpaYValue(1, Utils::strToDbl(std::string(subp->GetNodeContent().mb_str())));
          }
          // add additional values
          else
          {
            s->addDOFOpaYValue(Utils::strToDbl(std::string(subp->GetNodeContent().mb_str())));
          }
        }

        subp = subp->GetNext();
      }
    } // dof

    prop = prop->GetNext();
  }

  // add shape
  dgrmNew->addShape(s);
  s =NULL;

  attr = NULL;
  dof  = NULL;

  prop = NULL;
  subp = NULL;
}


// -- end -----------------------------------------------------------
