#include "display/UCDelphesEventDisplay.h"
#include <TChain.h>


UCDelphesEventDisplay::UCDelphesEventDisplay():
    DelphesEventDisplay()
{}


UCDelphesEventDisplay::~UCDelphesEventDisplay()
{
    delete chain_;
}

UCDelphesEventDisplay::UCDelphesEventDisplay(const char* configFile, const char* inputFile, Delphes3DGeometry &det3D):
    DelphesEventDisplay(configFile, inputFile, det3D)
{}
