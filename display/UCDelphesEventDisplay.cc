#include "display/UCDelphesEventDisplay.h"
#include "display/DelphesDisplay.h"
#include <TChain.h>
#include <TEveManager.h>
#include <TEveEventManager.h>
#include <TPRegexp.h>
#include <iostream>


UCDelphesEventDisplay::UCDelphesEventDisplay():
    DelphesEventDisplay()
{}


UCDelphesEventDisplay::~UCDelphesEventDisplay()
{
    delete chain_;
}

UCDelphesEventDisplay::UCDelphesEventDisplay(const char* configFile, const char* inputFile, Delphes3DGeometry &det3D):
    DelphesEventDisplay(configFile, inputFile, det3D)
{
    TurnOffBranches();
}


void UCDelphesEventDisplay::TurnOffBranches()
{
    TEveEventManager *manager = gEve->GetCurrentEvent();

    manager->FindChild("Particle")->SetRnrState(false);
    manager->FindChild("GenJet")->SetRnrState(false);
    manager->FindChild("GenFatJet")->SetRnrState(false);
    manager->FindChild("GenMissingET")->SetRnrState(false);
    manager->FindChild("Electron")->SetRnrState(false);
    manager->FindChild("Muon")->SetRnrState(false);
    manager->FindChild("Photon")->SetRnrState(false);
    manager->FindChild("TrackJet")->SetRnrState(false);
    manager->FindChild("MissingET")->SetRnrState(false);
    manager->FindChild("Track")->SetRnrState(false);
}


void UCDelphesEventDisplay::select_elements() // override
{
    FindBQuarks();
}


void UCDelphesEventDisplay::FindBQuarks()
{
    TEveEventManager *manager = gEve->GetCurrentEvent();
    TEveElement *particle_list = manager->FindChild("Particle");
    particle_list->SetRnrState(true);
    for(TEveElement::List_i it = particle_list->BeginChildren(); it != particle_list->EndChildren(); ++it)
    {
        TEveElement *element = static_cast<TEveElement*>(*it);
        element->SetRnrState(false);
    }

//  TurnOffChildren(particle_list);
    TEveElement::List_t b_quarks;
    TPRegexp b_name_pattern("b(_bar)? \\[\\d+\\]");
    particle_list->FindChildren(b_quarks, b_name_pattern);

    for (auto b_quark: b_quarks)
        b_quark->SetRnrState(true);
/*
    TEveElement::List_i list;
    TEveElement *element;
    for(list = particle_list->BeginChildren(); list != particle_list->EndChildren(); ++list)
    {
        element = ((TEveElement*)(*list));
        std::cout << element->GetElementName() << std::endl;
    }
*/
}
