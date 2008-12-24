// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2gen.h
//
// Declares the functions to convert process diagrams and architecture diagrams to mCRL2.

#ifndef MCRL2GEN_MCRL2GEN_H
#define MCRL2GEN_MCRL2GEN_H

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include "libgrape/preamble.h"

namespace grape
{
  namespace mcrl2gen
  {

    /**
     * Initialization function for the mCRL2 libraries.
     * Initializes the mCRL2 libraries and sets the correct parameters.
     * @param p_argc The number of command line arguments.
     * @param p_argv Command line arguments.
     * @pre True.
     * @post The mCRL2 libraries have been initialized and are ready for use.
     */
    void init_mcrl2libs(int p_argc, char** p_argv);

    /**
     * XML specification validation function.
     * Validates an XML GraPE specification and produces error messages if necessary.
     * @pre True.
     * @post The XML specification is validated and error messages are produced if necessary.
     * @param p_spec The specification to validate.
     * @return True if the specification is valid, false otherwise.
     */
    bool validate(wxXmlDocument &p_spec);

    /**
     * XML process diagram validation function.
     * Validates an XML process diagram and produces error messages if necessary.
     * @pre p_spec is a valid reference to a GraPE XML specification and p_diagram_id is a valid reference to a process diagram contained in p_spec.
     * @post The process diagram is validated and error messages are produced if necessary.
     * @param p_spec The GraPE XML specification containing the diagram to validate.
     * @param p_diagram_id The identifier of the process diagram to validate.
     * @return True if the diagram is valid, false otherwise.
     */
    bool validate_process_diagram(wxXmlDocument &p_spec, wxString &p_diagram_id);

    /**
     * XML architecture diagram validation function.
     * Validates an XML architecture diagram and produces error messages if necessary.
     * @pre p_spec is a valid reference to a GraPE XML specification and p_diagram is a valid reference to an architecture diagram contained in p_spec.
     * @post The architecture diagram is validated and error messages are produced if necessary.
     * @param p_spec The GraPE XML specification containing the diagram to validate.
     * @param p_diagram_id The identifier of the architecture diagram to validate.
     * @return True if the diagram is valid, false otherwise
     */
    bool validate_architecture_diagram(wxXmlDocument &p_spec, wxString &p_diagram_id);

    /**
     * XML specification convertion function.
     * Converts all spaces in the names of objects to underscores.
     * @pre p_spec is a valid reference to a valid XML specification.
     * @post All spaces in the names of oject in the XML specification have been replaced by underscores.
     * @param p_spec The specification to convert.
     */
    void convert_spaces(wxXmlDocument &p_spec);

    /**
     * Process diagram to mcrl2 export function.
     * Exports an XML GraPE specification, containing at least one process diagram and the initial parameter assignment, to an mCRL2 file.
     * @pre The XML GraPE specification contains at least one process diagram and the initial parameter assignment.
     * @post The XML specification is validated and saved to an mCRL2 file or error messages are produced.
     * @param p_spec The specification to export.
     * @param p_filename The filename of the generated mCRL2 file.
     * @param p_diagram_id The identifier of the diagram to be exported.
     * @param p_parameters_init The initial value of the parameters of the process diagram to be exported.
     * @param p_verbose Flag to set for verbose output.
     * @param p_save Flag to set if the output has to be saved to the specified file.
     * @return True if the specification is exported successfully, false otherwise.
     */
    bool export_process_diagram_to_mcrl2(wxXmlDocument &p_spec, wxString &p_filename, wxString &p_diagram_id, grape::libgrape::list_of_decl_init &p_parameters_init, bool p_verbose = false, bool p_save = true);

    /**
     * Architecture diagram to mcrl2 export function.
     * Exports an XML GraPE specification, containing at least one architecture diagram, to an mCRL2 file.
     * @pre The XML GraPE specification contains at least one architecture diagram.
     * @post The XML specification is validated and saved to an mCRL2 file or error messages are produced.
     * @param p_spec The specification to export.
     * @param p_filename The filename of the generated mCRL2 file.
     * @param p_diagram_id The identifier of the diagram to be exported.
     * @param p_verbose Flag to set for verbose output.
     * @param p_save Flag to set if the output has to be saved to the specified file.
     * @return True if the specification is exported successfully, false otherwise.
     */
    bool export_architecture_diagram_to_mcrl2(wxXmlDocument &p_spec, wxString &p_filename, wxString &p_diagram_id, bool p_verbose = false, bool p_save = true);



    void test_export(void);

  } // namespace mcrl2gen
} // namespace grape

#endif // MCRL2GEN_MCRL2GEN_H
