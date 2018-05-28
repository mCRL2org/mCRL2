#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>

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
    PropertyWidget(QString name, QString text, PropertiesDock *parent);


    /**
     * @brief getPropertyName Gets the name of the property of this widget
     * @return The name of the property
     */
    QString getPropertyName();

    /**
     * @brief getPropertyText Gets the text of the property of this widget
     * @return The text of the property
     */
    QString getPropertyText();

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
     * @brief actionEdit Allows the user to edit this property
     */
    void actionEdit();

    /**
     * @brief actionDelete Allows the user to delete this property
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
