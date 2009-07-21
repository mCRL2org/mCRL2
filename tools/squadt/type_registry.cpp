// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file type_registry.cpp
/// \brief Add your file description here.

#include "wx.hpp" // precompiled headers

#include "type_registry.hpp"

#include <functional>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/xpressive/xpressive_static.hpp>
#include <boost/bind.hpp>

#include "tipi/mime_type.hpp"

#ifdef USING_WX
#include <wx/wx.h>
#include <wx/mimetype.h>
#endif

#include "tool_manager.hpp"
#include "command.hpp"
#include "tool.hpp"

namespace squadt {

  /**
   * \internal
   **/
  class system_mime_type_manager {

    public:

      /* \brief Guesses the mime type from a filename */
      tipi::mime_type guess_mime_type(boost::filesystem::path const&) const;

      /* \brief Gets the system command associated with a type, if any */
      std::string get_command(tipi::mime_type const&) const;

      /* \brief Returns whether the type is known to the system */
      bool is_known_type(tipi::mime_type const&) const;
  };

#if defined(USING_WX)
  tipi::mime_type system_mime_type_manager::guess_mime_type(boost::filesystem::path const& name) const {
    static wxMimeTypesManager m;

    std::string extension = boost::filesystem::extension(name);

    if (!extension.empty()) {
      std::auto_ptr < wxFileType > wtype(m.GetFileTypeFromExtension(wxString(extension.substr(1).c_str(), wxConvLocal)));

      if (wtype.get()) {
        wxString mime_type;

        wtype->GetMimeType(&mime_type);

        return tipi::mime_type(std::string(mime_type.fn_str()));
      }
    }

    return tipi::mime_type("unknown");
  }

  std::string system_mime_type_manager::get_command(tipi::mime_type const& type) const {
    static wxMimeTypesManager m;

    std::auto_ptr< wxFileType > wtype(m.GetFileTypeFromMimeType(wxString(type.string().c_str(), wxConvLocal)));

    if (wtype.get()) {
      return std::string(wtype->GetOpenCommand(wxT("$")).fn_str());
    }
    else if (type.category() == tipi::mime_type::text) {
#if defined(__APPLE__)
      return "open -t $"; // .GetFileTypeFromMimeType("text/plain") does not work on OS X
#else
      wtype.reset(m.GetFileTypeFromMimeType(wxT("text/plain")));

      if (wtype.get()) {
        return std::string(wtype->GetOpenCommand(wxT("$")).fn_str());
      }
#endif
    }

    return std::string();
  }

  bool system_mime_type_manager::is_known_type(tipi::mime_type const& type) const {
    return !get_command(type).empty();
  }
#else
  inline tipi::mime_type system_mime_type_manager::guess_mime_type(boost::filesystem::path const&) const {
    return tipi::mime_type("unknown");
  }

  inline std::string system_mime_type_manager::get_command(tipi::mime_type const&) const {
    return std::string();
  }

  inline bool system_mime_type_manager::is_known_type(tipi::mime_type const&) const {
    return false;
  }
#endif

  static system_mime_type_manager global_mime_types_manager;

  const std::string type_registry::command_system = "system defined";
  const std::string type_registry::command_none;

  /**
   * Contacts the local tool manager to ask for the current list of tools,
   * and stores this information in the tool information cache (tool_information_cache).
   *
   * \attention Not thread safe
   **/
  void type_registry::build_index() {
    using tipi::tool::capabilities;

    tool_manager::const_tool_sequence tools = global_build_system.get_tool_manager().get_tools();

    /* Make sure the map is empty */
    categories_for_format.clear();

    for (tool_manager::const_tool_sequence::const_iterator t = tools.begin(); t != tools.end(); ++t) {
      if ((*t)->is_usable()) {
        capabilities::input_configuration_range inputs((*t)->get_capabilities()->get_input_configurations());

        BOOST_FOREACH(capabilities::input_configuration_range::value_type j, inputs) {
          capabilities::input_configuration::object_sequence input_range(j->object_range());

          BOOST_FOREACH(capabilities::input_configuration::object_sequence::value_type input_descriptor, input_range) {
            tipi::mime_type& format(input_descriptor.second);

            if (categories_for_format.find(format) == categories_for_format.end()) {
              /* Format unknown, create new map */
              tools_for_category temporary;

              categories_for_format.insert(std::make_pair(format, temporary));

              /* Make sure the type is registered */
              if (!has_registered_command(format, true)) {
                register_command(format, command_none);
              }
            }

            categories_for_format[format].insert(std::make_pair(j->get_category(), *t));
          }
        }
      }
    }
  }

  /**
   * @param f the format for which to execute the action a
   **/
  type_registry::tool_sequence type_registry::tools_by_mime_type(tipi::mime_type const& f) const {

    type_registry::tool_sequence range;

    categories_for_mime_type::const_iterator i(categories_for_format.find(f));

    if (i != categories_for_format.end()) {
      tools_for_category const& p((*i).second);

      range = boost::make_iterator_range(p.begin(), p.end());
    }
    else {
      i = categories_for_format.find(tipi::mime_type(f.sub_type(), tipi::mime_type::text));

      if (i != categories_for_format.end()) { // for unknown main type
        tools_for_category const& p((*i).second);

        range = boost::make_iterator_range(p.begin(), p.end());
      }
      else {
        i = categories_for_format.find(tipi::mime_type(f.sub_type(), tipi::mime_type::application));

        if (i != categories_for_format.end()) { // for unknown main type
          tools_for_category const& p((*i).second);

          range = boost::make_iterator_range(p.begin(), p.end());
        }
        else { // return an empty sequence
          type_registry::tool_sequence::iterator i;

          range = boost::make_iterator_range(i, i);
        }
      }
    }

    return (range);
  }

  /**
   * @param f the format for which to execute the action a
   **/
  std::set < type_registry::tool_category > type_registry::categories_by_mime_type(build_system::mime_type const& f) const {
    std::set < tool_category > categories;

    categories_for_mime_type::const_iterator i = categories_for_format.find(f);

    if (i != categories_for_format.end()) {
      BOOST_FOREACH(tools_for_category::value_type j, (*i).second) {
        categories.insert(j.first);
      }
    }

    return (categories);
  }

  std::set < build_system::tool_category > type_registry::get_categories() const {
    std::set < tool_category > categories;

    BOOST_FOREACH(categories_for_mime_type::value_type i, categories_for_format) {
      BOOST_FOREACH(tools_for_category::value_type j, i.second) {
        categories.insert(j.first);
      }
    }

    return (categories);
  }

  std::set < build_system::storage_format > type_registry::get_mime_types() const {
    using tipi::tool::capabilities;

    std::set < build_system::storage_format > formats;

    BOOST_FOREACH(categories_for_mime_type::value_type c, categories_for_format) {
      formats.insert(c.first);
    }

    for (actions_for_type::const_iterator i = command_for_type.begin(); i != command_for_type.end(); ++i) {
      if (i->second != command_none) {
        formats.insert((*i).first);
      }
    }

    tool_manager::const_tool_sequence tools = global_build_system.get_tool_manager().get_tools();

    BOOST_FOREACH(boost::shared_ptr< tool > t, tools) {
      tipi::tool::capabilities::output_configuration_range outputs(t->get_capabilities()->get_output_configurations());

      BOOST_FOREACH(tipi::tool::capabilities::output_configuration_range::value_type i, outputs) {
        formats.insert(i->get_format());
      }
    }

    return (formats);
  }

  /**
   * \brief Whether or not a command is associated with this type
   * \param[in] p path to a file
   * \param[in] c whether or not to consult the system mime-database
   **/
  tipi::mime_type type_registry::mime_type_from_name(boost::filesystem::path const& p, const bool c) const {
    std::string extension(boost::filesystem::extension(p));

    extension = ((extension.size() <= 1) ? "unknown" : extension.substr(1));

    BOOST_FOREACH(categories_for_mime_type::value_type c, categories_for_format) {
      if (c.first.sub_type() == extension) {
        return c.first;
      }
    }

    return mime_type(extension);
  }

  /**
   * \brief Whether or not a command is associated with this type
   * \param[in] t mime type for which to search
   * \param[in] c whether or not to consult the system mime-database
   **/
  bool type_registry::has_registered_command(mime_type const& t, const bool c) const {
    actions_for_type::const_iterator i = command_for_type.find(t);

    if (i == command_for_type.end()) {
      i = std::find_if(command_for_type.begin(), command_for_type.end(),
		  boost::bind(&mime_type::operator==, mime_type(t.sub_type(), mime_type::text), boost::bind(&actions_for_type::value_type::first, _1)));
    }
    if (i == command_for_type.end()) {
      i = std::find_if(command_for_type.begin(), command_for_type.end(),
		  boost::bind(&mime_type::operator==, mime_type(t.sub_type(), mime_type::application), boost::bind(&actions_for_type::value_type::first, _1)));
    }

    if (i == command_for_type.end()) {
      if (c) {
        bool result = global_mime_types_manager.is_known_type(t);

        return result || ((t.category() == tipi::mime_type::text)
                && (global_mime_types_manager.guess_mime_type("m.txt").sub_type() != "unknown"));
      }
    }
    else {
      return (*i).second != command_none;
    }

    return false;
  }

  /**
   * \param[in] t mime type for which to get the associated command
   * \param[in] c the command to be associated with the type ($ is replaced by a valid filename)
   *
   * command_for_type[t] -> command_none    if c == command_none and command_for_type[t] == command_system
   * command_for_type[t] -> command_none    if c == command_system and command_for_type[t] != command_system
   **/
  void type_registry::register_command(mime_type const& t, std::string const& c) {

    if (&c == &command_system) {
      if (global_mime_types_manager.is_known_type(t)) {
        command_for_type[t] = global_mime_types_manager.get_command(t);
      }
      else {
        command_for_type[t] = c;
      }
    }
    else if (c.empty() || &c == &command_none) {
      command_for_type[t] = command_none;
    }
    else {
      using namespace boost::xpressive;

      assert(regex_search(c, sregex(bos >> *set[~_w] >> +set[_w | punct] >> *(+set[~_w] >> *set[_w | punct]) >> eos)));

      command_for_type[t] = c;
    }
  }

  /**
   * \param[in] e extension for which to get the mime type
   **/
  std::auto_ptr < mime_type > type_registry::mime_type_for_extension(std::string const& e) const {
    std::auto_ptr < mime_type > result(new tipi::mime_type(global_mime_types_manager.guess_mime_type(e)));

    return (result);
  }

  /**
   * \param[in] t mime type for which to get the associated command
   * \param[in] f name of the file to operate on
   **/
  std::auto_ptr < command > type_registry::get_registered_command(mime_type const& t, std::string const& f) const {
    using namespace boost::xpressive;

    std::auto_ptr < command > p;

    actions_for_type::const_iterator i = command_for_type.find(t);

    if (i == command_for_type.end()) {
      i = std::find_if(command_for_type.begin(), command_for_type.end(),
                boost::bind(&mime_type::operator==, t, boost::bind(&actions_for_type::value_type::first, _1)));
    }

    if (i != command_for_type.end()) {
      std::string const& command_string = regex_replace((*i).second, sregex(as_xpr('$')), f);

      if (command_string == command_system) {
        std::string c(global_mime_types_manager.get_command(t));

        if (!c.empty()) {
          p = command::from_command_line(c);
        }
      }
      else if (command_string != command_none) {
        p = command::from_command_line(command_string);
      }
    }
    else {
      std::string c(global_mime_types_manager.get_command(t));

      if (!c.empty()) {
        p = command::from_command_line(c);
      }
    }

    if (p.get()) {
      command::argument_sequence s = p->get_arguments();

      for (command::argument_sequence::iterator i = s.begin(); i != s.end(); ++i) {
        *i = regex_replace(*i, sregex(bos >> !(set = '\'','"')
                              >> '$' >> !(set = '\'','"') >> eos), f);
      }
    }

    return (p);
  }
}
