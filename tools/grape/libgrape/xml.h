// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xml.h
//
// Declares XML generating functions.

#ifndef LIBGRAPE_XML_H
#define LIBGRAPE_XML_H

#include <wx/xml/xml.h>


#include "specification.h"

namespace grape
{
  namespace libgrape
  {
    // Note Remco:
    // Use wxXmlDocument for this and return it after construction.
    /**
     * GraPE Specification to XML convertor function.
     * Takes a GraPE Specification and converts it to an XML document.
     * @param p_spec The GraPE Specification to be converted.
     * @param p_filename The filename to which you want to save the xml-document
     * @param p_indent the number of indetation spaces
     * @return The converted XML document.
     */
    wxXmlDocument xml_convert( grape_specification &p_spec, wxString &p_filename, int p_indent, bool p_save = true );

    /**
     * Datatype specification to XML converter subfunction.
     * Takes a datatype specification and pointer to the root and adds a datatypespecificationlist-node to the tree.
     * @param p_root The root of the XML tree.
     * @param p_dat_spec. The datatype specification to be converted.
     */
    void add_datatype_specification_list( wxXmlNode* p_root, datatype_specification* p_dat_spec );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes an array of process diagrams and pointer to the root and adds a processdiagramlist-node to the tree.
     * @param p_root The root of the XML tree.
     * @param p_proc_list. The array of process diagrams to be processed.
     */
    void add_process_diagram_list( wxXmlNode* p_root, arr_process_diagram* p_proc_list );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the diagram whose comments are to be processed and adds a processdiagramlist-node as child of the parent node.
     * @param p_objectlist The root of the commentlist.
     * @param p_dia. The diagram whose comments are to be processed.
     */
    void add_comment_list( wxXmlNode* p_objectlist, diagram* p_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the process diagram whose initial designators are to be processed and adds a initialdesignatorlist-node as child of the parent node.
     * @param p_objectlist The root of the intial designator list.
     * @param p_proc_dia. The diagram whose initial designators are to be processed.
     */
    void add_initial_designator_list( wxXmlNode* p_objectlist, process_diagram* p_proc_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the process diagram whose reference states are to be processed and adds a referencestatelist-node as child of the parent node.
     * @param p_objectlist The root of the reference state list.
     * @param p_proc_dia. The diagram whose reference states are to be processed.
     */
    void add_reference_state_list( wxXmlNode* p_objectlist, process_diagram* p_proc_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the process diagram whose states are to be processed and adds a statelist-node as child of the parent node.
     * @param p_objectlist The root of the state list.
     * @param p_proc_dia. The diagram whose states are to be processed.
     */
    void add_state_list( wxXmlNode* p_objectlist, process_diagram* p_proc_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the process diagram whose terminating transitions are to be processed and adds a terminatingtransitionlist-node as child of the parent node.
     * @param p_objectlist The root of the terminating transition list.
     * @param p_proc_dia. The diagram whose terminating transitions are to be processed.
     */
    void add_terminating_transition_list( wxXmlNode* p_objectlist, process_diagram* p_proc_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the process diagram whose nonterminating transitions are to be processed and adds a nonterminatingtransitionlist-node as child of the parent node.
     * @param p_objectlist The root of the nonterminating transition list.
     * @param p_proc_dia. The diagram whose nonterminating transitions are to be processed.
     */
    void add_nonterminating_transition_list( wxXmlNode* p_objectlist, process_diagram* p_proc_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes an array of architecture diagrams and pointer to the root and adds an architecturediagramlist-node to the tree.
     * @param p_root The root of the XML tree.
     * @param p_arch_list. The array of architecture diagrams to be processed.
     */
    void add_architecture_diagram_list( wxXmlNode* p_root, arr_architecture_diagram* p_arch_list );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the architecture diagram whose blocked are to be processed and adds a blockedlist-node as child of the parent node.
     * @param p_objectlist The root of the blocked list.
     * @param p_arch_dia. The diagram whose blocked are to be processed.
     */
    void add_blocked_list( wxXmlNode* p_objectlist, architecture_diagram* p_arch_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the architecture diagram whose visibles are to be processed and adds a visiblelist-node as child of the parent node.
     * @param p_objectlist The root of the visible list.
     * @param p_arch_dia. The diagram whose visibles are to be processed.
     */
    void add_visible_list( wxXmlNode* p_objectlist, architecture_diagram* p_arch_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the architecture diagram whose channel communications are to be processed and adds a visiblelist-node as child of the parent node.
     * @param p_objectlist The root of the channel communication list.
     * @param p_arch_dia. The diagram whose channel communications are to be processed.
     */
    void add_channel_communication_list( wxXmlNode* p_objectlist, architecture_diagram* p_arch_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the architecture diagram whose channels are to be processed and adds a visiblelist-node as child of the parent node.
     * @param p_objectlist The root of the channel list.
     * @param p_arch_dia. The diagram whose channels are to be processed.
     */
    void add_channel_list( wxXmlNode* p_objectlist, architecture_diagram* p_arch_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the architecture diagram whose architecture references are to be processed and adds a visiblelist-node as child of the parent node.
     * @param p_objectlist The root of the architecture reference list.
     * @param p_arch_dia. The diagram whose architecture references are to be processed.
     */
    void add_architecture_reference_list( wxXmlNode* p_objectlist, architecture_diagram* p_arch_dia );

    /**
     * GraPE Specification to XML converter subfunction.
     * Takes a parent node and pointer to the architecture diagram whose process references are to be processed and adds a visiblelist-node as child of the parent node.
     * @param p_objectlist The root of the process reference list.
     * @param p_arch_dia. The diagram whose process references are to be processed.
     */
    void add_process_reference_list( wxXmlNode* p_objectlist, architecture_diagram* p_arch_dia );

    /**
     * Array of wxXmlNode.
     */
    WX_DECLARE_OBJARRAY( wxXmlNode, arr_wxXmlNode );

    /** @return "true" if p_b is true or "false" if p_b is false. */
    wxString get_bool_text( bool p_b );

    /**
     * Writes an element tree representing attributes of an object.
     * @param p_parent The parent node of the element tree.
     * @param p_object The object that contains the attributes.
     */
    void write_object_attributes( wxXmlNode *p_parent, object* p_object);

    /**
     * Writes an element tree representing a coordinate.
     * @param p_parent The parent node of the element tree.
     * @param p_x The x coordinate
     * @param p_y The y coordinate
     */
    void write_coordinate( wxXmlNode *p_parent, float p_x, float p_y );

    /**
     * Writes an element tree representing a size (width and height).
     * @param p_parent The parent node of the element tree.
     * @param p_x The x coordinate
     * @param p_y The y coordinate
     *
     */
    void write_size( wxXmlNode *p_parent, float p_width, float p_height );

    /**
     * Writes an element tree representing an identifier.
     * @param p_parent The parent node of the element tree.
     * @param p_id The id.
     */
    void write_id( wxXmlNode *p_parent, unsigned int p_id );

    /**
     * Writes an element tree representing a name.
     * @param p_parent The parent node of the element tree.
     * @param p_name The name.
     */
    void write_name( wxXmlNode *p_parent, const wxString &p_name );

    /**
     * Writes an element tree representing a rename.
     * @param p_parent The parent node of the element tree.
     * @param p_rename_to The rename.
     */
    void write_rename_to( wxXmlNode *p_parent, const wxString &p_rename_to );

    /**
     * Writes an element tree representing a line type.
     * @param p_parent The parent node of the element tree.
     * @param p_linetype The line type.
     */
    void write_linetype( wxXmlNode *p_parent, linetype p_linetype );

    /**
     * Writes an element tree representing a channel type.
     * @param p_parent The parent node of the element tree.
     * @param p_channeltype The channel type.
     */
    void write_channeltype( wxXmlNode *p_parent, channeltype p_channeltype );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_XML_H
