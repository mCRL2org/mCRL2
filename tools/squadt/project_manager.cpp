// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file project_manager.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#include <algorithm>
#include <fstream>
#include <map>
#include <stack>
#include <set>
#include <memory>

#include <boost/version.hpp>
#include <boost/bind.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp> // workaround for boost::thread that includes errno.h
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/function.hpp>

#include "build_system.hpp"
#include "project_manager.ipp"
#include "processor.ipp"
#include "settings_manager.hpp"
#include "type_registry.hpp"
#include "visitors.hpp"

inline std::string filename(boost::filesystem::path const& p) {
#if (103500 < BOOST_VERSION)
  return p.filename();
#else
  return p.leaf();
#endif
}
inline boost::filesystem::path parent_path(boost::filesystem::path const& p) {
#if (103500 < BOOST_VERSION)
  return p.parent_path();
#else
  return p.branch_path();
#endif
}

namespace squadt {
  /// \cond INTERNAL_DOCS

  namespace bf = boost::filesystem;

  project_manager_impl::project_manager_impl() : count(0), update_active(false) {
  }

  /**
   * \param l a path to the root of the project store
   * \param b whether or not to (re)create a project
   *
   * \pre l should be a path to a directory
   *
   * If the directory does not exist then it is created and an initial project
   * description file is written to it. If the directory exists but there is no
   * project description file in it, then such a file is created and all files
   * in the directory are imported into the project.
   **/
  void project_manager_impl::load(const boost::filesystem::path& l, bool b) {
    using namespace boost;
    using namespace boost::filesystem;

    if(l.empty()){
      throw std::runtime_error("Empty path");
    };

    if (filename(l) == settings_manager::project_definition_base_name) {
      store = parent_path(l);
    }
    else {
      store = (exists(l) && !is_directory(l)) ? parent_path(l) : l;
    }

    filesystem::path project_file = store / filesystem::path(settings_manager::project_definition_base_name);

    if (filesystem::exists(store) && filesystem::is_directory(store)) {
      if (!filesystem::exists(project_file)) {
        if (b) {
          import_directory(store);

          /* Create initial project description file */
          visitors::store(*this, project_file);
        }
        else {
          throw std::runtime_error("Unable to load project file `"+project_file.string()+"'.");
        }
      }
      else if (b) {
        /* Project description file is probably broken */
        filesystem::remove(project_file);

        import_directory(store);

        /* Create initial project description file */
        visitors::store(*this, project_file);
      }
      else {
        visitors::restore(*this, project_file);
      }
    }
    else if (b) {
      if (!exists(store.root_path())) { // root path need not exist on Windows
        throw std::runtime_error("Unable to create project store, " + store.root_path().string() + " does not exist.");
      }

      filesystem::create_directories(store);

      /* Create initial project description file */
      visitors::store(*this, project_file);
    }
    else {
      throw std::runtime_error("Project cannot be opened, directory `" + store.string() + "' does not exist.");
    }

    /* Compute reverse dependencies */
    for (processor_list::const_iterator i = processors.begin(); i != processors.end(); ++i) {
      boost::iterator_range< processor::input_object_iterator > input_range((*i)->get_input_iterators());

      BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& o, input_range) {
        boost::shared_ptr< processor > g(o->get_generator());

        if (g.get() != 0) {
          reverse_depends.insert(std::make_pair(g.get(), i->get()));
        }
      }
    }
  }

  /**
   * \param p a reference to the processor to add or update
   * \pre p->impl->manager.lock().get() == m_interface.lock().get()
   **/
  void project_manager_impl::commit(boost::shared_ptr< processor > p) {

    if(!(p->impl->manager.lock().get() == m_interface.lock().get())){
      throw std::runtime_error("Manager lock differs from interface lock");
    }

    boost::recursive_mutex::scoped_lock l(list_lock);

    if (std::find(processors.begin(), processors.end(), p) == processors.end()) {
      processors.push_back(p);

      update_dependencies(p);

      write();
    }
  }

  /**
   * \param p a reference to the processor to add
   * \pre p->impl->manager.lock().get() == m_interface.lock().get()
   **/
  bool project_manager_impl::update_dependencies(boost::shared_ptr < processor > p) {
    bool changes = false;

    if(!(p->impl->manager.lock().get() == m_interface.lock().get())){
      throw std::runtime_error("manager lock differs from interface lock");
    }

    // Collect reverse dependencies
    std::set < processor* > collected;

    boost::iterator_range< processor::input_object_iterator > input_range(p->get_input_iterators());

    BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& i, input_range) {
      boost::shared_ptr< processor > g(i->get_generator());

      if (g.get() != 0) {
        collected.insert(g.get());
      }
    }

    // Remove obsolete reverse dependencies
    dependency_map::iterator i = reverse_depends.begin();

    while (i != reverse_depends.end()) {
      if (i->second == p.get() && !collected.count(i->first)) {
        dependency_map::iterator j(i++);

        reverse_depends.erase(j);

        changes = true;
      }
      else {
        ++i;
      }
    }

    // Add new reverse dependencies
    for (std::set< processor* >::const_iterator i = collected.begin(); i != collected.end(); ++i) {
      dependency_map::value_type dependency(*i, p.get());

      if (std::find(reverse_depends.begin(), reverse_depends.end(), dependency) == reverse_depends.end()) {
        reverse_depends.insert(dependency);

        changes = true;
      }
    }

    return changes;
  }

  /**
   * \param l the directory that is to be imported
   **/
  void project_manager_impl::import_directory(const boost::filesystem::path& l) {

      
    if(!(bf::exists(l) && bf::is_directory(l))){
      throw std::runtime_error( "Directory does not exist" );
    }

    bf::directory_iterator end;

    for (bf::directory_iterator i(l); i != end; ++i) {
      if (!is_directory(*i) && !symbolic_link_exists(*i)) {
        if (filename(*i) != settings_manager::project_definition_base_name) {
          import_file(*i);
        }
      }
    }
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
  void project_manager_impl::sort_processors() {
    unsigned int number = 0; /* The number of inputs */

    struct compare_by_weight {
      static bool less(std::map< processor*, unsigned int >& weights, boost::shared_ptr< processor > const& a, boost::shared_ptr< processor > const& b) {
        return weights[a.get()] < weights[b.get()];
      }
    };

    std::map < processor*, unsigned int > weights;

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
          boost::iterator_range< processor::input_object_iterator > input_range((*i)->get_input_iterators());
          unsigned int maximum_weight  = 0;
          bool         all_have_weight = true;

          BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, input_range) {
            boost::shared_ptr< processor > target(object->get_generator());

            if (target.get() != 0) {
              if (weights.find(target.get()) == weights.end()) {
                all_have_weight = false;

                break;
              }
              else {
                unsigned int current_weight = weights[target.get()];

                if (maximum_weight < current_weight) {
                  maximum_weight = current_weight;
                }
              }
            }
          }

          if (all_have_weight) {
            weights[(*i).get()] = 1 + (std::max)(maximum_weight, number);
          }
        }
      }
    }

    /* Do the actual sorting */
    std::stable_sort(processors.begin(), processors.end(), boost::bind(&compare_by_weight::less, weights, _1, _2));
  }

  /**
   * \param[in] p the processor that selects the targets
   * \pre p->impl->manager.lock().get() == m_interface.lock().get()
   **/
  void project_manager_impl::demote_status(boost::shared_ptr < processor > p) {

    if(!(p->impl->manager.lock().get() == m_interface.lock().get())){
      throw std::runtime_error("Manager lock differs from interface lock");
    }

    if (0 < reverse_depends.count(p.get())) {
      std::stack < processor* > p_stack;

      // start with the outputs of p
      p_stack.push(p.get());

      while (0 < p_stack.size()) {
        std::pair < dependency_map::iterator, dependency_map::iterator > range = reverse_depends.equal_range(p_stack.top());

        p_stack.pop();

        BOOST_FOREACH(dependency_map::value_type i, range) {
          if (!i.second->is_active()) {
            if (i.second->demote_status()) {
              p_stack.push(i.second);
            }
          }
        }
      }
    }
  }

  /**
   * \param[in] p the processor that selects the targets
   * \param[in] b whether or not to force an update for all processors that depend on p
   * \pre p->impl->manager.lock().get() == m_interface.lock().get()
   **/
  void project_manager_impl::update_status(boost::shared_ptr< processor > p, bool u) {
    if(!(p->impl->manager.lock().get() == m_interface.lock().get())){
      throw std::runtime_error("Manager lock differs from interface lock");
    }

    if (0 < reverse_depends.count(p.get())) {

      std::stack < processor* > p_stack;

      if (u) {
        std::pair < dependency_map::iterator, dependency_map::iterator > range = reverse_depends.equal_range(p.get());

        BOOST_FOREACH(dependency_map::value_type i, range) {
          i.second->demote_status();

          p_stack.push(i.second);
        }
      }
      else {
        p_stack.push(p.get());
      }

      while (0 < p_stack.size()) {
        std::pair < dependency_map::iterator, dependency_map::iterator > range = reverse_depends.equal_range(p_stack.top());

        p_stack.pop();

        BOOST_FOREACH(dependency_map::value_type i, range) {
          if (i.second->check_status(true)) {
            p_stack.push(i.second);
          }
        }
      }
    }
  }

  /**
   * \param[in] p the processor that selects the targets
   * \pre p->impl->manager.lock().get() == m_interface.lock().get()
   **/
  std::auto_ptr < project_manager_impl::conflict_list > project_manager_impl::get_conflict_list(boost::shared_ptr< processor > p) const {
    if(!(p->impl->manager.lock().get() == m_interface.lock().get())){
      throw std::runtime_error("Manager lock differs from interface lock");
    }

    std::set < std::string > locations; // Names of files produced by p

    boost::iterator_range< processor::output_object_iterator > output_range(p->get_output_iterators());

    BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, output_range) {
      locations.insert(object->get_location().string());
    }

    boost::recursive_mutex::scoped_lock l(list_lock);

    std::auto_ptr < conflict_list > conflicts(new conflict_list);

    /* Check all processors for conflicts */
    for (processor_list::const_iterator i = processors.begin(); i != processors.end(); ++i) {
      if (*i != p) {
        boost::iterator_range< processor::output_object_iterator > output_range((*i)->get_output_iterators());

        BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, output_range) {
          if (locations.find(object->get_location().string()) != locations.end()) {
            /* Conflict */
            conflicts->push_back(object);
          }
        }
      }
    }

    return (conflicts);
  }

  /**
   * \param[in] p the processor that selects the targets
   * \pre p->impl->manager.lock().get() == m_interface.lock().get()
   **/
  void project_manager_impl::update(boost::shared_ptr< processor > p, boost::function < void (processor*) > h) {
    if(!(p->impl->manager.lock().get() == m_interface.lock().get())){
      throw std::runtime_error("Manager lock differs from interface lock");
    }

    /* Recursively update inputs */
    p->check_status(true);

    boost::iterator_range< processor::output_object_iterator > output_range(p->get_output_iterators());

    BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, output_range) {
      if (!object->is_up_to_date()) {
        h(p.get());

        p->update();

        break;
      }
    }
  }

  /**
   * \param[in] h a function that is called that is called just before a processor is updated
   **/
  void project_manager_impl::update(boost::function < void (processor*) > h) {
    static bool update_active = false;

    if (!update_active) {
      update_active = true;

      BOOST_FOREACH(processor_list::value_type i, processors) {
        if (i->get_tool().get() != 0 && reverse_depends.count(i.get()) == 0) {
          i->check_status(true);

          boost::iterator_range< processor::output_object_iterator > output_range(i->get_output_iterators());

          BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, output_range) {
            if (!object->is_up_to_date()) {
              h(i.get());

              i->update();

              break;
            }
          }
        }
      }

      update_active = false;
    }
  }

  /**
   * \param p shared pointer to the interface object associated to this object
   * \param s path that identifies the file that is to be imported
   * \param d new name for the file in the project
   *
   * Note that:
   *  - when d is empty, the original filename will be maintained
   *  - when the file is already in the project store it is not copied
   **/
  boost::shared_ptr< processor > project_manager_impl::import_file(const boost::filesystem::path& s, std::string const& d) {
    using namespace boost::filesystem;

    if(!(exists(s) && !is_directory(s))){
      throw std::runtime_error( "Directory does not exist" );
    }

    path           destination_path  = store / path(d.empty() ? filename(s) : d);
    boost::shared_ptr< processor > p = processor::create(m_interface.lock());

    if (s != destination_path && !exists(destination_path)) {
      copy_file(s, destination_path);
    }

    /* Add the file to the project */
    p->register_output("", global_build_system.get_type_registry().mime_type_from_name(filename(destination_path)),
                                filename(destination_path), processor::object_descriptor::original);

    processors.push_back(p);

    return (p);
  }

  /**
   * \param[in] s path (relative to the project store) to a file
   */
  bool project_manager_impl::file_exists(const boost::filesystem::path& s) {
    using namespace boost::filesystem;

    path full_path(store / s);

    if (exists(full_path)) {
      if (!is_regular(full_path)) {
        throw std::runtime_error("Conflict, non-file with name `" + s.string() + "' in directory " + store.string() + ".");
      }
    }
    if (s == settings_manager::project_definition_base_name) {
      throw std::runtime_error("Sorry, name `" + s.string() + "' is not allowed.");
    }

    for (processor_list::iterator i = processors.begin(); i != processors.end(); ++i) {
      boost::iterator_range< processor::output_object_iterator > output_range((*i)->get_output_iterators());

      BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, output_range) {
        boost::shared_ptr< processor > g(object->get_generator());

        if (object->get_location() == full_path) {
          return true;
        }
      }
    }

    return false;
  }

  /**
   * \param[in] s path to the file, relative to the project store
   **/
  boost::shared_ptr < processor::object_descriptor > project_manager_impl::search_object_descriptor(boost::filesystem::path const& s) {
    boost::filesystem::path full_path(store / s);

    for (processor_list::iterator i = processors.begin(); i != processors.end(); ++i) {
      boost::iterator_range< processor::output_object_iterator > output_range((*i)->get_output_iterators());

      BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, output_range) {
        boost::shared_ptr< processor > g(object->get_generator());

        if (boost::filesystem::equivalent(object->get_location(), full_path)) {
          return object;
        }
      }
    }

    return boost::shared_ptr < processor::object_descriptor >();
  }

  /**
   * \param[in] p pointer to the processor that is to be removed
   * \param[in] b whether or not to remove the associated files
   *
   * \attention also removes processors with inputs that are not longer available
   * \pre input should be sorted as can be obtained by doing sort_processors()
   * \pre p->impl->manager.lock().get() == m_interface.lock().get()
   **/
  void project_manager_impl::remove(boost::shared_ptr < processor > p, bool b) {
    if(!(p->impl->manager.lock().get() == m_interface.lock().get())){
      throw std::runtime_error("Manager lock differs from interface lock");
    }

    std::set < processor* > obsolete;

    obsolete.insert(p.get());

    if (0 < reverse_depends.count(p.get())) {

      std::stack < processor* > p_stack;

      p_stack.push(p.get());

      while (0 < p_stack.size()) {
        std::pair < dependency_map::iterator, dependency_map::iterator > range = reverse_depends.equal_range(p_stack.top());

        p_stack.pop();

        BOOST_FOREACH(dependency_map::value_type i, range) {
          obsolete.insert(i.second);

          p_stack.push(i.second);
        }
      }
    }

    boost::recursive_mutex::scoped_lock l(list_lock);

    /* Update reverse dependencies, remove files and processors */
    BOOST_FOREACH(processor* i, obsolete) {
      boost::iterator_range< processor::input_object_iterator > input_range(p->get_input_iterators());

      BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, input_range) {
        boost::shared_ptr< processor > g(object->get_generator());

        if (g.get() != 0) {
          std::pair < dependency_map::iterator, dependency_map::iterator > range(reverse_depends.equal_range(g.get()));

          while (range.first != range.second) {
            if ((*range.first).second == i) {
              dependency_map::iterator temporary = range.first++;

              reverse_depends.erase(temporary);
            }
            else {
              ++range.first;
            }
          }
        }
      }

      if (b) {
        i->flush_outputs();
      }
    }

    processor_list::iterator j = processors.begin();

    // Actually remove the processor
    while (j != processors.end()) {
      if (obsolete.find((*j).get()) != obsolete.end()) {
        // Deactivate events, before delete
        (*j)->shutdown();

        j = processors.erase(j);
      }
      else {
        ++j;
      }
    }

    write();
  }

  void project_manager_impl::clean_store(boost::shared_ptr< processor > p, bool b) {
    namespace bf = boost::filesystem;

    std::set < std::string > objects;

    for (processor_list::iterator i = processors.begin(); i != processors.end(); ++i) {
      boost::iterator_range< processor::output_object_iterator > output_range(p->get_output_iterators());

      BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, output_range) {
        objects.insert(filename(bf::path(object->get_location())));
      }
    }

    for (bf::directory_iterator i(store); i != bf::directory_iterator(); ++i) {
      std::string name(filename(*i));

      if (objects.find(name) == objects.end()) {
        if (bf::exists(name) && !bf::is_directory(name) && !bf::symbolic_link_exists(name)) {
          bf::remove(name);
        }
      }
    }

    update_status(p);
  }
  /// \endcond

  project_manager::project_manager() : impl(new project_manager_impl) {
  }

  void project_manager::load(const boost::filesystem::path& l, bool b) {
    impl->load(l, b);
  }

  boost::shared_ptr < project_manager > project_manager::create() {
    boost::shared_ptr < project_manager > p(new project_manager);

    p->impl->m_interface = p;

    return p;
  }

  /**
   * \param[in] l a path to the root of the project store
   * \param[in] b whether or not to create the project anew (ignore existing project file)
   **/
  boost::shared_ptr < project_manager > project_manager::create(const boost::filesystem::path& l, bool b) {
    boost::shared_ptr < project_manager > p(new project_manager);

    p->impl->m_interface = p;

    p->load(l, b);

    return (p);
  }

  std::string project_manager::get_name() const {
    return (filename(impl->store));
  }

  void project_manager_impl::write() const {
    boost::filesystem::path project_file(settings_manager::path_concatenate(store,
             settings_manager::project_definition_base_name));

    visitors::store(*this, project_file);
  }

  void project_manager_impl::shutdown() {
    BOOST_FOREACH(processor_list::value_type p, processors) {
      p->shutdown();
    }
  }

  void project_manager::store() const {
    impl->write();
  }

  /**
   * \param[in] s a path to a project file
   **/
/*  bool project_manager::is_project_store(const boost::filesystem::path& s) {
    bool return_value = false;

    bf::path path_to(settings_manager::path_concatenate(s, settings_manager::project_definition_base_name));

    if (bf::exists(path_to)) {
      try {
        boost::shared_ptr < project_manager > p(project_manager::create());

        visitors::restore(*p, path_to);

        return_value = true;
      }
      catch (...) {
      }
    }

    return (return_value);
  } */

  /**
   * \param d a description for this project
   **/
  void project_manager::set_description(const std::string& d) {
    impl->description = d;
  }

  project_manager::processor_count project_manager::get_unique_count() {
    return (++impl->count);
  }

  const std::string& project_manager::get_description() const {
    return (impl->description);
  }

  boost::filesystem::path project_manager::get_project_store() const {
    return (impl->store);
  }

  project_manager::processor_sequence project_manager::get_processor_sequence() const {
    return (processor_sequence(impl->processors.begin(), impl->processors.end()));
  }

  void project_manager::commit(boost::shared_ptr< processor > const& p) {
    return impl->commit(p);
  }

  boost::shared_ptr< processor > project_manager::construct() {
    return boost::shared_ptr< processor >(processor::create(impl->m_interface.lock()));

  }

  /**
   * \param[in] t the tool
   * \param[in] t the selected input configuration of a tool
   **/
  boost::shared_ptr< processor > project_manager::construct(boost::shared_ptr < const tool > t, boost::shared_ptr < const tool::input_configuration > ic) {
    return boost::shared_ptr< processor >(processor::create(impl->m_interface.lock(), t, ic));

  }

  std::auto_ptr < project_manager::conflict_list > project_manager::get_conflict_list(boost::shared_ptr< processor > const& p) const {
    return(impl->get_conflict_list(p));
  }

  void project_manager::demote_status(boost::shared_ptr< processor > const& p) {
    impl->demote_status(p);
  }

  void project_manager::update_status(boost::shared_ptr< processor > const& p, bool u) {
    impl->update_status(p, u);
  }

  void project_manager::update(boost::shared_ptr< processor > const& p, boost::function < void (processor*) > h) {
    impl->update(p, h);
  }

  void project_manager::update(boost::function < void (processor*) > h) {
    impl->update(h);
  }

  boost::shared_ptr< processor > project_manager::import_file(const boost::filesystem::path& s, const std::string& d) {
    return(impl->import_file(s, d));
  }

  void project_manager::import_directory(const boost::filesystem::path& l) {
    impl->import_directory(l);
  }

  void project_manager::remove(boost::shared_ptr< processor > const& p, bool b) {
    impl->remove(p, b);
  }

  void project_manager::clean_store(boost::shared_ptr< processor > const& p, bool b) {
    impl->clean_store(p, b);
  }

  /**
   * \param[in] s path (relative to the project store) to a file
   */
  bool project_manager::exists(const boost::filesystem::path& s) {
    return impl->file_exists(s);
  }
}

