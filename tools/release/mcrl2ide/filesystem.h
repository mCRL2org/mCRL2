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
#include <QSettings>
#include <QDateTime>
#include <QDomDocument>

enum class LtsReduction
{
  None = 0,
  StrongBisimulation = 1,
  BranchingBisimulation = 2
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

  /**
   * @brief Property Default constructor
   */
  Property();

  /**
   * @brief Property Constructor
   * @param name The name of the property
   * @param text The text of the property
   */
  Property(QString name, QString text);

  /**
   * @brief operator== Defines equality on two properties
   * @param property The property to compare to
   * @return Whether this property and the given property are equal
   */
  bool operator==(const Property& property) const;

  /**
   * @brief operator!= Defines inequality on two properties
   * @param property The property to compare to
   * @return Whether this property and the given property are not equal
   */
  bool operator!=(const Property& property) const;
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
   * @param settings The application settings
   * @param parent The main widget (main window)
   */
  FileSystem(CodeEditor* specificationEditor, QSettings* settings,
             QWidget* parent);

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
   * @param evidence Whether this is an evidence lps
   * @param propertyName The property name in case this is an evindence lps
   * @return The file path of the lps
   */
  QString lpsFilePath(bool evidence = false, const QString& propertyName = "");

  /**
   * @brief ltsFilePath Defines the file path of a lts
   * @param reduction The reduction applied to the lts
   * @param evidence Whether this is an evidence lts
   * @param propertyName The property name in case this is an evindence lts
   * @return The file path of the lts
   */
  QString ltsFilePath(LtsReduction reduction, bool evidence = false,
                      const QString& propertyName = "");

  /**
   * @brief propertyFilePath Defines the file path of a property
   * @param propertyName The name of the property
   * @return The file path of the property
   */
  QString propertyFilePath(const QString& propertyName);

  /**
   * @brief pbesFilePath Defines the file path of a pbes
   * @param propertyName The name of the property this pbes correspeonds to
   * @param evidence Whether the pbes has evidence info
   * @return The file path of the pbes
   */
  QString pbesFilePath(const QString& propertyName, bool evidence = false);

  /**
   * @brief toolPath Defines the path of a tool executable
   * @param tool The name of the tool
   * @return The path of a tool executable
   */
  QString toolPath(const QString& tool);

  /**
   * @brief parentFolderPath Returns the path the the folder that is the parent
   *   of the given folder
   * @param folderPath The folder to get the parent of
   * @return The path the the folder that is the parent of the given folder
   */
  QString parentFolderPath(const QString& folderPath);

  /**
   * @brief getProjectName Gets the name of the current project
   * @returns The name of the current project or the empty string if no project
   *   is opened
   */
  QString getProjectName();

  /**
   * @brief getSpecificationFileName Gets the name of the specificationFile
   * @return The name of the specificationFile or the empty string if no
   *   specification file is known
   */
  QString getSpecificationFileName();

  /**
   * @brief getProperties Gets the properties of the current project
   * @return The properties of the current project
   */
  std::list<Property> getProperties();

  /**
   * @brief projectOpened Checks whether a project is opened
   * @return Whether a project is opened
   */
  bool projectOpened();

  /**
   * @brief inSpecificationOnlyMode Checks whether the IDE is in specification
   *   only mode
   * @return Whether the IDE is in specification only mode
   */
  bool inSpecificationOnlyMode();

  /**
   * @brief specificationModified Checks whether the specification has been
   *   modified since it has been saved
   * @return Whether the specification has been modified since it has been saved
   */
  bool isSpecificationModified();

  /**
   * @brief isSpecificationModifiedFromOutside Checks whether the specification
   *   has been modified from outside the IDE since it was saved using the IDE
   *   or since it was last modified from outside
   * @return Whether the specification has been modified from outside
   */
  bool isSpecificationNewlyModifiedFromOutside();

  /**
   * @brief propertyNameExists Checks whether the given property name already
   *   exists
   * @param propertyName The property name to check for
   * @return Whether the given property name already exists
   */
  bool propertyNameExists(const QString& propertyName);

  /**
   * @brief upToDateLpsFileExists Checks whether an lps file exists that is
   *   created from the current specification
   * @param evidence Whether this is an evidence lps
   * @param propertyName The property name in case this is an evidence lps
   * @return Whether an lps file exists that is created from the current
   *   specification
   */
  bool upToDateLpsFileExists(bool evidence = false,
                             const QString& propertyName = "");

  /**
   * @brief upToDateLtsFileExists Checks whether an lts file exists with a given
   *   reduction that is created from the current specification
   * @param reduction The reduction that was applied to the lts
   * @param evidence Whether this is an evidence lts
   * @param propertyName The property name in case this is an evidence lts
   * @return Whether an lts file exists that is created from the current
   *   specification
   */
  bool upToDateLtsFileExists(LtsReduction reduction, bool evidence = false,
                             const QString& propertyName = "");

  /**
   * @brief upToDatePbesFileExists Checks whether a pbes file exists that is
   *   created from the current specification and property
   * @param propertyName The name of the property
   * @param evidence Whether the pbes has evidence info
   * @return Whether a pbes file exists that is created from the current
   *   specification and property
   */
  bool upToDatePbesFileExists(const QString& propertyName,
                              bool evidence = false);

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
   * @param askToSave Whether the user should be asked to save before creating a
   *   new project if the specification has been modified
   * @param forNewProject Whether this is done for "New Project" or "Save
   *   Project As"
   * @return Whether creating a new project was successfull
   */
  bool newProject(bool askToSave = true, bool forNewProject = true);

  /**
   * @brief loadSpecification Loads the specification from its file and puts it
   *   in the editor
   * @param loadSpecFilePath The path to the specification (optional)
   * @return Whether the specification file could be found
   */
  bool loadSpecification(QString loadSpecFilePath = "");

  /**
   * @brief openProjectFromArgument Opens a project from the input argument
   * @param InputFilePath The input argument
   */
  void openFromArgument(const QString& InputFilePath);

  /**
   * @brief openProjectFromFolder Opens a project from a given project folder
   * @param newProjectFolderPath The path to the project folder
   */
  void openProjectFromFolder(const QString& newProjectFolderPath);

  /**
   * @brief openProject Opens a project chosen by the user
   */
  void openProject();

  /**
   * @brief newProperty Adds a new property
   * @param property The new property to add
   * @param cleanPropertiesFolder Whether obsolete property files should be
   *   removed
   */
  void newProperty(const Property& property, bool cleanPropertiesFolder = true);

  /**
   * @brief importProperties Imports properties from file
   * @return The imported properties
   */
  std::list<Property> importProperties();

  /**
   * @brief editProperty Edits an existing property
   * @param oldProperty The property to be edited
   * @param newProperty The property after editing
   */
  void editProperty(const Property& oldProperty, const Property& newProperty);

  /**
   * @brief deleteProperty Deletes an existing property
   * @param property The property to delete
   * @return Whether the property has been deleted
   */
  bool deleteProperty(const Property& property);

  /**
   * @brief save Saves the project to file
   * @param forceSave Whether the files should be saved even if they have not
   *   been modified
   * @return Whether saving was successfull
   */
  bool save(bool forceSave = false);

  /**
   * @brief saveAs Saves the project to file under a new name
   * @return Whether saving was successfull
   */
  bool saveAs();

  /**
   * @brief saveProperty Saves a property to file
   * @param property The property to save
   */
  void saveProperty(const Property& property);

  /**
   * @brief actionOpenProjectFolderInExplorer Allows the user to open the
   *   project folder in the native file explorer
   */
  void openProjectFolderInExplorer();

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
   * @brief enterSpecificationOnlyMode Is emitted wheneverthe IDE is opened
   *   with a specification as argument
   */
  void enterSpecificationOnlyMode();

  /**
   * @brief newProjectOpened Is emitted whenever a new project has been openend
   */
  void newProjectOpened();

  private:
  QString projectFileExtension = ".mcrl2proj";
  QString projectFolderPath;
  QString specFilePath;
  QString propertiesFolderName = "properties";
  QTemporaryDir temporaryFolder;

  QWidget* parent;
  CodeEditor* specificationEditor;
  QSettings* settings;

  QString projectName;
  bool projectOpen;
  QDomDocument projectOptions;
  std::list<Property> properties;
  bool specificationModified;
  bool specificationOnlyMode;
  QDateTime lastKnownSpecificationModificationTime;

  /**
   * @brief makeSureProjectFolderExists Checks whether the properties folder
   *   exists, if not creates it
   */
  void makeSurePropertiesFolderExists();

  /**
   * @brief updateSpecificationModificationTime Updates the specification
   *   modification time to what it is according to the OS
   */
  void updateSpecificationModificationTime();

  /**
   * createFileDialog Creates a file dialog that can be used to ask the user for
   *   a file (location)
   * @param type What type of file window is needed: 0 = new project, 1 = save
   *   project as, 2 = open project, 3 = import property
   * @return The file dialog
   */
  QFileDialog* createFileDialog(int type);

  /**
   * @brief createNewProjectOptions Creates a DOM document containg the project
   *   options, assumes that the project state is already set
   * @return The DOM document containg the project options
   */
  QDomDocument createNewProjectOptions();

  /**
   * @brief updateProjectFile Saves the project options to the project file
   * assumes that a project is open
   */
  void updateProjectFile();

  /**
   * @brief convertProjectFileToNewFormat Converts a project file to the new
   *   project file format
   * Old format: "SPEC <path to spec>", new format: DOM structure
   * This method should be removed in the future
   * @param newProjectFolderPath The path to the project project folder
   * @param newProjectFilePath The path to the project file
   * @param oldFormat The contents of the project file in the old format
   * @return The new format
   */
  QDomDocument convertProjectFileToNewFormat(QString newProjectFolderPath,
                                             QString newProjectFilePath,
                                             QString oldFormat);

  /**
   * @brief readPropertyFromFile Reads a property from a file
   * @param propertyFilePath The path to the property file
   * @return The property read from the file
   */
  Property readPropertyFromFile(const QString& propertyFilePath);

  /**
   * @brief deletePropertyFile Deletes the file of a property
   * @param propertyName The name of the property
   * @param showIfFailed Whether the user should be told if deleting a property
   *   file was unsuccessful
   * @return Whether deleting the property file was successful
   */
  bool deletePropertyFile(const QString& propertyName,
                          bool showIfFailed = true);
};

#endif // FILESYSTEM_H
