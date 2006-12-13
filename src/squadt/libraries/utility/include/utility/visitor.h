#ifndef UTILITY_VISITOR_H__
#define UTILITY_VISITOR_H__

#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>

namespace utility {

  /** 
   * \brief Abstract visitor interface class (used by visitable class)
   *
   * Type S is the Source, the name of the derived class, type R is the result
   * value of a visit operation and the boolean argument b represents whether
   * or not the visitor preserves constness.
   **/
  template < typename S, typename R , bool b = true >
  class visitor {
    template < typename C >
    friend class visitable;

    public:

      /** \brief The result type of a visit operation */
      typedef R                                   result_type;

      /** \brief Whether or not the visitor preserves constness */
      typedef boost::integral_constant< bool, b > is_const_visitor;

    protected:

      /** \brief visit interface function */
      template < class T >
      result_type visit(T const& t, typename boost::enable_if< boost::is_void < result_type > >::type* dummy = 0) {
        dynamic_cast < S > (this)->visit(t);
      }

      /** \brief visit interface function */
      template < class T >
      result_type visit(T const& t, typename boost::disable_if< boost::is_void < result_type > >* dummy = 0) {
        return (dynamic_cast < S > (this)->visit(t));
      }

    public:

      /** \brief Constructor */
      visitor();

      /** \brief Pure virtual destructor */
      virtual ~visitor() = 0;
  };

  /**
   * \brief Interface for classes that should be visitable
   *
   * Classes can be made accessible to visitors by deriving from this class.
   * The type D represents the type of the derived class
   **/
  template < class D >
  class visitable {
    template < typename R, typename T, bool b >
    friend class visitor;

    public:

      /** \brief Hook for visitor pattern */
      template < typename T >
      typename T::result_type accept(T& v,
                typename boost::enable_if < typename T::is_const_visitor >::type* = 0,
                typename boost::enable_if < typename boost::is_void < typename T::result_type > >::type* = 0) const;

      /** \brief Hook for visitor pattern */
      template < typename T >
      typename T::result_type accept(T& v,
                typename boost::enable_if < typename T::is_const_visitor >::type* = 0,
                typename boost::disable_if < typename boost::is_void < typename T::result_type > >::type* = 0) const;

      /** \brief Hook for visitor pattern */
      template < typename T >
      typename T::result_type accept(T& v,
                typename boost::disable_if_c < T::is_const_visitor::value && boost::is_const< D >::value >::type* = 0,
                typename boost::enable_if < typename boost::is_void < typename T::result_type > >::type* = 0);

      /** \brief Hook for visitor pattern */
      template < typename T >
      typename T::result_type accept(T& v,
                typename boost::disable_if_c < T::is_const_visitor::value && boost::is_const< D >::value >::type* = 0,
                typename boost::disable_if < typename boost::is_void < typename T::result_type > >::type* = 0);

      /** \brief Pure virtual destructor */
      virtual ~visitable() = 0;
  };

  template < typename T, typename R , bool b >
  inline visitor< T, R, b >::visitor() {
  }

  template < typename T, typename R , bool b >
  inline visitor< T, R, b >::~visitor() {
  }

  template < class D >
  template < typename T >
  inline typename T::result_type visitable< D >::accept(T& v,
               typename boost::enable_if < typename T::is_const_visitor >::type* const_dummy,
               typename boost::enable_if < typename boost::is_void < typename T::result_type > >::type* void_dummy) const {

    v.visit(dynamic_cast < D const& > (*this));
  }

  template < class D >
  template < typename T >
  inline typename T::result_type visitable< D >::accept(T& v,
               typename boost::enable_if < typename T::is_const_visitor >::type* const_dummy,
               typename boost::disable_if < typename boost::is_void < typename T::result_type > >::type* void_dummy) const {

    return (v.visit(dynamic_cast < D const& > (*this)));
  }

  template < class D >
  template < typename T >
  inline typename T::result_type visitable< D >::accept(T& v,
               typename boost::disable_if_c < T::is_const_visitor::value && boost::is_const< D >::value >::type* const_dummy,
               typename boost::enable_if < typename boost::is_void < typename T::result_type > >::type* void_dummy) {

    v.visit(dynamic_cast < D& > (*this));
  }

  template < class D >
  template < typename T >
  inline typename T::result_type visitable< D >::accept(T& v,
               typename boost::disable_if_c < T::is_const_visitor::value && boost::is_const< D >::value >::type* const_dummy,
               typename boost::disable_if < typename boost::is_void < typename T::result_type > >::type* void_dummy) {

    return (v.visit(dynamic_cast < D& > (*this)));
  }

  template < class D >
  inline visitable< D >::~visitable() {
  }
}

#endif
