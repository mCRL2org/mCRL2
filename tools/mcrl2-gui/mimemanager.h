/*
 * mimemanager.h
 *
 *  Created on: Jul 5, 2010
 *      Author: fstapper
 */

#ifndef MCRL2_GUI_MIMEMANAGER_H_
#define MCRL2_GUI_MIMEMANAGER_H_

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/mimetype.h>
#include <wx/config.h>


class MimeManager
{
  public:
    MimeManager()
    {

      wxString str;
      wxConfig *config = new wxConfig(wxT("mcrl2_gui"));
      if (config->Read(wxT("mimes"), &str)) {
        wxStringToMapping(str);
      }
      delete config;

      wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("txt"));
      if (ft)
      {
        ;
        wxString cmd;
        ft->GetOpenCommand(&cmd,  wxFileType::MessageParameters( wxT("\%s"), wxT("text/plain")));

        if (!cmd.empty())
        {
          if (m_edittool_mapping.find(wxT("mcrl2")) == m_edittool_mapping.end())
            m_edittool_mapping[wxT("mcrl2")] = cmd;
          if (m_edittool_mapping.find(wxT("mcf")) == m_edittool_mapping.end())
            m_edittool_mapping[wxT("mcf")] = cmd;
          if (m_edittool_mapping.find(wxT("aut")) == m_edittool_mapping.end())
            m_edittool_mapping[wxT("aut")] = cmd;
        }
      }
    }
    ;

    wxString
    getCommandForExtention(wxString filenameWithExtension)
    {
      /* Get extension */
      wxString ext = filenameWithExtension.AfterLast(_T('.'));

      /* Get self-defined mapping */
      if (m_edittool_mapping.find(ext) != m_edittool_mapping.end())
      {
        //substitute %s by filenameWithExtention
        wxString cmd = m_edittool_mapping.find(ext)->second;
        cmd.Replace(wxT( "\%s" ), filenameWithExtension );
        return cmd;
      }

      /* System defined */
      wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
      if (ft)
      {
        wxString cmd;
        bool ok = ft->GetOpenCommand(&cmd, wxFileType::MessageParameters(
            filenameWithExtension));
        if (ok)
        {
          delete ft;
          return cmd;
        }
      }

      /* Return empty string */
      return wxString();
    }

    wxString
    getExtention(wxString filenameWithExtention)
    {
      /* Get extension */
      return filenameWithExtention.AfterLast(_T('.'));
    }

    map<wxString,wxString> getExtensionCommandMapping(){
      return m_edittool_mapping;
    }

    void removeExtensionMapping(wxString str ){
      m_edittool_mapping.erase(str);

      wxConfig *config = new wxConfig(wxT("mcrl2_gui"));
      config->Write( wxT("mimes"), MappingTowxString() );
      delete config;
    }

    void addExtensionMapping(wxString ext, wxString cmd){
      m_edittool_mapping[ext]=cmd;

      wxString str;
      wxConfig *config = new wxConfig(wxT("mcrl2_gui"));
      config->Write( wxT("mimes"), MappingTowxString() );
      delete config;
    }

private:
    wxString
    MappingTowxString()
    {
      wxString s;
      for (map<wxString, wxString>::iterator it = m_edittool_mapping.begin(); it
          != m_edittool_mapping.end(); ++it)
      {
        s.Append( wxT("{") + it->first + wxT("|") + it->second + wxT("}"));
      }
      return s;
    }

    void
    wxStringToMapping(wxString str)
    {

      str = str.AfterFirst(wxT('{'));
      while (!str.empty())
      {
        wxString item = str.BeforeFirst(wxT('}'));

        wxString ext = item.BeforeFirst(wxT('|'));
        wxString cmd = item.AfterFirst(wxT('|'));

        m_edittool_mapping[ext] = cmd;

        str = str.AfterFirst(wxT('{'));
      }

      return;
    }

     map<wxString,wxString> m_edittool_mapping;

  };

#endif /* MCRL2_GUI_MIMEMANAGER_H_ */
