// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2gen.cpp
//
// Implements the functions to convert process diagrams and architecture diagrams to mCRL2.

#include "wx.hpp" // precompiled headers

#include "mcrl2gen.h"
#include "mcrl2gen_validate.h"

#include <sstream>

#include "wx/wx.h"

#include <wx/file.h>
#include "mcrl2/atermpp/aterm_init.h"            // Library for messaging.
#include "mcrl2/utilities/logger.h"            // Library for messaging.
#include "mcrl2/core/parse.h"                // Parse library.
#include "mcrl2/core/typecheck.h"            // Type check library.

using namespace grape::mcrl2gen;
using namespace grape::libgrape;
using namespace mcrl2::core;
using namespace std;

// Dynamic array implementations
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(arr_channel_id)
WX_DEFINE_OBJARRAY(arr_action_reference)
WX_DEFINE_OBJARRAY(arr_channel_comm)
WX_DEFINE_OBJARRAY(arr_renamed)

void grape::mcrl2gen::init_mcrl2libs(int /*p_argc*/, char** p_argv)
{
  // initialize ATerm library and set verbose messaging
  MCRL2_ATERMPP_INIT(p_argc, p_argv);
  mcrl2::log::mcrl2_logger::set_reporting_level(mcrl2::log::verbose);
}

void grape::mcrl2gen::verbose_actions(list_of_action& p_actions)
{
  for (unsigned int i=0; i<p_actions.GetCount(); ++i)
  {
    cerr << "  " << p_actions[i].get_name().ToAscii();
    wxString action_text = wxEmptyString;
    for (unsigned int j=0; j<p_actions[i].get_parameters().GetCount(); ++j)
    {
      if (!p_actions[i].get_parameters()[j].get_type().IsEmpty())
      {
        action_text += p_actions[i].get_parameters()[j].get_type();
        if (j != p_actions[i].get_parameters().GetCount()-1)
        {
          action_text += _T(" # ");
        }
      }
    }
    if (!action_text.IsEmpty())
    {
      cerr << ": " << action_text.ToAscii();
    }
    cerr << endl;
  }
  return;
}

bool grape::mcrl2gen::is_channel_of_reference(wxXmlNode* p_architecture_diagram, wxString& p_channel_id, wxString& p_reference_id)
{
  // initialize variables
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
  wxXmlNode* objects = get_child(p_architecture_diagram, _T("objectlist"));
  wxXmlNode* channels = get_child(objects, _T("channellist"));

  // determine if there is a channel p_channel_id which belongs to p_reference_id
  for (wxXmlNode* channel_child = channels->GetChildren(); channel_child != 0; channel_child = channel_child->GetNext())
  {
    wxString channel_id = get_child_value(channel_child, _T("id"));
    if (channel_id == p_channel_id)
    {
      // found matching channel
      wxString channel_of = get_child_value(channel_child, _T("onreference"));
      if (channel_of == p_reference_id)
      {
        // found matching reference
        return true;
      }
    }
  }
  // nothing found, return false
  return false;
}

wxArrayString grape::mcrl2gen::get_process_references(wxXmlNode* p_diagram)
{
  // initialize variables
  wxArrayString refs;
  wxXmlNode* objects = get_child(p_diagram, _T("objectlist"));
  wxXmlNode* ref_list = 0;

  // get references
  if (p_diagram->GetName() == _T("processdiagram"))
  {
    ref_list = get_child(objects, _T("referencestatelist"));
  }
  else if (p_diagram->GetName() == _T("architecturediagram"))
  {
    ref_list = get_child(objects, _T("processreferencelist"));
  }

  if (ref_list != 0)
  {
    // collect references
    for (wxXmlNode* ref = ref_list->GetChildren(); ref != 0; ref = ref->GetNext())
    {
      refs.Add(get_child_value(ref, _T("propertyof")));
    }
  }
  return refs;
}

list_of_varupdate grape::mcrl2gen::get_process_reference_initialisation(wxXmlNode* p_doc_root, wxXmlNode* p_architecture_diagram, wxString& p_reference_id, wxString& /*p_reference_name*/, ATermAppl& datatype_spec)
{
  // initialize variables
  list_of_varupdate inits;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
  wxXmlNode* objects = get_child(p_architecture_diagram, _T("objectlist"));
  wxXmlNode* process_references = get_child(objects, _T("processreferencelist"));

  // find matching process reference
  wxXmlNode* reference;
  for (reference = process_references->GetChildren(); reference != 0; reference = reference->GetNext())
  {
    wxString ref_id = get_child_value(reference, _T("id"));
    if (ref_id == p_reference_id)
    {
      break;
    }
  }

  wxString proc_ref_prop = get_child_value(reference, _T("propertyof"));
  wxXmlNode* referenced_diagram = 0;
  referenced_diagram = get_diagram(p_doc_root, proc_ref_prop);

  if (!validate_reference_parameters(p_doc_root, reference, p_architecture_diagram, inits, datatype_spec))
  {
    return inits;
  }

  return inits;
}

wxArrayString grape::mcrl2gen::get_architecture_references(wxXmlNode* p_diagram)
{
  // initialize variables
  wxArrayString refs;
  wxXmlNode* objects = get_child(p_diagram, _T("objectlist"));
  wxXmlNode* ref_list;

  // get references
  ref_list = get_child(objects, _T("architecturereferencelist"));

  if (ref_list != 0)
  {
    // collect references
    for (wxXmlNode* ref = ref_list->GetChildren(); ref != 0; ref = ref->GetNext())
    {
      refs.Add(get_child_value(ref, _T("propertyof")));
    }
  }
  return refs;
}

wxString grape::mcrl2gen::get_state_name(wxXmlNode* p_process_diagram, wxString& p_id, bool& p_is_ref)
{
  wxXmlNode* objects = get_child(p_process_diagram, _T("objectlist"));
  for (wxXmlNode* child = objects->GetChildren(); child != 0; child = child->GetNext())
  {
    // process objectlist children
    if (child->GetName() == _T("statelist"))
    {
      for (wxXmlNode* t_state = child->GetChildren(); t_state != 0; t_state = t_state->GetNext())
      {
        wxString state_id = get_child_value(t_state, _T("id"));
        if (state_id == p_id)
        {
          p_is_ref = false;
          return get_child_value(t_state, _T("name"));
        }
      }
    }
    if (child->GetName() == _T("referencestatelist"))
    {
      for (wxXmlNode* t_refstate = child->GetChildren(); t_refstate != 0; t_refstate = t_refstate->GetNext())
      {
        wxString refstate_id = get_child_value(t_refstate, _T("id"));
        if (refstate_id == p_id)
        {
          p_is_ref = true;
          return get_child_value(t_refstate, _T("name"));
        }
      }
    }
  }
  // nothing found, return emptystring
  return wxEmptyString;
}

arr_channel_id grape::mcrl2gen::get_reference_channels(wxXmlNode* p_doc_root, wxXmlNode* p_architecture_diagram, action_reference& p_reference, ATermAppl& datatype_spec)
{
  // initialize variables
  arr_channel_id channels;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
  wxXmlNode* objects = get_child(p_architecture_diagram, _T("objectlist"));
  wxXmlNode* channel_list = get_child(objects, _T("channellist"));

  list_of_action reference_actions;
  if (p_reference.m_is_process_reference)
  {
    reference_actions = get_process_actions(p_doc_root, p_reference.m_diagram_id, datatype_spec);
  }
  else
  {
    reference_actions = get_architecture_visibles(p_doc_root, p_reference.m_diagram_id, datatype_spec);
  }
  // determine channels associated with reference
  for (wxXmlNode* channel_child = channel_list->GetChildren(); channel_child != 0;  channel_child = channel_child->GetNext())
  {
    wxString channel_ref = get_child_value(channel_child, _T("onreference"));
    if (channel_ref == p_reference.m_reference_id)
    {
      for (unsigned int i = 0; i < reference_actions.GetCount(); ++i)
      {
        wxString channel_name = get_child_value(channel_child, _T("name"));
        if (channel_name == reference_actions[i].get_name())
        {
          channel_id channel;
          channel.m_channel = reference_actions[i];
          channel.m_channel_id = get_child_value(channel_child, _T("id"));
          channels.Add(channel);
        }
      }
    }
  }
  return channels;
}

wxArrayString grape::mcrl2gen::get_reference_channels(wxXmlNode* p_architecture_diagram, wxString& p_reference_id, wxString& p_channel_type)
{
  // initialize variables
  wxArrayString channels;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
  wxXmlNode* objects = get_child(p_architecture_diagram, _T("objectlist"));
  wxXmlNode* channel_list = get_child(objects, _T("channellist"));

  // determine if reference p_reference_id has any associated blockeds
  for (wxXmlNode* channel_child = channel_list->GetChildren(); channel_child != 0; channel_child = channel_child->GetNext())
  {
    // get channel type
    wxString channel_type = get_child_value(channel_child, _T("channeltype"));
    wxXmlNode* connection_list = get_child(channel_child, _T("connectionlist"));
    wxXmlNode* connection = 0;
    if (connection_list)
    {
      connection = get_child(connection_list, _T("connectedtochannelcommunication"));
    }

    if (channel_type == p_channel_type && connection == 0)
    {
      // get channel id
      wxString channel_id = get_child_value(channel_child, _T("id"));
      if (is_channel_of_reference(p_architecture_diagram, channel_id, p_reference_id))
      {
        // get channel name
        wxString channel_name = get_child_value(channel_child, _T("name"));
        // the blocked channel belongs to this reference, add name of channel to list
        channels.Add(channel_name);
      }
    }
  }
  return channels;
}

wxArrayString grape::mcrl2gen::get_reference_hidden_actions(list_of_action& p_actions, arr_channel_id& p_channels, wxArrayString& p_invisibles)
{
  wxArrayString hidden;

  // for each action in p_actions, if it is not in p_channels or p_invisibles, it is hidden
  for (unsigned int i=0; i<p_actions.GetCount(); ++i)
  {
    bool is_hidden = true;
    for (unsigned int j=0; j<p_channels.GetCount(); ++j)
    {
      if (p_channels[j].m_channel.get_name() == p_actions[i].get_name())
      {
        is_hidden = false;
        break;
      }
    }
    if (is_hidden)
    {
      for (unsigned int j=0; j<p_invisibles.GetCount(); ++j)
      {
        if (p_invisibles[j] == p_actions[i].get_name())
        {
          is_hidden = false;
          break;
        }
      }
    }
    if (is_hidden)
    {
      hidden.Add(p_actions[i].get_name());
    }
  }

  return hidden;
}

arr_channel_id grape::mcrl2gen::get_reference_renamed_actions(arr_channel_id& p_channels, wxArrayString& p_blockeds)
{
  // initialize variables
  arr_channel_id ren;
  arr_channel_id new_ren;

  // for each action in p_channels, if it is not blocked, rename it
  for (unsigned int i=0; i<p_channels.GetCount(); ++i)
  {
    bool is_renamed = true;
    for (unsigned int j=0; j<p_blockeds.GetCount(); ++j)
    {
      if (p_blockeds[j] == p_channels[i].m_channel.get_name())
      {
        is_renamed = false;
        break;
      }
    }

    if (is_renamed)
    {
      ren.Add(p_channels[i]);
    }
  }

  // compact members
  for (unsigned int i=0; i<ren.GetCount(); ++i)
  {
    bool found = false;
    for (unsigned int j=0; j<new_ren.GetCount(); ++j)
    {
      if (ren[i].m_channel.get_name() == new_ren[j].m_channel.get_name())
      {
        list_of_dataexpression channel_params = ren[i].m_channel.get_parameters();
        list_of_dataexpression new_channel_params = new_ren[j].m_channel.get_parameters();
        if (channel_params.GetCount() == new_channel_params.GetCount())
        {
          found = true;
          unsigned int k = 0;
          while (found && k < channel_params.GetCount())
          {
            found = channel_params[k].get_type() == new_channel_params[k].get_type();
            ++k;
          }
          if (found)
          {
            break;
          }
        }
      }
    }
    if (!found)
    {
      new_ren.Add(ren[i]);
    }
  }
  return new_ren;
}

arr_channel_comm grape::mcrl2gen::get_communications(wxXmlNode* /* p_doc_root */ , wxXmlNode* p_architecture_diagram, arr_action_reference& p_refs, arr_channel_comm& p_blocked_comms, arr_channel_comm& p_hidden_comms, arr_channel_comm& p_visible_comms)
{
  // initialize variables
  arr_channel_comm comms;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
  wxXmlNode* objects = get_child(p_architecture_diagram, _T("objectlist"));
  wxXmlNode* channel_comms = get_child(objects, _T("channelcommunicationlist"));
  unsigned int comm_counter = 0;

  for (wxXmlNode* child = channel_comms->GetChildren(); child != 0; child = child->GetNext())
  {
    channel_comm current;
    wxString comm_id;
    comm_id.Printf(_T("%d"), comm_counter++);
    current.m_name = _T("comm") + diagram_name + comm_id;
    current.m_channels.Empty();
    current.m_id = get_child_value(child, _T("id"));

    wxXmlNode* connectionlist = get_child(child, _T("connectionlist"));
    for (wxXmlNode* connection = connectionlist->GetChildren(); connection != 0; connection = connection->GetNext())
    {
      wxString conn_id = connection->GetNodeContent();
      // search through renamed channels to find correct channel
      bool found = false;
      for (unsigned int i=0; i<p_refs.GetCount(); ++i)
      {
        for (unsigned int j=0; j<p_refs[i].m_renamed.GetCount(); ++j)
        {
          if (p_refs[i].m_renamed[j].m_channel_id == conn_id)
          {
            found = true;
            current.m_channels.Add(p_refs[i].m_renamed[j]);
          }
        }
        if (found)
        {
          break;
        }
      }
    }
    comms.Add(current);

    // get channel communication type
    wxString channel_communication_type = get_child_value(child, _T("channelcommunicationtype"));
    if (channel_communication_type == _T("blocked"))
    {
      p_blocked_comms.Add(current);
    }
    else if (channel_communication_type == _T("hidden"))
    {
      p_hidden_comms.Add(current);
    }
    else if (channel_communication_type == _T("visible"))
    {
      p_visible_comms.Add(current);
    }

  }
  return comms;
}

arr_renamed grape::mcrl2gen::get_communication_channel_renamed(wxXmlNode* /* p_doc_root */, wxXmlNode* p_architecture_diagram, arr_action_reference& p_refs, arr_channel_comm& p_communications)
{
  // initialize variables
  arr_renamed ren;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
  wxXmlNode* objects = get_child(p_architecture_diagram, _T("objectlist"));
  wxXmlNode* channels = get_child(objects, _T("channellist"));
  wxXmlNode* channel_communications = get_child(objects, _T("channelcommunicationlist"));
  // for each channel in p_refs, go through the list of visibles
  // in this specification and add it to ren if there is a visible property for the
  // channel.
  for (wxXmlNode* channel_child = channels->GetChildren(); channel_child != 0; channel_child = channel_child->GetNext())
  {
    // get channel type
    wxString channel_type = get_child_value(channel_child, _T("channeltype"));
    if (channel_type == _T("visible"))
    {
      bool is_found = false;
      // get channel id
      wxString channel_id = get_child_value(channel_child, _T("id"));
      for (unsigned int i=0; i<p_refs.GetCount(); ++i)
      {
        // only renamed channels are candidates
        for (unsigned int j=0; j<p_refs[i].m_renamed.GetCount(); ++j)
        {
          if (p_refs[i].m_renamed[j].m_channel_id == channel_id)
          {
            // found match
            // get channel rename
            wxString visible_channel_name = get_child_value(channel_child, _T("rename"));
            if (visible_channel_name.IsEmpty())
            {
              visible_channel_name = get_child_value(channel_child, _T("name"));
            }
            renamed vis_ren;
            vis_ren.m_old_name = p_refs[i].m_renamed[j].m_channel.get_name() + _T("_") + p_refs[i].m_renamed[j].m_channel_id;
            vis_ren.m_new.set_name(visible_channel_name);
            vis_ren.m_new.set_parameters(p_refs[i].m_renamed[j].m_channel.get_parameters());
            ren.Add(vis_ren);
            is_found = true;
            break;
          }
        }
        if (is_found)
        {
          // stop search
          break;
        }
      }
    }
  }

  // for each channel communication in p_communications, go through the list of visibles
  // in this specification and add it to ren if there is a visible property for the
  // channel communication.
  for (wxXmlNode* channel_communication_child = channel_communications->GetChildren(); channel_communication_child != 0; channel_communication_child = channel_communication_child->GetNext())
  {
    // get channel communication type
    wxString channel_communication_type = get_child_value(channel_communication_child, _T("channelcommunicationtype"));
    if (channel_communication_type == _T("visible"))
    {
      // get channel communication id
      wxString channel_communication_id = get_child_value(channel_communication_child, _T("id"));
      for (unsigned int i=0; i<p_communications.GetCount(); ++i)
      {
        if (channel_communication_id == p_communications[i].m_id)
        {
          // found match
          for (unsigned int j=0; j<p_communications[i].m_channels.GetCount(); ++j)
          {
            bool found = false;
            for (unsigned int k=j+1; k<p_communications[i].m_channels.GetCount(); ++k)
            {
              list_of_dataexpression comm_params1 = p_communications[i].m_channels[j].m_channel.get_parameters();
              list_of_dataexpression comm_params2 = p_communications[i].m_channels[k].m_channel.get_parameters();
              if (comm_params1.GetCount() == comm_params2.GetCount())
              {
                found = true;
                unsigned int l = 0;
                while (found && l < comm_params1.GetCount())
                {
                  found = comm_params1[l].get_type() == comm_params2[l].get_type();
                  ++l;
                }
                if (found)
                {
                  break;
                }
              }
            }
            if (!found)
            {
              // get channel communication rename
              wxString visible_channel_communication_name = get_child_value(channel_communication_child, _T("name"));
              renamed vis_ren;
              vis_ren.m_old_name = p_communications[i].m_name;
              vis_ren.m_new.set_name(visible_channel_communication_name);
              vis_ren.m_new.set_parameters(p_communications[i].m_channels[j].m_channel.get_parameters());
              ren.Add(vis_ren);
            }
          }
          break;
        }
      }
    }
  }
  return ren;
}

bool grape::mcrl2gen::export_architecture_diagram_to_mcrl2(wxXmlDocument& p_spec, wxString& p_filename, wxString& p_diagram_id, bool p_verbose, bool p_save)
{
  try
  {
    // an architecture diagram can contain both architecture references and process references,
    // this has to be taken into account in the main loop

    list_of_action  actions;                      // all occuring actions
    wxXmlNode*       doc_root = p_spec.GetRoot(); // stores the root node of the document
    wxArrayString   sort_expressions;             // stores all state sort expressions
    wxArrayString   internal_specs;               // stores all internal process specifications
    wxArrayString   proc_specs;                   // stores all process specifications
    wxArrayString   arch_specs;                   // stores all architecture diagram process specifications

    // stores all diagrams that need to be added to the mCRL2 specification.
    // make difference between arch and proc
    wxArrayString arch_to_mcrl2, proc_to_mcrl2, arch_in_mcrl2, proc_in_mcrl2;
    arch_to_mcrl2.Add(p_diagram_id);

    ATermAppl datatype_spec;
    validate_datatype_specification(doc_root, datatype_spec);

    // collect all needed diagrams found in the architectures
    while (!arch_to_mcrl2.IsEmpty())
    {
      wxString arch_id = arch_to_mcrl2[0];
      arch_to_mcrl2.RemoveAt(0);
      arch_in_mcrl2.Add(arch_id);

      wxXmlNode* diagram = get_diagram(doc_root, arch_id);
      // collect processes
      wxArrayString proc_refs = get_process_references(diagram);
      for (unsigned int i=0; i<proc_refs.GetCount(); ++i)
      {
        bool found = false;
        for (unsigned int j=0; j<proc_to_mcrl2.GetCount(); ++j)
        {
          if (proc_refs[i] == proc_to_mcrl2[j])
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          proc_to_mcrl2.Add(proc_refs[i]);
        }
      }

      // collect architectures
      wxArrayString arch_refs = get_architecture_references(diagram);
      for (unsigned int i=0; i<arch_refs.GetCount(); ++i)
      {
        bool found = false;
        for (unsigned int j=0; j<arch_to_mcrl2.GetCount(); ++j)
        {
          if (arch_refs[i] == arch_to_mcrl2[j])
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          arch_to_mcrl2.Add(arch_refs[i]);
        }
      }
    }

    // collect all needed process diagrams and process them
    while (!proc_to_mcrl2.IsEmpty())
    {
      wxString proc_id = proc_to_mcrl2[0];
      proc_to_mcrl2.RemoveAt(0);
      proc_in_mcrl2.Add(proc_id);

      // make proc_specification
      process_diagram_mcrl2(doc_root, proc_id, sort_expressions, actions, internal_specs, proc_specs, datatype_spec, p_verbose);
      wxXmlNode* diagram = get_diagram(doc_root, proc_id);
      wxArrayString proc_refs = get_process_references(diagram);
      for (unsigned int i=0; i<proc_refs.GetCount(); ++i)
      {
        bool found = false;
        for (unsigned int j=0; j<proc_in_mcrl2.GetCount(); ++j)
        {
          if (proc_refs[i] == proc_in_mcrl2[j])
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          proc_to_mcrl2.Add(proc_refs[i]);
        }
      }
    }

    // initialize arch_to_mcrl2 + reset arch_in_mcrl2
    arch_to_mcrl2 = arch_in_mcrl2;
    arch_in_mcrl2.Empty();

// @arcs: first collect actions from proc_refs (done above), then add type to channels (possible channel actions), then collect blocked, hidden, renamed channels
//        then collect channelcommunications + add type to chan_comms, then collect blocked, hidden, renamed chan_coms
//        make arch specification

    // loop through archs
    while (!arch_to_mcrl2.IsEmpty())
    {
      wxString arch_id = arch_to_mcrl2[0];
      arch_to_mcrl2.RemoveAt(0);
      arch_in_mcrl2.Add(arch_id);

      // make arch_specification
      arr_action_reference refs;
      arr_renamed renameds;
      arr_channel_comm comms;
      architecture_diagram_mcrl2(doc_root, arch_id, refs, renameds, comms, arch_specs, datatype_spec, p_verbose);

      // add renamed new actions to actions
      for (unsigned int i=0; i<renameds.GetCount(); ++i)
      {
        action ren;
        ren.set_name(renameds[i].m_new.get_name());
        ren.set_parameters(renameds[i].m_new.get_parameters());
        actions.Add(ren);
      }
      // add channels
      for (unsigned int i=0; i<refs.GetCount(); ++i)
      {
        for (unsigned int j=0; j<refs[i].m_renamed.GetCount(); ++j)
        {
          wxString ren = refs[i].m_renamed[j].m_channel.get_name() + _T("_") + refs[i].m_renamed[j].m_channel_id;
          action ren_decl;
          ren_decl.set_name(ren);
          ren_decl.set_parameters(refs[i].m_renamed[j].m_channel.get_parameters());
          actions.Add(ren_decl);
        }
      }
      // add channel communications
      for (unsigned int i=0; i<comms.GetCount(); ++i)
      {
        arr_channel_id actions_found, new_actions_found;
        for (unsigned int j=0; j<comms[i].m_channels.GetCount(); ++j)
        {
          wxString channel_id = comms[i].m_channels[j].m_channel_id;
          new_actions_found.Empty();
          for (unsigned int k=0; k<refs.GetCount(); ++k)
          {
            arr_channel_id comm_actions = refs[k].m_renamed;
            for (unsigned int l=0; l<comm_actions.GetCount(); ++l)
            {
              if (comm_actions[l].m_channel_id == channel_id)
              {
                bool act_found = false;
                for (unsigned m=0; m<actions_found.GetCount(); ++m)
                {
                  if (comm_actions[l].m_channel.get_parameters().GetCount() == actions_found[m].m_channel.get_parameters().GetCount())
                  {
                    act_found = true;
                    list_of_dataexpression actions_params = comm_actions[l].m_channel.get_parameters();
                    list_of_dataexpression actions_found_params = actions_found[m].m_channel.get_parameters();
                    unsigned int n = 0;
                    while (act_found && n < actions_params.GetCount())
                    {
                      act_found = actions_params[n].get_type() == actions_found_params[n].get_type();
                      ++n;
                    }
                    if (act_found)
                    {
                      break;
                    }
                  }
                }
                if (act_found || actions_found.IsEmpty())
                {
                  new_actions_found.Add(comm_actions[l]);
                }
              }
            }
          }
          actions_found = new_actions_found;
        }
        action ren;
        ren.set_name(comms[i].m_name);
        for (unsigned int j = 0; j < actions_found.GetCount(); ++j)
        {
          ren.set_parameters(actions_found[j].m_channel.get_parameters());
          actions.Add(ren);
        }
      }
    }

    // remove duplicate actions
    list_of_action new_actions;
    compact_list_action(actions, new_actions);
    actions = new_actions;

    // construct general expressions
    wxString dat_spec = datatype_specification_mcrl2(doc_root);
    wxXmlNode* diagram = get_diagram(doc_root, p_diagram_id);
    wxString exp_diagram_name = get_child_value(diagram, _T("name"));
    wxString init_spec = architecture_diagram_mcrl2_init(exp_diagram_name);
    if (p_verbose)
    {
      cerr << "initial process specification:" << endl << " "
           << init_spec.ToAscii() << endl;
    }

    // construct mCRL2 specification
    wxString specification = wxEmptyString;
    specification += dat_spec + _T("\n\n");
    for (unsigned int i=0; i<sort_expressions.GetCount(); ++i)
    {
      specification += sort_expressions[i];
    }
    specification += _T("\n");
    if (actions.GetCount() > 0)
    {
      specification += _T("act \n");
    }
    for (unsigned int i=0; i<actions.GetCount(); ++i)
    {
      specification += _T("  ");
      specification += actions[i].get_name();
      wxString action_text = wxEmptyString;
      if (actions[i].get_parameters().GetCount() != 0)
      {
        for (unsigned int j=0; j<actions[i].get_parameters().GetCount(); ++j)
        {
          if (!actions[i].get_parameters()[j].get_type().IsEmpty())
          {
            action_text += actions[i].get_parameters()[j].get_type();
            if (j != actions[i].get_parameters().GetCount()-1)
            {
              action_text += _T(" # ");
            }
          }
        }
      }
      if (!action_text.IsEmpty())
      {
        specification += _T(": ") + action_text;
      }
      specification += _T(";\n");
    }
    specification += _T("\n");
    for (size_t i=internal_specs.GetCount(); i>0; --i)
    {
      specification += internal_specs[i-1] + _T("\n");
      specification += proc_specs[i-1] + _T("\n\n");
    }
    for (size_t i=arch_specs.GetCount(); i>0; --i)
    {
      specification += arch_specs[i-1] + _T("\n\n");
    }
    specification += init_spec;

    if (p_verbose)
    {
      cerr << "spec:" << endl;
      cerr << specification.ToAscii() << endl;
    }

    /* For DEBUG
        wxFile file_error;
        wxString error_filename = p_filename;
        error_filename.Replace(_T(".mcrl2"), _T(".error"));
        file_error.Open(error_filename, wxFile::write);
        if(!file_error.IsOpened())
        {
          // ERROR: could not open file to write to
          cerr << "Could not open file "
               << error_filename.ToAscii() << "." << endl;
          throw CONVERSION_ERROR;
        }
        file_error.Write(specification);
        file_error.Close();
    */

    // try to parse constructed mCRL2 specification
    ATermAppl a_parsed_mcrl2_spec = parse_proc_spec(specification);
    if (a_parsed_mcrl2_spec == 0)
    {
      // ERROR: failed to parse constructed spec
      cerr << "Exported specification could not be parsed." << endl;
      throw CONVERSION_ERROR;
    }
    ATermAppl a_typed_mcrl2_spec = type_check_proc_spec(a_parsed_mcrl2_spec);
    if (a_typed_mcrl2_spec == 0)
    {
      // ERROR: failed to type check constructed spec
      cerr << "Exported specification is not well-typed." << endl;
      throw CONVERSION_ERROR;
    }

    if (p_save)
    {
      wxFile file_out;
      file_out.Open(p_filename, wxFile::write);
      if (!file_out.IsOpened())
      {
        // ERROR: could not open file to write to
        cerr << "Could not open file "
             << p_filename.ToAscii() << "." << endl;
        throw CONVERSION_ERROR;
      }
      file_out.Write(specification);
      file_out.Close();
    }
  }
  catch (int i)
  {
    throw i;
    return false;
  }

  cerr << "Export to mCRL2 is successful." << endl;
  return true;
}

bool grape::mcrl2gen::export_process_diagram_to_mcrl2(wxXmlDocument& p_spec, wxString& p_filename, wxString& p_diagram_id, list_of_decl_init& p_parameters_init, bool p_verbose, bool p_save)
{
  try
  {
    list_of_action actions;                     // all occuring actions
    wxXmlNode*       doc_root;                  // stores the root node of the document
    wxArrayString   sort_expressions;           // stores all state sort expressions
    wxArrayString   internal_specs;             // stores all internal process specifications
    wxArrayString   specs;                      // stores all process specifications

    // Stores all process diagrams that need to be added to the mCRL2 specification.
    wxArrayString to_mcrl2, in_mcrl2;
    to_mcrl2.Add(p_diagram_id);
    in_mcrl2.Empty();

    doc_root = p_spec.GetRoot();

    ATermAppl datatype_spec;
    validate_datatype_specification(doc_root, datatype_spec);

    sort_expressions.Empty();
    internal_specs.Empty();
    specs.Empty();

    while (!to_mcrl2.IsEmpty())
    {
      wxString id = to_mcrl2[0];
      to_mcrl2.RemoveAt(0);
      if (in_mcrl2.Index(id) != wxNOT_FOUND)
      {
        // diagram is already exported
        continue;
      }
      in_mcrl2.Add(id, 1);

      // assure specification is valid
      wxXmlNode* process_diagram = get_diagram(doc_root, p_diagram_id);
      if (!validate_process_diagram(doc_root, process_diagram, datatype_spec))
      {
        throw CONVERSION_ERROR;
      }

      // Apply steps of algorithm on XML data.
      process_diagram_mcrl2(doc_root, id, sort_expressions, actions, internal_specs, specs, datatype_spec, p_verbose);
      wxXmlNode* diagram = get_diagram(doc_root, id);

      // determine recursive process references
      wxArrayString references = get_process_references(diagram);
      for (unsigned int i=0; i<references.GetCount(); ++i)
      {
        bool found = false;
        for (unsigned int j=0; j<in_mcrl2.GetCount(); ++j)
        {
          if (in_mcrl2[j] == references[i])
          {
            found = true;
          }
        }

        if (!found)
        {
          to_mcrl2.Add(references[i]);
        }
      }
    }

    // construct general expressions
    wxString dat_spec = datatype_specification_mcrl2(doc_root);
    wxXmlNode* diagram = get_diagram(doc_root, p_diagram_id);
    wxString exp_diagram_name = get_child_value(diagram, _T("name"));
    wxString init_spec = process_diagram_mcrl2_init(doc_root, exp_diagram_name, p_parameters_init);
    if (p_verbose)
    {
      cerr << "initial process specification:" << endl << " "
           << init_spec.ToAscii() << endl;
    }

    // construct mCRL2 specification
    wxString specification = wxEmptyString;
    specification += dat_spec + _T("\n\n");
    for (unsigned int i=0; i<sort_expressions.GetCount(); ++i)
    {
      specification += sort_expressions[i];
    }
    specification += _T("\n");
    if (actions.GetCount() > 0)
    {
      specification += _T("act \n");
    }
    for (unsigned int i=0; i<actions.GetCount(); ++i)
    {
      specification += _T("  ");
      specification += actions[i].get_name();
      wxString action_text = wxEmptyString;
      if (actions[i].get_parameters().GetCount() != 0)
      {
        for (unsigned int j=0; j<actions[i].get_parameters().GetCount(); ++j)
        {
          if (!actions[i].get_parameters()[j].get_type().IsEmpty())
          {
            action_text += actions[i].get_parameters()[j].get_type();
            if (j != actions[i].get_parameters().GetCount()-1)
            {
              action_text += _T(" # ");
            }
          }
        }
      }
      if (!action_text.IsEmpty())
      {
        specification += _T(": ") + action_text;
      }
      specification += _T(";\n");
    }
    specification += _T("\n");
    for (size_t i=internal_specs.GetCount(); i>0; --i)
    {
      specification += internal_specs[i-1] + _T("\n");
      specification += specs[i-1] + _T("\n\n");
    }
    specification += init_spec;

    if (p_verbose)
    {
      cerr << "spec:" << endl;
      cerr << specification.ToAscii() << endl;
    }

    // try to parse constructed mCRL2 specification
    ATermAppl a_parsed_mcrl2_spec = parse_proc_spec(specification);
    if (a_parsed_mcrl2_spec == NULL)
    {
      // ERROR: failed to parse constructed spec
      cerr << "Exported specification could not be parsed." << endl;
      throw CONVERSION_ERROR;
    }
    ATermAppl a_typed_mcrl2_spec = type_check_proc_spec(a_parsed_mcrl2_spec);
    if (a_typed_mcrl2_spec == NULL)
    {
      // ERROR: failed to type check constructed spec
      cerr << "Exported specification is not well-typed." << endl;
      throw CONVERSION_ERROR;
    }

    if (p_save)
    {
      wxFile file_out;
      file_out.Open(p_filename, wxFile::write);
      if (!file_out.IsOpened())
      {
        // ERROR: could not open file to write to
        cerr << "Could not open file "
             << p_filename.ToAscii() << "." << endl;
        throw CONVERSION_ERROR;
      }
      file_out.Write(specification);
      file_out.Close();
    }
  }
  catch (int i)
  {
    throw i;
    return false;
  }

  cerr << "Export to mCRL2 is successful." << endl;
  return true;
}

bool grape::mcrl2gen::export_datatype_specification_to_mcrl2(wxXmlDocument& p_spec, wxString& p_filename, bool /*p_verbose*/, bool p_save)
{
  wxXmlNode* doc_root = p_spec.GetRoot();
  wxString dat_spec = datatype_specification_mcrl2(doc_root);
  if (p_save)
  {
    wxFile file_out;
    file_out.Open(p_filename, wxFile::write);
    if (!file_out.IsOpened())
    {
      // ERROR: could not open file to write to
      cerr << "Could not open file "
           << p_filename.ToAscii() << "." << endl;
      throw CONVERSION_ERROR;
      return false;
    }
    file_out.Write(dat_spec);
    file_out.Close();
  }
  cerr << "Export to mCRL2 is succesful." << endl;
  return true;
}

wxString grape::mcrl2gen::datatype_specification_mcrl2(wxXmlNode* p_doc_root)
{
  wxXmlNode* spec_list = get_child(p_doc_root, _T("datatypespecificationlist"));
  wxString datatype_spec = get_child_value(spec_list, _T("datatypespecification"));
  return datatype_spec;
}

void grape::mcrl2gen::architecture_diagram_mcrl2(wxXmlNode* p_doc_root, wxString& p_diagram_id, arr_action_reference& p_refs, arr_renamed& p_renameds, arr_channel_comm& p_channel_comms, wxArrayString& p_spec, ATermAppl& datatype_spec, bool p_verbose)
{
  // initialize variables
  wxXmlNode* diagram = get_diagram(p_doc_root, p_diagram_id);
  wxString diagram_name = get_child_value(diagram, _T("name"));
  if (p_verbose)
  {
    cerr << "Exporting architecture diagram " << diagram_name.ToAscii() << "." << endl;
  }

  // get all possible actions in this diagram
  if (p_verbose)
  {
    cerr << "Inferred possible actions in " << diagram_name.ToAscii() << ":" << endl;
  }

  p_refs.Empty();
  architecture_diagram_mcrl2_actions(p_doc_root, diagram, p_refs, datatype_spec);
  for (unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    list_of_action new_actions;
    compact_list_action(p_refs[i].m_actions, new_actions);
    p_refs[i].m_actions = new_actions;
  }

  if (p_verbose)
  {
    for (unsigned int i=0; i<p_refs.GetCount(); ++i)
    {
      cerr << " -" << p_refs[i].m_reference.ToAscii() << ":" << endl;
      verbose_actions(p_refs[i].m_actions);
    }
  }

  // get all channels in this diagram
  if (p_verbose)
  {
    cerr << "Inferred channels in " << diagram_name.ToAscii() << ":" << endl;
  }
  for (unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if (p_refs[i].m_reference_id != _T("-1"))
    {
      p_refs[i].m_channels = get_reference_channels(p_doc_root, diagram, p_refs[i], datatype_spec);
    }
  }
  if (p_verbose)
  {
    for (unsigned int i=0; i<p_refs.GetCount(); ++i)
    {
      if (p_refs[i].m_reference_id != _T("-1"))
      {
        cerr << " -" << p_refs[i].m_reference.ToAscii() << ":" << endl;
        list_of_action tmp_acts;
        for (unsigned int j=0; j<p_refs[i].m_channels.GetCount(); ++j)
        {
          tmp_acts.Add(p_refs[i].m_channels[j].m_channel);
        }
        verbose_actions(tmp_acts);
      }
    }
  }

  // get all blocked channels in this diagram
  if (p_verbose)
  {
    cerr << "Inferred blocked channels in " << diagram_name.ToAscii() << ":" << endl;
  }
  for (unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if (p_refs[i].m_reference_id != _T("-1"))
    {
      wxString blocked_text = _T("blocked");
      p_refs[i].m_blockeds = get_reference_channels(diagram, p_refs[i].m_reference_id, blocked_text);
    }
  }
  if (p_verbose)
  {
    for (unsigned int i=0; i<p_refs.GetCount(); ++i)
    {
      if (p_refs[i].m_reference_id != _T("-1"))
      {
        cerr << " -" << p_refs[i].m_reference.ToAscii() << ":" << endl;
        for (unsigned int j=0; j<p_refs[i].m_blockeds.GetCount(); ++j)
        {
          cerr << "  " << p_refs[i].m_blockeds[j].ToAscii() << endl;
        }
      }
    }
  }

  // get all hidden channels in this diagram
  if (p_verbose)
  {
    cerr << "Inferred hidden channels in " << diagram_name.ToAscii() << ":" << endl;
  }
  for (unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if (p_refs[i].m_reference_id != _T("-1"))
    {
      wxString hidden_text = _T("hidden");
      wxArrayString hidden = get_reference_channels(diagram, p_refs[i].m_reference_id, hidden_text);
      WX_APPEND_ARRAY(hidden, get_reference_hidden_actions(p_refs[i].m_actions, p_refs[i].m_channels, p_refs[i].m_blockeds));
      p_refs[i].m_hidden = hidden;
    }
  }
  if (p_verbose)
  {
    for (unsigned int i=0; i<p_refs.GetCount(); ++i)
    {
      if (p_refs[i].m_reference_id != _T("-1"))
      {
        cerr << " -" << p_refs[i].m_reference.ToAscii() << ":" << endl;
        for (unsigned int j=0; j<p_refs[i].m_hidden.GetCount(); ++j)
        {
          cerr << "  " << p_refs[i].m_hidden[j].ToAscii() << endl;
        }
      }
    }
  }

  // get all renamed actions in this diagram
  if (p_verbose)
  {
    cerr << "Inferred renamed actions in " << diagram_name.ToAscii() << ":" << endl;
  }
  for (unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if (p_refs[i].m_reference_id != _T("-1"))
    {
      wxArrayString invisibles = p_refs[i].m_blockeds;
      WX_APPEND_ARRAY(invisibles, p_refs[i].m_hidden);
      p_refs[i].m_renamed = get_reference_renamed_actions(p_refs[i].m_channels, invisibles);
    }
  }
  if (p_verbose)
  {
    for (unsigned int i=0; i<p_refs.GetCount(); ++i)
    {
      if (p_refs[i].m_reference_id != _T("-1"))
      {
        cerr << " -" << p_refs[i].m_reference.ToAscii() << ":" << endl;
        list_of_action tmp_actions;
        for (unsigned int j=0; j<p_refs[i].m_renamed.GetCount(); ++j)
        {
          tmp_actions.Add(p_refs[i].m_renamed[j].m_channel);
        }
        verbose_actions(tmp_actions);
      }
    }
  }

  // get all channel communications in this diagram
  if (p_verbose)
  {
    cerr << "Channel communications in " << diagram_name.ToAscii() << ":" << endl;
  }
  arr_channel_comm blocked_comms;
  arr_channel_comm hidden_comms;
  arr_channel_comm visible_comms;
  p_channel_comms = get_communications(p_doc_root, diagram, p_refs, blocked_comms, hidden_comms, visible_comms);
  if (p_verbose)
  {
    for (unsigned int i=0; i<p_channel_comms.GetCount(); ++i)
    {
      for (unsigned int j=0; j<p_channel_comms[i].m_channels.GetCount(); ++j)
      {
        bool found = false;
        for (unsigned int k=j+1; k<p_channel_comms[i].m_channels.GetCount(); ++k)
        {
          if (p_channel_comms[i].m_channels[j].m_channel.get_parameters().GetCount() == p_channel_comms[i].m_channels[k].m_channel.get_parameters().GetCount())
          {
            found = true;
            for (unsigned int l=0; l<p_channel_comms[i].m_channels[j].m_channel.get_parameters().GetCount(); ++l)
            {
              if (p_channel_comms[i].m_channels[j].m_channel.get_parameters()[l].get_type() != p_channel_comms[i].m_channels[k].m_channel.get_parameters()[l].get_type())
              {
                found = false;
                break;
              }
            }
            if (found)
            {
              break;
            }
          }
        }
        if (!found)
        {
          int channel_counter = 0;
          for (unsigned int k=0; k<p_channel_comms[i].m_channels.GetCount(); ++k)
          {
            bool found2 = false;
            for (unsigned int l=k+1; l<p_channel_comms[i].m_channels.GetCount(); ++l)
            {
              if (p_channel_comms[i].m_channels[k].m_channel_id == p_channel_comms[i].m_channels[l].m_channel_id)
              {
                found2 = true;
                break;
              }
            }
            if (!found2)
            {

              if (channel_counter > 0)
              {
                cerr << "|";
              }
              cerr << p_channel_comms[i].m_channels[k].m_channel.get_name().ToAscii()
                   << p_channel_comms[i].m_channels[k].m_channel_id.ToAscii();
              ++channel_counter;
            }
          }
          cerr << " -> " << p_channel_comms[i].m_name.ToAscii();

          wxString channel_comm_type_text = wxEmptyString;
          for (unsigned int l=0; l<p_channel_comms[i].m_channels[j].m_channel.get_parameters().GetCount(); ++l)
          {
            if (!p_channel_comms[i].m_channels[j].m_channel.get_parameters()[l].get_type().IsEmpty())
            {
              channel_comm_type_text += p_channel_comms[i].m_channels[j].m_channel.get_parameters()[l].get_type();
              if (l != p_channel_comms[i].m_channels[j].m_channel.get_parameters().GetCount()-1)
              {
                channel_comm_type_text += _T(" # ");
              }
            }
          }
          if (!channel_comm_type_text.IsEmpty())
          {
            cerr << ": " << channel_comm_type_text.ToAscii();
          }
          cerr << endl;
        }
      }
    }
  }

  // get all blocked communications in this diagram
  if (p_verbose)
  {
    cerr << "Blocked channel communications in " << diagram_name.ToAscii() << ":" << endl;
    for (unsigned int i=0; i<blocked_comms.GetCount(); ++i)
    {
      cerr << " " << blocked_comms[i].m_name.ToAscii() << endl;
    }
  }

  // get all non-visible communications in this diagram
  if (p_verbose)
  {
    cerr << "Hidden channel communications in " << diagram_name.ToAscii() << ":" << endl;
    for (unsigned int i=0; i<hidden_comms.GetCount(); ++i)
    {
      cerr << " " << hidden_comms[i].m_name.ToAscii() << endl;
    }
  }

  // get all visible channels and channel communications in this diagram
  if (p_verbose)
  {
    cerr << "Visible channels and channel communications in " << diagram_name.ToAscii() << ":" << endl;
  }
  p_renameds = get_communication_channel_renamed(p_doc_root, diagram, p_refs, p_channel_comms);
  if (p_verbose)
  {
    list_of_action tmp_actions;
    for (unsigned int i=0; i<p_renameds.GetCount(); ++i)
    {
      tmp_actions.Add(p_renameds[i].m_new);
    }
    verbose_actions(tmp_actions);
  }

  // construct an mCRL2 architecture diagram specification
  wxString spec;
  spec = _T("proc ") + diagram_name + _T(" =\n");
  // append visibility and communication information
  spec += _T("     ");
  if (p_renameds.GetCount() > 0)
  {
    spec += _T("rename({");
    int unique_rename_counter = 0;
    for (unsigned int i=0; i<p_renameds.GetCount(); ++i)
    {
      bool found = false;
      for (unsigned int j=i+1; j<p_renameds.GetCount(); ++j)
      {
        if (p_renameds[i].m_old_name == p_renameds[j].m_old_name)
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        if (unique_rename_counter > 0)
        {
          spec += _T(", ");
        }
        spec += p_renameds[i].m_old_name + _T(" -> ") + p_renameds[i].m_new.get_name();
        ++unique_rename_counter;
      }
    }
    spec += _T("}, ");
  }
  if (hidden_comms.GetCount() > 0)
  {
    spec += _T("hide({");
    for (unsigned int i=0; i<hidden_comms.GetCount(); ++i)
    {
      if (i > 0)
      {
        spec += _T(", ");
      }
      spec += hidden_comms[i].m_name;
    }
    spec += _T("}, ");
  }
  if (blocked_comms.GetCount() > 0)
  {
    spec += _T("block({");
    for (unsigned int i=0; i<blocked_comms.GetCount(); ++i)
    {
      if (i > 0)
      {
        spec += _T(", ");
      }
      spec += blocked_comms[i].m_name;
    }
    spec += _T("}, ");
  }
  if (p_channel_comms.GetCount() > 0)
  {
    spec += _T("block({");
    int unique_channel_counter = 0;
    for (unsigned int i=0; i<p_channel_comms.GetCount(); ++i)
    {
      for (unsigned int j=0; j<p_channel_comms[i].m_channels.GetCount(); ++j)
      {
        bool found = false;
        for (unsigned int k=j+1; k<p_channel_comms[i].m_channels.GetCount(); ++k)
        {
          if (p_channel_comms[i].m_channels[j].m_channel_id == p_channel_comms[i].m_channels[k].m_channel_id)
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          if (unique_channel_counter > 0)
          {
            spec += _T(", ");
          }
          spec += p_channel_comms[i].m_channels[j].m_channel.get_name() + _T("_") + p_channel_comms[i].m_channels[j].m_channel_id;
          ++unique_channel_counter;
        }
      }
    }
    spec += _T("}, ");
  }
  if (p_channel_comms.GetCount() > 0)
  {
    spec += _T("comm({");
    for (unsigned int i=0; i<p_channel_comms.GetCount(); ++i)
    {
      if (i > 0)
      {
        spec += _T(", ");
      }

      int unique_channel_counter = 0;
      for (unsigned int j=0; j<p_channel_comms[i].m_channels.GetCount(); ++j)
      {
        bool found = false;
        for (unsigned int k=j+1; k<p_channel_comms[i].m_channels.GetCount(); ++k)
        {
          if (p_channel_comms[i].m_channels[j].m_channel_id == p_channel_comms[i].m_channels[k].m_channel_id)
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          if (unique_channel_counter > 0)
          {
            spec += _T("|");
          }
          spec += p_channel_comms[i].m_channels[j].m_channel.get_name() + _T("_") + p_channel_comms[i].m_channels[j].m_channel_id;
          ++unique_channel_counter;
        }
      }
      spec += _T(" -> ");
      spec += p_channel_comms[i].m_name;
    }
    spec += _T("}, ");
  }

  // append references
  int ref_counter = 0;
  for (unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if (p_refs[i].m_reference != diagram_name)
    {
      if (ref_counter > 0)
      {
        spec += _T("     || ");
      }
      else
      {
        spec += _T("\n        ");
      }

      if (p_refs[i].m_renamed.GetCount() > 0)
      {
        spec += _T("rename({");
        int unique_renamed_counter = 0;
        for (unsigned int j=0; j<p_refs[i].m_renamed.GetCount(); ++j)
        {
          bool found = false;
          for (unsigned int k=j+1; k<p_refs[i].m_renamed.GetCount(); ++k)
          {
            if (p_refs[i].m_renamed[j].m_channel.get_name() == p_refs[i].m_renamed[k].m_channel.get_name())
            {
              if (p_refs[i].m_renamed[j].m_channel_id == p_refs[i].m_renamed[k].m_channel_id)
              {
                found = true;
                break;
              }
            }
          }
          if (!found)
          {
            if (unique_renamed_counter > 0)
            {
              spec += _T(", ");
            }

            spec += p_refs[i].m_renamed[j].m_channel.get_name();
            spec += _T(" -> ");
            spec += p_refs[i].m_renamed[j].m_channel.get_name() + _T("_") + p_refs[i].m_renamed[j].m_channel_id;
            ++unique_renamed_counter;
          }
        }
        spec += _T("}, ");
      }
      if (p_refs[i].m_hidden.GetCount() > 0)
      {
        spec += _T("hide({");
        int unique_hide_counter = 0;
        for (unsigned int j=0; j<p_refs[i].m_hidden.GetCount(); ++j)
        {
          bool found = false;
          for (unsigned int k=j+1; k<p_refs[i].m_hidden.GetCount(); ++k)
          {
            if (p_refs[i].m_hidden[j] == p_refs[i].m_hidden[k])
            {
              found = true;
              break;
            }
          }
          if (!found)
          {
            if (unique_hide_counter > 0)
            {
              spec += _T(", ");
            }
            spec += p_refs[i].m_hidden[j];
            ++unique_hide_counter;
          }
        }
        spec += _T("}, ");
      }
      if (p_refs[i].m_blockeds.GetCount() > 0)
      {
        spec += _T("block({");
        for (unsigned int j=0; j<p_refs[i].m_blockeds.GetCount(); ++j)
        {
          if (j > 0)
          {
            spec += _T(", ");
          }
          spec += p_refs[i].m_blockeds[j];
        }
        spec += _T("}, ");
      }

      // append reference specification
      spec += p_refs[i].m_reference;

      // append optional parameter initialisation
      if (p_refs[i].m_is_process_reference)
      {
        list_of_varupdate inits = get_process_reference_initialisation(p_doc_root, diagram, p_refs[i].m_reference_id, p_refs[i].m_reference, datatype_spec);

        if (inits.GetCount() > 0)
        {
          spec += _T("(");
          for (unsigned int j=0; j<inits.GetCount(); ++j)
          {
            if (j > 0)
            {
              spec += _T(", ");
            }
            spec += inits[j].get_rhs();
          }
          spec += _T(")");
        }
      }

      // append closing brackets
      if (p_refs[i].m_blockeds.GetCount() > 0)
      {
        spec += _T(")");
      }
      if (p_refs[i].m_hidden.GetCount() > 0)
      {
        spec += _T(")");
      }
      if (p_refs[i].m_renamed.GetCount() > 0)
      {
        spec += _T(")");
      }
      spec += _T("\n");

      ++ref_counter;
    }
  }

  // append closing brackets
  spec += _T("     ");
  if (p_channel_comms.GetCount() > 0)
  {
    spec += _T(")");
  }
  if (p_channel_comms.GetCount() > 0)
  {
    spec += _T(")");
  }
  if (blocked_comms.GetCount() > 0)
  {
    spec += _T(")");
  }
  if (hidden_comms.GetCount() > 0)
  {
    spec += _T(")");
  }
  if (p_renameds.GetCount() > 0)
  {
    spec += _T(")");
  }
  spec += _T(";");

  if (p_verbose)
  {
    cerr << "Process specification of " << diagram_name.ToAscii() << ":" << endl;
    cerr << spec.ToAscii() << endl;
  }

  p_spec.Add(spec);
  return;
}

void grape::mcrl2gen::process_diagram_mcrl2(wxXmlNode* p_doc_root, wxString& p_diagram_id, wxArrayString& p_sort_expressions, list_of_action& p_actions, wxArrayString& p_internal_specs, wxArrayString& p_specs, ATermAppl& datatype_spec, bool p_verbose)
{
  // initialize variables
  wxXmlNode* diagram = get_diagram(p_doc_root, p_diagram_id);
  wxString diagram_name = get_child_value(diagram, _T("name"));
  if (p_verbose)
  {
    cerr << "Exporting process diagram " << diagram_name.ToAscii() << "." << endl;
  }

  // get diagrams preamble
  list_of_decl preamble_parameter_decls;
  list_of_decl_init preamble_local_var_decls;
  validate_preamble(diagram, preamble_parameter_decls, preamble_local_var_decls, datatype_spec);
  if (p_verbose)
  {
    cerr << "Preamble:" << endl;
    for (unsigned int i=0; i<preamble_parameter_decls.GetCount(); ++i)
    {
      cerr << "  " << preamble_parameter_decls[i].get_name().ToAscii() << ": "
           << preamble_parameter_decls[i].get_type().ToAscii() << endl;
    }
    for (unsigned int i=0; i<preamble_local_var_decls.GetCount(); ++i)
    {
      cerr << " " << preamble_local_var_decls[i].get_name().ToAscii() << ": "
           << preamble_local_var_decls[i].get_type().ToAscii() << " = "
           << preamble_local_var_decls[i].get_value().ToAscii() << endl;
    }
  }

  // determine occuring actions
  list_of_action acts = get_process_actions(p_doc_root, p_diagram_id, datatype_spec);
  if (p_verbose)
  {
    cerr << "Actions:" << endl;
    for (unsigned int i=0; i<acts.GetCount(); ++i)
    {
      cerr << " " << acts[i].get_name().ToAscii();
      wxString action_text = wxEmptyString;
      for (unsigned int j=0; j<acts[i].get_parameters().GetCount(); ++j)
      {
        if (!acts[i].get_parameters()[j].get_type().IsEmpty())
        {
          action_text += acts[i].get_parameters()[j].get_type();
          if (j != acts[i].get_parameters().GetCount()-1)
          {
            action_text += _T(" # ");
          }
        }
      }
      if (!action_text.IsEmpty())
      {
        cerr << ": " << action_text.ToAscii();
      }
      cerr << endl;
    }
  }

  // add new unique actions to p_actions
  compact_list_action(acts, p_actions);

  // construct state sort expression
  wxString diagram_sort = process_diagram_mcrl2_sort(diagram);
  if (p_verbose)
  {
    cerr << "State sort expression:" << endl << " "
         << diagram_sort.ToAscii() << endl;
  }
  p_sort_expressions.Add(diagram_sort);

  // construct internal process specification
  wxString spec_internal = process_diagram_mcrl2_internal_proc(p_doc_root, diagram, preamble_parameter_decls, preamble_local_var_decls, datatype_spec);
  if (p_verbose)
  {
    cerr << "Internal process specification:" << endl << " "
         << spec_internal.ToAscii() << endl;
  }
  p_internal_specs.Add(spec_internal);

  // construct process specification
  wxString spec = process_diagram_mcrl2_proc(diagram, preamble_parameter_decls, preamble_local_var_decls);
  if (p_verbose)
  {
    cerr << "Process specification:" << endl << " "
         << spec.ToAscii() << endl;
  }
  p_specs.Add(spec);
}

wxString grape::mcrl2gen::process_diagram_mcrl2_sort(wxXmlNode* p_process_diagram)
{
  wxArrayString struct_names;
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));
  wxXmlNode* objects = get_child(p_process_diagram, _T("objectlist"));
  for (wxXmlNode* child = objects->GetChildren(); child != 0; child = child->GetNext())
  {
    if (child->GetName() == _T("statelist"))
    {
      // add names of all states in this <statelist> to struct_names
      for (wxXmlNode* child_state = child->GetChildren(); child_state != 0; child_state = child_state->GetNext())
      {
        wxString child_state_name = get_child_value(child_state, _T("name"));
        wxString child_state_id = get_child_value(child_state, _T("id"));
        struct_names.Add(child_state_name + _T("_") + child_state_id);
      }
    }
    else if (child->GetName() == _T("referencestatelist"))
    {
      // add names of all reference states in this <referencestatelist> to struct_names
      for (wxXmlNode* child_refstate = child->GetChildren(); child_refstate != 0; child_refstate = child_refstate->GetNext())
      {
        wxString child_refstate_name = get_child_value(child_refstate, _T("name"));
        wxString child_refstate_id = get_child_value(child_refstate, _T("id"));
        wxString ref_name = child_refstate_name + _T("_") + child_refstate_id;
        struct_names.Add(_T("S") + ref_name + _T("_in"));
        struct_names.Add(_T("S") + ref_name + _T("_out"));
      }
    }
  }

  // structnames now contains the names of all states in this diagram, along with 2 names of all reference states in this diagram
  wxString sort_struct_expr = _T("sort ") + diagram_name + _T("_state = struct ");
  for (unsigned int i=0; i<struct_names.GetCount(); ++i)
  {
    if (i > 0)
    {
      sort_struct_expr += _T("|");
    }
    sort_struct_expr += struct_names[i];
  }
  sort_struct_expr += _T(";\n");

  return sort_struct_expr;
}

wxString grape::mcrl2gen::process_diagram_mcrl2_internal_proc(wxXmlNode* p_doc_root, wxXmlNode* p_process_diagram, list_of_decl& p_preamble_parameter_decls, list_of_decl_init& p_preamble_local_var_decls, ATermAppl& datatype_spec)
{
  // construct the first part of the declaration
  wxString decl_internal = _T("proc ");

  // <processdiagram> node
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));
  decl_internal += diagram_name + _T("_internal(");

  // add state parameter to declaration
  decl_internal += _T("s: ") + diagram_name + _T("_state");
  // add preamble parameters and local variable declarations
  for (unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
  {
    decl_internal += _T(", ");
    decl_internal += p_preamble_parameter_decls[i].get_name();
    decl_internal += _T(": ");
    decl_internal += p_preamble_parameter_decls[i].get_type();
  }
  // add preamble local variables to declaration
  for (unsigned int i=0; i<p_preamble_local_var_decls.GetCount(); ++i)
  {
    decl_internal += _T(", ");
    decl_internal += p_preamble_local_var_decls[i].get_name();
    decl_internal += _T(": ");
    decl_internal += p_preamble_local_var_decls[i].get_type();
  }
  decl_internal += _T(") = ");

  // construct internal specification
  bool first_line = true;
  wxXmlNode* objects = get_child(p_process_diagram, _T("objectlist"));
  wxString decl_trans = wxEmptyString;
  for (wxXmlNode* child = objects->GetChildren(); child != 0; child = child->GetNext())
  {
    // process objectlist children
    if (child->GetName() == _T("terminatingtransitionlist") || child->GetName() == _T("nonterminatingtransitionlist"))
    {
      // process transition labels of all (non)-terminating transitions to actions
      for (wxXmlNode* child_trans = child->GetChildren(); child_trans != 0; child_trans = child_trans->GetNext())
      {
        decl_trans += transition_mcrl2(p_process_diagram, child_trans, !first_line, diagram_name, p_preamble_parameter_decls, p_preamble_local_var_decls, datatype_spec);
        first_line = false;
      }
    }
    if (child->GetName() == _T("referencestatelist"))
    {
      // parse all reference states in this <referencestatelist>
      for (wxXmlNode* child_ref = child->GetChildren(); child_ref != 0; child_ref = child_ref->GetNext())
      {
        decl_trans += transition_reference_mcrl2(p_doc_root, child_ref, !first_line, p_process_diagram, p_preamble_parameter_decls, p_preamble_local_var_decls, datatype_spec);
        first_line = false;
      }
    }
  }
  if (decl_trans.IsEmpty())
  {
    decl_internal += _T("delta");
  }
  else
  {
    decl_internal += decl_trans;
  }
  decl_internal += _T(";\n");
  return decl_internal;
}

wxString grape::mcrl2gen::process_diagram_mcrl2_proc(wxXmlNode* p_process_diagram, list_of_decl& p_preamble_parameter_decls, list_of_decl_init& p_preamble_local_var_decls)
{
  // construct declaration
  wxString proc_decl = _T("proc ");
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));
  proc_decl += diagram_name;
  if (p_preamble_parameter_decls.GetCount() > 0)
  {
    proc_decl += _T("(");
    for (unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
    {
      if (i > 0)
      {
        proc_decl += _T(", ");
      }
      proc_decl += p_preamble_parameter_decls[i].get_name();
      proc_decl += _T(": ");
      proc_decl += p_preamble_parameter_decls[i].get_type();
    }
    proc_decl += _T(")");
  }
  proc_decl += _T(" =\n");
  proc_decl += _T("     ");
  proc_decl += diagram_name + _T("_internal(");

  // extract initial designator
  wxString designator = initial_designator_mcrl2(p_process_diagram);
  proc_decl += designator;
  for (unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
  {
    proc_decl += _T(", ");
    proc_decl += p_preamble_parameter_decls[i].get_name();
  }
  for (unsigned int i=0; i<p_preamble_local_var_decls.GetCount(); ++i)
  {
    proc_decl += _T(", ");
    proc_decl += p_preamble_local_var_decls[i].get_value();
  }
  proc_decl += _T(");");

  return proc_decl;
}

wxString grape::mcrl2gen::transition_mcrl2(wxXmlNode* p_process_diagram, wxXmlNode* p_transition, bool p_alternative, wxString& p_diagram_name, list_of_decl& p_preamble_parameter_decls, list_of_decl_init& p_preamble_local_var_decls, ATermAppl& datatype_spec)
{
  // initialize variables
  wxString decl_transition;
  wxString state_from, state_to, id_from, id_to;
  bool is_ref_from, is_ref_to;

  // determine begin- and endstate identifiers of this transition
  id_from = get_child_value(p_transition, _T("from"));
  id_to = get_child_value(p_transition, _T("to"));
  // determine begin- and endstate names of this transition
  state_from = get_state_name(p_process_diagram, id_from, is_ref_from);
  state_to = get_state_name(p_process_diagram, id_to, is_ref_to);

  // load label
  label trans_label;
  if (!validate_transition_label(p_transition, p_preamble_parameter_decls, p_preamble_local_var_decls, trans_label, p_diagram_name, datatype_spec))
  {
    return wxEmptyString;
  }
  wxString variables = trans_label.get_declarations_text();
  wxString condition = trans_label.get_condition();
  wxString actions = trans_label.get_actions_text();
  if (actions.IsEmpty())
  {
    // an empty multiaction is translated as a tau
    actions = _T("tau");
  }
  wxString timestamp = trans_label.get_timestamp();
  list_of_varupdate updates = trans_label.get_variable_updates();

  decl_transition += _T("\n");

  if (p_alternative)
  {
    decl_transition += _T("     + ");
  }
  else
  {
    decl_transition += _T("       ");
  }
  decl_transition += _T("(s == ");
  if (is_ref_from)
  {
    decl_transition += _T("S") + state_from + _T("_") + id_from + _T("_out) -> ");
  }
  else
  {
    decl_transition += state_from + _T("_") + id_from + _T(") -> ");
  }

  // add optional variables
  if (!variables.IsEmpty())
  {
    decl_transition += _T("sum ") + variables + _T(".");
  }
  // add optional condition
  if (!condition.IsEmpty())
  {
    decl_transition += _T("(") + condition + _T(") -> ");
  }

  // add multiaction
  if (!actions.IsEmpty())
  {
    decl_transition += actions;
  }

  // add optional timestamp
  if (!timestamp.IsEmpty())
  {
    decl_transition += _T("@") + timestamp;
  }

  if (!state_to.IsEmpty())
  {
    decl_transition += _T(".");
    // add end state with optional variable updates
    decl_transition += p_diagram_name + _T("_internal(");
    if (is_ref_to)
    {
      decl_transition += _T("S") + state_to + _T("_") + id_to + _T("_in");
    }
    else
    {
      decl_transition += state_to + _T("_") + id_to;
    }

    // process parameters, local variables and variable updates
    for (unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
    {
      wxString variable_update = _T(", ") + p_preamble_parameter_decls[i].get_name();
      // check for updates
      for (unsigned int j=0; j<updates.GetCount(); ++j)
      {
        if (p_preamble_parameter_decls[i].get_name() == updates[j].get_lhs())
        {
          variable_update = _T(", ") + updates[j].get_rhs();
          break;
        }
      }
      decl_transition += variable_update;
    }
    for (unsigned int i=0; i<p_preamble_local_var_decls.GetCount(); ++i)
    {
      wxString variable_update = _T(", ") + p_preamble_local_var_decls[i].get_name();
      // check for updates
      for (unsigned int j=0; j<updates.GetCount(); ++j)
      {
        if (p_preamble_local_var_decls[i].get_name() == updates[j].get_lhs())
        {
          variable_update = _T(", ") + updates[j].get_rhs();
          break;
        }
      }
      decl_transition += variable_update;
    }
    decl_transition += _T(")");
  }
  return decl_transition;
}

wxString grape::mcrl2gen::transition_reference_mcrl2(wxXmlNode* p_doc_root, wxXmlNode* p_reference_state, bool p_alternative, wxXmlNode* p_diagram, list_of_decl& p_preamble_parameter_decls, list_of_decl_init& p_preamble_local_var_decls, ATermAppl& datatype_spec)
{
  // initialize variables
  wxString decl_transition_reference;
  wxString ref_name = get_child_value(p_reference_state, _T("name"));
  wxString ref_id = get_child_value(p_reference_state, _T("id"));


  wxString p_diagram_name = get_child_value(p_diagram, _T("name"));
  list_of_varupdate ref_inits;
  if (!validate_reference_parameters(p_doc_root, p_reference_state, p_diagram, ref_inits, datatype_spec))
  {
    return wxEmptyString;
  }

  decl_transition_reference += _T("\n");

  if (p_alternative)
  {
    decl_transition_reference += _T("     + ");
  }
  else
  {
    decl_transition_reference += _T("       ");
  }
  decl_transition_reference += _T("(s == ");
  decl_transition_reference += _T("S") + ref_name + _T("_") + ref_id;
  decl_transition_reference += _T("_in) -> ") + ref_name;
  if (ref_inits.GetCount() > 0)
  {
    decl_transition_reference += _T("(");
    for (unsigned int i=0; i<ref_inits.GetCount(); ++i)
    {
      if (i > 0)
      {
        decl_transition_reference += _T(", ");
      }
      decl_transition_reference += ref_inits[i].get_rhs();
    }
    decl_transition_reference += _T(")");
  }
  decl_transition_reference += _T(".") + p_diagram_name + _T("_internal(") + _T("S") + ref_name + _T("_") + ref_id + _T("_out");
  if (p_preamble_parameter_decls.GetCount() > 0)
  {
    for (unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
    {
      decl_transition_reference += _T(", ");
      decl_transition_reference += p_preamble_parameter_decls[i].get_name();
    }
  }
  if (p_preamble_local_var_decls.GetCount() > 0)
  {
    for (unsigned int i=0; i<p_preamble_local_var_decls.GetCount(); ++i)
    {
      decl_transition_reference += _T(", ");
      decl_transition_reference += p_preamble_local_var_decls[i].get_name();
    }
  }
  decl_transition_reference += _T(")");

  return decl_transition_reference;
}

wxString grape::mcrl2gen::initial_designator_mcrl2(wxXmlNode* p_process_diagram)
{
  // initialize variables
  wxXmlNode* obj_list = get_child(p_process_diagram, _T("objectlist"));
  wxXmlNode* des_list = get_child(obj_list, _T("initialdesignatorlist"));
  wxXmlNode* des = get_child(des_list, _T("initialdesignator"));
  wxString des_id = get_child_value(des, _T("propertyof"));

  // make mcrl2 of initial designator
  bool is_ref;
  wxString des_name = get_state_name(p_process_diagram, des_id, is_ref) + _T("_") + des_id;
  if (is_ref)
  {
    des_name = _T("S") + des_name + _T("_in");
  }
  return des_name;
}

wxString grape::mcrl2gen::process_diagram_mcrl2_init(wxXmlNode* /* p_doc_root */,
    wxString& p_diagram_name,
    list_of_decl_init& p_parameter_init)
{
  wxString init_decl = wxEmptyString;
  init_decl += _T("init ");
  init_decl += p_diagram_name;
  if (p_parameter_init.GetCount() > 0)
  {
    init_decl += _T("(");

    // sort parameter initialisation and print to init declaration
    list_of_decl_init decls = p_parameter_init;
    for (unsigned int i=0; i<decls.GetCount(); ++i)
    {
      if (i > 0)
      {
        init_decl += _T(", ");
      }
      init_decl += decls[i].get_value();
    }
    init_decl += _T(")");
  }
  init_decl += _T(";");
  return init_decl;
}

void grape::mcrl2gen::architecture_diagram_mcrl2_actions(wxXmlNode* p_doc_root, wxXmlNode* p_architecture_diagram, arr_action_reference& p_possibles, ATermAppl& datatype_spec)
{
  // initialize variables
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
  wxString reference_id = get_child_value(p_architecture_diagram, _T("id"));

  // go through all references and determine possible actions
  wxXmlNode* objects = get_child(p_architecture_diagram, _T("objectlist"));
  wxXmlNode* archs = get_child(objects, _T("architecturereferencelist"));
  wxXmlNode* procs = get_child(objects, _T("processreferencelist"));

  // process process references
  for (wxXmlNode* proc = procs->GetChildren(); proc != 0; proc = proc->GetNext())
  {
    action_reference aref;

    wxString ref_id = get_child_value(proc, _T("propertyof"));
    wxXmlNode* ref_diag = get_diagram(p_doc_root, ref_id);
    wxString ref_name = get_child_value(ref_diag, _T("name"));

    // update vars for referenced process diagram
    wxString reference_id = get_child_value(proc, _T("id"));
    aref.m_reference = ref_name;
    aref.m_reference_id = reference_id;
    aref.m_diagram_id = ref_id;
    aref.m_is_process_reference = true;

    // infer possible actions for this reference
    aref.m_actions = get_process_actions(p_doc_root, ref_id, datatype_spec);
    p_possibles.Add(aref);
  }

  // process architecture references
  for (wxXmlNode* arch = archs->GetChildren(); arch != 0; arch = arch->GetNext())
  {
    action_reference aref;

    wxString ref_id = get_child_value(arch, _T("propertyof"));
    wxXmlNode* arch_diagram = get_diagram(p_doc_root, ref_id);
    wxString ref_name = get_child_value(arch_diagram, _T("name"));

    // update vars for referenced architecture diagram
    wxString reference_id = get_child_value(arch, _T("id"));
    aref.m_reference = ref_name;
    aref.m_reference_id = reference_id;
    aref.m_diagram_id = ref_id;
    aref.m_is_process_reference = false;

    // infer possible actions for this reference
    aref.m_actions = get_architecture_visibles(p_doc_root, ref_id, datatype_spec);
    p_possibles.Add(aref);
  }
  action_reference aref;

  // update vars for architecture diagram
  aref.m_reference = diagram_name;
  aref.m_reference_id = _T("-1");
  aref.m_diagram_id = reference_id;
  aref.m_is_process_reference = false;

  // infer possible actions for this diagram
  aref.m_actions = get_architecture_visibles(p_doc_root, reference_id, datatype_spec);
  p_possibles.Add(aref);
}

wxString grape::mcrl2gen::architecture_diagram_mcrl2_init(wxString& p_diagram_name)
{
  wxString init = _T("init ") + p_diagram_name + _T(";\n");
  return init;
}

void grape::mcrl2gen::convert_spaces_node(wxXmlNode* p_node)
{
  wxString name = p_node->GetNodeContent();
  name.Replace(_T(" "), _T("_"));

  wxXmlNode* val = p_node->GetChildren();
  if (val)
  {
    val->SetContent(name);
  }
}

void grape::mcrl2gen::convert_spaces(wxXmlDocument& p_spec)
{
  wxXmlNode* root_node = p_spec.GetRoot();

  wxXmlNode* procs = get_child(root_node, _T("processdiagramlist"));
  if (procs != 0)
  {
    for (wxXmlNode* diagram = procs->GetChildren(); diagram != 0; diagram = diagram->GetNext())
    {
      convert_spaces_node(get_child(diagram, _T("name")));

      wxXmlNode* objects = get_child(diagram, _T("objectlist"));
      if (objects != 0)
      {
        wxXmlNode* ref_states = get_child(objects, _T("referencestatelist"));
        if (ref_states != 0)
        {
          for (wxXmlNode* ref_state = ref_states->GetChildren(); ref_state != 0; ref_state = ref_state->GetNext())
          {
            convert_spaces_node(get_child(ref_state, _T("name")));
          }
        }
        wxXmlNode* states = get_child(objects, _T("statelist"));
        if (states != 0)
        {
          for (wxXmlNode* state = states->GetChildren(); state != 0; state = state->GetNext())
          {
            convert_spaces_node(get_child(state, _T("name")));
          }
        }
      }
    }
  }

  wxXmlNode* archs = get_child(root_node, _T("architecturediagramlist"));
  if (archs != 0)
  {
    for (wxXmlNode* diagram = archs->GetChildren(); diagram != 0; diagram = diagram->GetNext())
    {
      convert_spaces_node(get_child(diagram, _T("name")));
    }
  }
}
