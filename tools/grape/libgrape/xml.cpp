// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xml.cpp
//
// Implements XML generating functions.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include "xml.h"

using namespace grape::libgrape;

wxXmlDocument grape::libgrape::xml_convert( grape_specification &p_spec, wxString &p_filename, int p_indent, bool p_save )
{
  wxXmlDocument xml_document; // the document to be written to.

  /* root of xml document */
  // no parent, type is element_node, name is grape, no content, no properties, no siblings
  wxXmlNode* xml_grape = new wxXmlNode( NULL, wxXML_ELEMENT_NODE, _T( "grape" ) );
  /* xml_grape set as root */
  xml_document.SetRoot( xml_grape );

  arr_architecture_diagram* a_dia_list = p_spec.get_architecture_diagram_list();
  add_architecture_diagram_list( xml_grape, a_dia_list );

  arr_process_diagram* p_dia_list = p_spec.get_process_diagram_list();
  add_process_diagram_list( xml_grape, p_dia_list );

  datatype_specification* d_spec = p_spec.get_datatype_specification();
  add_datatype_specification_list( xml_grape, d_spec );

  if(p_save)
  {
    xml_document.Save( p_filename, p_indent );
  }

  return xml_document;
}

void grape::libgrape::add_datatype_specification_list( wxXmlNode* p_root, datatype_specification* p_dat_spec )
{
  /* node <datatypespecificationlist> with parent p_root */
  wxXmlNode* xml_datatype_specification = new wxXmlNode( p_root, wxXML_ELEMENT_NODE,
                                              _T( "datatypespecificationlist" ) );
  // add properties to the node; save the datatypespecification string
  /* textnode <dataexpression> with parent xml_datatypespecification */
  wxString text = p_dat_spec->get_declarations();

  wxXmlNode* xml_dataexpression = new wxXmlNode( xml_datatype_specification, wxXML_ELEMENT_NODE,
                                    _T("datatypespecification") );
  new wxXmlNode( xml_dataexpression, wxXML_TEXT_NODE, _T("value"), text);
}

void grape::libgrape::add_process_diagram_list( wxXmlNode* p_root, arr_process_diagram* p_proc_list )
{
  /* node <processdiagramlist> */
  wxXmlNode* xml_process_diagram_list = new wxXmlNode( p_root, wxXML_ELEMENT_NODE, _T( "processdiagramlist" ) );
  // add properties to the node; iterate over the list of process diagrams
  int p_count = p_proc_list->GetCount();
  for ( int i = p_count-1; i >= 0; --i )
  {
    /* node <processdiagram> with parent xml_process_diagram_list */
    wxXmlNode* xml_process_diagram = new wxXmlNode( xml_process_diagram_list, wxXML_ELEMENT_NODE,
                                        _T( "processdiagram" ) );

    process_diagram &p_dia = p_proc_list->Item( i );
    preamble* proc_preamble = p_dia.get_preamble();

    { // offspring of xml_process_diagram

      /* node <objectlist> */
      wxXmlNode* xml_objectlist = new wxXmlNode( xml_process_diagram, wxXML_ELEMENT_NODE,
                                                _T( "objectlist" ) );

      { // children of xml_objectlist

        add_comment_list( xml_objectlist, &p_dia );
        add_initial_designator_list( xml_objectlist, &p_dia );
        add_reference_state_list( xml_objectlist, &p_dia );
        add_state_list( xml_objectlist, &p_dia );
        add_terminating_transition_list( xml_objectlist, &p_dia );
        add_nonterminating_transition_list( xml_objectlist, &p_dia );


      } // end children xml_objectlist

      /* node <preambledeclarations> */
      wxXmlNode* xml_preamble_declarations = new wxXmlNode( xml_process_diagram, wxXML_ELEMENT_NODE,
                                                _T( "preambledeclarations" ) );

      { // offspring of xml_preamble_declarations

        /* node <localvariablelist> */
	      list_of_decl_init local_vars = proc_preamble->get_local_variable_declarations_list();
        wxXmlNode* xml_loc_var_list = new wxXmlNode( xml_preamble_declarations, wxXML_ELEMENT_NODE,
                                                _T( "localvariablelist" ) );
        int count = local_vars.GetCount();
	      for (int i = count-1; i >= 0; --i)
        {
          decl_init local_var = local_vars.Item( i );
          wxString text = local_var.get_decl_init();
          wxXmlNode* xml_loc_var = new wxXmlNode( xml_loc_var_list, wxXML_ELEMENT_NODE, _T( "var" ) );
          new wxXmlNode( xml_loc_var, wxXML_TEXT_NODE, _T( "value" ), text );
        }

        /* node <parameterlist> */
        list_of_decl params = proc_preamble->get_parameter_declarations_list();
        wxXmlNode* xml_parameter_list = new wxXmlNode( xml_preamble_declarations, wxXML_ELEMENT_NODE, _T( "parameterlist" ) );

        count = params.GetCount();
        for (int i = count-1; i >= 0; --i)
        {
          decl param = params.Item( i );
          wxString text = param.get_decl();
          wxXmlNode* xml_param = new wxXmlNode( xml_parameter_list, wxXML_ELEMENT_NODE, _T( "param" ) );
          new wxXmlNode( xml_param, wxXML_TEXT_NODE, _T( "value" ), text );
        }

      } // end offspring xml_preamble_declarations

      write_name_to( xml_process_diagram, p_dia.get_name() );

      write_id (xml_process_diagram, p_dia.get_id() );

    } // end offspring xml_process_diagram

  } // end for
}

void grape::libgrape::add_comment_list( wxXmlNode* p_objectlist, diagram* p_dia )
{
  /* node <commentlist> */
  wxXmlNode* xml_commentlist = new wxXmlNode( p_objectlist, wxXML_ELEMENT_NODE, _T( "commentlist" ) );

  { // children xml_commentlist

    // iterate over all comments in the process diagram
    int comment_count = p_dia->count_comment();
    for ( int i = comment_count-1; i >= 0; --i)
    {

      wxXmlNode* xml_comment = new wxXmlNode( xml_commentlist, wxXML_ELEMENT_NODE, _T( "comment" ) );
      comment* comment_ptr = p_dia->get_comment( i );

      /* node <text> */
      wxXmlNode* xml_comment_text = new wxXmlNode( xml_comment, wxXML_ELEMENT_NODE, _T( "text" ) );

      wxString comment_text = comment_ptr->get_text();

      new wxXmlNode( xml_comment_text, wxXML_TEXT_NODE, _T( "value" ), comment_text );


      /* node <propertyof> */
      wxXmlNode* xml_comment_prop = new wxXmlNode( xml_comment, wxXML_ELEMENT_NODE, _T( "propertyof" ) );

      wxString prop = _T( "-1" );
      object* obj_ptr = comment_ptr->get_attached_object();
      if ( obj_ptr != 0 )
      {
        prop = wxString::Format( _T( "%u" ), obj_ptr->get_id() );
      }

      new wxXmlNode( xml_comment_prop, wxXML_TEXT_NODE, _T( "value" ), prop );

      /* Inherited */

      write_object_attributes( xml_comment, comment_ptr );
    }

  }
}

void grape::libgrape::add_initial_designator_list( wxXmlNode* p_objectlist, process_diagram* p_proc_dia )
{
  /* node <initialdesignatorlist> */
  wxXmlNode* xml_initial_designator_list = new wxXmlNode( p_objectlist, wxXML_ELEMENT_NODE,
  _T( "initialdesignatorlist" ) );

  { // children xml_initial_designator_list

    // iterate over all initial designators in the process diagram
    int init_count = p_proc_dia->count_initial_designator();
    for ( int i = init_count-1; i >= 0; --i )
    {

      wxXmlNode* xml_initial_designator = new wxXmlNode( xml_initial_designator_list, wxXML_ELEMENT_NODE,
                                              _T( "initialdesignator" ) );
      initial_designator* init_ptr = p_proc_dia->get_initial_designator( i );

      /* node <propertyof> */
      wxXmlNode* xml_initial_designator_prop = new wxXmlNode( xml_initial_designator, wxXML_ELEMENT_NODE, _T( "propertyof" ) );

      wxString propertyof = _T( "-1" );
      compound_state* state_ptr = init_ptr->get_attached_state();
      if ( state_ptr != 0 )
      {
        propertyof = wxString::Format( _T( "%u" ), state_ptr->get_id() );
      }

      new wxXmlNode( xml_initial_designator_prop, wxXML_TEXT_NODE, _T( "value" ), propertyof );

      /*Inherited*/

      write_object_attributes( xml_initial_designator, init_ptr );
    }

  }
}

void grape::libgrape::add_reference_state_list( wxXmlNode* p_objectlist, process_diagram* p_proc_dia )
{
  /* node <referencestatelist> */
  wxXmlNode* xml_reference_state_list = new wxXmlNode( p_objectlist, wxXML_ELEMENT_NODE,
  _T( "referencestatelist" ) );

  { // children xml_reference_state_list

    // iterate over all reference states in the process diagram
    int ref_state_count = p_proc_dia->count_reference_state();
    for ( int i = ref_state_count-1; i >= 0; --i )
    {
      wxXmlNode* xml_reference_state = new wxXmlNode( xml_reference_state_list, wxXML_ELEMENT_NODE,
                                          _T( "referencestate" ) );
      reference_state* ref_state_ptr = p_proc_dia->get_reference_state( i );

      /* node <parameterassignmentlist> */
      wxXmlNode* xml_reference_state_parameterlist = new wxXmlNode( xml_reference_state, wxXML_ELEMENT_NODE, _T( "parameterassignmentlist" ) );
      list_of_varupdate parameter_assignments = ref_state_ptr->get_parameter_updates();
      int param_ass_count = parameter_assignments.GetCount();
      for ( int j = param_ass_count-1; j >= 0; --j )
      {
        varupdate parameter_assignment = parameter_assignments.Item( j );
        wxString parameter_assignment_text = parameter_assignment.get_varupdate();

        wxXmlNode* xml_reference_state_parameter = new wxXmlNode( xml_reference_state_parameterlist, wxXML_ELEMENT_NODE, _T( "parameterassignment" ) );
        new wxXmlNode( xml_reference_state_parameter, wxXML_TEXT_NODE, _T( "value" ), parameter_assignment_text );
      }

      /* node <propertyof> */
      wxXmlNode* xml_reference_state_prop = new wxXmlNode( xml_reference_state, wxXML_ELEMENT_NODE, _T( "propertyof" ) );

      wxString ref_state_prop = _T( "-1" );
      process_diagram* proc_ptr = ref_state_ptr->get_relationship_refers_to();
      if ( proc_ptr != 0 )
      {
        ref_state_prop.Printf(_T("%u"), proc_ptr->get_id());
      }
      new wxXmlNode( xml_reference_state_prop, wxXML_TEXT_NODE, _T( "value" ), ref_state_prop );

      write_name_to( xml_reference_state, ref_state_ptr->get_name() );

      /* Inherited */
      write_object_attributes( xml_reference_state, ref_state_ptr );
    }

  }
}

void grape::libgrape::add_state_list( wxXmlNode* p_objectlist, process_diagram* p_proc_dia )
{
  /* node <statelist> */
  wxXmlNode* xml_state_list = new wxXmlNode( p_objectlist, wxXML_ELEMENT_NODE, _T( "statelist" ) );

  { // children xml_state_list

    // iterate over all states in the process diagram
    int state_count = p_proc_dia->count_state();
    for ( int i = state_count-1; i >= 0; --i )
    {

      wxXmlNode* xml_state = new wxXmlNode( xml_state_list, wxXML_ELEMENT_NODE,
                                          _T( "state" ) );
      state* state_ptr = p_proc_dia->get_state( i );

      write_name_to( xml_state, state_ptr->get_name() );

      /* Inherited */
      write_object_attributes( xml_state, state_ptr );
    }

  }
}

void grape::libgrape::add_terminating_transition_list( wxXmlNode* p_objectlist, process_diagram* p_proc_dia )
{
  /* node <terminatingtransitionlist> */
  wxXmlNode* xml_terminating_transition_list = new wxXmlNode( p_objectlist, wxXML_ELEMENT_NODE,
                                                  _T( "terminatingtransitionlist" ) );

  { // children xml_terminating_transition_list

    // iterate over all terminating transitions in the process diagram
    int tt_count = p_proc_dia->count_terminating_transition();
    for ( int i = tt_count-1; i >= 0; --i )
    {
      wxXmlNode* xml_terminating_transition = new wxXmlNode( xml_terminating_transition_list, wxXML_ELEMENT_NODE,
                                                  _T( "terminatingtransition" ) );

      terminating_transition* tt_ptr = p_proc_dia->get_terminating_transition( i );

      /* node <label> */
      wxXmlNode* xml_terminating_trans_label = new wxXmlNode( xml_terminating_transition, wxXML_ELEMENT_NODE, _T( "label" ) );

      /* node <variableupdates> */
      wxXmlNode* xml_terminating_trans_label_var_updates = new wxXmlNode( xml_terminating_trans_label, wxXML_ELEMENT_NODE, _T( "variableupdates" ) );

      list_of_varupdate tt_label_var_updates = tt_ptr->get_label()->get_variable_updates();
      int varupdate_count = tt_label_var_updates.GetCount();
      for ( int j = varupdate_count-1; j >= 0; --j )
      {
        varupdate tt_label_var_update = tt_label_var_updates.Item( j );
        wxString tt_label_var_update_text = tt_label_var_update.get_varupdate();

        /* node <variableupdate> */
        wxXmlNode* xml_terminating_trans_label_var_update = new wxXmlNode( xml_terminating_trans_label_var_updates, wxXML_ELEMENT_NODE, _T( "variableupdate" ) );
        new wxXmlNode( xml_terminating_trans_label_var_update, wxXML_TEXT_NODE, _T( "value" ), tt_label_var_update_text );
      }

      /* node <timestamp> */
      wxXmlNode* xml_terminating_trans_label_timestamp = new wxXmlNode( xml_terminating_trans_label, wxXML_ELEMENT_NODE, _T( "timestamp" ) );

      wxString tt_label_timestamp_text = tt_ptr->get_label()->get_timestamp();
      while (!tt_label_timestamp_text.IsEmpty())
      {
        new wxXmlNode( xml_terminating_trans_label_timestamp, wxXML_TEXT_NODE, _T( "value" ), tt_label_timestamp_text );
      }

      /* node <actions> */
      wxXmlNode* xml_terminating_trans_label_actions = new wxXmlNode( xml_terminating_trans_label, wxXML_ELEMENT_NODE, _T( "actions" ) );

      list_of_action tt_label_actions = tt_ptr->get_label()->get_actions();
      int action_count = tt_label_actions.GetCount();
      for ( int j = action_count-1; j >= 0; --j )
      {
        action tt_label_action = tt_label_actions.Item( j );
        wxString tt_label_action_name_text = tt_label_action.get_name();

        /* node <action> */
        wxXmlNode* xml_terminating_trans_label_action = new wxXmlNode( xml_terminating_trans_label_actions, wxXML_ELEMENT_NODE, _T( "action" ) );

        list_of_dataexpression tt_label_action_params = tt_label_action.get_parameters();
        if (tt_label_action_params.GetCount() > 0)
        {
          int action_param_count = tt_label_action_params.GetCount();
          for ( int k = action_param_count-1; k >= 0; --k )
          {
            dataexpression tt_label_action_param = tt_label_action_params.Item( k );
            wxString tt_label_action_param_text = tt_label_action_param.get_expression();

            /* node <param> */
            wxXmlNode* xml_terminating_trans_label_action_param = new wxXmlNode( xml_terminating_trans_label_action, wxXML_ELEMENT_NODE, _T( "param" ) );
            new wxXmlNode( xml_terminating_trans_label_action_param, wxXML_TEXT_NODE, _T( "value" ), tt_label_action_param_text );
          }
        }

        /* node <name> */
        wxXmlNode* xml_terminating_trans_label_action_name = new wxXmlNode( xml_terminating_trans_label_action, wxXML_ELEMENT_NODE, _T( "name" ) );
        new wxXmlNode( xml_terminating_trans_label_action_name, wxXML_TEXT_NODE, _T( "value" ), tt_label_action_name_text );
      }

      /* node <condition> */
      wxXmlNode* xml_terminating_trans_label_condition = new wxXmlNode( xml_terminating_trans_label, wxXML_ELEMENT_NODE, _T( "condition" ) );

      wxString tt_label_condition_text = tt_ptr->get_label()->get_condition();
      while (!tt_label_condition_text.IsEmpty())
      {
        new wxXmlNode( xml_terminating_trans_label_condition, wxXML_TEXT_NODE, _T( "value" ), tt_label_condition_text );
      }

      /* node <variabledeclarations> */
      wxXmlNode* xml_terminating_trans_label_var_decls = new wxXmlNode( xml_terminating_trans_label, wxXML_ELEMENT_NODE, _T( "variabledeclarations" ) );

      list_of_decl tt_label_var_decls = tt_ptr->get_label()->get_declarations();
      int var_decl_count = tt_label_var_decls.GetCount();
      for ( int j = var_decl_count-1; j >= 0; --j )
      {
        decl tt_label_var_decl = tt_label_var_decls.Item( j );
        wxString tt_label_var_decl_text = tt_label_var_decl.get_decl();

        /* node <variabledeclaration> */
        wxXmlNode* xml_terminating_trans_label_var_decl = new wxXmlNode( xml_terminating_trans_label_var_decls, wxXML_ELEMENT_NODE, _T( "variabledeclaration" ) );
        new wxXmlNode( xml_terminating_trans_label_var_decl, wxXML_TEXT_NODE, _T( "value" ), tt_label_var_decl_text );
      }

      /* node <from> */
      wxXmlNode* xml_terminating_trans_from = new wxXmlNode( xml_terminating_transition, wxXML_ELEMENT_NODE, _T( "from" ) );

      wxString tt_from = _T( "-1" );
      compound_state* begin_ptr = tt_ptr->get_beginstate();
      if ( begin_ptr != 0 )
      {
        tt_from = wxString::Format( _T( "%u"), begin_ptr->get_id() );
      }
      new wxXmlNode( xml_terminating_trans_from, wxXML_TEXT_NODE, _T( "value" ), tt_from );

      /* node <breakpointlist> */
      wxXmlNode* xml_terminating_trans_breakpointlist = new wxXmlNode( xml_terminating_transition, wxXML_ELEMENT_NODE, _T( "breakpointlist" ) );

          int breakpoints_count = tt_ptr->get_breakpoints()->GetCount();
          for ( int j = breakpoints_count-1; j >= 0; --j )
          {
            coordinate &break_ptr = tt_ptr->get_breakpoints()->Item( j );

            write_coordinate( xml_terminating_trans_breakpointlist, break_ptr.m_x, break_ptr.m_y );
          }

      write_linetype( xml_terminating_transition, tt_ptr->get_linetype() );

      /* Inherited */
      write_object_attributes( xml_terminating_transition, tt_ptr );
    }
  }
}

void grape::libgrape::add_nonterminating_transition_list( wxXmlNode* p_objectlist, process_diagram* p_proc_dia )
{
  /* node <nonterminatingtransitionlist> */
  wxXmlNode* xml_nonterminating_transition_list = new wxXmlNode( p_objectlist, wxXML_ELEMENT_NODE,
                                                      _T( "nonterminatingtransitionlist" ) );

  { // children xml_nonterminating_transition_list

    // iterate over all terminating transitions in the process diagram
    int ntt_count = p_proc_dia->count_nonterminating_transition();
    for ( int i = ntt_count-1; i >= 0; --i )
    {
      wxXmlNode* xml_nonterminating_transition = new wxXmlNode( xml_nonterminating_transition_list, wxXML_ELEMENT_NODE,
                                                  _T( "nonterminatingtransition" ) );
      nonterminating_transition* tt_ptr = p_proc_dia->get_nonterminating_transition( i );

      /* node <label> */
      wxXmlNode* xml_nonterminating_trans_label = new wxXmlNode( xml_nonterminating_transition, wxXML_ELEMENT_NODE, _T( "label" ) );

      /* node <variableupdates> */
      wxXmlNode* xml_nonterminating_trans_label_var_updates = new wxXmlNode( xml_nonterminating_trans_label, wxXML_ELEMENT_NODE, _T( "variableupdates" ) );

      list_of_varupdate tt_label_var_updates = tt_ptr->get_label()->get_variable_updates();
      int varupdate_count = tt_label_var_updates.GetCount();
      for ( int j = varupdate_count-1; j >= 0; --j )
      {
        varupdate tt_label_var_update = tt_label_var_updates.Item( j );
        wxString tt_label_var_update_text = tt_label_var_update.get_varupdate();

        /* node <variableupdate> */
        wxXmlNode* xml_nonterminating_trans_label_var_update = new wxXmlNode( xml_nonterminating_trans_label_var_updates, wxXML_ELEMENT_NODE, _T( "variableupdate" ) );
        new wxXmlNode( xml_nonterminating_trans_label_var_update, wxXML_TEXT_NODE, _T( "value" ), tt_label_var_update_text );
      }

      /* node <timestamp> */
      wxXmlNode* xml_nonterminating_trans_label_timestamp = new wxXmlNode( xml_nonterminating_trans_label, wxXML_ELEMENT_NODE, _T( "timestamp" ) );

      wxString tt_label_timestamp_text = tt_ptr->get_label()->get_timestamp();
      while (!tt_label_timestamp_text.IsEmpty())
      {
        new wxXmlNode( xml_nonterminating_trans_label_timestamp, wxXML_TEXT_NODE, _T( "value" ), tt_label_timestamp_text );
      }

      /* node <actions> */
      wxXmlNode* xml_nonterminating_trans_label_actions = new wxXmlNode( xml_nonterminating_trans_label, wxXML_ELEMENT_NODE, _T( "actions" ) );

      list_of_action tt_label_actions = tt_ptr->get_label()->get_actions();
      int action_count = tt_label_actions.GetCount();
      for ( int j = action_count-1; j >= 0; --j )
      {
        action tt_label_action = tt_label_actions.Item( j );
        wxString tt_label_action_name_text = tt_label_action.get_name();

        /* node <action> */
        wxXmlNode* xml_nonterminating_trans_label_action = new wxXmlNode( xml_nonterminating_trans_label_actions, wxXML_ELEMENT_NODE, _T( "action" ) );

        list_of_dataexpression tt_label_action_params = tt_label_action.get_parameters();
        if (tt_label_action_params.GetCount() > 0)
        {
          int action_param_count = tt_label_action_params.GetCount();
          for ( int k = action_param_count-1; k >= 0; --k )
          {
            dataexpression tt_label_action_param = tt_label_action_params.Item( k );
            wxString tt_label_action_param_text = tt_label_action_param.get_expression();

            /* node <param> */
            wxXmlNode* xml_nonterminating_trans_label_action_param = new wxXmlNode( xml_nonterminating_trans_label_action, wxXML_ELEMENT_NODE, _T( "param" ) );
            new wxXmlNode( xml_nonterminating_trans_label_action_param, wxXML_TEXT_NODE, _T( "value" ), tt_label_action_param_text );
          }
        }

       /* node <name> */
        wxXmlNode* xml_nonterminating_trans_label_action_name = new wxXmlNode( xml_nonterminating_trans_label_action, wxXML_ELEMENT_NODE, _T( "name" ) );
        new wxXmlNode( xml_nonterminating_trans_label_action_name, wxXML_TEXT_NODE, _T( "value" ), tt_label_action_name_text );
      }

      /* node <condition> */
      wxXmlNode* xml_nonterminating_trans_label_condition = new wxXmlNode( xml_nonterminating_trans_label, wxXML_ELEMENT_NODE, _T( "condition" ) );

      wxString tt_label_condition_text = tt_ptr->get_label()->get_condition();
      while (!tt_label_condition_text.IsEmpty())
      {
        new wxXmlNode( xml_nonterminating_trans_label_condition, wxXML_TEXT_NODE, _T( "value" ), tt_label_condition_text );
      }

      /* node <variabledeclarations> */
      wxXmlNode* xml_nonterminating_trans_label_var_decls = new wxXmlNode( xml_nonterminating_trans_label, wxXML_ELEMENT_NODE, _T( "variabledeclarations" ) );

      list_of_decl tt_label_var_decls = tt_ptr->get_label()->get_declarations();
      int var_decl_count = tt_label_var_decls.GetCount();
      for ( int j = var_decl_count-1; j >= 0; --j )
      {
        decl tt_label_var_decl = tt_label_var_decls.Item( j );
        wxString tt_label_var_decl_text = tt_label_var_decl.get_decl();

        /* node <variabledeclaration> */
        wxXmlNode* xml_nonterminating_trans_label_var_decl = new wxXmlNode( xml_nonterminating_trans_label_var_decls, wxXML_ELEMENT_NODE, _T( "variabledeclaration" ) );
        new wxXmlNode( xml_nonterminating_trans_label_var_decl, wxXML_TEXT_NODE, _T( "value" ), tt_label_var_decl_text );
      }

      /* node <to> */
      wxXmlNode* xml_nonterminating_trans_to = new wxXmlNode( xml_nonterminating_transition, wxXML_ELEMENT_NODE, _T( "to" ) );

      wxString tt_to = _T( "-1" );
      compound_state* end_ptr = tt_ptr->get_endstate();
      if ( end_ptr != 0 )
      {
        tt_to = wxString::Format( _T( "%u" ), end_ptr->get_id() );
      }
      new wxXmlNode( xml_nonterminating_trans_to, wxXML_TEXT_NODE, _T( "value" ), tt_to );

      /* node <from> */
      wxXmlNode* xml_nonterminating_trans_from = new wxXmlNode( xml_nonterminating_transition, wxXML_ELEMENT_NODE, _T( "from" ) );

      wxString tt_from = _T( "-1" );
      compound_state* begin_ptr = tt_ptr->get_beginstate();
      if ( begin_ptr != 0 )
      {
        tt_from = wxString::Format( _T( "%u" ), begin_ptr->get_id() );
      }
      new wxXmlNode( xml_nonterminating_trans_from, wxXML_TEXT_NODE, _T( "value" ), tt_from );

      /* node <breakpointlist> */
      wxXmlNode* xml_nonterminating_trans_breakpointlist = new wxXmlNode( xml_nonterminating_transition, wxXML_ELEMENT_NODE, _T( "breakpointlist" ) );

          int breakpoints_count = tt_ptr->get_breakpoints()->GetCount();
          for ( int j = breakpoints_count-1; j >= 0; --j )
          {
            coordinate& break_ptr = tt_ptr->get_breakpoints()->Item( i );

            write_coordinate( xml_nonterminating_trans_breakpointlist, break_ptr.m_x, break_ptr.m_y );
          }

      write_linetype( xml_nonterminating_transition, tt_ptr->get_linetype() );

      /* Inherited */
      write_object_attributes( xml_nonterminating_transition, tt_ptr );
    }
  }
}

void grape::libgrape::add_architecture_diagram_list( wxXmlNode* p_root, arr_architecture_diagram* p_arch_list )
{
  /* node <architecturediagramlist> */
  wxXmlNode* xml_architecture_diagram_list = new wxXmlNode( p_root, wxXML_ELEMENT_NODE,
                                                _T( "architecturediagramlist" ) );
  // add properties to the node; iterate over the list of architecture diagrams
  int count = p_arch_list->GetCount();
  for ( int i = count-1; i >= 0; --i )
  {
    /* node <architecturediagram> with parent xml_architecture_diagram_list */
    wxXmlNode* xml_architecture_diagram = new wxXmlNode( xml_architecture_diagram_list, wxXML_ELEMENT_NODE,
                                        _T( "architecturediagram" ) );

   architecture_diagram &arch_dia = p_arch_list->Item( i );


    { // offspring of xml_architecture_diagram
      /* node <objectlist> */
      wxXmlNode* xml_objectlist = new wxXmlNode( xml_architecture_diagram, wxXML_ELEMENT_NODE,
                                                _T( "objectlist" ) );
      { // children of xml_objectlist

        add_comment_list( xml_objectlist, &arch_dia );
        add_channel_communication_list( xml_objectlist, &arch_dia );
        add_channel_list( xml_objectlist, &arch_dia );
        add_architecture_reference_list( xml_objectlist, &arch_dia );
        add_process_reference_list( xml_objectlist, &arch_dia );

      } // end children xml_objectlist

      write_name_to( xml_architecture_diagram, arch_dia.get_name() );

      /* node <id> */
      write_id( xml_architecture_diagram, arch_dia.get_id() ) ;

    } // end offspring

  } // end for
}

void grape::libgrape::add_channel_communication_list( wxXmlNode* p_objectlist, architecture_diagram* p_arch_dia )
{
  /* node <channelcommunicationlist> */
  wxXmlNode* xml_channel_communication_list = new wxXmlNode( p_objectlist, wxXML_ELEMENT_NODE,
                                                _T( "channelcommunicationlist" ) );

  { // children xml_channel_communication_list

    // iterate over all channel communication in the architecture diagram
    int c_comm_count = p_arch_dia->count_channel_communication();
    for ( int i = c_comm_count-1; i >= 0; --i )
    {

      wxXmlNode* xml_channel_communication = new wxXmlNode( xml_channel_communication_list, wxXML_ELEMENT_NODE,
                                          _T( "channelcommunication" ) );
      channel_communication* c_comm_ptr = p_arch_dia->get_channel_communication( i );

      /* node <connectionlist> */
      wxXmlNode* xml_channel_communication_conn_list = new wxXmlNode( xml_channel_communication, wxXML_ELEMENT_NODE,
                                          _T( "connectionlist" ) );

      int conn_count = c_comm_ptr->count_channel();

      for ( int j = conn_count-1; j >= 0; --j )
      {
        /* node <connectedtochannels> */
        wxXmlNode* xml_channel_communication_connected = new wxXmlNode( xml_channel_communication_conn_list, wxXML_ELEMENT_NODE,
                                          _T( "connectedtochannel" ) );

        wxString text_connected = wxString::Format( _T( "%u" ), c_comm_ptr->get_communications()->Item( j ).get_channel()->get_id() );

        new wxXmlNode( xml_channel_communication_connected, wxXML_TEXT_NODE, _T( "value" ), text_connected );

      }
      
      write_name_to( xml_channel_communication, c_comm_ptr->get_name_to() );     
      
      write_channel_communication_type( xml_channel_communication, c_comm_ptr->get_channel_communication_type() );      

      /* Inherited */
      write_object_attributes( xml_channel_communication, c_comm_ptr );
    }

  }
}

void grape::libgrape::add_channel_list( wxXmlNode* p_objectlist, architecture_diagram* p_arch_dia )
{
  /* node <channellist> */
  wxXmlNode* xml_channel_list = new wxXmlNode( p_objectlist, wxXML_ELEMENT_NODE,
                                                _T( "channellist" ) );

  { // children xml_channel_list

    // iterate over all channel in the architecture diagram
    int chan_count = p_arch_dia->count_channel();
    for ( int i = chan_count-1; i >= 0; --i )
    {

      wxXmlNode* xml_channel = new wxXmlNode( xml_channel_list, wxXML_ELEMENT_NODE,
                                          _T( "channel" ) );
      channel* channel_ptr = p_arch_dia->get_channel( i );

      /* node <connectionlist> */
      wxXmlNode* xml_channel_conn_list = new wxXmlNode( xml_channel, wxXML_ELEMENT_NODE,
                                          _T( "connectionlist" ) );

      int conn_count = channel_ptr->get_channel_communications()->GetCount();

      for ( int j = conn_count-1; j >= 0; --j )
      {
        /* node <connectedtochannelcommunications> */
        wxXmlNode* xml_channel_connected = new wxXmlNode( xml_channel_conn_list, wxXML_ELEMENT_NODE,
                                          _T( "connectedtochannelcommunication" ) );

        wxString text_connected = wxString::Format( _T( "%u" ), channel_ptr->get_channel_communications()->Item( j )->get_id() );

        new wxXmlNode( xml_channel_connected, wxXML_TEXT_NODE, _T( "value" ), text_connected );

      }
      /* node <onreference> */
      wxXmlNode* xml_channel_prop = new wxXmlNode( xml_channel, wxXML_ELEMENT_NODE, _T( "onreference" ) );

      wxString prop = wxString::Format( _T( "%u" ), channel_ptr->get_reference()->get_id() );

      new wxXmlNode( xml_channel_prop, wxXML_TEXT_NODE, _T( "value" ), prop );

      write_name_to( xml_channel, channel_ptr->get_name() );
      
      write_rename_to( xml_channel, channel_ptr->get_rename_to() );      

      write_channel_type( xml_channel, channel_ptr->get_channel_type() );      
      
      /* Inherited */
      write_object_attributes( xml_channel, channel_ptr );
    }
  }
}

void grape::libgrape::add_architecture_reference_list( wxXmlNode* p_objectlist, architecture_diagram* p_arch_dia )
{
  /* node <architecturereferencelist> */
  wxXmlNode* xml_architecture_reference_list = new wxXmlNode( p_objectlist, wxXML_ELEMENT_NODE,
                                                _T( "architecturereferencelist" ) );

  { // children xml_architecture_reference_list

    // iterate over all architecture references in the architecture diagram
    int arch_ref_count = p_arch_dia->count_architecture_reference();
    for ( int i = arch_ref_count-1; i >= 0; --i )
    {

      wxXmlNode* xml_architecture_reference = new wxXmlNode( xml_architecture_reference_list, wxXML_ELEMENT_NODE,
                                                  _T( "architecturereference" ) );
      architecture_reference* arch_ref_ptr = p_arch_dia->get_architecture_reference( i );


      /* node <channellist> */
      wxXmlNode* xml_architecture_reference_channellist = new wxXmlNode( xml_architecture_reference, wxXML_ELEMENT_NODE, _T( "channellist" ) );

          int channel_count = arch_ref_ptr->count_channel();
          for ( int k = channel_count-1; k >= 0; --k )
          {
            channel* channel_ptr = arch_ref_ptr->get_channel( k );

            /* node <channel> */
            wxXmlNode* xml_architecture_reference_channel = new wxXmlNode( xml_architecture_reference_channellist, wxXML_ELEMENT_NODE, _T( "channel" ) );

            wxString text_channel = wxString::Format( _T( "%u" ), channel_ptr->get_id() );
            new wxXmlNode( xml_architecture_reference_channel, wxXML_TEXT_NODE, _T( "value" ), text_channel );
          }

      /* node <propertyof> */
      wxXmlNode* xml_architecture_reference_prop = new wxXmlNode( xml_architecture_reference, wxXML_ELEMENT_NODE, _T( "propertyof" ) );

      wxString arch_ref_prop = _T( "-1" );
      architecture_diagram* proc_ptr = arch_ref_ptr->get_relationship_refers_to();
      if ( proc_ptr != 0 )
      {
        arch_ref_prop = wxString::Format( _T( "%u" ), proc_ptr->get_id() );
      }

      new wxXmlNode( xml_architecture_reference_prop, wxXML_TEXT_NODE, _T( "value" ), arch_ref_prop );

      /* node <name> */
      write_name_to( xml_architecture_reference, arch_ref_ptr->get_name() );

      /* Inherited */
      write_object_attributes( xml_architecture_reference, arch_ref_ptr );
    }
  }
}

void grape::libgrape::add_process_reference_list( wxXmlNode* p_objectlist, architecture_diagram* p_arch_dia )
{
  /* node <processreferencelist> */
  wxXmlNode* xml_process_reference_list = new wxXmlNode( p_objectlist, wxXML_ELEMENT_NODE,
                                                _T( "processreferencelist" ) );

  { // children xml_process_reference_list

    // iterate over all process references in the architecture diagram
    int proc_ref_count = p_arch_dia->count_process_reference();
    for ( int i = proc_ref_count-1; i >= 0; --i )
    {

      wxXmlNode* xml_process_reference = new wxXmlNode( xml_process_reference_list, wxXML_ELEMENT_NODE,
                                                  _T( "processreference" ) );
      process_reference* proc_ref_ptr = p_arch_dia->get_process_reference( i );

      /* node <parameterassignmentlist> */
      wxXmlNode* xml_process_reference_parameterlist = new wxXmlNode( xml_process_reference, wxXML_ELEMENT_NODE, _T( "parameterassignmentlist" ) );

      list_of_varupdate parameter_assignments = proc_ref_ptr->get_parameter_updates();
      int param_ass_count = parameter_assignments.GetCount();
      for ( int j = param_ass_count-1; j >= 0; --j )
      {
        varupdate parameter_assignment = parameter_assignments.Item( j );
        wxString parameter_assignment_text = parameter_assignment.get_varupdate();

        wxXmlNode* xml_process_reference_parameter = new wxXmlNode( xml_process_reference_parameterlist, wxXML_ELEMENT_NODE, _T( "parameterassignment" ) );
        new wxXmlNode( xml_process_reference_parameter, wxXML_TEXT_NODE, _T( "value" ), parameter_assignment_text );
      }

      /* node <channellist> */
      wxXmlNode* xml_process_reference_channellist = new wxXmlNode( xml_process_reference, wxXML_ELEMENT_NODE, _T( "channellist" ) );

          int channel_count = proc_ref_ptr->count_channel();
          for ( int k = channel_count-1; k >= 0; --k )
          {
            channel* channel_ptr = proc_ref_ptr->get_channel( k );

            /* node <channel> */
            wxXmlNode* xml_process_reference_channel = new wxXmlNode( xml_process_reference_channellist, wxXML_ELEMENT_NODE, _T( "channel" ) );

            wxString text_channel = wxString::Format( _T( "%u" ), channel_ptr->get_id() );
            new wxXmlNode( xml_process_reference_channel, wxXML_TEXT_NODE, _T( "value" ), text_channel );
          }


      /* node <propertyof> */
      wxXmlNode* xml_process_reference_prop = new wxXmlNode( xml_process_reference, wxXML_ELEMENT_NODE, _T( "propertyof" ) );

      wxString proc_ref_prop = _T( "-1" );
      process_diagram* proc_ptr = proc_ref_ptr->get_relationship_refers_to();
      if ( proc_ptr != 0 )
      {
        proc_ref_prop = wxString::Format( _T( "%u" ), proc_ptr->get_id() );
      }

      new wxXmlNode( xml_process_reference_prop, wxXML_TEXT_NODE, _T( "value" ), proc_ref_prop );

      /* node <name> */
      write_name_to( xml_process_reference, proc_ref_ptr->get_name() );

      /* Inherited */
      write_object_attributes( xml_process_reference, proc_ref_ptr );
    }
  }
}

wxString grape::libgrape::get_bool_text( bool p_b )
{
  return p_b ? _T("true") : _T("false");
}

void grape::libgrape::write_coordinate( wxXmlNode *p_parent, float p_x, float p_y )
{
  /* node <coord> */
  wxXmlNode* xml_coord = new wxXmlNode( p_parent, wxXML_ELEMENT_NODE, _T( "coord" ) );

    /* node <y> */
    wxXmlNode* xml_coord_y = new wxXmlNode( xml_coord, wxXML_ELEMENT_NODE, _T( "y" ) );

    wxString coord_y;
    coord_y = wxString::Format( _T( "%f" ),  p_y );
    new wxXmlNode( xml_coord_y, wxXML_TEXT_NODE, _T( "value" ), coord_y );

    /* node <x> */
    wxXmlNode* xml_coord_x = new wxXmlNode( xml_coord, wxXML_ELEMENT_NODE, _T( "x" ) );

    wxString coord_x;
    coord_x = wxString::Format( _T( "%f" ), p_x );
    new wxXmlNode( xml_coord_x, wxXML_TEXT_NODE, _T( "value" ), coord_x );
}

void grape::libgrape::write_object_attributes( wxXmlNode *p_parent, object* p_object)
{
      write_size( p_parent, p_object->get_width(), p_object->get_height() );

      write_coordinate( p_parent, p_object->get_coordinate().m_x, p_object->get_coordinate().m_y );

      write_id( p_parent, p_object->get_id() );
}

void grape::libgrape::write_size( wxXmlNode *p_parent, float p_width, float p_height )
{
  /* node <size> */
  wxXmlNode* xml_size = new wxXmlNode( p_parent, wxXML_ELEMENT_NODE, _T( "size" ) );

    /* node <height> */
    wxXmlNode* xml_process_reference_height = new wxXmlNode( xml_size, wxXML_ELEMENT_NODE, _T( "height" ) );

    wxString height;
    height = wxString::Format( _T( "%f" ), p_height );
    new wxXmlNode( xml_process_reference_height, wxXML_TEXT_NODE, _T( "value" ), height );

    /* node <width> */
    wxXmlNode* xml_process_reference_width = new wxXmlNode( xml_size, wxXML_ELEMENT_NODE, _T( "width" ) );

    wxString width;
    width = wxString::Format( _T( "%f" ), p_width );
    new wxXmlNode( xml_process_reference_width, wxXML_TEXT_NODE, _T( "value" ), width );
}

void grape::libgrape::write_id( wxXmlNode *p_parent, unsigned int p_id )
{
  wxXmlNode* xml_comment_id = new wxXmlNode( p_parent, wxXML_ELEMENT_NODE, _T( "id" ) );

  wxString id = _T( "-1");
  id = wxString::Format( _T( "%u" ), p_id );
  new wxXmlNode( xml_comment_id, wxXML_TEXT_NODE, _T( "value" ), id );
}

void grape::libgrape::write_name_to( wxXmlNode *p_parent, const wxString &p_name )
{
  /* node <name> */
  wxXmlNode* xml_name = new wxXmlNode( p_parent, wxXML_ELEMENT_NODE, _T( "name" ) );

  new wxXmlNode( xml_name, wxXML_TEXT_NODE, _T( "value" ), p_name );
}

void grape::libgrape::write_rename_to( wxXmlNode *p_parent, const wxString &p_rename_to )
{
  /* node <rename> */
  wxXmlNode* xml_rename = new wxXmlNode( p_parent, wxXML_ELEMENT_NODE, _T( "rename" ) );

  new wxXmlNode( xml_rename, wxXML_TEXT_NODE, _T( "value" ), p_rename_to );
}

void grape::libgrape::write_linetype( wxXmlNode *p_parent, linetype p_linetype )
{
  wxXmlNode* xml_linetype = new wxXmlNode( p_parent, wxXML_ELEMENT_NODE, _T( "linetype" ) );

  wxString tt_linetype = wxEmptyString;
  switch( p_linetype )
  {
      case zigzag:
        tt_linetype = _T( "zigzag" ); break;
      case straight:
        tt_linetype = _T( "straight" ); break;
      case curved:
        tt_linetype = _T( "curved" ); break;
  }

  new wxXmlNode( xml_linetype, wxXML_TEXT_NODE, _T( "value" ), tt_linetype );
}

void grape::libgrape::write_channel_type( wxXmlNode *p_parent, channel_type p_channel_type )
{
  wxXmlNode* xml_channel_type = new wxXmlNode( p_parent, wxXML_ELEMENT_NODE, _T( "channeltype" ) );

  wxString cc_channel_type = wxEmptyString;
  switch( p_channel_type )
  {
      case VISIBLE_CHANNEL:
        cc_channel_type = _T( "visible" ); break;
      case HIDDEN_CHANNEL:
        cc_channel_type = _T( "hidden" ); break;
      case BLOCKED_CHANNEL:
        cc_channel_type = _T( "blocked" ); break;
  }

  new wxXmlNode( xml_channel_type, wxXML_TEXT_NODE, _T( "value" ), cc_channel_type );
}

void grape::libgrape::write_channel_communication_type( wxXmlNode *p_parent, channel_communication_type p_channel_communication_type )
{
  wxXmlNode* xml_channel_communication_type = new wxXmlNode( p_parent, wxXML_ELEMENT_NODE, _T( "channelcommunicationtype" ) );

  wxString cc_channel_communication_type = wxEmptyString;
  switch( p_channel_communication_type )
  {
      case VISIBLE_CHANNEL_COMMUNICATION:
        cc_channel_communication_type = _T( "visible" ); break;
      case HIDDEN_CHANNEL_COMMUNICATION:
        cc_channel_communication_type = _T( "hidden" ); break;
      case BLOCKED_CHANNEL_COMMUNICATION:
        cc_channel_communication_type = _T( "blocked" ); break;
  }

  new wxXmlNode( xml_channel_communication_type, wxXML_TEXT_NODE, _T( "value" ), cc_channel_communication_type );
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_wxXmlNode )
