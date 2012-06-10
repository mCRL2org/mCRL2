// Author(s): Yaroslav Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file alphabet_reduction.cpp

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <sstream>
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/process/print.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/process/alphabet_reduction.h"
#include "mcrl2/process/action_name_multiset.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::log;
using namespace mcrl2::data;

namespace mcrl2
{
  namespace process
  {
    namespace detail
    {

      static AFun afunPair;

      static ATermAppl Pair_allow(atermpp::term_list < core::identifier_string_list > ma1, process_identifier ma2)
      {
        return ATmakeAppl2(afunPair,(ATerm)(ATermList)ma1,(ATerm)(ATermAppl)ma2);
      }

      
      template < class T >
      static std::multiset <T> to_multiset(const atermpp::term_list <T> l)
      {
        std::multiset <T> s;
        for (typename atermpp::term_list <T> :: const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          s.insert(*i);
        }
        return s;
      }

      template < class T >
      static atermpp::term_list <T> to_list(const std::multiset <T> &l)
      {
        atermpp::term_list <T> r;
        for (typename std::multiset <T> :: const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          r=push_front(r,*i);
        }
        return r;
      }

      template < class T >
      static atermpp::term_list <T> gsaATsortList(atermpp::term_list <T> l)
      {
        return to_list(to_multiset(l));
      }

      template < class T >
      static atermpp::term_list <T> gsaATintersectList(atermpp::term_list <T> l, atermpp::term_list <T> m)
      {
        atermpp::term_list <T> r;
        for (typename atermpp::term_list <T>::const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          if (std::find(m.begin(),m.end(),*i)!=m.end())
          {
            r=push_front(r,*i);
          }
        }
        return reverse(r);
      }

      template < class T >
      static inline atermpp::term_list <T> remove_element(atermpp::term_list <T> l, T e)
      {
        atermpp::term_list <T> r;
        for(typename atermpp::term_list <T>::const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          if (*i!=e)
          {
            r=push_front(r,*i);
          }
        }
        return r;
      }

      template < class T >
      static bool gsaATisDisjoint(atermpp::term_list <T>  l, atermpp::term_list <T> m)
      {
        return (gsaATintersectList(l,m)).empty();
      }

      template <class T>
      static void gsaATindexedSetPutList(std::set < T > &m, atermpp::term_list <T> l)
      {
        //add l into m
        for (typename atermpp::term_list <T>::const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          m.insert(*i);
        }
      }

      static lps::action_label_list add_typeMA(identifier_string_list ma, sort_expression_list s)
      {
        //adds type s to all actions in ma
        lps::action_label_list r;
        for (identifier_string_list::const_iterator i=ma.begin(); i!=ma.end(); ++i)
        {
          r=push_front(r,lps::action_label(*i,s));
        }
        return gsaATsortList(r);
      }
 
      static atermpp::term_list < core::identifier_string_list > transform_list(const action_name_multiset_list l)
      {
        // Always insert empty list also.
        atermpp::term_list < core::identifier_string_list > result;
        result=push_front(result,core::identifier_string_list());

        for(action_name_multiset_list::const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          result=push_front(result,i->names());
        }
        return reverse(result);
      }

      static action_name_multiset_list transform_list_back(const atermpp::term_list < core::identifier_string_list > l)
      {
        action_name_multiset_list result;
        for(atermpp::term_list < core::identifier_string_list >::const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          // An action_name_multiset_list contains the empty multi_action by default, but it cannot be part of it.
          if (!i->empty())
          { 
            result=push_front(result,action_name_multiset(*i));
          }
        }
        return reverse(result);
      }

      template < class T >
      static atermpp::term_list<T> merge_list(atermpp::term_list<T> l, atermpp::term_list<T> m)
      {
        std::set < T> r;
        for (typename atermpp::term_list<T>::const_iterator i=m.begin(); i!=m.end(); ++i)
        {
          r.insert(*i);
        }
        for (typename atermpp::term_list<T>::const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          r.insert(*i);
        }

        atermpp::term_list<T> result;
        for (typename std::set<T>::const_iterator i=r.begin(); i!=r.end(); ++i)
        {
          result=push_front(result,*i);
        }
        return result;
      }

      template < class T >
        static atermpp::term_list<T>  list_minus(atermpp::term_list<T> l, atermpp::term_list<T> m)
        {
          if (m.empty())
          { 
            return l;
          }
          atermpp::term_list<T> n;
          for (typename atermpp::term_list<T>::const_iterator i=l.begin(); i!=l.end(); ++i)
          {
            if (std::find(m.begin(),m.end(),*i) == m.end()) // Not found
            {
              n = push_front(n,*i); 
            }
          }
          return reverse(n);
        }

      static lps::action_label_list list_minus_ignore_type(lps::action_label_list l, core::identifier_string_list m)
      {
        lps::action_label_list n;
        for (lps::action_label_list::const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          if (std::find(m.begin(),m.end(),i->name()) == m.end())
          {
            n = push_front(n,*i);
          }
        }
        return reverse(n);
      }

      static std::vector < std::multiset < identifier_string > > 
                 from_allow_list_to_allow_vector(const atermpp::term_list < core::identifier_string_list > V)
      {
        //returns the list of multiactions that are allowed
        std::vector < std::multiset < identifier_string > > m;
        for (atermpp::term_list < core::identifier_string_list >::const_iterator i=V.begin(); i!=V.end(); ++i)
        {
          m.push_back(detail::to_multiset(*i));
        }
        return m;
      }

      static atermpp::term_list < core::identifier_string_list > 
                 from_allow_vector_to_allow_list(const std::vector < std::multiset < identifier_string > > &V)
      {
        //returns the list of multiactions that are allowed
        atermpp::term_list < core::identifier_string_list > m;
        for (std::vector < std::multiset < identifier_string > > ::const_iterator i=V.begin(); i!=V.end(); ++i)
        {
          m=push_front(m,detail::to_list(*i));
        }
        return m;
      }

      static size_t get_max_allowed_length(atermpp::term_list < core::identifier_string_list > V)
      {
        //returns the length of the longest allowed multiaction (min 1).
        size_t m = 1;
        for (atermpp::term_list < core::identifier_string_list >::const_iterator i=V.begin(); i!=V.end(); ++i)
        {
          size_t c=i->size(); 
          if (c>m)
          {
            m=c;
          }
        }
        return m;
      }

      static size_t get_max_comm_length(communication_expression_list C)
      {
        //returns the length of the longest allowed multiaction (0 if unbounded).
        size_t m = 1;
        for (communication_expression_list::const_iterator i=C.begin(); i!=C.end(); ++i)
        {
          if (gsIsNil(i->name()))  // This should be removed.
          {
            return 0;
          }
          size_t l=i->action_name().names().size();
          if (l>m)
          {
            m=l;
          }
        }
        return m;
      }

      static atermpp::term_list < core::identifier_string_list > optimize_allow_list(atermpp::term_list < core::identifier_string_list > V, atermpp::term_list < core::identifier_string_list > ul)
      {
        //returns the subset of V that is in ul
        atermpp::term_list < core::identifier_string_list > m;
        for (atermpp::term_list < core::identifier_string_list >::const_iterator i=V.begin(); i!=V.end(); ++i)
        {
          if (std::find(ul.begin(),ul.end(),*i) != ul.end())
          {
            m = push_front(m,*i);
          }
        }
        return reverse(m);
      }

      static atermpp::term_list < core::identifier_string_list > sort_multiactions_allow(atermpp::term_list < core::identifier_string_list > V)
      {
        //sort the user defined multiactions in V
        atermpp::term_list < core::identifier_string_list > m;
        for (atermpp::term_list < core::identifier_string_list >::const_iterator i=V.begin(); i!=V.end(); ++i)
        {
          m = push_front(m,gsaATsortList(*i));
        }

        return reverse(m);
      }

      static communication_expression_list sort_multiactions_comm(communication_expression_list C)
      {
        //sort the user defined multiactions in C
        communication_expression_list m;
        for (communication_expression_list::const_iterator i=C.begin(); i!=C.end(); ++i)
        {
          action_name_multiset lhs=i->action_name(); 
          lhs=action_name_multiset(gsaATsortList(lhs.names()));
          m = push_front(m,communication_expression(lhs,i->name()));
        }

        return reverse(m);
      }

      // returns true if l1 united with l2 is a subset of m
      static bool sub_multiaction(
                     const std::multiset < identifier_string > &l1, 
                     const std::multiset < identifier_string > &l2, 
                     const std::multiset < identifier_string > &m)
      {
        std::multiset < identifier_string >::const_iterator l1_walker=l1.begin();
        std::multiset < identifier_string >::const_iterator l2_walker=l2.begin();
        for(std::multiset < identifier_string >::const_iterator m_walker=m.begin(); m_walker!=m.end(); m_walker++)
        {
          if (l1_walker==l1.end())
          { 
            if (l2_walker==l2.end())
            {
              return true;;
            }
            else
            {
              if (*m_walker==*l2_walker)
              {
                l2_walker++;  // Match found
              }
              // else if (*m_walker>*l2_walker)  ???? XXXX Why is > not available...
              else if (*l2_walker<*m_walker)
              {
                return false; // l2 contains an element not in m.
              }
            }
          }
          else
          {
            // l1 is not empty.
            // if (l2_walker==l2.end() || *l1_walker <= *l2_walker)
            if (l2_walker==l2.end() || !(*l2_walker < *l1_walker))
            {
              if (*m_walker==*l1_walker)
              {
                l1_walker++;  // Match found
              }
              // else if (*m_walker>*l1_walker)
              else if (*l1_walker<*m_walker)
              {
                return false; // l1 contains an element not in m.
              }
            }
            else // *l2_walker < *l1_walker
            {
              if (*m_walker==*l2_walker)
              {
                l2_walker++;  // Match found
              }
              // else if (*m_walker>*l2_walker)
              else if (*l2_walker<*m_walker)
              {
                return false; // l2 contains an element not in m.
              }
            }
          }
        }
        return l1_walker==l1.end() && l2_walker==l2.end(); // l1 union l2 is exactly equal to m.
      }
      
      // Return true iff the union of l1 and l2 is a subset of one of the elements of MActL.
      static bool sub_multiaction_list(
                     const std::multiset < identifier_string > &l1, 
                     const std::multiset < identifier_string > &l2, 
                     const std::vector < std::multiset < identifier_string > > &MActL)
      {
        // true if multiaction MAct is in a submultiaction of a multiaction from MActL (all untyped)
        for (std::vector < std::multiset < identifier_string > >::const_iterator i=MActL.begin(); i!=MActL.end(); ++i)
        {
          if (sub_multiaction(l1,l2,*i))
          {
            return true;
          }
        }
        return false;
      }

      static bool disjoint_multiaction(core::identifier_string_list MAct, 
                        atermpp::term_list < core::identifier_string_list > MActL)
      {
        // true if no part of multiaction MAct is in a submultiaction of a multiaction from MActL (all untyped)
        for (atermpp::term_list < core::identifier_string_list >::const_iterator i=MActL.begin(); i!=MActL.end(); ++i)
        {
          if (!gsaATisDisjoint(MAct,*i))
          {
            return false;
          }
        }
        return true;
      }

      static core::identifier_string_list apply_hide(core::identifier_string_list I, core::identifier_string_list MAct)
      {
        //apply hiding I to MAct
        core::identifier_string_list r;

        for (core::identifier_string_list::const_iterator i=MAct.begin(); i!=MAct.end(); ++i)
        {
          if (std::find(I.begin(),I.end(),*i)== I.end())
          {
            r=push_front(r,*i);
          }
        }
        return reverse(r);
      }

      static atermpp::term_list < core::identifier_string_list > extend_hide(
                    atermpp::term_list < core::identifier_string_list > V, 
                    core::identifier_string_list I, 
                    atermpp::term_list < core::identifier_string_list > L)
      {
        // Extend V to contain hidings of L with theta_I
        atermpp::term_list < core::identifier_string_list > r;
        for (atermpp::term_list < core::identifier_string_list >::const_iterator i=L.begin(); i!=L.end(); ++i)
        {
          core::identifier_string_list ma=*i;
          core::identifier_string_list maH=apply_hide(I,*i);
          if (((maH==core::identifier_string_list()) || 
                 std::find(V.begin(),V.end(),maH)!=V.end()) 
                       && std::find(r.begin(),r.end(),ma)==r.end())
          {
            r=push_front(r,ma);
          }
        }
        return reverse(r);
      }

      static core::identifier_string_list comm_lhs(communication_expression_list C)
      {
        core::identifier_string_list l;
        for (communication_expression_list::const_iterator i=C.begin(); i!=C.end(); ++i)
        {
          l = l + i->action_name().names();
        }
        return reverse(l);
      }

      static core::identifier_string_list comm_rhs(communication_expression_list C)
      {
        core::identifier_string_list l;
        for (communication_expression_list::const_iterator i=C.begin(); i!=C.end(); ++i)
        {
          assert(!gsIsNil(i->name()));
          l = push_front(l,i->name());
        }
        return reverse(l);
      }

      static bool can_split_comm(communication_expression_list C)
      {
        core::identifier_string_list lhs = comm_lhs(C);
        core::identifier_string_list rhs = comm_rhs(C);
        bool b = true;
        for (core::identifier_string_list::const_iterator i=lhs.begin(); i!=lhs.end(); ++i)
        {
          if (std::find(rhs.begin(),rhs.end(),*i) != rhs.end())
          {
            b = false;
            break;
          }
        }
        return b;
      }

      static lps::action_label_list apply_rename(lps::action_label_list l, rename_expression_list R)
      {
        //applies R to a multiaction l
        if (l.empty())
        {
          return l;
        }

        lps::action_label_list m;
        for (lps::action_label_list::const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          bool b=false;
          for (rename_expression_list::const_iterator j=R.begin(); j!=R.end(); ++j)
          {
            if (i->name()==j->source())
            {
              m = push_front(m,lps::action_label(j->target(),i->sorts())); 
              b=true;
              break;
            }
          }
          if (!b)
          {
            m = push_front(m,*i);
          }
        }
        return gsaATsortList(m);
      }

      static std::multiset < core::identifier_string > untypeMA_set(lps::action_label_list MAct)
      {
        std::multiset < core::identifier_string > result;
        for(lps::action_label_list::const_iterator i=MAct.begin(); i!=MAct.end(); ++i)
        {
          result.insert(i->name());
        }

        return result;
      }
    } // detail

    alphabet_reduction::action_label_list_list alphabet_reduction::filter_rename_list(action_label_list_list l, rename_expression_list R)
    {
      //apply R to all elements of l
      action_label_list_list m;
      for (action_label_list_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        m = push_front(m,detail::apply_rename(*i,R));
      }
      return reverse(m);
    }



    core::identifier_string_list alphabet_reduction::untypeMA(lps::action_label_list MAct)
    {
      if (MAct.empty())
      {
        return core::identifier_string_list();
      }

      if (untypes.count(MAct)>0)
      {
        return untypes[MAct];
      }

      core::identifier_string_list r=push_front(untypeMA(pop_front(MAct)),MAct.front().name());
      r=detail::gsaATsortList(r);
      untypes[MAct]=r;
      return r;
    }

    atermpp::term_list < core::identifier_string_list > alphabet_reduction::untypeMAL(action_label_list_list LMAct)
    {
      //returns List of "untyped multiaction name" of List(MAct)
      std::set < core::identifier_string_list > name_set;
      for (action_label_list_list::const_iterator i=LMAct.begin(); i!=LMAct.end(); ++i)
      {
        name_set.insert(untypeMA(*i));
      }

      atermpp::term_list < core::identifier_string_list > R;
      for(std::set < core::identifier_string_list >::const_iterator j=name_set.begin(); j!=name_set.end(); ++j)
      { 
        R=push_front(R,*j);
      }
      return R;

    }

    template <class T>
    atermpp::term_list < T > alphabet_reduction::sync_mact(atermpp::term_list < T > a, atermpp::term_list < T > b)
    {
      return detail::gsaATsortList(a+b);
    }


    alphabet_reduction::action_label_list_list alphabet_reduction::filter_block_list(
        alphabet_reduction::action_label_list_list l, core::identifier_string_list H)
    {
      //filters l not to contain untyped actions from H

      action_label_list_list m;

      for (action_label_list_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        bool b = true;
        core::identifier_string_list ma = untypeMA(*i);
        for (core::identifier_string_list::const_iterator j=H.begin(); j!=H.end(); ++j)
        {
          if (std::find(ma.begin(),ma.end(),*j) != ma.end())
          {
            b = false;
            break;
          }
        }
        if (b)
        {
          m = push_front(m,*i);
        }
      }
      return reverse(m);
    }

    alphabet_reduction::action_label_list_list alphabet_reduction::filter_hide_list(action_label_list_list l, core::identifier_string_list I)
    {
      //filters l renaming untyped actions from I to tau
      action_label_list_list m;
      for (action_label_list_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        lps::action_label_list new_ma;
        lps::action_label_list ma= *i;
        for (lps::action_label_list::const_iterator j=ma.begin(); j!=ma.end(); ++j)
        {
          if (std::find(I.begin(),I.end(),j->name())== I.end())
          {
            new_ma=push_front(new_ma,*j);
          }
        }
        if (new_ma.size()>0 && std::find(m.begin(),m.end(),new_ma)==m.end())
        {
          m=push_front(m,reverse(new_ma));
        }
      }
      return reverse(m);
    }

    alphabet_reduction::action_label_list_list alphabet_reduction::filter_allow_list(
              action_label_list_list l, 
              atermpp::term_list < core::identifier_string_list > V)
    {
      //filters l to contain only multiactions matching the untyped multiactions from V
      action_label_list_list m;
      for (action_label_list_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        if (std::find(V.begin(),V.end(),untypeMA(*i)) != V.end())
        {
          m = push_front(m,*i);
        }
      }
      return reverse(m);
    }

    atermpp::term_list < core::identifier_string_list > alphabet_reduction::split_allow(
           atermpp::term_list < core::identifier_string_list > V, 
           atermpp::term_list < core::identifier_string_list > ulp, 
           atermpp::term_list < core::identifier_string_list > ulq)
    {
      //splits V according to the 2 alphabets (see paper) and returns the first part.
      atermpp::term_list < core::identifier_string_list > m;

      std::set < core::identifier_string_list > VV;
      for (atermpp::term_list < core::identifier_string_list >::const_iterator i=V.begin(); i!=V.end(); ++i)
      {
        VV.insert(*i);
      }

      for (atermpp::term_list < core::identifier_string_list >::const_iterator i=ulp.begin(); i!=ulp.end(); ++i)
      {
        core::identifier_string_list ma=*i;
        for (atermpp::term_list < core::identifier_string_list >::const_iterator j=ulq.begin(); j!=ulq.end(); ++j)
        {
          if (VV.count(sync_mact(*i,*j))>0)
          {
            m = push_front(m,ma);
            break;
          }
        }
      }
      return reverse(m);
    }

    atermpp::term_list < core::identifier_string_list > alphabet_reduction::sync_list(
           const atermpp::term_list < core::identifier_string_list > l,
           const atermpp::term_list < core::identifier_string_list > m)
    {
      std::set < core::identifier_string_list > all_set;
      for (atermpp::term_list < core::identifier_string_list >::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        for (atermpp::term_list < core::identifier_string_list >::const_iterator j=m.begin(); j!=m.end(); ++j)
        {
          std::multiset < core::identifier_string > ma;
          std::merge(i->begin(),i->end(),j->begin(),j->end(),inserter(ma,ma.begin()));
          all_set.insert(detail::to_list(ma));
        }
      }
      atermpp::term_list < core::identifier_string_list > result;
      for (std::set <  core::identifier_string_list >::const_iterator i=all_set.begin(); i!=all_set.end(); ++i)
      {
        result=push_front(result,*i);
      }
      return result;
    }
           
           
    alphabet_reduction::action_label_list_list alphabet_reduction::sync_list(
           const action_label_list_list l, 
           const action_label_list_list m, 
           size_t length/*=0*/, 
           const std::vector < std::multiset < identifier_string > > &allowed/* std::vector < std::multiset < identifier_string > >() */)
    {
      std::vector < std::multiset < identifier_string > > m_untyped;      
      for (action_label_list_list::const_iterator j=m.begin(); j!=m.end(); ++j)
      {
        m_untyped.push_back(detail::untypeMA_set(*j));
      }
      
      std::set < lps::action_label_list > all_set;
      for (action_label_list_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        const std::multiset < identifier_string > i_untyped = detail::untypeMA_set(*i);
        std::vector < std::multiset < identifier_string > >::const_iterator j_untyped=m_untyped.begin();
        for (action_label_list_list::const_iterator j=m.begin(); j!=m.end(); ++j,++j_untyped)
        {
          if (length==0 || i_untyped.size()+j_untyped->size()<=length)
          {
            if (allowed.empty() || detail::sub_multiaction_list(i_untyped,*j_untyped,allowed))
            {
              std::multiset < lps::action_label > ma;
              std::merge(i->begin(),i->end(),j->begin(),j->end(),inserter(ma,ma.begin()));
              all_set.insert(detail::to_list(ma));
            }
          }
        }
      }
      action_label_list_list result;
      for (std::set < lps::action_label_list > :: const_iterator i=all_set.begin(); i!=all_set.end(); ++i)
      {
        result=push_front(result,*i);
      }
      return result;
    }

    /* return a list with the actions of l1, l2 and all the multiactions of l1 and l2 */
    alphabet_reduction::action_label_list_list alphabet_reduction::sync_list_ht(
                        action_label_list_list l1, 
                        action_label_list_list l2, 
                        bool including_products_of_actions/* =true */)
    {
      std::set < lps::action_label_list > all_set;
      for (action_label_list_list::const_iterator i=l1.begin(); i!=l1.end(); ++i)
      {
        all_set.insert(*i);
      }

      for (action_label_list_list::const_iterator j=l2.begin(); j!=l2.end(); ++j)
      {
        all_set.insert(*j);
      }
      
      if (including_products_of_actions)
      {
        //put the synchronization of l1 and l2 into m (if length, then not longer than length)
      
        for (action_label_list_list::const_iterator i=l1.begin(); i!=l1.end(); ++i)
        {
          for (action_label_list_list::const_iterator j=l2.begin(); j!=l2.end(); ++j)
          {
            all_set.insert(sync_mact(*i,*j));
          }
        }
      }
      action_label_list_list result;
      for (std::set < lps::action_label_list > :: const_iterator i=all_set.begin(); i!=all_set.end(); ++i)
      {
        result=push_front(result,*i);
      }
      return result;
    }

    atermpp::term_list< identifier_string_list > alphabet_reduction::apply_unrename(
                  core::identifier_string_list l, 
                  rename_expression_list R)
    {
      //applies R^{-1} to a multiaction l, returns a list of multiactions.

      atermpp::term_list< identifier_string_list > m=push_front(atermpp::term_list< identifier_string_list >(),identifier_string_list());
      if (l.empty())
      {
        return m;
      }

      for (core::identifier_string_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        core::identifier_string_list temp;
        for (rename_expression_list::const_iterator j=R.begin(); j!=R.end(); ++j)
        {
          if (*i == j->target())
          {
            temp=push_front(temp,j->source());
          }
        }
        if (temp.empty())
        {
          temp=push_front(temp,*i);    
        }
        m = sync_list(m,push_front(atermpp::term_list< identifier_string_list > (),temp)); 
      }
      return m;
    }

    atermpp::term_list < core::identifier_string_list > alphabet_reduction::apply_unrename_allow_list(atermpp::term_list < core::identifier_string_list > V, rename_expression_list R)
    {
      //applies R^{-1} to a multiaction V, returns a list V1 -- also allow-list.

      atermpp::term_list< identifier_string_list > m;
      if (V.empty())
      {
        return V;
      }

      for (atermpp::term_list < core::identifier_string_list >::const_iterator i=V.begin(); i!=V.end(); ++i)
      {
        m=detail::merge_list(m,apply_unrename(*i,R));  
      }

      return m;
    }

    // Establishes the possible multi_actions that can be done if the communications
    // in C are applied to the multi_action in l. If C constains a|b->c and l
    // contains actions a,b and d, then the result is typically {<a,b,d>,<c,d>}.
    
    alphabet_reduction::action_label_list_list alphabet_reduction::apply_comms(
                    lps::action_label_list l, 
                    communication_expression_list C)
    {
      
      //filter out actions not in the lhs of C;
      //split the rest of l to a composition of subactions of a similar type
      //to those apply a simplified procedure??

      lps::action_label_list ll=detail::list_minus_ignore_type(l,detail::comm_lhs(C));
      if (detail::gsaATsortList(l)==detail::gsaATsortList(ll))
      {
        return push_front(action_label_list_list(),l);  //C does not apply
      }
      
      l=detail::list_minus(l,ll);  //apply to the rest

      //Gives all possible results of application of C to a multiaction l.
      //Explanation: applying {a:Nat|b:Nat-c:Nat} to a|b can either give c, or a|b,
      //depending on the parameters of a and b. (in case a,b have no parameters,
      //the result is definitely c).
      //So, the result is an alphabet, not a single multiaction

      action_label_list_list m=push_front(action_label_list_list(),lps::action_label_list());
      lps::action_label_list r=l;
      while (r.size() > 0)
      {
        lps::action_label a = r.front();  
        r = pop_front(r);
        bool applied=false;
        for (communication_expression_list::const_iterator i=C.begin(); i!=C.end(); ++i)
        {
          const core::identifier_string_list c = i->action_name().names();
          if (std::find(c.begin(),c.end(),a.name()) !=c.end())
          {
            const sort_expression_list s = a.sorts();
            lps::action_label_list tr = r;
            bool b=true;
            const core::identifier_string_list c1 = remove_one_element(c,a.name());
            for (core::identifier_string_list::const_iterator j=c1.begin(); j!=c1.end(); ++j)
            {
              lps::action_label act(*j,s);
              if (std::find(tr.begin(),tr.end(),act) != tr.end())
              {
                tr = remove_one_element(tr,act);
              }
              else
              {
                b = false;
                break;
              }
            }
            if (b)    //can apply c -- no other c can be applied to a multiaction containing "a" (rules for C)
            {
              applied=true;
              r = tr;
              identifier_string rhs_c=i->name();
              action_label_list_list tm;
              if (!s.empty())
              {
                tm=push_front(action_label_list_list(),detail::add_typeMA(c,s));
              }
              assert(!gsIsNil(rhs_c));
              tm=detail::merge_list(tm,push_front(action_label_list_list(),
                                      push_front(lps::action_label_list(),lps::action_label(rhs_c,s))));
              m=sync_list(m,tm);
              break;
            }
          }
        }
        if (!applied)
        {
          m=sync_list(m,push_front(action_label_list_list(),push_front(lps::action_label_list(),a)));
        }
      }

      if (!r.empty())
      {
        m=sync_list(m,push_front(action_label_list_list(),r));
      } 

      if (!ll.empty())
      {
        m=sync_list(push_front(action_label_list_list(),ll),m);
      } 

      return m;

    }

    atermpp::term_list < core::identifier_string_list > alphabet_reduction::extend_allow_comm_with_alpha(
             atermpp::term_list < core::identifier_string_list > V, 
             communication_expression_list C, 
             action_label_list_list l)
    {
      //Extend V to V1 so that \allow_V(\com_C(x))=\allow_V(\com_C(\allow_V1(x))) where l is the set of multiactions of x
      //the result is between l and empty set of multiactions. Only those ma in l are kept that C(ma)\cap V != {}

      //make V a list of multiactions actions
      atermpp::term_list < core::identifier_string_list> nV;
      for (atermpp::term_list < core::identifier_string_list >::const_iterator i=V.begin(); i!=V.end(); ++i)
      {
        nV=push_front(nV,*i);
      }
      nV=reverse(nV);
      
      nV=push_front(nV,core::identifier_string_list()); //to include possible communications to tau

      atermpp::term_list < core::identifier_string_list > r;
      for (action_label_list_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        core::identifier_string_list ma=untypeMA(*i);
        if (std::find(r.begin(),r.end(),ma)==r.end())
        {
          atermpp::term_list < core::identifier_string_list > mas=untypeMAL(apply_comms(*i,C));
          if (!detail::gsaATisDisjoint(nV,mas))
          {
            r=push_front(r,ma);
          }
        }
      }
      return reverse(r);
    }

    atermpp::term_list < core::identifier_string_list > alphabet_reduction::extend_allow_comm(atermpp::term_list < core::identifier_string_list > V, communication_expression_list C)
    {
      //Extend V to V1 so that \allow_V(\com_C(x))=\allow_V(\com_C(\allow_V1(x)))
      //the result is between l and empty set of multiactions. Only those ma in l are kept that C(ma) in V


      //create a table with the reverse mappings of the actions in ran(C)
      std::map < identifier_string, atermpp::term_list < core::identifier_string_list > > rev;

      for (communication_expression_list::const_iterator i=C.begin(); i!=C.end(); ++i)
      {
        identifier_string target=i->name();
        if (gsIsNil(target))
        {
          continue;
        }
        atermpp::term_list < core::identifier_string_list > cur;
        if (rev.count(target)>0)
        {
          cur=rev[target];
        }
        rev[target]=push_front(cur,i->action_name().names());
      }

      // for all elements of V get a set of multiactions using the reverse mapping.
      std::set < identifier_string_list > m;

      for (atermpp::term_list < core::identifier_string_list >::const_iterator i=V.begin(); i!=V.end(); ++i)
      {
        m.insert(*i);
      }

      for (atermpp::term_list < core::identifier_string_list >::const_iterator i=V.begin(); i!=V.end(); ++i)
      {
        core::identifier_string_list v=*i;
        atermpp::term_list < identifier_string_list > res=push_front(atermpp::term_list < identifier_string_list >(),identifier_string_list()); 
        for (core::identifier_string_list::const_iterator j=v.begin(); j!=v.end(); ++j)
        {
          atermpp::term_list < core::identifier_string_list >  r;
          if (rev.count(*j)>0)
          {
            r=detail::merge_list(rev[*j],push_front(atermpp::term_list < core::identifier_string_list >(),
                                             push_front(core::identifier_string_list(),*j)));
          }
          else
          {
            r=push_front(atermpp::term_list < core::identifier_string_list >(),push_front(core::identifier_string_list(),*j));
          }
          res=sync_list(res,r);
        }
        detail::gsaATindexedSetPutList(m,res);
      }

      atermpp::term_list< identifier_string_list > l;
      for(std::set < identifier_string_list >::const_iterator i=m.begin(); i!=m.end(); ++i)
      {
        l=push_front(l,*i);
      }

      return l;
    }

    alphabet_reduction::action_label_list_list alphabet_reduction::filter_comm_list(
              action_label_list_list l, 
              communication_expression_list C)
    {

      //apply C to all elements of l

      std::set < lps::action_label_list > m;

      for (action_label_list_list::const_iterator i=l.begin(); i!=l.end(); ++i)
      {
        action_label_list_list mas=apply_comms(*i,C);
        mas=remove_one_element(mas,lps::action_label_list());
        detail::gsaATindexedSetPutList(m,mas);
      }

      action_label_list_list l1;
      for(std::set < lps::action_label_list >::const_iterator i=m.begin(); i!=m.end(); ++i)
      {
        l1=push_front(l1,*i);
      }
      return l1;
    }

    process_expression alphabet_reduction::PushBlock(core::identifier_string_list H, process_expression a)
    {
      if (is_delta(a) || is_tau(a))
      {
        return a;
      }
      else if (lps::is_action(a))
      {
        return std::find(H.begin(),H.end(),action(a).label().name())!=H.end()?delta():a;
      }
      else if (is_process_instance(a) || is_process_instance_assignment(a))
      {
        action_label_list_list l=alphas.count(a)>0?alphas[a]:gsaGetAlpha(a);
        l = filter_block_list(l,H);
        // XXX also adjust H

        a = block(H,a);

        alphas[a]=l;

        return a;
      }
      else if (is_block(a))
      {
        return PushBlock(detail::merge_list(H,block(a).block_set()),block(a).operand());
      }
      else if (is_hide(a))
      {
        process_expression p = hide(a).operand();

        H = detail::list_minus(H,hide(a).hide_set());

        action_label_list_list l = alphas.count(a)>0?alphas[a]:gsaGetAlpha(a);
        l = filter_block_list(l,H);

        p = PushBlock(H,p);

        a = hide(hide(a).hide_set(),p);

        alphas[a]=l;

        return a;
      }
      else if (is_rename(a))
      {
        // XXX
        action_label_list_list l = alphas.count(a)>0?alphas[a]:gsaGetAlpha(a);
        a = block(H,a);
        alphas[a]=l;
        return a;
      }
      else if (is_comm(a))
      {
        communication_expression_list C = detail::sort_multiactions_comm(comm(a).comm_set());
        core::identifier_string_list lhs = detail::comm_lhs(C);
        core::identifier_string_list rhs = detail::comm_rhs(C);
        core::identifier_string_list Ha;
        core::identifier_string_list Hc;

        for (core::identifier_string_list::const_iterator i=H.begin(); i!=H.end(); ++i)
        {
          if ((std::find(lhs.begin(),lhs.end(),*i) != lhs.end()) || 
                 (std::find(rhs.begin(),rhs.end(),*i) != rhs.end()))
          {
            Ha = push_front(Ha,*i);
          }
          else
          {
            Hc = push_front(Hc,*i);
          }
        }

        if (!Hc.empty())
        {
          a = PushBlock(Hc,comm(a).operand());
          a = comm(C,a);
        }

        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);  // Unclear how to deal with this case if it occurs.
        action_label_list_list l = alphas[a];

        if (!Ha.empty())
        {
          a = block(Ha,a);
          alphas[a]=filter_block_list(l,Ha);
        }
        return a;
      }
      else if (is_allow(a))
      {
        //XXX
        action_label_list_list l = alphas.count(a)>0?alphas[a]:gsaGetAlpha(a);
        a = gsApplyAlpha(a);
        a = block(H,a);
        alphas[a]=l;
        return a;
      }
      else if (is_sum(a) || is_at(a) || is_choice(a) || is_seq(a) ||
               is_if_then(a) || is_if_then_else(a) || is_sync(a) || 
               is_merge(a) || is_left_merge(a) || is_bounded_init(a))
      {
        // Do not distribute over these operator.
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = block(H,a);
        alphas[a]=l;
        return a;
      }

      assert(0);
      return process_expression();
    }

    process_expression alphabet_reduction::PushHide(core::identifier_string_list I, process_expression a)
    {
      if (is_delta(a) || is_tau(a))
      {
        return a;
      }
      else if (lps::is_action(a))
      {
        return (std::find(I.begin(),I.end(),action(a).label().name())!=I.end())?tau():a;
      }
      else if (is_process_instance(a) || is_process_instance_assignment(a))
      {
        action_label_list_list l = alphas.count(a)>0?alphas[a]:gsaGetAlpha(a);
        l = filter_hide_list(l,I);
        // XXX also adjust I?

        a = gsApplyAlpha(a);

        a = hide(I,a);

        alphas[a]=l;

        return a;
      }
      else if (is_block(a))
      {
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = hide(I,a);
        alphas[a]=filter_hide_list(l,I);
        return a;
      }
      else if (is_hide(a))
      {
        return PushHide(detail::merge_list(I,hide(a).hide_set()),hide(a).operand());
      }
      else if (is_rename(a))
      {
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = hide(I,a);
        alphas[a]=filter_hide_list(l,I);
        return a;
      }
      else if (is_comm(a))
      {
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = hide(I,a);
        alphas[a]=filter_hide_list(l,I);
        return a;
      }
      else if (is_allow(a))
      {
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = hide(I,a);
        alphas[a]=filter_hide_list(l,I);
        return a;
      }
      else if (is_sum(a) || is_at(a) || is_choice(a) || is_seq(a) ||
               is_if_then(a) || is_if_then_else(a) || is_sync(a) || 
               is_merge(a) || is_left_merge(a) || is_bounded_init(a))
      {
        // Distributing hide over these operators disallows the
        // linearizer to work properly.

        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = hide(I,a);
        alphas[a]=l;
        return a;
      }
      assert(0);
      return process_expression(); //to suppress warnings
    }

    process_expression alphabet_reduction::PushAllow(atermpp::term_list < core::identifier_string_list > V, process_expression a)
    {
      V=detail::sort_multiactions_allow(V);
      if (is_delta(a) || is_tau(a))
      {
        return a;
      }
      else if (lps::is_action(a))
      {
        if (std::find(V.begin(),V.end(),
                 push_front(core::identifier_string_list(),lps::action(a).label().name()))==V.end())
        {
          return delta();
        }
        return gsApplyAlpha(a);
      }
      else if (is_process_instance(a) || is_process_instance_assignment(a))
      {
        process_identifier pn=is_process_instance(a)?process_instance(a).identifier():
          process_instance_assignment(a).identifier();
        bool full_alpha_know=true;
        action_label_list_list l;
        if (alphas_process_identifiers.count(pn)==0)
        {
          size_t max_len=detail::get_max_allowed_length(V);
          l = gsaGetAlpha(a,max_len,detail::from_allow_list_to_allow_vector(V));    // ZZZZZZZ
          full_alpha_know=false;
        }
        else
        {
          l=alphas_process_identifiers[pn];
          alphas[a]=l;
        }


        action_label_list_list ll=l;
        l = filter_allow_list(ll,V);
        if (full_alpha_know && detail::gsaATsortList(l)==detail::gsaATsortList(ll))         //everything from alpha(a) is allowed by V -- no need in allow
        {
          alphas[a]=l; // not to forget: put the list in the table!!!
          return a;
        }

        atermpp::term_list < core::identifier_string_list > ul=untypeMAL(l);
        V = detail::optimize_allow_list(V,ul);

        // here we create (in case pn is not recursive) a new process equation to replace allow(V,a);
        // we call it pn_allow_i, where i is such that pn_allow_i is a fresh process name.
        // the parameters are the same as in pn.
        // ADDITION 2006-09-11: if this is a pCRL process we don't do this (not to break the current linearizer)

        if (non_recursive_set.count(pn)>0 && pCRL_set.count(pn)==0)
        {
          process_identifier new_pn;
          if (subs_alpha.count(detail::Pair_allow(V,pn))==0)
          {
            //create a new
            //process name with type pn, add _i
            short i=1;
            do
            {
              std::stringstream name;
              name << core::pp(pn.name()) << "_allow_" << i;
              new_pn =process_identifier(identifier_string(name.str()),pn.sorts());
              i++;
            }
            while (procs.count(new_pn)>0);

            mCRL2log(verbose) << "- created process " << pp(new_pn) << "\n";
            process_expression p=procs[pn];
            assert(p!=process_expression());
            p=PushAllow(V,p);

            procs[new_pn]=p;
            // Copy properties
            if (pCRL_set.count(pn)>0) pCRL_set.insert(new_pn);
            if (mCRL_set.count(pn)>0) mCRL_set.insert(new_pn);
            if (recursive_set.count(pn)>0) recursive_set.insert(new_pn);
            if (non_recursive_set.count(pn)>0) non_recursive_set.insert(new_pn);
            l=alphas[p];
            alphas_process_identifiers[new_pn]=l;

            // we save both direct and reverse mappings
            subs_alpha[detail::Pair_allow(V,pn)]=new_pn;
            subs_alpha_rev[new_pn]=pn;
          }
          else
          {
            new_pn=subs_alpha[detail::Pair_allow(V,pn)];
          }
          if (is_process_instance(a))
          {
            a=process_instance(new_pn,process_instance(a).actual_parameters());
          }
          else
          { 
            a=process_instance_assignment(new_pn,process_instance_assignment(a).assignments());
          }
          alphas[a]=l;
        }
        else
        {
          if (non_recursive_set.count(pn)>0 && pCRL_set.count(pn)>0)
          {
            mCRL2log(warning) << "an allow operation allowing only the (multi-)action(s) from " 
              << pp(detail::transform_list_back(V)) << std::endl
              << "is applied to sequential non-directly-recursive process " << pp(pn) << "." << std::endl
              << "This disallows (multi-)action(s) " 
              << pp(detail::transform_list_back(detail::list_minus(untypeMAL(ll),V)))
              << "of this process." << std::endl
              << "This warning could also indicate a forgotten (multi-)action in this allow operation." << std::endl << std::endl;
          }

          a = allow(detail::transform_list_back(V),a);
          alphas[a]=filter_allow_list(l,V);
        }

        return a;
      }
      else if (is_block(a))
      {
        core::identifier_string_list H=block(a).block_set();
        process_expression p=block(a).operand();

        p = PushAllow(V,p);
        assert(alphas.count(p)>0);
        action_label_list_list l = alphas[p];
        a = block(H,p);
        alphas[a]=filter_block_list(l,H);
        return a;
      }
      else if (is_hide(a))
      {
        core::identifier_string_list I=hide(a).hide_set();
        process_expression p=hide(a).operand();

        action_label_list_list l = alphas.count(p)>0?alphas[p]:gsaGetAlpha(p);
        atermpp::term_list < core::identifier_string_list > V1 = detail::extend_hide(V,I,untypeMAL(l));

        p = PushAllow(V1,p);

        assert(alphas.count(p)>0); 
        l = alphas[p];
        a = hide(hide(a).hide_set(),p);
        alphas[a]=filter_hide_list(l,I);
        return a;
      }
      else if (is_rename(a))
      {
        rename_expression_list R=rename(a).rename_set();
        process_expression p=rename(a).operand();

        atermpp::term_list < core::identifier_string_list >  V1 = apply_unrename_allow_list(V,R);

        p = PushAllow(V1,p);

        assert(alphas.count(p)>0);
        action_label_list_list l = alphas[p];
        a = rename(rename(a).rename_set(),p);
        alphas[a]=filter_rename_list(l,R);
        return a;
      }
      else if (is_allow(a))
      {
        a=PushAllow(detail::gsaATintersectList(V,detail::sort_multiactions_allow(detail::transform_list(allow(a).allow_set()))),
                         allow(a).operand());
        return a;
      }
      else if (is_comm(a))
      {
        
        communication_expression_list C=comm(a).comm_set();
        C=detail::sort_multiactions_comm(C);
         

        process_expression p=comm(a).operand();
        atermpp::term_list < core::identifier_string_list >  V1;

        action_label_list_list l;
        if (alphas.count(p)==0)
        {
          V1=extend_allow_comm(V,C);
        }
        else
        {
          l=alphas[p];
          V1 = extend_allow_comm_with_alpha(V,C,l);
        }
        p = PushAllow(V1,p);

        assert(alphas.count(p)>0);
        l = alphas[p];
        l = filter_comm_list(l,C);
        a = comm(comm(a).comm_set(),p);
        a = gsApplyAlpha(a);
        alphas[a]=l;

        {
          action_label_list_list ll=l;
          l = filter_allow_list(ll,V);
          if (detail::gsaATsortList(l)==detail::gsaATsortList(ll))
          {
            return a;  //everything from alpha(a) is allowed by V -- no need in allow
          }
        }

        V = detail::optimize_allow_list(V,untypeMAL(l));
        if (is_allow(a))
        {
          push_comm_through_allow=false;
          a = allow(detail::transform_list_back(V),a);
          a = gsApplyAlpha(a);
        }
        else
        {
          a = allow(detail::transform_list_back(V),a);
        }

        alphas[a]=filter_allow_list(l,V);
        return a;
      }
      else if (is_merge(a))
      {
        process_expression p = merge(a).left();
        process_expression q = merge(a).right();

        {
          atermpp::term_list < core::identifier_string_list > Vp,Vq;

          {
            size_t max_len=detail::get_max_allowed_length(V);
            const std::vector < std::multiset < identifier_string > > allowed=detail::from_allow_list_to_allow_vector(V);

            action_label_list_list lp=alphas.count(p)==0?gsaGetAlpha(p,max_len,allowed):alphas[p];    // ZZZZZ
            action_label_list_list lq=alphas.count(q)==0?gsaGetAlpha(q,max_len,allowed):alphas[q];

            atermpp::term_list < core::identifier_string_list > ulp = untypeMAL(lp);
            atermpp::term_list < core::identifier_string_list > ulq = untypeMAL(lq);

            Vp=detail::merge_list(V,split_allow(V,ulp,ulq));
            Vq=detail::merge_list(V,split_allow(V,ulq,ulp));
          }
          p=PushAllow(Vp,p);
          q=PushAllow(Vq,q);
        }

        assert(alphas.count(p)>0);
        action_label_list_list l=alphas[p];
        assert(alphas.count(q)>0);
        action_label_list_list l2=alphas[q];
        l=detail::merge_list(detail::merge_list(l,l2),sync_list(l,l2));
        a=merge(p,q);

        {
          action_label_list_list ll=l;
          l = filter_allow_list(ll,V);
          if (detail::gsaATsortList(l)==detail::gsaATsortList(ll))  //everything from alpha(a) is allowed by V -- no need in allow
          {
            alphas[a]=l;
            return a;
          }
        }

        V = detail::optimize_allow_list(V,untypeMAL(l));
        a = allow(detail::transform_list_back(V),a);
        assert(l!=aterm());
        alphas[a]=l;
        return a;
      }
      else if (is_sync(a)  || is_left_merge(a) ||
               is_sum(a) || is_at(a) || is_choice(a) || is_seq(a) ||
               is_if_then(a) || is_if_then_else(a) || is_bounded_init(a))
      {
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = allow(detail::transform_list_back(V),a);
        alphas[a]=l;
        return a;
      }
      assert(0);
      return process_expression(); //to suppress warnings
    }

    process_expression alphabet_reduction::PushComm(communication_expression_list C, process_expression a)
    {
      C=detail::sort_multiactions_comm(C);
      if (is_delta(a) || is_tau(a) || lps::is_action(a))
      {
        return a;
      }
      else if (is_process_instance(a) || is_process_instance_assignment(a))
      {
        action_label_list_list l = alphas.count(a)>0?alphas[a]:gsaGetAlpha(a);

        l = filter_comm_list(l,C);
        // XXX also adjust C?

        a = comm(C,a);

        alphas[a]=l;

        return a;
      }
      else if (is_block(a))
      {
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = comm(C,a);
        alphas[a]=filter_comm_list(l,C);
        return a;
      }
      else if (is_hide(a))
      {
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = comm(C,a);
        alphas[a]=filter_comm_list(l,C);
        return a;
      }
      else if (is_rename(a))
      {
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = comm(C,a);
        alphas[a]=filter_comm_list(l,C);
        return a;
      }
      else if (is_comm(a))
      {
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = comm(C,a);
        alphas[a]=filter_comm_list(l,C);
        return a;
      }
      else if (is_allow(a))
      {
        a = gsApplyAlpha(a);
        if (!is_allow(a)) /* call ourselves recursively */
        {
          return PushComm(C,a);
        }

        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];

        if (push_comm_through_allow)
        {
          atermpp::term_list < core::identifier_string_list > V = detail::transform_list(allow(a).allow_set());
          atermpp::term_list < core::identifier_string_list > V2=extend_allow_comm(V,C);
          if (detail::gsaATsortList(V)==detail::gsaATsortList(V2))
          {
            for (action_label_list_list::const_iterator lt=l.begin(); lt!=l.end(); ++lt)
            {
              atermpp::term_list < core::identifier_string_list > mas=untypeMAL(apply_comms(*lt,C));
              V2=detail::merge_list(V2,mas);
            }
            process_expression p=allow(a).operand();
            p=PushComm(C,p);
            assert(alphas.count(p)>0);
            action_label_list_list l1=alphas[p];
            a=allow(detail::transform_list_back(V2),p);
            alphas[a]=filter_allow_list(l1,V2);
            return a;
          }
        }
        a = comm(C,a);
        alphas[a]=filter_comm_list(l,C);
        return a;
      }
      else if (is_merge(a))
      {
        if (detail::can_split_comm(C))
        {
          process_expression p = merge(a).left();
          process_expression q = merge(a).right();

          action_label_list_list lp=alphas.count(p)>0?alphas[p]:gsaGetAlpha(p);
          action_label_list_list lq=alphas.count(q)>0?alphas[q]:gsaGetAlpha(q);

          atermpp::term_list < core::identifier_string_list > ulp = untypeMAL(lp);
          atermpp::term_list < core::identifier_string_list > ulq = untypeMAL(lq);
          communication_expression_list Cp;
          communication_expression_list Cq;
          communication_expression_list Ca;
          action_label_list_list l;

          for (communication_expression_list::const_iterator i=C.begin(); i!=C.end(); ++i)
          {
            identifier_string_list lhs=i->action_name().names();
            bool bp = detail::disjoint_multiaction(lhs,ulp);
            bool bq = detail::disjoint_multiaction(lhs,ulq);

            if (!bp)
            {
              if (!bq)
              {
                Ca = push_front(Ca,*i);
              }
              else
              {
                Cp = push_front(Cp,*i);
              }
            }
            else if (!bq)
            {
              Cq = push_front(Cq,*i);
            }
          }
          if (!(Cp.empty() && Cq.empty()))
          {
            if (!Cp.empty())
            {
              p = PushComm(Cp,p);
            }
            else
            {
              p = gsApplyAlpha(p);
            }
            if (!Cq.empty())
            {
              q = PushComm(Cq,q);
            }
            else
            {
              q = gsApplyAlpha(q);
            }
            {
              assert(alphas.count(p)>0);
              l=alphas[p];
              assert(alphas.count(q)>0);
              action_label_list_list l2=alphas[q];
              l=sync_list_ht(l,l2);
            }
            
            a=merge(p,q);
            alphas[a]=l;
          }
          else
          {
            l = alphas.count(a)>0?alphas[a]:gsaGetAlpha(a);
          }
          if (!Ca.empty())
          {
            a = comm(Ca,a);
            l = filter_comm_list(l,Ca);
            alphas[a]=l;
          }
          return a;
        }
        else
        {
          action_label_list_list l = alphas.count(a)>0?alphas[a]:gsaGetAlpha(a);
          a = comm(C,a);
          alphas[a]=filter_comm_list(l,C);
          return a;
        }
      }
      else if (is_sync(a) || is_left_merge(a) ||
               is_sum(a) || is_at(a) || is_choice(a) ||
               is_if_then(a) || is_if_then_else(a) || is_bounded_init(a)|| is_seq(a))
      {
        //Yarick, 2009-05-25: do not distribute comm over seq compositions.
        //and also not over sync and leftmerge.
        a = gsApplyAlpha(a);
        assert(alphas.count(a)>0);
        action_label_list_list l = alphas[a];
        a = comm(C,a);
        alphas[a]=filter_comm_list(l,C);
        return a;
      }
      assert(0);
      return process_expression(); //to suppress warnings
    }

    alphabet_reduction::action_label_list_list alphabet_reduction::gsaGetAlpha(
                                    process_expression a, 
                                    size_t length, 
                                    const std::vector < std::multiset < identifier_string > > &allowed) 
    {
      // calculate the alphabet process expression a of a up to the length.
      // if length==0, then the length is unlimited.
      // It updates the global hash table alphas (in case length is 0 and ignore is empty writes the alphabet of a into alphas).
      // The parameter allowed is a list of multiactions (w/o types) of which only sub-multiactions of which are allowed.
      // This may not be strict, e.g. more multiactions can be returned. This is because this parameter
      // is only needed for the performance purposes.

      action_label_list_list l; //result

      if (all_stable)
      {
        //check if the current call is already in the hash table.
        //if so, return the value

        if (is_process_instance(a) || is_process_instance_assignment(a))
        {
          process_identifier p=is_process_instance(a)?process_instance(a).identifier():process_instance_assignment(a).identifier();
          if (alphas_process_identifiers.count(p)>0l)
          {
            return alphas_process_identifiers[p];
          }
        }
        else if (alphas.count(a)>0l)
        {
          return alphas[a];
        }

/*        // MAYBE CACHING OF THESE VALUES MUST BE REESTABLISHED....
        if (length)
        {
          assert(length<(size_t)1 << (sizeof(int)*8-1));
          // Rare truuk. Hier moet nog naar gekeken worden.
          if (alphas_length.count(ATmakeList3((ATerm)(ATermAppl)a,(ATerm)ATmakeInt(static_cast<int>(length)),(ATerm)(ATermList)allowed))>0)
          {
            return alphas_length[ATmakeList3((ATerm)(ATermAppl)a,(ATerm)ATmakeInt(static_cast<int>(length)),(ATerm)(ATermList)allowed)];
          }
        }
*/
      }

      if (is_delta(a))
      {
        // return the empty multi action list
        l = action_label_list_list();
      }
      else if (is_tau(a))
      {
        // return a list containing the empty multi action.
        l = push_front(action_label_list_list(),lps::action_label_list());
      }
      else if (lps::is_action(a))
      {
        lps::action_label lab = lps::action(a).label();
        l = alphas.count(a)>0?alphas[a]:push_front(action_label_list_list(),push_front(lps::action_label_list(),lab));
      }
      else if (is_process_instance(a))
      {
        process_identifier pn=process_instance(a).identifier();
        //this should be an mCRL process (pCRL processes always have an entry).
        //we apply the alphabet reductions to its body and then we know the alphabet
        l=alphas_process_identifiers.count(pn)>0?alphas_process_identifiers[pn]:gsaGetAlpha(procs[pn],length,allowed);
      }
      else if (is_process_instance_assignment(a))
      {
        process_identifier pn=process_instance_assignment(a).identifier();
        //this should be an mCRL process (pCRL processes always have an entry).
        //we apply the alphabet reductions to its body and then we know the alphabet
        l=alphas_process_identifiers.count(pn)>0?alphas_process_identifiers[pn]:gsaGetAlpha(procs[pn],length,allowed);
      }
      else if (is_block(a))
      {
        process_expression p = block(a).operand();
        l=gsaGetAlpha(p,length,allowed);
        l=filter_block_list(l,block(a).block_set());
      }
      else if (is_hide(a))
      {
        process_expression p = hide(a).operand();
        l=gsaGetAlpha(p);

        l=filter_hide_list(l,hide(a).hide_set());
      }
      else if (is_rename(a))
      {
        process_expression p = rename(a).operand();
        l=gsaGetAlpha(p,length);

        l=filter_rename_list(l,rename(a).rename_set());
      }
      else if (is_allow(a))
      {
        process_expression p = allow(a).operand();
        atermpp::term_list < core::identifier_string_list > V=detail::sort_multiactions_allow(detail::transform_list(allow(a).allow_set()));
        size_t max_len = detail::get_max_allowed_length(V);
        if (length && max_len > length)
        {
          max_len=length;
        }

/*      // MAYBE CACHING OF THESE VALUES MUST BE REESTABLISHED....
        if (length)
        {
          if (allowed.empty())
          {
            allowed=detail::from_allow_list_to_allow_vector(V);
          }
          else
          {
            atermpp::term_list < core::identifier_string_list > a1=detail::gsaATintersectList(allowed,V);
            if (!a1.empty())
            {
              allowed=detail::from_allow_list_to_allow_vector(a1);
            }
          }
        }
*/
        l=gsaGetAlpha(p,max_len,detail::from_allow_list_to_allow_vector(V));
        l=filter_allow_list(l,V);
      }
      else if (is_comm(a))
      {
        process_expression p = comm(a).operand();
        communication_expression_list C = detail::sort_multiactions_comm(comm(a).comm_set());

        std::vector < std::multiset < identifier_string > > new_allowed;
        
        if (length && !allowed.empty())
        {
          new_allowed=detail::from_allow_list_to_allow_vector(extend_allow_comm(detail::from_allow_vector_to_allow_list(allowed),C));
        }
        l=gsaGetAlpha(p,length*detail::get_max_comm_length(C),new_allowed);
        l=filter_comm_list(l,C);
      }
      else if (is_sum(a))
      {
        l = gsaGetAlpha(sum(a).operand(),length,allowed);
      }
      else if (is_at(a))
      {
        l = gsaGetAlpha(at(a).operand(),length,allowed);
      }
      else if (is_choice(a))
      {
        l = gsaGetAlpha(choice(a).left(),length,allowed);
        action_label_list_list l2 = gsaGetAlpha(choice(a).right(),length,allowed);
        l = detail::merge_list(l,l2);
      }
      else if (is_seq(a))
      {
        l = gsaGetAlpha(seq(a).left(),length,allowed);
        action_label_list_list l2 = gsaGetAlpha(seq(a).right(),length,allowed);
        l = detail::merge_list(l,l2);
      }
      else if (is_if_then(a))
      {
        l = gsaGetAlpha(if_then(a).then_case(),length,allowed);
      }
      else if (is_if_then_else(a))
      {
        l = gsaGetAlpha(if_then_else(a).then_case(),length,allowed);
        action_label_list_list l2 = gsaGetAlpha(if_then_else(a).else_case(),length,allowed);
        l = detail::merge_list(l,l2);
      }
      else if (is_sync(a))
      {
        l = gsaGetAlpha(sync(a).left(),length,allowed);
        action_label_list_list l2 = gsaGetAlpha(sync(a).right(),length,allowed);
        action_label_list_list l1=l;
        l = detail::merge_list(l1,l2);
        action_label_list_list s=sync_list(l1,l2,length,allowed);
        l = detail::merge_list(l,s);
      }
      else if (is_merge(a))
      {

        action_label_list_list l1 = gsaGetAlpha(merge(a).left(),length,allowed);
        action_label_list_list l2 = gsaGetAlpha(merge(a).right(),length,allowed);

        l = detail::merge_list(l1,l2);

        action_label_list_list s=sync_list(l1,l2,length,allowed);
        l = detail::merge_list(l,s);
      }
      else if (is_left_merge(a))
      {
        action_label_list_list l1 = gsaGetAlpha(left_merge(a).left(),length,allowed);
        action_label_list_list l2 = gsaGetAlpha(left_merge(a).right(),length,allowed);

        l = detail::merge_list(l1,l2);
        action_label_list_list s=sync_list(l1,l2,length,allowed);
        l = detail::merge_list(l,s);
      }
      else if (is_bounded_init(a))
      {
        l = gsaGetAlpha(bounded_init(a).left(),length,allowed);
      }
      else
      {
        assert(0);
      }

      assert(l!=ATerm());  // This cannot be done comparing to action_label_list_list, as in this case
                           // the default constructor is empty;

      if (all_stable)
      {
        if (!length)
        {
          alphas[a]=l;
        }
        else
        {
/*        // MAYBE CACHING OF THESE VALUES MUST BE REESTABLISHED....
          assert(length<(size_t)1 << (sizeof(int)*8-1));
          alphas_length[ATmakeList3((ATerm)(ATermAppl)a,(ATerm)ATmakeInt((int)length),(ATerm)(ATermList)allowed)]=l;
 */
        }
      }

      return l;
    }


    process_expression alphabet_reduction::gsApplyAlpha(process_expression a)
    {
      // apply the alpha reductions to a.
      // makes sure that the alphabet of a is in the table alphas after the function returns its value
      assert(all_stable);
      if (is_delta(a) || is_tau(a))
      {
      }
      else if (lps::is_action(a))
      {
        alphas[lps::action(a)]=push_front(action_label_list_list(),push_front(lps::action_label_list(),action(a).label()));
      }
      else if (is_process_instance(a) || is_process_instance_assignment(a))
      {
        process_identifier pn= is_process_instance(a)?process_instance(a).identifier():process_instance_assignment(a).identifier();
        process_identifier_list l(alphas_process_identifiers[pn]); // for this particular process term

        // if this process is not recursive we apply the alphabet reductions to it
        if (non_recursive_set.count(pn)>0)
        {
          //if this is a mCRL process.
          //we apply the alphabet reductions to its body and then we know the alphabet
          process_expression new_p=gsApplyAlpha(procs[pn]);
          procs[pn]=new_p;
          assert(alphas.count(new_p)>0);
          alphas_process_identifiers[pn]=alphas[new_p];
          if (alphas_process_identifiers.count(pn)==0)
          {
            if (alphas.count(new_p)==0)
            {
              return process_expression();
            }
            else l=process_identifier_list(alphas[new_p]);
          }
          else l=process_identifier_list(alphas_process_identifiers[pn]);
        }
        alphas[a]=action_label_list_list(l); //for this full process call
      }
      else if (is_block(a))
      {
        process_expression p = block(a).operand();
        a = PushBlock(block(a).block_set(),p); //takes care about l
      }
      else if (is_hide(a))
      {
        process_expression p = hide(a).operand();
        a = PushHide(hide(a).hide_set(),p); //takes care about l
      }
      else if (is_rename(a))
      {
        process_expression p = rename(a).operand();
        p = gsApplyAlpha(p);
        a = rename(rename(a).rename_set(),p);
        action_label_list_list l = alphas.count(p)>0?alphas[p]:gsaGetAlpha(p);
        alphas[a]=filter_rename_list(l,rename(a).rename_set());
      }
      else if (is_allow(a))
      {
        process_expression p = allow(a).operand();
        a = PushAllow(detail::transform_list(allow(a).allow_set()),p); //takes care about l
      }
      else if (is_comm(a))
      {
        process_expression p = comm(a).operand();
        a = PushComm(comm(a).comm_set(),p); //takes care about l
      }
      else if (is_sum(a))
      {
        process_expression p = gsApplyAlpha(sum(a).operand());

        assert(alphas.count(p)>0);
        action_label_list_list l=alphas[p];

        a=sum(sum(a).bound_variables(),p);
        alphas[a]=l;
      }
      else if (is_at(a))
      {
        process_expression p = gsApplyAlpha(at(a).operand());

        assert(alphas.count(p)>0);
        action_label_list_list l=alphas[p];

        a=at(p,at(a).time_stamp());
        alphas[a]=l;
      }
      else if (is_choice(a))
      {
        process_expression p = gsApplyAlpha(choice(a).left());
        process_expression q = gsApplyAlpha(choice(a).right()); 
        action_label_list_list l,l1,l2;
        assert(alphas.count(p)>0);
        l=l1=alphas[p];
        assert(alphas.count(q)>0);
        l2=alphas[q];

        l = sync_list_ht(l,l2,false);

        a=choice(p,q);
        alphas[a]=l;
      }
      else if (is_seq(a))
      {
        process_expression p = gsApplyAlpha(seq(a).left());
        process_expression q = gsApplyAlpha(seq(a).right());

        action_label_list_list l,l1,l2;
        assert(alphas.count(p)>0);
        l=l1=alphas[p];
        assert(alphas.count(q)>0);
        l2=alphas[q];

        l = sync_list_ht(l,l2,false);

        a=seq(p,q);
        alphas[a]=l;
      }
      else if (is_if_then(a))
      {
        process_expression p = gsApplyAlpha(if_then(a).then_case());
        assert(alphas.count(p)>0);
        action_label_list_list l=alphas[p];
        a=if_then(if_then(a).condition(),p);
        alphas[a]=l;
      }
      else if (is_if_then_else(a))
      {
        process_expression p = gsApplyAlpha(if_then_else(a).then_case());
        process_expression q = gsApplyAlpha(if_then_else(a).else_case());

        action_label_list_list l,l1,l2;
        assert(alphas.count(p)>0);
        l=l1=alphas[p];
        assert(alphas.count(q)>0);
        l2=alphas[q];

        l = sync_list_ht(l,l2,false);
        a=if_then_else(if_then_else(a).condition(),p,q);
        alphas[a]=l;
      }
      else if (is_sync(a))
      {
        process_expression p = gsApplyAlpha(sync(a).left());
        process_expression q = gsApplyAlpha(sync(a).right());

        action_label_list_list l,l1,l2;
        assert(alphas.count(p)>0);
        l=l1=alphas[p];
        assert(alphas.count(q)>0);
        l2=alphas[q];

        l=sync_list_ht(l,l2);

        a=sync(p,q); 
        alphas[a]=l;
      }
      else if (is_merge(a))
      {
        process_expression p = gsApplyAlpha(merge(a).left());
        process_expression q = gsApplyAlpha(merge(a).right());

        action_label_list_list l,l1,l2;
        assert(alphas.count(p)>0);
        l=l1=alphas[p];
        assert(alphas.count(q)>0);
        l2=alphas[q];

        l=sync_list_ht(l,l2);

        a=merge(p,q); 

        alphas[a]=l;
      }
      else if (is_left_merge(a))
      {
        process_expression p = gsApplyAlpha(left_merge(a).left());
        process_expression q = gsApplyAlpha(left_merge(a).right());

        action_label_list_list l,l1,l2;
        assert(alphas.count(p)>0);
        l=l1=alphas[p];
        assert(alphas.count(q)>0);
        l2=alphas[q];

        l=sync_list_ht(l,l2);

        a=left_merge(p,q);
        alphas[a]=l;
      }
      else if (is_bounded_init(a))
      {
        process_expression p = gsApplyAlpha(bounded_init(a).left());
        assert(alphas.count(p)>0);
        action_label_list_list l=alphas[p];
        a=bounded_init(p,bounded_init(a).right());
        alphas[a]=l;
      }

      assert(alphas.count(a)>0);

      return a;
    }

    process_identifier_list alphabet_reduction::gsaGetDeps(process_expression a)
    {
      //returns process names that a depends to (should be applied iteratively).
      if (is_delta(a) || is_tau(a) || lps::is_action(a))
      {
        return process_identifier_list();
      }
      else if (is_process_instance(a) || is_process_instance_assignment(a))
      {
        process_identifier pn=is_process_instance(a)?process_instance(a).identifier():
                         process_instance_assignment(a).identifier();
        const process_identifier_list r=push_front(process_identifier_list(),pn);
        if (deps.count(pn)>0)
        {
          return detail::merge_list(r,deps[pn]);
        }
        return push_front(process_identifier_list(),pn);
      }
      else if (is_sum(a))
      {
        return gsaGetDeps(sum(a).operand());
      }
      else if (is_at(a))
      {
        return gsaGetDeps(at(a).operand());
      }
      else if (is_choice(a))
      {
        return detail::merge_list(gsaGetDeps(choice(a).left()),gsaGetDeps(choice(a).right()));
      }
      else if (is_seq(a))
      {
        return detail::merge_list(gsaGetDeps(seq(a).left()),gsaGetDeps(seq(a).right()));
      }
      else if (is_block(a))
      {
        return gsaGetDeps(block(a).operand());
      }
      else if (is_hide(a))
      {
        return gsaGetDeps(hide(a).operand());
      }
      else if (is_rename(a))
      {
        return gsaGetDeps(rename(a).operand());
      }
      else if (is_allow(a))
      {
        return gsaGetDeps(allow(a).operand());
      }
      else if (is_comm(a))
      {
        return gsaGetDeps(comm(a).operand());
      }
      else if (is_if_then(a))
      {
        return gsaGetDeps(if_then(a).then_case());
      }
      else if (is_if_then_else(a))
      {
        return detail::merge_list(gsaGetDeps(if_then_else(a).then_case()),gsaGetDeps(if_then_else(a).else_case()));
      }
      else if (is_sync(a))
      {
        return detail::merge_list(gsaGetDeps(sync(a).left()),gsaGetDeps(sync(a).right()));
      }
      else if (is_merge(a))
      {
        return detail::merge_list(gsaGetDeps(merge(a).left()),gsaGetDeps(merge(a).right()));
      }
      else if (is_left_merge(a))
      {
        return detail::merge_list(gsaGetDeps(left_merge(a).left()),gsaGetDeps(left_merge(a).right()));
      }
      else if (is_bounded_init(a))
      {
        return gsaGetDeps(bounded_init(a).left());
      }
      assert(0);
      return process_identifier_list(); //to suppress warnings
    }

    // Delivers true if this is a pCRL term, and false if this is an mCRL term.
    bool alphabet_reduction::gsaGetProp(const process_expression a, process_identifier context)
    {
      bool r=true;
      if (is_delta(a) || is_tau(a) || lps::is_action(a))
      {
        return true;
      }
      else if (is_process_instance(a))
      {
        process_identifier pn=process_instance(a).identifier();
        assert (pCRL_set.count(pn)>0 || mCRL_set.count(pn)>0);
        return pCRL_set.count(pn)>0;
      }
      else if (is_process_instance_assignment(a))
      {
        process_identifier pn=process_instance_assignment(a).identifier();
        assert (pCRL_set.count(pn)>0 || mCRL_set.count(pn)>0);
        return pCRL_set.count(pn)>0;
      }
      else if (is_sum(a))
      {
        return gsaGetProp(sum(a).operand(),context);
      }
      else if (is_at(a))
      {
        return gsaGetProp(at(a).operand(),context);
      }
      else if (is_choice(a))
      {
        return gsaGetProp(choice(a).left(),context) && gsaGetProp(choice(a).right(),context);
      }
      else if (is_seq(a))
      {
        return gsaGetProp(seq(a).left(),context) && gsaGetProp(seq(a).right(),context);
      }
      else if (is_block(a))
      {
        return gsaGetProp(block(a).operand(),context);
      }
      else if (is_hide(a))
      {
        return gsaGetProp(hide(a).operand(),context);
      }
      else if (is_rename(a))
      {
        return gsaGetProp(rename(a).operand(),context);
      }
      else if (is_allow(a))
      {
        return gsaGetProp(allow(a).operand(),context);
      }
      else if (is_comm(a))
      {
        return gsaGetProp(comm(a).operand(),context);
      }
      else if (is_if_then(a))
      {
        return gsaGetProp(if_then(a).then_case(),context);
      }
      else if (is_if_then_else(a))
      {
        return gsaGetProp(if_then_else(a).then_case(),context) && gsaGetProp(if_then_else(a).else_case(),context);
      }
      else if (is_bounded_init(a))
      {
        return gsaGetProp(bounded_init(a).left(),context);
      }
      else if (is_sync(a)||is_merge(a)||is_left_merge(a))
      {
        process_identifier_list deps=gsaGetDeps(a);
        if (std::find(deps.begin(),deps.end(),context)!=deps.end())
        {
          r=false;
        }
        else
        {
          //if any process name in deps is recursive, also r=mCRL_aterm;
          for (process_identifier_list::const_iterator l=deps.begin(); l!=deps.end(); ++l)
          {
            if (recursive_set.count(*l)>0)
            {
              r=false;
              break;
            }
          }
        }
      }
      else
      {
        assert(0);
      }

      return r;
    }

    /** \brief     Apply alphabet reduction to an mCRL2 process specification.
     *  \param[in] equations process equations to which alpha conversion needs to be applied.
     *  \param[in] init The initial process.
     **/
    void alphabet_reduction::gsAlpha(
        process_equation_list& equations,
        process_expression& init)
    {
      mCRL2log(verbose) << "applying alphabet reductions...\n";
      //create the tables
      detail::afunPair=AFun("p_allow",2,false);

      //fill in tables
      for (process_equation_list::const_iterator pr=equations.begin(); pr!=equations.end(); ++pr)
      {
        const process_equation p= *pr;
        const process_identifier pn=p.identifier();
        procs[pn]=p.expression();
        form_pars[pn]=p.formal_parameters();
      }

      procs[INIT_KEY()]=init;

      //Calculate the dependencies of the processes.
      //we start from init and iterate on the processes init depends upon init until the system stabilises.
      bool stable=false;
      while (!stable)
      {
        //apply to each and compare with the old values.
        stable=true;
        process_identifier_list todo;
        if (deps.count(INIT_KEY())>0)
        {
          todo=push_front(deps[INIT_KEY()],INIT_KEY());
        }
        else
        {
          todo=push_front(process_identifier_list(),INIT_KEY());
        }

        for (; !todo.empty(); todo=pop_front(todo))
        {
          process_identifier pn=todo.front();
          process_identifier_list old_dep;
          if (deps.count(pn)==0)
          {
            deps[pn]=old_dep;
          }
          else 
          {
            old_dep=deps[pn];
          }
          process_identifier_list dep=detail::gsaATsortList(gsaGetDeps(procs[pn]));
          if (dep!=old_dep)
          {
            stable=false;
            deps[pn]=dep;
          }
        }
      }

      //recursive or not?
      process_identifier_list todo;

      if (deps.count(INIT_KEY())>0)
      {
        todo=push_front(deps[INIT_KEY()],INIT_KEY());
      }
      else
      {
        todo=push_front(process_identifier_list(),INIT_KEY());
      }
      for (; !todo.empty(); todo=pop_front(todo))
      {
        process_identifier p=todo.front();
        process_identifier_list dep=deps[p];
        bool rec=std::find(dep.begin(),dep.end(),p)!=dep.end(); //true if found 
        if (rec)
        {
          recursive_set.insert(p);
        }
        else
        {
          non_recursive_set.insert(p);
        }
        pCRL_set.insert(p);
      }

      //mCRL, or pCRL
      stable=false;
      while (!stable)
      {
        //apply to each and compare with the old values.
        stable=true;
        process_identifier_list todo;
        if (deps.count(INIT_KEY())>0)
        {
          todo=push_front(deps[INIT_KEY()],INIT_KEY());
        }
        else
        {
          todo=push_front(process_identifier_list(),INIT_KEY());
        }
        for (; !todo.empty(); todo=pop_front(todo))
        {
          process_identifier p=todo.front();
          bool is_pCRL=gsaGetProp(procs[p],p);
          if (is_pCRL == (pCRL_set.count(p)==0))
          {
            if (is_pCRL)
            {
              pCRL_set.insert(p);
              mCRL_set.erase(p);
            }
            else
            { 
              pCRL_set.erase(p);
              mCRL_set.insert(p);
            }
            stable=false;
          }
        }
      }

      //calculate the alphabets of the processes iteratively
      //for pCRL processes (for || processes this may be too expensive)

      if (deps.count(INIT_KEY())>0)
      {
        todo=push_front(deps[INIT_KEY()],INIT_KEY());
      }
      else
      {
        todo=push_front(process_identifier_list(),INIT_KEY());
      }
      for (process_identifier_list::const_iterator pr=todo.begin(); pr!=todo.end(); ++pr)
      {
        process_identifier pn= *pr;
        if (mCRL_set.count(pn)>0)
        {
          continue;
        }
        alphas_process_identifiers[pn]=action_label_list_list();
      }

      alphas[delta()]=action_label_list_list();
      alphas[tau()]=action_label_list_list();

      if (deps.count(INIT_KEY())>0)
      {
        todo=push_front(deps[INIT_KEY()],INIT_KEY());
      }
      else
      {
        todo=push_front(process_identifier_list(),INIT_KEY());
      }
      stable=false;
      all_stable=false;
      //possibly endless loop (X=a.X||X ;)
      while (!stable)
      {
        //apply getAlpha to each and compare with the old values.
        stable=true;
        for (process_identifier_list::const_iterator pr=todo.begin(); pr!=todo.end(); ++pr)
        {
          process_identifier pn= *pr;
          if (mCRL_set.count(pn)>0)
          {
            continue;
          }
          assert(alphas_process_identifiers.count(pn)>0);
          action_label_list_list old_l=alphas_process_identifiers[pn];
          action_label_list_list l=gsaGetAlpha(procs[pn]);
          alphas_process_identifiers[pn]=l;
          if (old_l.size()!=l.size() || !(detail::list_minus(old_l,l).empty()))
          {
            stable=false;
          }
        }
      }
      all_stable=true;

      // apply the reduction to init
      // it will recursively trigger all mCRL processes that init depends upon.
      {
        process_identifier pn=INIT_KEY();
        process_expression new_p=gsApplyAlpha(procs[pn]);
        procs[pn]=new_p;
        alphas_process_identifiers[pn]=alphas[new_p];
      }

      //recalculate the new dependencies again
      {
        deps.clear();; //process dependencies : P(Pname,type) -> List(P(Pname,type))
        stable=false;
        while (!stable)
        {
          //apply to each and compare with the old values.
          stable=true;
          if (deps.count(INIT_KEY())>0)
          {
            todo=push_front(deps[INIT_KEY()],INIT_KEY());
          }
          else
          {
            todo=push_front(process_identifier_list(),INIT_KEY());
          }

          for (process_identifier_list::const_iterator i=todo.begin(); i!=todo.end(); ++i)
          {
            process_identifier pn= *i;
            process_identifier_list old_dep;
            if (deps.count(pn)==0)
            {
              deps[pn]=old_dep;
            }
            else
            {
              old_dep=deps[pn];
            }
            process_identifier_list dep=detail::gsaATsortList(gsaGetDeps(procs[pn]));
            if (dep!=old_dep)
            {
              stable=false;
              deps[pn]=dep;
            }
          }
        }
      }
      //== write out the process equations
      //first the original ones (except deleted)
      process_equation_list new_pr;
      for (process_equation_list::const_iterator pr=equations.begin(); pr!=equations.end(); ++pr)
      {
        process_equation p= *pr;
        process_identifier pn=p.identifier();
        process_expression res=procs[pn];
        if (res!=process_expression())
        {
          new_pr=push_front(new_pr,process_equation(p.identifier(),p.formal_parameters(),res));
          procs.erase(pn);
        }
      }
      //now the generated ones
      assert(deps.count(INIT_KEY()));
      todo=deps[INIT_KEY()];
      for (process_identifier_list::const_iterator pr=todo.begin(); pr!=todo.end(); ++pr)
      {
        process_identifier pn= *pr;
        if (pn==INIT_KEY())
        {
          continue;
        }
        if (procs.count(pn)>0)
        {
          variable_list fpars;
          if (form_pars.count(pn)>0)
          {
            fpars = form_pars[pn];
          }

          //if generated during the alpha substitutions
          process_identifier old_pn;
          if (subs_alpha_rev.count(pn)>0)
          {
            old_pn=subs_alpha_rev[pn];
            if (form_pars.count(old_pn)>0)
            {
              fpars = form_pars[old_pn];
            }
          }

          new_pr=push_front(new_pr,process_equation(pn,fpars,procs[pn]));
        }
      }
      new_pr=reverse(new_pr);

      equations=new_pr;
      init=process_expression(procs[INIT_KEY()]);
    }

    // void apply_alphabet_reduction(process_specification& p)
    void alphabet_reduction::operator()(process_specification& p)
    {
      // Must create termlists for all types to feed them back in process specification.
      lps::action_label_list action_labels(p.action_labels().begin(),p.action_labels().end());
      data::variable_list global_variables(p.global_variables().begin(),p.global_variables().end());
      process_equation_list equations(p.equations().begin(),p.equations().end());
      process_expression init=p.init();
      gsAlpha(equations,init);
      p=process_specification(p.data(),action_labels,global_variables,equations,init);
    }

  } // namespace process
} // namespace mcrl2

