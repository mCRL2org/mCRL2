#include "addeditpropertydialog.h"
#include "propertiesdock.h"
#include "propertywidget.h"

#include <QMessageBox>
#include <QSpacerItem>
#include <QStyleOption>
#include <QPainter>

PropertyWidget::PropertyWidget(QString name, QString text, FileSystem *fileSystem, PropertiesDock *parent) : QWidget(parent)
{
    this->fileSystem = fileSystem;
    this->parent = parent;
    this->name = name;
    this->text = text;

    /* create the label for the property name */
    propertyNameLabel = new QLabel(name);

    /* create the verify button */
    QPushButton *verifyButton = new QPushButton();
    verifyButton->setIcon(QIcon(":/icons/verify.png"));
    verifyButton->setIconSize(QSize(24, 24));
    verifyButton->setStyleSheet("border:none;");
    connect(verifyButton, SIGNAL(clicked()), this, SLOT(actionVerify()));

    /* create the abort button for when a property is being verified */
    QPushButton *abortButton = new QPushButton();
    abortButton->setIcon(QIcon(":/icons/abort.png"));
    abortButton->setIconSize(QSize(24, 24));
    abortButton->setStyleSheet("border:none;");
    connect(abortButton, SIGNAL(clicked()), this, SLOT(actionAbortVerification()));

    /* create the label for when a property is true */
    QPixmap *trueIcon = new QPixmap(":/icons/true.png");
    QLabel *trueLabel = new QLabel();
    trueLabel->setPixmap(trueIcon->scaled(QSize(24, 24)));

    /* create the label for when a property is false */
    QPixmap *falseIcon = new QPixmap(":/icons/false.png");
    QLabel *falseLabel = new QLabel();
    falseLabel->setPixmap(falseIcon->scaled(QSize(24, 24)));

    /* stack the verification widgets */
    verificationWidgets = new QStackedWidget(this);
    verificationWidgets->setMaximumSize(QSize(30, 30));
    verificationWidgets->addWidget(verifyButton); /* index = 0 */
    verificationWidgets->addWidget(abortButton);  /* index = 1 */
    verificationWidgets->addWidget(trueLabel);    /* index = 2 */
    verificationWidgets->addWidget(falseLabel);   /* index = 3 */
    verificationWidgets->setCurrentIndex(0);

    /* create the edit button */
    QPushButton *editButton = new QPushButton();
    editButton->setIcon(QIcon(":/icons/edit.png"));
    editButton->setIconSize(QSize(24, 24));
    editButton->setStyleSheet("border:none;");
    connect(editButton, SIGNAL(clicked()), this, SLOT(actionEdit()));

    /* create the delete button */
    QPushButton *deleteButton = new QPushButton();
    deleteButton->setIcon(QIcon(":/icons/delete.png"));
    deleteButton->setIconSize(QSize(24, 24));
    deleteButton->setStyleSheet("border:none;");
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(actionDelete()));

    /* lay them out */
    propertyLayout = new QHBoxLayout();
    propertyLayout->addWidget(propertyNameLabel);
    propertyLayout->addStretch();
    propertyLayout->addWidget(verificationWidgets);
    propertyLayout->addWidget(editButton);
    propertyLayout->addWidget(deleteButton);

    this->setLayout(propertyLayout);
    this->setMaximumHeight(40);
}

void PropertyWidget::paintEvent(QPaintEvent *pe) {
  QStyleOption o;
  o.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

QString PropertyWidget::getPropertyName()
{
    return name;
}

QString PropertyWidget::getPropertyText()
{
    return text;
}

void PropertyWidget::setPropertyName(QString name)
{
    this->name = name;
    propertyNameLabel->setText(name);
}

void PropertyWidget::setPropertyText(QString text)
{
    this->text = text;
}

void PropertyWidget::saveProperty()
{
    fileSystem->saveProperty(name, text);
}

void PropertyWidget::actionVerify()
{
    /* save the property */
    saveProperty();

    /* change the button */
    verificationWidgets->setCurrentIndex(1);

    /* create the lps */
    mcrl22lpsProcess = fileSystem->mcrl22lps(true);
    /* if we don't need to run this, move to the next step, else wait until it is finished */
    if (mcrl22lpsProcess == NULL) {
        actionVerify2();
    } else {
        connect(mcrl22lpsProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(actionVerify2()));
    }
}

void PropertyWidget::actionVerify2()
{
    /* create the pbes */
    lps2pbesProcess = fileSystem->lps2pbes(name);
    /* if we don't need to run this, move to the next step, else wait until it is finished */
    if (lps2pbesProcess == NULL) {
        actionVerify3();
    } else {
        connect(lps2pbesProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(actionVerify3()));
    }
}

void PropertyWidget::actionVerify3()
{
    /* extract the result */
    pbes2boolProcess = fileSystem->pbes2bool(name);
    connect(pbes2boolProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(actionVerifyResult()));
}

void PropertyWidget::actionVerifyResult()
{
    /* get the result (between the last and second last newline character) */
    std::string output = pbes2boolProcess->readAllStandardOutput().toStdString();
    if (output.find("true") == 0) {
        verificationWidgets->setCurrentIndex(2);
        this->setStyleSheet("background-color:rgb(153,255,153)");
    } else if (output.find("false") == 0) {
        verificationWidgets->setCurrentIndex(3);
        this->setStyleSheet("background-color:rgb(255,153,153)");
    } else {
        verificationWidgets->setCurrentIndex(0);
    }
}

void PropertyWidget::actionAbortVerification()
{
    qDebug("abort");
    /* first we disconnect the signal chaining so that abortion of one propcess does not start the next */
    //disconnect(mcrl22lpsProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(actionVerify2()));
    //disconnect(lps2pbesProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(actionVerify3()));
}

void PropertyWidget::actionEdit()
{
    AddEditPropertyDialog *editPropertyDialog = new AddEditPropertyDialog(false, parent, this, name, text);

    /* if editing was succesful (Edit button was pressed), update the property and its widget */
    if (editPropertyDialog->exec()) {
        name = editPropertyDialog->getPropertyName();
        text = editPropertyDialog->getPropertyText();
        propertyNameLabel->setText(name);
    }
}

void PropertyWidget::actionDelete()
{
    /* show a message box to ask the user whether he is sure to delete the property */
    QMessageBox *msgBox = new QMessageBox();
    msgBox->setText("Are you sure you want to delete the property " + name + "?");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    /* only delete the property if the user agrees */
    if (msgBox->exec() == QMessageBox::Yes) {
        parent->deleteProperty(this);
        delete this;
    }
}
