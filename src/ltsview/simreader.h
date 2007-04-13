#ifndef SIMREADER_H
#define SIMREADER_H
#include "simulation.h"

class simReader {
  public: 
    simReader(Simulation* s);
    virtual ~simReader();
    virtual void refresh() = 0; 
    virtual void selChange() = 0;
    virtual void setSim(Simulation* s);
 
  protected:
    Simulation* sim;

  private:
    Simulation::simConnection connection;
    Simulation::simConnection chooseConnection;
};

#endif

