#ifndef UTILITY_VISITOR_H__
#define UTILITY_VISITOR_H__

#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/mpl/and.hpp>

#include <utility/visitable.h>

namespace utility {

  template < typename S, bool b >
  class visitor_interface;

  /** 
   * \brief Abstract visitor interface class (used by visitable class)
   *
   * Type S is the Source, the name of the derived class and the boolean
   * argument b represents whether or not the visitor preserves constness.
   **/
  template < typename S, bool b = true >
  class visitor;

  /** 
   * \brief Abstract visitor interface class (used by visitable class)
   *
   * Type S is the Source, the name of the derived class and the boolean
   * argument b represents whether or not the visitor preserves constness.
   **/
  template < typename S >
  class visitor< S, true > {
    friend class visitor_interface< S, true >;

    template < typename C >
    friend class visitable;

    public:

      /** \brief Whether or not the visitor preserves constness */
      typedef boost::integral_constant< bool, true > is_const_visitor;

    public:

      /** \brief Visit constant interface function */
      template < class T >
      void visit(T const& t);

      /** \brief Visit constant interface function, with constant argument */
      template < class T, typename U >
      void visit(T const& t, U const& u);

      /** \brief Visit constant interface function, with non-constant argument */
      template < class T, typename U >
      void visit(T const& t, U& u);

      /** \brief Visit constant interface function */
      template < class T >
      void do_accept(T const& t);

      /** \brief Visit constant interface function, with constant argument */
      template < class T, typename U >
      void do_accept(T const& t, U const& u);

      /** \brief Visit constant interface function, with non-constant argument */
      template < class T, typename U >
      void do_accept(T const& t, U& u);

    protected:

      /** \brief Constructor */
      visitor();

    public:

      /** \brief Pure virtual destructor */
      virtual ~visitor() = 0;
  };

  /** 
   * \brief Abstract visitor interface class (used by visitable class)
   *
   * Type S is the Source, the name of the derived class and the boolean
   * argument b represents whether or not the visitor preserves constness.
   **/
  template < typename S >
  class visitor< S, false > {
    friend class visitor_interface< S, false >;

    template < typename C >
    friend class visitable;

    public:

      /** \brief Whether or not the visitor preserves constness */
      typedef boost::integral_constant< bool, false > is_const_visitor;

    public:

      /** \brief Visit interface function */
      template < class T >
      void visit(T& t);

      /** \brief Visit interface function, with constant argument */
      template < class T, typename U >
      void visit(T& t, U const& u);

      /** \brief Visit interface function, with non-constant argument */
      template < class T, typename U >
      void visit(T& t, U& u);

      /** \brief Visit interface function */
      template < class T >
      void do_accept(T& t);

      /** \brief Visit interface function, with constant argument */
      template < class T, typename U >
      void do_accept(T& t, U const& u);

      /** \brief Visit interface function, with non-constant argument */
      template < class T, typename U >
      void do_accept(T& t, U& u);

    protected:

      /** \brief Constructor */
      visitor();

    public:

      /** \brief Pure virtual destructor */
      virtual ~visitor() = 0;
  };

  /**
   * \brief Visitor interface class
   *
   * Type S is the type of the visitor implementation class, b specifies
   * whether or not the visitor is constant.
   **/
  template < typename S, bool b = true >
  class visitor_interface {

    public:

      /** \brief Whether or not the visitor preserves constness */
      typedef visitor < S, b >                 visitor_type;

    protected:

      /** \brief Pointer to implementation object (handle-body idiom) */
      boost::shared_ptr < visitor < S, b > > impl;

    public:

      /** \brief visit interface function */
      template < class T >
      void do_accept(T const& t, typename boost::enable_if< boost::integral_constant< bool, b > >* = 0);

      /** \brief visit interface function */
      template < class T >
      void do_accept(T& t, typename boost::disable_if < boost::mpl::and_< boost::integral_constant< bool, b >, boost::is_const< T > > >::type* = 0);

      /** \brief visit interface function */
      template < class T, typename U >
      void do_accept(T const& t, U const& r, typename boost::enable_if < boost::integral_constant< bool, b > >* = 0);

      /** \brief visit interface function */
      template < class T, typename U >
      void do_accept(T& t, U& r, typename boost::disable_if < boost::mpl::and_< boost::integral_constant< bool, b >, boost::is_const< T > > >::type* = 0);

      /** \brief Constructor */
      visitor_interface(boost::shared_ptr < visitor< S, b > > v);
  };

  template < typename T >
  inline visitor< T, true >::visitor() {
  }

  template < typename T >
  inline visitor< T, false >::visitor() {
  }

  template < typename T >
  inline visitor< T, true >::~visitor() {
  }

  template < typename T >
  inline visitor< T, false >::~visitor() {
  }

  template < typename S, bool b >
  visitor_interface< S, b >::visitor_interface(boost::shared_ptr < visitor< S, b > > v) : impl(v) {
  }

  template < typename S, bool b >
  template < class T >
  void visitor_interface< S, b>::do_accept(T const& t, typename boost::enable_if< boost::integral_constant< bool, b > >*) {

    t.accept(*boost::static_pointer_cast < visitor_type > (impl));
  }

  template < typename S, bool b >
  template < class T >
  void visitor_interface< S, b>::do_accept(T& t, typename boost::disable_if < boost::mpl::and_< boost::integral_constant< bool, b >, boost::is_const< T > > >::type*) {

    t.accept(*boost::static_pointer_cast < visitor_type > (impl));
  }

  template < typename S, bool b >
  template < class T, typename U >
  void visitor_interface< S, b>::do_accept(T const& t, U const& r, typename boost::enable_if < boost::integral_constant< bool, b > >*) {

    t.accept(*boost::static_pointer_cast < visitor_type > (impl), r);
  }

  template < typename S, bool b >
  template < class T, typename U >
  void visitor_interface< S, b>::do_accept(T& t, U & r,
            typename boost::disable_if < boost::mpl::and_< boost::integral_constant< bool, b >, boost::is_const< T > > >::type*) {

    t.accept(*boost::static_pointer_cast < visitor_type > (impl), r);
  }
}

#endif
