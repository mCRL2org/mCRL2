#ifndef PROJECT_MANAGER_H_
#define PROJECT_MANAGER_H_

#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/noncopyable.hpp>

#include <iterator_wrapper/indirect_iterator.h>

#include "processor.tcc"

namespace squadt {

  using iterator_wrapper::constant_indirect_iterator;

  /**
   * \brief Basic component that stores and retrieves information about projects
   *
   * A project is a collection of processors, that describe how to make output
   * from input objects. The output of one processor can be served as input to
   * another processor which creates potentially complex dependencies among
   * processors. The project manager stores this information and facilitates
   * the running the tools behind the processors to obtain a consistent set of
   * outputs.
   *
   * \attention Processors may not depend on themselves.
   **/
  class project_manager : public boost::noncopyable {

    public:

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < project_manager >                    ptr;

      /** \brief Convenience type alias */
      typedef std::vector < processor::ptr >                           processor_list;

      /** \brief Iterator type for the processor list */
      typedef constant_indirect_iterator < processor_list, processor > processor_iterator;

      /** \brief Constant that is used to specify to keep the existing file name (see import_file method) */
      static const std::string                                         maintain_old_name;

    private:

      /** \brief The location of the project store */
      boost::filesystem::path      directory;

      /** \brief A description of the project */
      std::string                  description;
 
      /** \brief The list of processors for this project */
      processor_list               processors;
 
    private:

      /** \brief Constructor for use by read() */
      inline project_manager();

      /** \brief Constructor */
      project_manager(const boost::filesystem::path&);

      /** \brief Sorts the processor list */
      void sort_processors();

      /** \brief Read project information from project_directory */
      void read();
 
      /** \brief Read configuration with an XML text reader */
      void read(xml2pp::text_reader&);

    public:
 
      /** \brief Factory */
      inline static project_manager::ptr create(const boost::filesystem::path&);

      /** \brief Get the name of the project */
      inline std::string get_name() const;

      /** \brief Get the path to the project store */
      inline std::string get_project_directory() const;

      /** \brief Recursively add all files in a directory to the project */
      void import_directory(const boost::filesystem::path&);
 
      /** \brief Add a file to the project under a new name */
      inline processor::ptr import_file(const boost::filesystem::path&, const std::string& = maintain_old_name);

      /** \brief Get a reference to the list of processors in this project */
      inline processor_iterator get_processor_iterator() const;

      /** \brief Get the description */
      inline void set_description(const std::string&);

      /** \brief Get the description */
      inline const std::string& get_description() const;

      /** \brief Read project information from project_directory */
      static project_manager::ptr read(const std::string&);
 
      /** \brief Writes project configuration to the project file */
      void write() const;

      /** \brief Writes project configuration to stream */
      void write(std::ostream&) const;
 
      /** \brief Add a new processor to the project */
      inline void add(processor::ptr&);

      /** \brief Remove a processor and all processors that depend one one of its outputs */
      inline void remove(processor*);
 
      /** \brief Make all specifications in the project up to date */
      void update();
  };

  inline project_manager::project_manager() {
  }

  /**
   * @param l a path to the root of the project store
   **/
  inline project_manager::ptr project_manager::create(const boost::filesystem::path& l) {
    return project_manager::ptr(new project_manager(l));
  }

  inline std::string project_manager::get_name() const {
    return (directory.leaf());
  }

  /**
   * @param d a description for this project
   **/
  inline void project_manager::set_description(const std::string& d) {
    description = d;
  }

  inline const std::string& project_manager::get_description() const {
    return (description);
  }

  inline std::string project_manager::get_project_directory() const {
    return (directory.native_directory_string());
  }

  inline project_manager::processor_iterator project_manager::get_processor_iterator() const {
    return (processor_iterator(processors));
  }

  /**
   * @param p a reference to the processor to add
   **/
  inline void project_manager::add(processor::ptr& p) {
    processors.push_back(p);
  }

  /**
   * @param s path that identifies the file that is to be imported
   * @param d new name for the file in the project
   *
   * Note that:
   *  - when d is empty, the original filename will be maintained
   *  - when the file is already in the project directory it is not copied
   **/
  inline processor::ptr project_manager::import_file(const boost::filesystem::path& s, const std::string& d) {
    using namespace boost::filesystem;

    assert(exists(s) && !is_directory(s) && native(d));

    path       destination_path = directory / path(d.empty() ? s.leaf() : d);
    processor::ptr p(new processor);

    if (s != destination_path) {
      copy_file(s, destination_path);
    }

    /* Add the file to the project */
    storage_format f = storage_format_unknown;

    /* TODO more intelligent file format check */
    if (!extension(s).empty()) {
      f = extension(s);

      f.erase(f.begin());
    }

    p->append_output(f, s.string());

    processors.push_back(p);

    return (p);
  }

  /**
   * @param p pointer to the processor that is to be removed
   *
   * \attention all processors with inconsistent inputs are also removed
   **/
  inline void project_manager::remove(processor* p) {
    processor_list::iterator i = processors.begin();

    while (i != processors.end()) {
      if ((*i).get() == p || !((*i)->consistent_inputs())) {
        (*i)->flush_outputs();

        i = processors.erase(i);
      }
      else {
        ++i;
      }
    }
  }
}
#endif
