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
    for (size_t j = 0; j < values.size(); ++j)
    {
      if (values[j].empty())
        values.assign(j, "-");
    }

    graph->addAttrDiscr(
          QString::fromStdString(parameter->first),
          QString::fromStdString(parameter->second),
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

  const std::vector<transition>& trans=l.get_transitions();
  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    graph->addEdge(
          pp(l.action_label(r->label(transition::default_label_map()))),
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
    QString name = graph->getAttribute(i)->name();
    QString sizeOrigValues = QString::number(int(graph->getAttribute(i)->getSizeOrigValues()));
    QString type = graph->getAttribute(i)->type();

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

    file.write(line.toLatin1());
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

    file.write(line.toLatin1());

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
    file.write(line.toLatin1());

    emit progressed(++lineCnt);
  }

  file.close();
}


void Parser::parseAttrConfig(
    QString filename,
    Graph* graph,
    map< size_t, size_t >& attrIdxFrTo,
    map< size_t, vector< string > >& attrCurDomains,
    map< size_t, map< size_t, size_t  > >& attrOrigToCurDomains)
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
  QString file = QFileInfo(graph->filename()).fileName();

  QDomDocument xml;
  QDomElement root = xml.createElement("Configuration");
  xml.appendChild(root);

  appendValue(xml, root, "File", file);

  // attributes
  for (size_t i = 0; i < graph->getSizeAttributes(); ++i)
  {
    Attribute* attribute = graph->getAttribute(i);
    QDomElement attributeElement = xml.createElement("Attribute");
    appendValue(xml, attributeElement, "Name", attribute->name());
    appendValue(xml, attributeElement, "Type", attribute->type());
    appendValue(xml, attributeElement, "OriginalCardinality", QString::number(attribute->getSizeOrigValues()));

    QDomElement currentDomainElement = xml.createElement("CurrentDomain");
    for (size_t j = 0; j < attribute->getSizeCurValues(); ++j)
    {
      appendValue(xml, currentDomainElement, "Value", QString::fromStdString(attribute->getCurValue(j)->getValue()));
    }
    attributeElement.appendChild(currentDomainElement);

    QDomElement originalToCurrentElement = xml.createElement("OriginalToCurrent");
    for (size_t j = 0; j < attribute->getSizeCurValues(); ++j)
    {
      appendValue(xml, originalToCurrentElement, "CurrentPosition", QString::number(int(attribute->mapToValue(j)->getIndex())));
    }
    attributeElement.appendChild(originalToCurrentElement);

    root.appendChild(attributeElement);
  }

  QFile data(filename);
  if (!data.open(QFile::WriteOnly | QFile::Truncate)) {
    throw mcrl2::runtime_error("Error opening file for writing.");
  }

  QTextStream out(&data);
  xml.save(out, 2);

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
  QString file = QFileInfo(graph->filename()).fileName();

  QMap<int, QString> types;
  types.insert(Shape::TYPE_LINE,     "TYPE_LINE");
  types.insert(Shape::TYPE_RECT,     "TYPE_RECT");
  types.insert(Shape::TYPE_ELLIPSE,  "TYPE_ELLIPSE");
  types.insert(Shape::TYPE_ARROW,    "TYPE_ARROW");
  types.insert(Shape::TYPE_DARROW,   "TYPE_DARROW");
  types.insert(Shape::TYPE_NOTE,     "TYPE_NOTE");

  QDomDocument xml;
  QDomElement root = xml.createElement("Diagram");
  xml.appendChild(root);

  appendValue(xml, root, "File", file);

  // shapes
  for (int i = 0; i < diagram->shapeCount(); ++i)
  {
    Shape* shape = diagram->shape(i);
    QDomElement shapeElement = xml.createElement("Shape");

    appendValue(xml, shapeElement, "XCenter", QString::number(shape->xCenter()));
    appendValue(xml, shapeElement, "YCenter", QString::number(shape->yCenter()));
    appendValue(xml, shapeElement, "XDistanceFromCenter", QString::number(shape->xDistance()));
    appendValue(xml, shapeElement, "YDistanceFromCenter", QString::number(shape->yDistance()));
    appendValue(xml, shapeElement, "XHinge", QString::number(shape->xHinge()));
    appendValue(xml, shapeElement, "YHinge", QString::number(shape->yHinge()));
    appendValue(xml, shapeElement, "AngleCenter", QString::number(shape->angle()));
    appendValue(xml, shapeElement, "Type", types[shape->shapeType()]);
    appendValue(xml, shapeElement, "LineWidth", QString::number(shape->lineWidth()));

    QColor lineColor = diagram->shape(i)->lineColor();
    QDomElement lineColorElement = xml.createElement("LineColor");
    appendValue(xml, lineColorElement, "Red", QString::number(lineColor.redF()));
    appendValue(xml, lineColorElement, "Green", QString::number(lineColor.greenF()));
    appendValue(xml, lineColorElement, "Blue", QString::number(lineColor.blueF()));
    appendValue(xml, lineColorElement, "Alpha", QString::number(lineColor.alphaF()));
    shapeElement.appendChild(lineColorElement);

    QColor fillColor = diagram->shape(i)->fillColor();
    QDomElement fillColorElement = xml.createElement("FillColor");
    appendValue(xml, fillColorElement, "Red", QString::number(fillColor.redF()));
    appendValue(xml, fillColorElement, "Green", QString::number(fillColor.greenF()));
    appendValue(xml, fillColorElement, "Blue", QString::number(fillColor.blueF()));
    appendValue(xml, fillColorElement, "Alpha", QString::number(fillColor.alphaF()));
    shapeElement.appendChild(fillColorElement);

    appendDOF(xml, shapeElement, "XCenterDOF", shape->xCenterDOF());
    appendDOF(xml, shapeElement, "YCenterDOF", shape->yCenterDOF());
    appendDOF(xml, shapeElement, "WidthDOF",   shape->widthDOF());
    appendDOF(xml, shapeElement, "HeightDOF",  shape->heightDOF());
    appendDOF(xml, shapeElement, "AngleDOF",   shape->angleDOF());
    appendDOF(xml, shapeElement, "XCenterDOF", shape->xCenterDOF());

    QDomElement colorDofElement = appendDOF(xml, shapeElement, "ColorDOF", shape->colorDOF());
    QList<double> colorYValues = shape->colorYValues();
    for (int i = 0; i < colorYValues.size(); ++i)
    {
      appendValue(xml, colorDofElement, "AuxilaryValue", QString::number(colorYValues[i]));
    }

    QDomElement opacityDofElement = appendDOF(xml, shapeElement, "OpacityDOF", shape->opacityDOF());
    QList<double> opacityYValues = shape->colorYValues();
    for (int i = 0; i < opacityYValues.size(); ++i)
    {
      appendValue(xml, opacityDofElement, "AuxilaryValue", QString::number(opacityYValues[i]));
    }

    root.appendChild(shapeElement);
  }

  QFile data(filename);
  if (!data.open(QFile::WriteOnly | QFile::Truncate)) {
    throw mcrl2::runtime_error("Error opening file for writing.");
  }

  QTextStream out(&data);
  xml.save(out, 2);
}


// -- private utility functions -------------------------------------

QMap<QString, QDomElement> Parser::findElements(QDomElement root, QStringList tagNames)
{
  QMap<QString, QDomElement> result;
  for (int i = 0; i < tagNames.size(); ++i)
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
  for (int i = 0; i < tagNames.size(); ++i)
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

  for (int i = 0; i < tagNames.size(); ++i)
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

  for (int i = 0; i < tagNames.size(); ++i)
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

  for (int i = 0; i < tagNames.size(); ++i)
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

QDomElement Parser::appendValue(QDomDocument document, QDomElement root, QString tag, QString val)
{
  QDomElement element = document.createElement(tag);
  QDomText textElement = document.createTextNode(val);
  element.appendChild(textElement);
  root.appendChild(element);
  return element;
}

QDomElement Parser::appendDOF(QDomDocument document, QDomElement root, QString name, DOF* dof)
{
  QDomElement element = document.createElement(name);
  Attribute* attribute = dof->attribute();
  appendValue(document, element, "Attribute", (attribute == 0 ? QString("") : attribute->name()));

  for (int i = 0; i < dof->valueCount(); ++i)
  {
    appendValue(document, element, "Value", QString::number(dof->value(i)));
  }

  root.appendChild(element);
  return element;
}

void Parser::parseAttribute(
    Graph* graph,
    map< size_t , size_t >& attrIdxFrTo,
    map< size_t, vector< string > >& attrCurDomains,
    map< size_t, map< size_t, size_t  > >& attrOrigToCurDomains,
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

    Attribute* attribute = graph->getAttribute(stringPropertyValues["Name"]);
    if (attribute == 0)
    {
      throw mcrl2::runtime_error(QString("Invalid attribute name (%1).").arg(stringPropertyValues["Name"]).toStdString());
    }

    if (attribute->type() != stringPropertyValues["Type"])
    {
      throw mcrl2::runtime_error(QString("Types do not match (%1 vs %2).").arg(attribute->type(), stringPropertyValues["Type"]).toStdString());
    }

    if (int(attribute->getSizeOrigValues()) != originalCardinality)
    {
      throw mcrl2::runtime_error(QString("Cardinalities do not match (%1 vs %2).").arg(attribute->getSizeOrigValues()).arg(originalCardinality).toStdString());
    }

    // update attribute mapping
    attrIdxFrTo.insert(pair< size_t, size_t >(attribute->getIndex(), attrIdxFrTo.size()));

    vector< string > domain;
    QDomElement valueElement = listPropertyElements["CurrentDomain"].firstChildElement("Value");
    for (int i = 0; !valueElement.isNull(); ++i, valueElement = valueElement.nextSiblingElement("Value"))
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
    for (i = 0; !positionElement.isNull(); ++i, positionElement = positionElement.nextSiblingElement("CurrentPosition"))
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
    for (int i = 0; i < colorProperties.size(); ++i)
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
          diagram->shapeCount(),
          doublePropertyValues["XCenter"],   doublePropertyValues["YCenter"],
          doublePropertyValues["XDistanceFromCenter"],   doublePropertyValues["YDistanceFromCenter"],
          doublePropertyValues["AngleCenter"], type,
          doublePropertyValues["XHinge"], doublePropertyValues["YHinge"]);
    shape->setLineWidth(doublePropertyValues["LineWidth"]);
    shape->setLineColor(colorPropertyValues["LineColor"]);
    shape->setFillColor(colorPropertyValues["FillColor"]);

    QMap<QString, DOF*> dofs;
    dofs.insert("XCenterDOF", shape->xCenterDOF());
    dofs.insert("YCenterDOF", shape->yCenterDOF());
    dofs.insert("WidthDOF",   shape->widthDOF());
    dofs.insert("HeightDOF",  shape->heightDOF());
    dofs.insert("AngleDOF",   shape->angleDOF());
    dofs.insert("ColorDOF",   shape->colorDOF());
    dofs.insert("OpacityDOF", shape->opacityDOF());

    QStringList dofTags = dofs.keys();
    QMap<QString, QDomElement> dofElements = findElements(shapeNode.toElement(), dofTags);

    for (int i = 0; i < dofTags.size(); ++i)
    {
      try
      {
        QDomElement dofElement = dofElements[dofTags[i]];
        DOF* dof = dofs[dofTags[i]];
        QMap<QString, QDomElement> attributeElements = findElements(dofElement, QStringList("Attribute"));
        QDomElement attributeElement = attributeElements["Attribute"];
        if (!attributeElement.text().isEmpty())
        {
          Attribute* attribute = graph->getAttribute(attributeElement.text());
          if (attribute != 0)
          {
            dof->setAttribute(attribute);
          }
        }
        QDomElement valueElement = dofElement.firstChildElement("Value");
        for (int j = 0; !valueElement.isNull(); ++j, valueElement = valueElement.nextSiblingElement("Value"))
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
          for (int j = 0; !valueElement.isNull(); ++j, valueElement = valueElement.nextSiblingElement("AuxilaryValue"))
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
          for (int j = 0; !valueElement.isNull(); ++j, valueElement = valueElement.nextSiblingElement("AuxilaryValue"))
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
