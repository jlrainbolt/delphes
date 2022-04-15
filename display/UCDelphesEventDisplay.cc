#include "display/UCDelphesEventDisplay.h"
#include "display/DelphesDisplay.h"
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include <TChain.h>
#include <TColor.h>
#include <TEveEventManager.h>
#include <TEveManager.h>
#include <TEveScene.h>
#include <TEveTrack.h>
#include <TGHtml.h>
#include <TPRegexp.h>
#include <TRef.h>
#include <TString.h>
#include <cmath>
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


const TString UCDelphesEventDisplay::PARTICLE_BRANCH_NAME = "Particle";
const TString UCDelphesEventDisplay::JET_BRANCH_NAME = "Jet";
const TString UCDelphesEventDisplay::FAT_JET_BRANCH_NAME = "FatJet";

const int UCDelphesEventDisplay::PDG_z = 23;
const int UCDelphesEventDisplay::PDG_h1 = 25;
const int UCDelphesEventDisplay::PDG_h2 = 35;

const int UCDelphesEventDisplay::COLOR_z_quark = kSpring + 6;
const int UCDelphesEventDisplay::COLOR_h1_quark = kTeal + 6;
const int UCDelphesEventDisplay::COLOR_h2_quark = kOrange + 6;
const int UCDelphesEventDisplay::COLOR_jet = kViolet - 9;
const int UCDelphesEventDisplay::COLOR_fat_jet = kAzure + 6;
const int UCDelphesEventDisplay::LINE_WIDTH_quark = 4;

const double UCDelphesEventDisplay::MAX_JET_ETA = 2.5;


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
    LoadBranches();
    TurnOffCalorimeter();
    UpdateEventManagement();
    TurnOffObjectRendering();
    ReserveContainers();
}


void UCDelphesEventDisplay::LoadBranches()
{
    particle_branch = treeReader_->UseBranch(PARTICLE_BRANCH_NAME);
    jet_branch = treeReader_->UseBranch(JET_BRANCH_NAME);
    fat_jet_branch = treeReader_->UseBranch(FAT_JET_BRANCH_NAME);
}


void UCDelphesEventDisplay::TurnOffCalorimeter()
{
    TEveSceneList *scene_list = gEve->GetScenes();
    TEveElement *geometry_scene = scene_list->FindChild("Geometry scene");
    TEveElement *calorimeter = geometry_scene->FindChild("TEveCalo3D");
    calorimeter->SetRnrState(false);
}


void UCDelphesEventDisplay::UpdateEventManagement()
{
    manager = gEve->GetCurrentEvent();
    particle_list = manager->FindChild(PARTICLE_BRANCH_NAME);
    jet_list = manager->FindChild(JET_BRANCH_NAME);
    fat_jet_list = manager->FindChild(FAT_JET_BRANCH_NAME);
}


void UCDelphesEventDisplay::TurnOffObjectRendering()
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


void UCDelphesEventDisplay::ReserveContainers()
{
    selected_b_quarks.reserve(4);
    selected_jets.reserve(10);
    selected_fat_jets.reserve(5);
}


void UCDelphesEventDisplay::process_event() // override
{
    ClearContainers();
    UpdateEventManagement();
    ProcessBQuarks();
    ProcessJets();
    FillTables();
}


void UCDelphesEventDisplay::ClearContainers()
{
    selected_b_quarks.clear();
    selected_jets.clear();
    selected_fat_jets.clear();
}


void UCDelphesEventDisplay::ProcessBQuarks()
{
    TurnOffChildren(particle_list);
    TPRegexp pattern("b(_bar)? \\[\\d+\\]");
    TEveElement::List_t b_quark_elements = FindElements(particle_list, pattern);
    SelectBQuarks(b_quark_elements);
    std::sort(selected_b_quarks.begin(), selected_b_quarks.end(), SortByPt<GenParticle>);
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


TEveElement::List_t UCDelphesEventDisplay::FindElements(TEveElement *list, TPRegexp &pattern) // static
{
    TEveElement::List_t selected_elements;
    list->FindChildren(selected_elements, pattern);
    return selected_elements;
}


void UCDelphesEventDisplay::SelectBQuarks(const TEveElement::List_t &list)
{
    for (TEveElement *element: list)
    {
        TEveTrack *track = static_cast<TEveTrack*>(element);
        GenParticle *particle = GetParticle(track);

        int mother_index = particle->M1;
        GenParticle *mother = GetParticle(mother_index);

        int mother_pid = mother->PID;
        if ( (mother_pid == PDG_z) || (mother_pid == PDG_h1) || (mother_pid == PDG_h2) )
        {
            SetBQuarkDisplayOptions(track, mother_pid);
            element->SetRnrState(true);
            selected_b_quarks.push_back(*particle);
            selected_b_quarks.back().M1 = mother_pid;
        }
    }
}


template<class T> bool UCDelphesEventDisplay::SortByPt(T &object_1, T &object_2) // static
{
    return (object_1.PT > object_2.PT);
}


GenParticle* UCDelphesEventDisplay::GetParticle(TEveTrack *track) const
{
    int index = track->GetLabel();
    return GetParticle(index);
}


GenParticle* UCDelphesEventDisplay::GetParticle(int index) const
{
    return dynamic_cast<GenParticle*>(particle_branch->At(index));
}


void UCDelphesEventDisplay::SetBQuarkDisplayOptions(TEveTrack *track, int mother_pid) // static
{
    track->SetLineStyle(kSolid);
    track->SetLineWidth(LINE_WIDTH_quark);

    if (mother_pid == PDG_z)
        track->SetMainColor(COLOR_z_quark);
    else if (mother_pid == PDG_h1)
        track->SetMainColor(COLOR_h1_quark);
    else if (mother_pid == PDG_h2)
        track->SetMainColor(COLOR_h2_quark);
}


void UCDelphesEventDisplay::ProcessJets()
{
    TurnOffChildren(jet_list);
    ColorChildren(jet_list, COLOR_jet);
    SelectJets(jet_list, jet_branch, selected_jets);
    std::sort(selected_jets.begin(), selected_jets.end(), SortByPt<Jet>);

    TurnOffChildren(fat_jet_list);
    ColorChildren(fat_jet_list, COLOR_fat_jet);
    SelectJets(fat_jet_list, fat_jet_branch, selected_fat_jets);
    std::sort(selected_fat_jets.begin(), selected_fat_jets.end(), SortByPt<Jet>);
}


void UCDelphesEventDisplay::SelectJets(TEveElement *list, TClonesArray *branch, std::vector<Jet> &selected_jets) // static
{
    for (TEveElement::List_i it = list->BeginChildren(); it != list->EndChildren(); ++it)
    {
        TEveElement *element = static_cast<TEveElement*>(*it);
        Jet *jet = GetJet(element, branch);
        if (PassCuts(jet))
        {
            element->SetRnrState(true);
            selected_jets.push_back(*jet);
        }
    }
}


Jet* UCDelphesEventDisplay::GetJet(TEveElement *element, TClonesArray *branch) // static
{
    int jet_index = GetIndex(element);
    return dynamic_cast<Jet*>(branch->At(jet_index));
}


bool UCDelphesEventDisplay::PassCuts(Jet *jet) // static
{
    double abs_eta = fabs(jet->Eta);
    return (abs_eta < MAX_JET_ETA);
}


int UCDelphesEventDisplay::GetIndex(TEveElement *element) // static
{
    TString element_name = element->GetElementName();

    Ssiz_t start = 0;
    Ssiz_t start_length = element_name.First("[") + 1 - start;
    element_name = element_name.Remove(start, start_length);

    Ssiz_t end = element_name.First("]");
    Ssiz_t end_length = element_name.Length() - end;
    TString index = element_name.Remove(end, end_length);

    return index.Atoi();
}


void UCDelphesEventDisplay::ColorChildren(TEveElement *list, int color) // static
{
    for (TEveElement::List_i it = list->BeginChildren(); it != list->EndChildren(); ++it)
    {
        TEveElement *element = static_cast<TEveElement*>(*it);
        element->SetMainColor(color);
    }
}


void UCDelphesEventDisplay::FillTables()
{
    htmlSummary_->Clear("D");

    MakeMassTable();
    MakeBQuarkTable();
    MakeJetTable(JET_BRANCH_NAME, selected_jets);
    MakeJetTable(FAT_JET_BRANCH_NAME, selected_fat_jets);

    htmlSummary_->Build();
    gHtml_->Clear();
    gHtml_->ParseText((char *)htmlSummary_->Html().Data());
    gHtml_->Layout();
}


void UCDelphesEventDisplay::MakeMassTable()
{
    TString name = "Event info";
    std::vector<TString> fields = {"A mass", "H mass"};
    int n_fields = fields.size();
    int n_rows = 1;
    bool should_expand = true;
    TString option = "first";
    DelphesHtmlObjTable *table = htmlSummary_->AddTable(name.Data(), n_fields, n_rows, should_expand, option.Data());

    SetLabels(table, fields);
    FillEventValues(table, fields);
}


void UCDelphesEventDisplay::SetLabels(DelphesHtmlObjTable *table, const std::vector<TString> &fields) // static
{
    for (int i = 0; i < int(fields.size()); ++i)
        table->SetLabel(i, fields.at(i));
}


void UCDelphesEventDisplay::FillEventValues(DelphesHtmlObjTable *table, const std::vector<TString> &fields)
{
    int row = 0;
    TPRegexp pattern;

    for (int i = 0; i < int(fields.size()); ++i)
    {
        TString field = fields.at(i);
        if (field == "A mass")
            pattern = TPRegexp("A0 \\[\\d+\\]");
        else if (field == "H mass")
            pattern = TPRegexp("H0 \\[\\d+\\]");
            
        int value = round(GetGenParticleMass(pattern)); // round because of weird floating point error
        table->SetValue(i, row, value);
    }
}


double UCDelphesEventDisplay::GetGenParticleMass(TPRegexp &pattern) const
{
    TEveElement::List_t h3_elements = FindElements(particle_list, pattern);
    TEveTrack *track = static_cast<TEveTrack*>(h3_elements.front());
    GenParticle *particle = GetParticle(track);
    return particle->Mass;
}


void UCDelphesEventDisplay::MakeBQuarkTable()
{
    std::vector<TString> fields = {"pt", "eta", "phi", "mother"};
    int n_fields = fields.size();
    int n_rows = selected_b_quarks.size();
    bool should_expand = true;
    TString option = "";
    DelphesHtmlObjTable *table = htmlSummary_->AddTable(PARTICLE_BRANCH_NAME.Data(), n_fields, n_rows, should_expand, option.Data());

    SetLabels(table, fields);
    FillValues(table, fields, selected_b_quarks);
}


void UCDelphesEventDisplay::FillValues(DelphesHtmlObjTable *table, const std::vector<TString> &fields, std::vector<GenParticle> &particles) // static
{
    for (int j = 0; j < int(particles.size()); ++j)
    {
        GenParticle particle = particles.at(j);
        double value = -9999;

        for (int i = 0; i < int(fields.size()); ++i)
        {
            TString field = fields.at(i);
            if (field == "pt")
                value = particle.PT;
            else if (field == "eta")
                value = particle.Eta;
            else if (field == "phi")
                value = particle.Phi;
            else if (field == "mother")
                value = particle.M1;

            table->SetValue(i, j, value);
        }
    }
}


void UCDelphesEventDisplay::MakeJetTable(const TString &branch_name, std::vector<Jet> &jets)
{
    std::vector<TString> fields = {"pt", "eta", "phi", "mass", "b tag"};
    int n_fields = fields.size();
    int n_rows = jets.size();
    bool should_expand = true;
    TString option = "";
    DelphesHtmlObjTable *table = htmlSummary_->AddTable(branch_name.Data(), n_fields, n_rows, should_expand, option.Data());

    SetLabels(table, fields);
    FillValues(table, fields, jets);
}


void UCDelphesEventDisplay::FillValues(DelphesHtmlObjTable *table, const std::vector<TString> &fields, std::vector<Jet> &jets) // static
{
    for (int j = 0; j < int(jets.size()); ++j)
    {
        Jet jet = jets.at(j);
        double value = -9999;

        for (int i = 0; i < int(fields.size()); ++i)
        {
            TString field_name = fields.at(i);
            if (field_name == "pt")
                value = jet.PT;
            else if (field_name == "eta")
                value = jet.Eta;
            else if (field_name == "phi")
                value = jet.Phi;
            else if (field_name == "mass")
                value = jet.Mass;
            else if (field_name == "b tag")
                value = jet.BTag;

            table->SetValue(i, j, value);
        }
    }
}
