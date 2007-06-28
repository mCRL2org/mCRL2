#ifndef SQUADT_UTILITY_H_
#define SQUADT_UTILITY_H_

#include <memory>
#include <string>

#include <tipi/tool.h>

namespace mcrl2 {
  namespace utilities {
    namespace squadt {

#ifdef __WXWINDOWS__
      /** 
       * \brief Convenience class for connecting wxWidgets applications to the environment (platform dependent wrapper around wxEntry)
       **/
      class entry_wrapper {
#ifdef __WINDOWS__
      private:
 
        HINSTANCE        hInstance;
        HINSTANCE        hPrevInstance;
        wxCmdLineArgType lpCmdLine;
        int              nCmdShow;
 
      public:
 
        entry_wrapper(HINSTANCE hc, HINSTANCE hp, wxCmdLineArgType lp, int ns) {
          hInstance     = hc;
          hPrevInstance = hp;
          lpCmdLine     = lp;
          nCmdShow      = ns;
        }
 
        bool perform_entry() {
          return (wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow) == 0);
        }
#else
      private:
 
        int&    argc;
        char**& argv;
 
      public:
 
        entry_wrapper(int& ac, char**& av) : argc(ac), argv(av) {
        }
 
        bool perform_entry() {
          return (wxEntry(argc, argv) == 0);
        }
#endif
      };
#endif

      /** \brief Helper function for showing/hiding an element based on widget state changes */
      template < typename T >
      void change_visibility_on_toggle(T& r, tipi::layout::manager* m, tipi::layout::element*& c);
  
      /** \brief Helper function for showing/hiding an element based on radio_button widget state changes */
      template <>
      inline void change_visibility_on_toggle(tipi::layout::elements::radio_button& r, tipi::layout::manager* m, tipi::layout::element*& c) {
        if (r.is_selected()) {
          m->show(c);
        }
        else {
          m->hide(c);
        }
      }
  
      /** \brief Helper function for showing/hiding an element based on checkbox widget state changes */
      template <>
      inline void change_visibility_on_toggle(tipi::layout::elements::checkbox& r, tipi::layout::manager* m, tipi::layout::element*& c) {
        if (r.get_status()) {
          m->show(c);
        }
        else {
          m->hide(c);
        }
      }
  
      /** Helper class to project the selected radio button in a group to instances of a type T */
      template < typename T >
      class radio_button_helper {
  
        typedef tipi::layout::elements::radio_button radio_button;
  
        private:
  
          std::map < radio_button const*, T > selector;
  
        public:
  
          /** \brief The first button in the group */
          radio_button* first;
  
        public:
  
          /** \brief constructor */
          template < typename M >
          radio_button_helper(M const&, T const&, radio_button*);
  
          /** \brief constructor */
          template < typename M >
          radio_button_helper(M const&, T const&, std::string const&);
  
          /** \brief associate a radio button with a layout manager and a value */
          template < typename M >
          void associate(M const&, T const&, radio_button*);
  
          /** \brief associate a radio button with a layout manager and a value */
          template < typename M >
          radio_button& associate(M const&, T const&, std::string const&, bool = false);
  
          /** \brief gets the button associated with a value */
          radio_button& get_button(T const&);
  
          /** \brief sets the selection for the group of radio buttons */
          void set_selection(T const&);
  
          /** \brief get the value for the selected radio button */
          T get_selection();
  
          /** \brief gets the label of the selected radio button */
          std::string get_selection_label() const;
      };
  
      /**
       * \param[in] l the layout manager to which the button should be attached
       * \param[in] r pointer to the radio button to attach
       * \param[in] v the value to associate the button with
       **/
      template < typename T >
      template < typename M >
      inline radio_button_helper< T >::radio_button_helper(M const& l, T const& v, radio_button* r) {
        first = r;
  
        associate(l, v, r);
      }
  
      /**
       * \param[in] l the layout manager to which the button should be attached
       * \param[in] v the value to associate the button with
       **/
      template < typename T >
      template < typename M >
      inline radio_button_helper< T >::radio_button_helper(M const& l, T const& v, std::string const& s) {
        first = new radio_button(s);
  
        associate(l, v, first);
      }
  
      /**
       * \param[in] l the layout manager to which the button should be attached
       * \param[in] r pointer to the radio button to attach
       * \param[in] v the value to associate the button with
       **/
      template < typename T >
      template < typename M >
      inline void radio_button_helper< T >::associate(M const& l, T const& v, radio_button* r) {
        l->add(r);
  
        selector[r] = v;
      }
  
      /**
       * \param[in] l the layout manager to which the button should be attached
       * \param[in] v the value to associate the button with
       **/
      template < typename T >
      template < typename M >
      inline tipi::layout::elements::radio_button& radio_button_helper< T >::associate(M const& l, T const& v, std::string const& s, bool b) {
        radio_button* button = new radio_button(s, first, b);
  
        l->add(button);
  
        selector[button] = v;
  
        return (*button);
      }
  
      template < typename T >
      inline tipi::layout::elements::radio_button& radio_button_helper< T >::get_button(T const& t) {
        for (typename std::map < radio_button const*, T >::iterator i = selector.begin(); i != selector.end(); ++i) {
          if (i->second == t) {
            return const_cast < radio_button& > (*(i->first));
          }
        }
  
        return *this;
      }
  
      template < typename T >
      inline void radio_button_helper< T >::set_selection(T const& t) {
        for (typename std::map < radio_button const*, T >::iterator i = selector.begin(); i != selector.end(); ++i) {
          if (i->second == t) {
            const_cast < radio_button* > (i->first)->select();
          }
        }
      }
  
      template < typename T >
      inline T radio_button_helper< T >::get_selection() {
        return (selector[first->get_selected()]);
      }
  
      template < typename T >
      std::string radio_button_helper< T >::get_selection_label() const {
        return (first->get_selected()->get_label());
      }
    }
  }
}
 
#endif
