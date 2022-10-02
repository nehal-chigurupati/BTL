#include <iostream>
#include "SiPMCrystalSteppingAction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include <fstream>
#include "G4SystemOfUnits.hh"

SiPMCrystalSteppingAction::SiPMCrystalSteppingAction() : G4UserSteppingAction() { 
  fScintillationCounter = 0;
  fCerenkovCounter      = 0;
  fEventNumber = -1;
  fEneDep = 0;
  fOutput.open( "output.dat" );
}

SiPMCrystalSteppingAction::~SiPMCrystalSteppingAction() {
  fOutput.close();
}

void SiPMCrystalSteppingAction::UserSteppingAction(const G4Step* step) {
  G4int eventNumber = G4RunManager::GetRunManager()->
                                              GetCurrentEvent()->GetEventID();
  G4LogicalVolume* volume 
    = step->GetPreStepPoint()->GetTouchableHandle()
    ->GetVolume()->GetLogicalVolume();
  
  //std::cout << volume->GetName() << "********" << std::endl;
  if (eventNumber != fEventNumber) {
     fEventNumber = eventNumber;
     fScintillationCounter = 0;
     fCerenkovCounter = 0;
     fEneDep = 0;
     //good place to store this numbers
  }
  G4Track* track = step->GetTrack();
  G4String ParticleName = track->GetDynamicParticle()->
                                 GetParticleDefinition()->GetParticleName();
  if (ParticleName == "opticalphoton") {
    // good place to ask for its position
    if( volume->GetName() == "lyso" ) {
      G4ThreeVector pos1 = step->GetPreStepPoint()->GetPosition();
      G4ThreeVector pos2 = step->GetPostStepPoint()->GetPosition();
      G4ThreeVector dir = step->GetPreStepPoint()->GetMomentumDirection();
      fOutput << pos1.x()/mm << " " << pos1.y()/mm << " " << pos1.z()/mm << " ";
      fOutput << pos2.x()/mm << " " << pos2.y()/mm << " " << pos2.z()/mm << " ";
      fOutput << dir.x()/mm << " " << dir.y()/mm << " " << dir.z()/mm << " ";
      fOutput << step->GetStepLength()/mm << " " << track->GetTotalEnergy()/eV << " ";
      fOutput << track->GetParentID() << std::endl;
    }
    /*
    if( track->GetParentID() > 1 ) { // AFTER FIRST SCINTILLATION
      std::cout << " NOT PRODUCED BY PRIMARY (PARENT ID " << track->GetParentID() << ")" << std::endl;
      std::cout << " OPTICAL PHOTON CREATED BY " << track->GetCreatorProcess()->GetProcessName() << std::endl;
    }
    */
  } else {
    std::cout << " TRACKING SOMETHING DIFFERENT THAN AN OPTICAL PHOTON ==> " << ParticleName << std::endl;
  }

  if( track->GetParentID()==0 ) { // primary
    fEneDep += (step->GetTotalEnergyDeposit() - step->GetNonIonizingEnergyDeposit())/MeV;
    std::cout << "ENEDEP until now " << fEneDep << std::endl;
  }

  // check if in this step there were secondaries 
  const std::vector<const G4Track*>* secondaries =
                                            step->GetSecondaryInCurrentStep();
  if (secondaries->size()>0) {
    //loop over secondaries
    for(unsigned int i=0; i<secondaries->size(); ++i) {
      if (secondaries->at(i)->GetParentID()>0) {
	// is not primary
	if(secondaries->at(i)->GetDynamicParticle()->GetParticleDefinition()
	   == G4OpticalPhoton::OpticalPhotonDefinition()){
	  // is an optical photon
	  if ( secondaries->at(i)->GetParentID() > 1 )
	    //std::cout << "  ==> SECONDARY OPTICAL PHOTON with parent " << secondaries->at(i)->GetParentID() << std::endl;
	  if (secondaries->at(i)->GetCreatorProcess()->GetProcessName()
	      == "Scintillation")fScintillationCounter++;
	  if (secondaries->at(i)->GetCreatorProcess()->GetProcessName()
	      == "Cerenkov")fCerenkovCounter++;
	}
      }
    }
  }
}

