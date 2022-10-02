#include "SiPMCrystalStackingAction.hh"

#include "G4VProcess.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4ios.hh"

SiPMCrystalStackingAction::SiPMCrystalStackingAction()
  : G4UserStackingAction(),
    fScintillationCounter(0), fCerenkovCounter(0) {}

SiPMCrystalStackingAction::~SiPMCrystalStackingAction() {}

G4ClassificationOfNewTrack SiPMCrystalStackingAction::ClassifyNewTrack(const G4Track * aTrack) {
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  { // particle is optical photon
    if(aTrack->GetParentID()>0)
    { // particle is secondary
      if(aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation")
        fScintillationCounter++;
      if(aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov")
        fCerenkovCounter++;
    }
  }
  return fUrgent;
}

void SiPMCrystalStackingAction::NewStage() {
  G4cout << "Number of Scintillation photons produced in this event : "
         << fScintillationCounter << G4endl;
  G4cout << "Number of Cerenkov photons produced in this event : "
         << fCerenkovCounter << G4endl;
}

void SiPMCrystalStackingAction::PrepareNewEvent() {
  fScintillationCounter = 0;
  fCerenkovCounter = 0;
}
