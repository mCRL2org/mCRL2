#ifndef PROJECT_MANAGER_H_
#define PROJECT_MANAGER_H_

#include <list>

#include "specification.h"

class ToolManager;

static Specification primary_specification(true);
static Specification generated_specification(false);

/*
 * A project is a collection of specifications objects along with meta
 * information about those specifications and how specifications that are not
 * initially in the project can be obtained by applying tools to specifications
 * that are in the project.
 *
 */
class ProjectManager {
  private:

    std::list < Specification > specifications;

    /* Keeps the smallest natural number that has not yet been assigned as identifier */
    unsigned int free_identifier;

    std::string  project_root;
    std::string  project_description;

    /* Writes project configuration in XML format to stream */
    bool Write(std::ostream& stream = std::cout);

  public:

    ProjectManager();

    /* Set a project directory */
    void SetProjectDirectory(std::string directory);

    /* Get project directory */
    std::string GetProjectDirectory();

    /* Set project description */
    void SetDescription(std::string description);

    /* Get project description */
    const std::string GetDescription();

    /* Get a pointer to the list of specifications in this project */
    const std::list < Specification >* GetSpecifications();

    /* Close project, writing out changes if necessary */
    bool Close();

    /* Read project information from project_directory */
    bool Load();

    /* Write project information to storage */
    bool Store();

    /* Write project information to stream */
    void Print(std::ostream& stream = std::cerr);

    /* Add a new specification to the project */
    Specification& Add(Specification& specification = generated_specification) throw (Specification&);

    /* Remove a specification from the project */
    bool Remove(const Specification* specification);

    /* Specifications are required to be present in the list */
    bool Remove(const std::vector < Specification* >& some_specifications);

    /* Remove instance from a specification from storage */
    bool Flush(Specification* specification);

    /* Update information about a specification that is already in the project */
    bool Update(Specification* specification);

    /* Make all specifications in the project up to date */
    bool UpdateAll();
};

#endif
