// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PROPERTIESDOCK_H
#define PROPERTIESDOCK_H

#include "propertywidget.h"
#include "processsystem.h"

#include <QDockWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>

/**
 * @brief The PropertiesDock class defines the dock that contains the properties
 */
class PropertiesDock : public QDockWidget
{
  Q_OBJECT

  public:
  const Qt::DockWidgetArea defaultArea = Qt::RightDockWidgetArea;

  /**
   * @brief PropertiesDock Constructor
   * @param processSystem The process system
   * @param fileSystem The file system
   * @param parent The parent of this widget
   */
  PropertiesDock(ProcessSystem* processSystem, FileSystem* fileSystem,
                 QWidget* parent);
  ~PropertiesDock();

  /**
   * @brief setToNoProperties Empties the dock
   */
  void setToNoProperties();

  /**
   * @brief addProperty Adds a property to the dock
   * @param property The property to add
   */
  void addProperty(const Property& property);

  /**
   * @brief verifyAllProperties Verifies all properties in this dock
   */
  void verifyAllProperties();

  public slots:
  /**
   * @brief keepContentsLeft When resizing the properties dock, this will make
   *   sure that the contents stay to the left as much as possible whenever a
   *   horizontal scroll bar is needed
   */
  void keepContentsLeft(int min, int max);

  /**
   * @brief deleteProperty Removes a property from the dock
   * @param propertyWidget The property widget to remove
   */
  void deletePropertyWidget(PropertyWidget* propertyWidget);

  /**
   * @brief resetAllPropertyWidgets Resets all property widgets to unverified
   *   state
   */
  void resetAllPropertyWidgets();

  private:
  ProcessSystem* processSystem;
  FileSystem* fileSystem;

  QScrollArea* innerDockWidget;
  QScrollBar* hScrollBar;
  int previousHorizontalScrollbarMaximum;
  QVBoxLayout* propertiesLayout;
  std::list<PropertyWidget*> propertyWidgets;
};

#endif // PROPERTIESDOCK_H
