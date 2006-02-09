#include <iostream>

#include <xml_text_reader.h>

#include "tool.h"

namespace squadt {
  namespace tool {

    /* Read tool information from XML using a libXML2 reader */
    void Tool::Read(xml2pp::text_reader& reader) {
      /* Active node, must constitute a tool profile (4 required attributes: name, identifier, location, category) */
      reader.get_attribute(&name, "name");
      reader.get_attribute(&identifier, "identifier");
      reader.get_attribute(&location, "location");
    
      reader.read();
    
      /* Read tool description, if it is there */
      if (reader.is_element("description")) {
        /* Proceed to content */
        reader.read();
    
        reader.get_value(&description);
    
        /* To end tag*/
        reader.read();
        reader.read();
      }
      else {
        description = "";
      }
    
      /* Read in mode specifications */
      while (reader.is_element("mode")) {
        ToolMode* new_mode = new ToolMode();
    
        new_mode->Read(reader);
        modes.push_back(new_mode);
      }
    
      /* Skip end of tool profile element */
      reader.read();
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
    void ToolMode::Read(xml2pp::text_reader& reader) {
      reader.get_attribute(&category, "category");
      reader.get_attribute(&selector, "selector");
    
      reader.read();
    
      /* Read optional description */
      if (reader.is_element("description")) {
        reader.read();
    
        reader.get_value(&description);
    
        reader.read();
        reader.read();
      }
      else {
        description = "";
      }
    
      /* Read in object specifications until an end element tag is encountered */
      while (!reader.is_end_element()) {
        std::string type;
     
        reader.get_attribute(&type, "type");
     
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
    
      reader.read();
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
    
    /* Return output object number <|object_number|> (must exist) */
    const ToolOutputObject& ToolMode::GetOutputObject(size_t object_number) const {
      std::list < ToolObject* >::const_iterator i = objects.begin();
      bool                                      b = (*i)->GetType() != output;
    
      while (b || 0 < object_number) {
        if (!b) {
          --object_number;
        }
    
        ++i;
    
        b = (*i)->GetType() != output;
      }
    
      return (reinterpret_cast < const ToolOutputObject& > (*(*i)));
    }
    
    /* Generates a name based on <|name|> and the restrictions of output with respect to input objects of this mode */
    const std::string ToolMode::ChooseName(size_t object_number, std::string name) const {
      std::list < ToolObject* >::const_iterator i = objects.begin();
      bool                                      b = (*i)->GetType() != output;
    
      while (b || 0 < object_number) {
        if (!b) {
          --object_number;
        }
    
        ++i;
    
        b = (*i)->GetType() != output;
      }
    
      /* name is taken to be the prefix of the chosen name */
      return (name.append(".").append((*i)->GetSomeFormat()));
    }
    
    /* Return input object number <|object_number|> (must exist) */
    const ToolInputObject& ToolMode::GetInputObject(size_t object_number) const {
      std::list < ToolObject* >::const_iterator i = objects.begin();
      bool                                      b = (*i)->GetType() != input;
    
      while (b || 0 < object_number) {
        if (!b) {
          --object_number;
        }
    
        ++i;
    
        b = (*i)->GetType() != input;
      }
    
      return (reinterpret_cast < const ToolInputObject& > (*(*i)));
    }
    
    /* Returns whether the mode produces output objects */
    const bool ToolMode::HasOutputObjects() const {
      std::list < ToolObject* >::const_iterator i = objects.begin();
    
      while (i != objects.end()) {
        if ((*i)->GetType() == output) {
          break;
        }
    
        ++i;
      }
    
      return (i != objects.end());
    }
    
    /* Returns whether the mode requires input objects */
    const bool ToolMode::HasInputObjects() const {
      std::list < ToolObject* >::const_iterator i = objects.begin();
    
      while (i != objects.end()) {
        if ((*i)->GetType() == input) {
          break;
        }
    
        ++i;
      }
    
      return (i != objects.end());
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
    
    inline void ToolInputObject::Read(xml2pp::text_reader& reader) {
      reader.get_attribute(&formalism, "formalism");
      reader.get_attribute(&name, "name");
      reader.get_attribute(&selector, "selector");
      reader.get_attribute(&default_format, "default-format");
    
      obligatory = !reader.get_attribute("virtual");
    
      /* Read format selectors */
      if (!reader.is_empty_element()) {
        /* Skip end tag */
        reader.read();
    
        while(!reader.is_end_element()) {
          std::string format;
          std::string selector;
     
          /* Required attributes is format */
          reader.get_attribute(&format, "format");
    
          /* Get text content of format-selector */
          reader.read();
    
          reader.get_value(&selector);
    
          format_selectors[format] = selector;
    
          /* Advance to end tag or next format-selector */
          reader.read();
          reader.read();
        }
      }
    
      /* Skip end of object element */
      reader.read();
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
      if (!obligatory) {
        stream << " virtual=\"true\"";
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
      if (!obligatory) {
        stream << "    Virtual        : yes\n";
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
    
    inline void ToolOutputObject::Read(xml2pp::text_reader& reader) {
      reader.get_attribute(&formalism, "formalism");
      reader.get_attribute(&name, "name");
      reader.get_attribute(&selector, "selector");
      reader.get_attribute(&default_format, "default-format");
    
      obligatory = !reader.get_attribute("virtual");
    
      /* Read format selectors */
      if (!reader.is_empty_element()) {
        /* Skip end tag */
        reader.read();
     
        while(!reader.is_end_element()) {
          std::string format;
          std::string selector;
     
          /* Required attributes is format */
          reader.get_attribute(&format, "format");
     
          /* Get text content of format-selector */
          reader.read();
     
          reader.get_value(&selector);
     
          format_selectors[format] = selector;
    
          /* Advance to end tag or next format-selector */
          reader.read();
          reader.read();
        }
      }
    
      /* Skip end of object element */
      reader.read();
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
      if (!obligatory) {
        stream << " virtual=\"true\"";
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
      if (!obligatory) {
        stream << "    Virtual        : yes\n";
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
  }
}

