//  Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file ./ltsgraph_backup.cpp

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
		tmp.sprintf(wxT("NODE(%d)(%d):%d(%d):%f-%f:%s%s"), BackupVectNode[n]->IsLocked(), 
        BackupVectNode[n]->IsInitState(),
        BackupVectNode[n]->Get_num(), 
        BackupVectNode[n]->LabelVisible(),
        BackupVectNode[n]->GetX(),
        BackupVectNode[n]->GetY(),
        BackupVectNode[n]->GetNodeColour().GetAsString(wxC2S_CSS_SYNTAX).c_str(),
        BackupVectNode[n]->GetBorderColour().GetAsString(wxC2S_CSS_SYNTAX).c_str());

           BckpFile.AddLine(tmp);
	}

	//EDGE:numNode1,numNode2,IsVisible?
	//label
  for (size_t n = 0; n < BackupVectEdge.size(); n++) {
		wxString lbl(BackupVectEdge[n]->get_lbl().c_str(), wxConvLocal);
		tmp.sprintf(wxT("EDGE:%d,%d,%d,%s"),  
                BackupVectEdge[n]->get_n1()->Get_num(),BackupVectEdge[n]->get_n2()->Get_num(),BackupVectEdge[n]->LabelVisible(),
		BackupVectEdge[n]->get_label_colour().GetAsString(wxC2S_CSS_SYNTAX).c_str());
		BckpFile.AddLine(tmp);
		BckpFile.AddLine(lbl);
  }

	if (!BckpFile.Write())
		return false;

	if (!BckpFile.Close())
		return false;

	return true;
}

bool LtsgraphBackup::Restore(wxString Rfilename) {
	
	Rfilename.Append(wxT(".ltsgraph"));
	wxString wx_str(Rfilename);
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

        long int tmp;
	if (!RtrFile.GetNextLine().ToLong(&tmp))
		return false;
        StateRadius = tmp;
	if (!RtrFile.GetNextLine().ToLong(&tmp))
		return false;
        StateLabel = tmp;
	if (!RtrFile.GetNextLine().ToLong(&tmp))
		return false;
        TransitionLabel = tmp;

	map<unsigned int, Node*> NodeMap;
	wxString wxstr = RtrFile.GetNextLine();

	for ( ;!RtrFile.Eof() ; wxstr = RtrFile.GetNextLine() ) {

                struct rgb_colour {
                  unsigned int red;
                  unsigned int green;
                  unsigned int blue;
                };

		if (wxstr.Left(4).IsSameAs(wxT("NODE"))) { //NODE(IsLocked?)(IsInitState?):num(IsVisible?):coordX-coordY
			int IsLocked, IsInitState, IsVisible;
			unsigned int numNode;
			double coordX, coordY;

                        rgb_colour node_colour, border_colour;

			sscanf(wxstr.fn_str(),"NODE(%d)(%d):%d(%d):%lf-%lf:rgb(%3d,%3d,%3d)rgb(%3d,%3d,%3d)", &IsLocked, &IsInitState, &numNode, &IsVisible, &coordX, &coordY,
                           &node_colour.red, &node_colour.green, &node_colour.blue, &border_colour.red, &border_colour.green, &border_colour.blue);

			wxString tmp_lbl;
			tmp_lbl.sprintf(wxT("%d"), numNode);
			wxString * Slbl_Node = new wxString(tmp_lbl);

			NodeMap[numNode] = new Node(numNode, coordX, coordY, *Slbl_Node, (IsInitState == 1) );	
                        NodeMap[numNode]->SetNodeColour(wxColour(static_cast< unsigned char > (node_colour.red),
                                                                static_cast< unsigned char > (node_colour.green),
                                                                static_cast< unsigned char > (node_colour.blue)));
                        NodeMap[numNode]->SetBorderColour(wxColour(static_cast < unsigned char > (border_colour.red),
                                                                  static_cast < unsigned char > (border_colour.green),
                                                                  static_cast < unsigned char > (border_colour.blue)));

			BackupVectNode.push_back( NodeMap[numNode] );

			delete Slbl_Node;
		}
		//EDGE:numNode1,numNode2,IsVisible?
		//label
		else if (wxstr.Left(4).IsSameAs(wxT("EDGE"))) { 
			int IsVisible;
			unsigned int NumNode1, NumNode2;
                        rgb_colour label_colour;

			sscanf(wxstr.fn_str(),"EDGE:%d,%d,%d,rgb(%3d,%3d,%3d)", &NumNode1, &NumNode2, &IsVisible,
                                &label_colour.red, &label_colour.green, &label_colour.blue);

			if (!RtrFile.Eof())
				wxstr = RtrFile.GetNextLine();
			BackupVectEdge.push_back(new edge(NodeMap[NumNode1],NodeMap[NumNode2],wxstr));

                        BackupVectEdge.back()->set_label_colour(wxColour(static_cast< unsigned char > (label_colour.red),
                                                                         static_cast< unsigned char > (label_colour.green),
                                                                         static_cast< unsigned char > (label_colour.blue)));
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


