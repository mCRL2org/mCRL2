// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file state.cpp
/// \brief Add your file description here.

#include "state.h"
using namespace std;
using namespace Utils;

State::State() {
  cluster = NULL;
  rank = 0;
  positionRadius = 0.0f;
  positionAngle = -1.0f;
  marked = false;
  visitState = DFS_WHITE;
  id = 0;
  simulated = false;
  selected = false;
  zoomLevel = 0;
}

State::~State() {
}

void State::addSuperior( State* s )
{
  superiors.insert( s );
}

void State::addSubordinate( State* s )
{
  subordinates.insert( s );
}

void State::addComrade( State* s )
{
  comrades.insert( s );
}

void State::addInTransition( Transition* trans )
{
  inTransitions.push_back( trans );
}

void State::addOutTransition( Transition* trans )
{
  outTransitions.push_back( trans );
}

void State::addLoop( Transition* trans )
{
  loops.push_back( trans );
}

void State::addParameterValue(int valindex) {
	stateVector.push_back(valindex);
}

bool State::isDeadlock() const
{
  return ( outTransitions.size() + loops.size() == 0 );
}

bool State::isMarked() const
{
  return marked;
}

void State::mark()
{
  marked = true;
}

void State::unmark()
{
  marked = false;
}

bool State::isSelected() const
{
  return selected;
}

void State::select()
{
  selected = true;
}

void State::deselect()
{
  selected = false;
}


int State::getID() {
	return id;
}

void State::setID(int i) {
	id = i;
}

int State::getRank() const
{
  return rank;
}

void State::setRank( int r )
{
  rank = r;
}

bool State::isCentered() const {
  return positionAngle < -0.9f;
}

void State::center() {
  positionRadius = 0.0f;
  positionAngle = -1.0f;
}

float State::getPositionAngle() const {
  return positionAngle;
}

float State::getPositionRadius() const {
  return positionRadius;
}

Point3D State::getPositionAbs() const {
  return positionAbs;
}

Point3D State::getOutgoingControl() const {
  return outgoingControl;
}

Point3D State::getIncomingControl() const {
  return incomingControl;
}

void State::setPosition(float r,float a) {
  positionRadius = r;
  positionAngle = a;
}

void State::setPositionAbs(Point3D p) {
  positionAbs = p;
}

void State::setOutgoingControl(Point3D p) 
{
  outgoingControl = p;
}

void State::setIncomingControl(Point3D p) 
{
  incomingControl = p;
}

Cluster* State::getCluster() const
{
  return cluster;
}

void State::setCluster( Cluster* c )
{
  cluster = c;
}

std::set<State*>::iterator State::getSubordinatesBegin() const {
  return subordinates.begin();
}

std::set<State*>::iterator State::getSubordinatesEnd() const {
  return subordinates.end();
}

std::set<State*>::iterator State::getSuperiorsBegin() const {
  return superiors.begin();
}

std::set<State*>::iterator State::getSuperiorsEnd() const {
  return superiors.end();
}

void State::getComrades( set< State* > &ss ) const
{
  ss = comrades;
}

void State::getInTransitions( vector< Transition* > &ts ) const
{
  ts = inTransitions;
}

Transition* State::getInTransition( int i ) const
{
  return inTransitions[i];
}

int State::getNumInTransitions() const
{
  return inTransitions.size();
}

void State::getOutTransitions( vector< Transition* > &ts ) const
{
  ts = outTransitions;
}

Transition* State::getOutTransitioni( int i ) const
{
  return outTransitions[i];
}

int State::getNumOutTransitions( ) const 
{
  return outTransitions.size();
}

void State::getLoops( vector< Transition* > & ls ) const
{
  ls = loops;
}

Transition* State::getLoopi(int i) const 
{
  return loops[i];
}

int State::getNumberOfLoops() const
{
  return loops.size();
}
void State::clearHierarchyInfo()
{
  superiors.clear();
  subordinates.clear();
  comrades.clear();
}
void State::DFSfinish() {
  visitState = DFS_BLACK;
}

void State::DFSclear() {
  visitState = DFS_WHITE;
}

void State::DFSvisit() {
  visitState = DFS_GREY;
}


DFSState State::getVisitState() const {
  return visitState;
}

int State::getParameterValue(int parindex) {
  return stateVector[parindex];
}

void State::setSimulated(bool simulated)
{
  this->simulated = simulated;
}

bool State::isSimulated() const
{
  return simulated;
}

int State::getZoomLevel() const
{
  return zoomLevel;
}


void State::setZoomLevel(const int level)
{
  zoomLevel = level;
}

