#ifndef PROJECT_MANAGER_TCC_
#define PROJECT_MANAGER_TCC_

#include "project_manager.h"

namespace squadt {

  /// \cond PRIVATE_PART

  class project_manager_impl : public utility::visitable< project_manager_impl > {
    friend class project_manager;
    friend class store_visitor_impl;
    friend class restore_visitor_impl;

    protected:

      /** \brief Convenience type alias */
      typedef project_manager::processor_list                          processor_list;

      /** \brief Convenience type alias for a list of conflicting objects */
      typedef project_manager::conflict_list                           conflict_list;

      /** \brief Iterator type for the processor list */
      typedef project_manager::processor_iterator                      processor_iterator;

      /** \brief Finite type for counting the number of added processors */
      typedef project_manager::processor_count                         processor_count;

      /** \brief Map type for keeping track of processor dependencies */
      typedef std::multimap < processor*, processor* >                 dependency_map;

    private:

      /** \brief The location of the project store */
      boost::filesystem::path             store;

      /** \brief A description of the project */
      std::string                         description;
 
      /** \brief The list of processors for this project */
      processor_list                      processors;

      /** \brief Pointer to the associated interface object */
      boost::weak_ptr < project_manager > m_interface;

      /** \brief Count of the number of processors added to the project */
      processor_count                     count;
 
      /** \brief Reverse dependency relation */
      dependency_map                      reverse_depends;

      /** \brief Used to guarantee atomicity of operations on the processor list */
      mutable boost::mutex                list_lock;

      /** \brief Whether or not a global update operation is in progress */
      bool                                update_active;

    private:

      /** \brief Default constructor */
      project_manager_impl();

      /** \brief Constructor */
      void load(const boost::filesystem::path&, bool);

      /** \brief Write to project file */
      void write() const;

      /** \brief Sorts the processor list */
      void sort_processors();

      /** \brief Add a new processor to the project, if it is not already */
      void add(processor::ptr const&);

      /** \brief Remove a processor and all processors that depend one one of its outputs */
      void remove(processor*, bool = true);

      /** \brief Recursively add all files in a directory to the project */
      void import_directory(const boost::filesystem::path&);
 
      /** \brief Add a file to the project under a new name */
      processor::ptr import_file(const boost::filesystem::path&, const std::string& = "");

      /** \brief Updates the status of all outputs that depend on the argument */
      void update_status(processor*);

      /** \brief Updates the status of all outputs that depend on the argument, sets it to out-of-date (unless it does not exist) */
      void demote_status(processor*);

      /** \brief Given a processor, it produces a list of object_descriptors that conflict with its outputs */
      std::auto_ptr < conflict_list > get_conflict_list(processor::sptr p) const;
 
      /** \brief Removes all files that cannot be recreated by any of the processors */
      void clean_store(processor* p, bool b);

      /** \brief Make objects in the project up to date */
      void update_single(processor::sptr);

      /** \brief Make objects in the project up to date */
      void update(boost::function< void (processor*) >);

      /** \brief Shutdown all running processors */
      void shutdown();
  };

  /// \endcond
}

#endif

