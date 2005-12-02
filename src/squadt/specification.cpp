#include <list>

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
        std::cerr << "    - " << (*i).derived_from.pointer->GetOutputObjects()[(*i).output_number].file_name << std::endl;

        ++i;
      }
    }
    else {
      stream << "  Single dependency : " << input_objects.front().derived_from.pointer->GetOutputObjects()[0].file_name << "\n";
    }
  }

  stream << " /\n";
}

/*
 * Recursively verifies whether specification is up to date by considering the
 * status of all specifications that it depends on.
 */
SpecificationStatus Specification::CheckStatus() {
  const std::vector < SpecificationInputType >::iterator b = input_objects.end();
        std::vector < SpecificationInputType >::iterator i = input_objects.begin();

  if (status != non_existent && status != being_computed) {
    bool go_condition = true;

    /* Recursively check status */
    while (i != b) {
      go_condition = (*i).derived_from.pointer->CheckStatus();
 
      i++;
    }

    /* TODO verify minimum output object date is not before than maximum input object date */
    if (go_condition && status == not_up_to_date) {
      status = up_to_date;

      visualiser->VisualiseStatusChange(status);
    }
  }
  
  return (status);
}

/*
 * Recursively generates the specification and all not up to date
 * specifications it depends on. 
 *
 * TODO Throws a pointer to the first specification that fails to be generated.
 */
bool Specification::Generate() throw (void*) {
  const std::vector < SpecificationInputType >::iterator b            = input_objects.end();
        std::vector < SpecificationInputType >::iterator i            = input_objects.begin();
        bool                                             go_condition = false;

  /* Recursively generate specifications */
  try {
    while (i != b) {
      go_condition |= (*i).derived_from.pointer->Generate();
 
      i++;
    }
  }
  catch (void* p) {
    throw (p);

    return (false);
  }
 
  if (go_condition || status != up_to_date) {
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
       
        final_configuration.append((*i)->String(aobject->file_name, aobject->format));
      }

      ++i;
    }

    /* Set status, for the convenience of the user */
    status = being_computed;
 
    visualiser->VisualiseStatusChange(status);

    /* Run tool via the tool executor with command using the tool_identifier to lookup the name of a tool */
    if (tool_executor.Execute(tool_manager, tool_identifier, final_configuration, std::cerr)) {
      /* For the moment this is in place instead of a call to the tool executor. Reason being that the tool executor has not been built yet. */
      status = up_to_date;
    }
    else {
      /* Give user some error */
      status = not_up_to_date;

      throw (&*(--i));
    }

    visualiser->VisualiseStatusChange(status);
  }

  return (go_condition);
}

bool Specification::Delete() {
  const std::vector < SpecificationOutputType >::iterator b = output_objects.end();
        std::vector < SpecificationOutputType >::iterator i = output_objects.begin();
  bool  result = false;

  while (i != b) {
    FILE* handle = fopen((*i).file_name.c_str(), "r");
 
    if (handle != NULL) {
      /* File exists */
      fclose(handle);
      remove((*i).file_name.c_str());
 
      result = true;
    }
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
      SpecificationOutputType new_output;

      /* Set file format */
      reader.GetAttribute(&new_output.format, "format");

      reader.Read();

      /* Set file name */
      reader.GetValue(&new_output.file_name);

      output_objects.push_back(new_output);

      /* To end tag*/
      reader.Read();
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
      stream << "  <input-object identifier=\"" << (*i).derived_from.pointer->identifier
             << "\" output-number=\"" << (*i).output_number << "\"/>\n";
 
      ++i;
    }
  }

  if (0 < output_objects.size()) {
    std::vector < SpecificationOutputType >::const_iterator b = output_objects.end();
    std::vector < SpecificationOutputType >::const_iterator i = output_objects.begin();
 
    while (i != b) {
      stream << "  <output-object format=\"" << (*i).format << "\">" << (*i).file_name << "</output-object>\n";
 
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

