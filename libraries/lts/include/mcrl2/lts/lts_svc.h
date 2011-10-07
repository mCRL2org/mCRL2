// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file lts_svc.h
 *
 * \brief This file contains a class that contains labelled transition systems in svc format.
 * \details The svc format allows ATerms as state and action labels. It is a residue of the
 *          muCRL format that used ATerms everywhere.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_SVC_MCRL2_H
#define MCRL2_LTS_SVC_MCRL2_H

#include <string>
#include <vector>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/lts/lts.h"


namespace mcrl2
{
namespace lts
{
namespace detail
{

/** \brief A state label for svc format is just an ATerm.
*/
typedef atermpp::aterm state_label_svc;

/** \brief Pretty print a state value of this LTS.
 * \param[in] l  The state value to pretty print.
 * \return           The pretty-printed representation of value.
 */
inline std::string pp(const state_label_svc l)
{
  return ATwriteToString(l);
  // return ATwriteToString(l.aterm());
}

/** \brief A class containing an ATerm, which is the action label for the svc format.
*/
class action_label_svc:public atermpp::aterm
{
    friend struct atermpp::aterm_traits<action_label_svc>;

  public:
    /** \brief Default constructor.
    */
    action_label_svc()
    {}

    /** \brief Constructor setting this action label to a.
    */
    action_label_svc(const ATerm a):atermpp::aterm(a)
    {
    }

    /** \brief A function hiding actions. As svc action labels have no structure, this will throw an exception.
    */
    bool hide_actions(const std::vector<std::string> &)
    {
      using namespace std;
      mCRL2log(log::error) << "Hiding actions in a .svc file is not possible, because the labels do not necessarily have an action structure \n";
      return false;
    }
};

/** \brief Pretty print function for svc action labels.
*/
inline std::string pp(const action_label_svc l)
{
  return ATwriteToString(l);
}

} // namespace detail


/** \brief A transition system for the svc format, where states and action labels are ATerms.
*/
class lts_svc_t : public lts< detail::state_label_svc, detail::action_label_svc >
{

  public:

    /** \brief Provides the type of this lts. In this case it is lts_svc. */
    lts_type type() const
    {
      return lts_svc;
    }

    /** \brief Save the labelled transition system to file.
     *  \details If the filename is empty, the result is read from stdin.
     *  \param[in] filename Name of the file from which this lts is read.
     */
    void load(const std::string& filename);

    /** \brief Save the labelled transition system to file.
     *  \details If the filename is empty, the result is written to stdout.
     *  \param[in] filename Name of the file to which this lts is written.
     */
    void save(const std::string& filename) const;
};
} // namespace lts
} // namespace mcrl2


/// \cond INTERNAL_DOCS

namespace atermpp
{
template<>
struct aterm_traits<mcrl2::lts::detail::action_label_svc>
{
  static void protect(const mcrl2::lts::detail::action_label_svc& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::lts::detail::action_label_svc& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::lts::detail::action_label_svc& t)
  {
    t.mark();
  }
};
} // namespace atermpp
/// \endcond


#endif
