#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <string>

using namespace std;

class Mediator
{
  public:
    virtual void applyDefaultSettings() = 0;
    virtual void applySettings() = 0;
    virtual void drawLTS() = 0;
    virtual void openFile( string fileName ) = 0;
    virtual void setRankStyle( string rs ) = 0;
    virtual void showMarkStateRuleDialog() = 0;
};

#endif
