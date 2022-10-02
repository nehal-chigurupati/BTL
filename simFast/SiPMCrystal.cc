#include "G4Types.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalProcessIndex.hh"
#include "G4EmStandardPhysics_option4.hh"

#include "SiPMCrystalDetectorConstruction.hh"
#include "SiPMCrystalActionInitialization.hh"
#include "SiPMCrystalPrimaryGeneratorAction.hh"
#include "SiPMCrystalRunAction.hh"
#include "SiPMCrystalSteppingAction.hh"
#include "SiPMCrystalStackingAction.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " SiPMCrystal [-m macro ] [-u UIsession] [-t nThreads] [-r seed] "
           << G4endl;
    G4cerr << "   note: -t option is available only for multi-threaded mode."
           << G4endl;
  }
}

int main(int argc,char** argv) {
  // Evaluate arguments
  //
  if ( argc > 9 ) {
    PrintUsage();
    return 1;
  }
  G4String macro;
  G4String session;

  G4long myseed = 345354;
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro   = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
    else if ( G4String(argv[i]) == "-r" ) myseed  = atoi(argv[i+1]);
    else {
      PrintUsage();
      return 1;
    }
  }

  // Instantiate G4UIExecutive if interactive mode
  G4UIExecutive* ui = nullptr;
  if ( macro.size() == 0 ) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  //
  G4RunManager * runManager = new G4RunManager;

  // Seed the random number generator manually
  G4Random::setTheSeed(myseed);

  // Set mandatory initialization classes
  //
  // Detector construction
  runManager-> SetUserInitialization(new SiPMCrystalDetectorConstruction());
  // Physics list
  G4VModularPhysicsList *physicsList = new FTFP_BERT;
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  G4OpticalPhysics *opticalPhysics = new G4OpticalPhysics();
  opticalPhysics->SetTrackSecondariesFirst( G4OpticalProcessIndex::kScintillation, true );
  physicsList->RegisterPhysics(opticalPhysics);


  runManager-> SetUserInitialization(physicsList);

  // User action initialization
  //runManager->SetUserInitialization(new SiPMCrystalActionInitialization());
  runManager->SetUserAction(new SiPMCrystalPrimaryGeneratorAction());
  runManager->SetUserAction(new SiPMCrystalRunAction());
  //SiPMCrystalSteppingAction *step_action = new SiPMCrystalSteppingAction();
  runManager->SetUserAction(new SiPMCrystalSteppingAction());
  runManager->SetUserAction(new SiPMCrystalStackingAction());
  
  // Initialize visualization
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if ( macro.size() ) {
     // Batch mode
     G4String command = "/control/execute ";
     UImanager->ApplyCommand(command+macro);
  }
  else // Define UI session for interactive mode
  {
     UImanager->ApplyCommand("/control/execute vis.mac");
     if (ui->IsGUI())
        UImanager->ApplyCommand("/control/execute gui.mac");
     ui->SessionStart();
     delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

  delete visManager;
  delete runManager;

  return 0;
}

