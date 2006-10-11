#ifndef SQUADT_MISCELLANEOUS_H
#define SQUADT_MISCELLANEOUS_H

#include <map>
#include <set>
#include <utility>
#include <functional>

#include <boost/shared_ptr.hpp>
#include <boost/range/iterator_range.hpp>

#include <sip/detail/configuration.h>

#include "tool.h"

namespace squadt {
  namespace execution {
    class command;
  }

  namespace miscellaneous {

    using execution::command;

    /**
     * \brief Class that provides a partial mime-type implementation
     **/
    class mime_type {

      public:

        /** \brief Recognised main types */
        enum main_type {
          unknown,
          application,
          text
        };

      private:

        /** \brief Strings for conversion of main types */
        static char* const main_type_as_string[];

      private:

        /** \brief The main type */
        main_type   m_main;

        /** \brief The sub type */
        std::string m_sub;

      public:

        /** \brief Constructor */
        mime_type(std::string const&, main_type s = unknown);

        /** \brief Gets the main type */
        std::string get_main_type();

        /** \brief Gets the sub type */
        std::string get_sub_type();

        /** \brief Converts to string */
        std::string to_string();

        /** \brief Compare for equality */
        bool operator== (mime_type const&);

        /** \brief Compare for inequality */
        bool operator!= (mime_type const&);
    };

    /**
     * \brief Base class for information on known file formats (by mime types)
     *
     * Indexes tools by format and category to help list the available tools
     * and tool categories when given a format.
     **/
    class type_registry {

      public:

        /** \brief Iterator on categories selected by a type (file format) **/
        class category_iterator;

        /** \brief Iterator on categories selected by a type (file format) **/
        class tool_iterator;

      public:

        /** \brief Tool category type */
        typedef sip::configuration::tool_category                             tool_category;

        /** \brief Alias for convenient reference of shared pointer implementation */
        typedef boost::shared_ptr < type_registry >                           sptr;

        /** \brief Maps a tool category to a number of tool objects that can accept input in that */
        typedef std::multimap < tool_category, tool::sptr >                   tools_for_category;

        /** \brief Maps a storage format to a number of tool categories */
        typedef std::map < storage_format, tools_for_category >               categories_for_mime_type;

        /** \brief Iterator range type on tools_for_category */
        typedef boost::iterator_range < tools_for_category::const_iterator >  tool_sequence;

      private:

        /** \brief Special value that indicates that the system command is linked (see command_for_type) */
        static std::string   command_system_defined;

        /** \brief Special value that indicates that no command is linked (see command_for_type) */
        static std::string   command_none;

      private:

        /** \brief Maps a format to a map that maps a category to a set of tools for that format and category */
        categories_for_mime_type             categories_for_format;

        /** \brief Maps a mime type to a command string */
        std::map < mime_type, std::string >  command_for_type;

      private:

        /** \brief Helper function that gathers the tool information that is used to on-the-fly build context menus */
        void build_index();

      public:

        /** \brief Constructor */
        type_registry();

        /** \brief Factory method */
        static boost::shared_ptr < type_registry > create();

        /** \brief Rebuilds the tool indices (currently only by formats) */
        void rebuild_indices();

        /** \brief Returns a set of known formats */
        std::set < storage_format > get_storage_formats();

        /** \brief Returns a set of known formats */
        std::set < tool_category > get_categories() const;

        /** \brief Returns the list of tool categories with tools that operate on input a given type */
        std::set < tool_category > categories_by_mime_type(const storage_format f) const;

        /** \brief Returns a sequence containing tool category pairs that operate on input a given type */
        tool_sequence tools_by_mime_type(const storage_format f) const;

        std::auto_ptr < command > command_for_mime_type(mime_type const&);
    };

    inline type_registry::type_registry() {
      build_index();
    }

    inline boost::shared_ptr < type_registry > type_registry::create() {
      boost::shared_ptr < type_registry > p(new type_registry);

      return (p);
    }

    inline void type_registry::rebuild_indices() {
      build_index();
    }

    /**
     * \param[in] s the subtype string (must not contain white space characters)
     * \param[in] m the main type
     **/
    inline mime_type::mime_type(std::string const& s, main_type m) : m_main(m), m_sub(s) {
      assert(!s.empty() && !s.find(' ') && !(s.find('\t')));
    }

    inline std::string mime_type::get_main_type() {
      return (main_type_as_string[m_main]);
    }

    inline std::string mime_type::get_sub_type() {
      return (main_type_as_string[m_main]);
    }

    inline std::string mime_type::to_string() {
      return (std::string(main_type_as_string[m_main]) + "/" + m_sub);
    }

    inline bool mime_type::operator==(mime_type const& r) {
      return (m_main == r.m_main && m_sub == r.m_sub);
    }

    inline bool mime_type::operator!=(mime_type const& r) {
      return (m_main != r.m_main && m_sub != r.m_sub);
    }
  }
}

#endif
