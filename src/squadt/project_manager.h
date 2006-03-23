#ifndef PROJECT_MANAGER_H_
#define PROJECT_MANAGER_H_

#include <list>

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
      std::string                  directory;

      /** \brief A description of the project */
      std::string                  description;
 
      /**
       * \brief The list of processors for this project
       *
       * \invariant A processor p that a processor q depends on occurs before q.
       **/
      std::list < processor::ptr > processors;
 
    public:
 
      /** \brief Constructor */
      inline project_manager(const std::string&, const std::string&);
 
      /** \brief Get a reference to the list of processors in this project */
      inline const processor_list& get_processors();

      /** \brief Get the description */
      inline const std::string& get_description();
 
      /** \brief Read project information from project_directory */
      static project_manager::ptr read(const std::string&);
 
      /** \brief Read configuration with an XML text reader */
      static project_manager::ptr read(xml2pp::text_reader&) throw ();
 
      /** \brief Writes project configuration to the project file */
      void write() const;

      /** \brief Writes project configuration to stream */
      void write(std::ostream& = std::cout) const;
 
      /** \brief Add a new processor to the project */
      inline processor::ptr add(tool&, processor::visualisation_handler = processor::dummy_visualiser);

      /** \brief Remove a processor and all processors that depend one one of its outputs */
      inline void remove(processor*);
 
      /** \brief Make all specifications in the project up to date */
      void update();
  };

  /**
   * @param l a path to the root of the project directory
   * @param d a description for the project
   **/
  inline project_manager::project_manager(const std::string& l, const std::string& d)
          : directory(l), description(d) {
  }

  inline const std::string& project_manager::get_description() {
    return (description);
  }

  inline const project_manager::processor_list& project_manager::get_processors() {
    return (processors);
  }

  inline processor::ptr project_manager::add(tool& t, processor::visualisation_handler h) {
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
