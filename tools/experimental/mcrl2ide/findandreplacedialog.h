#ifndef FINDANDREPLACEDIALOG_H
#define FINDANDREPLACEDIALOG_H

#include <QDialog>
#include "codeeditor.h"

namespace Ui {
class FindAndReplaceDialog;
}

class FindAndReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief FindAndReplaceDialog Constructor
     * @param codeEditor The editor to find/replace in
     * @param parent The parent of this widget
     */
    explicit FindAndReplaceDialog(CodeEditor *codeEditor, QWidget *parent = 0);
    ~FindAndReplaceDialog();

public slots:
    /**
     * @brief textToFindChanged Is called when the text in the find field changes
     * Enables or disables the find button
     */
    void setFindEnabled();

    /**
     * @brief setReplaceEnabled Is called when the selection in the text editor has changed
     * Enables or disables the replace button
     */
    void setReplaceEnabled();

    /**
     * @brief actionFind Allows the user to find a string in the editor
     */
    void actionFind();

    /**
     * @brief actionReplace Allows the user to replace a string in the editor
     */
    void actionReplace();

    /**
     * @brief actionReplaceAll Allows the user to replace all occurences of a string in the editor
     */
    void actionReplaceAll();

private:
    Ui::FindAndReplaceDialog *ui;

    CodeEditor *codeEditor;

    void showMessage(QString message, bool error = false);
};

#endif // FINDANDREPLACEDIALOG_H
