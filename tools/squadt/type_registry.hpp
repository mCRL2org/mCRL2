// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file type_registry.h
/// \brief Add your file description here.

#ifndef SQUADT_MISCELLANEOUS_H
#define SQUADT_MISCELLANEOUS_H

#include <map>
#include <set>
#include <utility>
#include <functional>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/range/iterator_range.hpp>

#include "tipi/configuration.hpp"
#include "tipi/mime_type.hpp"

#include "tipi/detail/utility/generic_visitor.hpp"

#include "build_system.hpp"

namespace squadt {

  namespace execution {
    class command;
  }

  using tipi::mime_type;
  using execution::command;

  /**
   * \brief Base class for information on known file formats (by mime types)
   *
   * Indexes tools by format and category to help list the available tools
   * and tool categories when given a format.
   **/
  class type_registry : public utility::visitable {
    template < typename R, typename S >
    friend class utility::visitor;

    public:

      /** \brief Iterator on categories selected by a type (file format) **/
      class category_iterator;

      /** \brief Iterator on categories selected by a type (file format) **/
      class tool_iterator;

    public:

      /** \brief Tool category type */
      typedef tipi::tool::category                                                tool_category;

      /** \brief Maps a tool category to a number of tool objects that can accept input in that */
      typedef std::multimap < tool_category, boost::shared_ptr < const tool > >   tools_for_category;

      /** \brief Maps a storage format to a number of tool categories */
      typedef std::map < build_system::mime_type, tools_for_category >            categories_for_mime_type;

      /** \brief Iterator range type on tools_for_category */
      typedef boost::iterator_range < tools_for_category::const_iterator >        tool_sequence;

      /** \brief Map that associates commands to mime-types */
      typedef std::map < mime_type, std::string >                                 actions_for_type;

    public:

      /** \brief Special value that indicates that the system specified command may be used (see command_for_type) */
      static const std::string   command_system;

      /** \brief Special value that indicates that no command is available (see command_for_type) */
      static const std::string   command_none;

    private:

      /** \brief Maps a format to a map that maps a category to a set of tools for that format and category */
      categories_for_mime_type             categories_for_format;

      /** \brief Maps a mime type to a command string */
      actions_for_type                     command_for_type;

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
      std::set < build_system::mime_type > get_mime_types() const;

      /** \brief Returns a set of known formats */
      std::set < build_system::tool_category > get_categories() const;

      /** \brief Returns the list of tool categories with tools that operate on input a given type */
      std::set < build_system::tool_category > categories_by_mime_type(build_system::mime_type const&) const;

      /** \brief Returns a sequence containing tool category pairs that operate on input a given type */
      tool_sequence tools_by_mime_type(build_system::mime_type const&) const;

      /** \brief Returns a mime_type for a specified extension */
      std::auto_ptr < mime_type > mime_type_for_extension(std::string const&) const;

      /** \brief Returns a pointer to a command that performs a configured action on a file */
      std::auto_ptr < command > get_registered_command(mime_type const&, std::string const& = "$") const;

      /** \brief Guesses a mime type from the name of a file */
      tipi::mime_type mime_type_from_name(boost::filesystem::path const& p, const bool c = true) const;

      /** \brief Associates a type with a command */
      void register_command(mime_type const&, std::string const&);

      /** \brief Whether or not a command is associated with this type */
      bool has_registered_command(mime_type const& t, const bool c = true) const;
  };

  inline type_registry::type_registry() {
  }

  inline boost::shared_ptr < type_registry > type_registry::create() {
    boost::shared_ptr < type_registry > p(new type_registry);

    return (p);
  }

  inline void type_registry::rebuild_indices() {
    build_index();
  }
}

#endif
