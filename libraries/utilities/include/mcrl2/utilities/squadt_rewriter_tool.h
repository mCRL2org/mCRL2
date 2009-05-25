// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/squadt_rewriter_tool.h
/// \brief Base class for tools that use a rewriter and squadt interactor.

#ifndef MCRL2_UTILITIES_SQUADT_REWRITER_TOOL_H
#define MCRL2_UTILITIES_SQUADT_REWRITER_TOOL_H

#include "mcrl2/utilities/squadt_tool.h"

namespace mcrl2 {
  namespace utilities {
    namespace tools {

      /// \brief Specialisation for rewrite tools
      template < typename Tool >
      class squadt_tool< rewriter_tool< Tool > > : public rewriter_tool< squadt_tool< Tool > > {

        private:

          typedef rewriter_tool< squadt_tool< Tool > > super_type;

          void set_rewrite_strategy(mcrl2::data::rewriter::strategy s)
          {
            this->m_rewrite_strategy = s;
          }

        protected:

          /// \brief activates convesion from rewriter::strategy to string and vice versa
          bool activate_strategy_conversion()
          {
            tipi::datatype::enumeration< mcrl2::data::rewriter::strategy > strategy_enumeration;

            strategy_enumeration.
              add(mcrl2::data::rewriter::innermost, "inner").
              add(mcrl2::data::rewriter::jitty, "jitty");

# ifdef MCRL2_INNERC_AVAILABLE
            strategy_enumeration.add(mcrl2::data::rewriter::innermost_compiling, "innerc");
# endif
# ifdef MCRL2_JITTYC_AVAILABLE
            strategy_enumeration.add(mcrl2::data::rewriter::jitty_compiling, "jittyc");
# endif
            return true;
          }

          /// \brief adds the standard rewrite strategy selection controls to a display
          /// Type Arguments:
          /// \arg LayoutManager either tipi::layout::horizontal_box or tipi::layout::vertical_box
          /// Arguments:
          /// \param[in] display the display on which the controls are to be placed
          /// \param[in] base a layout manager owned by display into which the selection controls are placed
          /// \note the value of m_rewrite_strategy determines the default stratgy
          /// \note the value of m_rewrite_strategy is updated accordingly
          template < typename LayoutManager >
          void add_rewrite_option(tipi::tool_display& display, LayoutManager& base)
          {
            using data::rewriter;
            using namespace tipi::layout;
            using namespace tipi::layout::elements;

            bool initialised = activate_strategy_conversion();

            static_cast< void >(initialised);

            mcrl2::utilities::squadt::radio_button_helper< rewriter::strategy > strategy_selector(display);

            horizontal_box& m = display.create< horizontal_box >();

            m.append(strategy_selector.associate(rewriter::innermost, "inner")).
              append(strategy_selector.associate(rewriter::jitty, "jitty"));

            strategy_selector.get_button(rewriter::innermost).on_change(
                boost::bind(&squadt_tool< rewriter_tool< Tool > >::set_rewrite_strategy, this, rewriter::innermost));
            strategy_selector.get_button(rewriter::jitty).on_change(
                boost::bind(&squadt_tool< rewriter_tool< Tool > >::set_rewrite_strategy, this, rewriter::jitty));

# ifdef MCRL2_INNERC_AVAILABLE
            m.append(strategy_selector.associate(rewriter::innermost_compiling, "inner (compiling)"));

            strategy_selector.get_button(rewriter::innermost_compiling).on_change(
                boost::bind(&squadt_tool< rewriter_tool< Tool > >::set_rewrite_strategy, this, rewriter::innermost_compiling));
# endif
# ifdef MCRL2_JITTYC_AVAILABLE
            m.append(strategy_selector.associate(rewriter::jitty_compiling, "jitty (compiling)"));

            strategy_selector.get_button(rewriter::jitty_compiling).on_change(
                boost::bind(&squadt_tool< rewriter_tool< Tool > >::set_rewrite_strategy, this, rewriter::jitty_compiling));
# endif

            base.append(display.create< label >().set_text("Rewrite strategy")).append(m);
          }

          /// \brief Updates rewrite strategy in a configuration
          void update_configuration(tipi::configuration& c) {
            if (!c.option_exists("rewrite-strategy")) {
              c.add_option("rewrite-strategy");
            }

            static_cast< tipi::configuration::option& >(c.add_option("rewrite-strategy")).
                                set_argument_value< 0 >(this->rewrite_strategy());

            super_type::update_configuration(c);
          }

          /// \brief Adds rewrite strategy to the configuration or vice versa if the strategy is part of the configuration
          void synchronise_with_configuration(tipi::configuration& c) {
            if (c.option_exists("rewrite-strategy")) {
              this->m_rewrite_strategy = c.get_option_argument< mcrl2::data::rewriter::strategy >("rewrite-strategy", 0);
            }
            else {
              static_cast< tipi::configuration::option& >(c.add_option("rewrite-strategy")).
                                set_argument_value< 0 >(this->rewrite_strategy());
            }

            super_type::synchronise_with_configuration(c);
          }

        public:

          /// \brief Constructor.
          squadt_tool(const std::string& name,
                        const std::string& author,
                        const std::string& what_is,
                        const std::string& tool_description,
                        std::string known_issues = "")
            : super_type(name, author, what_is, tool_description, known_issues)
          {}
      };
    }
  }
}
#endif

