#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "codeeditor.h"
#include "consoledock.h"

#include <QDir>
#include <QProcess>

/**
 * @brief The FileSystem class handles all file related operations, including execution of mCRL2 tools
 */
class FileSystem
{
public:    
    /**
     * @brief FileSystem Constructor
     * @param specificationEditor The specification editor in the main window
     */
    explicit FileSystem(CodeEditor *specificationEditor);

    /**
     * @brief setConsoleDock Assigns the console dock to the file system for logging
     * @param consoleDock The console dock
     */

    void setConsoleDock(ConsoleDock *consoleDock);
    /**
     * @brief projectOpened Returns whether a project is opened
     * @return Whether a project is opened
     */
    bool projectOpened();

    /**
     * @brief getExecutablesFolder Gets the folder containing the mCRL2 tool executables
     * @return The folder with the executables
     */
    const QDir *getExecutablesFolder();

    /**
     * @brief getCurrentSpecification Gets the specification that is in the specification editor
     * @return the current specification ion the specification editor
     */
    QString getCurrentSpecification();

    /**
     * @brief newProject Creates a new project with the corresponding file structure
     */
    void newProject(QString projectName);

    /**
     * @brief openProject Opens the project with the given project name
     */
    void openProject(QString projectName);

    /**
     * @brief saveProject Saves the current specification
     */
    void saveSpecification();

    /**
     * @brief saveProperty Saves the given property
     * @param propertyName The name of the property
     * @param propertyText The text of the property
     */
    void saveProperty(QString propertyName, QString propertyText);

    /**
     * @brief saveProjectAs Saves the current project under a different name
     */
    void saveProjectAs(QString projectName);


    /**
     * @brief mcrl22lps Executes mcrl22lps on the current specification
     * @param verification Determines what console dock tab to use
     */
    QProcess *mcrl22lps(bool verification);

    /**
     * @brief lpsxsim Executes lpsxsim on the lps that corresponds to the current specification
     */
    QProcess *lpsxsim();

    /**
     * @brief lps2lts Executes lps2lts on the lps that corresponds to the current specification
     */
    QProcess *lps2lts();

    /**
     * @brief ltsconvert Executes ltsconvert on the lts that corresponds to the current specification
     */
    QProcess *ltsconvert();

    /**
     * @brief lps2pbes Executes lps2pbes on the lps that corresponds to the current specification and the given property
     * @param propertyName The name of the property to include
     */
    QProcess *lps2pbes(QString propertyName);

    /**
     * @brief pbes2bool Executes pbes2bool on the pbes that corresponds to the current specification and the given property
     * @param propertyName The name of the property to include
     */
    QProcess *pbes2bool(QString propertyName);

private:
    const QDir *projectsFolder = new QDir("projects");
    const QString propertiesFolderName = "properties";
    const QDir *executablesFolder = new QDir("C:\\Users\\s123188\\Documents\\QtProjects\\mCRL2-IDE\\execs");

    bool projectOpen;
    CodeEditor *specificationEditor;
    ConsoleDock *consoleDock;
    QString projectName;
    QDir *projectFolder;
    QDir *propertiesFolder;

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
};

#endif // FILESYSTEM_H
