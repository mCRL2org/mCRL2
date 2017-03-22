// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_QT_TOOL_H
#define MCRL2_UTILITIES_QT_TOOL_H

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>
#include <QUrl>
#include <QtGlobal>
#include <string>
#include "mcrl2/utilities/toolset_version.h"
#include "mcrl2/utilities/workarounds.h"

namespace mcrl2
{
namespace gui
{
namespace qt
{

class HelpMenu : public QMenu
{
  Q_OBJECT

  private:
    QMainWindow *m_window;
    QString m_name;
    QString m_author;
    QString m_description;
    QString m_manualUrl;

  public:
    HelpMenu(QMainWindow *window, const std::string &name, const std::string &author, const std::string &description, const std::string &manualUrl):
      QMenu(window->menuBar()),
      m_window(window),
      m_name(QString::fromStdString(name)),
      m_author(QString::fromStdString(author)),
      m_description(QString::fromStdString(description)),
      m_manualUrl(QString::fromStdString(manualUrl))
    {
      QAction *actionContents = new QAction(window);
      actionContents->setText("&Contents");
      actionContents->setShortcut(QString("F1"));
      connect(actionContents, SIGNAL(triggered()), this, SLOT(showContents()));

      QAction *actionAbout = new QAction(window);
      actionAbout->setText("&About");
      connect(actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

      setTitle("&Help");
      addAction(actionContents);
      window->addAction(actionContents);
      addSeparator();
      addAction(actionAbout);
    }

  protected slots:
    void showContents()
    {
      QDesktopServices::openUrl(QUrl(m_manualUrl));
    }

    void showAbout()
    {
      QString version = QString(mcrl2::utilities::get_toolset_version().c_str());
      QString description = m_description;
      description.replace("\n", "<br>");
      QString message;
      message += "<h1>" + m_name + "</h1>";
      message += "<p>" + description + "</p>";
      message += "<p>Written by " + m_author + "</p>";
      message += "<\br>";
      message += "<p>Version: " + version + "</p>";
      QMessageBox::about(m_window, QString("About ") + m_name, message);
    }
};

template <typename Tool>
class qt_tool: public Tool
{
  protected:
    QApplication* m_application;
    std::string m_name;
    std::string m_author;
    std::string m_about_description;
    std::string m_manual_url;

  public:
    qt_tool(const std::string& name,
            const std::string& author,
            const std::string& what_is,
            const std::string& tool_description,
            const std::string& about_description,
            const std::string& manual_url,
            std::string known_issues = ""
           )
      : Tool(name, author, what_is, tool_description, known_issues),
        m_application(NULL),
        m_name(name),
        m_author(author), 
        m_about_description(about_description),
        m_manual_url(manual_url)
    {
    }

    int execute(int& argc, char** argv)
    {
      return Tool::execute(argc, argv);
    }

    bool pre_run(int& argc, char** argv)
    {
      // The instantiation of QApplication has been postponed
      // to here, since creating it in execute would make it
      // impossible to view the help text in an environment
      // without display server
      m_application = new QApplication(argc, argv);
      qsrand(QDateTime::currentDateTime().toTime_t());
      return true;
    }

    bool show_main_window(QMainWindow *window)
    {
      HelpMenu *menu = new HelpMenu(window, m_name, m_author, m_about_description, m_manual_url);
      window->menuBar()->addAction(menu->menuAction());
#ifdef __APPLE__
      window->menuBar()->setNativeMenuBar(false); // Ugly workaround as menu bar is not shown until application 
                                                  // is unfocused and focused again on the Mac. 
#endif
      window->show();
      return m_application->exec() == 0;
    }

    virtual ~qt_tool()
    {
      delete m_application;
    }
};

} // namespace qt
} // namespace gui
} // namespace mcrl2

#endif // MCRL2_UTILITIES_QT_TOOL_H
