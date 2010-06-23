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
#include <iostream>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/aui/auibook.h>
#include <gui/outputlistbox.h>
#include <gui/configpanel.h>

#define ID_GDC		2000
#define ID_OPEN    2001
#define ID_EDIT		2002
#define ID_RENAME  2003
#define ID_DETAILS	2004
#define ID_DELETE	2005
#define ID_NEW_FILE 2006
#define ID_NEW_DIR 2007

using namespace std;

class GenericDirCtrl: public wxGenericDirCtrl {
public:

	std::vector<Tool> m_tool_catalog;

	multimap<string, string> m_extention_tool_mapping;

	OutputListBox *m_listbox_output;

	wxAuiNotebook *m_notebookpanel;

	vector<wxRadioBox*> m_radiobox_ptrs;
	vector<wxCheckBox*> m_checkbox_ptrs;

	GenericDirCtrl(wxWindow *parent, const std::vector<Tool>& tool_catalog,
			multimap<string, string> extention_tool_mapping, OutputListBox *output,
			wxAuiNotebook *notebookpanel) :
		wxGenericDirCtrl(parent, ID_GDC, wxDirDialogDefaultFolderStr, wxPoint(
				-1, -1), wxSize(-1, -1), wxDIRCTRL_EDIT_LABELS
				| wxDIRCTRL_3D_INTERNAL | wxSUNKEN_BORDER) {
		m_parent = parent;
		m_tool_catalog = tool_catalog;
		m_listbox_output = output;
		m_notebookpanel = notebookpanel;
		m_extention_tool_mapping = extention_tool_mapping;
		refresh_dir = new wxTimer(this, ID_TIMER);
		refresh_dir->Start(500);
		this->Fit();
		this->Layout();

	}

	void OnPopupClick(wxCommandEvent &evt) {

		if (evt.GetId() < ID_GDC) {


			Tool tool = m_tool_catalog[evt.GetId()];

			FileIO fileIO;
			fileIO.input_file = this->GetPath().mb_str(wxConvUTF8);

			ConfigPanel *ncp = new ConfigPanel(m_notebookpanel, wxID_ANY,
					m_tool_catalog[evt.GetId()], m_listbox_output, fileIO);

			ncp->Layout();

			m_notebookpanel->AddPage( ncp , this->GetPath(), true );

		} else {

			wxTreeItemId id, pid;
			wxString s;
			string new_file = "new_file";
			wxString new_dir = wxT("new_dir");
			FILE *file;
			string filepath;

			switch (evt.GetId()) {
			case ID_NEW_FILE:
				//Parent ID
				pid = this->GetTreeCtrl()->GetItemParent(
						this->GetTreeCtrl()->GetSelection());

				if (!wxDir::Exists(this->GetPath())) {
					s = this->GetPath().BeforeLast(_T('/')).Append(_T('/'));
				} else {
					s = this->GetPath().Append(_T('/'));
				}

				filepath = static_cast<string> (s.mb_str(wxConvUTF8))
						+ new_file;

				// Generate new name for file if file exists
				if (wxFile::Exists(wxString(filepath.c_str(), wxConvUTF8))) {
					int i = 2;

					while (wxFile::Exists(wxString::Format(wxString(
							filepath.c_str(), wxConvUTF8) + wxT("(%i)"), i))) {
						++i;
					}
					filepath = wxString::Format(wxString(filepath.c_str(), wxConvUTF8) + wxT("(%i)"), i).mb_str(wxConvUTF8);

				}

				// Try to create file
				file = fopen(filepath.c_str(), "wt");

				// If create fails
				if (!file) {
					wxLogMessage(wxT("Cannot create the following file: '")
							+ wxString(string(s.mb_str(wxConvUTF8)).append(
									new_file).c_str(), wxConvUTF8) + wxT("'."));
				} else {
					//If file creation succeeds, create new element in tree, with nice icon
					id = this->GetTreeCtrl()->AppendItem(pid, wxString(
							new_file.c_str(), wxConvUTF8), 7);

					//Update Tree (by collapse parent and goto new created file
					if (!wxDir::Exists(this->GetPath())) {
						this->GetTreeCtrl()->Collapse(pid);
					} else {
						this->GetTreeCtrl()->Collapse(
								this->GetTreeCtrl()->GetSelection());
					}

					//Expand to new created file
					this->ExpandPath(wxString(filepath.c_str(), wxConvUTF8));

					//Possibility to rename file
					this->GetTreeCtrl()->EditLabel(
							this->GetTreeCtrl()->GetSelection());

				}

				break;
			case ID_NEW_DIR:
				//Parent ID
				pid = this->GetTreeCtrl()->GetItemParent(
						this->GetTreeCtrl()->GetSelection());

				if (!wxDir::Exists(this->GetPath())) {
					s = this->GetPath().BeforeLast(_T('/')).Append(_T('/'));
				} else {
					s = this->GetPath().Append(_T('/'));
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
					wxLogMessage(
							wxT("Cannot create the following directory: '")
									+ s + wxT("'."));
				} else {
					//Create Dir, with icon in tree
					this->GetTreeCtrl()->AppendItem(pid, s, 0);
					//Collapse tree
					this->GetTreeCtrl()->Collapse(pid);
					//Expand tree to new dir
					this->ExpandPath(s);
					//Re-label dir
					this->GetTreeCtrl()->EditLabel(
							this->GetTreeCtrl()->GetSelection());
				}

				break;
			case ID_EDIT:
				if (!this->GetPath().empty()) {

					wxString ext = this->GetPath().AfterLast(_T('.'));
					wxFileType
							*ft =
									wxTheMimeTypesManager->GetFileTypeFromExtension(
											ext);
					if (ft) {
						wxString cmd;
						bool ok = ft->GetOpenCommand(&cmd,
								wxFileType::MessageParameters(this->GetPath()));
						if (ok) {
							wxExecute(cmd, wxEXEC_ASYNC, NULL);
						}
						delete ft;
					} else {
#ifdef __linux
						wxLogMessage(_T("No editor defined by UNIX operating system for editing files of extension '%s'."),
								ext.c_str());
#endif
#ifdef __WINDOWS__
						wxLogMessage(_T("No editor defined by Windows operating system for editing files of extension '%s'."),
								ext.c_str());
#endif
						//TODO:: Pop-Up for assigning editor to file extention
						return;
					}
				}
				break;
			case ID_DETAILS:
				std::cout << "TODO: implement this" << std::endl;
				break;
			case ID_RENAME:

				this->GetTreeCtrl()->EditLabel(
						this->GetTreeCtrl()->GetSelection());

				break;
			case ID_DELETE:
				wxMessageDialog
						*dial =
								new wxMessageDialog(
										NULL,
										wxT("This action will delete the file from disk.\nAre you sure to continue?"),
										wxT("Question"), wxYES_NO
												| wxNO_DEFAULT
												| wxICON_QUESTION);
				if (dial->ShowModal() == wxID_YES) {

					if (wxFile::Exists(this->GetPath())) {
						if (remove(this->GetPath().mb_str(wxConvUTF8)) != 0) {
							wxLogError(wxT("Error deleting file"));
							break;
						}
					};

					if (wxDir::Exists(this->GetPath())) {
						if (!wxRmdir(this->GetPath())) {
							wxLogError(wxT("Error deleting directory"));
							break;
						}
					};

					{
						/* Remove from tree */
						wxTreeItemId item_for_removal =
								this->GetTreeCtrl()->GetSelection();

						if (this->GetTreeCtrl()->GetNextSibling(
								item_for_removal).IsOk()) {
							this->GetTreeCtrl()->SelectItem(
									this->GetTreeCtrl()->GetNextSibling(
											item_for_removal));
						} else if (this->GetTreeCtrl()->GetPrevSibling(
								item_for_removal).IsOk()) {
							this->GetTreeCtrl()->SelectItem(
									this->GetTreeCtrl()->GetPrevSibling(
											item_for_removal));
						} else if (this->GetTreeCtrl()->GetPrevSibling(
								item_for_removal).IsOk()) {
							this->GetTreeCtrl()->SelectItem(
									this->GetTreeCtrl()->GetPrevVisible(
											item_for_removal));
						};

						this->GetTreeCtrl()->Delete(item_for_removal);
					}
				}

				break;
			}
		}
	}

	void OnRightClick(wxTreeEvent& evt) {

		wxMenu mnu;

		wxString ext = this->GetPath().AfterLast(_T('.'));
		std::string sext = (std::string) ext.mb_str(wxConvUTF8);

		wxMenu *reporting = new wxMenu();
		wxMenu *transformation = new wxMenu();

		if (!this->GetPath().empty()) {
			mnu.Append(ID_EDIT, wxT("Edit"));

			for (vector<Tool>::iterator i = m_tool_catalog.begin(); i
					!= m_tool_catalog.end(); ++i) {
				//Compare if extension occurs as input parameter
				//TODO: take alternative extensions into account

				multimap<string, string>::iterator fnd =
						m_extention_tool_mapping.find(sext);
				if (fnd != m_extention_tool_mapping.end()) {
					if (i->m_input_type.compare(fnd->second) == 0) {

						if (i->m_output_type.empty()) {
							reporting->Append(distance(m_tool_catalog.begin(),
									i), wxString((*i).m_name.c_str(),
									wxConvUTF8));
						} else {
							transformation->Append(distance(
									m_tool_catalog.begin(), i), wxString(
									(*i).m_name.c_str(), wxConvUTF8));
						}
					}
				}
			}
		}

		if ((transformation->GetMenuItemCount() != 0)
				|| (reporting->GetMenuItemCount() != 0)) {
			mnu.AppendSeparator();
		}

		if (reporting->GetMenuItemCount() != 0) {
			mnu.AppendSubMenu(reporting, wxT("Reporting"));
		}
		if (transformation->GetMenuItemCount() != 0) {
			mnu.AppendSubMenu(transformation, wxT("Transformation"));
		}
		reporting->Connect(wxEVT_COMMAND_MENU_SELECTED,
				(wxObjectEventFunction) &GenericDirCtrl::OnPopupClick, NULL,
				this);
		transformation->Connect(wxEVT_COMMAND_MENU_SELECTED,
				(wxObjectEventFunction) &GenericDirCtrl::OnPopupClick, NULL,
				this);

		if ((transformation->GetMenuItemCount() != 0)
				|| (reporting->GetMenuItemCount() != 0)) {
			mnu.AppendSeparator();
		}

		mnu.Append(ID_NEW_FILE, wxT("New File"));
		mnu.Append(ID_NEW_DIR, wxT("New Directory"));
		mnu.AppendSeparator();
		mnu.Append(ID_RENAME, wxT("Rename"));
		mnu.Append(ID_DELETE, wxT("Delete"));
		mnu.AppendSeparator();
		mnu.Append(ID_DETAILS, wxT("Details"));
		mnu.Connect(wxEVT_COMMAND_MENU_SELECTED,
				(wxObjectEventFunction) &GenericDirCtrl::OnPopupClick, NULL,
				this);
		PopupMenu(&mnu);
	}

	wxTimer *refresh_dir;
protected:
	wxWindow *m_parent;

DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(GenericDirCtrl, wxGenericDirCtrl)
EVT_TREE_ITEM_RIGHT_CLICK( wxID_ANY, GenericDirCtrl::OnRightClick )
//EVT_TREE_ITEM_LEFT_CLICK(GenericDirCtrl::OnLeftClick )
END_EVENT_TABLE		()

#endif /* PROJECTCONTROL_H_ */
