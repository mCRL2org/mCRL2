#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <vector>
#include <string>
#include <utils.h>

using namespace std;
using namespace Utils;

class Mediator
{
  public:
    virtual ~Mediator() {}
    virtual void addMarkRule() = 0;
    virtual void applyDefaultSettings() = 0;
    virtual void applyMarkStyle( MarkStyle ms ) = 0;
    virtual void applySettings() = 0;
    virtual void drawLTS() = 0;
    virtual void openFile( string fileName ) = 0;
    virtual void removeMarkRules( const vector<int> &mrs ) = 0;
    virtual void setMatchAnyMarkRule( bool b ) = 0;
    virtual void setRankStyle( RankStyle rs ) = 0;
};

#endif
