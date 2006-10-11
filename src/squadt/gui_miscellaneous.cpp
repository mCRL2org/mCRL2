#include "gui_miscellaneous.h"

#include "tool.h"
#include "tool_manager.h"

#include <boost/foreach.hpp>

namespace squadt {
  namespace miscellaneous {

    char* const mime_type::main_type_as_string[] = { "application", "text" };

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
    type_registry::tool_sequence type_registry::tools_by_mime_type(const storage_format f) const {

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
    std::set < type_registry::tool_category > type_registry::categories_by_mime_type(const storage_format f) const {
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

    std::set < storage_format > type_registry::get_storage_formats() {
      std::set < storage_format > formats;

      BOOST_FOREACH(categories_for_mime_type::value_type c, categories_for_format) {
        formats.insert(c.first);
      }

      return (formats);
    }
  }
}
