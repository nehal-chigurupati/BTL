#ifndef SiPMCrystalSteppingAction_h
#define SiPMCrystalSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include <fstream>

class SiPMCrystalSteppingAction : public G4UserSteppingAction {
  public:
    SiPMCrystalSteppingAction();
    virtual ~SiPMCrystalSteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);

  private:
    G4int fScintillationCounter;
    G4int fCerenkovCounter;
    G4int fEventNumber;
    G4double fEneDep;
    std::ofstream fOutput;
};

#endif
