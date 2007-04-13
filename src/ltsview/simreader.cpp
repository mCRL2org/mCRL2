#include "simreader.h"

simReader::simReader(Simulation* s)
  : sim(s) 
{
  if (s != NULL) {
    connection = sim->connect(boost::bind(&simReader::refresh, this));
    chooseConnection = sim->connectSel(boost::bind(&simReader::selChange, 
                                       this));
  }
}

simReader::~simReader()
{
  sim->disconnect(connection);
}

void simReader::setSim(Simulation* s) 
{
  // Disconnect from old sim
  sim->disconnect(connection);
  sim->disconnect(chooseConnection);
  
  // Connect to new sim

  sim = s;
  connection = sim->connect(boost::bind(&simReader::refresh, this));
  chooseConnection = sim->connectSel(boost::bind(&simReader::selChange, 
                                       this));
}
