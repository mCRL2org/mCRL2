#include "simulation.h"

using namespace std;

Simulation::Simulation()
{
  //currState = NULL;
  started=false;
  chosenTrans = -1;
}
  
void Simulation::start(State* initialState) {
  stateHis.push_back(initialState);
  currState = initialState;

  vector< Transition* > selfLoops;
  
  initialState->getOutTransitions(posTrans);
  initialState->getLoops(selfLoops);

  size_t totalSize = posTrans.size() + selfLoops.size();
  posTrans.reserve(totalSize);

  posTrans.insert< vector<Transition*>::iterator > (posTrans.end(), 
                                                    selfLoops.begin(), 
                                                    selfLoops.end());

  chosenTrans = -1;
  
  started = true;
  //Fire signal
  signal();
}
void Simulation::stop()
{
  // Set started to false
  started = false;

  // Clear history
  stateHis.clear();
  transHis.clear();
  // Fire signal
  signal();
  
}
  
Simulation::~Simulation() {
  //Stop simulation
  stop();

  transHis.clear();
  stateHis.clear();
  posTrans.clear();
}

vector< Transition* > const& Simulation::getTransHis() const {
  return transHis;
}

vector< State* > const& Simulation::getStateHis() const {
  return stateHis;
}

State* Simulation::getCurrState() const {
  return currState;
}

vector< Transition* > const& Simulation::getPosTrans() const {
  return posTrans;
}

Transition* Simulation::getChosenTrans() const {
  return posTrans[chosenTrans];
}

int Simulation::getChosenTransi() const {
  return chosenTrans;
}
bool Simulation::getStarted() const {
  return started;
}

void Simulation::followTrans() {
  Transition* toFollow = posTrans[chosenTrans];
  State* nextState = toFollow->getEndState();
  vector< Transition* > selfLoops;
  
  transHis.push_back(posTrans[chosenTrans]);
  stateHis.push_back(nextState);
  currState = nextState;
  
  nextState->getOutTransitions(posTrans);
  nextState->getLoops(selfLoops);

  size_t totalSize = posTrans.size() + selfLoops.size();
  posTrans.reserve(totalSize);

  posTrans.insert< vector<Transition*>::iterator > (posTrans.end(), 
                                                    selfLoops.begin(),
                                                    selfLoops.end());
  chosenTrans = -1;

  //Fire signal
  signal();
}

void Simulation::chooseTrans(int i) {
  chosenTrans = i;

  // Fire signal
  selChangeSignal();
}

void Simulation::undoStep() {
  State* lastState;
  vector< Transition* > selfLoops;
  
  // Remove last transition, state from history
  transHis.pop_back();
  stateHis.pop_back();

  // Set new states
  lastState = stateHis.back();
  currState = lastState;
  lastState->getOutTransitions(posTrans);
  lastState->getLoops(selfLoops);

  size_t totalSize = posTrans.size() + selfLoops.size();
  posTrans.reserve(totalSize);

  posTrans.insert< vector<Transition*>::iterator > ( posTrans.end(),
                                                     selfLoops.begin(),
                                                     selfLoops.end()); 
  
  chosenTrans = -1;

  // Fire signal
  signal();
}


void Simulation::resetSim() {
  State* firstState = stateHis.front();
  vector<Transition*> selfLoops;
  
  transHis.clear();
  stateHis.clear();
  stateHis.push_back(firstState);
  
  currState = firstState;

  firstState->getLoops(selfLoops);
  firstState->getOutTransitions(posTrans);

  size_t totalSize = posTrans.size() + selfLoops.size();
  posTrans.reserve(totalSize);

  posTrans.insert< vector<Transition*>::iterator > (posTrans.end(),
                                                    selfLoops.begin(),
                                                    selfLoops.end());
  chosenTrans = -1;

  // Fire signal
  signal();
}

Simulation::simConnection Simulation::connectSel(
              simulationSignal::slot_function_type subscriber
            )
{
  simConnection result = selChangeSignal.connect(subscriber);

  // Send acknowledgement to subscriber;
  selChangeSignal();

  return result;
}

Simulation::simConnection Simulation::connect( 
              simulationSignal::slot_function_type subscriber
            )
{
  simConnection result = signal.connect(subscriber);
  // Send acknowledge to subscriber
  signal();
  // return value
  return result;
}

void Simulation::disconnect(Simulation::simConnection subscriber)
{
  subscriber.disconnect();
}

