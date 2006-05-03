#include "GraphFrame.h" 

BEGIN_EVENT_TABLE(GraphFrame, wxFrame)
    EVT_PAINT(GraphFrame::OnPaint)
	EVT_LEFT_DOWN(GraphFrame::PressLeft)
    EVT_MOTION(GraphFrame::Drag)
	EVT_LEFT_UP(GraphFrame::ReleaseLeft)
	EVT_RIGHT_UP(GraphFrame::PressRight)
	EVT_MENU(wxID_OPEN, GraphFrame::OnOpen)
	EVT_SIZE(GraphFrame::OnResize)
END_EVENT_TABLE()


int GenRandom(const int &max) {
    return static_cast <int> (rand()%max+CIRCLE_RADIUS); 
}

GraphFrame::GraphFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxFrame(NULL, -1, title, pos, size, style) {

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
 
    #ifdef MCRL2_BCG
        BCG_INIT();
    #endif
	sz = GetClientSize();

	CreateMenu();

}

void GraphFrame::CreateMenu() {

    menu = new wxMenuBar;
    file = new wxMenu;

    openitem = file->Append( wxID_OPEN, wxT("&Open...	CTRL-o"), wxT("") );

	menu->Append( file, wxT("&File") );

	SetMenuBar( menu );
}

//Rezising Graph
void GraphFrame::OnResize(wxSizeEvent& event) {

	
	wxSize sz2 = GetClientSize();

	double diff_x = (double) sz2.GetWidth() / (double) sz.GetWidth();
	double diff_y = (double) sz2.GetHeight() / (double) sz.GetHeight();

	wxCoord x;
	wxCoord y;
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

void GraphFrame::OnPaint(wxPaintEvent& evt) {
    
    wxPaintDC myDC(this);

    //Call Edge and Node OnPaint() method (Edge 1st)
    
    for (size_t n = 0; n < vectEdge.size(); n++) {
        vectEdge[n]->OnPaint(&myDC);
    }
      
    for (size_t n = 0; n < vectNode.size(); n++) {
        vectNode[n]->OnPaint(&myDC);
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
        wxPoint * p;
        
        wxString tmp;
		int randX = sz.GetWidth()  - 2*CIRCLE_RADIUS;
		int randY = sz.GetHeight() - 4*CIRCLE_RADIUS;		

        while (si.more()) {
            tmp.sprintf(wxT("%d"), *si);
            wxString * Slbl_Node = new wxString(tmp);
            p = new wxPoint(GenRandom(randX),GenRandom(randY));
            vectNode.push_back(new Node(*si, *p, *Slbl_Node));
            ++ si;
            delete p;
            delete Slbl_Node;
        }   
    
    
        //initialize vectEdge

        transition_iterator ti = mylts.get_transitions();
        wxPoint * pe1;
        wxPoint * pe2;
    
        //AT_writeToStringBuffer should return a string at most 99 char
        int i;
        while (ti.more()) {
            char buffer[100];
			for (int i=0;i<100;i++)
				buffer[i] = '\0';
            AT_writeToStringBuffer(mylts.label_value(ti.label()),buffer);
            wxString * Slbl_Edge = new wxString(buffer, wxConvLocal);

            for (size_t n = 0; n < vectNode.size(); n++) {
                if (vectNode[n]->Get_num() == ti.from()) {
                    for (size_t m = 0; m < vectNode.size(); m++) {
                        if (vectNode[m]->Get_num() == ti.to()) {
                            pe1 = new wxPoint(vectNode[n]->GetX(), vectNode[n]->GetY());
                            pe2 = new wxPoint(vectNode[m]->GetX(), vectNode[m]->GetY());
                            vectEdge.push_back(new Edge(ti.from(),ti.to(),*pe1, *pe2, *Slbl_Edge));
                            delete pe1;
                            delete pe2; 
                        }
                    }
                }
            }
        
            delete Slbl_Edge;       
            ++ ti;
        }

    }
    else {
        cout << "Error : cannot read " << st_LTSfile << endl;
		exit(0);
	}
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
