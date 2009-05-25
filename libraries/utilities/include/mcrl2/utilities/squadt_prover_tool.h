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

#ifndef MCRL2_UTILITIES_SQUADT_PROVER_TOOL_H
#define MCRL2_UTILITIES_SQUADT_PROVER_TOOL_H

#include "mcrl2/utilities/squadt_tool.h"

namespace mcrl2 {
  namespace utilities {
    namespace tools {

      /// \brief Specialisation for rewrite tools
      template < typename Tool >
      class squadt_tool< prover_tool< Tool > > : public prover_tool< squadt_tool< Tool > > {

        private:

          typedef prover_tool< squadt_tool< Tool > > super_type;

          void set_solver_type(super::smt_solver_type s)
          {
            this->m_solver_type = s;
          }

        protected:

          /// \brief activates convesion from rewriter::strategy to string and vice versa
          bool activate_solver_conversion()
          {
            tipi::datatype::enumeration< super::solver_type > solver_enumeration;

            strategy_enumeration.
              add(solver_type_cvc_fast, "none").
              add(solver_type_ario, "ario").
              add(solver_type_cvc, "CVC3", true);

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
            using namespace mcrl2::data::detail;
            using namespace tipi::layout;
            using namespace tipi::layout::elements;

            bool initialised = activate_solver_conversion();

            static_cast< void >(initialised);

            mcrl2::utilities::squadt::radio_button_helper< super::smt_solver_type > strategy_selector(display);

            horizontal_box& m = display.create< horizontal_box >();

            m.append(solver_selector.associate(solver_type_cvc_fast, "none")).
              append(solver_selector.associate(solver_type_ario, "ario")).
              append(solver_selector.associate(solver_type_cvc, "CVC3", true));

            strategy_selector.get_button(solver_type_cvc_fast).on_change(
                boost::bind(&squadt_tool< rewriter_tool< Tool > >::set_solver_type, this, solver_type_cvc_fast));
            strategy_selector.get_button(solver_type_ario).on_change(
                boost::bind(&squadt_tool< rewriter_tool< Tool > >::set_solver_type, this, solver_type_ario));
            strategy_selector.get_button(solver_type_cvc).on_change(
                boost::bind(&squadt_tool< rewriter_tool< Tool > >::set_solver_type, this, solver_type_cvc));

            base.append(display.create< label >().set_text("SMT solver")).append(m);
          }

          /// \brief Updates rewrite strategy in a configuration
          void update_configuration(tipi::configuration& c) {
            if (!c.option_exists("smt-solver")) {
              c.add_option("smt-solver");
            }

            static_cast< tipi::configuration::option& >(c.add_option("smt-solver")).
                                set_argument_value< 0 >(this->solver_type());

            super_type::update_configuration(c);
          }

          /// \brief Adds rewrite strategy to the configuration or vice versa if the strategy is part of the configuration
          void synchronise_with_configuration(tipi::configuration& c) {
            if (c.option_exists("smt-solver")) {
              this->m_rewrite_strategy = c.get_option_argument< super::smt_solver_type >("smt-solver", 0);
            }
            else {
              static_cast< tipi::configuration::option& >(c.add_option("smt-solver")).
                                set_argument_value< 0 >(this->solver_type());
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

