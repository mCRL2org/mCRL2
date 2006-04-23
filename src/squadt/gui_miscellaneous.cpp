#include "gui_miscellaneous.h"

#include "tool.h"
#include "tool_manager.h"

namespace squadt {
  namespace miscellaneous {

    /**
     * Contacts the local tool manager to ask for the current list of tools,
     * and stores this information in the tool information cache (tool_information_cache).
     *
     * \attention Not thread safe
     **/
    void tool_selection_helper::build_index() {
      const tool_manager::tool_list& tools = global_tool_manager->get_tools();

      /* Make sure the map is empty */
      categories_for_format.clear();

      for (tool_manager::tool_list::const_iterator i = tools.begin(); i != tools.end(); ++i) {
        sip::tool::capabilities::input_combination_list c = (*i)->get_capabilities()->get_input_combinations();

        for (sip::tool::capabilities::input_combination_list::const_iterator j = c.begin(); j != c.end(); ++j) {
          if (categories_for_format.find((*j).format) == categories_for_format.end()) {
            /* Format is not known, create new map */
            tools_by_category temporary;

            categories_for_format[(*j).format] = temporary;
          }

          categories_for_format[(*j).format].insert(tools_by_category::value_type((*j).category, (*i)));
        }
      }
    }

    /**
     * @param f the format for which to execute the action a
     * @param a the function to execute for each category/tool pair
     **/
    void tool_selection_helper::by_format(const storage_format f, const category_tool_action a) const {
      categories_by_format::const_iterator i = categories_for_format.find(f);

      if (i != categories_for_format.end()) {
        const tools_by_category& t = (*i).second;

        std::for_each(t.begin(), t.end(), a);
      }
    }
  }
}
