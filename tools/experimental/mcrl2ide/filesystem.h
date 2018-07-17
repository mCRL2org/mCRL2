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
#include <QFileDialog>
#include <QStandardPaths>
#include <QTemporaryDir>

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
  FileSystem(CodeEditor* specificationEditor, QWidget* parent);

  /**
   * @brief makeSureProjectFolderExists Checks whether the properties folder
   *   exists, if not creates it
   */
  void makeSurePropertiesFolderExists();

  /**
   * @brief projectFilePath Defines the file path for the project file,
   *   which contains some info and indicates whether the parent folder is a
   *   valid mcrl2 project folder
   * @return The file path of the project file
   */
  QString projectFilePath();

  /**
   * @brief propertiesFolderPath Defines the file path of the current properties
   *   folder
   * @return The file path of the properties folder
   */
  QString propertiesFolderPath();

  /**
   * @brief defaultSpecificationFilePath Defines the default file path of a
   *   specification
   * @return The default file path of the specification
   */
  QString defaultSpecificationFilePath();

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
   * createFileDialog Creates a file dialog that can be used to ask the user for
   *   a file (location)
   * @param type What type of file window is needed
   *  0 = new project, 1 = save project as, 2 = open project
   * @return The file dialog
   */
  QFileDialog* createFileDialog(int type);

  /**
   * @brief createProjectFile Create a project file to store some info and to
   *   indicate that the parent folder is a valid mcrl2 project folder
   */
  void createProjectFile();

  /**
   * @brief newProject Creates a new project with the corresponding file
   *   structure
   * @param askToSave Whether the user should be asked to save before creating a
   *   new project if the specification has been modified
   * @param forNewProject Whether this is done for "New Project" or "Save
   *   Project As"
   * @return The name of the new project, is "" if failed
   */
  QString newProject(bool askToSave = true, bool forNewProject = true);

  /**
   * @brief deletePropertyFile Deletes the file of a property
   * @param propertyName The name of the property
   * @param showIfFailed Whether the user should be told if deleting a property
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
   * @param newProjectName A pointer to store the projectname of the opened
   *   project
   * @param newProperties A pointer to store the properties of the opened
   *   project
   */
  void openProject(QString* newProjectName,
                   std::list<Property*>* newProperties);

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

  /**
   * @brief clearTemporaryFolder Removes the tomporary folder and its contents
   */
  void removeTemporaryFolder();

  public slots:
  /**
   * @brief setSpecificationModified Sets the specification to modified or
   *   unmodified
   * @param modified Whether the specification was modified
   */
  void setSpecificationModified(bool modified);

  /**
   * @brief deleteUnlistedPropertyFiles Deletes all property files for which
   *   there is no entry in the "properties" list
   */
  void deleteUnlistedPropertyFiles();

  signals:
  /**
   * @brief changesSaved Is emitted whenever changes in the specification have
   *   been saved
   */
  void changesSaved();

  private:
  QString projectFolderPath;
  QString specFilePath;
  QString propertiesFolderName = "properties";
  QTemporaryDir temporaryFolder;

  QWidget* parent;
  CodeEditor* specificationEditor;
  QString projectName;
  bool projectOpen;
  std::list<Property*> properties;

  bool specificationModified;
  std::map<QString, bool> propertyModified;
};

#endif // FILESYSTEM_H
