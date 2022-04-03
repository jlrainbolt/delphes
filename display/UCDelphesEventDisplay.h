#ifndef UCDelphesEventDisplay_h
#define UCDelphesEventDisplay_h

#include "display/DelphesEventDisplay.h"
#include "display/Delphes3DGeometry.h"
#include <TClonesArray.h>
#include <TEveElement.h>
#include <TEveEventManager.h>
#include <TEveTrack.h>


class UCDelphesEventDisplay: public DelphesEventDisplay
{
    public:
        UCDelphesEventDisplay();
        UCDelphesEventDisplay(const char *configFile, const char *inputFile, Delphes3DGeometry &det3D);
        ~UCDelphesEventDisplay();

    private:
        void select_elements() override;
        void TurnOffCalorimeter();
        void TurnOffBranches();

        void SelectBQuarks();
        static void TurnOffChildren(TEveElement*);
        static TEveElement::List_t FindBQuarks(TEveElement*);
        void DisplayHardScatterBQuarks(const TEveElement::List_t&);
        static void SetBQuarkDisplayOptions(TEveTrack*, int mother_pid);

        void ColorJets();
        static void ColorChildren(TEveElement*, int color);

        TClonesArray *particle_branch;
        TEveEventManager *manager;

        static const int PDG_z, PDG_h1, PDG_h2;
        static const int COLOR_z_quark, COLOR_h1_quark, COLOR_h2_quark, COLOR_jet, COLOR_fat_jet;
};


#endif
