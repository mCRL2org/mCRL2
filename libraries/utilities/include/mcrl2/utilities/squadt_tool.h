// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/squadt_tool.h
/// \brief Base class for tools that use a squadt interactor.

#ifndef MCRL2_UTILITIES_SQUADT_TOOL_H
#define MCRL2_UTILITIES_SQUADT_TOOL_H

# ifndef ENABLE_SQUADT_CONNECTIVITY
namespace mcrl2 {
  namespace utilities {
    namespace tools {

      /// \brief Dummy class for tools that can use a squadt interactor but squadt connectivity is not enabled.
      template < typename Tool >
      struct squadt_tool : public Tool {
        /// \brief Constructor.
        squadt_tool(const std::string& name,
                      const std::string& author,
                      const std::string& what_is,
                      const std::string& tool_description)
          : Tool(name, author, what_is, tool_description)
        {}
      };
    } // namespace tools
  } // namespace utilities
} // namespace mcrl2
# else

#include "boost/bind.hpp"

# include "mcrl2/utilities/mcrl2_squadt_interface.h"
# include "mcrl2/utilities/input_output_tool.h"
# include "mcrl2/utilities/rewriter_tool.h"
# include "mcrl2/utilities/pbes_rewriter_tool.h"
# include "mcrl2/utilities/tipi_ext.h"

namespace mcrl2 {
  namespace utilities {
    namespace tools {

      namespace detail {
        struct dummy_tool {
        };
      }

      /// \brief Base class for tools that use a squadt interactor
      template < typename Tool >
      class squadt_tool: public Tool, public mcrl2::utilities::squadt::mcrl2_tool_interface {

        public:

          void update_configuration(tipi::configuration& c);

          void synchronise_with_configuration(tipi::configuration& c);

        public:

          /// \brief Constructor.
          squadt_tool(const std::string& name,
                        const std::string& author,
                        const std::string& what_is,
                        const std::string& tool_description)
            : Tool(name, author, what_is, tool_description)
          {}

          int execute(int argc, char** argv)
          {
            if (mcrl2::utilities::squadt::free_activation(*this, argc, argv)) {
              return EXIT_SUCCESS;
            }

            return Tool::execute(argc, argv);
          }
      };

      template <>
      void squadt_tool< input_tool >::update_configuration(tipi::configuration&) {
      }

      template <>
      void squadt_tool< input_output_tool >::update_configuration(tipi::configuration& c) {
        input_output_tool::m_output_filename = c.get_output("main-output").location();
      }

      template <>
      void squadt_tool< input_tool >::synchronise_with_configuration(tipi::configuration& c) {
        input_tool::m_input_filename = c.get_input("main-input").location();
      }

      template <>
      void squadt_tool< input_output_tool >::synchronise_with_configuration(tipi::configuration& c) {
        if (!c.output_exists("main-output")) {
          c.add_output("main-output",
              tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
        }
        else {
          input_output_tool::m_output_filename = c.get_output("main-output").location();
        }

        input_output_tool::m_input_filename = c.get_input("main-input").location();
      }

      /// \brief Specialisation for rewrite tools
      template < typename Tool >
      class squadt_tool< rewriter_tool< Tool > > : public rewriter_tool< squadt_tool< Tool > > {

        private:

          typedef rewriter_tool< squadt_tool< Tool > > super_type;

          void set_rewrite_strategy(mcrl2::new_data::rewriter::strategy s)
          {
            this->m_rewrite_strategy = s;
          }

        protected:

          /// \brief actives convesion from rewriter::strategy to string and vice versa
          bool activate_strategy_conversion()
          {
            tipi::datatype::enumeration< mcrl2::new_data::rewriter::strategy > strategy_enumeration;

            strategy_enumeration.
              add(mcrl2::new_data::rewriter::innermost, "inner").
              add(mcrl2::new_data::rewriter::jitty, "jitty");

# ifdef MCRL2_INNERC_AVAILABLE
            strategy_enumeration.add(mcrl2::new_data::rewriter::innermost_compiling, "innerc");
# endif
# ifdef MCRL2_JITTYC_AVAILABLE
            strategy_enumeration.add(mcrl2::new_data::rewriter::jitty_compiling, "jittyc");
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
            using new_data::rewriter;
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
              this->m_rewrite_strategy = c.get_option_argument< mcrl2::new_data::rewriter::strategy >("rewrite-strategy", 0);
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
                        const std::string& tool_description)
            : super_type(name, author, what_is, tool_description)
          {}
      };

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
                        const std::string& tool_description)
            : super_type(name, author, what_is, tool_description)
          {}
      };

    } // namespace tools
  } // namespace utilities
} // namespace mcrl2
#endif

#endif // MCRL2_UTILITIES_REWRITER_TOOL_H
