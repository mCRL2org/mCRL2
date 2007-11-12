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
  id = 0;
  simulated = false;
  selected = false;
  zoomLevel = 0;
}

State::~State() {
  unsigned int i;
  for (i = 0; i < outTransitions.size(); ++i) {
    delete outTransitions[i];
  }
  for (i = 0; i < loops.size(); ++i) {
    delete loops[i];
  }
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
  return (positionAngle < -0.9f);
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

void State::setPositionRadius(float r) {
  positionRadius = r;
}

void State::setPositionAngle(float a) {
  positionAngle = a;
}

void State::setPositionAbs(Point3D &p) {
  positionAbs = p;
}

void State::setOutgoingControl(Point3D &p) 
{
  outgoingControl = p;
}

void State::setIncomingControl(Point3D &p) 
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

Transition* State::getInTransition(int i) const {
  return inTransitions[i];
}

int State::getNumInTransitions() const {
  return inTransitions.size();
}

Transition* State::getOutTransition(int i) const {
  return outTransitions[i];
}

int State::getNumOutTransitions() const {
  return outTransitions.size();
}

Transition* State::getLoop(int i) const {
  return loops[i];
}

int State::getNumLoops() const {
  return loops.size();
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

Point3D State::getForce() {
  return force;
}

void State::resetForce() {
  force.x = 0;
  force.y = 0;
  force.z = 0;
}

void State::addForce(Point3D f) {
  force = force + f;
}

Vect State::getVelocity() {
  return velocity;
}

void State::resetVelocity() {
  velocity.x = 0;
  velocity.y = 0;
}

void State::addVelocity(Vect v) {
  velocity = velocity + v;
}
