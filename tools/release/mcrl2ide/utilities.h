// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef UTILITIES_H
#define UTILITIES_H

#include "mcrl2/lts/lts_equivalence.h"

#include <QComboBox>
#include <QMessageBox>

/**
 * @brief LTSEQUIVALENCENAMES Defines a name for each supported equivalence to
 *   show in the interface
 */
const std::map<mcrl2::lts::lts_equivalence, QString> LTSEQUIVALENCENAMES = {
    {mcrl2::lts::lts_eq_none, "None"},
    {mcrl2::lts::lts_eq_bisim, "Strong Bisimulation"},
    {mcrl2::lts::lts_eq_trace, "Trace equivalence"},
    {mcrl2::lts::lts_eq_divergence_preserving_branching_bisim,
     "Divergence Preserving Branching Bisimulation"},
    {mcrl2::lts::lts_eq_branching_bisim, "Branching Bisimulation"},
    {mcrl2::lts::lts_eq_weak_bisim, "Weak Bisimulation"},
    {mcrl2::lts::lts_eq_weak_trace, "Weak Trace Equivalence"}};

/**
 * @brief LTSEQUIVALENCESWITHABSTRACTION Defines which equivalences use
 *   abstraction
 */
const std::list<mcrl2::lts::lts_equivalence> LTSEQUIVALENCESWITHABSTRACTION = {
    mcrl2::lts::lts_eq_branching_bisim,
    mcrl2::lts::lts_eq_divergence_preserving_branching_bisim,
    mcrl2::lts::lts_eq_weak_bisim, mcrl2::lts::lts_eq_weak_trace};

/**
 * @brief LTSEQUIVALENCESWITHOUTABSTRACTION Defines which equivalences do not
 *   use abstraction
 */
const std::list<mcrl2::lts::lts_equivalence> LTSEQUIVALENCESWITHOUTABSTRACTION =
    {mcrl2::lts::lts_eq_bisim, mcrl2::lts::lts_eq_trace};

/**
 * @brief The EquivalenceComboBox class defines a combobox for selecting an
 *   equivalence
 */
class EquivalenceComboBox : public QComboBox
{
  public:
  /**
   * @brief EquivalenceComboBox Constructor
   * @param parent The parent of this widget
   */
  EquivalenceComboBox(QWidget* parent = 0);

  /**
   * @brief getSelectedEquivalence Returns the equivalence that has been
   *   selected
   * @return The equivalence that has been selected
   */
  mcrl2::lts::lts_equivalence getSelectedEquivalence();

  /**
   * @brief setSelectedEquivalence Sets the equivalence that is selected
   * @param equivalence The equivalence to be selected
   */
  void setSelectedEquivalence(mcrl2::lts::lts_equivalence equivalence);
};

/**
 * @brief getEquivalenceName Returns the name that corresponds to the given
 *   equivalence
 * @param equivalence An equivalence
 * @param fillSpaces Whether the spaces in the name need to be replaced by
 *   underscores
 * @return The name that corresponds to the given equivalence
 */
QString getEquivalenceName(mcrl2::lts::lts_equivalence equivalence,
                           bool fillSpaces = false);

/**
 * @brief getEquivalenceFromName Returns the equivalence that corresponds to the
 *   given name
 * @param name The name of an equivalence
 * @return The equivalence that corresponds to the given name, lts_eq_none if
 *   not found
 */
mcrl2::lts::lts_equivalence getEquivalenceFromName(const QString& name);

/**
 * @brief executeInformationBox Executes an information box (pop-up dialog to
 *   the user with a message and an OK button)
 * @param parent The parent of the information box
 * @param title The title of the information box
 * @param message The message body of the information box
 */
void executeInformationBox(QWidget* parent, const QString& title,
                           const QString& message);

/**
 * @brief executeBinaryQuestionBox Executes a question box (pop-up dialog to the
 *   user with a message, a yes button and a no button)
 * @param parent The parent of the question box
 * @param title The title of the question box
 * @param message The message body of the question box
 * @return Whether the button Yes has been pressed (true) or another button
 *   (false)
 */
bool executeBinaryQuestionBox(QWidget* parent, const QString& title,
                              const QString& message);

/**
 * @brief executeQuestionBox Executes a question box (pop-up dialog to the user
 *   with a message, a yes button, a no button and a cancel button)
 * @param parent The parent of the question box
 * @param title The title of the question box
 * @param message The message body of the question box
 * @return The button that has been pressed by the user
 */
QMessageBox::StandardButton executeQuestionBox(QWidget* parent,
                                               const QString& title,
                                               const QString& message);

#endif // UTILITIES_H
