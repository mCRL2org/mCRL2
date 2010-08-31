/*
 * projectcontrol.h
 *
 *  Created on: Jun 3, 2010
 *      Author: fstapper
 */

#ifndef MCRL2_GUI_PROJECTCONTROL_H_
#define MCRL2_GUI_PROJECTCONTROL_H_

#include <wx/dirctrl.h>
#include "initialization.h"
#include <wx/mimetype.h>
#include <wx/utils.h>
#include <mcrl2-process.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/mdi.h>
#include <wx/notebook.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/aui/auibook.h>
#include <gui/tooloutputlistbox.h>
#include <gui/configpanel.h>
#include <iostream>
#include <mimemanager.h>
#include <wx/filename.h>

//To store configurations
#include <wx/config.h>
//#include <wx/confbase.h> //(base config class)
//#include <wx/fileconf.h> //(wxFileConfig class)
//#include <wx/msw/regconf.h> //(wxRegConfig class)

#define ID_GDC		2000
#define ID_OPEN    2001
#define ID_EDIT		2002
#define ID_RENAME  2003
#define ID_DETAILS	2004
#define ID_DELETE	 2005
#define ID_NEW_FILE 2006
#define ID_NEW_DIR 2007
#define ID_REFRESH 2008
#define ID_EXPAND 2009
#define ID_COPY_FILE 2010

using namespace std;

class GenericDirCtrl: public wxGenericDirCtrl {
public:

	std::vector<Tool> m_tool_catalog;

	multimap<string, string> m_extention_tool_mapping;

	OutPutTextCtrl *m_listbox_output;

	wxAuiNotebook *m_notebookpanel;

	vector<wxRadioBox*> m_radiobox_ptrs;
	vector<wxCheckBox*> m_checkbox_ptrs;

	GenericDirCtrl(wxWindow *parent, const std::vector<Tool>& tool_catalog,
			multimap<string, string> extention_tool_mapping, OutPutTextCtrl *output,
			wxAuiNotebook *notebookpanel) :
		wxGenericDirCtrl(parent, ID_GDC, wxDirDialogDefaultFolderStr, wxPoint(-1,
				-1), wxSize(-1, -1), wxDIRCTRL_EDIT_LABELS | wxDIRCTRL_3D_INTERNAL
				| wxSUNKEN_BORDER)
	{
		m_parent = parent;
		m_tool_catalog = tool_catalog;
		m_listbox_output = output;
		m_notebookpanel = notebookpanel;
		m_extention_tool_mapping = extention_tool_mapping;
		refresh_dir = new wxTimer(this, ID_TIMER);
		refresh_dir->Start(500);
		this->Fit();
		this->Layout();

		wxString str;
		wxConfig *config = new wxConfig(wxT("mcrl2_gui"));
		if (config->Read(wxT("LastSelectedFile"), &str)) {
			this->SetPath(str);
		}
		delete config;

	}

	~GenericDirCtrl() {
		wxConfig *config = new wxConfig(wxT("mcrl2_gui"));
		config->Write(wxT("LastSelectedFile"), this->GetPath());
		delete config;
	}

	void OnPopupClick(wxCommandEvent &evt) {

		if (evt.GetId() < ID_GDC) {

			Tool tool = m_tool_catalog[evt.GetId()];

			FileIO fileIO;
			fileIO.input_file = this->GetPath().mb_str(wxConvUTF8);

			ConfigPanel *ncp = new ConfigPanel(m_notebookpanel, wxID_ANY,
					m_tool_catalog[evt.GetId()], m_listbox_output, fileIO);

			ncp->Layout();

			/* Todo: Fix tooltip such that it appears on top the ctrl panels */
			ncp->SetToolTip( this->GetPath());

			m_notebookpanel->AddPage(ncp, wxString( m_tool_catalog[evt.GetId()].m_name.c_str(), wxConvUTF8), true);

		} else {

			wxTreeItemId id, pid;
			wxString s;
			string new_file = "new_file";
			wxString new_dir = wxT("new_dir");
			string filepath;

			switch (evt.GetId()) {
			case ID_NEW_FILE:
				//Parent ID
				CreateNewFile();
				break;
			case ID_NEW_DIR:

				if (wxDir::Exists(this->GetPath())) {
					s = this->GetPath() + wxFileName::GetPathSeparator() ;
				}

				if (wxFile::Exists(this->GetPath())){
			    wxString name;
			    wxString ext;
				wxFileName::SplitPath(this->GetPath(), &s, &name, &ext);
				s = s + wxFileName::GetPathSeparator();
				}

				//Generate unique name directory name
				s = s + new_dir;
				if (wxDir::Exists(s)) {
					int i = 2;
					while (wxDir::Exists(wxString::Format(s + wxT("(%i)"), i))) {
						++i;
					}
					s = wxString::Format(s + wxT("(%i)"), i);
				}

				//Create new dir
				if (!wxMkdir(s)) {
					wxLogMessage(wxT("Cannot create the following directory: '") + s
							+ wxT("'."));
				} else {
					//Collapse tree
		            this->Refresh();
					//Expand tree to new dir
					this->ExpandPath(s);
					//Re-label dir
					this->GetTreeCtrl()->EditLabel(this->GetTreeCtrl()->GetSelection());
				}

				break;
			case ID_EDIT:
        Edit();
				break;
			case ID_DETAILS:
				ShowDetails();
				break;
			case ID_RENAME:
				Rename();
				break;
			case ID_DELETE:
				Delete();
				break;
			case ID_REFRESH:
				Refresh();
				break;
			case ID_EXPAND:
			  this->ExpandPath(this->GetPath());
			  break;
			case ID_COPY_FILE:
			  CopyFile();
			  break;
			}
		}
	}

	void
    ShowDetails()
    {
	    wxNotebookPage *wp = new wxNotebookPage( m_notebookpanel, wxID_ANY );

	    wxGridBagSizer *fgs = new wxGridBagSizer(10, 10);

	    int row = 1;

	  wxString path; 
	  wxString name;
	  wxString ext;

	  wxFileName::SplitPath(this->GetPath(), &path, &name, &ext);

      if (wxFileName::FileExists(this->GetPath()))
        fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("File:")) , wxGBPosition(row,1));

      if (wxFileName::DirExists(this->GetPath()))
        fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("Directory:")) , wxGBPosition(row,1));

	  if (wxFileName::DirExists(this->GetPath()) || wxFileName::FileExists(this->GetPath()))
	  {
        fgs->Add( new wxStaticText(wp, wxID_ANY, name + wxT(".") + ext ) , wxGBPosition(row,2));
	  } else {
		fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("File or directory does not exists")) , wxGBPosition(row,2));
	  }

      ++row;
      fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("Path:")) , wxGBPosition(row,1));
      fgs->Add( new wxStaticText(wp, wxID_ANY, path) , wxGBPosition(row,2));

	  if (wxFileName::FileExists(this->GetPath()))
	  {
		++row;
		fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("Readable:")) , wxGBPosition(row,1));
		fgs->Add( new wxStaticText(wp, wxID_ANY, wxFileName::IsFileReadable( this->GetPath())?wxT("Yes"):wxT("No") ) , wxGBPosition(row,2));

		++row;
        fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("Writable:")) , wxGBPosition(row,1));
        fgs->Add( new wxStaticText(wp, wxID_ANY, wxFileName::IsFileWritable( this->GetPath())?wxT("Yes"):wxT("No") ) , wxGBPosition(row,2));

		++row;
        fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("Executable:")) , wxGBPosition(row,1));
        fgs->Add( new wxStaticText(wp, wxID_ANY, wxFileName::IsFileExecutable( this->GetPath())?wxT("Yes"):wxT("No") ) , wxGBPosition(row,2));

		++row;
	    wxFileName *wf = new wxFileName( this->GetPath() );
        wxDateTime tm = wf->GetModificationTime();
        fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("Date modified")) , wxGBPosition(row,1));
        fgs->Add( new wxStaticText(wp, wxID_ANY, tm.Format() ), wxGBPosition(row,2));

		++row;
		wxString wxs;
        fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("Size On Disk")) , wxGBPosition(row,1));
        wxs << wxFileName::GetHumanReadableSize(  wxULongLong(wxFile(this->GetPath()).Length()) );
		fgs->Add( new wxStaticText(wp, wxID_ANY, wxs ), wxGBPosition(row,2));
	  }

	  if (wxFileName::DirExists(this->GetPath()))
	  {
		++row;
		fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("Readable:")) , wxGBPosition(row,1));
		fgs->Add( new wxStaticText(wp, wxID_ANY, wxFileName::IsDirReadable( this->GetPath())?wxT("Yes"):wxT("No") ) , wxGBPosition(row,2));

		++row;
        fgs->Add( new wxStaticText(wp, wxID_ANY, wxT("Writable:")) , wxGBPosition(row,1));
        fgs->Add( new wxStaticText(wp, wxID_ANY, wxFileName::IsDirWritable( this->GetPath())?wxT("Yes"):wxT("No") ) , wxGBPosition(row,2));
	  }

      wp->SetSizer(fgs);
      wp->Layout();

      m_notebookpanel->AddPage(wp, this->GetPath(), true);
    }

	void OnRightClick(wxTreeEvent& /*evt*/) {
		DisplayMenu();
	}

	void onKeyDown(wxTreeEvent& evt) {
		//std::cout << "Pressed key {" << evt.GetKeyCode() << "}\n";

		switch (evt.GetKeyCode()) {
		case WXK_F2:
			Rename();
			break;
		case WXK_NUMPAD_DELETE:
			Delete();
			break;
		case WXK_DELETE:
			Delete();
			break;
		case WXK_MENU:
			DisplayMenu();
			break;
		case WXK_SPACE:
			DisplayMenu();
			break;
		case 14: //14  == CTRL+n
			CreateNewFile();
			break;
    case WXK_F5:
    	Refresh();
			break;
    case WXK_F6:
      CopyFile();
      break;

		}

		evt.Skip();

	}

	void Edit(){
		if (!this->GetPath().empty()) {

		  MimeManager mm;
		  wxString cmd = mm.getCommandForExtention( this->GetPath() );
		  wxString ext = this->GetPath().AfterLast(_T('.') );

		  if (!cmd.empty())
		  {
        wxExecute(cmd, wxEXEC_ASYNC, NULL);
		  } else {

			#ifdef __linux
            wxLogMessage(_T("No editor defined by UNIX operating system for editing files of extension '%s'. An editor can be set under 'Window -> Preferences'."),
                ext.c_str());
#endif
#ifdef __WINDOWS__
            wxLogMessage(_T("No editor defined by Windows operating system for editing files of extension '%s'. An editor can be set under 'Window -> Preferences'."),
                ext.c_str());
#endif
      }
    }
	}

	void Refresh(){
		//std::cout<< "Refresh Tree" << std::	endl;
		wxString selected_file = this->GetPath();
		this->ReCreateTree();
		this->SetPath(selected_file);
	}

	void Rename() {
		this->GetTreeCtrl()->EditLabel(this->GetTreeCtrl()->GetSelection());
	}

	void CopyFile(){
	  wxFileDialog *fd = new wxFileDialog(this, wxT("Copy file"), ::wxPathOnly(this->GetPath()), ::wxFileNameFromPath(this->GetPath()), wxT("*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT );

	  if( fd->ShowModal() == wxID_OK ){
	    if(::wxCopyFile(this->GetPath(), fd->GetPath())){
	      this->ReCreateTree();
	      this->SetPath(fd->GetPath());
	    } else{
	      wxMessageDialog *dial = new wxMessageDialog(NULL,
	          wxT("Copy operation failed"), wxT("Exclamation"),
	          wxOK | wxICON_EXCLAMATION);
	      dial->ShowModal();
	    }
	  }
	}

	bool
    rmdir(wxString dir)
    {
      wxString path(dir);
      if (path.Last() != wxFILE_SEP_PATH)
        path += wxFILE_SEP_PATH;

      wxDir d(path);

      wxString filename;
      bool cont;

      // first delete sub-directories
      cont = d.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
      while (cont)
      {
        rmdir(path + filename);
        cont = d.GetNext(&filename);
      }

      // delete all files
      cont = d.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_HIDDEN);
      while (cont)
      {
        wxRemoveFile(path + filename);
        cont = d.GetNext(&filename);
      }

      return wxRmdir(dir);
    }

	void
    Delete()
    {

	  bool rm_from_disk = false;

      if (wxFile::Exists(this->GetPath()))
      {
        wxMessageDialog
            *dial =
                new wxMessageDialog(
                    NULL,
                    wxT("This action deletes the selected file from disk.\nAre you sure to continue?"),
                    wxT("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
        if (dial->ShowModal() == wxID_YES)
        {

          if (remove(this->GetPath().mb_str(wxConvUTF8)) != 0)
          {
            wxLogError(wxT("Error deleting file"));
            return;
          }

          rm_from_disk = true;
        };
      }

      if (wxDir::Exists(this->GetPath()))
      {
        wxMessageDialog
            *dial =
                new wxMessageDialog(
                    NULL,
                    wxT("This action recursively deletes the directory from disk.\nAre you sure to continue?"),
                    wxT("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
        if (dial->ShowModal() == wxID_YES)
        {

          if (!rmdir(this->GetPath()))
          {
            wxLogError(wxT("Error deleting directory"));
            return;
          } else {
            rm_from_disk = true;

          }
        }
      };

      if(rm_from_disk)
      {
        /* Remove from tree */
        wxTreeItemId item_for_removal = this->GetTreeCtrl()->GetSelection();

        if (this->GetTreeCtrl()->GetNextSibling(item_for_removal).IsOk())
        {
          this->GetTreeCtrl()->SelectItem(this->GetTreeCtrl()->GetNextSibling(
              item_for_removal));
        }
        else if (this->GetTreeCtrl()->GetPrevSibling(item_for_removal).IsOk())
        {
          this->GetTreeCtrl()->SelectItem(this->GetTreeCtrl()->GetPrevSibling(
              item_for_removal));
        }
        else if (this->GetTreeCtrl()->GetPrevSibling(item_for_removal).IsOk())
        {
          this->GetTreeCtrl()->SelectItem(this->GetTreeCtrl()->GetPrevVisible(
              item_for_removal));
        };

        this->GetTreeCtrl()->Delete(item_for_removal);
      }
    }

	void DisplayMenu() {
		wxMenu mnu;

		wxString ext = this->GetPath().AfterLast(_T('.'));
		std::string sext = (std::string) ext.mb_str(wxConvUTF8);

		wxMenu *reporting = new wxMenu();
		wxMenu *transformation = new wxMenu();

		if (wxFile::Exists(this->GetPath())) {
			mnu.Append(ID_EDIT, wxT("Edit \tCtrl-E"));

			for (vector<Tool>::iterator i = m_tool_catalog.begin(); i
					!= m_tool_catalog.end(); ++i) {
				//Compare if extension occurs as input parameter
				//TODO: take alternative "to-open"-options for extensions into account

				multimap<string, string>::iterator fnd = m_extention_tool_mapping.find(
						sext);
				if (fnd != m_extention_tool_mapping.end()) {
					if (i->m_input_type.compare(fnd->second) == 0) {

						if (i->m_output_type.empty()) {
							reporting->Append(distance(m_tool_catalog.begin(), i), wxString(
									(*i).m_name.c_str(), wxConvUTF8));
						} else {
							transformation->Append(distance(m_tool_catalog.begin(), i),
									wxString((*i).m_name.c_str(), wxConvUTF8));
						}
					}
				}
			}
		}

		if (wxDir::Exists(this->GetPath())){
		  mnu.Append(ID_EXPAND, wxT("Expand/Collapse Folder"));
		  mnu.AppendSeparator();
		}

		if ((transformation->GetMenuItemCount() != 0)
				|| (reporting->GetMenuItemCount() != 0)) {
			mnu.AppendSeparator();
		}

		if (reporting->GetMenuItemCount() != 0) {
			mnu.AppendSubMenu(reporting, wxT("Analysis"));
		}
		if (transformation->GetMenuItemCount() != 0) {
			mnu.AppendSubMenu(transformation, wxT("Transformation"));
		}
		reporting->Connect(wxEVT_COMMAND_MENU_SELECTED,
				(wxObjectEventFunction) &GenericDirCtrl::OnPopupClick, NULL, this);
		transformation->Connect(wxEVT_COMMAND_MENU_SELECTED,
				(wxObjectEventFunction) &GenericDirCtrl::OnPopupClick, NULL, this);

		if ((transformation->GetMenuItemCount() != 0)
				|| (reporting->GetMenuItemCount() != 0)) {
			mnu.AppendSeparator();
		}

		mnu.Append(ID_NEW_FILE, wxT("New File \tCtrl-N"));
		mnu.Append(ID_NEW_DIR, wxT("New Directory"));
		mnu.AppendSeparator();
		mnu.Append(ID_COPY_FILE, wxT("Copy File \tF6"));
		mnu.Append(ID_RENAME, wxT("Rename \tF2"));
		mnu.Append(ID_DELETE, wxT("Delete \tDel"));
		mnu.AppendSeparator();
		mnu.Append(ID_REFRESH, wxT("Refresh \tF5"));
		mnu.Append(ID_DETAILS, wxT("Details"));
		mnu.Connect(wxEVT_COMMAND_MENU_SELECTED,
				(wxObjectEventFunction) &GenericDirCtrl::OnPopupClick, NULL, this);
		PopupMenu(&mnu);
	}

	void CreateNewFile() {
		wxString new_file = wxT("new_file");

		wxString path;
		wxString name;
		wxString ext;

    if( wxFileName::DirExists(this->GetPath())){
      path = this->GetPath() ;
    }

    if( wxFileName::FileExists(this->GetPath())){
      wxFileName::SplitPath(this->GetPath(), &path, &name, &ext);
    }
    wxFileName *wf = new wxFileName(path, new_file, wxT(""));
    wxFile f;

    if (!wf->FileExists()){

      f.Create( wf->GetFullPath() , false);
      //Expand to new created file
      this->Refresh();
      this->ExpandPath( wf->GetFullPath());

    } else {
      int i = 2;
      while
        (wxFile::Exists( wxString::Format( wf->GetFullPath() + wxT("(%i)"), i))){
        ++i;
      }
      f.Create(wxString::Format( wf->GetFullPath() + wxT("(%i)"), i));

      //Expand to new created file
      this->Refresh();
      this->ExpandPath(wxString::Format( wf->GetFullPath() + wxT("(%i)"), i));
    }

	}
	;

	void OnActivate(wxTreeEvent& /*evt*/){

	  if(!wxDir::Exists(this->GetPath())){
	    Edit();
	  } else{
	    this->ExpandPath(this->GetPath());
	  }
	}

	void OnActivity(wxTreeEvent& /*evt*/){
	  wxString path; 
	  wxString name;
	  wxString ext;

    if( wxFileName::DirExists(this->GetPath())){
      path = this->GetPath() ;
    }

    if( wxFileName::FileExists(this->GetPath())){
      wxFileName::SplitPath(this->GetPath(), &path, &name, &ext);
    }

	  if(!wxFileName::IsDirWritable( path )){
	    wxLogStatus(wxT("WARNING: No write permissions in the selected directory!"));
	  } else {
		wxLogStatus(wxT(""));
	  }

	}

	wxTimer *refresh_dir;
protected:
	wxWindow *m_parent;

DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(GenericDirCtrl, wxGenericDirCtrl)
  EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, GenericDirCtrl::OnRightClick )
  EVT_TREE_KEY_DOWN		(wxID_ANY, GenericDirCtrl::onKeyDown)
  EVT_TREE_ITEM_ACTIVATED(wxID_ANY, GenericDirCtrl::OnActivate )
  EVT_TREE_SEL_CHANGED(wxID_ANY, GenericDirCtrl::OnActivity)
END_EVENT_TABLE ()

#endif /* PROJECTCONTROL_H_ */
