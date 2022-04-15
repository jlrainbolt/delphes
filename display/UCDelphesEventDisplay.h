#ifndef UCDelphesEventDisplay_h
#define UCDelphesEventDisplay_h

#include "display/DelphesEventDisplay.h"
#include "display/Delphes3DGeometry.h"
#include "display/DelphesHtmlSummary.h"
#include "classes/DelphesClasses.h"
#include <TClonesArray.h>
#include <TEveElement.h>
#include <TEveEventManager.h>
#include <TEveTrack.h>
#include <TPRegexp.h>
#include <TString.h>
#include <vector>


class UCDelphesEventDisplay: public DelphesEventDisplay
{
    public:
        UCDelphesEventDisplay();
        UCDelphesEventDisplay(const char *configFile, const char *inputFile, Delphes3DGeometry &det3D);
        ~UCDelphesEventDisplay();


    private:
        void process_event() override;

        void LoadBranches();
        void TurnOffCalorimeter();
        void UpdateEventManagement();
        void TurnOffObjectRendering();
        void ReserveContainers();
        void ClearContainers();

        void ProcessBQuarks();
        static void TurnOffChildren(TEveElement*);
        static TEveElement::List_t FindElements(TEveElement*, TPRegexp&);
        GenParticle* GetParticle(TEveTrack*) const;
        GenParticle* GetParticle(int) const;
        static int GetIndex(TEveElement*);
        void SelectBQuarks(const TEveElement::List_t&);
        static void SetBQuarkDisplayOptions(TEveTrack*, int mother_pid);
        template<class T> static bool SortByPt(T&, T&);

        void ProcessJets();
        static void SelectJets(TEveElement*, TClonesArray*, std::vector<Jet>&);
        static Jet* GetJet(TEveElement*, TClonesArray*);
        static bool PassCuts(Jet*);
        static void ColorChildren(TEveElement*, int color);

        void FillTables();
        void SetLabels(DelphesHtmlObjTable*, const std::vector<TString>&);
        void FillEventValues(DelphesHtmlObjTable*, const std::vector<TString>&);
        void MakeMassTable();
        double GetGenParticleMass(TPRegexp&) const;
        void MakeBQuarkTable();
        void FillValues(DelphesHtmlObjTable*, const std::vector<TString>&, std::vector<GenParticle>&);
        void MakeJetTable(const TString&, std::vector<Jet>&);
        void FillValues(DelphesHtmlObjTable*, const std::vector<TString>&, std::vector<Jet>&);
        template<class T> void FillValues(DelphesHtmlObjTable*, const std::vector<TString>&, std::vector<T>&);

        TEveEventManager *manager;
        TEveElement *particle_list, *jet_list, *fat_jet_list;
        TClonesArray *particle_branch, *jet_branch, *fat_jet_branch;
        std::vector<GenParticle> selected_b_quarks;
        std::vector<Jet> selected_jets, selected_fat_jets;

        static const TString PARTICLE_BRANCH_NAME, JET_BRANCH_NAME, FAT_JET_BRANCH_NAME;
        static const int PDG_z, PDG_h1, PDG_h2;
        static const int COLOR_z_quark, COLOR_h1_quark, COLOR_h2_quark, COLOR_jet, COLOR_fat_jet;
        static const int LINE_WIDTH_quark;
        static const double MAX_JET_ETA;
};


#endif
