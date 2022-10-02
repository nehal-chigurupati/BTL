#ifndef SiPMCrystalPrimaryGeneratorMessenger_h
#define SiPMCrystalPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class SiPMCrystalPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;

class SiPMCrystalPrimaryGeneratorMessenger: public G4UImessenger {
  public:
    SiPMCrystalPrimaryGeneratorMessenger(SiPMCrystalPrimaryGeneratorAction* );
    virtual ~SiPMCrystalPrimaryGeneratorMessenger();
 
    virtual void SetNewValue(G4UIcommand*, G4String);
 
  private:
    SiPMCrystalPrimaryGeneratorAction* fSiPMCrystalAction;
    G4UIdirectory*                  fGunDir;
    G4UIcmdWithADoubleAndUnit*      fPolarCmd;
};

#endif
