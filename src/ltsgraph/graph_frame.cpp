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
  EVT_MENU(ID_OPTIMIZE, GraphFrame::OnOptimize)
  EVT_MENU(ID_STOP_OPTIMIZE, GraphFrame::OnStopOptimize)
  EVT_MENU(ID_SET_EDGE_STIF, GraphFrame::OnSetEdgeStiffness)
  EVT_MENU(ID_SET_NODE_STRE, GraphFrame::OnSetNodeStrength)
  EVT_MENU(ID_SET_NATU_LENG, GraphFrame::OnSetNaturalLength)
  EVT_SIZE(GraphFrame::OnResize)
END_EVENT_TABLE()

static vector<Node*> vectNode;
static vector<Edge*> vectEdge;

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
  StopOpti = false;

  EdgeStiffness = 1.0; 
  NodeStrength = 1000.0; 
  // value below is reset later when the number of nodes is known.
  NaturalLength = 20.0;

  CreateMenu();

}

void GraphFrame::CreateMenu() {

    menu = new wxMenuBar;
    
  //file
  file = new wxMenu;
  openItem = file->Append( wxID_OPEN, wxT("&Open...\tCTRL-o"), wxT("") );
  quitItem = file->Append( wxID_EXIT, wxT("&Quit\tCTRL-q"), wxT("") );
  menu->Append( file, wxT("&File") );

  //draw
  draw = new wxMenu;
  optimizeGraph = draw->Append(ID_OPTIMIZE, wxT("&Optimize Graph\tCTRL-g"), wxT("") );
  stopOptimize = draw->Append(ID_STOP_OPTIMIZE, wxT("&Stop Optimize Graph\tCTRL-s"), wxT("") );
  menu->Append(draw, wxT("&Draw") );

  //options 
  options = new wxMenu;
  setEdgeStiffnessItem = options->Append( ID_SET_EDGE_STIF, wxT("Set &Transition Attracting Force"), wxT("") );
  setNodeStrengthItem  = options->Append( ID_SET_NODE_STRE, wxT("Set &State Repulsion"), wxT("") );
  setNaturalLengthItem = options->Append( ID_SET_NATU_LENG, wxT("Set Natural Transition &Length"), wxT("") );
  menu->Append( options, wxT("&Options") );

  SetMenuBar( menu );
}

//Resizing Graph
void GraphFrame::OnResize(wxSizeEvent& event) {
  wxSize sz2 = GetClientSize();

  double diff_x = (double) sz2.GetWidth() / (double) sz.GetWidth();
  double diff_y = (double) sz2.GetHeight() / (double) sz.GetHeight();

  for (size_t m = 0; m < vectNode.size(); m++) 
  { vectNode[m]->SetXY(vectNode[m]->GetX() * diff_x, 
                       vectNode[m]->GetY() * diff_y);
  }

  Refresh();

  sz = sz2;
}


void GraphFrame::OnOpen( wxCommandEvent& /* event */ ) {
	wxFileDialog dialog( this, wxT("Select a LTS file..."), wxT(""), wxT(""), wxT("*.aut |*.aut|*.svc|*.svc|All files|*"));
	if ( dialog.ShowModal() == wxID_OK ) {
		StopOpti = true;
		vectEdge.clear();
		vectNode.clear();
		Init(dialog.GetPath());
		Refresh();
	}
}

void GraphFrame::OnQuit( wxCommandEvent& /* event */ ) {
	StopOpti = true;
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

void GraphFrame::OnSetNodeStrength( wxCommandEvent& /* event */ ) {
  wxString InitValue;
  InitValue.sprintf(wxT("%f"), NodeStrength);
  wxTextEntryDialog * sns = new wxTextEntryDialog(this,wxT(""), wxT("Node Strength"), InitValue,wxOK | wxCANCEL | wxCENTRE, wxDefaultPosition);
  sns->ShowModal();
  wxString StrValue = sns->GetValue();
  StrValue.ToDouble(&NodeStrength);
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

	while (!OptimizeDrawing(0.0) && !StopOpti) {
		wxTheApp->Yield(true); // to allow user to interrupt optimizing
	}
	StopOpti = false;
}

void GraphFrame::OnStopOptimize( wxCommandEvent& /* event */ ) {
	StopOpti = true;
}

//init vectNode & vectEdge
void GraphFrame::Init(wxString LTSfile) {

  string st_LTSfile = string(LTSfile.fn_str());

    //read lts file
  lts mylts;
    if (mylts.read_from(st_LTSfile)) 
    {
      //initialize vectNode

      state_iterator si = mylts.get_states();
        
      wxString tmp;
      int randX = sz.GetWidth()  - 2*CIRCLE_RADIUS;
      int randY = sz.GetHeight() - 4*CIRCLE_RADIUS;    
      for(unsigned int i=0; si.more(); i++) 
      {
        tmp.sprintf(wxT("%d"), *si);
        wxString * Slbl_Node = new wxString(tmp);
        vectNode.push_back(
             new Node(*si, GenRandom(randX), GenRandom(randY), *Slbl_Node));
        ++ si;
        delete Slbl_Node;
      }   
      NaturalLength = (400 / vectNode.size())+8;
    
    
      //initialize vectEdge

      transition_iterator ti = mylts.get_transitions();
    
      for(unsigned int i=0; (ti.more()); i++) 
      {
        wxString * Slbl_Edge = new wxString(ATwriteToString(mylts.label_value(ti.label())), wxConvLocal);
  
        for (size_t n = 0; n < vectNode.size(); n++) 
        { if (vectNode[n]->Get_num() == ti.from()) 
          { for (size_t m = 0; m < vectNode.size(); m++) 
            { if (vectNode[m]->Get_num() == ti.to()) 
              { vectEdge.push_back(new Edge(vectNode[n],vectNode[m],*Slbl_Edge));
              }
            }
          }
        }
    
        delete Slbl_Edge;       
        ++ ti;
      }

      Refresh();
    }
    else 
    {
      cout << "Error : cannot read " << st_LTSfile << endl;
      exit(0);
    }
}

bool GraphFrame::OptimizeDrawing(double precision) 
{

  double arraySumForceX[vectNode.size()];
  double arraySumForceY[vectNode.size()];

  double WindowWidth = (double)sz.GetWidth();
  double WindowHeight = (double)sz.GetHeight();

  // Reset the forces to 0, to begin with.
  for (size_t i = 0; i<vectNode.size(); i++) {
    arraySumForceX[i]=0.0;
    arraySumForceY[i]=0.0;
  }
  
  //Calculate forces
  for (size_t i = 0; i<vectNode.size(); i++) 
  { double x1 = vectNode[i]->GetX();
    double y1 = vectNode[i]->GetY();

    // First calculate the repulsing force for node i with respect to
    // the other nodes, and cumulate it in <arraySumForceX[i],arraySumForceY[i];
    for (size_t j = 0; j<vectNode.size(); j++) 
    {
      if (i != j) 
      { double x2 = vectNode[j]->GetX();
        double y2 = vectNode[j]->GetY();
        double x2Minx1 = x1 - x2;
        double y2Miny1 = y1 - y2;
        
        //Euclidean distance
        double distance = sqrt( (x2Minx1*x2Minx1) + (y2Miny1*y2Miny1) );

        if (distance > 1) 
        {
          // below the force is divided by the vectNode.size()^2 to
          // compensate for the fact that for all nodes a force
          // is summed. Otherwise the forces would be extremely big.
          double s = NodeStrength / (distance * distance * distance);
          
          arraySumForceX[i] += s * x2Minx1;
          arraySumForceY[i] += s * y2Miny1;
        }
        else 
        { // If the nodes are on top of each other, they must have
          // a repulsing force, away from each other. The direction
          // is determined by the node number. We arbitrarily take
          // 1 as a repulsing force.

          if (i>j)
          { arraySumForceX[i] += CIRCLE_RADIUS / 2;
            arraySumForceY[i] += CIRCLE_RADIUS / 2;
          }
          else
          { arraySumForceX[i] += -CIRCLE_RADIUS / 2;
            arraySumForceY[i] += -CIRCLE_RADIUS / 2;
          }
        }
      }
    }

    // Subsequently calculate the attracting forces of the edges.
    for (size_t n = 0; n < vectEdge.size(); n++) 
    { 
      bool calculate=false;
      double x2=0.0,y2=0.0;
      if (vectEdge[n]->Get_N1() == vectNode[i] &&
          vectEdge[n]->Get_N1()!=vectEdge[n]->Get_N2())
      { // x2 = vectEdge[n]->GetXpos2();
        // y2 = vectEdge[n]->GetYpos2();
        x2 = (vectEdge[n]->Get_N2())->GetX();
        y2 = (vectEdge[n]->Get_N2())->GetY();
        calculate=true;
      }
      else
      if (vectEdge[n]->Get_N2() == vectNode[i] &&
          vectEdge[n]->Get_N1()!=vectEdge[n]->Get_N2())
      { // x2 = vectEdge[n]->GetXpos1();
        // y2 = vectEdge[n]->GetYpos1();
        x2 = (vectEdge[n]->Get_N1())->GetX();
        y2 = (vectEdge[n]->Get_N1())->GetY();
        calculate=true;
      }
        
      if (calculate)
      {
        double x2Minx1 = x2 - x1;
        double y2Miny1 = y2 - y1;
        double distance = sqrt( (x2Minx1*x2Minx1) + (y2Miny1*y2Miny1) );
  
        if (distance>0.1)
        { 
          // below the force is divided by the number of edges, as
          // otherwise the cumulative force can become excessively
          // big for transition systems with many vectors.
          // double s = (EdgeStiffness * (distance - NaturalLength)) / distance;
          double s = (EdgeStiffness * log(distance / NaturalLength)) / distance;

          arraySumForceX[i] += s * x2Minx1;
          arraySumForceY[i] += s * y2Miny1;
        }
      }
    }
    // Finally add a tiny center petal force to center the whole
    // graph on the screen
    
    arraySumForceX[i]+=(WindowWidth-2*x1) / (1 * WindowWidth); 
    arraySumForceY[i]+=(WindowHeight-2*y1) /(1 * WindowHeight); 
  }

  //Replace the nodes & edges according to their new position
  for (size_t i = 0; i<vectNode.size(); i++) 
  { double newX = 0;
    double newY = 0;
    // cerr << "force on node " << i << " is (" << arraySumForceX[i]
    //     << "," << arraySumForceY[i] << ")\n";
    newX = vectNode[i]->GetX() + arraySumForceX[i];
    newY = vectNode[i]->GetY() + arraySumForceY[i];
    
    //Check whether positions are outside of the window
    if (newX + CIRCLE_RADIUS  > sz.GetWidth())
        newX = sz.GetWidth() - CIRCLE_RADIUS ;
    if (newX < CIRCLE_RADIUS)
        newX = 0 + CIRCLE_RADIUS ;
    if (newY + CIRCLE_RADIUS > sz.GetHeight())
        newY = sz.GetHeight() - CIRCLE_RADIUS ;
    if (newY < CIRCLE_RADIUS)
        newY = 0 + CIRCLE_RADIUS;

    vectNode[i]->SetXY( newX , newY );
  }
    

  //Calculate the just achieved precision of the drawing
  double achieved_precision = 0.0;

  for (size_t i = 0; i<vectNode.size(); i++) 
  { 
    achieved_precision += fabs(arraySumForceX[i])+fabs(arraySumForceY[i]);
  }
  
  // compensate for the number of nodes
  achieved_precision=achieved_precision / vectNode.size();

  Refresh();

  return achieved_precision<precision;

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
        
        //Redraw
        Refresh();
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

  if (numFix != -1) 
  {
    //Fix Node
    if (vectNode[numFix]->IsLocked()) 
    { vectNode[numFix]->Unlock();
    }
    else 
    { vectNode[numFix]->Lock();
    }
    Refresh();
  }

}
