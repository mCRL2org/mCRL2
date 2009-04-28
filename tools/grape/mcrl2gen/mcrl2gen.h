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

#include "mcrl2gen_validate.h"

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
    void verbose_actions(list_of_action &p_actions);
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
    /**
     * XML process diagram to mCRL2 sort expression convertor function.
     * Extracts all state/reference state names of an XML process diagram
     * and constructs an mCRL2 sort expression containing a struct of all
     * encountered names.
     * @param p_process_diagram A valid XML process diagram node.
     * @return A string containing the constructed mCRL2 expression if conversion went successfully, the empty string otherwise.
     * @pre p_process_diagram is a valid XML process diagram.
     * @post A string containing the constructed mCRL2 expression is returned or the empty string is returned and error messages are produced.
     */
    wxString process_diagram_mcrl2_sort(wxXmlNode *p_process_diagram);
    /**
     * XML state identifier inference function.
     * Infers the identifier of a state (or process reference) in a process diagram, based on its name.
     * @param p_process_diagram The XML process diagram that contains the state to infer.
     * @param p_state_name The name of the state to infer.
     * @return The identifier of the inferred state (or process reference) or the empty string if the state was not found.
     * @pre p_process_diagram is a valid pointer to an XML process diagram and p_state_name is a valid reference to a state name.
     * @post The identifier of the inferred state (or process reference) is returned or the empty string is returned and error messages are produced.
     */
    wxString get_state_id(wxXmlNode *p_process_diagram, wxString &p_state_name);
    /**
     * XML state name inference function.
     * Infers the name of a state (or process reference) in a process diagram, based on its identifier.
     * @param p_process_diagram The XML process diagram that contains the state to infer.
     * @param p_id The identifier of the state to infer.
     * @param p_is_ref Reference to a boolean which indicates whether the inferred state is actually a process reference.
     * @return The name of the inferred state (or process reference) or the empty string if the state was not found.
     * @pre p_process_diagram is a valid pointer to an XML process diagram and p_id is a valid reference to an identifier.
     * @post The name of the inferred state (or process reference) is returned and p_is_ref is true if the inferred state is a process reference and false otherwise, or error messages are produced.
     */
    wxString get_state_name(wxXmlNode *p_process_diagram, wxString &p_id, bool &p_is_ref);
    /**
     * XML transition parsing function.
     * Parses an XML transition to construct an mCRL2 choice expression
     * for the internal mCRL2 process definition.
     * @param p_process_diagram The XML process diagram that contains the transition to be parsed.
     * @param p_transition The XML transition to parse.
     * @param p_declaration The mCRL2 process definition to parse the XML transition to.
     * @param p_alternative Boolean flag to indicate if the constructed mCRL2 process definition already has preceding choice expressions.
     * @param p_is_terminating Boolean flag to indicate if the transition to be parsed is terminating.
     * @param p_diagram_name The name of the process diagram that contains the transition to be parsed.
     * @param p_preamble_parameter_decls The preamble parameter declarations of the process diagram that contains the transition to be parsed.
     * @param p_preamble_local_var_decls The preamble local variable declarations of the process diagram that contains the transition to be parse.
     * @param datatype_spec The datatype specification.
     * @pre p_process_diagram is a valid pointer to an XML process diagram, p_transition is a valid pointer to an XML transition, p_declaration is a valid reference to an mCRL2 process definition, p_diagram_name is a valid process diagram name, p_preamble_parameter_decls is a valid reference to a parameter declaration and p_preamble_local_var_decls is a valid reference to a local variable declaration.
     * @post The transition is parsed and the results are appended to p_declaration or error messages are produced.
     */
    wxString transition_mcrl2(wxXmlNode *p_process_diagram, wxXmlNode *p_transition, bool p_alternative, wxString &p_diagram_name, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec);
    /**
     * Process diagram parameter sorting function.
     * Sorts a list of process diagram parameters, based on the sequence in which
     * they appear in the process diagram's preamble.
     * @param p_doc_root The root node of the XML specification.
     * @param p_diagram_name The name of the diagram that contains the parameters.
     * @param p_inits The parameters (with values) to sort.
     * @param datatype_spec The datatype specification.
     * @return The list of parameters (with values) sorted in the order in which the parameters appear in the process diagram's preamble.
     * @pre p_doc_root is a valid pointer to an XML node, p_diagram_name is a valid name of a process diagram, p_inits is a valid reference to a list of process diagram parameters.
     * @post The list of parameters (with values) is returned in sorted order or error messages are produced.
     */
    list_of_decl_init sort_parameters(wxXmlNode *p_doc_root, wxString &p_diagram_name, list_of_decl_init &p_inits, ATermAppl &datatype_spec);
    list_of_varupdate sort_parameters(wxXmlNode *p_doc_root, wxString &p_diagram_name, list_of_varupdate &p_inits, ATermAppl &datatype_spec);
    /**
     * XML process reference transition parsing function.
     * Parses a transition (which is not visible in the GraPE specification) for a process reference
     * of a process diagram to construct an mCRL2 choice expression for the internal mCRL2 process definition.
     * @param p_doc_root The root node of the XML specification.
     * @param p_reference_state The XML reference state to construct a hidden transition for.
     * @param p_diagram_name The name of the process diagram that contains the process reference.
     * @param p_declaration The mCRL2 process definition to parse the XML transition to.
     * @param p_alternative Boolean flag to indicate if the constructed mCRL2 process definition already has preceding choice expressions.
     * @param p_preamble_parameter_decls The preamble parameter declarations of the process diagram that contains the process reference to be parsed.
     * @param p_preamble_local_var_decls The preamble local variable declarations of the process diagram that contains the process reference to be parsed.
     * @pre p_doc_root is a valid pointer to an XML node, p_reference_state is a valid pointer to an XML reference state, p_diagram name is a valid process diagram name, p_declarations is a valid reference to an mCRL2 process definition, p_preamble_parameter_decls is a valid reference to a parameter declaration, p_preamble_local_var_decls is a valid reference to a loval variable declaration.
     * @post The hidden transition of the process reference is parsed and the results are appended to p_declarations or error messages are produced.
     */
    wxString transition_reference_mcrl2(wxXmlNode *p_doc_root, wxXmlNode *p_reference_state, bool p_alternative, wxString &p_diagram_name, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec);
    /**
     * XML process diagram to mCRL2 internal process specification convertor function.
     * Constructs an mCRL2 internal process specification out of a valid XML process diagram.
     * @param p_doc_root The root node of the XML specification.
     * @param p_process_diagram The XML process diagram to construct an internal process specification for.
     * @param p_preamble_parameter_decls The preamble's parameter declaration of the process diagram to construct an internal process specification for.
     * @param p_preamble_local_var_decls The preamble's local variable declarations of the process diagram to construct an internal process specification for.
     * @return The internal process specification or the empty string if construction failed.
     * @pre p_doc_root is a valid pointer to an XML node, p_process_diagram is a valid pointer to an XML process diagram, p_preamble_parameter_decls is a valid reference to a parameter declaration and p_preamble_local_var_decls is a valid reference to a local variable declaration.
     * @post The internal process specification is returned or the empty string is returned and error messages are produced.
     */
    wxString process_diagram_mcrl2_internal_proc(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec);
    /**
     * XML initial designator inference function.
     * Infers the initial designator of a process diagram.
     * @param p_process_diagram The process diagram to infer the initial designator of.
     * @return The name of the state (or process reference) of the process diagram that contains the initial designator.
     * @pre p_process_diagram is a valid pointer to an XML process diagram.
     * @post The name of the inferred state (or process reference) connected to the initial designator is returned or error messages are produced.
     */
    wxString initial_designator_mcrl2(wxXmlNode *p_process_diagram);
    /**
     * XML process diagram to mCRL2 process specification convertor function.
     * Constructs an mCRL2 process specification out of a valid XML process diagram.
     * @param p_process_diagram The XML process diagram to construct a process specification for.
     * @param p_preamble_parameter_decls The preamble's parameter declaration of the process diagram to construct a process specification for.
     * @param p_preamble_local_var_decls The preamble's local variable declaration of the process diagram to construct a process specification for.
     * @return The process specification or the empty string if construction failed.
     * @pre p_process diagram is a valid pointer to an XML process diagram, p_preamble_parameter_decls is a valid reference to a parameter declaration and p_preamble_local_var_decls is a valid reference to a local variable declaration.
     * @post The process specification is returned, or the empty string is returned and error messages are produced.
     */
    wxString process_diagram_mcrl2_proc(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls);
    /**
     * XML process diagram to mCRL2 initial process specification convertor function.
     * Constructs an mCRL2 initial process specification out of a parameter initialisation.
     * @param p_doc_root The root of a valid XML specification.
     * @param p_diagram_name The name of the exported process diagram.
     * @param p_parameter_init The parameter initialisation for the exported process diagram.
     * @param datatype_spec The datatype specification.
     * @return The initial process specification for the exported process diagram.
     * @pre p_doc_root is a valid pointer to an XML specification, p_diagram_name is a valid reference to a process diagram and p_parameter init is a valid reference to a parameter initialisation.
     * @post The initial process specification is returned or error messages are produced.
     */
    wxString process_diagram_mcrl2_init(wxXmlNode *p_doc_root, wxString &p_diagram_name, list_of_decl_init &p_parameter_init, ATermAppl &datatype_spec);
    /**
     * XML datatype specification to mCRL2 datatype specification convertor function.
     * Constructs an mCRL2 datatype specification out of an XML datatype specification.
     * @param p_doc_root The root of a valid XML specification.
     * @return The datatype specification extracted from the XML specification or the empty string if it is not present.
     * @pre p_doc_root is a valid XML specification.
     * @post The datatype specification is returned or the empty string is returned and error messages are produced.
     */
    wxString datatype_specification_mcrl2(wxXmlNode *p_doc_root);
    /**
     * Process reference inference function.
     * Infers the process references in a given process or architecture diagram.
     * @param p_diagram The XML process or architecture diagram to infer the process references for.
     * @return An array of identifiers of all process references in this diagram.
     * @pre p_diagram is a valid pointer to an XML process or architecture diagram.
     * @post An array of identifiers of all process references in p_diagram is returned or the empty array is returned and error messages are produced.
     */
    wxArrayString get_process_references(wxXmlNode *p_diagram);
    /**
     * Architecture reference inference function.
     * Infers the architecture references in a given architecture diagram.
     * @param p_diagram The XML architecture diagram to infer the architecture references for.
     * @return An array of identifiers of all architecture references in this diagram.
     * @pre p_diagram is a valid pointer to an XML architecture diagram.
     * @post An array of identifiers of all architecture references in p_diagram is returned or the empty array is returned and error messages are produced.
     */
    wxArrayString get_architecture_references(wxXmlNode *p_diagram);
    /**
     * Architecture diagram to mCRL2 possible actions convertor function.
     * Extracts all possible actions out of an architecture diagram and
     * adds them to an array.
     * @param p_doc_root The XML specification.
     * @param p_architecture_diagram The architecture diagram.
     * @param p_possibles An array containing all possible actions in this architecture diagram.
     * @pre p_doc_root is a valid pointer to an XML specification, p_architecture_diagram is a valid pointer to an XML architecture diagram and p_possibles is a valid reference to an array of actions.
     * @post All possible actions in this architecture diagram are stored in p_possibles, sorted by reference.
     */
    void architecture_diagram_mcrl2_actions(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, arr_action_reference &p_possibles, ATermAppl &datatype_spec);
    /**
     * Action list compacting function.
     * Removes duplicate actions from the list.
     * @param p_action The array of actions.
     * @pre p_action is a valid list of action.
     * @post All duplicates in p_actions are removed.
     */
    list_of_action compact_list_action(list_of_action &p_actions);
    /**
     * Channel of reference inference function.
     * Determines whether a given channel lies on a given reference in an architecture diagram.
     * @param p_architecture_diagram The XML architecture diagram which contains the reference.
     * @param p_channel_id The identifier of the channel.
     * @param p_reference_id The identifier of the reference supposed to contain the channel.
     * @return True if the channel lies on the given reference, false otherwise.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram, p_channel_id is a valid reference to a channel identifier and p_reference_id is a valid reference to a reference identifier.
     * @post A flag indicating whether the given channel lies on the given reference is returned or error messages are produced.
     */
    bool is_channel_of_reference(wxXmlNode *p_architecture_diagram, wxString &p_channel_id, wxString &p_reference_id);
    /**
     * Channel name inference function.
     * Infers the name of a given channel.
     * @param p_architecture_diagram The XML architecture diagram containing the channel.
     * @param p_channel_id The identifier of the channel.
     * @return The name of the channel or the empty string if p_channel_id is not valid.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram, p_channel_id is a valid reference to a channel identifier.
     * @post The name of the channel is returned or the empty string is returned and error messages are produced.
     */
    wxString get_channel_name(wxXmlNode *p_architecture_diagram, wxString &p_channel_id);
    /**
     * Reference channel inference function.
     * Infers the channels of a given reference.
     * @param p_architecture_diagram The XML architecture diagram containing the reference.
     * @param p_reference_id The identifier of the reference to infer the channels of.
     * @return An array of channel names, tupled with unique identifiers, containing the names of the channels present on the reference p_reference_id.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram and p_reference_id is a valid reference to a reference identifier.
     * @post An array containing the names and identifiers of all channels of the given reference is returned or error messages are produced.
     */
    arr_channel_id get_reference_channels(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, action_reference &p_reference, ATermAppl &datatype_spec);
    /**
     * Blocked channels inference function.
     * Infers the blocked channels of a given reference.
     * @param p_architecture_diagram The XML architecture diagram containing the channels and references.
     * @param p_reference_id The identifier of the reference to infer its blocked channels of.
     * @return An array of strings containing the names of the blocked channels of the reference.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram and p_reference_id is a valid reference to a reference identifier.
     * @post An array containing the names of the blocked channels associated with the reference is returned or error messages are produced.
     */
    wxArrayString get_reference_blocked_channels(wxXmlNode *p_architecture_diagram, wxString &p_reference_id);
    /**
     * Hidden actions inference function.
     * Infers the hidden actions of a reference.
     * @param p_actions The actions occuring inside the reference.
     * @param p_channels The channels on the reference.
     * @param p_blockeds The channels on the reference that are blocked.
     * @return An array of strings containing all the actions inside the reference that are hidden (not visible).
     * @pre p_actions is a valid reference to an array of actions, p_channels is a valid reference to an array of channels and p_blockeds is a valid reference to an array of blocked channels.
     * @post An array containing all hidden actions of this reference is returned.
     */
    list_of_action get_reference_hidden_actions(list_of_action &p_actions, arr_channel_id &p_channels, wxArrayString &p_blockeds);
    /**
     * Renamed actions inference function.
     * Infers the renamed actions of a reference.
     * @param p_channels The channels on the reference.
     * @param p_blockeds The channels on the reference that are blocked.
     * @return An array of channels with their unique identifiers of all actions that are to be renemaed.
     * @pre p_channels is a valid reference to an array of channels and p_blockeds is a valid reference to an array of blocked actions.
     * @post An array containing the names and unique identifiers of all actions which are to be renamed is returned.
     */
    arr_channel_id get_reference_renamed_actions(arr_channel_id &p_channels, wxArrayString &p_blockeds);
    /**
     * Channel communication to mCRL2 convertor function.
     * Extracts all channel communications out of an architecture diagram and matches them
     * with the already extracted channels.
     * @param p_architecture_diagram An XML architecture diagram.
     * @param p_renamed All renamed channels in the architecture diagram.
     * @return An array of channel communications in this diagram.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram and p_renamed is a valid reference to an array of renamed channels.
     * @post An array of channel communications in this diagram is returned or error messages are produced.
     */
    arr_channel_comm get_communications(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, arr_action_reference &p_refs, arr_channel_comm &p_blocked_comms, arr_channel_comm &p_hidden_comms, arr_channel_comm &p_visible_comms);
    /**
     * Blocked channel communication inference function.
     * Infers all the blocked channel communications inside a given architecture diagram.
     * @param p_architecture_diagram The XML architecture diagram containing the channel communications.
     * @param p_communications The array of channel communications inside the architecture diagram.
     * @return An array of channel communications that are blocked inside the given architecture diagram.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram containing the channel communications and p_communications is a valid reference to an array of channel communications inside the given XML architecture diagram.
     * @post An array of channel communications that are blocked inside the given architecture diagram is returned and error messages are produced if necessary.
     */
    arr_channel_comm get_communication_blocked(wxXmlNode *p_architecture_diagram, arr_channel_comm &p_communications);
    /**
     * Non-visible channel communications inference function.
     * Infers all non-visible channel communications in an architecture diagram.
     * @param p_architecture_diagram An XML architecture diagram.
     * @param p_communications An array of channel communications contained in the architecture diagram.
     * @return An array of channel communications which are not connected to the visibility frame of the architecture diagram.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram and p_communications is a valid reference to an array of channel communications.
     * @post An array containing all non-visible channel communications in the given architecture diagram is returned or error messages are produced.
     */
    arr_channel_comm get_communication_non_visible(wxXmlNode *p_architecture_diagram, arr_channel_comm &p_communications);
    /**
     * Blocked channel communications removal function.
     * Removes the blocked channel communications from an array of channel communications.
     * @param p_communications The array of channel communications.
     * @param p_blocked_communications The array of blocked channel communications.
     * @return An array of channel communications that contains the channel communications in p_communications that do not occur in p_blocked_communications is returned.
     * @pre p_communications is a valid reference to an array of channel communications and p_blocked_communications is a valid reference to an array of channel communications.
     * @post An array of channel communications that contains the channel communications in p_communications that do not occur in p_blocked_communications is returned.
     */
    arr_channel_comm remove_blocked_comms(arr_channel_comm &p_communications, arr_channel_comm &p_blocked_communications);
    /**
     * Architecture reference action fixing function.
     * Fixes actions for an architecture diagram, they need to be associated with visibles inside the actual diagram.
     * @param p_doc_root The XML GraPE specification.
     * @param p_refs The references to fix.
     * @pre p_doc_root is a valid pointer to an XML GraPE specification and p_refs is a valid reference to an array of references.
     * @post The references are fixed.
     */
    void fix_architecture_references(wxXmlNode *p_doc_root, arr_action_reference &p_refs);
    /**
     * Renamed channels and channel communications inference function.
     * Infers all channels and channel communications connected to the visibility frame (so they have to be renamed).
     * @param p_doc_root The root of the XML document.
     * @param p_architecture_diagram An XML architecture diagram.
     * @param p_refs An array of references containing the channels per contained reference.
     * @param p_communications An array of contained channel communications.
     * @return An array of "renameds", where each element contains both the old and new name of the renamed channel/channel-communication.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram, p_refs is a valid reference to an array of references and p_communications is a valid reference to an array of channel communications.
     * @post An array of "renameds" is returned or error messages are produced.
     */
    arr_renamed get_communication_channel_renamed(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, arr_action_reference &p_refs, arr_channel_comm &p_communications);
    /**
     * Process reference initialisation inference function.
     * Infers the parameter initialisation of a process reference in an architecture diagram.
     * @param p_doc_root An XML specification.
     * @param p_architecture_diagram An XML architecture diagram.
     * @param p_reference_id The identifier of the process reference to infer the parameter initialisation of.
     * @param p_reference_name The name of the process reference to infer the parameter initialisation of.
     * @return A list containing the parameter declarations and initialisations of the inferred process reference.
     * @pre p_doc_root is a valid pointer to an XML specification, p_architecture_diagram is a valid pointer to an XML architecture diagram, p_reference_id is a valid reference to an identifier of a process reference and p_reference_name is a valid reference to a name of a process reference.
     * @post A list containing the parameter declarations and initialisations of the inferred process reference is returned or error messages are produced.
     */
    list_of_varupdate get_process_reference_initialisation(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxString &p_reference_id, wxString &p_reference_name, ATermAppl &datatype_spec);
    /**
     * XML architecture diagram to mCRL2 convertor function.
     * Converts an XML architecture diagram to an mCRL2 specification.
     * @param p_doc_root An XML specification containing the architecture diagram.
     * @param p_diagram_id The identifier of the architecture diagram to convert.
     * @param p_verbose Flag to set for verbose output.
     * @param p_refs An array of references, containing the actions inside each reference in this architecture diagram.
     * @param p_renameds An array of renamed actions.
     * @param p_channel_comms An array of channel communications.
     * @return A string containing the mCRL2 specification of the architecture diagram.
     * @pre p_doc_root is a valid pointer to an XML specification, p_diagram_id is a valid reference to an identifier of a valid architecture diagram and p_refs is a valid reference to an array of references (may be empty).
     * @post A string containing the mCRL2 specification for the converted architecture diagram is returned and p_refs, p_renameds and p_channel_comms are updated according to the diagram or error messages are produced and the empty string is returned.
     */
    void architecture_diagram_mcrl2(wxXmlNode *p_doc_root, wxString &p_diagram_id, arr_action_reference &p_refs, arr_renamed &p_renameds, arr_channel_comm &p_channel_comms, wxArrayString &p_specs, ATermAppl &datatype_spec, bool p_verbose);
    /**
     * XML process diagram to mCRL2 convertor function.
     * Converts an XML process diagram to an mCRL2 specification.
     * @param p_doc_root The XML specification containing the diagram to be exported.
     * @param p_diagram_id The identifier of the diagram to be exported.
     * @param p_sort_expressions An array containing the mCRL2 sort expressions constructed during the convertion process.
     * @param p_actions An array containing the mCRL2 actions constructed during the convertion process.
     * @param p_internal_specs An array containing the internal mCRL2 specifications constructed during the convertion process.
     * @param p_specs An array containing the mCRL2 specifications constructed during the convertion process.
     * @param p_verbose Flag to be set for verbose output.
     * @param datatype_spec The datatype specification.
     * @pre p_doc_root is a valid pointer to an XML specification, p_diagram_id is a valid reference to an identifier of a valid process diagram, p_sort_expressions is a valid reference to an array of sort expressions, p_actions is a valid reference to an array of actions, p_internal_specs is a valid reference to an array of internal specifications and p_specs is a valid reference to an array of specifications.
     * @post The XML process diagram is converted to mCRL2 and p_sort_expressions, p_actions, p_internal_specs and p_specs are updated accordingly or error messages are produced.
     */
    void process_diagram_mcrl2(wxXmlNode *p_doc_root, wxString &p_diagram_id, wxArrayString &p_sort_expressions, list_of_action &p_actions, wxArrayString &p_internal_specs, wxArrayString &p_specs, ATermAppl &datatype_spec, bool p_verbose);
    /**
     * Process diagram inference function.
     * Infers whether a given diagram identifier belongs to a process diagram (or architecture diagram).
     * @param p_doc_root A valid XML specification containing the diagrams.
     * @param p_diagram_id The identifier to infer whether it belongs to a process diagram.
     * @return True if the identifier belongs to a process diagram and false otherwise.
     * @pre p_doc_root is a valid pointer to a valid XML specification and p_diagram_id is a valid reference to a diagram identifier.
     * @post A flag indicating whether the identifier belongs to a process diagram is returned or error messages are produced.
     */
    bool is_process_diagram(wxXmlNode *p_doc_root, wxString &p_diagram_id);
    /**
     * XML architecture diagram to mCRL2 initial process specification convertor function.
     * Constructs an mCRL2 initial process specification for an architecture diagram.
     * @param p_diagram_name The name of the architecture diagram to construct an initial process specification for.
     * @return The mCRL2 initial process specification.
     * @pre p_diagram_name is a valid reference to an XML architecture diagram name.
     * @post The mCRL2 initial process specification is returned.
     */
    wxString architecture_diagram_mcrl2_init(wxString &p_diagram_name);
    /**
     * XML specification convertion function.
     * Converts all spaces in the names of objects to underscores.
     * @pre p_spec is a valid reference to a valid XML specification.
     * @post All spaces in the names of oject in the XML specification have been replaced by underscores.
     * @param p_spec The specification to convert.
     */
    void convert_spaces_node(wxXmlNode *p_node);
    void convert_spaces(wxXmlDocument &p_spec);

    void test_export(void);

  } // namespace mcrl2gen
} // namespace grape

#endif // MCRL2GEN_MCRL2GEN_H
