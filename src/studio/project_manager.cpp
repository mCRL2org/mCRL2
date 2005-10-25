#include <iostream>
#include <fstream>
#include <map>

#include <stdio.h>

#include "project_manager.h"
#include "xml_text_reader.h"

ProjectManager::ProjectManager() {
  free_identifier = 0;
}

void ProjectManager::SetProjectDirectory(std::string directory) {
  project_root = directory;
}

std::string ProjectManager::GetProjectDirectory() {
  return(project_root);
}

void ProjectManager::SetDescription(std::string description) {
  project_description = description;
}

const std::string ProjectManager::GetDescription() {
  return (project_description);
}

const std::list < Specification >* ProjectManager::GetSpecifications() {
  return (&specifications);
}

bool ProjectManager::Close() {
  project_root        = "";
  project_description = "";
  free_identifier     = 0;

  specifications.clear();

  return (true);
}

/* Loads project configuration from XML file, project directory must be set in advance */
bool ProjectManager::Load() {
  std::string   project_file(project_root);
  bool          return_value = true;
  XMLTextReader reader(project_file.append("/studio.project").c_str());

  /* Maps an identifier to a pointer to a specification object */
  std::map < unsigned int, Specification* > identifier_resolution;

  /* From here on assume that the XML file satisfies the project-catalog XML schema. */
  if (!reader.StreamIsOpened()) {
    /* TODO Errors should be logged somewhere, but for the time std::cerr suffices */
#ifndef NDEBUG
    std::cerr << "Fatal: Unable to open project master file. (" << project_file << ")\n";
#endif

    /* Clean up */
    reader.~XMLTextReader();

    return(false);
  }

#if defined(PARSER_SCHEMA_VALIDATION)
  if (!reader.SetSchemaForValidation("schemas/studio_project.xsd")) {
    /* TODO Errors should be logged somewhere, but for the time std::cerr suffices */
#ifndef NDEBUG
    std::cerr << "Error: schema is not usable.\n";
#endif

    /* Clean up */
    reader.~XMLTextReader();

    return (false);
  }
#endif

  try {
    Specification dummy;

    /* Read root element (studio-project) */
    reader.Read();
 
    reader.Read();

    /* Active node, must be either an optional project description or a specification element */
    if (reader.IsElement("description")) {
      /* Proceed to content */
      reader.Read();

      /* Read attribute value */
      reader.GetValue(&project_description);

      /* To end tag */
      reader.Read();

      reader.Read();
    }

    do {
      /* Add new specification */
      specifications.push_back(dummy);

      /* Specification reads its content from reader */
      specifications.back().Read(reader);

      /* Update identifier resolution map */
      identifier_resolution[specifications.back().identifier] = &specifications.back();

      reader.Read();
    } while (!reader.IsEndElement());
  }
  catch(int status) {
    if (status != 0) {
      /* TODO Errors should be logged somewhere, but for the time std::cerr suffices */
#ifndef NDEBUG
      std::cerr << "Fatal: Parse error(s) or unknown exception with parsing studio-project.xsd.\n";
#endif

      return_value = false;
    }
  }

  /* Clean up */
  reader.Close();
  reader.Destroy();

  if (return_value) {
    /* Input completely parsed, replace identifiers by pointers to specifications */
    const std::list < Specification >::iterator b = specifications.end();
          std::list < Specification >::iterator i = specifications.begin();

    while (i != b) {
      const std::list < InputPair >::iterator c = (*i).input_objects.end();
            std::list < InputPair >::iterator j = (*i).input_objects.begin();

      while (j != c) {
        (*j).first = identifier_resolution[(unsigned int) (*j).first];

        ++j;
      }

      ++i;
    }

    /* Re-assign identifiers */
    i = specifications.begin();

    free_identifier = 0;

    while (i != b) {
      (*i).identifier = free_identifier++;

      ++i;
    }
  }

  return (return_value);
}

/*
 * Write project information to storage
 *
 * TODO :
 *
 *  - exception handling
 *  - atomicity of writing
 */
bool ProjectManager::Store() {
  const std::list < Specification >::iterator b = specifications.end();
        std::list < Specification >::iterator i = specifications.begin();

  std::string   project_file(project_root);
  std::ofstream project_stream;

  project_file.append("/studio.project");

  project_stream.open(project_file.c_str(), std::ios::out | std::ios::trunc);

  /* Write header */
  project_stream << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                 << "<studio-project xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
                 << "xsi:noNamespaceSchemaLocation=\"tool_catalog.xsd\">\n";

  while (i != b) {
    i->Write(project_stream);

    ++i;
  }

  /* Write footer */
  project_stream << "</studio-project>\n";

  project_stream.close();

  return (true);
}

/* Pretty print specification information */
void ProjectManager::Print(std::ostream& stream) {
  const std::list < Specification >::iterator b = specifications.end();
        std::list < Specification >::iterator i = specifications.begin();
  unsigned int                                n = 0;

  while (i != b) {
    stream << "Specification <" << n++ << ">" << std::endl << std::endl;

    i->Print(stream);

    ++i;
  }
}

/*
 * Notice
 *
 * - The specification.location must be a basename and is taken relative to the project root
 * - if specificaiton.location is not specified then a suitable location is determined
 *
 * TODO
 *
 * - If the a file exists that matches specification.location then NULL is returned
 * - Verify that the location is a relative path that points to a place in the project directory
 */
Specification* ProjectManager::Add(Specification& specification) {
  /* Set identifier */
  specification.identifier = free_identifier++;

  specifications.push_back(specification);

  return (&specifications.back());
}

bool ProjectManager::Remove(Specification* specification) {
  std::list < Specification >::iterator i = specifications.begin();

  /* Specification is required to be present in the list */
  while (&(*i) != specification) {
    ++i;
  }

  specifications.erase(i);

  return (true);
}

/*
 * Remove instance from a specification from storage
 */
bool ProjectManager::Flush(Specification* specification) {
  return(specification->Delete());
}

/*
 * Returns whether the specification was actually (re)generated
 */
bool ProjectManager::Update(Specification* specification) {
  bool changed = false;

  if (specification->CheckStatus()) {
    specification->Generate();

    changed = true;
  }

  return (changed);
}

/*
 * Returns whether the specification was actually (re)generated
 */
bool ProjectManager::UpdateAll() {
  std::list < Specification >::iterator i = specifications.begin();
  std::list < Specification >::iterator b = specifications.end();

  bool changed = false;

  while (i != b) {
    if (i->CheckStatus()) {
      i->Generate();

      changed = true;
    }

    ++i;
  }

  return (changed);
}

/**************************************************************************
 * Implementation of Specification methods
 **************************************************************************/
Specification::Specification() {
  uptodate           = false;
  name               = "";
  tool_configuration = "";
  tool_identifier    = UNSPECIFIED_TOOL;
}

bool Specification::IsUpToDate() {
  return (uptodate);
}

void Specification::SetNotUpToDate() {
  uptodate = false;
}

/*
 * Pretty prints the information about a specification
 *
 */
void Specification::Print(std::ostream& stream) {
  stream << " Pretty printed specification \\\n\n"
         << "  Name              : " << name << std::endl
         << "  Identifier        : " << identifier << std::endl
         << std::endl;
  if (0 < input_objects.size()) {
    stream << "  Command           : "
           << ((tool_configuration == "") ? "#unspecified#" : tool_configuration)
           << std::endl
           << "  Tool identifier   : ";
  }

  if (tool_identifier == UNSPECIFIED_TOOL) {
    stream << "#unspecified#";
  }
  else {
    stream << tool_identifier;
  }

  stream << " (should query tool manager to add tool name)\n";

  if (0 < input_objects.size()) {
    if (1 < input_objects.size()) {
      const std::list < InputPair >::iterator b = input_objects.end();
            std::list < InputPair >::iterator i = input_objects.begin();

      stream << "  Dependencies      :\n\n";

      while (i != b) {
        std::cerr << "    - " << (*i).second << std::endl;

        ++i;
      }
    }
    else {
      stream << "  Single dependency : " << input_objects.back().second << "\n";
    }
  }

  stream << " /\n";
}

/*
 * Recursively verifies whether specification is up to date by considering the
 * status of all specifications that it depends on.
 */
bool Specification::CheckStatus() {
  if (uptodate) {
    const std::list < InputPair >::iterator b = input_objects.end();
          std::list < InputPair >::iterator i = input_objects.begin();

    /* Recursively check status */
    while (i != b && (*i).first->CheckStatus()) {
      i++;
    }

    uptodate = (i != b);
  }
  
  return (uptodate);
}

/*
 * Recursively generates the specification and all not up to date
 * specifications it depends on. 
 *
 * Throws a pointer to the first specification that fails to be generated.
 */
bool Specification::Generate() throw (void*) {
  const std::list < InputPair >::iterator b = input_objects.end();
        std::list < InputPair >::iterator i = input_objects.begin();

  uptodate = false;

  /* Recursively generate specifications */
  while (i != b && (*i).first->Generate()) {
    i++;
  }
 
  if (i == b) {
    /* Run tool via the tool executor with command using the tool_identifier to lookup the name of a tool */
    if (TOOL_RUN_SUCCESSFUL) {
      /* For the moment this is in place instead of a call to the tool executor. Reason being that the tool executor has not been built yet. */
      uptodate = true;
    }
  }

  return (uptodate);
}

bool Specification::Delete() {
  const std::list < OutputPair >::iterator b = output_objects.end();
        std::list < OutputPair >::iterator i = output_objects.begin();
  bool  result = false;

  while (i != b) {
    FILE* handle = fopen((*i).second.c_str(), "r");
 
    if (handle != NULL) {
      /* File exists */
      fclose(handle);
      remove((*i).second.c_str());
 
      result = true;
    }
  }

  uptodate = false;

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
    uptodate = (temporary == "true" || temporary == "1");
  }
  else {
    uptodate = false;
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
     
      reader.Read();
     
      reader.GetValue(&tool_configuration);

      /* To end tag*/
      reader.Read();
      reader.Read();
    }

    /* Dependent specifications follow until node type is XML_READER_TYPE_END_ELEMENT */
    while (!reader.IsEndElement() && reader.IsElement("input-object")) {
      InputPair new_pair;

      /* Resolve object identifier to pointer, works only if there are no dependency cycles */
      reader.GetAttribute((unsigned int*) &new_pair.first, "identifier");
    
      reader.Read();

      reader.GetValue(&new_pair.second);

      input_objects.push_back(new_pair);

      /* To end tag*/
      reader.Read();
      reader.Read();
    }

    /* Dependent specifications follow until node type is XML_READER_TYPE_END_ELEMENT */
    while (!reader.IsEndElement() && reader.IsElement("output-object")) {
      OutputPair new_pair;

      /* Set file format */
      reader.GetAttribute(&new_pair.first, "format");

      reader.Read();

      /* Set file name */
      reader.GetValue(&new_pair.second);

      output_objects.push_back(new_pair);

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

  if (uptodate) {
    stream << " uptodate=\"true\"";
  }

  stream << ">\n";

  if (0 < input_objects.size()) {
    std::list < InputPair >::iterator b = input_objects.end();
    std::list < InputPair >::iterator i = input_objects.begin();

    stream << "  <tool-configuration tool-identifier=\"" << tool_identifier << "\">"
           << tool_configuration << "</tool-configuration>\n";

    while (i != b) {
      stream << "  <input-object identifier=\"" << (*i).first->identifier << "\">" << (*i).second << "</input-object>\n";
 
      ++i;
    }
  }

  if (0 < output_objects.size()) {
    std::list < OutputPair >::iterator b = output_objects.end();
    std::list < OutputPair >::iterator i = output_objects.begin();
 
    while (i != b) {
      stream << "  <output-object format=\"" << (*i).first << "\">" << (*i).second << "</output-object>\n";
 
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
