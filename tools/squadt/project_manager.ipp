// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file project_manager.ipp
/// \brief Add your file description here.

#ifndef PROJECT_MANAGER_TCC_
#define PROJECT_MANAGER_TCC_

#include <boost/thread/recursive_mutex.hpp>

#include "project_manager.hpp"

namespace squadt {

  /// \cond INTERNAL_DOCS

  class project_manager_impl : public utility::visitable {
    friend class project_manager;
    friend class processor_impl;

    template < typename R, typename S >
    friend class utility::visitor;

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
      mutable boost::recursive_mutex      list_lock;

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

      /** \brief Add a new processor to the project */
      processor* add();

      /** \brief Check for conflicts and add to project */
      void commit(boost::shared_ptr< processor >);

      /** \brief Remove a processor and all processors that depend one one of its outputs */
      void remove(boost::shared_ptr< processor >, bool = true);

      /** \brief Recursively add all files in a directory to the project */
      void import_directory(const boost::filesystem::path&);
 
      /** \brief Add a file to the project under a new name */
      boost::shared_ptr < processor > import_file(const boost::filesystem::path&, const std::string& = "");

      /** \brief Updates the status of all outputs that depend on the argument */
      void update_status(boost::shared_ptr< processor >, bool = false);

      /** \brief Updates the status of all outputs that depend on the argument, sets it to out-of-date (unless it does not exist) */
      void demote_status(boost::shared_ptr< processor >);

      /** \brief Given a processor, it produces a list of object_descriptors that conflict with its outputs */
      std::auto_ptr < conflict_list > get_conflict_list(boost::shared_ptr< processor > p) const;
 
      /** \brief Removes all files that cannot be recreated by any of the processors */
      void clean_store(boost::shared_ptr< processor > p, bool b);

      /** \brief Make objects in the project up to date */
      void update(boost::shared_ptr< processor >, boost::function< void (processor*) >);

      /** \brief Make objects in the project up to date */
      void update(boost::function< void (processor*) >);

      /** \brief Updates dependencies for a processor that is part of the project */
      bool update_dependencies(boost::shared_ptr< processor >);

      /** \brief Shutdown all running processors */
      void shutdown();
  };

  /// \endcond
}

#endif

