// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file build_system.cpp

#include "boost.hpp" // precompiled headers

#include <boost/version.hpp>
#include <boost/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include "build_system.hpp"

#include "visitors.hpp"
#include "settings_manager.hpp"
#include "type_registry.hpp"
#include "tool_manager.hpp"
#include "executor.hpp"
#include "tool.hpp"

inline boost::filesystem::path parent_path(boost::filesystem::path const& p) {
#if (103500 < BOOST_VERSION)
  return p.parent_path();
#else
  return p.branch_path();
#endif
}

/**
 * \namespace squadt
 *
 * The global namespace for all squadt components.
 **/
namespace squadt {
  void build_system::default_tool_collection(tool_manager& m) const {
    static char const* default_tools[] = { "chi2mcrl2", "diagraphica.app",
      "lps2lts", "lps2pbes", "lpsactionrename", "lpsbinary", "lpsconstelm",
      "lpsinfo",
      "lpsparelm", "lpsrewr", "lpssumelm", "lpssuminst", "lpsuntime",
      "ltsconvert", "ltsinfo", "ltsgraph.app", "ltsview.app",
      "mcrl22lps",
      "pbes2bool",
      "pnml2mcrl2", "xsim.app", 0 };

    using boost::filesystem::basename;
    using boost::filesystem::path;

    const path default_path(m_settings_manager->path_to_default_binaries());

    for (char const** t = default_tools; *t != 0; ++t) {
#if (BOOST_WINDOWS)
      path path_to_binary(std::string(basename(path(*t))).append(".exe"));

      path_to_binary = default_path / path_to_binary;
#elif defined(__APPLE__)
      path path_to_binary(*t);

      if (extension(path_to_binary).empty()) {
        path_to_binary = default_path / path_to_binary;
      }
      else {
        path_to_binary = parent_path(default_path) / path_to_binary;
      }
#else
      path path_to_binary(basename(*t));

      path_to_binary = default_path / path_to_binary;
#endif

      m.add_tool(basename(*t), path_to_binary);
    }
  }

  build_system global_build_system;

  build_system::build_system() {
  }

  build_system::build_system(std::auto_ptr < settings_manager > s,
                             std::auto_ptr < tool_manager > t,
                             std::auto_ptr < executor > e,
                             std::auto_ptr < type_registry > r) : m_settings_manager(s), m_tool_manager(t), m_executor(e), m_type_registry(r) {
  }

  void build_system::initialise(std::auto_ptr < settings_manager > s,
                                std::auto_ptr < tool_manager > t,
                                std::auto_ptr < executor > e,
                                std::auto_ptr < type_registry > r) {
    m_settings_manager = s;
    m_tool_manager     = t;
    m_executor         = e;
    m_type_registry    = r;

    restore();

    m_type_registry->rebuild_indices();
  }

  std::auto_ptr < build_system > build_system::create(std::auto_ptr < settings_manager > s,
                                                      std::auto_ptr < tool_manager > t,
                                                      std::auto_ptr < executor > e,
                                                      std::auto_ptr < type_registry > r) {

    std::auto_ptr < build_system > new_build_system(new build_system(s, t, e, r));

    return (new_build_system);
  }

  settings_manager const& build_system::get_settings_manager() const {
    return *m_settings_manager;
  }

  settings_manager& build_system::get_settings_manager() {
    return *m_settings_manager;
  }

  void build_system::set_settings_manager(std::auto_ptr < settings_manager > t) {
    m_settings_manager = t;
  }

  tool_manager const& build_system::get_tool_manager() const {
    return *m_tool_manager;
  }

  tool_manager& build_system::get_tool_manager() {
    return *m_tool_manager;
  }

  tool_manager_impl const* build_system::get_tool_manager_impl() const {
    return (m_tool_manager->impl.get());
  }

  tool_manager_impl* build_system::get_tool_manager_impl() {
    return (m_tool_manager->impl.get());
  }

  void build_system::set_tool_manager(std::auto_ptr< tool_manager > t) {
    m_tool_manager = t;
  }

  executor const& build_system::get_executor() const {
    return *m_executor;
  }

  executor& build_system::get_executor() {
    return *m_executor;
  }

  executor_impl const* build_system::get_executor_impl() const {
    return (m_executor->impl.get());
  }

  executor_impl* build_system::get_executor_impl() {
    return (m_executor->impl.get());
  }

  void build_system::set_executor(std::auto_ptr< executor > t) {
    m_executor = t;
  }

  type_registry const& build_system::get_type_registry() const {
    return *m_type_registry.get();
  }

  type_registry& build_system::get_type_registry() {
    return *m_type_registry.get();
  }

  void build_system::set_type_registry(std::auto_ptr< type_registry > t) {
    m_type_registry = t;
  }

  void build_system::shutdown() {
    store();

    m_tool_manager->shutdown();
  }

  void build_system::restore() {

    const boost::filesystem::path miscellaneous_file_name(
                m_settings_manager->path_to_user_settings("preferences"));
    const boost::filesystem::path tool_manager_file_name(
                m_settings_manager->path_to_user_settings(settings_manager::tool_catalog_base_name));

    if (!boost::filesystem::exists(tool_manager_file_name)) { // set default tool collection
      tool_manager dummy;

      default_tool_collection(dummy);

      visitors::store(dummy, tool_manager_file_name);
    }

    visitors::restore(*m_tool_manager, tool_manager_file_name);

    if (boost::filesystem::exists(miscellaneous_file_name)) {
      restore_visitor preferences(miscellaneous_file_name);

      preferences.restore(*m_executor);
      preferences.restore(*m_type_registry);
    }
  }

  void build_system::store() {

    const boost::filesystem::path miscellaneous_file_name(
                m_settings_manager->path_to_user_settings("preferences"));
    const boost::filesystem::path tool_manager_file_name(
                m_settings_manager->path_to_user_settings(settings_manager::tool_catalog_base_name));

    visitors::store(*m_tool_manager, tool_manager_file_name);

    store_visitor preferences(miscellaneous_file_name);

    preferences.store(*m_executor);
    preferences.store(*m_type_registry);
  }
}
