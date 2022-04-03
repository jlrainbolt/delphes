#include "display/UCDelphesEventDisplay.h"
#include "display/DelphesDisplay.h"
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include <TChain.h>
#include <TColor.h>
#include <TEveManager.h>
#include <TEveScene.h>
#include <TPRegexp.h>
#include <TRef.h>
#include <iostream>


namespace
{
    void PrintChildren(TEveElement *list)
    {
        std::cout << std::endl << list->GetElementName() << ":" << std:: endl;
        for(TEveElement::List_i it = list->BeginChildren(); it != list->EndChildren(); ++it)
        {
            TEveElement *element = static_cast<TEveElement*>(*it);
            std::cout << "\t" << element->GetElementName() << std::endl;
        }
    }
}


const int UCDelphesEventDisplay::PDG_z = 23;
const int UCDelphesEventDisplay::PDG_h1 = 25;
const int UCDelphesEventDisplay::PDG_h2 = 35;

const int UCDelphesEventDisplay::COLOR_z_quark = kAzure - 4;
const int UCDelphesEventDisplay::COLOR_h1_quark = kViolet + 6;
const int UCDelphesEventDisplay::COLOR_h2_quark = kPink - 4;
const int UCDelphesEventDisplay::COLOR_jet = kTeal - 9;
const int UCDelphesEventDisplay::COLOR_fat_jet = kWhite;


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
    particle_branch = treeReader_->UseBranch("Particle");
    manager = gEve->GetCurrentEvent();

    TurnOffCalorimeter();
    TurnOffBranches();
}


void UCDelphesEventDisplay::TurnOffCalorimeter()
{
    TEveSceneList *scene_list = gEve->GetScenes();
    TEveElement *geometry_scene = scene_list->FindChild("Geometry scene");
    TEveElement *calorimeter = geometry_scene->FindChild("TEveCalo3D");
    calorimeter->SetRnrState(false);
}


void UCDelphesEventDisplay::TurnOffBranches()
{
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
    manager = gEve->GetCurrentEvent();
    SelectBQuarks();
    ColorJets();
}


void UCDelphesEventDisplay::SelectBQuarks()
{
    TEveElement *particle_list = manager->FindChild("Particle");
    TurnOffChildren(particle_list);

    TEveElement::List_t b_quark_elements = FindBQuarks(particle_list);
    DisplayHardScatterBQuarks(b_quark_elements);
}


void UCDelphesEventDisplay::TurnOffChildren(TEveElement *list) // static
{
    list->SetRnrState(true);
    for(TEveElement::List_i it = list->BeginChildren(); it != list->EndChildren(); ++it)
    {
        TEveElement *element = static_cast<TEveElement*>(*it);
        element->SetRnrState(false);
    }
}


TEveElement::List_t UCDelphesEventDisplay::FindBQuarks(TEveElement *list) // static
{
    TEveElement::List_t b_quark_elements;
    TPRegexp b_name_pattern("b(_bar)? \\[\\d+\\]");
    list->FindChildren(b_quark_elements, b_name_pattern);
    return b_quark_elements;
}


void UCDelphesEventDisplay::DisplayHardScatterBQuarks(const TEveElement::List_t &list)
{
    for (TEveElement *element: list)
    {
        TEveTrack *track = static_cast<TEveTrack*>(element);

        int particle_index = track->GetLabel();
        GenParticle *particle = dynamic_cast<GenParticle*>(particle_branch->At(particle_index));

        int mother_index = particle->M1;
        GenParticle *mother = dynamic_cast<GenParticle*>(particle_branch->At(mother_index));

        int mother_pid = mother->PID;
        if ( (mother_pid == PDG_z) || (mother_pid == PDG_h1) || (mother_pid == PDG_h2) )
        {
            SetBQuarkDisplayOptions(track, mother_pid);
            element->SetRnrState(true);
        }
    }
}


void UCDelphesEventDisplay::SetBQuarkDisplayOptions(TEveTrack *track, int mother_pid) // static
{
    track->SetLineStyle(kSolid);
    track->SetLineWidth(2);

    if (mother_pid == PDG_z)
        track->SetMainColor(COLOR_z_quark);
    else if (mother_pid == PDG_h1)
        track->SetMainColor(COLOR_h1_quark);
    else if (mother_pid == PDG_h2)
        track->SetMainColor(COLOR_h2_quark);
}


void UCDelphesEventDisplay::ColorJets()
{
    TEveElement *jet_list = manager->FindChild("Jet");
    ColorChildren(jet_list, COLOR_jet);
    TEveElement *fat_jet_list = manager->FindChild("FatJet");
    ColorChildren(fat_jet_list, COLOR_fat_jet);
}

void UCDelphesEventDisplay::ColorChildren(TEveElement *list, int color) // static
{
    for(TEveElement::List_i it = list->BeginChildren(); it != list->EndChildren(); ++it)
    {
        TEveElement *element = static_cast<TEveElement*>(*it);
        element->SetMainColor(color);
    }
}

