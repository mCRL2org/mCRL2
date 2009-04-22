// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/map_substitution_adapter.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_MAP_SUBSTITUTION_ADAPTER_H
#define MCRL2_DATA_MAP_SUBSTITUTION_ADAPTER_H

#include "mcrl2/new_data/substitution.h"
#include "mcrl2/new_data/replace.h"

namespace mcrl2 {

namespace new_data {

  /// \brief An adapter that turns a mapping from variables to expressions into a substitution.
  template <typename MapContainer>
  class map_substitution_adapter
  {
    public:
      /// \brief type used to represent variables
      typedef typename MapContainer::key_type variable_type;

      /// \brief type used to represent expressions
      typedef typename MapContainer::mapped_type expression_type;

    protected:
      /// \brief The wrapped substitution
      const MapContainer& m_map;

      /// \brief Iterator type for constant element access
      typedef typename MapContainer::const_iterator const_iterator;

      /// \brief Iterator type for non-constant element access
      typedef typename MapContainer::const_iterator iterator;

    public: 
      /// \brief Constructor
      map_substitution_adapter(const MapContainer& m)
        : m_map(m)
      {}

      /// \brief Apply on single single variable expression
      /// \param[in] v the variable for which to give the associated expression
      /// \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
      expression_type operator()(variable_type const& v) const {
        const_iterator i = m_map.find(v);
        expression_type t; t=v; // Do not assume existence of a constructor.
        return i == m_map.end() ? t : i->second;
      }

      /** \brief Apply substitution to an expression
       *
       * \param[in] e the expression to which to apply substitution
       * \return expression equivalent to <|s|>(<|e|>), or a reference to such an expression
       * \note This overload is only available if Expression is not equal to Variable (modulo const-volatile qualifiers)
       **/
      expression_type operator()(const expression_type& e) const {
        return variable_map_replace(e, m_map);
      }
  };

  /// \brief Utility function for creating a map_substitution_adapter.
  template <typename MapContainer>
  map_substitution_adapter<MapContainer> make_map_substitution_adapter(const MapContainer& m)
  {
    return map_substitution_adapter<MapContainer>(m);
  }

} // namespace new_data

} // namespace mcrl2

#endif // MCRL2_DATA_MAP_SUBSTITUTION_ADAPTER_H
