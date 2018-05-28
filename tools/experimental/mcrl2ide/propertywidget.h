#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>

class PropertiesDock;

class PropertyWidget : public QWidget
{
    Q_OBJECT

public:
    PropertyWidget(QString name, QString text, PropertiesDock *parent);

    QString getPropertyName();
    QString getPropertyText();
    void setPropertyName(QString name);
    void setPropertyText(QString text);

public slots:
    void actionVerify();
    void actionEdit();
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
