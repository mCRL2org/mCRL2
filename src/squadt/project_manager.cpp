#include <fstream>
#include <map>

#include <boost/filesystem/operations.hpp>

#include <xml2pp/detail/text_reader.tcc>

#include "project_manager.h"
#include "settings_manager.tcc"
#include "specification.h"
#include "ui_core.h"

namespace squadt {

  namespace bf = boost::filesystem;

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
    xml2pp::text_reader::file_name< std::string > f(settings_manager::path_concatenate(project_root, settings_manager::project_definition_base_name));

    if (!bf::exists(bf::path(f.get()))) {
      return (false);
    }

    xml2pp::text_reader reader(f);

    reader.set_schema(xml2pp::text_reader::file_name< std::string >(
                            _settings_manager->path_to_schemas(
                                    settings_manager::append_schema_suffix(
                                            settings_manager::project_definition_base_name)).c_str()));

    /* Maps an identifier to a pointer to a specification object */
    std::map < unsigned int, Specification* > identifier_resolution;
    bool                                      return_value = true;

    try {
      Specification dummy(false);

      /* Read root element (project) */
      reader.read();
 
      reader.read();

      /* Active node, must be either an optional project description or a specification element */
      if (reader.is_element("description")) {
        /* Proceed to content */
        reader.read();

        /* Read attribute value */
        reader.get_value(&project_description);

        /* To end tag */
        reader.read();

        reader.read();
      }

      while (!reader.is_end_element()) {
        /* Add new specification */
        specifications.push_back(dummy);

        /* Specification reads its content from reader */
        specifications.back().Read(reader);

        /* Update identifier resolution map */
        identifier_resolution[specifications.back().identifier] = &specifications.back();

        reader.read();
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

    if (return_value) {
      /* Input completely parsed, replace identifiers by pointers to specifications */
      const std::list < Specification >::iterator b = specifications.end();
            std::list < Specification >::iterator i = specifications.begin();

      while (i != b) {
        const std::vector < SpecificationInputType >::iterator c = (*i).GetInputObjects().end();
              std::vector < SpecificationInputType >::iterator j = (*i).GetInputObjects().begin();

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
    std::ofstream project_stream(settings_manager::path_concatenate(project_root, settings_manager::project_definition_base_name).c_str(),
                    std::ios::out | std::ios::trunc);
 
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
      if (bf::exists(bf::path((*k).location))) {
        bf::remove(bf::path((*k).location));
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
          if (bf::exists(bf::path((*k).location))) {
            bf::remove(bf::path((*k).location));
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
}

