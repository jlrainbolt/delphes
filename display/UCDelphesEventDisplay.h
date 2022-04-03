#ifndef UCDelphesEventDisplay_h
#define UCDelphesEventDisplay_h

#include "display/DelphesEventDisplay.h"
#include "display/Delphes3DGeometry.h"


class UCDelphesEventDisplay: public DelphesEventDisplay
{
    public:
        UCDelphesEventDisplay();
        UCDelphesEventDisplay(const char *configFile, const char *inputFile, Delphes3DGeometry &det3D);
        ~UCDelphesEventDisplay();

//  private:
//      void load_event() override;
};


#endif
