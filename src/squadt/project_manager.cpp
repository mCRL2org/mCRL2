#include <fstream>
#include <map>

#include <boost/filesystem/operations.hpp>

#include "project_manager.h"
#include "settings_manager.h"
#include "specification.h"
#include "xml_text_reader.h"
#include "ui_core.h"

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
  XMLTextReader reader(project_file.append("/").append(settings_manager.GetProjectFileName()).c_str());

  /* Maps an identifier to a pointer to a specification object */
  std::map < unsigned int, Specification* > identifier_resolution;

  /* From here on assume that the XML file satisfies the project-catalog XML schema. */
  if (!reader.StreamIsOpened()) {
    /* TODO Errors should be logged somewhere, but for the time std::cerr suffices */
#ifndef NDEBUG
    std::cerr << "Fatal: Unable to open project master file. (" << project_file << ")\n";
#endif

    return(false);
  }

#if defined(PARSER_SCHEMA_VALIDATION)
  if (!reader.SetSchemaForValidation("schemas/project.xsd")) {
    /* TODO Errors should be logged somewhere, but for the time std::cerr suffices */
#ifndef NDEBUG
    std::cerr << "Error: schema is not usable.\n";
#endif

    return (false);
  }
#endif

  try {
    Specification dummy(false);

    /* Read root element (project) */
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

    while (!reader.IsEndElement()) {
      /* Add new specification */
      specifications.push_back(dummy);

      /* Specification reads its content from reader */
      specifications.back().Read(reader);

      /* Update identifier resolution map */
      identifier_resolution[specifications.back().identifier] = &specifications.back();

      reader.Read();
    }
  }
  catch(int status) {
    if (status != 0) {
      /* TODO Errors should be logged somewhere, but for the time std::cerr suffices */
#ifndef NDEBUG
      std::cerr << "Fatal: Parse error(s) or unknown exception with parsing project.xsd.\n";
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
      const std::vector < SpecificationInputType >::iterator c = (*i).GetModifiableInputObjects().end();
            std::vector < SpecificationInputType >::iterator j = (*i).GetModifiableInputObjects().begin();

      while (j != c) {
        (*j).derived_from.pointer = identifier_resolution[(*j).derived_from.identifier];

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
  std::string   project_file(project_root);
  std::ofstream project_stream;

  project_file.append("/").append(settings_manager.GetProjectFileName());

  project_stream.open(project_file.c_str(), std::ios::out | std::ios::trunc);

  bool return_value = Write(project_stream);

  project_stream.close();

  return (return_value);
}

bool ProjectManager::Write(std::ostream& stream) {
  const std::list < Specification >::iterator b = specifications.end();
        std::list < Specification >::iterator i = specifications.begin();

  /* Write header */
  stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
         << "<project xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
         << " xsi:noNamespaceSchemaLocation=\"project.xsd\" version=\"1.0\">\n";

  while (i != b) {
    i->Write(stream);

    ++i;
  }

  /* Write footer */
  stream << "</project>\n";

  return (true);
}


/* Pretty print specification information */
void ProjectManager::Print(std::ostream& stream) {
  const std::list < Specification >::iterator b = specifications.end();
        std::list < Specification >::iterator i = specifications.begin();
        unsigned int                          n = 0;

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
 * TODO exception handling
 *
 * - If the a file exists that matches specification.location
 * - Verify that the location is a relative path that points to a place in the project directory
 */
Specification& ProjectManager::Add(Specification& specification) throw (Specification&) {
  /* Set identifier */
  specification.identifier = free_identifier++;

  specifications.push_back(specification);

  /* TODO store in a smarter way */
  Store();

  return (specifications.back());
}

bool ProjectManager::Remove(const Specification* specification) {
  std::list < Specification >::iterator i = specifications.begin();

  /* Specification is required to be present in the list */
  while (&(*i) != specification) {
    ++i;
  }

        std::vector < SpecificationOutputType >::const_iterator k = (*i).GetOutputObjects().begin();
  const std::vector < SpecificationOutputType >::const_iterator d = (*i).GetOutputObjects().end();

  while (k != d) {
    using namespace boost::filesystem;

    if (exists(path((*k).location))) {
      remove(path((*k).location));
    }
  }

  specifications.erase(i);

  /* TODO store in a smarter way */
  Store();

  return (true);
}

/* Specifications are required to be present in the list */
bool ProjectManager::Remove(const std::vector < Specification* >& some_specifications) {
        std::list   < Specification >::iterator        i = specifications.begin();
        std::vector < Specification* >::const_iterator j = some_specifications.begin();
  const std::vector < Specification* >::const_iterator c = some_specifications.end();

  /* Specification that a specification depends on must be stored on a lower index */
  while (j != c) {
    if (&(*i) == *j) {
            std::vector < SpecificationOutputType >::const_iterator k = (*i).GetOutputObjects().begin();
      const std::vector < SpecificationOutputType >::const_iterator d = (*i).GetOutputObjects().end();

      while (k != d) {
        using namespace boost::filesystem;

        if (exists(path((*k).location))) {
          remove(path((*k).location));
        }

        ++k;
      }

      i = specifications.erase(i);

      ++j;
    }
    else {
      ++i;
    }
  }

  /* TODO store in a smarter way */
  Store();

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

