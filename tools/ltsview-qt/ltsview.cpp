// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsview.cpp
/// \brief Contains implementation of the LTSView application.

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QWidget>



#include "wx.hpp" // precompiled headers

#define NAME "ltsview"
#define AUTHOR "Bas Ploeger and Carst Tankink"

#include "ltsview.h"
#include <string>
#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/image.h>
#include "mcrl2/lts/lts.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/logger.h"
#include "cluster.h"
#include "glcanvas.h"
#include "lts.h"
#include "mcrl2/lts/lts_io.h"
#include "mainframe.h"
#include "markmanager.h"
#include "settings.h"
#include "state.h"
#include "visualizer.h"

using namespace std;

static
std::vector< std::string > developers()
{
  static char const* developer_names[] = {"Bas Ploeger", "Carst Tankink"};
  return std::vector< std::string >(&developer_names[0], &developer_names[2]);
}


LTSView::LTSView() : super("LTSView",
                             "3D interactive visualization of a labelled transition system", // what-is
                             "Tool for interactive visualization of state transition systems.\n" // gui-specific description
                             "\n"
                             "LTSView is based on visualization techniques by Frank van Ham and Jack van Wijk.\n"
                             "See: F. van Ham, H. van de Wetering and J.J. van Wijk,\n"
                             "\"Visualization of State Transition Graphs\". "
                             "Proceedings of the IEEE Symposium on Information Visualization 2001. IEEE CS Press, pp. 59-66, 2001.\n"
                             "\n"
                             "The default colour scheme for state marking was obtained from http://www.colorbrewer.org",
                             "Start the LTSView application. If INFILE is supplied then the " // command-line description
                             "LTS in INFILE is loaded into the application.\n"
                             "\n"
                             "The input format is determined by the contents of INFILE. If that fails, "
                             "an attempt is made to force the input format based on the file extension. "
                             "The supported formats with file extensions are:\n"
                             "  Aldebaran format (CADP; *.aut);\n"
#ifdef USE_BCG
                             "  Binary Coded Graph format (CADP; *.bcg);\n"
#endif
                             "  GraphViz format (*.dot);\n"
                             "  Finite State Machine format (*.fsm);\n"
                             "  mCRL SVC format (*.svc);\n"
                             "  mCRL2 format (*.lts).",
                             developers()), lts(0)
{ }

IMPLEMENT_APP_NO_MAIN(LTSView_gui_tool)
IMPLEMENT_WX_THEME_SUPPORT
BEGIN_EVENT_TABLE(LTSView, wxApp)
END_EVENT_TABLE()

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,
                              HINSTANCE hPrevInstance,
                              wxCmdLineArgType lpCmdLine,
                              int nCmdShow)
{

  MCRL2_ATERMPP_INIT(0, lpCmdLine)

  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  QApplication app(argc, argv);

  return wxEntry(argc, argv);
}
#endif

class LogMessenger : public mcrl2::log::output_policy
{
  public:
    LogMessenger(QWidget *parent): m_parent(parent) {}
    void output(const mcrl2::log::log_level_t level, const std::string& hint, const time_t timestamp, const std::string& msg)
    {
      if (level == mcrl2::log::error)
      {
        QMessageBox::critical(m_parent, "LTSView - An error occured", QString::fromStdString(msg));
      }
    }
  private:
    QWidget *m_parent;
};


bool LTSView::run()
{
  // TODO: s/0/this/
  log = new LogMessenger(0);
  mcrl2::log::logger::register_output_policy(*log);
  settings = new Settings();
  connect(&settings->stateRankStyleCyclic, SIGNAL(changed(bool)), this, SLOT(setRankStyle()));
  connect(&settings->fsmStyle, SIGNAL(changed(bool)), this, SLOT(setFSMStyle()));
  connect(&settings->statePosStyleMultiPass, SIGNAL(changed(bool)), this, SLOT(setStatePosStyle()));
  connect(&settings->clusterVisStyleTubes, SIGNAL(changed(bool)), this, SLOT(setVisStyle()));
  markManager = new MarkManager();
  connect(markManager, SIGNAL(changed()), this, SLOT(applyMarkStyle()));
  mainFrame = new MainFrame(this, settings, markManager);
  visualizer = new Visualizer(settings, markManager);
  glCanvas = mainFrame->getGLCanvas();
  glCanvas->setVisualizer(visualizer);

  SetTopWindow(mainFrame);
  mainFrame->Show(true);
  glCanvas->initialize();
  mainFrame->Layout();

  wxInitAllImageHandlers();

  if (!input_filename().empty())
  {
    wxFileName fileName(wxString(input_filename().c_str(), wxConvLocal));
    fileName.Normalize();
    mainFrame->setFileInfo(fileName);
    openFile(static_cast< string >(fileName.GetFullPath().mb_str(wxConvUTF8)));
  }
  return true;
}

LTSView::~LTSView()
{
  if (lts != 0)
  {
    delete lts;
    delete settings;
    delete visualizer;
    delete markManager;
  }
  mcrl2::log::logger::unregister_output_policy(*log);
  delete log;
}

std::string LTSView::getVersionString()
{
  return mcrl2::utilities::interface_description("", NAME, AUTHOR, "",
         "", "").version_information();
}

void LTSView::openFile(string fileName)
{
  try
  {
    glCanvas->disableDisplay();

    mainFrame->createProgressDialog("Opening file","Loading file");
    mainFrame->updateProgressDialog(0,"Loading file");
    LTS* newlts = new LTS();
    if (!newlts->readFromFile(fileName))
    {
      delete newlts;
      mainFrame->updateProgressDialog(100,"Error loading file");
      mainFrame->showMessage(
        "Error loading file",
        "Could not load file " + fileName);
      return;
    }
    delete lts;
    lts = newlts;
    connect(lts->getSimulation(), SIGNAL(started()), this, SLOT(simulationStarted()));

    mainFrame->updateProgressDialog(17,"Ranking states");
    lts->rankStates(settings->stateRankStyleCyclic.value());

    mainFrame->updateProgressDialog(33,"Clustering states");
    lts->clusterStates(settings->stateRankStyleCyclic.value());

    mainFrame->updateProgressDialog(50,"Setting cluster info");
    lts->computeClusterInfo();

    mainFrame->updateProgressDialog(67,"Positioning clusters");
    lts->positionClusters(settings->fsmStyle.value());

    markManager->setLTS(lts,true);
    visualizer->setLTS(lts,true);

    mainFrame->updateProgressDialog(83,"Positioning states");
    lts->positionStates(settings->statePosStyleMultiPass.value());

    mainFrame->updateProgressDialog(100,"Done");
    visualizer->markStyleChanged();

    glCanvas->enableDisplay();
    glCanvas->resetView();

    mainFrame->loadTitle();
    mainFrame->setNumberInfo(lts->getNumStates(),
                             lts->getNumTransitions(),lts->getNumClusters(),
                             lts->getNumRanks());
    QStringList parameters;
    for (unsigned int i = 0; i < lts->getNumParameters(); ++i)
    {
      parameters += QString::fromStdString(lts->getParameterName(i));
    }
    mainFrame->setParameterNames(parameters);
    mainFrame->resetMarkRules();

    vector< string > ls;
    lts->getActionLabels(ls);

    mainFrame->setSim(lts->getSimulation());
    mainFrame->setActionLabels(ls);

    glCanvas->setSim(lts->getSimulation());

    mainFrame->setMarkedStatesInfo(0);
    mainFrame->setMarkedTransitionsInfo(0);
  }
  catch (mcrl2::runtime_error e)
  {

    ostringstream s; //(captureOutput);

    wxString output;//(s.str(), wxConvUTF8);
    wxMessageDialog* dial = new wxMessageDialog(NULL,
        output + wxT("\n") +
        wxString(e.what(), wxConvUTF8), wxT("Error"),  wxOK | wxICON_ERROR);
    dial->ShowModal();
    mainFrame->updateProgressDialog(100,"Done");
  }
}

void LTSView::setRankStyle()
{
  if (lts != NULL)
  {
    zoomOutTillTop();
    glCanvas->disableDisplay();

    mainFrame->createProgressDialog("Structuring LTS","Applying ranking");

    mainFrame->updateProgressDialog(17,"Ranking states");
    lts->rankStates(settings->stateRankStyleCyclic.value());

    mainFrame->updateProgressDialog(33,"Clustering states");
    lts->clusterStates(settings->stateRankStyleCyclic.value());

    mainFrame->updateProgressDialog(40,"Setting cluster info");
    lts->computeClusterInfo();
    markManager->markClusters();

    mainFrame->updateProgressDialog(60,"Positioning clusters");
    lts->positionClusters(settings->fsmStyle.value());

    visualizer->setLTS(lts,true);

    mainFrame->updateProgressDialog(80,"Positioning states");
    lts->positionStates(settings->statePosStyleMultiPass.value());

    mainFrame->updateProgressDialog(100,"Done");

    glCanvas->enableDisplay();
    glCanvas->resetView();

    mainFrame->setNumberInfo(lts->getNumStates(),
                             lts->getNumTransitions(),lts->getNumClusters(),
                             lts->getNumRanks());
  }
}

void LTSView::setStatePosStyle()
{
  if (lts != NULL)
  {
    lts->positionStates(settings->statePosStyleMultiPass.value());
    visualizer->dirtyPositions();
    glCanvas->display();
  }
}

void LTSView::setVisStyle()
{
  visualizer->visualizationStyleChanged();
  glCanvas->display();
}

void LTSView::setFSMStyle()
{
  if (lts != NULL)
  {
    glCanvas->disableDisplay();
    lts->positionClusters(settings->fsmStyle.value());
    visualizer->setLTS(lts, true);
    glCanvas->enableDisplay();
    glCanvas->resetView();
  }
}

void LTSView::applyMarkStyle()
{
  if (lts == NULL)
  {
    return;
  }

  switch (markManager->getMarkStyle())
  {
    case MARK_DEADLOCKS:
      mainFrame->setMarkedStatesInfo(lts->getNumDeadlocks());
      mainFrame->setMarkedTransitionsInfo(0);
      break;
    case MARK_STATES:
      mainFrame->setMarkedStatesInfo(markManager->getNumMarkedStates());
      mainFrame->setMarkedTransitionsInfo(0);
      break;
    case MARK_TRANSITIONS:
      mainFrame->setMarkedStatesInfo(0);
      mainFrame->setMarkedTransitionsInfo(
        markManager->getNumMarkedTransitions());
      break;
    case NO_MARKS:
    default:
      mainFrame->setMarkedStatesInfo(0);
      mainFrame->setMarkedTransitionsInfo(0);
      break;
  }
  visualizer->dirtyColors();
  glCanvas->display();
}

void LTSView::notifyRenderingStarted()
{
  mainFrame->startRendering();
}

void LTSView::notifyRenderingFinished()
{
  mainFrame->stopRendering();
}


void LTSView::simulationStarted()
{
  // Select/deselect initial state of simulation, to initialize state info pane.
  selectStateByID(lts->getSimulation()->getCurrState()->getID());
}


void LTSView::selectStateByID(const int id)
{
  if (lts != NULL)
  {
    State* s = lts->selectStateByID(id);
    lts->getSimulation()->setInitialState(s);
    for (unsigned int i = 0; i < lts->getNumParameters(); ++i)
    {
      mainFrame->setParameterValue(i, QString::fromStdString(lts->getStateParameterValueStr(s,i)));
    }
  }
}

void LTSView::selectCluster(const int rank, const int pos)
{
  if (lts != NULL)
  {
    Cluster* c = lts->selectCluster(rank, pos);
    mainFrame->setStatesInCluster(c->getNumStates());
    for (unsigned int i = 0; i < lts->getNumParameters(); ++i)
    {
      std::set<std::string> values = lts->getClusterParameterValues(c,i);
      std::set<std::string>::iterator val_it;
      QStringList val;
      for (val_it = values.begin(); val_it != values.end(); ++val_it)
      {
        // val.push_back(lts->prettyPrintParameterValue(*val_it));
        val += QString::fromStdString(*val_it);
      }
      mainFrame->setParameterValues(i, val);
    }
  }
}

void LTSView::deselect()
{
  if (lts != NULL)
  {
    lts->deselect();

    mainFrame->resetParameterValues();
  }
}

int LTSView::getNumberOfObjects()
{
  int result = 0;

  if (lts != NULL)
  {
    result += lts->getNumClusters();
    result += lts->getNumStates();
  }

  return result;
}

void LTSView::zoomInBelow()
{
  LTS* newLTS = lts->zoomIntoBelow();
  deselect();
  lts = newLTS;
  markManager->setLTS(lts,false);
  visualizer->setLTS(lts,false);
  mainFrame->setNumberInfo(lts->getNumStates(),
                           lts->getNumTransitions(),lts->getNumClusters(),
                           lts->getNumRanks());
  applyMarkStyle();
  glCanvas->setSim(lts->getSimulation());
  mainFrame->setSim(lts->getSimulation());
}

void LTSView::zoomInAbove()
{
  LTS* newLTS = lts->zoomIntoAbove();
  deselect();
  lts = newLTS;
  markManager->setLTS(lts,false);
  visualizer->setLTS(lts,false);
  mainFrame->setNumberInfo(lts->getNumStates(),
                           lts->getNumTransitions(),lts->getNumClusters(),
                           lts->getNumRanks());
  applyMarkStyle();
  glCanvas->setSim(lts->getSimulation());
  mainFrame->setSim(lts->getSimulation());
}

void LTSView::zoomOutTillTop()
{
  LTS* oldLTS = lts;
  do
  {
    zoomOut();
    oldLTS = lts;
  }
  while (lts != oldLTS);
}

void LTSView::zoomOut()
{
  LTS* oldLTS = lts;
  lts = oldLTS->zoomOut();
  oldLTS->deselect();
  markManager->setLTS(lts,false);
  visualizer->setLTS(lts,false);
  mainFrame->setNumberInfo(lts->getNumStates(),
                           lts->getNumTransitions(),lts->getNumClusters(),
                           lts->getNumRanks());
  applyMarkStyle();
  glCanvas->setSim(lts->getSimulation());
  mainFrame->setSim(lts->getSimulation());

  if (oldLTS != lts)
  {
    delete oldLTS;
  }
}

void LTSView::loadTrace(std::string const& path)
{
  lts->loadTrace(path);
  glCanvas->setSim(lts->getSimulation());
  mainFrame->setSim(lts->getSimulation());
}

void LTSView::exportToText(std::string filename)
{
  visualizer->exportToText(filename);
}
