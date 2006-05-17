#include "graph_frame.h" 
#include <wx/utils.h>
#include <wx/msgdlg.h>


BEGIN_EVENT_TABLE(GraphFrame, wxFrame)
    EVT_PAINT(GraphFrame::OnPaint)
	EVT_LEFT_DOWN(GraphFrame::PressLeft)
    EVT_MOTION(GraphFrame::Drag)
	EVT_LEFT_UP(GraphFrame::ReleaseLeft)
	EVT_RIGHT_UP(GraphFrame::PressRight)
	EVT_MENU(wxID_OPEN, GraphFrame::OnOpen)
	EVT_MENU(wxID_EXIT, GraphFrame::OnQuit)
	EVT_MENU(ID_SET_EDGE_STIF, GraphFrame::OnSetEdgeStiffness)
	EVT_MENU(ID_SET_NODE_STRE, GraphFrame::OnSetNodeStrenght)
	EVT_MENU(ID_SET_NATU_LENG, GraphFrame::OnSetNaturalLength)
	EVT_MENU(ID_OPTIMIZE, GraphFrame::OnOptimize)
	EVT_SIZE(GraphFrame::OnResize)
END_EVENT_TABLE()


double GenRandom(const int &max) {
    return static_cast <double> (rand()%max+CIRCLE_RADIUS); 
}

GraphFrame::GraphFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxFrame(NULL, -1, title, pos, size, style) {

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
 
    #ifdef MCRL2_BCG
        BCG_INIT();
    #endif
	sz = GetClientSize();

	EdgeStiffness = 0.5; 
	NodeStrenght = 5000.0; 
	NaturalLength = 150.0;

	CreateMenu();

}

void GraphFrame::CreateMenu() {

    menu = new wxMenuBar;
    
	//file
	file = new wxMenu;
    openItem = file->Append( wxID_OPEN, wxT("&Open...	CTRL-o"), wxT("") );
	quitItem = file->Append( wxID_EXIT, wxT("&Quit	CTRL-q"), wxT("") );
	menu->Append( file, wxT("&File") );

	//draw
	draw = new wxMenu;
	optimizegraph = draw->Append(ID_OPTIMIZE, wxT("Op&timize Graph"), wxT("") );
	menu->Append(draw, wxT("&Draw") );

	//options 
	options = new wxMenu;
	setEdgeStiffnessItem = options->Append( ID_SET_EDGE_STIF, wxT("Set &Edge Stiffness"), wxT("") );
	setNodeStrenghtItem  = options->Append( ID_SET_NODE_STRE, wxT("Set &Node Strenght"), wxT("") );
	setNaturalLengthItem = options->Append( ID_SET_NATU_LENG, wxT("Set Edge Natural &Length"), wxT("") );
	menu->Append( options, wxT("&Options") );

	SetMenuBar( menu );
}

//Rezising Graph
void GraphFrame::OnResize(wxSizeEvent& event) {

	
	wxSize sz2 = GetClientSize();

	double diff_x = (double) sz2.GetWidth() / (double) sz.GetWidth();
	double diff_y = (double) sz2.GetHeight() / (double) sz.GetHeight();

	double x;
	double y;
    for (size_t m = 0; m < vectNode.size(); m++) {
        vectNode[m]->SetXY((int)(vectNode[m]->GetX() * diff_x), (int)(vectNode[m]->GetY() * diff_y));

		x = vectNode[m]->GetX();
		y = vectNode[m]->GetY();

		//find Edges and change coord
		for (size_t n = 0; n < vectEdge.size(); n++) {
			if (vectEdge[n]->Get_numN1() == vectNode[m]->Get_num()) {
				vectEdge[n]->Set_pos1(x,y);
			}
			if (vectEdge[n]->Get_numN2() == vectNode[m]->Get_num()) {
				vectEdge[n]->Set_pos2(x,y);
			}
		}
    }

	Refresh();
	Update();

	sz = sz2;

}


void GraphFrame::OnOpen( wxCommandEvent& /* event */ ) {
    wxFileDialog dialog( this, wxT("Select a LTS file..."), wxT(""), wxT(""), wxT("*.aut |*.aut|*.svc|*.svc|All files|*"));
    if ( dialog.ShowModal() == wxID_OK )
    {
	    vectEdge.clear();
		vectNode.clear();
		Init(dialog.GetPath());
		Refresh();
		Update();
    }
}

void GraphFrame::OnQuit( wxCommandEvent& /* event */ )
{
     Close( TRUE );
}

void GraphFrame::OnSetEdgeStiffness( wxCommandEvent& /* event */ ) {
	wxString InitValue;
	InitValue.sprintf(wxT("%f"), EdgeStiffness);
	wxTextEntryDialog * ses = new wxTextEntryDialog(this,wxT(""), wxT("Edge Stiffness"), InitValue, wxOK | wxCANCEL | wxCENTRE, wxDefaultPosition);
	ses->ShowModal();
	wxString StrValue = ses->GetValue();
	StrValue.ToDouble(&EdgeStiffness);
}

void GraphFrame::OnSetNodeStrenght( wxCommandEvent& /* event */ ) {
	wxString InitValue;
	InitValue.sprintf(wxT("%f"), NodeStrenght,.2);
	wxTextEntryDialog * sns = new wxTextEntryDialog(this,wxT(""), wxT("Node Strenght"), InitValue,wxOK | wxCANCEL | wxCENTRE, wxDefaultPosition);
	sns->ShowModal();
	wxString StrValue = sns->GetValue();
	StrValue.ToDouble(&NodeStrenght);
}

void GraphFrame::OnSetNaturalLength( wxCommandEvent& /* event */ ) {
	wxString InitValue;
	InitValue.sprintf(wxT("%f"), NaturalLength);
	wxTextEntryDialog * snl = new wxTextEntryDialog(this,wxT(""), wxT("Natural Length"), InitValue,wxOK | wxCANCEL | wxCENTRE, wxDefaultPosition);
	snl->ShowModal();
	wxString StrValue = snl->GetValue();
	StrValue.ToDouble(&NaturalLength);
}

void GraphFrame::OnPaint(wxPaintEvent& /* event */) {
    
    wxPaintDC myDC(this);

    //Call Edge and Node OnPaint() method (Edge 1st)
    
    for (size_t n = 0; n < vectEdge.size(); n++) {
        vectEdge[n]->OnPaint(&myDC);
    }
      
    for (size_t n = 0; n < vectNode.size(); n++) {
        vectNode[n]->OnPaint(&myDC);
    }

}


void GraphFrame::OnOptimize( wxCommandEvent& /* event */ ) {

		while (!OptimizeDrawing(0.0)) {
			wxTheApp->Yield(true); // to allow user to interrupt optimizing
		}

}

//init vectNode & vectEdge
void GraphFrame::Init(wxString LTSfile) {

	string st_LTSfile = string(LTSfile.fn_str());

    //read lts file
	lts mylts;
    if (mylts.read_from(st_LTSfile)) {
    
        //Information
        cout << "Initial state : " << mylts.initial_state() << endl;
        cout << "Num states : " << mylts.num_states() << endl;
        cout << "Num transitions : " << mylts.num_transitions() << endl;
        cout << "Num labels : " << mylts.num_labels() << endl;
  
        //initialize vectNode

        state_iterator si = mylts.get_states();
        
        wxString tmp;
		int randX = sz.GetWidth()  - 2*CIRCLE_RADIUS;
		int randY = sz.GetHeight() - 4*CIRCLE_RADIUS;		

        while (si.more()) {
            tmp.sprintf(wxT("%d"), *si);
            wxString * Slbl_Node = new wxString(tmp);
            vectNode.push_back(new Node(*si, GenRandom(randX), GenRandom(randY), *Slbl_Node));
            ++ si;
            delete Slbl_Node;
        }   
    
    
        //initialize vectEdge

        transition_iterator ti = mylts.get_transitions();
        wxPoint * pe1;
        wxPoint * pe2;
    
        while (ti.more()) {
            
            wxString * Slbl_Edge = new wxString(ATwriteToString(mylts.label_value(ti.label())), wxConvLocal);

            for (size_t n = 0; n < vectNode.size(); n++) {
                if (vectNode[n]->Get_num() == ti.from()) {
                    for (size_t m = 0; m < vectNode.size(); m++) {
                        if (vectNode[m]->Get_num() == ti.to()) {
                            vectEdge.push_back(new Edge(ti.from(),ti.to(),vectNode[n]->GetX(), vectNode[n]->GetY(), vectNode[m]->GetX(), vectNode[m]->GetY(), *Slbl_Edge));
                        }
                    }
                }
            }
        
            delete Slbl_Edge;       
            ++ ti;
        }

		Refresh();
		Update();
    }
    else {
        cout << "Error : cannot read " << st_LTSfile << endl;
		exit(0);
	}
}

////////////// VERSION WITH LOGARITHMIC APPROACH  ///////////////////////////////////

// static double old_precX = 0.0;
// static double old_precY = 0.0;
// 
// bool GraphFrame::OptimizeDrawing(double precision) {
// 
// 	double x1, y1, x2, y2 = 0.0;
// 	double x2Minx1, y2Miny1 = 0.0;	
// 	double x2Minx1DivDist, y2Miny1DivDist = 0.0;
// 
// 	double distance, forceX, forceNodeX, forceY, forceNodeY, force = 0.0;
// 
// 	double arrayForceX[vectNode.size()][vectNode.size()];
// 	double arrayForceY[vectNode.size()][vectNode.size()];
// 	double arraySumForceX[vectNode.size()];
// 	double arraySumForceY[vectNode.size()];
// 
// 	bool end = false;
// 
// 	
// 	//Calculate forces
// 	for (size_t i = 0; i<vectNode.size(); i++) {
// 		double X = 0;
// 		double Y = 0;
// 		x1 = vectNode[i]->GetX();
// 		y1 = vectNode[i]->GetY();
// 
// 		for (size_t j = 0; j<vectNode.size(); j++) {
// 			if (i != j) {
// 				x2 = vectNode[j]->GetX();
// 				y2 = vectNode[j]->GetY();
// 
// 				x2Minx1 = x2 - x1;
// 				y2Miny1 = y2 - y1;
// 				distance = sqrt( (x2Minx1*x2Minx1) + (y2Miny1*y2Miny1) );//Euclidean distance
// 
// 				double forceEdgeX = 0;
// 				double forceEdgeY = 0;
// 				double forceNodeX = 0;
// 				double forceNodeY = 0;
// 
// 				if (distance != NaturalLength && distance > 0) {
// 
// 					for (size_t n = 0; n < vectEdge.size(); n++) {
// 						if (vectEdge[n]->Get_numN1() == vectNode[i]->Get_num() && vectEdge[n]->Get_numN2() == vectNode[j]->Get_num()) {
// 							forceEdgeX += ( EdgeStiffness * log(distance / NaturalLength) );
// 							forceEdgeY += ( EdgeStiffness * log(distance / NaturalLength) );
// 						}
// 					}
// 				}
// 
// 
// 				if (distance > 0) { // to avoid division per 0
// 					forceNodeX = ( NodeStrenght / ( distance*distance ) );
// 					forceNodeY = ( NodeStrenght / ( distance*distance ) );
// 				}
// 				X += forceEdgeX;
// 				Y += forceEdgeY;
// 				forceX = 0.1*(forceEdgeX + forceNodeX);
// 				forceY = 0.1*(forceEdgeY + forceNodeY);
// 
// 				arrayForceX[i][j] = forceX;
// 				arrayForceY[i][j] = forceY;
// 
// 			}
// 		}
// 	}
// 
// 	for (size_t i = 0; i<vectNode.size(); i++) {
// 		arraySumForceX[i] = 0.0;
// 		arraySumForceY[i] = 0.0;
// 	}
// 
// 	//Sum forces for each nodes
// 	for (size_t i = 0; i<vectNode.size(); i++) {
// 		for (size_t j = 0; j<vectNode.size(); j++) {
// 			arraySumForceX[i] += arrayForceX[i][j];
// 			arraySumForceY[i] += arrayForceY[i][j];
// 		}
// 	}
// 
// 	//Replace the nodes & edges according to their new position
// 	for (size_t i = 0; i<vectNode.size(); i++) {
// 		double newX = 0;
// 		double newY = 0;
// 		newX = vectNode[i]->GetX() + arraySumForceX[i];
// 		newY = vectNode[i]->GetY() + arraySumForceY[i];
// 		
// 		//Check whether positions are outside of the window
// 		if (newX > sz.GetWidth())
// 			newX = sz.GetWidth() - CIRCLE_RADIUS;
// 		if (newX < CIRCLE_RADIUS)
// 			newX = 0 + CIRCLE_RADIUS;
// 		if (newY > sz.GetHeight())
// 			newY = sz.GetHeight() - CIRCLE_RADIUS;
// 		if (newY < CIRCLE_RADIUS)
// 			newY = 0 + CIRCLE_RADIUS;
// 
// 		vectNode[i]->SetXY( newX , newY );
// 		
// 		for (size_t n = 0; n < vectEdge.size(); n++) {
// 			if (vectEdge[n]->Get_numN1() == vectNode[i]->Get_num()) {
// 				vectEdge[n]->Set_pos1(vectNode[i]->GetX(),vectNode[i]->GetY());
// 			}
// 			if (vectEdge[n]->Get_numN2() == vectNode[i]->Get_num()) {
// 				vectEdge[n]->Set_pos2(vectNode[i]->GetX(),vectNode[i]->GetY());
// 			}
// 		}
// 	}
// 
// 	//Calculate the precision of the drawing
// 	double precX = 0.0;
// 	double precY = 0.0;
// 
// 	for (size_t i = 0; i<vectNode.size(); i++) { 
// 		precX += arraySumForceX[i];
// 		precY += arraySumForceY[i];
// 	}
// 
// 	double precX2 = 0.0;
// 	double precY2 = 0.0;
// 	for (size_t i = 0; i<vectNode.size(); i++) { 
// 		precX2 += floor(fabs(arraySumForceX[i]));
// 		precY2 += floor(fabs(arraySumForceY[i]));
// 	}
// 
// 	if ((old_precX == precX && old_precY == precY) || (precX == precision && precY == precision ) /*|| (isnan(precX) || isnan(precY) )*/)
// 		end = true;
// 
// 	old_precX = precX;
// 	old_precY = precY;
// 
// 	Refresh();
// 	Update();
// 
// 	return end;
// 
// }


static double old_precX = 0.0;
static double old_precY = 0.0;

bool GraphFrame::OptimizeDrawing(double precision) {

	double x1, y1, x2, y2 = 0.0;
	double x2Minx1, y2Miny1 = 0.0;	
	double x2Minx1DivDist, y2Miny1DivDist = 0.0;

	double distance, forceX, forceNodeX, forceY, forceNodeY, force = 0.0;

	double arrayForceX[vectNode.size()][vectNode.size()];
	double arrayForceY[vectNode.size()][vectNode.size()];
	double arraySumForceX[vectNode.size()];
	double arraySumForceY[vectNode.size()];

	bool end = false;

	
	//Calculate forces
	for (size_t i = 0; i<vectNode.size(); i++) {
		double X = 0;
		double Y = 0;
		x1 = vectNode[i]->GetX();
		y1 = vectNode[i]->GetY();

		for (size_t j = 0; j<vectNode.size(); j++) {
			if (i != j) {
				x2 = vectNode[j]->GetX();
				y2 = vectNode[j]->GetY();

				x2Minx1 = x2 - x1;
				y2Miny1 = y2 - y1;
				distance = sqrt( (x2Minx1*x2Minx1) + (y2Miny1*y2Miny1) );//Euclidean distance

				double forceEdgeX = 0;
				double forceEdgeY = 0;
				double forceNodeX = 0;
				double forceNodeY = 0;

				if (distance != NaturalLength && distance > 0) {
					x2Minx1DivDist = x2Minx1 / distance;
					y2Miny1DivDist = y2Miny1 / distance;

					for (size_t n = 0; n < vectEdge.size(); n++) {
						if (vectEdge[n]->Get_numN1() == vectNode[i]->Get_num() && vectEdge[n]->Get_numN2() == vectNode[j]->Get_num()) {
							forceEdgeX += ( EdgeStiffness * (distance - NaturalLength) ) * x2Minx1DivDist;
							forceEdgeY += ( EdgeStiffness * (distance - NaturalLength) ) * y2Miny1DivDist;
						}
					}
				}


				if (distance > 0) { // to avoid division per 0
					forceNodeX = ( NodeStrenght / ( distance*distance ) ) * x2Minx1DivDist;
					forceNodeY = ( NodeStrenght / ( distance*distance ) ) * y2Miny1DivDist;
				}
				X += forceEdgeX;
				Y += forceEdgeY;
				forceX = forceEdgeX + forceNodeX;
				forceY = forceEdgeY + forceNodeY;

				arrayForceX[i][j] = forceX;
				arrayForceY[i][j] = forceY;

			}
		}
	}

	for (size_t i = 0; i<vectNode.size(); i++) {
		arraySumForceX[i] = 0.0;
		arraySumForceY[i] = 0.0;
	}

	//Sum forces for each nodes
	for (size_t i = 0; i<vectNode.size(); i++) {
		for (size_t j = 0; j<vectNode.size(); j++) {
			arraySumForceX[i] += arrayForceX[i][j];
			arraySumForceY[i] += arrayForceY[i][j];
		}
	}

	//Replace the nodes & edges according to their new position
	for (size_t i = 0; i<vectNode.size(); i++) {
		double newX = 0;
		double newY = 0;
		newX = vectNode[i]->GetX() + arraySumForceX[i];
		newY = vectNode[i]->GetY() + arraySumForceY[i];
		
		//Check whether positions are outside of the window
		if (newX > sz.GetWidth())
			newX = sz.GetWidth() - CIRCLE_RADIUS;
		if (newX < CIRCLE_RADIUS)
			newX = 0 + CIRCLE_RADIUS;
		if (newY > sz.GetHeight())
			newY = sz.GetHeight() - CIRCLE_RADIUS;
		if (newY < CIRCLE_RADIUS)
			newY = 0 + CIRCLE_RADIUS;

		vectNode[i]->SetXY( newX , newY );
		
		for (size_t n = 0; n < vectEdge.size(); n++) {
			if (vectEdge[n]->Get_numN1() == vectNode[i]->Get_num()) {
				vectEdge[n]->Set_pos1(vectNode[i]->GetX(),vectNode[i]->GetY());
			}
			if (vectEdge[n]->Get_numN2() == vectNode[i]->Get_num()) {
				vectEdge[n]->Set_pos2(vectNode[i]->GetX(),vectNode[i]->GetY());
			}
		}
	}

	//Calculate the precision of the drawing
	double precX = 0.0;
	double precY = 0.0;

	for (size_t i = 0; i<vectNode.size(); i++) { 
		precX += arraySumForceX[i];
		precY += arraySumForceY[i];
	}

	double precX2 = 0.0;
	double precY2 = 0.0;
	for (size_t i = 0; i<vectNode.size(); i++) { 
		precX2 += floor(fabs(arraySumForceX[i]));
		precY2 += floor(fabs(arraySumForceY[i]));
	}

	if ((old_precX == precX && old_precY == precY) || (precX == precision && precY == precision ) /*|| (isnan(precX) || isnan(precY) )*/)
		end = true;

	old_precX = precX;
	old_precY = precY;

	Refresh();
	Update();

	return end;

}

static int ind_node_dragged = -1;

int GraphFrame::FindNode(wxPoint pt) {
	int ind_node_dragged_tmp = -1;
	for (size_t n = 0; n < vectNode.size(); n++) {
		if (vectNode[n]->GetX() > pt.x-CIRCLE_RADIUS && vectNode[n]->GetX() < pt.x+CIRCLE_RADIUS) {
			if (vectNode[n]->GetY() > pt.y-CIRCLE_RADIUS && vectNode[n]->GetY() < pt.y+CIRCLE_RADIUS) {
				ind_node_dragged_tmp = n;
			}
		}
	}
	return ind_node_dragged_tmp;
}

void GraphFrame::PressLeft(wxMouseEvent& event) {

	wxPoint pt_start = event.GetPosition();
	//Identify the node concerned by Left click
	ind_node_dragged = FindNode(pt_start);

}

void GraphFrame::Drag(wxMouseEvent& event) {
	
	if(event.Dragging() && !event.Moving() && !event.Entering() && !event.Leaving()) {
		if (ind_node_dragged != -1) { //if num == -1 : no node selected
			wxPoint pt_end = event.GetPosition();//Find the destination 
			if (pt_end.x > CIRCLE_RADIUS && pt_end.x < sz.GetWidth()-CIRCLE_RADIUS  && pt_end.y > CIRCLE_RADIUS && pt_end.y < sz.GetHeight()-CIRCLE_RADIUS) {
				vectNode[ind_node_dragged]->SetXY(pt_end.x,pt_end.y);//redefine node coord
				//find Edges and change coord
				for (size_t n = 0; n < vectEdge.size(); n++) {
					if (vectEdge[n]->Get_numN1() == vectNode[ind_node_dragged]->Get_num()) {
						vectEdge[n]->Set_pos1(pt_end.x,pt_end.y);
					}
					if (vectEdge[n]->Get_numN2() == vectNode[ind_node_dragged]->Get_num()) {
						vectEdge[n]->Set_pos2(pt_end.x,pt_end.y);
					}
				}
				//Redraw
				Refresh();
				Update();
			}
		}
	}
}

void GraphFrame::ReleaseLeft(wxMouseEvent& event) {
	ind_node_dragged = -1;
}		

void GraphFrame::PressRight(wxMouseEvent& event) {

	wxPoint pt_fix = event.GetPosition();

	//Find the node concerned by the fixing
	int numFix = FindNode(pt_fix);

	if (numFix != -1) {
		//Fix Nodes & Edges
		if (vectNode[numFix]->IsLocked()) {
			vectNode[numFix]->Unlock();
			for (size_t n = 0; n < vectEdge.size(); n++) {
				if (vectEdge[n]->Get_numN1() == vectNode[numFix]->Get_num()) {
					vectEdge[n]->Unlock1();
				}
				if (vectEdge[n]->Get_numN2() == vectNode[numFix]->Get_num()) {
					vectEdge[n]->Unlock2();
				}
			}
		}
		else {
			vectNode[numFix]->Lock();
			for (size_t n = 0; n < vectEdge.size(); n++) {
				if (vectEdge[n]->Get_numN1() == vectNode[numFix]->Get_num()) {
					vectEdge[n]->Lock1();
				}
				if (vectEdge[n]->Get_numN2() == vectNode[numFix]->Get_num()) {
					vectEdge[n]->Lock2();
				}
			}
		}
		Refresh();
		Update();
	}

}
