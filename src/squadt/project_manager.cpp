#include <algorithm>
#include <fstream>
#include <map>

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/filesystem/operations.hpp>

#include <xml2pp/detail/text_reader.tcc>

#include "project_manager.h"
#include "settings_manager.tcc"
#include "processor.h"
#include "ui_core.h"

namespace squadt {

  namespace bf = boost::filesystem;

  void project_manager::write() const {
    std::ofstream project_stream(settings_manager::path_concatenate(directory,
             settings_manager::project_definition_base_name).c_str(), std::ios::out | std::ios::trunc);
 
    write(project_stream);
 
    project_stream.close();
  }

  /**
   * @param s the stream the output is written to
   **/
  void project_manager::write(std::ostream& s) const {
    /* Write header */
    s << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
      << "<project xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
      << " xsi:noNamespaceSchemaLocation=\"project.xsd\" version=\"1.0\">\n";

    if (!description.empty()) {
      s << "<description>" << description << "</desription>";
    }
 
    std::for_each(processors.begin(), processors.end(),
                    boost::bind(&processor::write, _1, boost::ref(s)));
 
    s << "</project>\n";
  }

  project_manager::ptr project_manager::read(const std::string& l) {
    xml2pp::text_reader::file_name< std::string > f(settings_manager::path_concatenate(l, settings_manager::project_definition_base_name));

    if (!bf::exists(bf::path(f.get()))) {
      throw (exception(exception_identifier::failed_loading_object, "squadt project", f.get()));
    }

    xml2pp::text_reader reader(f);

    reader.set_schema(xml2pp::text_reader::file_name< std::string >(
                            global_settings_manager->path_to_schemas(
                                    settings_manager::append_schema_suffix(
                                            settings_manager::project_definition_base_name)).c_str()));

    project_manager::ptr p = read(reader);

    p->directory = l;

    return (p);
  }

  project_manager::ptr project_manager::read(xml2pp::text_reader& r) throw () {
    processor::id_conversion_map m;

    project_manager::ptr p(new project_manager("",""));

    /* Advance to project element */
    r.read();

    if (r.is_element("description") && !r.is_empty_element()) {
      r.read();

      r.get_value(&p->description);

      r.read(2);
    }

    /* Read processors */
    while (r.is_element("processor")) {
      r.read();

      r.skip_end_element("processor");
    }

    return (p);
  }

  void project_manager::update() {
    /* TODO implementation */
  }
}

