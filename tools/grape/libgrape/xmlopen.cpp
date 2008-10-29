// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xmlopen.cpp
//
// Implements functions to convert an XML file into a grape specification.

#include <wx/tokenzr.h>

#include "specification.h"
#include "xmlopen.h"

using namespace grape::libgrape;

long g_max_id = 0;

compound_state* grape::libgrape::find_compound_state( process_diagram* p_proc_dia, uint p_id )
{
  object *obj_ptr = process_diagram::find_object( p_proc_dia, p_id, STATE );
  obj_ptr = obj_ptr ? obj_ptr : process_diagram::find_object( p_proc_dia, p_id, REFERENCE_STATE );
  return static_cast< compound_state* >( obj_ptr );
}

compound_reference* grape::libgrape::find_compound_reference( architecture_diagram* p_arch_dia, uint p_id )
{
  object *obj_ptr = architecture_diagram::find_object( p_arch_dia, p_id, PROCESS_REFERENCE );
  obj_ptr = obj_ptr ? obj_ptr : architecture_diagram::find_object( p_arch_dia, p_id, ARCHITECTURE_REFERENCE );
  return static_cast< compound_reference* >( obj_ptr );
}

bool grape::libgrape::xml_open( grape_specification* p_spec, const wxString &p_filename, long &p_max_id )
{
  g_max_id = 0;
  bool result = true;
  wxXmlDocument xml_document; // the document to be written to.

  xml_document.Load( p_filename );

  wxString name; //holds name of the child

  /* xml_grape set as root */
  wxXmlNode *xml_grape = xml_document.GetRoot();
  if ( !xml_grape ) // if document is empty or has no root
  {
    return false;
  }

  if ( xml_grape->GetName() != _T( "grape" ) )
  {
    // The root has aan incorrect name
    return false;
  }

  /* look at nodes inside <grape> </grape> */
  wxXmlNode* child = xml_grape->GetChildren();

  while ( child )
  {
    name = child->GetName();
    if (name == _T( "datatypespecificationlist" ) )
    {
      result = result  && open_datatype_specification( p_spec, child );
    }
    else if ( name == _T( "processdiagramlist" ) )
    {
      result = result && open_process_diagrams( p_spec, child );
    }
    else if( name == _T( "architecturediagramlist" ) )
    {
      result = result && open_architecture_diagrams( p_spec, child );
    }
    else
    {
      return false;
    }

    child = child->GetNext();
  }

  // set the references
  for ( uint i = 0; i < p_spec->count_process_diagram(); ++i )
  {
    process_diagram* proc_dia_ptr = p_spec->get_process_diagram( i );
    p_spec->check_references( proc_dia_ptr->get_name(), proc_dia_ptr );
  }
  for ( uint i = 0; i < p_spec->count_architecture_diagram(); ++i )
  {
    architecture_diagram* arch_dia_ptr = p_spec->get_architecture_diagram( i );
    p_spec->check_references( arch_dia_ptr->get_name(), arch_dia_ptr );
  }

  p_max_id = g_max_id;
  return result;
}

bool grape::libgrape::open_datatype_specification( grape_specification* p_spec, wxXmlNode* p_dat_spec_node )
{
  wxXmlNode* d_spec_content = p_dat_spec_node->GetChildren(); // child of the wxXmlNode containing the datatype specification; there is only one child

  wxString name = d_spec_content->GetName();
  if (name != _T( "datatypespecification" ) )
  {
    // Wrong node name, the XML file cannot be used.
    return false;
  }

  p_spec->get_datatype_specification()->set_declarations( d_spec_content->GetNodeContent() );
  return true;
}

bool grape::libgrape::open_process_diagrams( grape_specification* p_spec, wxXmlNode* p_proc_list_node )
{
  bool result = true;
  // The children of a proc list are all process diagrams
  wxXmlNode* proc_dia_node = p_proc_list_node->GetChildren();
  while ( proc_dia_node )
  {
    list_of_decl preamble_parameters;
    list_of_decl_init preamble_variables;
    uint proc_dia_id = 0;
    wxString proc_dia_name = _T( "" );
    wxString node_name = proc_dia_node->GetName();
    if ( node_name == _T( "processdiagram" ) )
    {
      wxXmlNode* proc_dia_info_node = proc_dia_node->GetChildren();
      while ( proc_dia_info_node )
      {
        wxString what_info = proc_dia_info_node->GetName();
        if ( what_info == _T( "id" ) )
        {
          wxString identifier = proc_dia_info_node->GetNodeContent();
          long dummy_id;
          identifier.ToLong( &dummy_id );
          proc_dia_id = ( uint ) dummy_id;
          if ( g_max_id < dummy_id )
          {
             g_max_id = dummy_id;
          }
        }
        else if ( what_info == _T( "name" ) )
        {
          proc_dia_name = proc_dia_info_node->GetNodeContent();
        }
        else if ( what_info == _T( "preambledeclarations" ))
        {
          // get the preamble
          for(wxXmlNode *preamble_node = proc_dia_info_node->GetChildren(); preamble_node != 0; preamble_node = preamble_node->GetNext())
          {
            if(preamble_node->GetName() == _T("parameterlist"))
            {
              // get the parameterlist
              decl preamble_parameter_decl;
              for(wxXmlNode *preamble_parameter = preamble_node->GetChildren(); preamble_parameter != 0; preamble_parameter = preamble_parameter->GetNext())
              {
                if(preamble_parameter->GetName() == _T("param"))
                {
                  wxString preamble_param = preamble_parameter->GetNodeContent();
                  // process parameter declaration
                  wxStringTokenizer tkt( preamble_param, _T(":") );
                  if ( tkt.CountTokens() != 2 || preamble_param.IsEmpty() )
                  {
                    return false;
                  }
                  wxString param_name = tkt.GetNextToken();
                  param_name.Trim( true );
                  param_name.Trim( false );
                  wxStringTokenizer tks( param_name );
                  if ( tks.CountTokens() != 1 || param_name.IsEmpty() )
                  {
                    return false;
                  }
                  wxString param_type = tkt.GetNextToken();
                  param_type.Trim( true );
                  param_type.Trim( false );
                  tks.SetString( param_type );
                  if ( tks.CountTokens() != 1 || param_type.IsEmpty() )
                  {
                    return false;
                  }
		  preamble_parameter_decl.set_name(param_name);
                  preamble_parameter_decl.set_type( param_type );
                  preamble_parameters.Add( preamble_parameter_decl );
                }
              }
            }
            else if(preamble_node->GetName() == _T("localvariablelist"))
            {
              // get the localvariablelist
              decl_init preamble_local_var_decl;
              for(wxXmlNode *preamble_local_variable = preamble_node->GetChildren(); preamble_local_variable != 0; preamble_local_variable = preamble_local_variable->GetNext())
              {
                if(preamble_local_variable->GetName() == _T("var"))
                {
                  wxString preamble_local_var = preamble_local_variable->GetNodeContent();
                  // process parameter declaration
                  wxStringTokenizer tkt( preamble_local_var, _T(":") );
                  if ( tkt.CountTokens() != 2 || preamble_local_var.IsEmpty() )
                  {
                    return false;
                  }
                  wxString local_var_name = tkt.GetNextToken();
                  local_var_name.Trim( true );
                  local_var_name.Trim( false );
                  wxStringTokenizer tks( local_var_name );
                  if ( tks.CountTokens() != 1 || local_var_name.IsEmpty() )
                  {
                    return false;
                  }
                  wxString local_var_rest = tkt.GetNextToken();
                  wxStringTokenizer tkr( local_var_rest, _T("=") );
                  if ( tkr.CountTokens() != 2 || local_var_rest.IsEmpty() )
                  {
                    return false;
                  }
                  wxString local_var_type = tkr.GetNextToken();
                  local_var_type.Trim( true );
                  local_var_type.Trim( false );
                  tks.SetString( local_var_type );
                  if ( tks.CountTokens() != 1 || local_var_type.IsEmpty() )
                  {
                    return false;
                  }
                  wxString local_var_val = tkr.GetNextToken();
                  local_var_val.Trim( true );
                  local_var_val.Trim( false );
                  tks.SetString( local_var_val );
                  if ( tks.CountTokens() != 1 || local_var_val.IsEmpty() )
                  {
                    return false;
                  }
		  preamble_local_var_decl.set_name(local_var_name);
                  preamble_local_var_decl.set_type( local_var_type );
                  preamble_local_var_decl.set_value( local_var_val );
                  preamble_variables.Add( preamble_local_var_decl );
                }
              }
            }
          }
        }

        // get next piece of information about the diagram.
        proc_dia_info_node = proc_dia_info_node->GetNext();
      }
      // create the diagram with the information we've retrieved
      process_diagram* new_process_diagram = p_spec->add_process_diagram( proc_dia_id, proc_dia_name );

      // create the preamble
      new_process_diagram->get_preamble()->set_parameter_declarations_list(preamble_parameters);
      new_process_diagram->get_preamble()->set_local_variable_declarations_list(preamble_variables);

      result = result && open_states( p_spec, proc_dia_node, new_process_diagram );
      result = result && open_reference_states( p_spec, proc_dia_node, new_process_diagram );
      result = result && open_nonterminating_transitions( p_spec, proc_dia_node, new_process_diagram );
      result = result && open_terminating_transitions( p_spec, proc_dia_node, new_process_diagram );
      result = result && open_initial_designators( p_spec, proc_dia_node, new_process_diagram );
      result = result && open_comments( p_spec, proc_dia_node, new_process_diagram );
    }
    else
    { /* invalid! */ }

    // get next diagram.
    proc_dia_node = proc_dia_node->GetNext();
  }

  return result;
}

bool grape::libgrape::open_states( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr )
{
  wxString node_name = p_proc_dia_node->GetName();
  if ( node_name == _T( "processdiagram" ) )
  {
    wxXmlNode* proc_dia_info_node = p_proc_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( proc_dia_info_node )
    {
      wxString what_info = proc_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of states
        wxXmlNode* object_node = proc_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "statelist" ) )
          {
            wxXmlNode* state_node = object_node->GetChildren();
            while ( state_node )
            {
              uint state_identifier = 0;
              coordinate state_coordinate = { 0.0f, 0.0f };
              float state_height = 0.1f;
              float state_width = 0.1f;
              wxString state_name = _T( "" );

              wxXmlNode* state_information = state_node->GetChildren();
              while ( state_information )
              {
                wxString what_info = state_information->GetName();
                if ( what_info == _T( "name" ) )
                {
                  state_name = state_information->GetNodeContent();
                }
                else if ( what_info == _T( "id" ) )
                {
                  wxString identifier = state_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  state_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = state_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      state_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      state_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = state_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      state_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      state_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the state
                state_information = state_information->GetNext();
              }
              // create the state
              state* state_ptr = p_proc_dia_ptr->add_state( state_identifier, state_coordinate, state_width, state_height );
              // set the state name
              state_ptr->set_name( state_name );

              // retrieve the next state
              state_node = state_node->GetNext();
            }
            // break from the while loop; there can only be one statelist
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      proc_dia_info_node = proc_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_reference_states( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr )
{
  wxString node_name = p_proc_dia_node->GetName();
  if ( node_name == _T( "processdiagram" ) )
  {
    wxXmlNode* proc_dia_info_node = p_proc_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( proc_dia_info_node )
    {
      wxString what_info = proc_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of reference states
        wxXmlNode* object_node = proc_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "referencestatelist" ) )
          {
            wxXmlNode* ref_state_node = object_node->GetChildren();
            while ( ref_state_node )
            {
              uint ref_state_identifier = 0;
              coordinate ref_state_coordinate = { 0.0f, 0.0f };
              float ref_state_height = 0.1f;
              float ref_state_width = 0.1f;
              wxString ref_state_name = _T( "" );
              wxString ref_state_parameter_assignments = _T( "" );

              wxXmlNode* ref_state_information = ref_state_node->GetChildren();
              while ( ref_state_information )
              {
                wxString what_info = ref_state_information->GetName();
                if ( what_info == _T( "name" ) )
                {
                  ref_state_name = ref_state_information->GetNodeContent();
                }
                else if ( what_info == _T( "id" ) )
                {
                  wxString identifier = ref_state_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  ref_state_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = ref_state_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      ref_state_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      ref_state_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = ref_state_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      ref_state_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      ref_state_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "parameterassignmentlist" ) )
                {
                  ref_state_parameter_assignments = ref_state_information->GetNodeContent();
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the state
                ref_state_information = ref_state_information->GetNext();
              }
              // create the state
              reference_state* ref_state_ptr = p_proc_dia_ptr->add_reference_state( ref_state_identifier, ref_state_coordinate, ref_state_width, ref_state_height );
              // set the state name
              ref_state_ptr->set_name( ref_state_name );
              ref_state_ptr->set_text( ref_state_parameter_assignments );

              // set the relationship refers to later
              // retrieve the next state
              ref_state_node = ref_state_node->GetNext();
            }
            // break from the while loop; there can only be one statelist
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      proc_dia_info_node = proc_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_nonterminating_transitions( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr )
{
  wxString node_name = p_proc_dia_node->GetName();
  if ( node_name == _T( "processdiagram" ) )
  {
    wxXmlNode* proc_dia_info_node = p_proc_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( proc_dia_info_node )
    {
      wxString what_info = proc_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of nonterminating transitions.
        wxXmlNode* object_node = proc_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "nonterminatingtransitionlist" ) )
          {
            wxXmlNode* ntt_node = object_node->GetChildren();
            while ( ntt_node )
            {
              uint ntt_identifier = 0;
              coordinate ntt_coordinate = { 0.0f, 0.0f };
              float ntt_height = 0.1f;
              float ntt_width = 0.1f;
              uint ntt_to_id = 0;
              uint ntt_from_id = 0;
              label ntt_label;

              wxXmlNode* ntt_information = ntt_node->GetChildren();
              while ( ntt_information )
              {
                wxString what_info = ntt_information->GetName();
                if ( what_info == _T( "id" ) )
                {
                  wxString identifier = ntt_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  ntt_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = ntt_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      ntt_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      ntt_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = ntt_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      ntt_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      ntt_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "to" ) )
                {
                  wxString identifier = ntt_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  ntt_to_id = ( uint ) dummy_id;
                }
                else if ( what_info == _T( "from" ) )
                {
                  wxString identifier = ntt_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  ntt_from_id = ( uint ) dummy_id;
                }
                else if ( what_info == _T("label"))
                {
                  wxXmlNode* label_info = ntt_information->GetChildren();
                  list_of_decl variable_decl_list;
                  wxString condition;
                  list_of_action action_list;
                  wxString timestamp;
                  list_of_varupdate variable_update_list;
                  while( label_info )
                  {
                    if ( label_info->GetName() == _T( "variabledeclarations" ) )
                    {
                      wxXmlNode* variable_declarations = label_info->GetChildren();
                      while ( variable_declarations )
                      {
                        if ( variable_declarations->GetName() == _T( "variabledeclaration" ) )
                        {
                          wxString variable_declaration = variable_declarations->GetNodeContent();
                          wxStringTokenizer tkt( variable_declaration, _T(":") );
                          if ( tkt.CountTokens() == 2 || !variable_declaration.IsEmpty() )
                          {
                            wxString variable_name = tkt.GetNextToken();
                            variable_name.Trim(true);
                            variable_name.Trim(false);
                            wxStringTokenizer tks( variable_name );
                            if (tks.CountTokens() != 1 || variable_name.IsEmpty() )
                            {
                              /* invalid */
                            }
                            wxString variable_type = tkt.GetNextToken();
                            variable_type.Trim(true);
                            variable_type.Trim(false);
                            tks.SetString( variable_type );
                            if (tks.CountTokens() != 1 || variable_type.IsEmpty() )
                            {
                              /* invalid */
                            }
                            decl variable_decl;
                            variable_decl.set_name( variable_name );
                            variable_decl.set_type( variable_type );
                            variable_decl_list.Add( variable_decl );
                          }
                        }
                        variable_declarations = variable_declarations->GetNext();
                      }
                    }
                    else if ( label_info->GetName() == _T( "condition" ) )
                    {
                      condition = label_info->GetNodeContent();
                    }
                    else if ( label_info->GetName() == _T( "actions" ) )
                    {
                      wxXmlNode* actions_info = label_info->GetChildren();
                      while ( actions_info )
                      {
                        action action;
                        if ( actions_info->GetName() == _T( "action" ) )
                        {
                          wxXmlNode* action_info = actions_info->GetChildren();
                          wxString action_name;
                          list_of_dataexpression param_list;
                          while ( action_info )
                          {
                            if ( action_info->GetName() == _T( "name" ) )
                            {
                              action_name = action_info->GetNodeContent();
                            }
                            else if ( action_info->GetName() == _T( "param" ) )
                            {
                              dataexpression action_param;
                              action_param.set_expression( action_info->GetNodeContent() );
                              param_list.Add( action_param );
                            }
                            else
                            {
                              /* invalid */
                            }

                            action_info = action_info->GetNext();
                          }
                          action.set_name( action_name );
                          action.set_parameters( param_list );
                        }
                        else
                        {
                          /* invalid */
                        }
                        action_list.Add( action );

                        actions_info = actions_info->GetNext();
                      }
                    }
                    else if ( label_info->GetName() == _T( "timestamp" ) )
                    {
                      timestamp = label_info->GetNodeContent();
                    }
                    else if ( label_info->GetName() == _T( "variableupdates" ) )
                    {
                      wxXmlNode* variable_updates = label_info->GetChildren();
                      varupdate variable_update;
                      while ( variable_updates )
                      {
                        
                        if ( variable_updates->GetName() == _T( "variableupdate" ) )
                        {
                          wxString variable_update_text = variable_updates->GetNodeContent();
                          int pos = variable_update_text.Find( _T( ":=" ) );
                          wxString variable_update_lhs = variable_update_text.Mid( 0, pos );
                          wxString variable_update_rhs = variable_update_text.Mid( pos+2 );
                          if ( variable_update_lhs.IsEmpty() || variable_update_rhs.IsEmpty() )
                          {
                            /* invalid */
                          }
                          variable_update.set_lhs( variable_update_lhs );
                          variable_update.set_rhs( variable_update_rhs );
                        }
                        else
                        {
                          /* invalid */
                        }
                        variable_update_list.Add( variable_update );

                        variable_updates = variable_updates->GetNext();
                      }
                    }
                    else
                    {
                      /* invalid */
                    }

                    label_info = label_info->GetNext();
                  }
                  ntt_label.set_declarations( variable_decl_list );
                  ntt_label.set_condition( condition );
                  ntt_label.set_actions( action_list );
                  ntt_label.set_timestamp( timestamp );
                  ntt_label.set_variable_updates( variable_update_list );
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the nonterminating transition
                ntt_information = ntt_information->GetNext();
              }
              // find the corresponding beginstate
              compound_state* begin_state_ptr = find_compound_state( p_proc_dia_ptr, ntt_from_id );

              // find the corresponding endstate
              compound_state* end_state_ptr = find_compound_state( p_proc_dia_ptr, ntt_to_id );

              // create the nonterminating transition
              nonterminating_transition* ntt_ptr = p_proc_dia_ptr->add_nonterminating_transition( ntt_identifier, begin_state_ptr, end_state_ptr );
              // set the nonterminating transition width, height and coordinate
              ntt_ptr->set_width( ntt_width );
              ntt_ptr->set_height( ntt_height );
              ntt_ptr->set_coordinate( ntt_coordinate );

              // set the nonterminating transition label
              ntt_ptr->set_label( ntt_label );

              // retrieve the next nonterminating transition
              ntt_node = ntt_node->GetNext();
            }
            // break from the while loop; there can only be one nonterminating transition list
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      proc_dia_info_node = proc_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_terminating_transitions( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr )
{
  wxString node_name = p_proc_dia_node->GetName();
  if ( node_name == _T( "processdiagram" ) )
  {
    wxXmlNode* proc_dia_info_node = p_proc_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( proc_dia_info_node )
    {
      wxString what_info = proc_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of terminating transitions.
        wxXmlNode* object_node = proc_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "terminatingtransitionlist" ) )
          {
            wxXmlNode* tt_node = object_node->GetChildren();
            while ( tt_node )
            {
              uint tt_identifier = 0;
              coordinate tt_coordinate = { 0.0f, 0.0f };
              float tt_height = 0.1f;
              float tt_width = 0.1f;
              uint tt_from_id = 0;
              label tt_label;

              wxXmlNode* tt_information = tt_node->GetChildren();
              while ( tt_information )
              {
                wxString what_info = tt_information->GetName();
                if ( what_info == _T( "id" ) )
                {
                  wxString identifier = tt_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  tt_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = tt_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      tt_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      tt_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = tt_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      tt_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      tt_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "from" ) )
                {
                  wxString identifier = tt_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  tt_from_id = ( uint ) dummy_id;
                }
                else if ( what_info == _T("label"))
                {
                  wxXmlNode* label_info = tt_information->GetChildren();
                  list_of_decl variable_decl_list;
                  wxString condition;
                  list_of_action action_list;
                  wxString timestamp;
                  list_of_varupdate variable_update_list;
                  while( label_info )
                  {
                    if ( label_info->GetName() == _T( "variabledeclarations" ) )
                    {
                      wxXmlNode* variable_declarations = label_info->GetChildren();
                      while ( variable_declarations )
                      {
                        if ( variable_declarations->GetName() == _T( "variabledeclaration" ) )
                        {
                          wxString variable_declaration = variable_declarations->GetNodeContent();
                          wxStringTokenizer tkt( variable_declaration, _T(":") );
                          if ( tkt.CountTokens() == 2 || !variable_declaration.IsEmpty() )
                          {
                            wxString variable_name = tkt.GetNextToken();
                            variable_name.Trim(true);
                            variable_name.Trim(false);
                            wxStringTokenizer tks( variable_name );
                            if (tks.CountTokens() != 1 || variable_name.IsEmpty() )
                            {
                              /* invalid */
                            }
                            wxString variable_type = tkt.GetNextToken();
                            variable_type.Trim(true);
                            variable_type.Trim(false);
                            tks.SetString( variable_type );
                            if (tks.CountTokens() != 1 || variable_type.IsEmpty() )
                            {
                              /* invalid */
                            }
                            decl variable_decl;
                            variable_decl.set_name( variable_name );
                            variable_decl.set_type( variable_type );
                            variable_decl_list.Add( variable_decl );
                          }
                        }
                        variable_declarations = variable_declarations->GetNext();
                      }
                    }
                    else if ( label_info->GetName() == _T( "condition" ) )
                    {
                      condition = label_info->GetNodeContent();
                    }
                    else if ( label_info->GetName() == _T( "actions" ) )
                    {
                      wxXmlNode* actions_info = label_info->GetChildren();
                      while ( actions_info )
                      {
                        action action;
                        if ( actions_info->GetName() == _T( "action" ) )
                        {
                          wxXmlNode* action_info = actions_info->GetChildren();
                          wxString action_name;
                          list_of_dataexpression param_list;
                          while ( action_info )
                          {
                            if ( action_info->GetName() == _T( "name" ) )
                            {
                              action_name = action_info->GetNodeContent();
                            }
                            else if ( action_info->GetName() == _T( "param" ) )
                            {
                              dataexpression action_param;
                              action_param.set_expression( action_info->GetNodeContent() );
                              param_list.Add( action_param );
                            }
                            else
                            {
                              /* invalid */
                            }

                            action_info = action_info->GetNext();
                          }
                          action.set_name( action_name );
                          action.set_parameters( param_list );
                        }
                        else
                        {
                          /* invalid */
                        }
                        action_list.Add( action );

                        actions_info = actions_info->GetNext();
                      }
                    }
                    else if ( label_info->GetName() == _T( "timestamp" ) )
                    {
                      timestamp = label_info->GetNodeContent();
                    }
                    else if ( label_info->GetName() == _T( "variableupdates" ) )
                    {
                      wxXmlNode* variable_updates = label_info->GetChildren();
                      varupdate variable_update;
                      while ( variable_updates )
                      {
                        if ( variable_updates->GetName() == _T( "variableupdate" ) )
                        {
                          wxString variable_update_text = variable_updates->GetNodeContent();
                          int pos = variable_update_text.Find( _T( ":=" ) );
                          wxString variable_update_lhs = variable_update_text.Mid( 0, pos );
                          wxString variable_update_rhs = variable_update_text.Mid( pos+2 );
                          if ( variable_update_lhs.IsEmpty() || variable_update_rhs.IsEmpty() )
                          {
                            /* invalid */
                          }
                          variable_update.set_lhs( variable_update_lhs );
                          variable_update.set_rhs( variable_update_rhs );
                        }
                        else
                        {
                          /* invalid */
                        }
                        variable_update_list.Add( variable_update );

                        variable_updates = variable_updates->GetNext();
                      }
                    }
                    else
                    {
                      /* invalid */
                    }

                    label_info = label_info->GetNext();
                  }
                  tt_label.set_declarations( variable_decl_list );
                  tt_label.set_condition( condition );
                  tt_label.set_actions( action_list );
                  tt_label.set_timestamp( timestamp );
                  tt_label.set_variable_updates( variable_update_list );
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the terminating transition
                tt_information = tt_information->GetNext();
              }
              // find the corresponding beginstate
              compound_state* begin_state_ptr = find_compound_state( p_proc_dia_ptr, tt_from_id );

              // create the terminating transition
              // use a dummy coordinate, width and height and coordinate are set later.
              coordinate dummy_coordinate;
              terminating_transition* tt_ptr = p_proc_dia_ptr->add_terminating_transition( tt_identifier, begin_state_ptr, dummy_coordinate );
              // set the terminating transition width, height and coordinate
              tt_ptr->set_width( tt_width );
              tt_ptr->set_height( tt_height );
              tt_ptr->set_coordinate( tt_coordinate );

              // set the terminating transition label
              tt_ptr->set_label( tt_label );

              // retrieve the next terminating transition
              tt_node = tt_node->GetNext();
            }
            // break from the while loop; there can only be one terminating transition list
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      proc_dia_info_node = proc_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_initial_designators( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr )
{
  wxString node_name = p_proc_dia_node->GetName();
  if ( node_name == _T( "processdiagram" ) )
  {
    wxXmlNode* proc_dia_info_node = p_proc_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( proc_dia_info_node )
    {
      wxString what_info = proc_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of initial designators.
        wxXmlNode* object_node = proc_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "initialdesignatorlist" ) )
          {
            wxXmlNode* init_node = object_node->GetChildren();
            while ( init_node )
            {
              uint init_identifier = 0;
              coordinate init_coordinate = { 0.0f, 0.0f };
              float init_height = 0.1f;
              float init_width = 0.1f;
              uint init_to_id = 0;

              wxXmlNode* init_information = init_node->GetChildren();
              while ( init_information )
              {
                wxString what_info = init_information->GetName();
                if ( what_info == _T( "id" ) )
                {
                  wxString identifier = init_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  init_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = init_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      init_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      init_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = init_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      init_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      init_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "propertyof" ) )
                {
                  wxString identifier = init_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  init_to_id = ( uint ) dummy_id;
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the initial designator
                init_information = init_information->GetNext();
              }
              // find the corresponding beginstate
              compound_state* designated_ptr = find_compound_state( p_proc_dia_ptr, init_to_id );

              // create the initial designator
              // use a dummy coordinate, width and height and coordinate are set later.
              p_proc_dia_ptr->add_initial_designator( init_identifier, designated_ptr, init_width, init_height, init_coordinate );

              // retrieve the next initial designator
              init_node = init_node->GetNext();
            }
            // break from the while loop; there can only be one initial designator list
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      proc_dia_info_node = proc_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_comments( grape_specification* p_spec, wxXmlNode* p_proc_dia_node, process_diagram* p_proc_dia_ptr )
{
  wxString node_name = p_proc_dia_node->GetName();
  if ( node_name == _T( "processdiagram" ) )
  {
    wxXmlNode* proc_dia_info_node = p_proc_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( proc_dia_info_node )
    {
      wxString what_info = proc_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of comments
        wxXmlNode* object_node = proc_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "commentlist" ) )
          {
            wxXmlNode* comment_node = object_node->GetChildren();
            while ( comment_node )
            {
              uint comment_identifier = 0;
              coordinate comment_coordinate = { 0.0f, 0.0f };
              float comment_height = 0.1f;
              float comment_width = 0.1f;
              wxString comment_text = _T( "" );
              uint object_id = 0;

              wxXmlNode* comment_information = comment_node->GetChildren();
              while ( comment_information )
              {
                wxString what_info = comment_information->GetName();
                if ( what_info == _T( "id" ) )
                {
                  wxString identifier = comment_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  comment_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = comment_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      comment_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      comment_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = comment_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      comment_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      comment_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "propertyof" ) )
                {
                  wxString identifier = comment_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  object_id = ( uint ) dummy_id;
                }
                else if ( what_info == _T( "text" ) )
                {
                  comment_text = comment_information->GetNodeContent();
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the nonterminating transition
                comment_information = comment_information->GetNext();
              }
              // find the corresponding beginstate
              object* obj_ptr = process_diagram::find_object( p_proc_dia_ptr, object_id );

              // create the comment
              // use a dummy coordinate, width and height and coordinate are set later.
              comment* comment_ptr = p_proc_dia_ptr->add_comment( comment_identifier, comment_coordinate, comment_width, comment_height );
              comment_ptr->set_text( comment_text );
              if ( obj_ptr )
              {
                p_proc_dia_ptr->attach_comment_to_object( comment_ptr, obj_ptr );
              }

              // retrieve the next nonterminating transition
              comment_node = comment_node->GetNext();
            }
            // break from the while loop; there can only be one statelist
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      proc_dia_info_node = proc_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_architecture_diagrams( grape_specification* p_spec, wxXmlNode* p_arch_list_node )
{
  bool result = true;
  // The children of an arch list are all architecture diagrams
  wxXmlNode* arch_dia_node = p_arch_list_node->GetChildren();
  while ( arch_dia_node )
  {
    uint arch_dia_id = 0;
    wxString arch_dia_name = _T( "" );
    wxString node_name = arch_dia_node->GetName();
    if ( node_name == _T( "architecturediagram" ) )
    {
      wxXmlNode* arch_dia_info_node = arch_dia_node->GetChildren();
      while ( arch_dia_info_node )
      {
        wxString what_info = arch_dia_info_node->GetName();
        if ( what_info == _T( "id" ) )
        {
          wxString identifier = arch_dia_info_node->GetNodeContent();
          long dummy_id;
          identifier.ToLong( &dummy_id );
          arch_dia_id = ( uint ) dummy_id;
          if ( g_max_id < dummy_id )
          {
             g_max_id = dummy_id;
          }
        }
        else if ( what_info == _T( "name" ) )
        {
          arch_dia_name = arch_dia_info_node->GetNodeContent();
        }
        // get next piece of information about the diagram.
        arch_dia_info_node = arch_dia_info_node->GetNext();
      }
      // create the diagram with the information we've retrieved
      architecture_diagram* new_architecture_diagram = p_spec->add_architecture_diagram( arch_dia_id, arch_dia_name );
      result = result && open_process_references( p_spec, arch_dia_node, new_architecture_diagram );
      result = result && open_architecture_references( p_spec, arch_dia_node, new_architecture_diagram );
      result = result && open_channels( p_spec, arch_dia_node, new_architecture_diagram );
      result = result && open_channel_communications( p_spec, arch_dia_node, new_architecture_diagram );
      result = result && open_visibles( p_spec, arch_dia_node, new_architecture_diagram );
      result = result && open_blockeds( p_spec, arch_dia_node, new_architecture_diagram );
      result = result && open_comments( p_spec, arch_dia_node, new_architecture_diagram );
    }
    else
    { /* invalid! */ }

    // get next diagram.
    arch_dia_node = arch_dia_node->GetNext();
  }
  return result;
}

bool grape::libgrape::open_process_references( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr )
{
  wxString node_name = p_arch_dia_node->GetName();
  if ( node_name == _T( "architecturediagram" ) )
  {
    wxXmlNode* arch_dia_info_node = p_arch_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( arch_dia_info_node )
    {
      wxString what_info = arch_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of process references
        wxXmlNode* object_node = arch_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "processreferencelist" ) )
          {
            wxXmlNode* proc_ref_node = object_node->GetChildren();
            while ( proc_ref_node )
            {
              uint proc_ref_identifier = 0;
              coordinate proc_ref_coordinate = { 0.0f, 0.0f };
              float proc_ref_height = 0.1f;
              float proc_ref_width = 0.1f;
              wxString proc_ref_name = _T( "" );
              list_of_varupdate proc_ref_parameter_assignments;

              wxXmlNode* proc_ref_information = proc_ref_node->GetChildren();
              while ( proc_ref_information )
              {
                wxString what_info = proc_ref_information->GetName();
                if ( what_info == _T( "name" ) )
                {
                  proc_ref_name = proc_ref_information->GetNodeContent();
                }
                else if ( what_info == _T( "id" ) )
                {
                  wxString identifier = proc_ref_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  proc_ref_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = proc_ref_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      proc_ref_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      proc_ref_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = proc_ref_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      proc_ref_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      proc_ref_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "parameterassignmentlist" ) )
                {
                  wxXmlNode* parameter_info = proc_ref_information->GetChildren();
                  varupdate parameter_update;
                  while ( parameter_info )
                  {
                    if ( parameter_info->GetName() == _T( "parameterassignment" ) )
                    {
                      wxString parameter_update_text = parameter_info->GetNodeContent();
                      int pos = parameter_update_text.Find( _T( ":=" ) );
                      wxString parameter_update_lhs = parameter_update_text.Mid( 0, pos );
                      wxString parameter_update_rhs = parameter_update_text.Mid( pos+2 );
                      if ( parameter_update_lhs.IsEmpty() || parameter_update_rhs.IsEmpty() )
                      {
                        /* invalid */
                      }
                      parameter_update.set_lhs( parameter_update_lhs );
                      parameter_update.set_rhs( parameter_update_rhs );
                    }
                    else
                    {
                      /* invalid */
                    }
                    proc_ref_parameter_assignments.Add( parameter_update );

                    parameter_info = parameter_info->GetNext();
                  }
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the state
                proc_ref_information = proc_ref_information->GetNext();
              }
              // create the state
              process_reference* proc_ref_ptr = p_arch_dia_ptr->add_process_reference( proc_ref_identifier, proc_ref_coordinate, proc_ref_width, proc_ref_height );
              // set the state name
              proc_ref_ptr->set_name( proc_ref_name );
              proc_ref_ptr->set_parameter_updates( proc_ref_parameter_assignments );

              // set the relationship refers to later
              // retrieve the next state
              proc_ref_node = proc_ref_node->GetNext();
            }
            // break from the while loop; there can only be one statelist
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      arch_dia_info_node = arch_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_architecture_references( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr )
{
  wxString node_name = p_arch_dia_node->GetName();
  if ( node_name == _T( "architecturediagram" ) )
  {
    wxXmlNode* arch_dia_info_node = p_arch_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( arch_dia_info_node )
    {
      wxString what_info = arch_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of architecture references
        wxXmlNode* object_node = arch_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "architecturereferencelist" ) )
          {
            wxXmlNode* arch_ref_node = object_node->GetChildren();
            while ( arch_ref_node )
            {
              uint arch_ref_identifier = 0;
              coordinate arch_ref_coordinate = { 0.0f, 0.0f };
              float arch_ref_height = 0.1f;
              float arch_ref_width = 0.1f;
              wxString arch_ref_name = _T( "" );

              wxXmlNode* arch_ref_information = arch_ref_node->GetChildren();
              while ( arch_ref_information )
              {
                wxString what_info = arch_ref_information->GetName();
                if ( what_info == _T( "name" ) )
                {
                  arch_ref_name = arch_ref_information->GetNodeContent();
                }
                else if ( what_info == _T( "id" ) )
                {
                  wxString identifier = arch_ref_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  arch_ref_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = arch_ref_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      arch_ref_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      arch_ref_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = arch_ref_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      arch_ref_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      arch_ref_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the state
                arch_ref_information = arch_ref_information->GetNext();
              }
              // create the state
              architecture_reference* arch_ref_ptr = p_arch_dia_ptr->add_architecture_reference( arch_ref_identifier, arch_ref_coordinate, arch_ref_width, arch_ref_height );
              // set the state name
              arch_ref_ptr->set_name( arch_ref_name );

              // set the relationship refers to later
              // retrieve the next state
              arch_ref_node = arch_ref_node->GetNext();
            }
            // break from the while loop; there can only be one statelist
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      arch_dia_info_node = arch_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_channels( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr )
{
  wxString node_name = p_arch_dia_node->GetName();
  if ( node_name == _T( "architecturediagram" ) )
  {
    wxXmlNode* arch_dia_info_node = p_arch_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( arch_dia_info_node )
    {
      wxString what_info = arch_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of architecture references
        wxXmlNode* object_node = arch_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "channellist" ) )
          {
            wxXmlNode* channel_node = object_node->GetChildren();
            while ( channel_node )
            {
              uint channel_identifier = 0;
              coordinate channel_coordinate = { 0.0f, 0.0f };
              float channel_height = 0.1f;
              float channel_width = 0.1f;
              wxString channel_name = _T( "" );
              uint channel_reference_id = 0;

              wxXmlNode* channel_information = channel_node->GetChildren();
              while ( channel_information )
              {
                wxString what_info = channel_information->GetName();
                if ( what_info == _T( "name" ) )
                {
                  channel_name = channel_information->GetNodeContent();
                }
                else if ( what_info == _T( "id" ) )
                {
                  wxString identifier = channel_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  channel_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = channel_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      channel_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      channel_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = channel_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      channel_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      channel_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "onreference" ) )
                {
                  wxString identifier = channel_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  channel_reference_id = ( uint ) dummy_id;
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the state
                channel_information = channel_information->GetNext();
              }
              compound_reference* ref_ptr = find_compound_reference( p_arch_dia_ptr, channel_reference_id );
              if( ref_ptr == 0 )
              {
                return false;
              }
              // create the channel
              channel* chan_ptr = p_arch_dia_ptr->add_channel( channel_identifier, channel_coordinate, channel_width, channel_height, ref_ptr );
              // set the channel name
              chan_ptr->set_name( channel_name );

              // retrieve the next channel
              channel_node = channel_node->GetNext();
            }
            // break from the while loop; there can only be one statelist
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      arch_dia_info_node = arch_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}


bool grape::libgrape::open_channel_communications( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr )
{
  wxString node_name = p_arch_dia_node->GetName();
  if ( node_name == _T( "architecturediagram" ) )
  {
    wxXmlNode* arch_dia_info_node = p_arch_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( arch_dia_info_node )
    {
      wxString what_info = arch_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of architecture references
        wxXmlNode* object_node = arch_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "channelcommunicationlist" ) )
          {
            wxXmlNode* communication_node = object_node->GetChildren();
            while ( communication_node )
            {
              uint communication_identifier = 0;
              coordinate communication_coordinate = { 0.0f, 0.0f };
              float communication_height = 0.1f;
              float communication_width = 0.1f;
              arr_channel_ptr channels;
              channels.Empty();

              wxXmlNode* communication_information = communication_node->GetChildren();
              while ( communication_information )
              {
                wxString what_info = communication_information->GetName();
                if ( what_info == _T( "id" ) )
                {
                  wxString identifier = communication_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  communication_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = communication_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      communication_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      communication_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = communication_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      communication_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      communication_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "connectionlist" ) )
                {
                  wxXmlNode* connected_channel = communication_information->GetChildren();
                  while ( connected_channel )
                  {
                    if ( connected_channel->GetName() == _T( "connectedtochannels" ) )
                    {
                      wxString identifier = connected_channel->GetNodeContent();
                      long dummy_id;
                      identifier.ToLong( &dummy_id );
                      uint channel_identifier = ( uint ) dummy_id;
                      channel* chan_ptr = dynamic_cast<channel*> ( architecture_diagram::find_object( p_arch_dia_ptr, channel_identifier ) );
                      channels.Add( chan_ptr );
                    }
                    else
                    { /* invalid! */ }

                    connected_channel = connected_channel->GetNext();
                  }
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the state
                communication_information = communication_information->GetNext();
              }
              if ( channels.GetCount() >= 2 )
              {
                channel_communication* comm_ptr = p_arch_dia_ptr->add_channel_communication( communication_identifier, communication_coordinate, channels.Item( 0 ), channels.Item( 1 ) );
                comm_ptr->set_width( communication_width );
                comm_ptr->set_height( communication_height );
                for ( uint i = 2; i < channels.GetCount(); ++i )
                {
                  channel* chan_ptr = channels.Item( i );
                  p_arch_dia_ptr->attach_channel_communication_to_channel( comm_ptr,  chan_ptr );
                }
              }
              else
              { /* invalid! */ }

              // retrieve the next channel communication
              communication_node = communication_node->GetNext();
            }
            // break from the while loop; there can only be one statelist
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      arch_dia_info_node = arch_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_visibles( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr )
{
  wxString node_name = p_arch_dia_node->GetName();
  if ( node_name == _T( "architecturediagram" ) )
  {
    wxXmlNode* arch_dia_info_node = p_arch_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( arch_dia_info_node )
    {
      wxString what_info = arch_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of architecture references
        wxXmlNode* object_node = arch_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "visiblelist" ) )
          {
            wxXmlNode* visible_node = object_node->GetChildren();
            while ( visible_node )
            {
              uint visible_identifier = 0;
              coordinate visible_coordinate = { 0.0f, 0.0f };
              float visible_height = 0.1f;
              float visible_width = 0.1f;
              wxString visible_name = _T( "" );
              uint visible_connection_id = 0;

              wxXmlNode* visible_information = visible_node->GetChildren();
              while ( visible_information )
              {
                wxString what_info = visible_information->GetName();
                if ( what_info == _T( "name" ) )
                {
                  visible_name = visible_information->GetNodeContent();
                }
                else if ( what_info == _T( "id" ) )
                {
                  wxString identifier = visible_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  visible_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = visible_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      visible_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      visible_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = visible_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      visible_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      visible_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "propertyof" ) )
                {
                  wxString identifier = visible_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  visible_connection_id = ( uint ) dummy_id;
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the state
                visible_information = visible_information->GetNext();
              }
              connection* conn_ptr = dynamic_cast<connection*> ( architecture_diagram::find_object( p_arch_dia_ptr, visible_connection_id ) );

              // create the visible
              visible* vis_ptr = p_arch_dia_ptr->add_visible( visible_identifier, visible_coordinate, visible_width, visible_height, conn_ptr );
              // set the visible name
              vis_ptr->set_name( visible_name );

              // retrieve the next visible
              visible_node = visible_node->GetNext();
            }
            // break from the while loop; there can only be one statelist
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      arch_dia_info_node = arch_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_blockeds( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr )
{
  wxString node_name = p_arch_dia_node->GetName();
  if ( node_name == _T( "architecturediagram" ) )
  {
    wxXmlNode* arch_dia_info_node = p_arch_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( arch_dia_info_node )
    {
      wxString what_info = arch_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of architecture references
        wxXmlNode* object_node = arch_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "blockedlist" ) )
          {
            wxXmlNode* blocked_node = object_node->GetChildren();
            while ( blocked_node )
            {
              uint blocked_identifier = 0;
              coordinate blocked_coordinate = { 0.0f, 0.0f };
              float blocked_height = 0.1f;
              float blocked_width = 0.1f;
              uint blocked_connection_id = 0;

              wxXmlNode* blocked_information = blocked_node->GetChildren();
              while ( blocked_information )
              {
                wxString what_info = blocked_information->GetName();
                if ( what_info == _T( "id" ) )
                {
                  wxString identifier = blocked_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  blocked_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = blocked_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      blocked_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      blocked_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = blocked_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      blocked_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      blocked_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "propertyof" ) )
                {
                  wxString identifier = blocked_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  blocked_connection_id = ( uint ) dummy_id;
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the state
                blocked_information = blocked_information->GetNext();
              }
              connection* conn_ptr = dynamic_cast<connection*> ( architecture_diagram::find_object( p_arch_dia_ptr, blocked_connection_id ) );

              // create the blocked
              p_arch_dia_ptr->add_blocked( blocked_identifier, blocked_coordinate, blocked_width, blocked_height, conn_ptr );

              // retrieve the next blocked
              blocked_node = blocked_node->GetNext();
            }
            // break from the while loop; there can only be one statelist
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      arch_dia_info_node = arch_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}

bool grape::libgrape::open_comments( grape_specification* p_spec, wxXmlNode* p_arch_dia_node, architecture_diagram* p_arch_dia_ptr )
{
  wxString node_name = p_arch_dia_node->GetName();
  if ( node_name == _T( "architecturediagram" ) )
  {
    wxXmlNode* proc_dia_info_node = p_arch_dia_node->GetChildren();
    // search for information about the diagram; find the list of objects
    while ( proc_dia_info_node )
    {
      wxString what_info = proc_dia_info_node->GetName();
      if ( what_info == _T( "objectlist" ) )
      {
        // search for the list of comments
        wxXmlNode* object_node = proc_dia_info_node->GetChildren();
        while ( object_node )
        {
          wxString what_object = object_node->GetName();
          if ( what_object == _T( "commentlist" ) )
          {
            wxXmlNode* comment_node = object_node->GetChildren();
            while ( comment_node )
            {
              uint comment_identifier = 0;
              coordinate comment_coordinate = { 0.0f, 0.0f };
              float comment_height = 0.1f;
              float comment_width = 0.1f;
              wxString comment_text = _T( "" );
              uint object_id = 0;

              wxXmlNode* comment_information = comment_node->GetChildren();
              while ( comment_information )
              {
                wxString what_info = comment_information->GetName();
                if ( what_info == _T( "id" ) )
                {
                  wxString identifier = comment_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  comment_identifier = ( uint ) dummy_id;
                  if ( g_max_id < dummy_id )
                  {
                     g_max_id = dummy_id;
                  }
                }
                else if ( what_info == _T( "size" ) )
                {
                  wxXmlNode* size_info = comment_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "width" ) )
                    {
                      wxString width = size_info->GetNodeContent();
                      double dummy_width;
                      width.ToDouble( &dummy_width );
                      comment_width = ( float ) dummy_width;
                    }
                    else if ( size_info->GetName() == _T( "height" ) )
                    {
                      wxString height = size_info->GetNodeContent();
                      double dummy_height;
                      height.ToDouble( &dummy_height );
                      comment_height = ( float ) dummy_height;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "coord" ) )
                {
                  wxXmlNode* size_info = comment_information->GetChildren();
                  while ( size_info )
                  {
                    if ( size_info->GetName() == _T( "x" ) )
                    {
                      wxString x = size_info->GetNodeContent();
                      double dummy_x;
                      x.ToDouble( &dummy_x );
                      comment_coordinate.m_x = ( float ) dummy_x;
                    }
                    else if ( size_info->GetName() == _T( "y" ) )
                    {
                      wxString y = size_info->GetNodeContent();
                      double dummy_y;
                      y.ToDouble( &dummy_y );
                      comment_coordinate.m_y = ( float ) dummy_y;
                    }
                    else
                    { /* invalid! */ }
                    size_info = size_info->GetNext();
                  }
                }
                else if ( what_info == _T( "propertyof" ) )
                {
                  wxString identifier = comment_information->GetNodeContent();
                  long dummy_id;
                  identifier.ToLong( &dummy_id );
                  object_id = ( uint ) dummy_id;
                }
                else if ( what_info == _T( "text" ) )
                {
                  comment_text = comment_information->GetNodeContent();
                }
                else
                { /* invalid! */ }

                // find the next piece of information about the nonterminating transition
                comment_information = comment_information->GetNext();
              }
              // find the corresponding beginstate
              object* obj_ptr = architecture_diagram::find_object( p_arch_dia_ptr, object_id );

              // create the comment
              // use a dummy coordinate, width and height and coordinate are set later.
              comment* comment_ptr = p_arch_dia_ptr->add_comment( comment_identifier, comment_coordinate, comment_width, comment_height );
              comment_ptr->set_text( comment_text );
              if ( obj_ptr )
              {
                p_arch_dia_ptr->attach_comment_to_object( comment_ptr, obj_ptr );
              }

              // retrieve the next nonterminating transition
              comment_node = comment_node->GetNext();
            }
            // break from the while loop; there can only be one statelist
            break;
          } // end if
          object_node = object_node->GetNext();
        }
      }
      proc_dia_info_node = proc_dia_info_node->GetNext();
    }
  }
  else
  { /* invalid! */ }

  return true;
}
