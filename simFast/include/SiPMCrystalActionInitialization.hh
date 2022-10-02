#ifndef SiPMCrystalActionInitialization_h
#define SiPMCrystalActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class B4DetectorConstruction;

/// Action initialization class.
///

class SiPMCrystalActionInitialization : public G4VUserActionInitialization {
  public:
    SiPMCrystalActionInitialization();
    virtual ~SiPMCrystalActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
};

#endif
