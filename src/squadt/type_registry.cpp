#include "type_registry.h"

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include <sip/mime_type.h>

#include <wx/mimetype.h>

#include "tool_manager.h"
#include "command.h"
#include "tool.h"

wxMimeTypesManager global_mime_types_manager;

namespace squadt {

  const std::string type_registry::command_system;
  const std::string type_registry::command_none;

  /**
   * Contacts the local tool manager to ask for the current list of tools,
   * and stores this information in the tool information cache (tool_information_cache).
   *
   * \attention Not thread safe
   **/
  void type_registry::build_index() {
    tool_manager::tool_const_sequence tools = global_build_system.get_tool_manager()->get_tools();

    /* Make sure the map is empty */
    categories_for_format.clear();

    for (tool_manager::tool_const_sequence::const_iterator t = tools.begin(); t != tools.end(); ++t) {

      BOOST_FOREACH(sip::tool::capabilities::input_combination j, (*t)->get_capabilities()->get_input_combinations()) {
        if (categories_for_format.find(j.format) == categories_for_format.end()) {
          /* Format unknown, create new map */
          tools_for_category temporary;

          categories_for_format.insert(std::make_pair(j.format,temporary));
        }

        categories_for_format[j.format].insert(std::make_pair(j.category, *t));
      }
    }
  }

  /**
   * @param f the format for which to execute the action a
   **/
  type_registry::tool_sequence type_registry::tools_by_mime_type(build_system::storage_format const& f) const {

    type_registry::tool_sequence range;

    categories_for_mime_type::const_iterator i = categories_for_format.find(f);

    if (i != categories_for_format.end()) {
      tools_for_category const& p((*i).second);

      range = boost::make_iterator_range(p.begin(), p.end());
    }
    else { // return an empty sequence
      type_registry::tool_sequence::iterator i;

      range = boost::make_iterator_range(i, i);
    }

    return (range);
  }

  /**
   * @param f the format for which to execute the action a
   **/
  std::set < type_registry::tool_category > type_registry::categories_by_mime_type(build_system::storage_format const& f) const {
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

  std::set < build_system::storage_format > type_registry::get_storage_formats() const {
    std::set < build_system::storage_format > formats;

    BOOST_FOREACH(categories_for_mime_type::value_type c, categories_for_format) {
      formats.insert(c.first);
    }

    BOOST_FOREACH(tool::sptr t, global_build_system.get_tool_manager()->get_tools()) {
      BOOST_FOREACH(sip::tool::capabilities::output_combination j, t->get_capabilities()->get_output_combinations()) {
        formats.insert(j.format);
      }
    }

    return (formats);
  }

  /** \brief Whether or not a command is associated with this type */
  bool type_registry::has_registered_command(mime_type const& t) const {
    bool result = true;

    if (command_for_type.find(t) == command_for_type.end()) {
      result = (global_mime_types_manager.GetFileTypeFromMimeType(wxString(t.to_string().c_str(), wxConvLocal)) != 0);
    }
    else {
      result = ((*command_for_type.find(t)).second != command_none);
    }

    return (result);
  }

  /**
   * \param[in] t mime type for which to get the associated command
   * \param[in] c the command to be associated with the type ($ is replaced by a valid filename)
   *
   * command_for_type[t] -> command_none    if c == command_none and command_for_type[t] == command_system
   * command_for_type[t] -> command_none    if c == command_system and command_for_type[t] != command_system
   **/
  void type_registry::register_command(mime_type const& t, std::string const& c) {
    assert(boost::regex_search(c, boost::regex("\\`[^[:word:]]*([[:word:][:punct:]]+)(?:[^[:word:]]+([[:word:][:punct:]]*))*\\'")));

    if (&c == &command_system) {
      if (global_mime_types_manager.GetFileTypeFromMimeType(wxString(t.to_string().c_str(), wxConvLocal)) != 0) {
        command_for_type[t] = c;
      }
      else {
        command_for_type[t] = global_mime_types_manager.GetFileTypeFromMimeType(wxString(t.to_string().c_str(), wxConvLocal))->GetOpenCommand(wxT("$")).fn_str();
      }
    }
    else if (c.empty()) {
      command_for_type[t] = command_none;
    }
    else {
      command_for_type[t] = c;
    }
  }

  /**
   * \param[in] e extension for which to get the mime type
   **/
  std::auto_ptr < mime_type > type_registry::mime_type_for_extension(std::string const& e) const {
    std::auto_ptr < mime_type > result;

    wxString file_mime_type;

    if (global_mime_types_manager.GetFileTypeFromExtension(wxString(e.c_str(), wxConvLocal))->GetMimeType(&file_mime_type)) {
      result.reset(new mime_type(std::string(file_mime_type.fn_str())));
    }

    return (result);
  }

  /**
   * \param[in] t mime type for which to get the associated command
   * \param[in] f name of the file to operate on
   **/
  std::auto_ptr < command > type_registry::get_registered_command(mime_type const& t, std::string const& f) const {
    std::auto_ptr < command > p;

    std::map < mime_type, std::string >::const_iterator i = command_for_type.find(t);

    if (i != command_for_type.end()) {
      std::string const& command_string = boost::regex_replace((*i).second, boost::regex("\\b\\$\\b"), f);

      if (command_string == command_system) {
        wxFileType* wxt = global_mime_types_manager.GetFileTypeFromMimeType(wxString(t.to_string().c_str(), wxConvLocal));

        if (wxt != 0) {
          p = command::from_command_line(std::string(wxt->GetOpenCommand(wxString(f.c_str(), wxConvLocal)).fn_str()));
        }
      }
      else if (command_string != command_none) {
        p = command::from_command_line(command_string);
      }
    }
    else {
      wxFileType* wxt = global_mime_types_manager.GetFileTypeFromMimeType(wxString(t.to_string().c_str(), wxConvLocal));

      if (wxt != 0) {
        p = command::from_command_line(std::string(wxt->GetOpenCommand(wxString(f.c_str(), wxConvLocal)).fn_str()));
      }
    }

    if (p.get()) {
      command::argument_sequence s = p->get_arguments();

      for (command::argument_sequence::iterator i = s.begin(); i != s.end(); ++i) {
        *i = boost::regex_replace(*i, boost::regex("\\`\\$\\'"), f);
      }
    }

    return (p);
  }
}
