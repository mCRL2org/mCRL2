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
    {LtsReduction::StrongBisimulation, "StrongBisimulation"},
    {LtsReduction::BranchingBisimulation, "BranchingBisimulation"},
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
   * @parent The main widget (main window)
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
   * @param projectName The name of the current project
   * @return The file path of the project folder
   */
  QString projectFolderPath(QString projectName);

  /**
   * @brief propertiesFolderPath Defines the file path of the current properties
   *   folder
   * @param projectName The name of the current project
   * @return The file path of the properties folder
   */
  QString propertiesFolderPath(QString projectName);

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
   * @brief propertyModified Checks whether the property has been modified since
   *   it has been saved
   * @param propertyName The property
   * @return Whether the property has been modified since it has been saved
   */
  bool isPropertyModified(Property* property);

  /**
   * @brief setPropertyModified Sets the property to modified
   * @param PropertyName The property
   */
  void setPropertyModified(Property* property);

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
   * @brief newProject Creates a new project with the corresponding file
   *   structure
   * @param context Whether this is done for "Create Project" or "Save Project
   *   As"
   * @return The name of the new project, is "" if failed
   */
  QString newProject(QString context = "New Project");

  /**
   * @brief newProperty Creates a new property
   * @return The new property, is NULL if failed
   */
  Property* newProperty();

  /**
   * @brief editProperty Edits an existing property
   * @param oldProperty The property to be edited
   * @return The edited property, the old property if failed
   */
  Property* editProperty(Property* oldProperty);

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

  public slots:
  /**
   * @brief setSpecificationModified Sets the specification to modified
   */
  void setSpecificationModified();

  signals:
  /**
   * @brief modified Is emitted whenever the specification or a property is
   *   changed
   */
  void hasChanges(bool changes);

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
