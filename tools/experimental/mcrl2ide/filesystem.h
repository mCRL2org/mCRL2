// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "codeeditor.h"

#include <QObject>
#include <QDir>

enum class LtsReduction
{
  None,
  StrongBisimulation,
  BranchingBisimulation
};

const std::map<LtsReduction, QString> LTSREDUCTIONNAMES = {
    {LtsReduction::None, "None"},
    {LtsReduction::StrongBisimulation, "Strong Bisimulation"},
    {LtsReduction::BranchingBisimulation, "Branching Bisimulation"},
};

class Property
{
  public:
  QString name;
  QString text;

  Property(QString name, QString text);

  bool equals(Property* property);
};

class Project
{
  public:
  QString projectName;
  std::list<Property*> properties;

  Project(QString projectName, std::list<Property*> properties);
};

/**
 * @brief The FileSystem class handles all file related operations
 */
class FileSystem : public QObject
{
  Q_OBJECT

  public:
  /**
   * @brief FileSystem Constructor
   * @param specificationEditor The specification editor in the main window
   * @param parent The main widget (main window)
   */
  explicit FileSystem(CodeEditor* specificationEditor, QWidget* parent);

  /**
   * @brief makeSureProjectFolderExists Checks whether the projects folder
   *   exists, if not creates it
   */
  void makeSureProjectsFolderExists();

  /**
   * @brief makeSureProjectFolderExists Checks whether the properties folder
   *   exists, if not creates it
   */
  void makeSurePropertiesFolderExists();

  /**
   * @brief projectFolderPath Defines the file path of the current project
   *   folder
   * @return The file path of the project folder
   */
  QString projectFolderPath();

  /**
   * @brief propertiesFolderPath Defines the file path of the current properties
   *   folder
   * @return The file path of the properties folder
   */
  QString propertiesFolderPath();

  /**
   * @brief specificationFilePath Defines the file path of a specification
   * @return The file path of the specification
   */
  QString specificationFilePath();

  /**
   * @brief lpsFilePath Defines the file path of a lps
   * @return The file path of the lps
   */
  QString lpsFilePath();

  /**
   * @brief ltsFilePath Defines the file path of a lts
   * @return The file path of the lts
   */
  QString ltsFilePath(LtsReduction reduction);

  /**
   * @brief propertyFilePath Defines the file path of a property
   * @param propertyName The name of the property
   * @return The file path of the property
   */
  QString propertyFilePath(QString propertyName);

  /**
   * @brief pbesFilePath Defines the file path of a pbes
   * @param propertyName The name of the property this pbes correspeonds to
   * @return The file path of the pbes
   */
  QString pbesFilePath(QString propertyName);

  /**
   * @brief projectOpened Checks whether a project is opened
   * @return Whether a project is opened
   */
  bool projectOpened();

  /**
   * @brief getCurrentSpecification Gets the specification that is in the
   *   specification editor
   * @return the current specification ion the specification editor
   */
  QString getCurrentSpecification();

  /**
   * @brief specificationModified Checks whether the specification has been
   *   modified since it has been saved
   * @return Whether the specification has been modified since it has been saved
   */
  bool isSpecificationModified();

  /**
   * @brief propertyNameExists Checks whether the given property name already
   *   exists
   * @param propertyName The property name to check for
   * @return Whether the given property name already exists
   */
  bool propertyNameExists(QString propertyName);

  /**
   * @brief upToDateLpsFileExists Checks whether an lps file exists that is
   *   created from the current specification
   * @return Whether an lps file exists that is created from the current
   *   specification
   */
  bool upToDateLpsFileExists();

  /**
   * @brief upToDateLtsFileExists Checks whether an lts file exists with a given
   *   reduction that is created from the current specification
   * @param reduction The reduction that was applied to the lts
   * @return Whether an lts file exists that is created from the current
   *   specification
   */
  bool upToDateLtsFileExists(LtsReduction reduction);

  /**
   * @brief upToDatePbesFileExists Checks whether a pbes file exists that is
   *   created from the current specification and property
   * @param propertyName The name of the property
   * @return Whether a pbes file exists that is created from the current
   *   specification and property
   */
  bool upToDatePbesFileExists(QString propertyName);

  /**
   * @brief setSpecificationEditorCursor Puts the cursor in the specification
   *   editor to the given location
   * @param row The row to put the cursor in
   * @param column The column to put the cursor in
   */
  void setSpecificationEditorCursor(int row, int column);

  /**
   * @brief newProject Creates a new project with the corresponding file
   *   structure
   * @param context Whether this is done for "Create Project" or "Save Project
   *   As"
   * @return The name of the new project, is "" if failed
   */
  QString newProject(QString context = "New Project");

  /**
   * @brief deletePropertyFile Deletes the file of a property
   * @param propertyName The name of the property
   * @param showIfFailed Whether the suer should be told if deleting a property
   *   file was unsuccessful
   * @return Whether deleting the property file was successful
   */
  bool deletePropertyFile(QString propertyName, bool showIfFailed = true);

  /**
   * @brief newProperty Adds a new property
   * @param property The new property to add
   */
  void newProperty(Property* property);

  /**
   * @brief editProperty Edits an existing property
   * @param oldProperty The property to be edited
   * @param newProperty The property after editing
   */
  void editProperty(Property* oldProperty, Property* newProperty);

  /**
   * @brief deleteProperty Deletes an existing property
   * @param property The property to delete
   * @return Whether the property has been deleted
   */
  bool deleteProperty(Property* property);

  /**
   * @brief openProject Opens the project with the given project name
   * @return The opened project, is NULL if failed
   */
  Project openProject();

  /**
   * @brief saveProject Saves the project to file
   * @param forceSave Whether the files should be saved even if they have not
   *   been modified
   * @return The new project name, is empty if failed
   */
  QString saveProject(bool forceSave = false);

  /**
   * @brief saveProjectAs Saves the project to file under a new name
   * @return The new project name, is empty if failed
   */
  QString saveProjectAs();

  /**
   * @brief saveProperty Saves a property to file
   * @param property The property to save
   */
  void saveProperty(Property* property);

  public slots:
  /**
   * @brief setSpecificationModified Sets the specification to modified
   */
  void setSpecificationModified();

  /**
   * @brief deleteUnlistedPropertyFiles Deletes all property files for which
   *   there is no entry in the "properties" list
   */
  void deleteUnlistedPropertyFiles();

  signals:
  /**
   * @brief modified Is emitted whenever the specification or a property is
   *   changed
   */
  void hasChanges(bool changes);

  /**
   * @brief changesSaved Is emitted whenever changes in the specification have
   *   been saved
   */
  void changesSaved();

  private:
  QString projectsFolderPath =
      QDir::currentPath() + QDir::separator() + "projects";
  QString propertiesFolderName = "properties";

  QWidget* parent;
  CodeEditor* specificationEditor;
  QString projectName;
  bool projectOpen;
  std::list<Property*> properties;

  bool specificationModified;
  std::map<QString, bool> propertyModified;
};

#endif // FILESYSTEM_H
