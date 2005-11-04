#include <iostream>
#include "tool.h"
#include "xml_text_reader.h"

/* Read tool information from XML using a libXML2 reader */
void Tool::Read(XMLTextReader& reader) {
  /* Active node, must constitute a tool profile (4 required attributes: name, identifier, location, category) */
  reader.GetAttribute(&name, "name");
  reader.GetAttribute(&identifier, "identifier");
  reader.GetAttribute(&location, "location");

  reader.Read();

  /* Read tool description, if it is there */
  if (reader.IsElement("description")) {
    /* Proceed to content */
    reader.Read();

    reader.GetValue(&description);

    /* To end tag*/
    reader.Read();
    reader.Read();
  }
  else {
    description = "";
  }

  /* Read in mode specifications */
  while (reader.IsElement("mode")) {
    ToolMode* new_mode = new ToolMode();

    new_mode->Read(reader);
    modes.push_back(new_mode);
  }

  /* Skip end of tool profile element */
  reader.Read();
}

/* Write tool information as XML to stream */
bool Tool::Write(std::ostream& stream) const {
  stream << " <tool name=\"" << name << "\" identifier=\"" << identifier
         << "\" location=\"" << location << "\">\n";

  if (description != "") {
    stream << "  <description>" << description << "</description>\n";
  }

  const std::vector < ToolMode* >::const_iterator b = modes.end();
        std::vector < ToolMode* >::const_iterator i = modes.begin();

  /* Print tool mode configurations */
  while (i != b) {
    (*i)->Write(stream);

    ++i;
  }

  stream << " </tool>\n";

  return (true);
}

void Tool::Print(std::ostream& stream) const {
  stream << " Pretty printed specification \\\n\n"
         << "  Name           : " << name << std::endl
         << "  Identifier     : " << identifier << std::endl
         << "  Location       : " << location << std::endl;

  if (description != "") {
    stream << "  Description    : \n\n`"
           << description << "'" << std::endl << std::endl;
  }
  else {
    stream << std::endl;
  }

  const std::vector < ToolMode* >::const_iterator b = modes.end();
        std::vector < ToolMode* >::const_iterator i = modes.begin();

  /* Print tool mode configurations */
  while (i != b) {
    (*i)->Print(stream);

    ++i;
  }

  stream << " /\n";
}

const std::vector < ToolMode* >& Tool::GetModes() const {
  return (modes);
}

Tool::Tool() {
  modes.clear();
}

Tool::~Tool() {
  const std::vector < ToolMode* >::const_iterator b = modes.end();
        std::vector < ToolMode* >::const_iterator i = modes.begin();

  while (i != b) {
    delete *i;

    ++i;
  }
}

/* Reads in the mode specification of a tool */
void ToolMode::Read(XMLTextReader& reader) {
  reader.GetAttribute(&category, "category");
  reader.GetAttribute(&selector, "selector");

  reader.Read();

  /* Read optional description */
  if (reader.IsElement("description")) {
    reader.Read();

    reader.GetValue(&description);

    reader.Read();
    reader.Read();
  }
  else {
    description = "";
  }

  /* Read in object specifications until an end element tag is encountered */
  while (!reader.IsEndElement()) {
    std::string type;
 
    reader.GetAttribute(&type, "type");
 
    if (type == "input") {
      ToolInputObject* dummy = new ToolInputObject();

      dummy->Read(reader);

      objects.push_back(dummy);
    }
    else {
      /* output */
      ToolOutputObject* dummy = new ToolOutputObject();

      dummy->Read(reader);

      objects.push_back(dummy);
    }
  }

  reader.Read();
}

bool ToolMode::Write(std::ostream& stream) const {

  if (0 < objects.size()) {
    stream << "  <mode";

    stream << " category=\"" << category << "\"";

    if (selector != "") {
      stream << " selector=\"" << selector << "\"";
    }

    std::list < ToolObject* >::const_iterator b = objects.end();
    std::list < ToolObject* >::const_iterator i = objects.begin();
 
    stream << ((description == "" && i == b) ? "/" : "") << ">\n";

    if (description != "") {
      stream << "   <description>" << description << "</description>\n";
    }

    /* Write objects */
    while (i != b) {
      (*i)->Write(stream);
 
      ++i;
    }

    stream << "  </mode>\n";
  }

  return (true);
}

void ToolMode::Print(std::ostream& stream) const {
  stream << "  Mode of operation ";

  if (selector != "") {
    /* Selection is explicit by command line argument */
    stream << "triggered with `" << selector << "' ";;
  }

  stream <<  "\\\n\n"
         << "  Category       : " << category << std::endl;

  if (description != "") {
    stream << "  Description    : \n\n`"
           << description << "'" << std::endl << std::endl;
  }
  else {
    stream << std::endl;
  }

  const std::list < ToolObject* >::const_iterator b = objects.end();
        std::list < ToolObject* >::const_iterator i = objects.begin();

  /* Print objects */
  while (i != b) {
    (*i)->Print(stream);

    ++i;
  }

  stream << "  /\n";
}

ToolMode::~ToolMode() {
  const std::list < ToolObject* >::iterator b = objects.end();
        std::list < ToolObject* >::iterator i = objects.begin();

  while (i != b) {
    delete *i;

    ++i;
  }
}

ToolObject::~ToolObject() {
}

void ToolObject::Print(std::ostream& stream) const {
  if (type == input) {
    dynamic_cast < const ToolInputObject* > (this)->Print(stream);
  }
  else {
    dynamic_cast < const ToolOutputObject* > (this)->Print(stream);
  }
}

ToolInputObject::ToolInputObject() {
  type = input;
}

ToolInputObject::~ToolInputObject() {
}

inline void ToolInputObject::Read(XMLTextReader& reader) {
  reader.GetAttribute(&formalism, "formalism");
  reader.GetAttribute(&name, "name");
  reader.GetAttribute(&selector, "selector");
  reader.GetAttribute(&default_format, "default-format");

  /* Read format selectors */
  if (!reader.IsEmptyElement()) {
    /* Skip end tag */
    reader.Read();

    while(!reader.IsEndElement()) {
      std::string format;
      std::string selector;
 
      /* Required attributes is format */
      reader.GetAttribute(&format, "format");

      /* Get text content of format-selector */
      reader.Read();

      reader.GetValue(&selector);

      format_selectors[format] = selector;

      /* Advance to end tag or next format-selector */
      reader.Read();
      reader.Read();
    }
  }

  /* Skip end of object element */
  reader.Read();
}

inline bool ToolInputObject::Write(std::ostream& stream) const {
  stream << "   <object type=\"input\" formalism=\"" << formalism << "\"";

  if (name != "") {
    stream << " name=\"" << name << "\"";
  }
  if (selector != "") {
    stream << " selector=\"" << selector << "\"";
  }
  if (default_format != "") {
    stream << " default-format=\"" << default_format << "\"";
  }

  if (0 < format_selectors.size()) {
    const std::map < std::string, std::string >::const_iterator b = format_selectors.end();
          std::map < std::string, std::string >::const_iterator i = format_selectors.begin();

    stream << ">\n";

    while (i != b) {
      stream << "    <format-selector format=\"" << (*i).first << "\">" << (*i).second << "</format-selector>\n";

      ++i;
    }

    stream << "   </object>\n";
  }
  else {
    stream << "/>\n";
  }

  return (true);
}

void ToolInputObject::Print(std::ostream& stream) const {
  stream << "   Input object :\n\n"
         << "    Formalism      : " << formalism << std::endl;

  if (name != "") {
    stream << "    Name           : " << name << std::endl;
  }
  if (selector != "") {
    stream << "    Selector       : " << selector << std::endl;
  }
  if (default_format != "") {
    stream << "    Default format : " << default_format << std::endl;
  }

  if (0 < format_selectors.size()) {
    const std::map < std::string, std::string >::const_iterator b = format_selectors.end();
          std::map < std::string, std::string >::const_iterator i = format_selectors.begin();

    stream << "\n    Format selectors : \n\n";

    while (i != b) {
      stream << "     `" << (*i).second << "' selects " << (*i).first << std::endl;

      ++i;
    }
  }

  stream << std::endl;
}

ToolOutputObject::ToolOutputObject() {
  type = output;
}

ToolOutputObject::~ToolOutputObject() {
}

inline void ToolOutputObject::Read(XMLTextReader& reader) {
  reader.GetAttribute(&formalism, "formalism");
  reader.GetAttribute(&name, "name");
  reader.GetAttribute(&selector, "selector");
  reader.GetAttribute(&default_format, "default-format");

  /* Read format selectors */
  if (!reader.IsEmptyElement()) {
    /* Skip end tag */
    reader.Read();
 
    while(!reader.IsEndElement()) {
      std::string format;
      std::string selector;
 
      /* Required attributes is format */
      reader.GetAttribute(&format, "format");
 
      /* Get text content of format-selector */
      reader.Read();
 
      reader.GetValue(&selector);
 
      format_selectors[format] = selector;

      /* Advance to end tag or next format-selector */
      reader.Read();
      reader.Read();
    }
  }

  /* Skip end of object element */
  reader.Read();
}

inline bool ToolOutputObject::Write(std::ostream& stream) const {
  stream << "   <object type=\"output\" formalism=\"" << formalism << "\"";

  if (name != "") {
    stream << " name=\"" << name << "\"";
  }
  if (selector != "") {
    stream << " selector=\"" << selector << "\"";
  }
  if (default_format != "") {
    stream << " default-format=\"" << default_format << "\"";
  }

  if (0 < format_selectors.size()) {
    const std::map < std::string, std::string >::const_iterator b = format_selectors.end();
          std::map < std::string, std::string >::const_iterator i = format_selectors.begin();

    stream << ">\n";

    while (i != b) {
      stream << "    <format-selector format=\"" << (*i).first << "\">" << (*i).second << "</format-selector>\n";

      ++i;
    }

    stream << "   </object>\n";
  }
  else {
    stream << "/>\n";
  }

  return (true);
}

void ToolOutputObject::Print(std::ostream& stream) const {
  stream << "   Output object :\n\n"
         << "    Formalism      : " << formalism << std::endl;

  if (name != "") {
    stream << "    Name           : " << name << std::endl;
  }
  if (selector != "") {
    stream << "    Selector       : " << selector << std::endl;
  }
  if (default_format != "") {
    stream << "    Default format : " << default_format << std::endl;
  }

  if (0 < format_selectors.size()) {
    const std::map < std::string, std::string >::const_iterator b = format_selectors.end();
          std::map < std::string, std::string >::const_iterator i = format_selectors.begin();

    stream << "\n    Format selectors : \n\n";

    while (i != b) {
      stream << "     `" << (*i).second << "' selects " << (*i).first << std::endl;

      ++i;
    }
  }

  stream << std::endl;
}

