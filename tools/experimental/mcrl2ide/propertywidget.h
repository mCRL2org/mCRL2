#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>

class PropertiesDock;

/**
 * @brief The PropertyWidget classdefines a widget for a single property in the properties dock
 */
class PropertyWidget : public QWidget
{
    Q_OBJECT

public:
    PropertyWidget(QString name, QString text, PropertiesDock *parent);


    /**
     * @brief getPropertyName gets the name of the property of this widget
     * @return the name of the property
     */
    QString getPropertyName();

    /**
     * @brief getPropertyText gets the text of the property of this widget
     * @return the text of the property
     */
    QString getPropertyText();

    /**
     * @brief setPropertyName set the name of the property of this widget
     * @param name the new name of the property
     */
    void setPropertyName(QString name);

    /**
     * @brief setPropertyText set the text of the property of this widget
     * @param text the new text of the property
     */
    void setPropertyText(QString text);

public slots:
    /**
     * @brief actionVerify allows the user to verify this property
     */
    void actionVerify();

    /**
     * @brief actionEdit allows the user to edit this property
     */
    void actionEdit();

    /**
     * @brief actionDelete allows the user to delete this property
     */
    void actionDelete();

private:
    PropertiesDock *parent;
    QString name;
    QString text;
    QHBoxLayout *propertyLayout;
    QLabel *propertyNameLabel;
    QPushButton *verifyButton;

    void setToDefault();
};

#endif // PROPERTYWIDGET_H
