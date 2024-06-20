// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TOOLOPTIONSDIALOG_H
#define TOOLOPTIONSDIALOG_H

#include <filesystem.h>

#include <QDialog>

namespace Ui
{
  class ToolOptionsDialog;
};

/**
 * @brief The AddEditPropertyDialog class defines the dialog used to add or edit
 *   a property
 */
class ToolOptionsDialog : public QDialog
{
  Q_OBJECT

public:
    ToolOptionsDialog(QWidget* parent, FileSystem* fileSystem);

public slots:  
    /**
     * Updates the tool options when a new project was loaded.
     */
    void updateToolOptions();

private slots:

    /**
     * Saves the tool options.
     */
    void saveToolOptions();

    /**
     * Reset the tool options to their default values.
     */
    void resetToolOptions();

private:  
  Ui::ToolOptionsDialog* ui;

  FileSystem* m_fileSystem;

};

#endif // TOOLOPTIONSDIALOG_H