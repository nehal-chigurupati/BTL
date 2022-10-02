#ifndef SiPMCrystalPrimaryGeneratorAction_h
#define SiPMCrystalPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class SiPMCrystalPrimaryGeneratorMessenger;

class SiPMCrystalPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
  public:
    SiPMCrystalPrimaryGeneratorAction();
    virtual ~SiPMCrystalPrimaryGeneratorAction();

  public:
    virtual void GeneratePrimaries(G4Event*);

    void SetOptPhotonPolar();
    void SetOptPhotonPolar(G4double);

  private:
    G4ParticleGun* fParticleGun;
    SiPMCrystalPrimaryGeneratorMessenger* fGunMessenger;
};

#endif
