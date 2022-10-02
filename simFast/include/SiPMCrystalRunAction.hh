#ifndef SiPMCrystalRunAction_h
#define SiPMCrystalRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"

class G4Timer;
class G4Run;

class SiPMCrystalRunAction : public G4UserRunAction {
  public:
    SiPMCrystalRunAction();
    virtual ~SiPMCrystalRunAction();

  public:
    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);

  private:
    G4Timer* fTimer;
};

#endif
