// Make this appear first!
#include "G4Timer.hh"

#include "SiPMCrystalRunAction.hh"

#include "G4Run.hh"

SiPMCrystalRunAction::SiPMCrystalRunAction()
 : G4UserRunAction(),
   fTimer(0) {
  fTimer = new G4Timer;
}

SiPMCrystalRunAction::~SiPMCrystalRunAction() {
  delete fTimer;
}

void SiPMCrystalRunAction::BeginOfRunAction(const G4Run* aRun) {
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  fTimer->Start();
}

void SiPMCrystalRunAction::EndOfRunAction(const G4Run* aRun) {
  fTimer->Stop();
  G4cout << "number of event = " << aRun->GetNumberOfEvent()
         << " " << *fTimer << G4endl;
}
