#ifndef PROJECT_MANAGER_H_
#define PROJECT_MANAGER_H_

#include <list>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

#include "processor.tcc"

namespace squadt {

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
      typedef boost::shared_ptr < project_manager >  ptr;

      /** \brief Convenience type alias */
      typedef std::list < processor::ptr >           processor_list;

    private:

      /** \brief The location of the project directory */
      boost::filesystem::path      directory;

      /** \brief A description of the project */
      std::string                  description;
 
      /**
       * \brief The list of processors for this project
       *
       * \invariant A processor p that a processor q depends on occurs before q.
       **/
      std::list < processor::ptr > processors;
 
    private:

      /** \brief Constructor */
      inline project_manager(const boost::filesystem::path&);
 
      /** \brief Constructor for use by read() */
      inline project_manager();

    public:
 
      /** \brief Factory */
      inline static project_manager::ptr create(const boost::filesystem::path& l);

      /** \brief Get the name of the project */
      inline std::string get_name() const;

      /** \brief Get the path to the project directory */
      inline std::string get_project_directory() const;

      /** \brief Get a reference to the list of processors in this project */
      inline const processor_list& get_processors() const;

      /** \brief Get the description */
      inline const std::string& get_description() const;
 
      /** \brief Read project information from project_directory */
      static project_manager::ptr read(const std::string&);
 
      /** \brief Read configuration with an XML text reader */
      static project_manager::ptr read(xml2pp::text_reader&) throw ();
 
      /** \brief Writes project configuration to the project file */
      void write() const;

      /** \brief Writes project configuration to stream */
      void write(std::ostream& = std::cout) const;
 
      /** \brief Add a new processor to the project */
      inline processor::ptr add(tool&, processor::reporter::callback_handler = processor::reporter::dummy);

      /** \brief Remove a processor and all processors that depend one one of its outputs */
      inline void remove(processor*);
 
      /** \brief Make all specifications in the project up to date */
      void update();
  };

  /**
   * @param l a path to the root of the project directory
   **/
  inline project_manager::project_manager(const boost::filesystem::path& l) : directory(l) {
  }

  inline project_manager::project_manager() {
  }

  /**
   * @param l a path to the root of the project directory
   **/
  inline project_manager::ptr project_manager::create(const boost::filesystem::path& l) {
    return project_manager::ptr(new project_manager(l));
  }

  inline std::string project_manager::get_name() const {
    return (directory.leaf());
  }

  inline std::string project_manager::get_project_directory() const {
    return (directory.native_directory_string());
  }

  inline const project_manager::processor_list& project_manager::get_processors() const {
    return (processors);
  }

  inline processor::ptr project_manager::add(tool& t, processor::reporter::callback_handler h) {
    return (processor::ptr(new processor(t, h)));
  }

  /**
   * @param p pointer to the processor that is to be removed
   **/
  inline void project_manager::remove(processor* p) {
    processor_list::iterator i = processors.begin();

    while (i != processors.end()) {
      if ((*i).get() == p || !((*i)->consistent_inputs())) {
        i = processors.erase(i);
      }
      else {
        ++i;
      }
    }
  }
}
#endif
