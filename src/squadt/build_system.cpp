#include "build_system.h"

#include "preferences_visitors.h"
#include "settings_manager.h"
#include "type_registry.h"
#include "tool_manager.h"
#include "executor.h"
#include "tool.h"

namespace squadt {

  const build_system::storage_format build_system::storage_format_unknown = "";

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

  settings_manager const* build_system::get_settings_manager() const {
    return (m_settings_manager.get());
  }

  settings_manager* build_system::get_settings_manager() {
    return (m_settings_manager.get());
  }

  void build_system::set_settings_manager(std::auto_ptr < settings_manager > t) {
    m_settings_manager = t;
  }

  tool_manager const* build_system::get_tool_manager() const {
    return (m_tool_manager.get());
  }

  tool_manager* build_system::get_tool_manager() {
    return (m_tool_manager.get());
  }

  void build_system::set_tool_manager(std::auto_ptr< tool_manager > t) {
    m_tool_manager = t;
  }

  executor const* build_system::get_executor() const {
    return (m_executor.get());
  }

  executor* build_system::get_executor() {
    return (m_executor.get());
  }

  void build_system::set_executor(std::auto_ptr< executor > t) {
    m_executor = t;
  }

  type_registry const* build_system::get_type_registry() const {
    return (m_type_registry.get());
  }

  type_registry* build_system::get_type_registry() {
    return (m_type_registry.get());
  }

  void build_system::set_type_registry(std::auto_ptr< type_registry > t) {
    m_type_registry = t;
  }

  void build_system::shutdown() {
    store();
  }

  void build_system::restore() {
    static boost::filesystem::path path_to_file(m_settings_manager->path_to_user_settings("preferences"));

    read_preferences_visitor::restore(*this, path_to_file);
  }

  void build_system::store() {
    static boost::filesystem::path path_to_file(m_settings_manager->path_to_user_settings("preferences"));

    write_preferences_visitor::store(*this, path_to_file);
  }
}
