/* Example:
 * root -l examples/UCEventDisplay.C'("cards/delphes_card_CMS.tcl","delphes_output.root")'
 * root -l examples/UCEventDisplay.C'("cards/delphes_card_FCC_basic.tcl","delphes_output.root","ParticlePropagator","ChargedHadronTrackingEfficiency","MuonTrackingEfficiency","Ecal,Hcal")'
 */

#ifdef __CLING__
R__LOAD_LIBRARY(libEve)
R__LOAD_LIBRARY(libDelphesDisplay)
#include "display/UCDelphesEventDisplay.h"
#include "display/Delphes3DGeometry.h"
#endif

void UCEventDisplay(const char *configfile = "delphes_card_CMS.tcl",
                  const char *datafile = "delphes_output.root",
                  const char *ParticlePropagator = "ParticlePropagator",
                  const char *TrackingEfficiency = "ChargedHadronTrackingEfficiency",
                  const char *MuonEfficiency = "MuonEfficiency",
                  const char *Calorimeters = "HCal",
                  bool displayGeometryOnly = false)
{
  // load the libraries
  gSystem->Load("libGeom");
  gSystem->Load("libGuiHtml");
  gSystem->Load("libDelphesDisplay");

  if(displayGeometryOnly)
  {
    // create the detector representation without transparency
    Delphes3DGeometry det3D_geom(new TGeoManager("delphes", "Delphes geometry"), false);
    det3D_geom.readFile(configfile, ParticlePropagator, TrackingEfficiency, MuonEfficiency, Calorimeters);

    // display
    det3D_geom.getDetector()->Draw("ogl");
  } 
  else
  {
    // create the detector representation
    Delphes3DGeometry det3D(new TGeoManager("delphes", "Delphes geometry"), true);
    det3D.readFile(configfile, ParticlePropagator, TrackingEfficiency, MuonEfficiency, Calorimeters);

    // create the application
    UCDelphesEventDisplay *display = new UCDelphesEventDisplay(configfile, datafile, det3D);
  }
}

