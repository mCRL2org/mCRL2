// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// LICENSE_1_0.txt)
//
/// \file tipi/detail/utility/generic_visitor.hpp

#ifndef UTILITY_GENERIC_VISITOR_H__
#define UTILITY_GENERIC_VISITOR_H__

#include <stdexcept>
#include <algorithm>
#include <typeinfo>
#include <vector>
#include <functional>

#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/type_traits/is_object.hpp>
#include <boost/static_assert.hpp>
#include <boost/shared_ptr.hpp>

namespace utility {

  template < typename R >
  class abstract_visitor;

  template < typename S, typename R >
  class visitor;

  template < typename S, typename R >
  class visitor_interface;

  /**
   * \brief Interface used by visitors to visit
   * 
   * A visitable class needs to derive from this class.
   **/
  class visitable {

    public:

      /** \brief Virtual destructor */
      virtual ~visitable() {
      }
  };

  /// \cond INTERNAL_DOCS
  namespace detail {

    /** \brief Implements a primitive map using std::vector */
    template < typename S >
    class vector_map;

    /** \brief Wrapper around std::type_info */
    class type_info_wrapper;

    /** \brief Wrapper around std::type_info and a vector_map */
    template < typename S >
    class type_info_map_wrapper;

    /** \brief Wrapper around std::type_info and a callback function */
    class type_info_callback_wrapper;

    template < typename T, bool b = boost::is_const< T >::value >
    struct visitable_type_helper {
      typedef const visitable visitable_type;
    };

    template < typename T >
    struct visitable_type_helper< T, false > {
      typedef visitable visitable_type;
    };
  }

  /**
   * Because virtual template member functions do not exist in C++, and there
   * is no other convenient way to simulate them this abstract visitor uses
   * RTTI to implement double and triple dispatching.
   **/
  template < typename R >
  class abstract_visitor {

    public:

      /** \brief Map containing the callback functions */
      typedef detail::type_info_callback_wrapper                                                        argument_type_for_visit_callback;

      /** \brief Map from target type_info_wrapper to argument map */
      typedef detail::vector_map < detail::type_info_map_wrapper< argument_type_for_visit_callback > >  visitable_type_tree;

    private:

      /** \brief Helper function to get a map for looking up a methods by the type_id of the arguments */
      virtual visitable_type_tree& get_visitable_type_tree() const = 0;

    protected:

      /** \brief caller function that resolves types of the first to arguments and executes visit */
      static R call_visit(abstract_visitor< R >& v, visitable const& t);


      /** \brief caller function that resolves types of the first to arguments and executes visit with an additional argument */
      template < typename U >
      static R call_visit(abstract_visitor< R >& v, visitable const& t, U& u);

    public:

      /** \brief Visit constant interface function */
      template < class T >
      R do_visit(T const& t) {
        return abstract_visitor< R >::call_visit(*this, static_cast < visitable const& > (t));
      }

      /** \brief Visit constant interface function, with non-constant argument */
      template < class T, typename U >
      R do_visit(T const& t, U& u) {
        return abstract_visitor< R >::call_visit(*this, static_cast < visitable const& > (t), u);
      }

      /** \brief Visit constant interface function */
      template < class T >
      R do_visit(T& t) {
        return abstract_visitor< R >::call_visit(*this, static_cast < visitable& > (t));
      }

      /** \brief Visit constant interface function, with non-constant argument */
      template < class T, typename U >
      R do_visit(T& t, U& u) {
        return abstract_visitor< R >::call_visit(*this, static_cast < visitable& > (t), u);
      }

      /** \brief Virtual destructor */
      virtual ~abstract_visitor() {
      }
  };

  namespace detail {

    template < typename S >
    class vector_map {

      public:

        typedef S value_type;

      protected:

        std::vector < S > m_map;
  
      public:

        inline S& insert(S const& e) {
          typename std::vector < S >::iterator i = std::lower_bound(m_map.begin(), m_map.end(), e);
  
          if (i == m_map.end() || *i != e) {
            i = m_map.insert(i, e);
          }

          return *i;
        }

        inline S& find(S const& e) {
          typename std::vector < S >::iterator i = std::lower_bound(m_map.begin(), m_map.end(), e);
  
          if (i == m_map.end() || *i != e) {
            throw std::runtime_error("No visit method found with the requested signature");
          }

          return *i;
        }
    };

    struct basic_visit_method_wrapper {
    };

    template < typename R, typename V, typename T, typename U = void >
    struct visit_method_wrapper : public basic_visit_method_wrapper {
      typedef R (*callback_type)(V&, T&, U& u);

      callback_type callback;

      visit_method_wrapper(callback_type c) : callback(c) {
      }
    };

    template < typename R, typename V, typename T >
    struct visit_method_wrapper< R, V, T, void > : public basic_visit_method_wrapper {
      typedef R (*callback_type)(V&, T&);

      callback_type callback;

      visit_method_wrapper(callback_type c) : callback(c) {
      }
    };

    class type_info_wrapper {

      protected:
  
        std::type_info const* m_info;
  
      public:
  
        type_info_wrapper(std::type_info const& o) : m_info(&o) {
        }

        bool operator==(std::type_info const& o) const {
          return (*m_info == o);
        }
  
        bool operator==(type_info_wrapper const& o) const {
          return (*m_info == *o.m_info);
        }
  
        bool operator!=(type_info_wrapper const& o) const {
          return (*m_info != *o.m_info);
        }
  
        bool operator<(type_info_wrapper const& o) const {
          return (m_info->before(*o.m_info) != 0);
        };
    };

    template < typename S >
    class type_info_map_wrapper : public type_info_wrapper  {

      private:
  
        vector_map< S > m_map;

      public:
  
        inline type_info_map_wrapper(std::type_info const& o) : type_info_wrapper(o) {
        }

        inline S& insert(std::type_info const& e) {
          return m_map.insert(S(e));
        }

        inline S& find(std::type_info const& e) {
          return m_map.find(S(e));
        }
    };

    class type_info_callback_wrapper : public type_info_wrapper {

      private:

        boost::shared_ptr < basic_visit_method_wrapper > callback;

      public:

        inline type_info_callback_wrapper(std::type_info const& o) : type_info_wrapper(o) {
        }

        inline type_info_callback_wrapper(type_info_callback_wrapper const& o) :
                type_info_wrapper(static_cast < type_info_wrapper const& > (o)), callback(o.callback) {
        }

        template < typename R, typename V, typename T, typename U >
        inline void set(visit_method_wrapper< R, V, T, U > const& o) {
          callback.reset(new visit_method_wrapper< R, V, T, U >(o));
        }

        inline basic_visit_method_wrapper& get() {
          return (*callback);
        }
    };

  }

  template < typename R >
  R abstract_visitor< R >::call_visit(abstract_visitor& v, visitable const& t) {
    detail::basic_visit_method_wrapper& visit_method = v.get_visitable_type_tree().find(typeid(t)).find(typeid(void)).get();

    return static_cast < detail::visit_method_wrapper< R, abstract_visitor, const visitable, void >& > (visit_method).callback(v, t);
  }

  template < typename R >
  template < typename U >
  R abstract_visitor< R >::call_visit(abstract_visitor& v, visitable const& t, U& u) {
    detail::basic_visit_method_wrapper& visit_method = v.get_visitable_type_tree().find(typeid(t)).find(typeid(U)).get();

    return static_cast < detail::visit_method_wrapper< R, abstract_visitor, const visitable, U >& > (visit_method).callback(v, t, u);
  }

  /**
   * S should be the type of a class that contains resources necessary for implementation of the visit methods
   **/
  template < typename S, typename R = void >
  class visitor : public abstract_visitor< R >, public S {

    friend class abstract_visitor< R >;

    friend class visitor_interface< S, R >;

    private:

      /** \brief Map for looking up visit methods */
      typename abstract_visitor< R >::visitable_type_tree& visitable_types;

    private:

      static typename abstract_visitor< R >::visitable_type_tree& get_master_types() {
        static typename abstract_visitor< R >::visitable_type_tree master_visitable_types;

        return master_visitable_types;
      }

      /** \brief Initialise handler map; registers available visit methods */
      static bool initialise();

      virtual inline typename abstract_visitor< R >::visitable_type_tree& get_visitable_type_tree() const {
        return visitable_types;
      }

    protected:

      /**
       * Registers a visit method for objects of type T
       **/
      template < class T >
      static inline void register_visit_method() {
        struct local {
          static R trampoline(abstract_visitor< R >& v, typename detail::visitable_type_helper< T >::visitable_type& t) {
            return static_cast< visitor& > (v).visit(static_cast < T& > (t));
          }
        };

        get_master_types().insert(typeid(T)).insert(typeid(void)).
            set(detail::visit_method_wrapper< R, abstract_visitor< R >,
		typename detail::visitable_type_helper< T >::visitable_type, void >(&local::trampoline));
      }

      /**
       * Registers a visit method for objects of type T and an additional argument of type U
       **/
      template < class T, class U >
      static inline void register_visit_method() {
        struct local {
          static R trampoline(abstract_visitor< R >& v, typename detail::visitable_type_helper< T >::visitable_type& t, U& u) {
            return static_cast< visitor& > (v).visit(static_cast < T& > (t), u);
          }
        };

        get_master_types().insert(typeid(T)).insert(typeid(U)).
            set(detail::visit_method_wrapper< R, abstract_visitor< R >,
		typename detail::visitable_type_helper< T >::visitable_type, U >(&local::trampoline));
      }

      /** \brief Visit constant interface function */
      template < class T >
      R visit(T& t);

      /** \brief Visit constant interface function, with non-constant argument */
      template < class T, typename U >
      R visit(T& t, U& u);

    public:

      /** \brief Visit constant interface function */
      template < class T >
      R do_visit(T const& t) {
        return abstract_visitor< R >::call_visit(*this, t);
      }

      /** \brief Visit constant interface function, with non-constant argument */
      template < class T, typename U >
      R do_visit(T const& t, U& u) {
        return abstract_visitor< R >::call_visit(*this, t, u);
      }

      /** \brief Default constructor */
      visitor() : visitable_types(get_master_types()) {
      }

      /** \brief Alternative initialisation with reference argument */
      template < typename T >
      visitor(T& t) : S(t), visitable_types(get_master_types()) {
      }

      /** \brief Alternative initialisation with pointer argument */
      template < typename T >
      visitor(T* t) : S(t), visitable_types(get_master_types()) {
      }
  };
  /// \endcond

  /**
   * \brief Interface for new visitors
   *
   * This implementation of a generic visitor component aims to be flexible:
   *  - visit functions can have an optional argument, with boost::tuple this can be generalised
   *  - arguments to specific visit functions can be specified constant
   *
   * To use :
   *  - for each class C that can be visited:
   *
   *    * specify: `template < typename S, typename R > friend class utility::visitor' where necessary to classes in a hierarchy
   *      and derive from utility::visitable
   *
   *  - create a context class Vc for the visitor and instantiate a new type visitor_interface< Vc >:
   *
   *    * implement the desired visit functions by specialising one of the visitor< Vc, R >::visit() methods
   *    * implement visitor< Vc, R >::initialise():
   *
   *      for each visit method specialisation add a line register_visit_method< const A, ... >()
   *      where const A is the type of the first argument etc
   *
   * \note Type S should be the name of a visitor implementation class from which the visitor class should be derived
   * \note If ever the C++ standard implements virtual template methods, then the implementation can be simplified to
   * use the commonly used double dispatch instead of RTTI.
   **/
  template < typename S, typename R = void >
  class visitor_interface {

    template < typename B, typename T >
    friend class basic_visitable;

    private:

      /** \brief Pointer to implementation object (handle-body idiom) */
      boost::shared_ptr < abstract_visitor< R > > impl;

    private:

      /** \brief Method that is used for automated registration of visit functions */
      inline void initialise() {
        static const bool initialised(visitor< S, R >::initialise());
       
        if (initialised) {
        }
      }

    public:

      /** \brief Visit constant interface function */
      template < class T >
      R visit(T const& t) {
        return impl->do_visit(t);
      }

      /** \brief Visit constant interface function, with constant argument */
      template < class T, typename U >
      R visit(T const& t, U const& u) {
        return impl->do_visit(t, u);
      }

      /** \brief Visit constant interface function, with non-constant argument */
      template < class T, typename U >
      R visit(T const& t, U& u) {
        return impl->do_visit(t, u);
      }

      /** \brief Constructor */
      visitor_interface();

      /** \brief Constructor */
      visitor_interface(boost::shared_ptr < visitor< S, R > > const&);
  };

  template < typename S, typename R >
  inline visitor_interface< S, R >::visitor_interface() : impl(new visitor< S, R >) {
    initialise();
  }

  template < typename S, typename R >
  inline visitor_interface< S, R >::visitor_interface(boost::shared_ptr< visitor< S, R > > const& c) : impl(c) {
    initialise();
  }
}

#endif
