#include <ticpp.h>

#include <sip/visitors.h>

namespace sip {

  class restore_visitor_impl :
                        public utility::visitor < restore_visitor_impl, false >,
                        private boost::noncopyable {

    friend class visitors;
    friend class restore_visitor;

    private:

      /** \brief Alias for the first base class */
      typedef utility::visitor < restore_visitor_impl, false > base;

    private:

      /** \brief Points to interface object */
      sip::restore_visitor&     m_interface;

    protected:

      /** \brief Points to the current element */
      ticpp::Element*  tree;

    protected:

      /** \brief Default constructor */
      restore_visitor_impl(restore_visitor&);

    private:

      /** \brief Changes the currently pointed to tree (FIXME this is a temporary solution) */
      restore_visitor_impl& visit_tree(ticpp::Element*);

    public:

      /** \brief Reads from parse tree */
      restore_visitor_impl(restore_visitor&, ticpp::Element* s);

    public:

      template < typename T >
      void visit(T&);

      template < typename T, typename U >
      void visit(T&, U&);
  };
}
