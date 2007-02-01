#include <boost/utility/enable_if.hpp>

namespace utility {

  /**
   * \brief Base class for polymorphic classes
   *
   * To use derive the base class of a visitable class from this class. Derive
   * an implementation class parametrised with a type T from the base class and
   * from visitable < T >. All polymorphic types should now be derived from
   * this implementation class.
   *
   * Implement appropriate declare the appropriate do_accept methods in the
   * base class and implement them in the implementation class.
   **/
  class base_visitable {

    public:

      /** \brief Default destructor */
      base_visitable();

      /** \brief Hook for visitor pattern */
      template < typename V >
      typename boost::enable_if < typename V::is_const_visitor >::type accept(V&) const;

      /** \brief Hook for visitor pattern */
      template < typename V, typename U >
      typename boost::enable_if < typename V::is_const_visitor >::type accept(V&, U const&) const;

      /** \brief Hook for visitor pattern */
      template < typename V, typename U >
      typename boost::enable_if < typename V::is_const_visitor >::type accept(V&, U&) const;

      /** \brief Hook for visitor pattern */
      template < typename V >
      typename boost::disable_if < typename V::is_const_visitor >::type accept(V&);

      /** \brief Hook for visitor pattern */
      template < typename V, typename U >
      typename boost::disable_if < typename V::is_const_visitor >::type accept(V&, U const&);

      /** \brief Hook for visitor pattern */
      template < typename V, typename U >
      typename boost::disable_if < typename V::is_const_visitor >::type accept(V&, U&);

      /** \brief Pure virtual destructor */
      virtual ~base_visitable() = 0;
  };

  /**
   * \brief Interface for classes that should be visitable
   *
   * Classes can be made accessible to visitors by deriving from this class.
   * The type D represents the type of the derived class (curiously recurring template pattern)
   **/
  template < class D >
  class visitable {
    template < typename S, bool b >
    friend class visitor;

    public:

      /** \brief Hook for visitor pattern */
      template < typename V >
      typename boost::enable_if < typename V::is_const_visitor >::type accept(V&) const;

      /** \brief Hook for visitor pattern */
      template < typename V, typename U >
      typename boost::enable_if < typename V::is_const_visitor >::type accept(V&, U const&) const;

      /** \brief Hook for visitor pattern */
      template < typename V, typename U >
      typename boost::enable_if < typename V::is_const_visitor >::type accept(V&, U&) const;

      /** \brief Hook for visitor pattern */
      template < typename V >
      typename boost::disable_if < typename V::is_const_visitor >::type accept(V&);

      /** \brief Hook for visitor pattern */
      template < typename V, typename U >
      typename boost::disable_if < typename V::is_const_visitor >::type accept(V&, U const&);

      /** \brief Hook for visitor pattern */
      template < typename V, typename U >
      typename boost::disable_if < typename V::is_const_visitor >::type accept(V&, U&);

      /** \brief Pure virtual destructor */
      virtual ~visitable() = 0;
  };

  template < class D >
  template < typename V >
  inline typename boost::enable_if < typename V::is_const_visitor >::type visitable< D >::accept(V& v) const {

    v.visit(static_cast < D const& > (*this));
  }

  template < class D >
  template < typename V, typename U >
  inline typename boost::enable_if < typename V::is_const_visitor >::type visitable< D >::accept(V& v, U const& u) const {

    v.visit(static_cast < D const& > (*this), u);
  }

  template < class D >
  template < typename V, typename U >
  inline typename boost::enable_if < typename V::is_const_visitor >::type visitable< D >::accept(V& v, U& u) const {

    v.visit(static_cast < D const& > (*this), u);
  }

  template < class D >
  template < typename V >
  inline typename boost::disable_if < typename V::is_const_visitor >::type visitable< D >::accept(V& v) {

    v.visit(static_cast < D& > (*this));
  }

  template < class D >
  template < typename V, typename U >
  inline typename boost::disable_if < typename V::is_const_visitor >::type visitable< D >::accept(V& v, U const& u) {

    v.visit(static_cast < D& > (*this), u);
  }

  template < class D >
  template < typename V, typename U >
  inline typename boost::disable_if < typename V::is_const_visitor >::type visitable< D >::accept(V& v, U& u) {

    v.visit(static_cast < D& > (*this), u);
  }

  template < class D >
  inline visitable< D >::~visitable() {
  }

  template < typename V >
  inline typename boost::enable_if < typename V::is_const_visitor >::type base_visitable::accept(V& v) const {

  }

  template < typename V, typename U >
  inline typename boost::enable_if < typename V::is_const_visitor >::type base_visitable::accept(V& v, U const& u) const {

  }

  template < typename V, typename U >
  inline typename boost::enable_if < typename V::is_const_visitor >::type base_visitable::accept(V& v, U& u) const {

  }

  template < typename V >
  inline typename boost::disable_if < typename V::is_const_visitor >::type base_visitable::accept(V& v) {

  }

  template < typename V, typename U >
  inline typename boost::disable_if < typename V::is_const_visitor >::type base_visitable::accept(V& v, U const& u) {

  }

  template < typename V, typename U >
  inline typename boost::disable_if < typename V::is_const_visitor >::type base_visitable::accept(V& v, U& u) {

  }

  inline base_visitable::base_visitable() {
  }

  inline base_visitable::~base_visitable() {
  }
}
