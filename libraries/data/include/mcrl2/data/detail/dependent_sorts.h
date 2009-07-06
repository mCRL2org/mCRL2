// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/dependent_sorts.h
/// \brief Compute sorts on which an expression depends

#include <set>

#include "mcrl2/data/find.h"

namespace mcrl2 {

  namespace data {
    /// \cond INTERNAL_DOCS
    namespace detail {

      template < typename Expression >
      class unique_traversal_condition {

        private:

          std::set< Expression > m_known;

        public:

          bool operator()(Expression const& e)
          {
            return m_known.insert(e).second;
          }

          unique_traversal_condition()
          { }
      };

      /// \brief Computes the set of sorts that depend on the sorts that are added
      template < typename Action >
      class dependent_sort_helper : public detail::selective_sort_traverser< dependent_sort_helper< Action >, unique_traversal_condition< sort_expression > >
      {
        friend class detail::sort_traverser< dependent_sort_helper >;

        typedef detail::selective_sort_traverser< dependent_sort_helper, unique_traversal_condition< sort_expression > > super;

        public:

          typedef std::set< sort_expression >::const_iterator iterator;
          typedef std::set< sort_expression >::const_iterator const_iterator;

        private:

          data_specification const&   m_specification;

          Action                      m_action;

        protected:

          using super::enter;

          void visit_constructors(basic_sort const& s)
          {
            for (data_specification::constructors_const_range r(m_specification.constructors(s)); !r.empty(); r.advance_begin(1))
            {
              if (r.front().sort().is_function_sort())
              {
                for (function_sort::domain_const_range i(function_sort(r.front().sort()).domain()); !i.empty(); i.advance_begin(1))
                {
                  if (i.front() != s && (!i.front().is_basic_sort() || m_specification.find_referenced_sort(i.front()) != s))
                  {
                    static_cast< super& >(*this)(i.front());
                  }
                }
              }
            }
          }

          void enter(const container_sort& s)
          {
            m_action(s);
          }

          void enter(const structured_sort& s)
          {
            m_action(s);
          }

          void enter(const basic_sort& s)
          {
            sort_expression actual_sort = m_specification.find_referenced_sort(s);

            if (actual_sort == s)
            {
              visit_constructors(s);

              m_action(s);
            }
            else
            {
              static_cast< super& >(*this)(actual_sort);
            }
          }

        public:

          // Alternative traversal for function_sort
          void operator()(const function_sort& s)
          {
            m_action(s);
            static_cast< super& >(*this)(s.domain());
          }

          template < typename Expression >
          void operator()(const Expression& e)
          {
            static_cast< super& >(*this)(e);
          }

          dependent_sort_helper(data_specification const& specification) :
                                                m_specification(specification)
          {}

          dependent_sort_helper(data_specification const& specification, Action action) :
                                                m_specification(specification), m_action(action)
          {}

          template < typename Sequence >
          void add(const Sequence& s, bool assume_self_dependence = false,
                       typename detail::enable_if_container< Sequence >::type* = 0)
          {
            for (typename Sequence::const_iterator i = s.begin(); i != s.end(); ++i)
            {
              add(*i, assume_self_dependence);
            }
          }

          /// \brief Adds the sorts on which a sort expression depends
          ///
          /// \param[in] s A sort expression.
          /// \param[in] assume_self_dependence add the sort as well as all dependent sorts
          /// \return All sorts on which s depends.
          void add(const sort_expression& s, bool assume_self_dependence = false)
          {
            if (assume_self_dependence) {
              m_action(s);
            }

            if (is_basic_sort(s) && m_specification.find_referenced_sort(s))
            {
              visit_constructors(s);
            }
            else
            {
              static_cast< super& >(*this)(s);
            }
          }

          template < typename Expression >
          void add(Expression const& s, bool assume_self_dependence = false, typename detail::disable_if_container< Expression >::type* = 0)
          {
            (*this)(s);
          }
      };
    } // namespace detail
    /// \endcond

    template < typename Container, typename OutputIterator >
    void find_dependent_sorts(data_specification const& specification, Container const& s, OutputIterator sink) {
      detail::dependent_sort_helper< detail::collect_action< sort_expression, OutputIterator > >(
		specification, detail::collect_action< sort_expression, OutputIterator >(sink)).add(s);
    }

    template < typename Container >
    std::set< sort_expression > find_dependent_sorts(data_specification const& specification, Container const& s) {
      std::set< sort_expression > result;

      find_dependent_sorts(specification, s, std::inserter(result, result.end()));

      return result;
    }
  } // namespace data
} // namespace mcrl2 

