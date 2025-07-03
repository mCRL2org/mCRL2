// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_PERSISTENTFILEDIALOG_H
#define MCRL2_UTILITIES_PERSISTENTFILEDIALOG_H

#include <QFileDialog>

namespace mcrl2::gui::qt
{

class PersistentFileDialog : QObject
{
    Q_OBJECT
public:
    explicit PersistentFileDialog(QString directory = QString(), QWidget *parent = nullptr);

    QString getExistingDirectory(const QString& caption = QString(), 
                                 QFileDialog::Options options = QFileDialog::ShowDirsOnly);
    QString getOpenFileName(const QString& caption = QString(), 
                            const QString& filter = QString(), 
                            QString* selectedFilter = 0, 
                            QFileDialog::Options options = QFileDialog::Options());
    QStringList getOpenFileNames(const QString& caption = QString(), 
                                 const QString& filter = QString(), 
                                 QString* selectedFilter = 0, 
                                 QFileDialog::Options options = QFileDialog::Options());
    QString  getSaveFileName(const QString& caption = QString(), 
                             const QString& filter = QString(), 
                             QString* selectedFilter = 0, 
                             QFileDialog::Options options = QFileDialog::Options());

private:
    QWidget* m_parent;
    QString m_directory;

};

} // namespace mcrl2::gui::qt

#endif // MCRL2_UTILITIES_PERSISTENTFILEDIALOG_H
