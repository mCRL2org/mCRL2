// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CONSOLEDOCK_H
#define CONSOLEDOCK_H

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QProcess>

/**
 * @brief ProcessType Defines all possible process types
 */
enum class ProcessType
{
  Parsing,
  Simulation,
  LtsCreation,
  Verification,
  Rewriting,
};

Q_DECLARE_METATYPE(ProcessType)

/**
 * @brief PROCESSTYPES Defines a list of all process types so they can be
 *   iterated over
 */
const std::vector<ProcessType> PROCESSTYPES = {
    ProcessType::Parsing, 
    ProcessType::Simulation, 
    ProcessType::LtsCreation,
    ProcessType::Verification,
    ProcessType::Rewriting
};

/**
 * @brief PROCESSTYPENAMES Defines a name for each process type to show in the
 *   interface
 */
const std::map<ProcessType, QString> PROCESSTYPENAMES = {
    {ProcessType::Parsing, "Parsing"},
    {ProcessType::Simulation, "Simulation"},
    {ProcessType::LtsCreation, "State Space Generation"},
    {ProcessType::Verification, "Verification"},
    {ProcessType::Rewriting, "Rewriting"}
};

/**
 * @brief The ConsoleWidget class defines a widget used to show console output
 */
class ConsoleWidget : public QPlainTextEdit
{
  Q_OBJECT

  public:
  /**
   * @brief ConsoleDock Constructor
   * @param parent The parent of this widget
   */
  explicit ConsoleWidget(QWidget* parent);
  ~ConsoleWidget();

  private slots:
  /**
   * @brief showContextMenu Creates and shows a context menu
   * @param position The position where to create the context menu
   */
  void showContextMenu(const QPoint& position);
};

/**
 * @brief The ConsoleDock class defines the dock that shows console output
 */
class ConsoleDock : public QDockWidget
{
  Q_OBJECT

  public:
  const Qt::DockWidgetArea defaultArea = Qt::BottomDockWidgetArea;

  /**
   * @brief ConsoleDock Constructor
   * @param parent The parent of this widget
   */
  explicit ConsoleDock(QWidget* parent);
  ~ConsoleDock();

  /**
   * @brief setConsoleTab Set which tab should be visible
   * @param processType The tab to make visible
   */
  void setConsoleTab(ProcessType processType);

  /**
   * @brief writeToConsole Write something to the console
   * @param processType Which console tab to write to
   * @param output What to write
   */
  void writeToConsole(ProcessType processType, const QString& output);

  /**
   * @brief broadcast Writes a message to all consoles
   * @param output The message to write
   */
  void broadcast(const QString& output);

  /**
   * @brief getConsoleOutput Get the output currently in the console
   * @param processType The tab to get the output from
   * @return The console output
   */
  QString getConsoleOutput(ProcessType processType);

  public slots:
  /**
   * @brief logToParsingConsole Write output in the parsing console
   */
  void logToParsingConsole();

  /**
   * @brief logToSimulationConsole Write output in the simulation console
   */
  void logToSimulationConsole();

  /**
   * @brief logToLtsCreationConsole Write output in the lts creation console
   */
  void logToLtsCreationConsole();

  /**
   * @brief logToVerificationConsole Write output in the verification console
   */
  void logToVerificationConsole();

  /**
   * @brief logToVerificationConsole Write output in the rewrite console
   */
  void logToRewriteConsole();

  private:
  QTabWidget* consoleTabs;
  std::map<ProcessType, ConsoleWidget*> consoles;

  /**
   * @brief logToConsole Write output in the console
   * @param processType Which console tab to write to
   * @param process The process that wants to write on the console
   */
  void logToConsole(ProcessType processType, QProcess* process);
};

#endif // CONSOLEDOCK_H
