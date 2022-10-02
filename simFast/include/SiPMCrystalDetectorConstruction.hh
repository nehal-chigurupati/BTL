#ifndef SiPMCrystalDetectorConstruction_h
#define SiPMCrystalDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
class SiPMCrystalDetectorConstruction : public G4VUserDetectorConstruction {
  public:
    SiPMCrystalDetectorConstruction();
    virtual ~SiPMCrystalDetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();

  private:
    G4double fExpHall_x;
    G4double fExpHall_y;
    G4double fExpHall_z;

    G4double fTank_x;
    G4double fTank_y;
    G4double fTank_z;

    G4double fBubble_x;
    G4double fBubble_y;
    G4double fBubble_z;
};

#endif
