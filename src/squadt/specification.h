#ifndef SPECIFICATION_H
#define SPECIFICATION_H

#include <string>
#include <vector>
#include <iosfwd>
#include <ctime>

#include "md5/md5pp.h"

/*
 * A specification is a container that specifies the dependencies on other
 * specifications and what tool is used to generate a set of new `output objects'.
 * Potentially an output object is a new specification, but the current
 * visualisation does not support it. A future more advanced visualisation
 * might, so the data is already gathered here.
 *
 * A better concept than specification would be Processor, because what is
 * captured here are input/output relations. This does not fit the current
 * visualisation at all so we stick to the name specification.
 */

#define UNSPECIFIED_TOOL UINT_MAX

namespace xml2pp {
  class text_reader;
}

namespace squadt {
  class Specification;
  class ToolManager;

  /* Type that is used to keep the status of a specification */
  typedef enum {
    non_existent,         /* outputs do not exist */
    being_computed,       /* outputs are being generated */
    not_up_to_date,       /* outputs exist but are not up to date */
    up_to_date            /* outputs exist and are up-to-date */
  } SpecificationStatus;

  /* Type to hold information about input objects */
  typedef struct {
    union {
      Specification* pointer;
      unsigned int   identifier;
    }                 derived_from;  /* Specification (with output objects) from which these outputs are derived */
    unsigned int      output_number; /* The number of an output object */
  } SpecificationInputType;

  /* Type to hold information about output objects */
  typedef struct {
    std::string         format;
    std::string         location;
    md5::compact_digest checksum;   /* Checksum for the object */
    std::time_t         timestamp;  /* Time just before the last checksum was computed */
  } SpecificationOutputType;

  /* Interface class that contains functions for processing GUI update events */
  class SpecificationVisualiser {
    public:
      /* Callback function for updating a user interface */
      inline virtual void VisualiseStatusChange(SpecificationStatus) {
      }
 
      virtual ~SpecificationVisualiser() {
      }
  };

  /*
   * A specification is either provided, in this case it is not generated from
   * other specifications, or generated.
   *
   * A generated specification is build using some tool with a particular
   * configuration from some input objects. The result is one or more output
   * objects that represent this specification on storage.
   */
  class Specification {
    friend class ProjectManager;
    friend class ToolManager;
 
    private:
 
      std::string  name;                        /* A name for the specification (need not be unique) */
      unsigned int identifier;                  /* A number that uniquely identifies this specification */
      std::string  tool_configuration;          /* A string of command line arguments that should be passed at tool execution */
      unsigned int tool_mode;                   /* What mode the tool is configured to run in */
      unsigned int tool_identifier;             /* Identifies the tool that is required to run the command */
      std::string  description;                 /* Optional description */
 
      std::vector < SpecificationInputType >  input_objects;   /* Storage objects that this specification depends on */
      std::vector < SpecificationOutputType > output_objects;  /* Storage objects that this specification is comprised of */
 
      bool         consistent;                  /* Whether the data is consistent w.r.t. the data stored in the project file */
 
      /* Whether the specification is up to date */
      SpecificationStatus status;
 
      /* Object used to update visualisations of a specification */
      SpecificationVisualiser* visualiser;
 
      /* dummy specification initialiser, for when no visualisation is needed */
      static SpecificationVisualiser dummy_visualiser;
 
      /* Recursively checks whether specification is up to date (ignores the set status) */
      SpecificationStatus CheckStatus();
 
      /* Checks whether the status of files matches that of the specification object */
      inline bool CheckInstances();
 
      /* Remove specification from storage */
      bool Delete();
 
      /* Read from XML using a libXML2 reader */
      bool Read(xml2pp::text_reader& reader) throw (int);
 
      /* Write as XML to stream */
      bool Write(std::ostream& stream = std::cout);
 
      /* Store changes to the configuration of this specification to storage (TODO) */
      bool Commit();
 
      void SetStatus(SpecificationStatus);
 
    public:
 
      Specification(bool, SpecificationVisualiser* avisualiser = &dummy_visualiser);
 
      /** Generate the specification (instantiation on storage) */
      bool Generate() throw (void*);
 
      inline void SetVisualiser(SpecificationVisualiser* avisualiser);
 
      /** Get name */
      inline std::string GetName() const;
 
      /** Set name */
      inline void SetName(std::string new_name);
 
      /** Get tool configuration */
      inline std::string GetToolConfiguration() const;
 
      /** Set tool configuration */
      inline void SetToolConfiguration(std::string new_configuration);
 
      /** Get tool mode of operation */
      inline const unsigned int GetToolMode() const;
 
      /** Set tool mode of operation */
      inline void SetToolMode(unsigned int new_mode);
 
      /** Get tool identifier */
      inline const unsigned int GetToolIdentifier() const;
 
      /** Set tool idenfier */
      inline void SetToolIdentifier(unsigned int new_identifier);
 
      /** Get description */
      inline std::string GetDescription() const;
 
      /** Set description */
      inline void SetDescription(std::string new_description);
 
      /** Get input objects */
      inline std::vector < SpecificationInputType >& GetInputObjects();
 
      /** Get number of input objects */
      inline unsigned int GetNumberOfInputObjects() const;
 
      /** Get output objects */
      inline std::vector < SpecificationOutputType >& GetOutputObjects();
 
      /** Get number output objects */
      inline unsigned int GetNumberOfOutputObjects() const;
 
      /** Returns the value of uptodate */
      inline bool IsUpToDate() const;
 
      /** Sets status to one of the not-available ones and removes the instances of specification from storage */
      inline void ResetAvailability();
 
      /** If a specification is up-to-date this toggles the state to */
      inline void ToggleRegeneration();
 
      /** Regenerate */
      inline void ForceRegeneration();
 
      /** Recursively set the status of specification and */
      inline const SpecificationStatus GetStatus() const;
 
      /** Pretty prints the fields of the specification */
      void Print(std::ostream& stream = std::cerr) const;
  };

  inline void Specification::SetVisualiser(SpecificationVisualiser* avisualiser) {
    visualiser = avisualiser;
  }

  inline std::string Specification::GetName() const {
    return (name);
  }

  inline void Specification::SetName(std::string new_name) {
    consistent &= (name == new_name);

    name = new_name;
  }

  inline std::string Specification::GetToolConfiguration() const {
    return (tool_configuration);
  }

  inline void Specification::SetToolConfiguration(std::string new_configuration) {
    consistent &= (tool_configuration == new_configuration);

    tool_configuration = new_configuration;
  }

  inline const unsigned int Specification::GetToolMode() const {
    return (tool_mode);
  }

  inline void Specification::SetToolMode(unsigned int new_mode) {
    consistent &= (tool_mode == new_mode);

    tool_mode = new_mode;
  }

  inline const unsigned int Specification::GetToolIdentifier() const {
    return (tool_identifier);
  }

  inline void Specification::SetToolIdentifier(unsigned int new_identifier) {
    consistent &= (identifier == new_identifier);

    tool_identifier = new_identifier;
  }

  inline std::string Specification::GetDescription() const {
    return (description);
  }

  inline void Specification::SetDescription(std::string new_description) {
    consistent &= (description == new_description);

    description = new_description;
  }

  inline std::vector < SpecificationInputType >& Specification::GetInputObjects() {
    /* Potentially violates consistency */
    consistent = false;

    return (input_objects);
  }

  inline unsigned int Specification::GetNumberOfInputObjects() const {
    return (input_objects.size());
  }

  inline std::vector < SpecificationOutputType >& Specification::GetOutputObjects() {
    /* Potentially violates consistency */
    consistent = false;

    return (output_objects);
  }

  inline unsigned int Specification::GetNumberOfOutputObjects() const {
    return (output_objects.size());
  }

  inline bool Specification::IsUpToDate() const {
    return (input_objects.size() == 0 || status == up_to_date);
  }

  inline void Specification::ResetAvailability() {
    /* TODO remove all output objects from storage */
    status = non_existent;

    visualiser->VisualiseStatusChange(status);
  }

  inline void Specification::ToggleRegeneration() {
    if (status == up_to_date) {
      status = not_up_to_date;
    }
    else {
      status = CheckStatus();
    }

    visualiser->VisualiseStatusChange(status);
  }

  inline void Specification::ForceRegeneration() {
    if (status == up_to_date) {
      status = not_up_to_date;
    }
    else {
      status = non_existent;
    }

    visualiser->VisualiseStatusChange(status);
  }

  inline const SpecificationStatus Specification::GetStatus() const {
    return(status);
  }
}

#endif

