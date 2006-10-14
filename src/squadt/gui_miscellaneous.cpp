#include "gui_miscellaneous.h"

#include "tool.h"
#include "tool_manager.h"

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include <wx/mimetype.h>

wxMimeTypesManager global_mime_types_manager;

namespace squadt {
  namespace miscellaneous {

    char* const mime_type::main_type_as_string[] = { "application", "audio", "image", "message", "multipart", "text", "video", 0 };

    static const boost::regex match_type_and_subtype("([^ \\n()<>@,;:\\\\\"/\\[\\]?.=]+)/?([^ \\n()<>@,;:\\\\\"/\\[\\]?.=]+)?");

    /**
     * \param[in] s a string that represents a mime type
     **/
    mime_type::mime_type(std::string const& s) : m_main(unknown) {
      boost::smatch  matches;

      if (boost::regex_match(s, matches, match_type_and_subtype)) {
        if (matches.size() == 3) {
          m_sub = matches[2];

          char* const* x = &main_type_as_string[0];

          while (*x != 0) {
            if (*x == matches[1]) {
              m_main = static_cast < main_type > (x - &main_type_as_string[0]);

              break;
            }

            ++x;
          }
        }
        else if (matches.size() == 2) {
          m_sub = matches[1];
        }
      }
    }

    /**
     * \param[in] s the subtype string (must not contain white space characters)
     * \param[in] m the main type
     **/
    mime_type::mime_type(std::string const& s, main_type m) : m_main(m), m_sub(s) {
      assert(!s.empty() && !s.find(' ') && !(s.find('\t')));
    }

    /**
     * Contacts the local tool manager to ask for the current list of tools,
     * and stores this information in the tool information cache (tool_information_cache).
     *
     * \attention Not thread safe
     **/
    void type_registry::build_index() {
      const tool_manager::tool_list& tools = global_tool_manager->get_tools();

      /* Make sure the map is empty */
      categories_for_format.clear();

      for (tool_manager::tool_list::const_iterator i = tools.begin(); i != tools.end(); ++i) {
        sip::tool::capabilities::input_combination_list c = (*i)->get_capabilities()->get_input_combinations();

        for (sip::tool::capabilities::input_combination_list::const_iterator j = c.begin(); j != c.end(); ++j) {
          if (categories_for_format.find((*j).format) == categories_for_format.end()) {
            /* Format is not known, create new map */
            tools_for_category temporary;

            categories_for_format[(*j).format] = temporary;
          }

          categories_for_format[(*j).format].insert(tools_for_category::value_type((*j).category, (*i)));
        }
      }
    }

    /**
     * @param f the format for which to execute the action a
     * @param a the function to execute for each category/tool pair
     **/
    type_registry::tool_sequence type_registry::tools_by_mime_type(storage_format const& f) const {

      type_registry::tool_sequence range;

      categories_for_mime_type::const_iterator i = categories_for_format.find(f);

      if (i != categories_for_format.end()) {
        tools_for_category const& p((*i).second);

        range = boost::make_iterator_range(p.begin(), p.end());
      }

      return (range);
    }

    /**
     * @param f the format for which to execute the action a
     * @param a the function to execute for each category/tool pair
     **/
    std::set < type_registry::tool_category > type_registry::categories_by_mime_type(storage_format const& f) const {
      std::set < tool_category > categories;
      
      categories_for_mime_type::const_iterator i = categories_for_format.find(f);

      if (i != categories_for_format.end()) {
        tools_for_category const& p((*i).second);

        for (tools_for_category::const_iterator j = p.begin(); j != p.end(); ++j) {
          categories.insert((*j).first);
        }
      }

      return (categories);
    }

    std::set < storage_format > type_registry::get_categories() const {
      std::set < tool_category > categories;
      
      BOOST_FOREACH(categories_for_mime_type::value_type i, categories_for_format) {
        tools_for_category const& p(i.second);

        for (tools_for_category::const_iterator j = p.begin(); j != p.end(); ++j) {
          categories.insert((*j).first);
        }
      }

      return (categories);
    }

    std::set < storage_format > type_registry::get_storage_formats() const {
      std::set < storage_format > formats;

      BOOST_FOREACH(categories_for_mime_type::value_type c, categories_for_format) {
        formats.insert(c.first);
      }

      return (formats);
    }

    /**
     * \param[in] t mime type for which to get the associated command
     * \param[in] c the command to be associated with the type ($ is replaced by a valid filename)
     **/
    void associate_command_with_mime_type(mime_type const& t, std::string const& c) {
      static const boost::regex command_parse_expression("(executable)()*");

      boost::smatch  matches;

    }

    /**
     * \param[in] e extension for which to get the mime type
     **/
    std::auto_ptr < mime_type > type_registry::mime_type_for_extension(std::string const& e) const {
      std::auto_ptr < mime_type > p;

//      global_mime_types_manager.GetFileTypeFromExtension(wxString(e.c_str(), wxConvLocal))->GetOpenCommand();

      return (p);
    }

    /**
     * \param[in] t mime type for which to get the associated command
     **/
    std::auto_ptr < command > type_registry::command_for_mime_type(mime_type const& t) const {
      std::auto_ptr < command > p;

//      std::string c(global_mime_types_manager.GetFileTypeFromMimeType(wxString(t.to_string().c_str(), wxConvLocal)).fn_str());

      // substitute values for %s, %t and %{}
//      if (c) {
//        static const boost::regex("\\%s");
//      }

      return (p);
    }
  }
}
