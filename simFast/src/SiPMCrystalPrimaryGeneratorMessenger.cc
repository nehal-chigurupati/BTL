#include "SiPMCrystalPrimaryGeneratorMessenger.hh"

#include "SiPMCrystalPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4SystemOfUnits.hh"

SiPMCrystalPrimaryGeneratorMessenger::
  SiPMCrystalPrimaryGeneratorMessenger(SiPMCrystalPrimaryGeneratorAction* SiPMCrystalGun)
  : G4UImessenger(),
    fSiPMCrystalAction(SiPMCrystalGun) {
  fGunDir = new G4UIdirectory("/SiPMCrystal/gun/");
  fGunDir->SetGuidance("PrimaryGenerator control");

  fPolarCmd =
           new G4UIcmdWithADoubleAndUnit("/SiPMCrystal/gun/optPhotonPolar",this);
  fPolarCmd->SetGuidance("Set linear polarization");
  fPolarCmd->SetGuidance("  angle w.r.t. (k,n) plane");
  fPolarCmd->SetParameterName("angle",true);
  fPolarCmd->SetUnitCategory("Angle");
  fPolarCmd->SetDefaultValue(-360.0);
  fPolarCmd->SetDefaultUnit("deg");
  fPolarCmd->AvailableForStates(G4State_Idle);
}

SiPMCrystalPrimaryGeneratorMessenger::~SiPMCrystalPrimaryGeneratorMessenger() {
  delete fPolarCmd;
  delete fGunDir;
}

void SiPMCrystalPrimaryGeneratorMessenger::SetNewValue(
                                        G4UIcommand* command, G4String newValue) {
  if( command == fPolarCmd ) {
      G4double angle = fPolarCmd->GetNewDoubleValue(newValue);
      if ( angle == -360.0*deg ) {
         fSiPMCrystalAction->SetOptPhotonPolar();
      } else {
         fSiPMCrystalAction->SetOptPhotonPolar(angle);
      }
  }
}

