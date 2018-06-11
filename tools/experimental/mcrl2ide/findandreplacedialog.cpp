#include "findandreplacedialog.h"
#include "ui_findandreplacedialog.h"

FindAndReplaceDialog::FindAndReplaceDialog(CodeEditor *codeEditor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindAndReplaceDialog)
{
    ui->setupUi(this);

    this->codeEditor = codeEditor;

    setWindowFlags(Qt::Window);

    connect(ui->textToFind, SIGNAL(textChanged(QString)), this, SLOT(setFindEnabled()));
    connect(codeEditor, SIGNAL(selectionChanged()), this, SLOT(setReplaceEnabled()));

    connect(ui->findButton, SIGNAL(clicked()), this, SLOT(actionFind()));
    connect(ui->replaceButton, SIGNAL(clicked()), this, SLOT(actionReplace()));
    connect(ui->replaceAllButton, SIGNAL(clicked()), this, SLOT(actionReplaceAll()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void FindAndReplaceDialog::showMessage(QString message, bool error)
{
    if (error) {
        ui->infoLabel->setStyleSheet("color:red");
    } else {
        ui->infoLabel->setStyleSheet("color:green");
    }
    ui->infoLabel->setText(message);
}

void FindAndReplaceDialog::setFindEnabled()
{
    ui->findButton->setEnabled(ui->textToFind->text().count() > 0);
}

void FindAndReplaceDialog::setReplaceEnabled()
{
    ui->replaceButton->setEnabled(codeEditor->textCursor().selectedText() == ui->textToFind->text());
}

void FindAndReplaceDialog::actionFind()
{
    bool back = ui->upRadioButton->isChecked();
    QString toSearch = ui->textToFind->text();
    bool result = false;

    QTextDocument::FindFlags flags;

    if (back) {
        flags |= QTextDocument::FindBackward;
    }
    if (ui->caseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (ui->wholeCheckBox->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }

    result = codeEditor->find(toSearch, flags);

    /* if found, we are done */
    if (result) {
        showMessage("");
        return;
    }

    /* if the string was not found, try to wrap around begin/end of file */
    QTextCursor originalPosition = codeEditor->textCursor();
    codeEditor->moveCursor(back ? QTextCursor::End : QTextCursor::Start);
    result = codeEditor->find(toSearch, flags);

    /* if found, we are done and tell the user that we have wrapped around */
    if(result)
    {
        if (back) {
            showMessage("Found the last occurrence");
        } else {
            showMessage("Found the first occurence");
        }
        return;
    }

    /* if the string was still not found, it is not in the text */
    showMessage("No match found", true);
    codeEditor->setTextCursor(originalPosition);
}

void FindAndReplaceDialog::actionReplace()
{
    codeEditor->textCursor().insertText(ui->textToReplace->text());
    actionFind();
}

void FindAndReplaceDialog::actionReplaceAll()
{
    QTextCursor originalPosition = codeEditor->textCursor();

    codeEditor->moveCursor(QTextCursor::Start);
    actionFind();

    int i = 0;
    while (codeEditor->textCursor().hasSelection()) {
        codeEditor->textCursor().insertText(ui->textToReplace->text());
        actionFind();
        i++;
    }

    showMessage(tr("Replaced %1 occurrence(s)").arg(i));

    codeEditor->setTextCursor(originalPosition);
}

FindAndReplaceDialog::~FindAndReplaceDialog()
{
    delete ui;
}
