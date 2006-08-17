#include "graph_frame.h" 
const wxColour border_colour_selected = "BLUE";

BEGIN_EVENT_TABLE(GraphFrame, wxFrame)
  EVT_MENU(wxID_OPEN, GraphFrame::OnOpen)
	EVT_MENU(ID_EXPORT_PS, GraphFrame::ExportPostScript)
	EVT_MENU(ID_EXPORT_LATEX, GraphFrame::ExportLatex)
	EVT_MENU(ID_BACKUP_CREATE, GraphFrame::CreateBackup)
  EVT_MENU(wxID_EXIT, GraphFrame::OnQuit)
  EVT_MENU(ID_OPTIMIZE, GraphFrame::OnOptimize)
  EVT_MENU(ID_STOP_OPTIMIZE, GraphFrame::OnStopOptimize)
	EVT_CLOSE(GraphFrame::OnClose)
	EVT_CHECKBOX(ID_CHECK_NODE, GraphFrame::OnCheckNode)
	EVT_CHECKBOX(ID_CHECK_EDGE, GraphFrame::OnCheckEdge)
	EVT_BUTTON(ID_BUTTON_OPTI, GraphFrame::OnBtnOpti)
	EVT_BUTTON(ID_BUTTON_COLOUR, GraphFrame::on_btn_pick_colour)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ViewPort, wxPanel)
  EVT_PAINT(ViewPort::OnPaint)
  EVT_LEFT_DOWN(ViewPort::PressLeft)
  EVT_MOTION(ViewPort::Drag)
  EVT_LEFT_UP(ViewPort::ReleaseLeft)
  EVT_RIGHT_UP(ViewPort::PressRight)
  EVT_SIZE(ViewPort::OnResize)
END_EVENT_TABLE()

static vector<Node*> vectNode;
static vector<Edge*> vectEdge;
static int CircleRadius;

double GenRandom(const int &max) {
    return static_cast <double> (rand()%max+CircleRadius);
}

GraphFrame::GraphFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
  : wxFrame(NULL, -1, title, pos, size, style) {

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
 
    #ifdef MCRL2_BCG
        BCG_INIT();
    #endif

  StopOpti = true;
  StoppedOpti = true;


  // values below are reset later when the right panel is setuped
  EdgeStiffness = 1.0; 
  NodeStrength = 1000.0; 
  NaturalLength = 20.0;
  CircleRadius = 10;

  BuildLayout();

}

void GraphFrame::BuildLayout() {

	//Menu
  CreateMenu();

	//Status bar
	CreateStatusBar();

	int rightPanelWidth = INITIAL_WIN_WIDTH - (INITIAL_WIN_WIDTH / 4 + 15);

	wxSplitterWindow * sw = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & !wxSP_PERMIT_UNSPLIT);
	
  leftPanel = new ViewPort( sw, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );
  rightPanel = new wxPanel( sw, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );

	sw->SetMinimumPaneSize(10);
	sw->SplitVertically(leftPanel,rightPanel);
	sw->SetSashPosition(rightPanelWidth,true);
	sw->SetSashGravity(1.0);

	wxBoxSizer * winSizer = new wxBoxSizer(wxHORIZONTAL);
	winSizer->Add(sw,1,wxEXPAND);


	wxBoxSizer * rightSizer = new  wxBoxSizer(wxVERTICAL);
	rightPanel->SetSizer(rightSizer);

// setup the top part (information box)
	wxStaticBoxSizer* infoSizer = new wxStaticBoxSizer( wxVERTICAL, rightPanel, wxT("Information") );
  int lflags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int rflags = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;
  
  wxFlexGridSizer* topRightSizer = new wxFlexGridSizer( 4, 2, 0, 0 );
  initialStateLabel = new wxStaticText( rightPanel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfStatesLabel = new wxStaticText( rightPanel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfTransitionsLabel = new wxStaticText( rightPanel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfLabelsLabel = new wxStaticText( rightPanel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );

  topRightSizer->Add( new wxStaticText( rightPanel, wxID_ANY,	wxT("Initial state:         ") ), 0, lflags, 4 );
  topRightSizer->Add( initialStateLabel, 0, rflags, 3 );
  topRightSizer->Add( new wxStaticText( rightPanel, wxID_ANY, wxT("Number of states:      ") ), 0, lflags, 4 );
  topRightSizer->Add( numberOfStatesLabel, 0, rflags, 3 );
  topRightSizer->Add( new wxStaticText( rightPanel, wxID_ANY,	wxT("Number of transitions: ") ), 0, lflags, 4 );
  topRightSizer->Add( numberOfTransitionsLabel, 0, rflags, 3 );
  topRightSizer->Add( new wxStaticText( rightPanel, wxID_ANY,	wxT("Number of labels:      ") ), 0, lflags, 4 );
  topRightSizer->Add( numberOfLabelsLabel, 0, rflags, 3 );

	infoSizer->Add(topRightSizer, 1, wxEXPAND | wxALL, 0 );
  rightSizer->Add(infoSizer, 0, wxEXPAND | wxALL, 0 );

	rightSizer->AddSpacer(20);

// setup the middle part (algorithm settings box)
	wxStaticBoxSizer* algoSettingsSizer = new wxStaticBoxSizer( wxVERTICAL, rightPanel, wxT("Algorithm settings") );
	wxFlexGridSizer* middleRightSizer = new wxFlexGridSizer( 3, 2, 0, 0 );
	
	wxSize spinSize(1,1);
	spinNodeStrength  = new wxSpinCtrlFloat(rightPanel, wxID_ANY, 100.0, 90000.0, 200.0, 1000.0,wxDefaultPosition,spinSize);
	spinEdgeStiffness = new wxSpinCtrlFloat(rightPanel, wxID_ANY, 0.0, 15.0, 0.1, 1.0,wxDefaultPosition,spinSize);
	spinNaturalLength = new wxSpinCtrlFloat(rightPanel, wxID_ANY, 2.0, 900.0, 1.0, 20.0,wxDefaultPosition,spinSize);

	middleRightSizer->Add( new wxStaticText( rightPanel, wxID_ANY,	wxT("State repulsion") ), 0, lflags, 4 );
	middleRightSizer->Add(spinNodeStrength, 0, rflags, 3 );
	middleRightSizer->Add( new wxStaticText( rightPanel, wxID_ANY,	wxT("Transition attracting force") ), 0, lflags, 4 );
	middleRightSizer->Add(spinEdgeStiffness, 0, rflags, 3 );
	middleRightSizer->Add( new wxStaticText( rightPanel, wxID_ANY,	wxT("Natural transition length") ), 0, lflags, 4 );
	middleRightSizer->Add(spinNaturalLength, 0, rflags, 3 );

	algoSettingsSizer->Add(middleRightSizer, 1, wxEXPAND | wxALL, 0 );
	rightSizer->Add(algoSettingsSizer, 0, wxEXPAND | wxALL, 0 );

	rightSizer->AddSpacer(20);

// setup the bottom part (others settings box)
	wxStaticBoxSizer* othersSettingsSizer = new wxStaticBoxSizer( wxVERTICAL, rightPanel, wxT("Others settings") );

	ckNodeLabels = new wxCheckBox(rightPanel, ID_CHECK_NODE, wxT("Display state's labels"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	ckNodeLabels->SetValue(true);
	ckEdgeLabels = new wxCheckBox(rightPanel, ID_CHECK_EDGE, wxT("Display transition's labels"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	ckEdgeLabels->SetValue(true);

	othersSettingsSizer->Add(ckNodeLabels, 0, lflags, 4 );
	othersSettingsSizer->Add(ckEdgeLabels, 0, lflags, 4 );

	wxFlexGridSizer* bottomRightSizer = new wxFlexGridSizer( 1, 2, 0, 0 );
	spinNodeRadius = new wxSpinCtrl(rightPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 2, 50, 10);
	bottomRightSizer->Add( new wxStaticText( rightPanel, wxID_ANY,	wxT("State radius") ), 0, lflags, 4 );
	bottomRightSizer->Add(spinNodeRadius, 0, rflags, 3 );
	othersSettingsSizer->Add(bottomRightSizer, 1, wxEXPAND | wxALL, 0 );

	// Button for opening colour picker for selected node.
	btn_pick_colour = new wxButton(rightPanel, ID_BUTTON_COLOUR, wxT("Edit node &colour"), wxDefaultPosition, wxDefaultSize);
	btn_pick_colour->Enable(false);
        othersSettingsSizer->Add(btn_pick_colour, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 4);
	
	rightSizer->Add(othersSettingsSizer, 0, wxEXPAND | wxALL, 0 );

	rightSizer->AddSpacer(20);

	btnOptiStop = new wxButton(rightPanel, ID_BUTTON_OPTI, wxT("&Optimize"), wxDefaultPosition, wxDefaultSize);
	btnOptiStop->Enable(false);

	rightSizer->Add(btnOptiStop, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 4 );

  SetSizer(winSizer);

	Layout();
	sw->UpdateSize();

}

void GraphFrame::CreateMenu() {

  menu = new wxMenuBar;
    
  //file
  file = new wxMenu;
  openItem        = file->Append( wxID_OPEN, wxT("&Open...\tCTRL-o"), wxT("") );
	exports = new wxMenu;
	file->Append(wxID_ANY,wxT("&Export"),exports);
	exportPsItem    = exports->Append( ID_EXPORT_PS, wxT("Export to &PostScript\tCTRL-p"), wxT("") );
	exportLatexItem = exports->Append( ID_EXPORT_LATEX, wxT("Export to &Latex\tCTRL-l"), wxT("") );
	backupCreate    = file->Append( ID_BACKUP_CREATE, wxT("&Create a backup\tCTRL-c"), wxT("") );
  quitItem        = file->Append( wxID_EXIT, wxT("&Quit\tCTRL-q"), wxT("") );
  menu->Append( file, wxT("&File") );

  //draw
  draw = new wxMenu;
  optimizeGraph = draw->Append(ID_OPTIMIZE, wxT("&Optimize Graph\tCTRL-g"), wxT("") );
  stopOptimize = draw->Append(ID_STOP_OPTIMIZE, wxT("&Stop Optimize Graph\tCTRL-s"), wxT("") );
  menu->Append(draw, wxT("&Draw") );


  SetMenuBar( menu );

	optimizeGraph->Enable(false);
	stopOptimize->Enable(false);
	exportLatexItem->Enable(false);
	exportPsItem->Enable(false);
	backupCreate->Enable(false);
}

void GraphFrame::CreateStatusBar() {

	statusBar = new wxStatusBar(this, wxID_ANY, wxST_SIZEGRIP);
	statusBar->SetFieldsCount(3);
 	int StatusBar_Widths[3];
 	StatusBar_Widths[0] = 200;
	StatusBar_Widths[1] = 200;
	StatusBar_Widths[2] = -1;
	statusBar->SetStatusWidths(3,StatusBar_Widths);
	this->SetStatusBar(statusBar);
}

void GraphFrame::OnOpen( wxCommandEvent& /* event */ ) {
	StopOpti    = true;
	StoppedOpti = true;
	btnOptiStop->SetLabel(wxT("&Optimize"));
	wxFileDialog dialog( this, wxT("Select a LTS file (.aut .svc) or a backup file (.ltsgraph)..."), wxT(""), wxT(""), 
											wxT("*.aut |*.aut|*.svc|*.svc|*.ltsgraph|*.ltsgraph|All files|*"));
	if ( dialog.ShowModal() == wxID_OK ) {
		vectEdge.clear();
		vectNode.clear();
		Init(dialog.GetPath());
		Refresh();
	}
}

void GraphFrame::OnQuit( wxCommandEvent& /* event */ ) {
  Close( TRUE );
}

void GraphFrame::OnClose(wxCloseEvent& /*event*/) {
  StopOpti = true;

//std::cerr << "closeing" << std::endl;
//  while(!StoppedOpti) {
//    wxTheApp->Yield(true); // to allow user to interrupt optimizing
//  }
  SetEvtHandlerEnabled(false);

  static_cast < wxSplitterWindow* > (leftPanel->GetParent())->Unsplit(leftPanel);
  delete leftPanel;
  Destroy();
}

void GraphFrame::OnOptimize( wxCommandEvent& /* event */ ) {

	optimizeGraph->Enable(false);
	stopOptimize->Enable(true);
	btnOptiStop->SetLabel(wxT("&Stop    "));
	StopOpti = false;
	StoppedOpti = false;
	while (!OptimizeDrawing(0.0) && !StopOpti) {
//std::cerr << "running'";
		wxTheApp->Yield(true); // to allow user to interrupt optimizing
	}

	StoppedOpti = true;
//std::cerr << "end'";
}

void GraphFrame::OnStopOptimize( wxCommandEvent& /* event */ ) {
	stopOptimize->Enable(false);
	optimizeGraph->Enable(true);
	btnOptiStop->SetLabel(wxT("&Optimize"));
	StopOpti = true;
}

void GraphFrame::OnCheckNode( wxCommandEvent& /* event */ ) {

	if (ckNodeLabels->IsChecked()) {
		for (size_t i = 0; i < vectNode.size(); i++)  
			vectNode[i]->ShowLabels();
	}
	else {
		for (size_t i = 0; i < vectNode.size(); i++)  
			vectNode[i]->HideLabels();
	}
	Refresh();

}

void GraphFrame::OnCheckEdge( wxCommandEvent& /* event */ ) {

	if (ckEdgeLabels->IsChecked()) {
		for (size_t i = 0; i < vectEdge.size(); i++)  
			vectEdge[i]->ShowLabels();
	}
	else {
		for (size_t i = 0; i < vectEdge.size(); i++)  
			vectEdge[i]->HideLabels();
	}
	Refresh();
}

void GraphFrame::OnBtnOpti( wxCommandEvent& event ) {
	if(StopOpti) {
		OnOptimize(event);
	}
	else {
		OnStopOptimize(event);
	}

}

void GraphFrame::on_btn_pick_colour( wxCommandEvent& /* event */ ) {
  wxColourData colour_picker_data;

  // Allow Windows users to pick full range of colours. (No effect on other
  // platforms
  colour_picker_data.SetChooseFull(true);

  // Get colour of the currently selected node.
  wxColour current_colour = leftPanel->get_selected_node()->get_node_colour(); 
  colour_picker_data.SetColour(current_colour);
  
  // Create colour picker dialog, and show it to the user
  wxColourDialog colour_picker(this, &colour_picker_data);

  if (colour_picker.ShowModal() == wxID_OK) {
    wxColourData returned_data = colour_picker.GetColourData();
    wxColour new_colour = returned_data.GetColour();
    leftPanel->get_selected_node()->set_node_colour(new_colour);
    leftPanel->Refresh();
  }
  
}
//init vectNode & vectEdge
void GraphFrame::Init(wxString LTSfile) {

  	//leftPanel->sz = leftPanel->GetSize();

  	string st_LTSfile = string(LTSfile.fn_str());

		//Find extension
		string ext = st_LTSfile.substr(st_LTSfile.find_last_of( '.' )+1);

		//initialize file name
		inputFileName = st_LTSfile.substr( st_LTSfile.find_last_of( '/' ) + 1 ); //without path
		inputFileName = inputFileName.substr( 0, inputFileName.find_last_of( '.' ) ); //without extension

		if (ext != "ltsgraph") {

			//read lts file
			lts mylts;
			if (mylts.read_from(st_LTSfile)) 
			{
	
				//Information about graph
				wxString text;
				text.Printf(wxT("%u"),mylts.initial_state());
				initialStateLabel->SetLabel(text);
				text.Printf(wxT("%u"),mylts.num_states());
				numberOfStatesLabel->SetLabel(text);
				text.Printf(wxT("%u"),mylts.num_transitions());
				numberOfTransitionsLabel->SetLabel(text);
				text.Printf(wxT("%u"),mylts.num_labels());
				numberOfLabelsLabel->SetLabel(text);
	
				//Fill status bar
				FillStatusBar(LTSfile,2);	
	
				//initialize vectNode
	
				state_iterator si = mylts.get_states();
					
				wxString tmp;
				int randX = leftPanel->Get_Width()  - 2*CircleRadius;
				int randY = leftPanel->Get_Height() - 4*CircleRadius;
	
	
				for(unsigned int i=0; si.more(); i++) 
				{
						tmp.sprintf(wxT("%d"), *si);
						wxString * Slbl_Node = new wxString(tmp);

						vectNode.push_back( new Node(*si, GenRandom(randX), GenRandom(randY), *Slbl_Node, (mylts.initial_state() == *si) ));

						delete Slbl_Node;
					++ si;
				}   
				//NaturalLength = (400 / vectNode.size())+8;
			
				//initialize vectEdge
	
				transition_iterator ti = mylts.get_transitions();
			
				for(unsigned int i=0; (ti.more()); i++) 
				{
					wxString * Slbl_Edge = new wxString(ATwriteToString(mylts.label_value(ti.label())), wxConvLocal);
	
					for (size_t n = 0; n < vectNode.size(); n++) { 
						if (vectNode[n]->Get_num() == ti.from()) { 
							for (size_t m = 0; m < vectNode.size(); m++) { 
									if (vectNode[m]->Get_num() == ti.to()) 
										vectEdge.push_back(new Edge(vectNode[n],vectNode[m],*Slbl_Edge));
							}
						}
						//Init circle radius
						vectNode[n]->SetRadius(CircleRadius);
					}
			
					delete Slbl_Edge;       
					++ ti;
				}
				optimizeGraph->Enable(true);
				stopOptimize->Enable(false);
				btnOptiStop->Enable(true);
				exportLatexItem->Enable(true);
				exportPsItem->Enable(true);
				backupCreate->Enable(true);
				Refresh();
			}
			else 
			{
				cout << "Error : cannot read " << st_LTSfile << endl;
				exit(0);
			}
		}
		else { //restore a backup
			
			RestoreBackup();
 
		}
}

bool GraphFrame::OptimizeDrawing(double precision) {

  EdgeStiffness = spinEdgeStiffness->GetValue();
  NodeStrength  = spinNodeStrength->GetValue();
  NaturalLength = spinNaturalLength->GetValue();
	CircleRadius  = spinNodeRadius->GetValue();
	
  double arraySumForceX[vectNode.size()];
  double arraySumForceY[vectNode.size()];

  double WindowWidth  = (double)leftPanel->Get_Width();
  double WindowHeight = (double)leftPanel->Get_Height();

  // Reset the forces to 0, to begin with.
	// Set the nodes' radius value
  for (size_t i = 0; i<vectNode.size(); i++) {
    arraySumForceX[i]=0.0;
    arraySumForceY[i]=0.0;
		vectNode[i]->SetRadius(CircleRadius);
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
          { arraySumForceX[i] += CircleRadius / 2;
            arraySumForceY[i] += CircleRadius / 2;
          }
          else
          { arraySumForceX[i] += -CircleRadius / 2;
            arraySumForceY[i] += -CircleRadius / 2;
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
      { 
        x2 = (vectEdge[n]->Get_N2())->GetX();
        y2 = (vectEdge[n]->Get_N2())->GetY();
        calculate=true;
      }
      else
      if (vectEdge[n]->Get_N2() == vectNode[i] &&
          vectEdge[n]->Get_N1()!=vectEdge[n]->Get_N2())
      { 
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
					// Linear approach : 
          // double s = (EdgeStiffness * (distance - NaturalLength)) / distance;
					// Logarithmic approach : 
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

    newX = vectNode[i]->GetX() + arraySumForceX[i];
    newY = vectNode[i]->GetY() + arraySumForceY[i];
    
    
    //Check whether positions are outside of the window
    //Use outsideWindow to monitor this result.
    bool outsideWindow = false;

    if (newX + CircleRadius  > leftPanel->Get_Width()) {
        newX = leftPanel->Get_Width() - CircleRadius ;
        outsideWindow = true;
    }
    if (newX < CircleRadius) {
        newX = 0 + CircleRadius ;
        outsideWindow = true;
    }

    if (newY + CircleRadius > leftPanel->Get_Height()) {
        newY = leftPanel->Get_Height() - CircleRadius ;
        outsideWindow = true;
    }

    if (newY < CircleRadius) {
        newY = 0 + CircleRadius;
        outsideWindow = true;
    }
    
    if (outsideWindow) {
      vectNode[i]->ForceSetXY( newX , newY);
      // Force the position of the node, a pin can go off-screen. 
    }
    else {
      vectNode[i]->SetXY( newX , newY );
    }
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

void GraphFrame::Draw(wxPaintDC * myDC) {

		//fix a bug (the size status text disappeared)
 		wxSize size = wxSize(leftPanel->Get_Width(), leftPanel->Get_Height());
 		FillStatusBar(GetInfoWinSize(size),0);

    //Call Edge and Node OnPaint() method (Edge 1st)
    for (size_t n = 0; n < vectEdge.size(); n++) {
        vectEdge[n]->OnPaint(myDC);
    }
      
    for (size_t n = 0; n < vectNode.size(); n++) {
        vectNode[n]->OnPaint(myDC);
    }
}

void GraphFrame::ExportPostScript( wxCommandEvent& /* event */ ) {
	

		string str(inputFileName);
		str.append(".ps");
		wxString wx_str(str.c_str(), wxConvLocal);

		wxPrintData pd;
		pd.SetFilename(wx_str);
    pd.SetPrintMode(wxPRINT_MODE_FILE);

    wxPostScriptDC myDC(pd);
		myDC.StartDoc(wx_str);


		//fix a bug (the size status text disappeared)
 		wxSize size = wxSize(leftPanel->Get_Width(), leftPanel->Get_Height());
 		FillStatusBar(GetInfoWinSize(size),0);

    //Call Edge and Node OnPaint() method (Edge 1st)
    for (size_t n = 0; n < vectEdge.size(); n++) {
        vectEdge[n]->OnPaint(&myDC);
    }
      
    for (size_t n = 0; n < vectNode.size(); n++) {
        vectNode[n]->OnPaint(&myDC);
    }

		myDC.EndDoc();

		wxMessageBox(wxT("Export finished"),wxT("Information"),wxOK| wxICON_INFORMATION);

}

void GraphFrame::ExportLatex( wxCommandEvent& /* event */ ) {

	vector<nodeLatex> vectNodeLatex;
	vector<edgeLatex> vectEdgeLatex;

	nodeLatex StructNodeLatex;
	edgeLatex StructEdgeLatex;

	for (size_t n = 0; n < vectNode.size(); n++) {
			StructNodeLatex.num = vectNode[n]->Get_num();
			StructNodeLatex.x   = vectNode[n]->GetX();
			StructNodeLatex.y   = vectNode[n]->GetY();
			StructNodeLatex.lbl = vectNode[n]->Get_lbl();

			vectNodeLatex.push_back(StructNodeLatex);
	}


	for (size_t n = 0; n < vectEdge.size(); n++) {
			StructEdgeLatex.numNode1 = vectEdge[n]->Get_N1()->Get_num();
			StructEdgeLatex.numNode2 = vectEdge[n]->Get_N2()->Get_num();
			StructEdgeLatex.lbl      = vectEdge[n]->Get_lbl();

			vectEdgeLatex.push_back(StructEdgeLatex);
	}
	
 	ExportToLatex * ltx = new ExportToLatex(strcat((char *)inputFileName.c_str(),".tex"),vectNodeLatex,vectEdgeLatex,leftPanel->Get_Height());
 	if (ltx->Generate()) {
		wxMessageBox(wxT("Export successful"),wxT("Information"),wxOK| wxICON_INFORMATION);
	}
	else {
		wxMessageBox(wxT("Export unsuccessful"),wxT("Error"),wxOK | wxICON_ERROR);
	}

}

void GraphFrame::CreateBackup(wxCommandEvent& event) {

	LtsgraphBackup bckp(leftPanel->GetSize());

	bckp.SetLayout(vectNode, vectEdge);
	bckp.SetInformation(initialStateLabel->GetLabel(), 
											numberOfStatesLabel->GetLabel(), 
											numberOfTransitionsLabel->GetLabel(), 
											numberOfLabelsLabel->GetLabel()
											);
	bckp.SetAlgoSettings(spinEdgeStiffness->GetValue(), spinNodeStrength->GetValue(), spinNaturalLength->GetValue());
	bckp.SetOtherSettings(spinNodeRadius->GetValue(), ckNodeLabels->IsChecked(), ckEdgeLabels->IsChecked());

	if (bckp.Backup(inputFileName)) {
		wxMessageBox(wxT("Backup successful"),wxT("Information"),wxOK| wxICON_INFORMATION);
	}
	else {
		wxMessageBox(wxT("Backup unsuccessful"),wxT("Error"),wxOK | wxICON_ERROR);
	}
		
				
}

void GraphFrame::RestoreBackup() {

	LtsgraphBackup bckp(leftPanel->GetSize());
	if (bckp.Restore(inputFileName)) {

		FillStatusBar(wxT(""),2);		

		initialStateLabel->SetLabel(bckp.GetInitialState());  
		numberOfStatesLabel->SetLabel(bckp.GetNumStates()); 
		numberOfTransitionsLabel->SetLabel(bckp.GetNumTransitions()); 
		numberOfLabelsLabel->SetLabel(bckp.GetNumLabels()); 

		spinEdgeStiffness->SetValue(bckp.GetEdgeStiffness());
		spinNodeStrength->SetValue(bckp.GetNodeStrength());
		spinNaturalLength->SetValue(bckp.GetNaturalLength());
	
		spinNodeRadius->SetValue(bckp.GetStateRadius());
		ckNodeLabels->SetValue(bckp.GetStateLabel());
		ckEdgeLabels->SetValue(bckp.GetTransitionLabel());

		vectEdge.clear();
		vectNode.clear();
		vectEdge = bckp.GetVectEdge();
		vectNode = bckp.GetVectNode();
		
		
		optimizeGraph->Enable(true);
		stopOptimize->Enable(false);
		btnOptiStop->Enable(true);

	}
	else
		wxMessageBox(wxT("Restore unsuccessful"),wxT("Error"),wxOK | wxICON_ERROR);
	
	
	Refresh();


}

void GraphFrame::Resize(wxSize sz2) {

	double diff_x = (double) sz2.GetWidth() / (double) leftPanel->Get_Width();
  double diff_y = (double) sz2.GetHeight() / (double) leftPanel->Get_Height();

  for (size_t m = 0; m < vectNode.size(); m++) 
  { vectNode[m]->ForceSetXY(vectNode[m]->GetX() * diff_x, 
                       vectNode[m]->GetY() * diff_y);
    // We use ForceSetXY here, otherwise the pin can fall out of the window.
  }

	FillStatusBar(GetInfoWinSize(sz2),0);

}

int GraphFrame::FindNode(wxPoint pt) {
  int ind_node_dragged_tmp = -1;
  for (size_t n = 0; n < vectNode.size(); n++) {
    if (vectNode[n]->GetX() > pt.x-CircleRadius && vectNode[n]->GetX() < pt.x+CircleRadius) {
      if (vectNode[n]->GetY() > pt.y-CircleRadius && vectNode[n]->GetY() < pt.y+CircleRadius) {
        ind_node_dragged_tmp = n;
      }
    }
  }
  return ind_node_dragged_tmp;
}

static int ind_node_dragged = -1;

void GraphFrame::ReplaceAfterDrag(wxPoint pt) {
	vectNode[ind_node_dragged]->ForceSetXY(pt.x,pt.y);//redefine node coord
}

void GraphFrame::FixNode(int num) {

    if (vectNode[num]->IsLocked()) { 
			vectNode[num]->Unlock();
    }
    else { 
			vectNode[num]->Lock();
    }
}

void GraphFrame::FillStatusBar(const wxString text, unsigned int no) {
	statusBar->SetStatusText(text,no);
}

wxString GraphFrame::GetInfoCurrentNode(int num) const {

	wxString text;

	text.Printf(wxT("Current node: %u  ( %u , %u )"), 
							vectNode[num]->Get_num(),
							(unsigned int) round(vectNode[num]->GetX()),
							(unsigned int) round(vectNode[num]->GetY()));
	
	return  text;

}

wxString GraphFrame::GetInfoWinSize(wxSize sz2) const {

	wxString text;

	text.Printf(wxT("Window size: %d * %d"), sz2.GetWidth(), sz2.GetHeight());
	
	return  text;

}

void GraphFrame::enable_btn_colour_picker() {
  btn_pick_colour->Enable(true);
}

void GraphFrame::disable_btn_colour_picker() {
  btn_pick_colour->Enable(false);
}

////////////////////////////////VIEWPORT CLASS IMPLEMENTATION////////////////////////////////

ViewPort::ViewPort(wxWindow * parent, const wxPoint& pos, const wxSize& size, long style) 
  : wxPanel(parent, wxID_ANY, pos, size, style) { 

  GF = static_cast<GraphFrame*>(GetParent()->GetParent());
  selected_node = NULL;

}

Node * ViewPort::get_selected_node() {
  return selected_node;
}

void ViewPort::OnPaint(wxPaintEvent& /* event */) {
		sz = GetClientSize();

    //Draw the graph
		wxPaintDC myDC(this);
		GF->Draw(&myDC);

}

//Resizing Graph
void ViewPort::OnResize(wxSizeEvent& event) {
  wxSize sz2 = GetClientSize();

	GF->Resize(sz2); 
	
	sz = sz2;

	Refresh();

}

void ViewPort::PressLeft(wxMouseEvent& event) {

  wxPoint pt_start = event.GetPosition();
  //Identify the node concerned by Left click
  ind_node_dragged = GF->FindNode(pt_start);

  if (ind_node_dragged != -1) {
    if (selected_node) {
      // Reset border of formerly selected node
      selected_node->reset_border_colour();
    }
    //Store pointer of selected_node
    selected_node = vectNode[ind_node_dragged];
    
    // Give its border a color to identify it on-screen
    selected_node->set_border_colour(border_colour_selected);

    // Activate button for colour picking:
    GF->enable_btn_colour_picker();
  }
  else {
    if (selected_node) {  
      // Reset border colour of formerly selected node
      selected_node->reset_border_colour();

      // Reset pointer to selected node
      selected_node = NULL;

      // Disable colour picking button
      GF->disable_btn_colour_picker();
    }
  }
  FillStatusBar();
  Refresh();
}

void ViewPort::Drag(wxMouseEvent& event) {

  if(event.Dragging() && !event.Moving() && !event.Entering() && !event.Leaving()) {
    if (ind_node_dragged != -1) { //if num == -1 : no node selected
      wxPoint pt_end = event.GetPosition();//Find the destination 
      if (pt_end.x > CircleRadius && pt_end.x < sz.GetWidth()-CircleRadius  && pt_end.y > CircleRadius && pt_end.y < sz.GetHeight()-CircleRadius) {
        GF->ReplaceAfterDrag(pt_end);
        Refresh();
      }
    }
  }
}

void ViewPort::ReleaseLeft(wxMouseEvent& event) {
  ind_node_dragged = -1;
}    

void ViewPort::PressRight(wxMouseEvent& event) {

  wxPoint pt_fix = event.GetPosition();

  //Find the node concerned by the fixing
  int numFix = GF->FindNode(pt_fix);

  if (numFix != -1) {
		GF->FixNode(numFix);
    Refresh();
  }

}

void ViewPort::FillStatusBar() {
	//Fill current node status bar
	if (ind_node_dragged != -1) {
		GF->FillStatusBar(GF->GetInfoCurrentNode(ind_node_dragged),1);
	}
	else {
		GF->FillStatusBar(wxT(""),1);
  }
}

int ViewPort::Get_Width() {
	return sz.GetWidth();
}

int ViewPort::Get_Height() {
	return sz.GetHeight();
}
