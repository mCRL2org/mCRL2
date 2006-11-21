#ifndef BUILD_SYSTEM_H__
#define BUILD_SYSTEM_H__

#include <memory>

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <sip/tool_capabilities.h>

namespace squadt {
  class settings_manager;
  class tool_manager;
  class type_registry;
  class tool;

  namespace execution {
    class executor;
  }

  using execution::executor;

  class build_system : private boost::noncopyable {
    friend class read_preferences_visitor_impl;
    friend class write_preferences_visitor_impl;

    private:

      std::auto_ptr < settings_manager > m_settings_manager;

      std::auto_ptr < tool_manager >     m_tool_manager;

      std::auto_ptr < executor >         m_executor;

      std::auto_ptr < type_registry >    m_type_registry;

    private:

      /** \brief Constructor with complete initialisation */
      build_system(std::auto_ptr < settings_manager >,
                   std::auto_ptr < tool_manager >,
                   std::auto_ptr < executor >,
                   std::auto_ptr < type_registry >);

    public:

      /** \brief Convenience type alias */
      typedef sip::tool::capabilities::storage_format storage_format;

      /** \brief Convenience type alias */
      typedef sip::tool::capabilities::tool_category  tool_category;

    public:

      /** \brief Special storage format constant for when the type is unknown */
      static const storage_format storage_format_unknown;

    public:

      /** \brief Default constructor */
      build_system();

      /** \brief Constructor with complete initialisation */
      void initialise(std::auto_ptr < settings_manager >,
                      std::auto_ptr < tool_manager >,
                      std::auto_ptr < executor >,
                      std::auto_ptr < type_registry >);

      /** \brief Factory function for object creation */
      static std::auto_ptr < build_system > create(std::auto_ptr < settings_manager > s,
                                                      std::auto_ptr < tool_manager > t,
                                                      std::auto_ptr < executor > e,
                                                      std::auto_ptr < type_registry > r);


      /** \brief Gets settings manager object */
      settings_manager const* get_settings_manager() const;

      /** \brief Gets settings manager object */
      settings_manager* get_settings_manager();

      /** \brief Sets settings manager object */
      void set_settings_manager(std::auto_ptr < settings_manager >);

      /** \brief Gets tool manager object */
      tool_manager const* get_tool_manager() const;

      /** \brief Gets tool manager object */
      tool_manager* get_tool_manager();

      /** \brief Sets tool manager object */
      void set_tool_manager(std::auto_ptr< tool_manager >);

      /** \brief Gets tool manager object */
      executor const* get_executor() const;

      /** \brief Gets tool manager object */
      executor* get_executor();

      /** \brief Sets tool manager object */
      void set_executor(std::auto_ptr< executor >);

      /** \brief Gets type registry object */
      type_registry const* get_type_registry() const;

      /** \brief Gets type registry object */
      type_registry* get_type_registry();

      /** \brief Sets type registry object */
      void set_type_registry(std::auto_ptr< type_registry >);

      /** \brief Restores the state of this object from file (and those contained) */
      void restore();

      /** \brief Stores the state of this object (and those contained) */
      void store();

      /** \brief Stores user preferences and terminates running tools */
      void shutdown();
  };

  extern build_system global_build_system;
}

#endif

