#ifndef TOOL_H
#define TOOL_H

#include <string>
#include <iostream>

#include <sip/tool/capabilities.h>

#include <utility/visitor.h>

#include "exception.h"
#include "build_system.h"

namespace squadt {

  class extractor;

  /**
   * \brief Container for information about a single tool
   **/
  class tool : public utility::visitable< tool > {
    friend class extractor;
    friend class tool_manager_impl;
    friend class preferences_read_visitor_impl;

    public:

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < tool >                 sptr;

      /** \brief Convenience type alias */
      typedef sip::tool::capabilities::input_combination input_combination;

    private:

      /** \brief A name for the tool */
      std::string                                name;

      /** \brief The location where the tool can be found */
      std::string                                location;

      /** \brief Stores the tool capabilities object obtained through protocol implementation */
      sip::tool::capabilities::sptr              capabilities;

      /** \brief The default capabilities descriptor; one that is empty */
      static const sip::tool::capabilities::sptr no_capabilities;

    private:

      /** \brief Constructor */
      inline tool(std::string, std::string, sip::tool::capabilities::sptr = tool::no_capabilities);

      /** \brief Copy constructor */
      inline tool(tool const&);

      /** \brief Default constructor */
      inline tool();

    public:

      /** \brief Set capabilities object for this tool */
      inline void set_capabilities(sip::tool::capabilities::sptr);

      /** \brief Get capabilities object for this tool */
      inline const sip::tool::capabilities::sptr get_capabilities() const;

      /** \brief Get the location to for this tool */
      inline std::string get_location() const;

      /** \brief Get the name of this tool */
      inline std::string get_name() const;

      /** \brief Find a specific input combination of this tool, if it exists */
      inline input_combination const* find_input_combination(build_system::tool_category const&, build_system::storage_format const&) const;
  };

  /**
   * \param[in] l a full path to the executable
   * \param[in] n a name for the tool
   * \param[in] c a tool::capabilities object for the tool
   **/
  inline tool::tool(std::string n, std::string l, sip::tool::capabilities::sptr c) : name(n), location(l), capabilities(c) {
  }

  inline tool::tool(tool const& t) : name(t.name), location(t.location), capabilities(t.capabilities) {
  }

  inline tool::tool() : capabilities(tool::no_capabilities) {
  }

  /**
   * \param[in] c a shared pointer to a capabilities object
   **/
  inline void tool::set_capabilities(sip::tool::capabilities::sptr c) {
    if (c.get() != 0) {
      capabilities = c;
    }
  }

  inline const sip::tool::capabilities::sptr tool::get_capabilities() const {
    return (capabilities);
  }

  inline std::string tool::get_location() const {
    return (location);
  }

  inline std::string tool::get_name() const {
    return (name);
  }

  /**
   * \param[in] f the storage format
   * \param[in] t the category in which the tool operates
   **/
  inline tool::input_combination const* tool::find_input_combination(build_system::tool_category const& t, build_system::storage_format const& f) const {
    return (capabilities->find_input_combination(f, t));
  }
}

#endif
