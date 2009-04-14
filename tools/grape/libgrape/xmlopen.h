// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xmlopen.h
//
// Implements functions to convert an XML file into a grape specification.

#ifndef LIBGRAPE_XMLOPEN_H
#define LIBGRAPE_XMLOPEN_H

#include <wx/xml/xml.h>
#include <wx/dynarray.h>
#include <iostream>

namespace grape
{
  namespace libgrape
  {
    /**
     * Compound state retrieval function.
     * @param p_id The identifier of the desired compound state.
     * @return Returns the compound state that has the specified id, if it exists (0 if not).
     */
    compound_state* find_compound_state( process_diagram* p_proc_dia, unsigned int p_id );

    /**
     * Compound reference retrieval function.
     * @param p_id The identifier of the desired compound reference.
     * @return Returns the compound reference that has the specified id, if it exists (0 if not).
     */
    compound_reference* find_compound_reference( architecture_diagram* p_arch_dia, unsigned int p_id );

    /**
     * XML to GraPE Specification convertor function.
     * Takes an XML file and converts it to a GraPE Specification .
     * @param p_spec The GraPE Specification to be created.
     * @param p_filename The filename of the xml-document that is read in.
     * @param p_max_id The maximum id found in the xml-document.
     * @return The converted XML document.
     */
    bool xml_open( grape_specification* p_spec, const wxString &p_filename, long &p_max_id );

    /**
     * Subfunction of xml_open that retrieves the datatypespecification.
     * @param p_spec The GraPE Specification to be created.
     * @param p_dat_spec_node The node containing the datatypespecification.
     * @return Returns whether the datatype specification was succesfully opened.
     */
    bool open_datatype_specification( grape_specification* p_spec, wxXmlNode* p_dat_spec_node );

    /**
     * Subfunction of xml_open that recreates all process diagrams.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_list_node The node containing the list of process diagrams.
     * @return Returns whether the process diagrams were succesfully recreated.
     */
    bool open_process_diagrams( grape_specification* p_spec, wxXmlNode* p_proc_list_node );

    /**
     * Subfunction of xml_open that recreates all states.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the states.
     * @param p_proc_dia_ptr A pointer to the diagram the states are to be added to.
     * @return Returns whether the states were succesfully recreated.
     */
    bool open_states( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all reference states.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the reference states.
     * @param p_proc_dia_ptr A pointer to the diagram the reference states are to be added to.
     * @return Returns whether the reference states were succesfully recreated.
     */
    bool open_reference_states( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all nonterminating transitions.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the nonterminating transitions.
     * @param p_proc_dia_ptr A pointer to the diagram the nonterminating transitions are to be added to.
     * @return Returns whether the nonterminating transitions were succesfully recreated.
     */
    bool open_nonterminating_transitions( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all terminating transitions.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the terminating transitions.
     * @param p_proc_dia_ptr A pointer to the diagram the terminating transitions are to be added to.
     * @return Returns whether the terminating transitions were succesfully recreated.
     */
    bool open_terminating_transitions( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all initial designators.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the initial designators.
     * @param p_proc_dia_ptr A pointer to the diagram the initial designators are to be added to.
     * @return Returns whether the initial designators were succesfully recreated.
     */
    bool open_initial_designators( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all comments.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the comments.
     * @param p_proc_dia_ptr A pointer to the process diagram the comments are to be added to.
     * @return Returns whether the comments were succesfully recreated.
     */
    bool open_comments( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all architecture diagrams.
     * @param p_spec The GraPE Specification to be created.
     * @param p_arch_list_node The node containing the list of architecture diagrams.
     * @return Returns whether the architecture diagrams were succesfully recreated.
     */
    bool open_architecture_diagrams( grape_specification* p_spec, wxXmlNode* p_arch_list_node );

    /**
     * Subfunction of xml_open that recreates all process references.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the process references.
     * @param p_proc_dia_ptr A pointer to the diagram the process references are to be added to.
     * @return Returns whether the process references were succesfully recreated.
     */
    bool open_process_references( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all architecture references.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the architecture references.
     * @param p_proc_dia_ptr A pointer to the diagram the architecture references are to be added to.
     * @return Returns whether the architecture references were succesfully recreated.
     */
    bool open_architecture_references( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all channels.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the channels.
     * @param p_proc_dia_ptr A pointer to the diagram the channels are to be added to.
     * @return Returns whether the channels were succesfully recreated.
     */
    bool open_channels( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all channel communications.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the channel communications.
     * @param p_proc_dia_ptr A pointer to the diagram the channel communications are to be added to.
     * @return Returns whether the channel communications were succesfully recreated.
     */
    bool open_channel_communications( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all visibles.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the visibles.
     * @param p_proc_dia_ptr A pointer to the diagram the visibles are to be added to.
     * @return Returns whether the visibles were succesfully recreated.
     */
    bool open_visibles( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all blockeds.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the blockeds.
     * @param p_proc_dia_ptr A pointer to the diagram the blockeds are to be added to.
     * @return Returns whether the blockeds were succesfully recreated.
     */
    bool open_blockeds( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr );

    /**
     * Subfunction of xml_open that recreates all comments.
     * @param p_spec The GraPE Specification to be created.
     * @param p_proc_dia_node The node containing the information needed for the comments.
     * @param p_proc_dia_ptr A pointer to the architecture diagram the comments are to be added to.
     * @return Returns whether the comments were succesfully recreated.
     */
    bool open_comments( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr );
  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_XMLOPEN_H
