#include "ltsgraph_backup.h"
#include "node.h"
#include <wx/textfile.h>
#include <map>


LtsgraphBackup::LtsgraphBackup(wxSize _sz) : sz(_sz) {}

void LtsgraphBackup::SetLayout(vector<Node*> _BackupVectNode, vector<edge*> _BackupVectEdge) {

	BackupVectNode = _BackupVectNode;
	BackupVectEdge = _BackupVectEdge;

}

void LtsgraphBackup::SetInformation(wxString _InitialState, wxString _NumStates, wxString _NumTransitions, wxString _NumLabels) {

	InitialState   = _InitialState;
	NumStates      = _NumStates;
	NumTransitions = _NumTransitions;
	NumLabels      = _NumLabels;

} 

void LtsgraphBackup::SetAlgoSettings(double _EdgeStiffness, double _NodeStrength, double _NaturalLength) {
	
	EdgeStiffness = _EdgeStiffness;
	NodeStrength  = _NodeStrength;
	NaturalLength = _NaturalLength;

}

void LtsgraphBackup::SetOtherSettings(int _StateRadius, bool _StateLabel, bool _TransitionLabel) {

	StateRadius     = _StateRadius;
	StateLabel      = _StateLabel;
	TransitionLabel = _TransitionLabel;

}

bool LtsgraphBackup::Backup(wxString Bfilename) {

	wxTextFile BckpFile(Bfilename);

	if (BckpFile.Exists()) {
		if (!BckpFile.Open(Bfilename))
			return false;
		else
			BckpFile.Clear();
	}
	else {
		if (!BckpFile.Create(Bfilename))
			return false;
	}
		

	BckpFile.AddLine(InitialState);
	BckpFile.AddLine(NumStates);
	BckpFile.AddLine(NumTransitions);
	BckpFile.AddLine(NumLabels);

	wxString tmp;
	tmp.sprintf(wxT("%f"), NodeStrength);
	BckpFile.AddLine(tmp);
	tmp.sprintf(wxT("%f"), EdgeStiffness);
	BckpFile.AddLine(tmp);
	tmp.sprintf(wxT("%f"), NaturalLength);
	BckpFile.AddLine(tmp);

	tmp.sprintf(wxT("%d"), StateRadius);
	BckpFile.AddLine(tmp);
	tmp.sprintf(wxT("%d"), StateLabel);
	BckpFile.AddLine(tmp);
	tmp.sprintf(wxT("%d"), TransitionLabel);
	BckpFile.AddLine(tmp);

	//NODE(IsLocked?)(IsInitState?):num(IsVisible?):coordX-coordY
	for (size_t n = 0; n < BackupVectNode.size(); n++) {
		tmp.sprintf(wxT("NODE(%d)(%d):%d(%d):%f-%f"), BackupVectNode[n]->IsLocked(), 
																									BackupVectNode[n]->IsInitState(),
																		 					    BackupVectNode[n]->Get_num(), 
																							    BackupVectNode[n]->LabelVisible(),
																		 					    BackupVectNode[n]->GetX(),
																		 					    BackupVectNode[n]->GetY()
							 );				
		BckpFile.AddLine(tmp);
	}

	//EDGE:numNode1,numNode2,IsVisible?
	//label
  for (size_t n = 0; n < BackupVectEdge.size(); n++) {
		wxString lbl(BackupVectEdge[n]->get_lbl().c_str(), wxConvLocal);
		tmp.sprintf(wxT("EDGE:%d,%d,%d,"),  
                BackupVectEdge[n]->get_n1()->Get_num(),		 				BackupVectEdge[n]->get_n2()->Get_num(),						BackupVectEdge[n]->LabelVisible()
							 );				
		BckpFile.AddLine(tmp);
		BckpFile.AddLine(lbl);
  }

	BckpFile.AddLine(wxT(""));
	
	if (!BckpFile.Write())
		return false;

	if (!BckpFile.Close())
		return false;

	return true;
}

bool LtsgraphBackup::Restore(wxString Rfilename) {
	
	Rfilename.Append(wxT(".ltsgraph"));
	wxString wx_str(Rfilename.c_str(), wxConvLocal);
	wxTextFile RtrFile(wx_str);
	if (RtrFile.Exists()) {
		if (!RtrFile.Open(wx_str))
			return false;
	}
	else
		return false;

	InitialState   = RtrFile.GetFirstLine();
	NumStates      = RtrFile.GetNextLine();
	NumTransitions = RtrFile.GetNextLine();
	NumLabels      = RtrFile.GetNextLine();

	if (!RtrFile.GetNextLine().ToDouble(&NodeStrength))
		return false;
	if (!RtrFile.GetNextLine().ToDouble(&EdgeStiffness))
		return false;
	if (!RtrFile.GetNextLine().ToDouble(&NaturalLength))
		return false;

	if (!RtrFile.GetNextLine().ToULong((long unsigned int*)&StateRadius))
		return false;
	if (!RtrFile.GetNextLine().ToULong((long unsigned int*)&StateLabel))
		return false;
	if (!RtrFile.GetNextLine().ToULong((long unsigned int*)&TransitionLabel))
		return false;


	map<unsigned int, Node*> NodeMap;
	wxString wxstr = RtrFile.GetNextLine();

	for ( ;!RtrFile.Eof() ; wxstr = RtrFile.GetNextLine() ) {

		if (wxstr.Left(4).IsSameAs(wxT("NODE"))) { //NODE(IsLocked?)(IsInitState?):num(IsVisible?):coordX-coordY
			int IsLocked, IsInitState, IsVisible;
			unsigned int numNode;
			double coordX, coordY;

			sscanf(wxstr.fn_str(),"NODE(%d)(%d):%d(%d):%lf-%lf", &IsLocked, &IsInitState, &numNode, &IsVisible, &coordX, &coordY );

			wxString tmp_lbl;
			tmp_lbl.sprintf(wxT("%d"), numNode);
			wxString * Slbl_Node = new wxString(tmp_lbl);

			NodeMap[numNode] = new Node(numNode, coordX, coordY, *Slbl_Node, (IsInitState == 1) );	

			BackupVectNode.push_back( NodeMap[numNode] );

			delete Slbl_Node;
		}
		//EDGE:numNode1,numNode2,IsVisible?
		//label
		else if (wxstr.Left(4).IsSameAs(wxT("EDGE"))) { 
			int IsVisible;
			unsigned int NumNode1, NumNode2;

			sscanf(wxstr.fn_str(),"EDGE:%d,%d,%d,", &NumNode1, &NumNode2, &IsVisible );

			if (!RtrFile.Eof())
				wxstr = RtrFile.GetNextLine();
			BackupVectEdge.push_back(new edge(NodeMap[NumNode1],NodeMap[NumNode2],wxstr));

		}
		else 
			return false;
	}

	return true;
}

vector<Node*> LtsgraphBackup::GetVectNode() {
	return BackupVectNode;
}

vector<edge*> LtsgraphBackup::GetVectEdge() {
	return BackupVectEdge;
}
		
wxString LtsgraphBackup::GetInitialState() {
	return InitialState;
}

wxString LtsgraphBackup::GetNumStates() {
	return NumStates;
}

wxString LtsgraphBackup::GetNumTransitions() {
	return NumTransitions;
}

wxString LtsgraphBackup::GetNumLabels() {
	return NumLabels;
}

double LtsgraphBackup::GetEdgeStiffness() {
	return EdgeStiffness;
}

double LtsgraphBackup::GetNodeStrength() {
	return NodeStrength;
}

double LtsgraphBackup::GetNaturalLength() {
	return NaturalLength;
}

int LtsgraphBackup::GetStateRadius() {
	return StateRadius;
}

bool LtsgraphBackup::GetStateLabel() {
	return StateLabel;
}

bool LtsgraphBackup::GetTransitionLabel() {
	return TransitionLabel;
}


