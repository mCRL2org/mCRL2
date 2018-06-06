#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include "propertiesdock.h"
#include "processsystem.h"
#include "propertywidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QProcess>
#include <QStackedWidget>

class PropertiesDock;

/**
 * @brief The PropertyWidget class defines a widget for a single property in the properties dock
 */
class PropertyWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief PropertyWidget Constructor
     * @param name The name of the property
     * @param text The text of the property
     * @param parent The parent of this widget
     */
    PropertyWidget(QString name, QString text, ProcessSystem *processSystem, PropertiesDock *parent);


    /**
     * @brief getPropertyName Gets the property of this widget
     * @return The property
     */
    Property *getProperty();

    /**
     * @brief setPropertyName Set the name of the property of this widget
     * @param name The new name of the property
     */
    void setPropertyName(QString name);

    /**
     * @brief setPropertyText Set the text of the property of this widget
     * @param text The new text of the property
     */
    void setPropertyText(QString text);

public slots:
    /**
     * @brief actionVerify Allows the user to verify this property
     */
    void actionVerify();

    /**
     * @brief actionAbortVerification Allows the user to abort the verification
     */
    void actionAbortVerification();

    /**
     * @brief actionEdit Allows the user to edit this property
     */
    void actionEdit();

    /**
     * @brief actionDelete Allows the user to delete this property
     */
    void actionDelete();

protected:
    void paintEvent(QPaintEvent *pe);

private:
    ProcessSystem *processSystem;
    PropertiesDock *parent;
    Property *property;
    QHBoxLayout *propertyLayout;
    QLabel *propertyNameLabel;
    QStackedWidget *verificationWidgets;

    QProcess *mcrl22lpsProcess;
    QProcess *lps2pbesProcess;
    QProcess *pbes2boolProcess;

    void setToDefault();

private slots:
    /**
     * @brief actionVerify2 The second step of verification, creating the pbes
     * @param exitStatus How the process was exited
     */
    void actionVerify2();

    /**
     * @brief actionVerify3 The third step of verification, solving the pbes
     */
    void actionVerify3();

    /**
     * @brief actionVerifyResult Applies the result of the verification
     */
    void actionVerifyResult();
};

#endif // PROPERTYWIDGET_H
