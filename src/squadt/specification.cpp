#include <list>
#include <fstream>

#include <boost/filesystem/operations.hpp>

#include "specification.h"
#include "xml_text_reader.h"
#include "tool_executor.h"
#include "tool_manager.h"
#include "ui_core.h"

SpecificationVisualiser Specification::dummy_visualiser;

Specification::Specification(bool exists) {
  status = (exists) ? up_to_date : non_existent;

  name               = "";
  tool_configuration = "";
  tool_identifier    = UNSPECIFIED_TOOL;
  visualiser         = &dummy_visualiser;
}

/*
 * Pretty prints the information about a specification
 *
 */
void Specification::Print(std::ostream& stream) const {
  stream << " Pretty printed specification \\\n\n"
         << "  Name              : " << name << std::endl
         << "  Identifier        : " << identifier << std::endl
         << std::endl;

  if (0 < input_objects.size()) {
    stream << "  Command           : "
           << ((tool_configuration == "") ? "#unspecified#" : tool_configuration)
           << std::endl
           << "  Tool identifier   : ";

    if (tool_identifier == UNSPECIFIED_TOOL) {
      stream << "#unspecified#";
    }
    else {
      stream << tool_identifier;
    }

    stream << std::endl;
  }

  if (0 < input_objects.size()) {
    if (1 < input_objects.size()) {
      const std::vector < SpecificationInputType >::const_iterator b = input_objects.end();
            std::vector < SpecificationInputType >::const_iterator i = input_objects.begin();

      stream << "  Dependencies      :\n\n";

      while (i != b) {
        std::cerr << "    - " << (*i).derived_from.pointer->GetOutputObjects()[(*i).output_number].location << std::endl;

        ++i;
      }
    }
    else {
      stream << "  Single dependency : " << input_objects.front().derived_from.pointer->GetOutputObjects()[0].location << "\n";
    }
  }

  stream << " /\n";
}

/*
 * Checks whether the status of the instances (files) match the status of the
 * specification. Returns a boolean: whether there was a match. Updates the status.
 */
inline bool Specification::CheckInstances() {
  const std::vector < SpecificationOutputType >::iterator c = output_objects.end();
        std::vector < SpecificationOutputType >::iterator j = output_objects.begin();
 
  using namespace boost::filesystem;
 
  assert(status == up_to_date);
 
  /* Verify status of output files */
  while (j != c) {
    path location = path((*j).location);
 
    if ((*j).timestamp < last_write_time(location)) {
      /* Verify checksum for changes */
      std::ifstream object;
 
      object.open(location.string().c_str());
     
      if (object.good()) {
        /* File exists */
        md5::compact_digest old;
     
        (*j).timestamp = last_write_time(location);
        (*j).checksum  = md5::MD5::MD5_Sum(object);
     
        object.close();
 
        if (!(old == (*j).checksum)) {
          if (0 < input_objects.size()) {
            /* Not a primary object */
            status = not_up_to_date;
          }

          return (false);
        }
      }
      else {
        if (0 < input_objects.size()) {
          /* Not a primary object */
          status = not_up_to_date;
        }

        return (false);
      }
    }
 
    ++j;
  }

  return (false);
}

/*
 * Recursively verifies whether specification is up to date by considering the
 * status of all specifications that it depends on.
 *
 * Returns implied status
 */
SpecificationStatus Specification::CheckStatus() {
  if (input_objects.size() == 0) {
    if (CheckInstances()) {
      return (up_to_date);
    }
    else {
std::cerr << "name : " << name << std::endl;
      return (not_up_to_date);
    }
  }

  if (status != non_existent && status != being_computed) {
    const std::vector < SpecificationInputType >::iterator b = input_objects.end();
          std::vector < SpecificationInputType >::iterator i = input_objects.begin();

    bool go_condition = true;

    /* Recursively check status */
    while (i != b && go_condition) {
      go_condition = (*i).derived_from.pointer->CheckStatus() == up_to_date;
 
      i++;
    }

    if (go_condition) {
      if (status == up_to_date && !CheckInstances()) {
        status = not_up_to_date;
      }
    }
    else {
      status = not_up_to_date;
    }

    visualiser->VisualiseStatusChange(status);
  }
  
  return (status);
}

/*
 * Recursively generates the specification and all not up to date
 * specifications it depends on. Returns true if and only if any of the outputs
 * was changed
 *
 * TODO Throws a pointer to the first specification that fails to be generated.
 */
bool Specification::Generate() throw (void*) {
  const std::vector < SpecificationInputType >::iterator b            = input_objects.end();
        std::vector < SpecificationInputType >::iterator i            = input_objects.begin();
        bool                                             go_condition = true;

  /* Recursively generate specifications */
  while (i != b) {
    go_condition |= (*i).derived_from.pointer->Generate();
 
    i++;
  }

  if (status == up_to_date) {
    CheckInstances();
  }

  if (go_condition && status != up_to_date) {
    const Tool*     tool = tool_manager.GetTool(tool_identifier);
    const ToolMode& mode = tool->GetMode(tool_mode);

    const std::list < ToolObject* >::const_iterator b = mode.GetObjects().end();
          std::list < ToolObject* >::const_iterator i = mode.GetObjects().begin();
          size_t it = 0;
          size_t ot = 0;

    std::string final_configuration = tool_configuration;

    if (mode.HasSelector()) {
      final_configuration.append(" ").append(mode.GetSelector());
    }

    /* Generate input output object arguments */
    while (i != b) {
      SpecificationOutputType* aobject;

      if ((*i)->IsObligatory()) {
        if ((*i)->GetType() == input) {
          aobject = const_cast < SpecificationOutputType* > (&input_objects[it].derived_from.pointer->GetOutputObjects()[input_objects[it].output_number]);
       
          ++it;
        }
        else {
          aobject = &(output_objects[ot]);
       
          ++ot;
        }
       
        final_configuration.append((*i)->String(aobject->location, aobject->format));
      }

      ++i;
    }

    /* Set status, for the convenience of the user */
    status = being_computed;
 
    visualiser->VisualiseStatusChange(status);

    /* Run tool via the tool executor with command using the tool_identifier to lookup the name of a tool */
    if (tool_executor.Execute(tool_manager, tool_identifier, final_configuration, std::cerr)) {
      status = up_to_date;
    }
    else {
      /* Give user some error */
      status = not_up_to_date;

      throw (&*(--i));
    }

    visualiser->VisualiseStatusChange(status);

    return (true);
  }

  return (false);
}

bool Specification::Delete() {
  const std::vector < SpecificationOutputType >::iterator b = output_objects.end();
        std::vector < SpecificationOutputType >::iterator i = output_objects.begin();
  bool  result = false;

  while (i != b) {
    FILE* handle = fopen((*i).location.c_str(), "r");
 
    if (handle != NULL) {
      /* File exists */
      fclose(handle);
      remove((*i).location.c_str());
 
      result = true;
    }

    /* Reset checksum and timestamp */
    md5::zero_out((*i).checksum);

    (*i).timestamp = 0;
  }

  status = non_existent;

  return (result);
}

/*
 * Read from XML
 *
 * Precondition: should xmlTextReaderPtr should point to a specification element
 *
 * Notice that pointers in input_objects are NOT restored via this function.
 *
 * TODO :
 *  - Exception handling what if writing to stream fails
 */
bool Specification::Read(XMLTextReader& reader) throw (int) {
  std::string temporary;

  reader.GetAttribute(&name, "name");
  reader.GetAttribute(&identifier, "identifier");

  /* Is specification explicitly marked up to date, or not */
  if (reader.GetAttribute(&temporary, "uptodate")) {
    if (temporary == "true" || temporary == "1") {
      status = up_to_date;
    }
  }
  else {
    status = non_existent;
  }

  if (!reader.IsEmptyElement()) {
    reader.Read();

    /* Active node, must be either an optional description for a specification or a tool-configuration */
    if (reader.IsElement("description")) {
      /* Proceed to content */
      reader.Read();

      reader.GetValue(&description);
   
      /* To end tag*/
      reader.Read();
      reader.Read();
    }

    /* Process tool-configuration tag */
    if (reader.IsElement("tool-configuration")) {
      /* Retrieve command: the value of the tool name */
      reader.GetAttribute(&tool_identifier, "tool-identifier");
      reader.GetAttribute(&tool_mode, "tool-mode");
     
      reader.Read();
     
      reader.GetValue(&tool_configuration);

      /* To end tag*/
      reader.Read();

      if (!reader.IsEmptyElement()) {
        reader.Read();
      }
    }

    if (!reader.IsElement("input-object")) {
      /* No input objects, so set status accordingly */
      status = up_to_date;
    }

    /* Dependent specifications follow until node type is XML_READER_TYPE_END_ELEMENT */
    while (!reader.IsEndElement() && reader.IsElement("input-object")) {
      SpecificationInputType new_input;

      /* Resolve object identifier to pointer, works only if there are no dependency cycles */
      reader.GetAttribute(&new_input.derived_from.identifier, "identifier");
    
      /* The number of the output object of the derived specification */
      if (!reader.GetAttribute(&new_input.output_number, "output-number")) {
        new_input.output_number = 0; /* Default mode */
      }

      input_objects.push_back(new_input);

      /* Past end tag*/
      reader.Read();
    }

    /* Dependent specifications follow until node type is XML_READER_TYPE_END_ELEMENT */
    while (!reader.IsEndElement() && reader.IsElement("output-object")) {
      std::string checksum;

      SpecificationOutputType new_output;

      /* Set file format */
      reader.GetAttribute(&new_output.format, "format");

      md5::zero_out(new_output.checksum);

      if (reader.GetAttribute(&checksum, "checksum")) {
        md5::convert(new_output.checksum, checksum.c_str());
      }

      /* Set file name */
      reader.GetAttribute(&new_output.location, "name");

      /* Set file name */
      if (!reader.GetAttribute((unsigned long*) &new_output.timestamp, "timestamp")) {
        new_output.timestamp = 0;
      }

      output_objects.push_back(new_output);

      /* To end tag*/
      reader.Read();
    }
  }

  return (true);
}

/*
 * Write as XML to stream
 *
 * TODO :
 *  - Exception handling what if writing to stream fails
 */
bool Specification::Write(std::ostream& stream) {
  /* Complex block */
  stream << " <specification name=\"" << name << "\" identifier=\""
         << identifier << "\"";

  if (status == up_to_date) {
    stream << " uptodate=\"true\"";
  }

  stream << ">\n";

  if (0 < input_objects.size()) {
    std::vector < SpecificationInputType >::const_iterator b = input_objects.end();
    std::vector < SpecificationInputType >::const_iterator i = input_objects.begin();

    stream << "  <tool-configuration tool-identifier=\"" << tool_identifier;

    if (tool_mode != 0) {
      stream << "\" tool-mode=\"" << tool_mode;
    }

    stream << "\">" << tool_configuration << "</tool-configuration>\n";

    while (i != b) {
      stream << "  <input-object identifier=\"" << std::dec << (*i).derived_from.pointer->identifier
             << "\" output-number=\"" << (*i).output_number << "\"/>\n";
 
      ++i;
    }
  }

  if (0 < output_objects.size()) {
    std::vector < SpecificationOutputType >::iterator b = output_objects.end();
    std::vector < SpecificationOutputType >::iterator i = output_objects.begin();
 
    while (i != b) {
      stream << "  <output-object format=\"" << (*i).format;

      if (is_zero((*i).checksum)) {
        /* For objects stored locally... */
        std::ifstream object;
        
        object.open((*i).location.c_str());

        if (object.good()) {
          /* File exists */
          using namespace md5;
          using namespace boost::filesystem;

          (*i).timestamp = last_write_time(path((*i).location));
          (*i).checksum  = MD5::MD5_Sum(object);

          object.close();
        }
      }
      else {
        stream << "\" checksum=\"" << (*i).checksum;
      }

      stream << "\" name=\"" << (*i).location;

      if ((*i).timestamp != 0) {
        stream << "\" timestamp=\"" << std::dec << (*i).timestamp;
      }

      stream << "\"/>\n";
 
      ++i;
    }
  }

  stream << " </specification>\n";

  return (true);
}

bool Specification::Commit() {
  /* Will be implemented at a later time */
  return (false);
}

