#include "SiPMCrystalActionInitialization.hh"
#include "SiPMCrystalPrimaryGeneratorAction.hh"
#include "SiPMCrystalRunAction.hh"
#include "SiPMCrystalSteppingAction.hh"
#include "SiPMCrystalStackingAction.hh"

SiPMCrystalActionInitialization::SiPMCrystalActionInitialization() : G4VUserActionInitialization() {}

SiPMCrystalActionInitialization::~SiPMCrystalActionInitialization() {}

void SiPMCrystalActionInitialization::BuildForMaster() const {
  SetUserAction(new SiPMCrystalRunAction());
}

void SiPMCrystalActionInitialization::Build() const {
  SetUserAction(new SiPMCrystalPrimaryGeneratorAction());
  SetUserAction(new SiPMCrystalRunAction());
  SetUserAction(new SiPMCrystalSteppingAction());
  SetUserAction(new SiPMCrystalStackingAction());
}
