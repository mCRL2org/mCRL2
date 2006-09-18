#ifndef SQUADT_MISCELLANEOUS_H
#define SQUADT_MISCELLANEOUS_H

#include <map>
#include <utility>

#include <boost/shared_ptr.hpp>

#include "tool.h"

namespace squadt {
  namespace miscellaneous {

    /**
     * \brief Base class for tool selection
     *
     * Indexes tools by format and category to help list the available tools
     * and tool categories when given a format.
     **/
    class tool_selection_helper {

      public:

        /** \brief Alias for convenient reference of shared pointer implementation */
        typedef boost::shared_ptr < tool_selection_helper >                     sptr;

        /** \brief Maps a tool category to a number of tool objects that can accept input in that */
        typedef std::multimap < tool_category, tool::sptr >                     tools_by_category;

        /** \brief Maps a storage format to a number of tool categories */
        typedef std::map < storage_format, tools_by_category >                  categories_by_format;

        /** \brief Pair of iterators for traversal of a tools_by_category map */
        typedef boost::function < void (const tools_by_category::value_type&) > category_tool_action;

      private:

        /** \brief Maps a format to a map that maps a category to a set of tools for that format and category */
        categories_by_format  categories_for_format;

        /** \brief Helper function that gathers the tool information that is used to on-the-fly build context menus */
        void build_index();

      public:

        /** \brief Constructor */
        inline tool_selection_helper();

        /** \brief Constructor */
        inline void rebuild_index();

        /** \brief A pair of iterators (begin,end) for iterating the set of tools that operate on a given format */
        void by_format(const storage_format f, const category_tool_action a) const;
    };

    inline tool_selection_helper::tool_selection_helper() {
      build_index();
    }

    inline void tool_selection_helper::rebuild_index() {
      build_index();
    }
  }
}

#endif
