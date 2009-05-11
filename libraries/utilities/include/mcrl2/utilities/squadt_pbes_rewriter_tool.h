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

#ifndef MCRL2_UTILITIES_SQUADT_PBES_REWRITER_TOOL_H
#define MCRL2_UTILITIES_SQUADT_PBES_REWRITER_TOOL_H

#include "mcrl2/utilities/squadt_tool.h"

namespace mcrl2 {
  namespace utilities {
    namespace tools {

      /// \brief Specialisation for rewrite tools
      template < typename Tool >
      class squadt_tool< pbes_rewriter_tool< Tool > > : public pbes_rewriter_tool< squadt_tool< Tool > > {

        private:

          typedef pbes_rewriter_tool< squadt_tool< Tool > > super_type;
          typedef typename super_type::pbes_rewriter_type   pbes_rewriter_type;

          void set_pbes_rewriter_type(pbes_rewriter_type s)
          {
            this->m_pbes_rewriter_type = s;
          }

        protected:

          /// \brief actives convesion from rewriter::strategy to string and vice versa
          bool activate_strategy_conversion()
          {
            tipi::datatype::enumeration< pbes_rewriter_type > rewriter_type_enumeration;

            rewriter_type_enumeration.
                add(super_type::simplify, "simplify").
                add(super_type::quantifier_all, "quantifier_all").
                add(super_type::quantifier_finite, "quantifier_finite");

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
          void add_pbes_rewrite_option(tipi::tool_display& display, LayoutManager& base)
          {
            using namespace tipi::layout;
            using namespace tipi::layout::elements;
            using namespace utilities;

            bool initialised = activate_strategy_conversion();

            static_cast< void >(initialised);

            // Helper for pbes rewrite strategy selection, aka quantifier elimination
            mcrl2::utilities::squadt::radio_button_helper < pbes_rewriter_type > pbes_rewriter_type_selector(display);

            horizontal_box& m = display.create< horizontal_box >();

            m.append(display.create< horizontal_box >().
                          append(pbes_rewriter_type_selector.associate(super_type::simplify, "none")).
                          append(pbes_rewriter_type_selector.associate(super_type::quantifier_all, "all")).
                          append(pbes_rewriter_type_selector.associate(super_type::quantifier_finite, "only finite")),
                    margins(0,5,0,5));

            pbes_rewriter_type_selector.get_button(super_type::simplify).on_change(
                boost::bind(&squadt_tool< pbes_rewriter_tool< Tool > >::set_pbes_rewriter_type, this, super_type::simplify));
            pbes_rewriter_type_selector.get_button(super_type::quantifier_all).on_change(
                boost::bind(&squadt_tool< pbes_rewriter_tool< Tool > >::set_pbes_rewriter_type, this, super_type::quantifier_all));
            pbes_rewriter_type_selector.get_button(super_type::quantifier_finite).on_change(
                boost::bind(&squadt_tool< pbes_rewriter_tool< Tool > >::set_pbes_rewriter_type, this, super_type::quantifier_finite));

            base.append(display.create< label >().set_text("Quantifier elimination : ")).append(m);
          }

          /// \brief Updates rewrite strategy in a configuration
          void update_configuration(tipi::configuration& c) {
            static_cast< tipi::configuration::option& >(c.add_option("pbes-rewriter-type")).
                set_argument_value< 0 >(this->rewriter_type());

            super_type::update_configuration(c);
          }

          /// \brief Adds rewrite strategy to the configuration or vice versa if the strategy is part of the configuration
          void synchronise_with_configuration(tipi::configuration& c) {
            if (c.option_exists("pbes-rewriter-type")) {
              this->m_pbes_rewriter_type = c.get_option_argument< pbes_rewriter_type >("pbes-rewriter-type", 0);
            }
            else {
              static_cast< tipi::configuration::option& >(c.add_option("pbes-rewriter-type")).
                 set_argument_value< 0 >(this->rewriter_type());
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

