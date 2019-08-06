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
#include "utilities.h"
#include "mcrl2/lts/lts_equivalence.h"

#include <QObject>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QSettings>
#include <QDateTime>
#include <QDomDocument>

enum IntermediateFileType
{
  Lps = 0,
  Lts = 1,
  Pbes = 2
};

const std::map<IntermediateFileType, QString> INTERMEDIATEFILETYPENAMES = {
    {IntermediateFileType::Lps, "LPS"},
    {IntermediateFileType::Lts, "LTS"},
    {IntermediateFileType::Pbes, "PBES"}};

class Property
{
  public:
  QString name;
  QString text;
  bool mucalculus;
  mcrl2::lts::lts_equivalence equivalence;

  /**
   * @brief Property Default constructor
   */
  Property();

  /**
   * @brief Property Constructor
   * @param name The name of the property
   * @param text The text of the property
   * @param mucalculus Whether this is a mu-calculus property (true) or an
   *   equivalence property (false)
   * @param equivalence The equivalence in case this is an equivalence property
   */
  Property(QString name, QString text, bool mucalculus = true,
           mcrl2::lts::lts_equivalence equivalence = mcrl2::lts::lts_eq_none);

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
   * @return The path of the properties folder
   */
  QString propertiesFolderPath();

  /**
   * @brief artefactsFolderPath Defines the file path of the artefacts folder,
   *   which is a folder in the project folder used for intermediate files
   * @return The path of the artefacts folder
   */
  QString artefactsFolderPath();

  /**
   * @brief intermediateFilesFolderPath Defines the file path of the
   *   intermediate files folder. This can be the temporary folder or the
   *   artefacts folder, depending on the options.
   * @param fileType For which file type we want to know the intermediate files
   *   folder
   * @return The file path of the intermediate files folder for the given file
   *   type
   */
  QString intermediateFilesFolderPath(IntermediateFileType fileType);

  /**
   * @brief defaultSpecificationFilePath Defines the default file path of a
   *   specification
   * @return The default file path of the specification
   */
  QString defaultSpecificationFilePath();

  /**
   * @brief specificationFilePath Defines the file path of a specification
   * @param propertyName The property name in case this is a generated
   *   specification to compare against
   * @return The file path of the specification
   */
  QString specificationFilePath(const QString& propertyName = "");

  /**
   * @brief lpsFilePath Defines the file path of a lps
   * @param propertyName The property name in case this is an evidence lps
   * @param evidence Whether this is an evidence lps
   * @return The file path of the lps
   */
  QString lpsFilePath(const QString& propertyName = "", bool evidence = false);

  /**
   * @brief ltsFilePath Defines the file path of a lts
   * @param equivalence The equivalence reduction applied to the lts
   * @param propertyName The property name in case this is an evidence lts
   * @param evidence Whether this is an evidence lts
   * @return The file path of the lts
   */
  QString
  ltsFilePath(mcrl2::lts::lts_equivalence equivalence = mcrl2::lts::lts_eq_none,
              const QString& propertyName = "", bool evidence = false);

  /**
   * @brief propertyFilePath Defines the file path of a property
   * @param property The property
   * @param forParsing Whether we want to use the file to parse the property
   * @return The file path of the property
   */
  QString propertyFilePath(const Property& property, bool forParsing = false);

  /**
   * @brief pbesFilePath Defines the file path of a pbes
   * @param propertyName The name of the property this pbes corresponds to
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
   * @brief parentFolderPath Returns the path to the folder that is the parent
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
   * @brief getSpecificationFileName Gets the name of the specification file
   * @return The name of the specification file or the empty string if no
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
   * @brief isSpecificationModified Checks whether the specification has been
   *   modified since it has been saved
   * @return Whether the specification has been modified since it has been saved
   */
  bool isSpecificationModified();

  /**
   * @brief isSpecificationNewlyModifiedFromOutside Checks whether the
   *   specification has been modified from outside the IDE since it was saved
   *   using the IDE or since it was last modified from outside
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
   * @brief upToDateOutputFileExists Checks whether there already exists an up
   *   to date output file, given one or two input files
   * @param inputFile An input file
   * @param outputFile The output file
   * @param inputFile2 An optional second input file
   * @return Whether there already exists an up to date output file
   */
  bool upToDateOutputFileExists(const QString& inputFile,
                                const QString& outputFile,
                                const QString& inputFile2 = "");

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
   * @return Whether creating a new project was successful
   */
  bool newProject(bool askToSave = true, bool forNewProject = true);

  /**
   * @brief loadSpecification Loads the specification from its file and puts it
   *   in the editor
   * @param specPath The path to the specification (optional)
   * @return Whether the specification file could be found
   */
  bool loadSpecification(QString specPath = "");

  /**
   * @brief openProjectFromArgument Opens a project from the input argument
   * @param inputFilePath The input argument
   */
  void openFromArgument(const QString& inputFilePath);

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
   */
  void newProperty(const Property& property);

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
   * @brief deletePropertyFile Deletes the file of a property from the
   *   properties folder
   * @param property The property whose file to delete
   */
  void deletePropertyFile(const Property& property);

  /**
   * @brief deleteProperty Deletes an existing property
   * @param property The property to delete
   */
  void deleteProperty(const Property& property);

  /**
   * @brief save Saves the current project
   * @param forceSave Whether the files should be saved even if they have not
   *   been modified
   * @return Whether saving was successful
   */
  bool save(bool forceSave = false);

  /**
   * @brief saveAs Saves the current project under a new name
   * @return Whether saving was successful
   */
  bool saveAs();

  /**
   * @brief saveProperty Saves a property to file
   * @param property The property to save
   * @param forParsing Whether the saved property file will be used for parsing
   */
  void saveProperty(const Property& property, bool forParsing = false);

  /**
   * @brief createReinitialisedSpecification Creates a new mCRL2 specification
   *   which is a copy of the current one but with its initial process replaced
   *   by a given one; assumes a specification is open
   * @param property The equivalence property for which the new specification
   *   needs to be created
   * @param forParsing Whether we want to use the file to parse the property
   */
  void createReinitialisedSpecification(const Property& property,
                                        bool forParsing);

  /**
   * @brief openProjectFolderInExplorer Allows the user to open the project
   *   folder in the native file explorer
   */
  void openProjectFolderInExplorer();

  /**
   * @brief removeTemporaryFolder Removes the temporary folder and its contents
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
   * @brief setSaveIntermediateFilesOptions Sets the options on whether
   *   intermediate files need to be saved
   * @param checked Whether an options was checked or unchecked
   */
  void setSaveIntermediateFilesOptions(bool checked);

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
  QString artefactsFolderName = "artefacts";
  QTemporaryDir temporaryFolder;

  QWidget* parent;
  CodeEditor* specificationEditor;
  QSettings* settings;
  std::map<IntermediateFileType, bool> saveIntermediateFilesOptions;

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
   * @brief makeSureArtefactsFolderExists Checks whether the artefacts folder
   *   exists, if not creates it
   */
  void makeSureArtefactsFolderExists();

  /**
   * @brief updateSpecificationModificationTime Updates the specification
   *   modification time to what it is according to the OS
   */
  void updateSpecificationModificationTime();

  /**
   * @brief createFileDialog Creates a file dialog that can be used to ask the
   *   user for a file (location)
   * @param type What type of file window is needed: 0 = new project, 1 = save
   *   project as, 2 = open project, 3 = import property
   * @return The file dialog
   */
  QFileDialog* createFileDialog(int type);

  /**
   * @brief createNewProjectOptions Creates a DOM document containing the
   *   project options, assumes that the project state is already set
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
  QDomDocument
  convertProjectFileToNewFormat(const QString& newProjectFolderPath,
                                const QString& newProjectFilePath,
                                const QString& oldFormat);

  /**
   * @brief readSpecification Reads the contents of a specification
   * @param specPath The file path of the specification; picks the currently
   *   opened specification if empty
   * @return The contents of the specification if successful, an error string
   *   otherwise
   */
  QString readSpecification(QString specPath = "");

  /**
   * @brief propertyFilePath Find the file path of a property file with the
   *   given property name
   * @param propertyName The property name
   * @return The file path of the property if found, the empty string otherwise
   */
  QString findPropertyFilePath(const QString& propertyName);

  /**
   * @brief readPropertyFromFile Reads a property from a file
   * @param propertyFilePath The path to the property file
   * @param context For what purpose the propertyFile is read
   * @return The property read from the file
   */
  Property readPropertyFromFile(const QString& propertyFilePath,
                                const QString& context);

  /**
   * @brief removePropertyFromProjectFile Removes a property from the project
   *   file, assumes a project is open or being opened
   * @param propertyName The name of the property to remove
   */
  void removePropertyFromProjectFile(const QString& propertyName);
};

#endif // FILESYSTEM_H
