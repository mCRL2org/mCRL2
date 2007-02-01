#include <algorithm>
#include <fstream>
#include <map>
#include <stack>
#include <set>
#include <memory>

#include <boost/bind.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/function.hpp>

#include "build_system.h"
#include "project_manager.tcc"
#include "settings_manager.h"
#include "visitors.h"

namespace squadt {
  /// \cond PRIVATE_PART

  namespace bf = boost::filesystem;

  project_manager_impl::project_manager_impl() : count(0), update_active(false) {
  }

  /**
   * \param l a path to the root of the project store
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

    assert(!l.empty());

    store = l;

    filesystem::path project_file = l / filesystem::path(settings_manager::project_definition_base_name);

    if (filesystem::exists(l)) {
      assert(filesystem::is_directory(l));

      if (!filesystem::exists(project_file)) {
        import_directory(l);

        /* Create initial project description file */
        visitors::store(*this, project_file);
      }
      else {
        if (b) {
          filesystem::remove(project_file);
        }
        else {
          try {
            visitors::restore(*this, project_file);
          }
          catch (...) {
            b = true;
          }
        }

        if (b) {
          /* Project description file is probably broken */
          import_directory(l);

          /* Create initial project description file */
          visitors::store(*this, project_file);
        }
      }
    }
    else {
      filesystem::create_directories(l);

      /* Create initial project description file */
      visitors::store(*this, project_file);
    }

    /* Compute reverse dependencies */
    for (processor_list::const_iterator i = processors.begin(); i != processors.end(); ++i) {
      for (processor::input_object_iterator j = (*i)->get_input_iterator(); j.valid(); ++j) {
        processor::sptr g((*j)->generator.lock());

        if (g.get() != 0) {
          reverse_depends.insert(std::make_pair(g.get(), (*i).get()));
        }
      }
    }
  }

  /**
   * \param p a reference to the processor to add
   **/
  void project_manager_impl::add(processor::ptr const& p) {
    boost::mutex::scoped_lock l(list_lock);

    if (processors.end() == std::find(processors.begin(), processors.end(), p)) {
      processors.push_back(p);

      /* Register reverse dependencies */
      for (processor::input_object_iterator i = p->get_input_iterator(); i.valid(); ++i) {
        processor::sptr g((*i)->generator.lock());

        if (g.get() != 0) {
          reverse_depends.insert(std::make_pair(g.get(), p.get()));
        }
      }

      write();
    }
  }

  /**
   * \param l the directory that is to be imported
   **/
  void project_manager_impl::import_directory(const boost::filesystem::path& l) {
    assert(bf::exists(l) && bf::is_directory(l));

    bf::directory_iterator end;
    bf::directory_iterator i(l);

    while (i != end) {
      if (is_directory(*i) && !symbolic_link_exists(*i)) {
        /* Recursively import */
        import_directory(*i);
      }
      else {
        if ((*i).leaf() != settings_manager::project_definition_base_name) {
          import_file(*i);
        }
      }

      ++i;
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
    unsigned int number = 0; /* The number of */

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
            processor::sptr target((*k)->generator);

            if (target.get() != 0) {
              if (weights.find(target.get()) == weights.end()) {
                break;
              }
              else {
                unsigned int current_weight = weights[target.get()];
             
                if (maximum_weight < current_weight) {
                  maximum_weight = current_weight;
                }
              }
            }

            ++k;
          }

          if (!k.valid()) {
            weights[(*i).get()] = 1 + std::max(maximum_weight, number);
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

  /**
   * \param[in] p the processor that selects the targets
   **/
  void project_manager_impl::demote_status(processor* p) {
    if (0 < reverse_depends.count(p)) {
      std::stack < processor* > p_stack;

      // start with the outputs of p
      p_stack.push(p);

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
   **/
  void project_manager_impl::update_status(processor* p) {
    if (0 < reverse_depends.count(p)) {

      std::stack < processor* > p_stack;

      p_stack.push(p);

      while (0 < p_stack.size()) {
        std::pair < dependency_map::iterator, dependency_map::iterator > range = reverse_depends.equal_range(p_stack.top());

        p_stack.pop();

        BOOST_FOREACH(dependency_map::value_type i, range) {
          if (i.second->check_status(false)) {
            p_stack.push(i.second);
          }
        }
      }
    }
  }

  std::auto_ptr < project_manager_impl::conflict_list > project_manager_impl::get_conflict_list(processor::sptr p) const {
    std::set < std::string > locations; // Names of files produced by p

    for (processor::output_object_iterator j = p->get_output_iterator(); j.valid(); ++j) {
      locations.insert((*j)->location);
    }

    boost::mutex::scoped_lock l(list_lock);

    std::auto_ptr < conflict_list > conflicts(new conflict_list);

    /* Check all processors for conflicts */
    for (processor_list::const_iterator i = processors.begin(); i != processors.end(); ++i) {
      if (*i != p) {
        for (processor::output_object_iterator j = (*i)->get_output_iterator(); j.valid(); ++j) {
          if (locations.find((*j)->location) != locations.end()) {
            /* Conflict */
            conflicts->push_back(j.pointer());
          }
        }
      }
    }

    return (conflicts);
  }

  /**
   * \param[in] p the processor that selects the targets
   **/
  void project_manager_impl::update_single(processor::sptr p) {

    /* Recursively update inputs */
    for (processor::input_object_iterator j = p->get_input_iterator(); j.valid(); ++j) {
      processor::sptr dependency((*j)->generator.lock());

      if (dependency.get() != 0) {
        update_single(dependency);
      }
    }

    if (p->check_status(true)) {
      p->update(boost::bind(&project_manager_impl::update_single, this, p));
    }
  }

  /**
   * \param[in] h a function that is called that is called just before a processor is updated
   **/
  void project_manager_impl::update(boost::function < void (processor*) > h) {
    bool update_active = false;

    if (!update_active) {
      update_active = true;

      BOOST_FOREACH(processor_list::value_type i, processors) {
        if (i->get_tool()) {
          h(i.get());

          update_single(i);
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
  processor::ptr project_manager_impl::import_file(const boost::filesystem::path& s, std::string const& d) {
    using namespace boost::filesystem;

    assert(exists(s) && !is_directory(s) && native(d));

    path           destination_path = store / path(d.empty() ? s.leaf() : d);
    processor::ptr p                = processor::create(m_interface.lock());

    if (s != destination_path && !exists(destination_path)) {
      copy_file(s, destination_path);
    }

    build_system::mime_type type(extension(s).empty() ? "unknown" : extension(s).substr(1));

    /* Add the file to the project */
    p->append_output(type, "", destination_path.leaf(), processor::object_descriptor::original);

    processors.push_back(p);

    return (p);
  }

  /**
   * \param[in] p pointer to the processor that is to be removed
   * \param[in] b whether or not to remove the associated files
   *
   * \attention also removes processors with inputs that are not longer available
   * \pre input should be sorted as can be obtained by doing sort_processors()
   **/
  void project_manager_impl::remove(processor* p, bool b) {
    std::set < processor* > obsolete;

    obsolete.insert(p);

    if (0 < reverse_depends.count(p)) {

      std::stack < processor* > p_stack;

      p_stack.push(p);

      while (0 < p_stack.size()) {
        std::pair < dependency_map::iterator, dependency_map::iterator > range = reverse_depends.equal_range(p_stack.top());

        p_stack.pop();

        BOOST_FOREACH(dependency_map::value_type i, range) {
          obsolete.insert(i.second);

          p_stack.push(i.second);
        }
      }
    }

    boost::mutex::scoped_lock l(list_lock);

    /* Update reverse dependencies, remove files and processors */
    BOOST_FOREACH(processor* i, obsolete) {
      for (processor::input_object_iterator j = i->get_input_iterator(); j.valid(); ++j) {
        processor::sptr g((*j)->generator.lock());

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

    /** Actually remove the processor */
    while (j != processors.end()) {
      if (obsolete.find((*j).get()) != obsolete.end()) {
        j = processors.erase(j);
      }
      else {
        ++j;
      }
    }

    write();
  }

  void project_manager_impl::clean_store(processor* p, bool b) {
    namespace bf = boost::filesystem;

    std::set < std::string > objects;

    for (processor_list::iterator i = processors.begin(); i != processors.end(); ++i) {
      for (processor::output_object_iterator j = (*i)->get_output_iterator(); j.valid(); ++j) {
        objects.insert(bf::path((*j)->location).leaf());
      }
    }

    for (bf::directory_iterator i(store); i != bf::directory_iterator(); ++i) {
      if (objects.find((*i).leaf()) == objects.end()) {
        if (bf::exists((*i).leaf()) && !bf::is_directory((*i).leaf()) && !bf::symbolic_link_exists((*i).leaf())) {
          bf::remove((*i).leaf());
        }
      }
    }

    update_status(p);
  }

  project_manager::project_manager() : impl(new project_manager_impl) {
  }

  void project_manager::load(const boost::filesystem::path& l, bool b) {
    impl->load(l, b);
  }

  /**
   * \param[in] l a path to the root of the project store
   * \param[in] b whether or not to create the project anew (ignore existing project file)
   **/
  project_manager::ptr project_manager::create(const boost::filesystem::path& l, bool b) {
    project_manager::ptr p(new project_manager);

    p->impl->m_interface = p;

    p->load(l, b);

    return (p);
  }

  std::string project_manager::get_name() const {
    return (impl->store.leaf());
  }

  /**
   * \param n the name of the file
   **/
  boost::filesystem::path project_manager::get_path_for_name(const std::string& n) const {
    assert(boost::filesystem::native(n));

    return (impl->store / n);
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
  bool project_manager::is_project_store(const std::string& s) {
    bool return_value = true;

    bf::path path_to(settings_manager::path_concatenate(s, settings_manager::project_definition_base_name));

    if (bf::exists(path_to)) {
      try {
        project_manager p;

        visitors::restore(p, path_to);
      }
      catch (...) {
        return_value = false;
      }
    }

    return (return_value);
  }

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

  std::string project_manager::get_project_store() const {
    return (impl->store.native_directory_string());
  }

  project_manager::processor_iterator project_manager::get_processor_iterator() const {
    return (processor_iterator(impl->processors));
  }

  void project_manager::add(processor::ptr const& p) {
    impl->add(p);
  }

  std::auto_ptr < project_manager::conflict_list > project_manager::get_conflict_list(processor::sptr p) const {
    return(impl->get_conflict_list(p));
  }

  void project_manager::demote_status(processor* p) {
    impl->demote_status(p);
  }

  void project_manager::update_status(processor* p) {
    impl->update_status(p);
  }

  void project_manager::update_single(processor::sptr p) {
    impl->update_single(p);
  }

  void project_manager::update(boost::function < void (processor*) > h) {
    impl->update(h);
  }

  processor::ptr project_manager::import_file(const boost::filesystem::path& s, const std::string& d) {
    return(impl->import_file(s, d));
  }

  void project_manager::import_directory(const boost::filesystem::path& l) {
    impl->import_directory(l);
  }

  void project_manager::remove(processor* p, bool b) {
    impl->remove(p, b);
  }

  void project_manager::clean_store(processor* p, bool b) {
    impl->clean_store(p, b);
  }

  /// \endcond
}

