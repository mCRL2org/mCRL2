// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./parser.cpp

#include "wx.hpp" // precompiled headers

#include "parser.h"

#include "mcrl2/utilities/logger.h"
#include "mcrl2/exception.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_fsm.h"


using namespace std;

// -- constructors and destructor -----------------------------------


// --------------------------
Parser::Parser(Mediator* m)
  : Colleague(m)
// --------------------------
{
  delims = "() \"";
}


// --------------
Parser::~Parser()
// --------------
{}


// -- parsing functions ---------------------------------------------


// ------------------------------------------
int Parser::getFileSize(const string& path)
// ------------------------------------------
// ------------------------------------------------------------------
// This function returns the size of the file identified by 'path' in
// bytes. Thanks to http://www.cplusplus.com/doc/tutorial/files.html
// ------------------------------------------------------------------
{
  int result = 0;
  ifstream file;

  int begin  = 0;
  int end    = 0;

  file.open(path.c_str());
  if (!file.is_open())
  {
    throw mcrl2::runtime_error("Error opening file for computing file size.");
  }

  try
  {
    // get starting position
    begin = file.tellg();

    // seek and set current position to 'EOF'
    file.seekg(0, ios::end);
    end = file.tellg();

    // calc size
    result = end - begin;
  }
  catch (...)
  {
    throw mcrl2::runtime_error("Error computing file size.");
  }
  file.close();

  return result;
}


// -----------------------
void Parser::parseFile(const string& path, Graph* graph)
// -----------------------
// ------------------------------------------------------------------
// Parse the file identified by 'fileName' by calling:
//  - Parser::parseStateVarDescr()
//  - Parser::parseStates()
//  - Parser::parseTransitions()
// Also, report to 'mediator' on the current progress: number of bytes
// already read.
// ------------------------------------------------------------------
{
  using namespace mcrl2::lts;
//using namespace mcrl2::core;

  //ifstream file;
  string line = "";
  //int sect = 0;
  //int lineCnt = 0;
  //int byteCnt = 0;

  ////////////////////////////////////////////////////

  mcrl2::lts::lts_fsm_t l;
  load_lts_as_fsm_file(path,l);

  const std::vector < std::pair < std::string, std::string > > process_parameters=l.process_parameters();
  std::vector < std::pair < std::string, std::string > >::const_iterator parameter=process_parameters.begin();
  for (unsigned int i = 0; i < process_parameters.size(); ++i, ++parameter)
  {
    line.clear();
    // line.append(l.state_parameter_name_str(i));
    line.append(parameter->first); // variable name
    line.append("(");
    std::vector< string > tmp = l.state_element_values(i);
    line.append(to_string(tmp.size()));
    line.append(") ");
    // line.append(l.state_parameter_sort_str(i)) ;
    line.append(parameter->second);  // sort of the variable
    //Following line of code is needed to avoid iteration over a changing object.
    for (std::vector< std::string >::iterator z = tmp.begin(); z !=  tmp.end() ; z++)
    {
      line.append(" \"");
      string str = *z;
      if (str.empty())
      {
        str ="-";
      }
      line.append(str);
      line.append("\"");
    }
    parseStateVarDescr(line, graph);
  }

  for (unsigned int si= 0; si<l.num_states(); ++si)
  {
    line.clear();
    /* if(l.has_process_parameters ())
    { */
    for (unsigned int i = 0; i < process_parameters.size(); ++i)
    {
      if (i != 0)
      {
        line.append(" ");
      }

      line.append(to_string(l.state_label(si)[i]));
    }
    // }
    parseStates(line, graph);
  }

  const std::vector<transition> &trans=l.get_transitions();
  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r) 
  {
    line.clear();
    line.append(to_string(r->from()+1));
    line.append(" ");
    line.append(to_string(r->to()+1));
    line.append(" \"");
    line.append(detail::pp(l.action_label(r->label())));
    line.append("\"");
    parseTransitions(
      line,
      graph);
  }

  mediator->updateProgress(1);
}


// ---------------------------
void Parser::writeFSMFile(
  const string& path,
  Graph* graph)
// ---------------------------
{
  size_t begIdx, endIdx;
  string fileName;
  string delims    = "\\/";

  ofstream file;
  string line = "";

  int lineCnt = 0;
  size_t lineTot = 0;

  file.open(path.c_str());
  if (!file.is_open())
  {
    throw mcrl2::runtime_error("Error opening file for writing.");
  }

  // get filename
  begIdx = path.find_last_of(delims);
  if (begIdx == string::npos)
  {
    begIdx = 0;
  }
  else
  {
    begIdx += 1;
  }
  endIdx   = path.size();
  fileName = path.substr(begIdx, endIdx-begIdx);

  // init progress
  lineCnt = 0;
  lineTot = graph->getSizeAttributes() + graph->getSizeNodes() + graph->getSizeEdges();
  mediator->initProgress(
    "Saving file",
    "Writing " + fileName,
    lineTot);


  // write state variable description
  for (size_t i = 0; i < graph->getSizeAttributes(); ++i)
  {
    line  = "";
    line.append(graph->getAttribute(i)->getName());
    line.append("(");
    line.append(Utils::intToStr((int) graph->getAttribute(i)->getSizeOrigValues()));
    line.append(") ");
    line.append(graph->getAttribute(i)->getType());
    line.append(" ");

    if (graph->getAttribute(i)->getSizeOrigValues() != 0)
    {
      for (size_t j = 0; j < graph->getAttribute(i)->getSizeCurValues(); ++j)
      {
        line.append("\"");
        line.append(graph->getAttribute(i)->getCurValue(j)->getValue());
        line.append("\"");

        if (j < graph->getAttribute(i)->getSizeCurValues()-1)
        {
          line.append(" ");
        }
      }
    }
    else
    {
      line.append("[");
      line.append(Utils::dblToStr(graph->getAttribute(i)->getLowerBound()));
      line.append(", ");
      line.append(Utils::dblToStr(graph->getAttribute(i)->getUpperBound()));
      line.append("]");
    }

    line.append("\n");

    file << line;

    if (lineCnt % 1000 == 0)
    {
      mediator->updateProgress(lineCnt);
    }
    ++lineCnt;
  }

  // write state vectors
  line = "---\n";
  file << line;

  for (size_t i = 0; i < graph->getSizeNodes(); ++i)
  {
    line = "";

    for (size_t j = 0; j < graph->getNode(i)->getSizeTuple(); ++j)
    {
      line.append(Utils::dblToStr(graph->getNode(i)->getTupleVal(j)));

      if (j < graph->getNode(i)->getSizeTuple()-1)
      {
        line.append(" ");
      }
    }

    line.append("\n");

    file << line;

    if (lineCnt % 1000 == 0)
    {
      mediator->updateProgress(lineCnt);
    }
    ++lineCnt;
  }

  // write transitions
  line = "---\n";
  file << line;

  for (size_t i = 0; i < graph->getSizeEdges(); ++i)
  {
    line = "";

    line.append(Utils::size_tToStr(graph->getEdge(i)->getInNode()->getIndex()+1));
    line.append(" ");
    line.append(Utils::size_tToStr(graph->getEdge(i)->getOutNode()->getIndex()+1));
    line.append(" \"");
    line.append(graph->getEdge(i)->getLabel());
    line.append("\"");

    line.append("\n");

    file << line;

    if (lineCnt % 1000 == 0)
    {
      mediator->updateProgress(lineCnt);
    }
    ++lineCnt;
  }

  file.close();

  // close progress
  mediator->closeProgress();
}


// -----------------------------------------------------
void Parser::parseAttrConfig(
  const string& path,
  Graph* graph,
  map< size_t, size_t > &attrIdxFrTo,
  map< size_t, vector< string > > &attrCurDomains,
  map< size_t, map< size_t, size_t  > > &attrOrigToCurDomains)
// -----------------------------------------------------
{
  wxXmlDocument doc;
  if (doc.Load(wxString(path.c_str(), wxConvUTF8)) == true)
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


// --------------------------
void Parser::writeAttrConfig(
  const string& path,
  Graph* graph)
// --------------------------
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

      /*
      // original domain
      domn = new wxXmlNode( "OriginalDomain" );
      attr->LinkEndChild( domn );
      {
      for ( int j = 0; j < graph->getAttribute(i)->getSizeOrigValues(); ++j )
      {
          // value
          valu = new wxXmlNode( "Value" );
          domn->LinkEndChild( valu );
          valu->LinkEndChild(
              new TiXmlText(
                  graph->getAttribute(i)->getOrigValue(j)->getValue().c_str() ) );
      }
      }
      */

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

    doc.Save(wxString(path.c_str(), wxConvUTF8));

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


// -----------------------
void Parser::parseDiagram(
  const string& path,
  Graph* graph,
  Diagram* dgrmOld,
  Diagram* dgrmNew)
// -----------------------
{
  wxXmlDocument doc;

  if (doc.Load(wxString(path.c_str(), wxConvUTF8)))
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


// -----------------------
void Parser::writeDiagram(
  const string& path,
  Graph* graph,
  Diagram* diagram)
// -----------------------
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
    ColorRGB col;
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

      diagram->getShape(i)->getLineColor(col);

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Red"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.r).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Green"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.g).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Blue"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.b).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Alpha"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.a).c_str(), wxConvUTF8));

      // color fill
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("FillColor"));
      shpe -> AddChild(prop);

      diagram->getShape(i)->getFillColor(col);
      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Red"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.r).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Green"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.g).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Blue"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.b).c_str(), wxConvUTF8));

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Alpha"));
      prop -> AddChild(subp);
      new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.a).c_str(), wxConvUTF8));

      // X center DOF
      prop = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("XCenterDOF"));
      shpe -> AddChild(prop);

      subp = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Attribute"));
      prop -> AddChild(subp);
      attr = diagram->getShape(i)->getDOFXCtr()->getAttribute();
      if (attr != NULL)
      {
        new wxXmlNode(subp, wxXML_TEXT_NODE, wxEmptyString, wxString(Utils::dblToStr(col.a).c_str(), wxConvUTF8));
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

    doc.Save(wxString(path.c_str(), wxConvUTF8));

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


// -----------------------------
void Parser::parseStateVarDescr(
  const string& nextLine,
  Graph* graph)
// -----------------------------
// ------------------------------------------------------------------
// This function is used by Parser::parseFsmFile() to parse the state
// description (first) part of an fsm file.
// ------------------------------------------------------------------
{
  try
  {
    vector< string > values;

    // description
    string::size_type begIdx = nextLine.find_first_not_of(delims);
    string::size_type endIdx = nextLine.find_first_of(delims);
    string token = nextLine.substr(begIdx, endIdx-begIdx);
    string name;
    if (token == "\n")
    {
      name = "unspecified";
    }
    else
    {
      name = token;
    }

    // cardinality
    begIdx = nextLine.find_first_not_of(delims, endIdx);
    endIdx = nextLine.find_first_of(delims, begIdx);
    token = nextLine.substr(begIdx, endIdx-begIdx);
    string card = token;

    // type
    begIdx = nextLine.find_first_not_of("() ", endIdx);
    endIdx = nextLine.find_first_of("() ", begIdx);
    token = nextLine.substr(begIdx, endIdx-begIdx);
    string type;
    if (token == "\n")
      // end of line
    {
      type = "Unspecified";
    }
    else if (token.substr(0, 1) == "\"")
    {
      // no type specified
      type = "Unspecified";
      endIdx = begIdx;
    }
    else
    {
      type = token;
    }

    // domain
    if (card != "0")
    {
      // all domain values are specified
      begIdx = nextLine.find_first_not_of(" \"", endIdx);
      endIdx = nextLine.find_first_of("\"", begIdx);

      while (endIdx != string::npos)
      {
        token = nextLine.substr(begIdx, endIdx-begIdx);
        if (token != "\n")
        {
          values.push_back(token);
        }
        begIdx = nextLine.find_first_not_of(" \"", endIdx);
        endIdx = nextLine.find_first_of("\"", begIdx);
      }

      // add new discrete attribute to graph
      graph->addAttrDiscr(
        name,
        type,
        graph->getSizeAttributes(),
        values);
    }
    else
    {
      // range of domain values are specified
      double lwrBnd = 0.0;
      double uprBnd = 0.0;

      // lower bound
      begIdx = nextLine.find_first_not_of(" [", endIdx);
      endIdx = nextLine.find_first_of(" ,", begIdx);
      if (endIdx != string::npos)
      {
        token  = nextLine.substr(begIdx, endIdx-begIdx);
        lwrBnd = Utils::strToDbl(token);

        // upper bound
        begIdx = nextLine.find_first_not_of(" ,", endIdx);
        endIdx = nextLine.find_first_of("]", begIdx);
        if (endIdx != string::npos)
        {
          token  = nextLine.substr(begIdx, endIdx-begIdx);
          uprBnd = Utils::strToDbl(token);
        }
      }

      // add new continuous attribute to graph
      graph->addAttrConti(
        name,
        type,
        graph->getSizeAttributes(),
        lwrBnd,
        uprBnd);
    }
  }
  catch (...)
  {
    throw mcrl2::runtime_error(
      "Error parsing state description.");
  }

}


// ------------------------
void Parser::parseStates(
  const string& nextLine,
  Graph* graph)
// ------------------------
// -----------------------------------------------------------------------
// This function is used by Parser::ParseFsmFile() to parse the states
// (second) part of an fsm file.
// -----------------------------------------------------------------------
{
  vector< double > stateVector;
  try
  {
    string::size_type begIdx = nextLine.find_first_not_of(delims);
    string::size_type endIdx = nextLine.find_first_of(delims);
    // get all tokens in line
    while (begIdx != string::npos)
    {
      string token = nextLine.substr(begIdx, endIdx-begIdx);
      if (token != "\n")
      {
        stateVector.push_back(Utils::strToDbl(token));
      }
      begIdx = nextLine.find_first_not_of(delims, endIdx);
      endIdx = nextLine.find_first_of(delims, begIdx);
    }

    // add new node to graph
    graph->addNode(stateVector);
  }
  catch (...)
  {
    throw mcrl2::runtime_error("Error parsing states.");
  }
}


// ---------------------------
void Parser::parseTransitions(
  const string& nextLine,
  Graph* graph)
// ---------------------------
// -----------------------------------------------------------------------
// This function is used by Parser::ParseFsmFile() to parse the
// transitions (third) part of an fsm file.
// -----------------------------------------------------------------------
{
  //vector< string > params;
  try
  {
    // index of from state
    string::size_type begIdx     = nextLine.find_first_not_of(delims);
    string::size_type endIdx     = nextLine.find_first_of(delims);
    string token      = nextLine.substr(begIdx, endIdx-begIdx);
    int frStateIdx = Utils::strToInt(token) - 1;

    // index (1-based) of to state
    begIdx     = nextLine.find_first_not_of(delims, endIdx);
    endIdx     = nextLine.find_first_of(delims, begIdx);
    token      = nextLine.substr(begIdx, endIdx-begIdx);
    int toStateIdx = Utils::strToInt(token) - 1;

    // label of transition
    begIdx = nextLine.find_first_not_of(delims, endIdx);
    string lblDelims = "()\"";
    endIdx = nextLine.find_first_of(lblDelims, begIdx);
    if (endIdx != string::npos)
    {
      token = nextLine.substr(begIdx, endIdx-begIdx);
    }
    else
    {
      token = "";
    }
    string trLbl = token;
    /*
    // transition parameters
    if ( endIdx != string::npos )
    {
        begIdx = nextLine.find_first_not_of( delims, endIdx );
        endIdx = nextLine.find_last_of( ")" );
        token = nextLine.substr( begIdx, endIdx-begIdx );
    }
    else
        token = "";
    if ( token != "\n" )
        params.push_back( token );
    */
    graph->addEdge(
      trLbl,
      frStateIdx,
      toStateIdx);
  }
  catch (...)
  {
    throw mcrl2::runtime_error("Error parsing transitions.");
  }
}


// ---------------------------------------------------
void Parser::parseAttrConfig(
  Graph* graph,
  map< size_t , size_t > &attrIdxFrTo,
  map< size_t, vector< string > > &attrCurDomains,
  map< size_t, map< size_t, size_t  > > &attrOrigToCurDomains,
  wxXmlNode* curNode)
// ---------------------------------------------------
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


// ----------------------------------------------------
void Parser::parseAttr(
  Graph* graph,
  map< size_t , size_t > &attrIdxFrTo,
  map< size_t, vector< string > > &attrCurDomains,
  map< size_t, map< size_t, size_t  > > &attrOrigToCurDomains,
  wxXmlNode* curNode)
// ----------------------------------------------------
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


// ------------------------
void Parser::parseDiagram(
  Graph* graph,
  Diagram* dgrmOld,
  Diagram* dgrmNew,
  wxXmlNode* curNode)
// ------------------------
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


// ------------------------
void Parser::parseShape(
  Graph* graph,
  Diagram* /*dgrmOld*/,
  Diagram* dgrmNew,
  wxXmlNode* curNode)
// ------------------------
{
  wxXmlNode* prop = NULL;
  wxXmlNode* subp = NULL;

  double xCtr, yCtr;
  double xDFC, yDFC;
  double aglCtr;

  int      type;
  double   lineWth;
  ColorRGB lineCol;
  ColorRGB fillCol;

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
    // red
    subp = prop->GetChildren();
    if (subp != NULL &&
        subp->GetName() == wxT("Red") &&
        subp->GetNodeContent() != wxEmptyString)
    {
      lineCol.r = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
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
      lineCol.g = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
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
      lineCol.b = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
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
      lineCol.a = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
    }
    else
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("LineColor: Missing alpha channel.");
    }
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
    // red
    subp = prop->GetChildren();
    if (subp != NULL &&
        subp->GetName() == wxT("Red") &&
        subp->GetNodeContent() != wxEmptyString)
    {
      fillCol.r = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
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
      fillCol.g = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
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
      fillCol.b = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
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
      fillCol.a = Utils::strToDbl(std::string(subp->GetNodeContent().mb_str()));
    }
    else
    {
      prop = NULL;
      subp = NULL;
      throw mcrl2::runtime_error("FillColor:Missing alpha channel.");
    }
  }
  else
  {
    prop = NULL;
    throw mcrl2::runtime_error("Missing fill color.");
  }

  // init shape
  Shape* s = new Shape(
    mediator,
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
