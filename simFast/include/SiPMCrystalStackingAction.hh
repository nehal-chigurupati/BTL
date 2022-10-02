#ifndef SiPMCrystalStackingAction_H
#define SiPMCrystalStackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"

class SiPMCrystalStackingAction : public G4UserStackingAction {
  public:
    SiPMCrystalStackingAction();
    virtual ~SiPMCrystalStackingAction();

  public:
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();

  private:
    G4int fScintillationCounter;
    G4int fCerenkovCounter;
};

#endif
