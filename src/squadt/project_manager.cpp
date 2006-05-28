#include <algorithm>
#include <fstream>
#include <map>
#include <algorithm>

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/filesystem/operations.hpp>

#include <xml2pp/detail/text_reader.tcc>

#include "project_manager.h"
#include "settings_manager.tcc"
#include "processor.tcc"
#include "core.h"

namespace squadt {

  namespace bf = boost::filesystem;

  const std::string project_manager::maintain_old_name("");

  /**
   * @param l a path to the root of the project store
   *
   * \pre l should be a path to a directory
   * 
   * If the directory does not exist then it is created and an initial project
   * description file is written to it. If the directory exists but there is no
   * project description file in it, then such a file is created and all files
   * in the directory are imported into the project.
   **/
  project_manager::project_manager(const boost::filesystem::path& l) : directory(l) {
    using namespace boost;

    assert(!l.empty());

    if (filesystem::exists(l)) {
      assert(filesystem::is_directory(l));

      if (!filesystem::exists(l / filesystem::path(settings_manager::project_definition_base_name))) {
        import_directory(l);

        /* Create initial project description file */
        write();
      }
      else {
        read();
      }
    }
    else {
      filesystem::create_directories(l);

      /* Create initial project description file */
      write();
    }
  }

  /**
   * @param l the directory that is to be imported
   **/
  void project_manager::import_directory(const boost::filesystem::path& l) {
    assert(bf::exists(l) && bf::is_directory(l));

    bf::directory_iterator end;
    bf::directory_iterator i(l);

    while (i != end) {
      if (is_directory(*i) && !symbolic_link_exists(*i)) {
        /* Recursively import */
        import_directory(*i);
      }
      else {
        import_file(*i);
      }

      ++i;
    }
  }

  void project_manager::write() const {
    std::ofstream project_stream(settings_manager::path_concatenate(directory,
             settings_manager::project_definition_base_name).c_str(), std::ios::out | std::ios::trunc);
 
    write(project_stream);
 
    project_stream.close();
  }

  /**
   * @param[out] s the stream the output is written to
   **/
  void project_manager::write(std::ostream& s) const {
    /* Write header */
    s << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
      << "<project xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
      << " xsi:noNamespaceSchemaLocation=\"project.xsd\" version=\"1.0\">\n";

    if (!description.empty()) {
      s << "<description>" << description << "</description>\n";
    }
 
    std::for_each(processors.begin(), processors.end(),
                    boost::bind(&processor::write, _1, boost::ref(s)));
 
    s << "</project>\n";
  }

  /**
   * \pre directory/<|settings_manager::project_definition_base_name|> must exist
   **/
  void project_manager::read() {
    xml2pp::text_reader::file_name< std::string > f(settings_manager::path_concatenate(directory, settings_manager::project_definition_base_name));

    assert(bf::exists(f.get()) && !bf::is_directory(f.get()));

    try {
      xml2pp::text_reader reader(f);

      reader.set_schema(xml2pp::text_reader::file_name< std::string >(
                            global_settings_manager->path_to_schemas(
                                    settings_manager::append_schema_suffix(
                                            settings_manager::project_definition_base_name)).c_str()));

      read(reader);
    }
    catch (...) {
      throw;
    }
  }

  /**
   * @param[in] l a path to a project file
   **/
  project_manager::ptr project_manager::read(const std::string& l) {
    xml2pp::text_reader::file_name< std::string > f(settings_manager::path_concatenate(l, settings_manager::project_definition_base_name));

    if (!bf::exists(bf::path(f.get()))) {
      throw (exception::exception(exception::failed_loading_object, "SQuADT project", f.get()));
    }

    project_manager::ptr p(new project_manager());

    try {
      xml2pp::text_reader reader(f);

      reader.set_schema(xml2pp::text_reader::file_name< std::string >(
                              global_settings_manager->path_to_schemas(
                                      settings_manager::append_schema_suffix(
                                              settings_manager::project_definition_base_name)).c_str()));
     
      p->read(reader);

      p->directory = l;
    }
    catch (...) {
      throw;
    }

    return (p);
  }

  /**
   * @param[in] r an xml2pp text reader that has been constructed with a project file
   **/
  void project_manager::read(xml2pp::text_reader& r) {
    /* Advance beyond project element */
    r.read(1);

    if (r.is_element("description") && !r.is_empty_element()) {
      r.read();

      r.get_value(&description);

      r.read(2);
    }

    processor::id_conversion_map c;

    /* Read processors */
    while (r.is_element("processor")) {
      processors.push_back(processor::read(c, r));
    }

    sort_processors();
  }

  /**
   * Sort processors such that for all i < j:
   *
   *   f(processors[i]) < f(processors[j]), or
   *
   *   f(processors[i]) = f(processors[j]) implies
   *
   *     not exists l < k : processor[l] -> processors[j] and processor[l].number_of_inputs() = 0 and
   *                        processor[k] -> processors[i] and processor[k].number_of_inputs() = 0
   *
   *  where:
   *
   *    - processor[k] -> processor[i] is a dependency of processor i on k
   *
   *    - function f is defined as:
   *      - f(p) = 0, if p.number_of_inputs() == 0
   *      - f(p) = 1 + f(max i : f(p.inputs[i].generator))
   **/
  void project_manager::sort_processors() {
    unsigned short number = 0; /* The number of */

    std::map < processor*, unsigned short > weights;

    /* Compute weights */
    processor_list::const_iterator j = processors.begin(); /* Lower bound */

    for (processor_list::const_iterator i = j; i != processors.end(); ++i) {
      if ((*i)->number_of_inputs() == 0) {
        weights[(*i).get()] = ++number;
      }
    }

    while (j != processors.end()) {
      for (processor_list::const_iterator i = j; i != processors.end(); ++i) {
        if (weights.find((*i).get()) != weights.end()) {
          if (i == j) {
            ++j;
          }
        }
        else {
          processor::input_object_iterator k              = (*i)->get_input_iterator();
          unsigned int                     maximum_weight = 0;

          while (k.valid()) {
            processor* target = (*k)->generator;

            if (target != 0) {
              if (weights.find(target) == weights.end()) {
                break;
              }
              else {
                unsigned int current_weight = weights[target];
             
                if (maximum_weight < current_weight) {
                  maximum_weight = current_weight;
                }
              }
            }

            ++k;
          }

          if (!k.valid()) {
            weights[(*i).get()] = 1 + maximum_weight;
          }
        }
      }
    }

    /* Do the actual sorting */
    std::stable_sort(processors.begin(), processors.end(), boost::bind(std::less< unsigned short >(), 
                        boost::bind(&std::map < processor*, unsigned short >::operator[], weights,
                                boost::bind(&processor::ptr::get, _1)),
                        boost::bind(&std::map < processor*, unsigned short >::operator[], weights,
                                boost::bind(&processor::ptr::get, _2))));
  }

  void project_manager::update() {
    /* TODO implementation */
  }
}

