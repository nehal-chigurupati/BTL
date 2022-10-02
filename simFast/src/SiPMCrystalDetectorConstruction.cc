#include "SiPMCrystalDetectorConstruction.hh"
#include <iostream>
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

SiPMCrystalDetectorConstruction::SiPMCrystalDetectorConstruction() : G4VUserDetectorConstruction() {
  fExpHall_x = fExpHall_y = fExpHall_z = 10.0*m;
  fTank_x    = fTank_y    = fTank_z    =  5.0*m;
  fBubble_x  = fBubble_y  = fBubble_z  =  0.5*m;
}

SiPMCrystalDetectorConstruction::~SiPMCrystalDetectorConstruction(){;}

G4VPhysicalVolume* SiPMCrystalDetectorConstruction::Construct() {
  std::cout << "SiPMCrystalDetectorConstruction::Construct" << std::endl;
  // ------------- Materials -------------
  G4NistManager* man = G4NistManager::Instance();
  man->SetVerbose(1);

  G4double pho_ene[5] = { 1.91*eV, 2.25*eV, 2.75*eV, 3.10*eV, 3.54*eV};

  // Air
  G4Material* air = new G4Material("Air", 1.29*mg/cm3, 2);
  {
    // Composition
    G4Element* N = new G4Element("Nitrogen", "N", 7 , 14.01*g/mole);
    G4Element* Ox = new G4Element("Oxygen"  , "O", 8 , 16.00*g/mole);
    air->AddElement(N, 70.*perCent);
    air->AddElement(Ox,30.*perCent);
    // Optical Properties
    G4MaterialPropertiesTable *air_mpt = new G4MaterialPropertiesTable();
    G4double pho_100[5] = {    1.00,    1.00,    1.00,    1.00,    1.00};
    air_mpt->AddProperty("RINDEX",    pho_ene, pho_100, 5)->SetSpline(true);
    //air_mpt->AddConstProperty("RINDEX",1.0);
    air->SetMaterialPropertiesTable(air_mpt);
  }
  // LYSO
  G4Material *LYSO = new G4Material( "LYSO", 7.1*g/cm3, 4 ); // SAINT-GOBAIN
  {
    // Composition
    G4Element *Lu = man->FindOrBuildElement("Lu");
    G4Element *Y  = man->FindOrBuildElement("Y");
    G4Element *Si = man->FindOrBuildElement("Si");
    G4Element *O  = man->FindOrBuildElement("O");
    LYSO->AddElement( Lu, 71*perCent ); // NEEDS VERIFICATION!
    LYSO->AddElement( Y,   4*perCent ); // NEEDS VERIFICATION!
    LYSO->AddElement( Si,  6*perCent ); // NEEDS VERIFICATION!
    LYSO->AddElement( O,  19*perCent ); // NEEDS VERIFICATION!
    // Optical Properties
    G4MaterialPropertiesTable* lyso_mpt = new G4MaterialPropertiesTable();
    lyso_mpt->AddConstProperty("SCINTILLATIONYIELD",33200/MeV);
    lyso_mpt->AddConstProperty("RESOLUTIONSCALE",1.0);
    //lyso_mpt->AddConstProperty("RINDEX",1.81); // should change to energy dependant
    //lyso_mpt->AddConstProperty("ABSLENGTH",1.2*cm); // should change to energy dependant
    lyso_mpt->AddConstProperty("YIELDRATIO",1.0); // no slow component
    lyso_mpt->AddConstProperty("FASTTIMECONSTANT",45.*ns);
    lyso_mpt->AddConstProperty("FASTSCINTILLATIONRISETIME", 1.*ns);
    G4double pho_cmp[5] = {    0.00,    0.05,    0.25,    0.15,    0.00};
    G4double pho_rix[5] = {    1.81,    1.81,    1.81,    1.81,    1.81};
    G4double pho_ale[5] = {  1.2*cm,  1.2*cm,  1.2*cm,  1.2*cm,  1.2*cm};
    lyso_mpt->AddProperty("FASTCOMPONENT", pho_ene, pho_cmp, 5)->SetSpline(true);
    lyso_mpt->AddProperty("RINDEX",    pho_ene, pho_rix, 5)->SetSpline(true);
    lyso_mpt->AddProperty("ABSLENGTH", pho_ene, pho_ale, 5)->SetSpline(true);
    LYSO->SetMaterialPropertiesTable( lyso_mpt );
    LYSO->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
  }

  // ------------- Volumes --------------

  //The experimental Hall
  G4Box* expHall_box = new G4Box( "World", 3.5*mm, 3.5*mm, 5.5*cm );
  G4LogicalVolume *expHall_log =
    new G4LogicalVolume( expHall_box, air, "World", 0, 0, 0 );
  G4VPhysicalVolume *expHall_phys =
    new G4PVPlacement( 0, G4ThreeVector(), expHall_log, "Mundo", 0, false, 0 );

  //LYSO
  G4Box* lyso_box = new G4Box("lyso", 3.3/2*mm, 3.3/2*mm, 5.5/2*cm);
  G4LogicalVolume *lyso_log =
    new G4LogicalVolume( lyso_box, LYSO, "lyso", 0, 0, 0 );
  //G4VPhysicalVolume *lyso_phy =
  new G4PVPlacement( 0, G4ThreeVector(), lyso_log, "Cristal", expHall_log, false, 0 );

  //SiPM
  G4Box* sipm_box = new G4Box("sipm", 3.3/2*mm, 3.3/2*mm, 2.0/2*mm);
  G4LogicalVolume *sipm_log =
    new G4LogicalVolume( sipm_box, air, "sipm", 0, 0, 0 );
  //G4VPhysicalVolume *sipm1_phy =
  new G4PVPlacement( 0, G4ThreeVector(0,0,5.5/2*cm+2.0/2*mm), sipm_log, "SiPM1", expHall_log, false, 0 );
  //G4VPhysicalVolume *sipm2_phy =
  new G4PVPlacement( 0, G4ThreeVector(0,0,-5.5/2*cm-2.0/2*mm), sipm_log, "SiPM2", expHall_log, false, 1 );

  
  // ------------- Surfaces --------------
  //LYSO
  //G4LogicalBorderSurface* lyso_teflon_bs =
  //  new G4LogicalBorderSurface("lyson_teflon_bs", lyso_phy, expHall_phys, lyso_teflon_os);
  /*
  G4OpticalSurface* lyso_teflon_os = new G4OpticalSurface("lyso_teflon_os");
  lyso_teflon_os->SetType(dielectric_LUT);
  lyso_teflon_os->SetType(dielectric_LUT);
  lyso_teflon_os->SetModel(DAVIS);
  lyso_teflon_os->SetFinish(PolishedTeflon_LUT);
  //G4LogicalSkinSurface* lyso_teflon_ss =
  //  new G4LogicalSkinSurface("lyson_teflon_ss", lyso_log, lyso_teflon_os);

  G4double pp[2] = {1*eV, 4*eV};
  G4double reflectivity[2] = {1., 1.};
  G4double efficiency[2] = {0.0, 0.0};
  G4MaterialPropertiesTable* wrapperProperty = new G4MaterialPropertiesTable();
  wrapperProperty->AddProperty("REFLECTIVITY", pp, reflectivity, 2);
  wrapperProperty->AddProperty("EFFICIENCY", pp, efficiency, 2);
  wrapperProperty->AddConstProperty("RINDEX",1.3);
  lyso_teflon_os->SetMaterialPropertiesTable(wrapperProperty);
  */
  
  std::cout << "SiPMCrystalDetectorConstruction::Construct DONE" << std::endl;
  return expHall_phys;
}
