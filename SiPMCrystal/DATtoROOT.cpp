#include <fstream>
#include <iostream>
#include <ios>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include <string.h>

#include "TString.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMath.h"
#include "TF1.h"
#include "TGraph.h"
#include "TCanvas.h"


double findRMS(double ch[1024], float t[1024], float lowtime, float hightime);
double findmean(double ch[1024], float t[1024], float lowtime, float hightime);
double findAmp(double ch[1024], float t[1024], float lowtime, float hightime);

//int convert_test(int argc, char **argv)
int convert_test(const char* name, int Nchan)
{
  //  int Nchan = 3;
  //Defining pulse window

  float tlow = 10;  //ns
  float thigh = 190; //ns

  //checking that there is a file specified for processing and then opening it.
  std::ifstream infile;
  infile.open(name, std::ios::binary | std::ios::in);
  std::cout << "Opening file " << name << " ...................." << std::endl;
  /*
    //This section was intended to make things easier and more general, but I couldn't get it to work...  
  if (argc == 2)
    {
      
      infile.open(argv[1], std::ios::binary | std::ios::in);
      //infile.open("SiPM-67V-Dark-RedAdvatech.dat", std::ios::binary | std::ios::in);
      std::cout << "Opening file " << argv[1] << " ............." << std::endl;
      std::cout << std::endl;
      if (!infile.is_open())
	{
	  std::cerr << "!! Error opening file: " << argv[1] << std::endl;
	  return 1;
	}
    }
  else
    {
      std::cerr << "!! There is no input file!!" << std::endl;
      return 1;
    }  
  int file_len = strlen(argv[1]); //determining length of the input file name 
  string filename = argv[1];  // initializing output filename to be same as input filename
  filename.replace(file_len -3, 3, "root"); //replacing "dat" at the end with "root"
*/


  //Creating a root file with the same name as the dat file
  int file_len = strlen(name);
  string filename = name;
  filename.replace(file_len -3, 3, "root");

  //Initializing the output ROOT file
  TFile *outfile = new TFile ((char *) filename.c_str (), "recreate");
  std::cout << "Creating ROOT file " << filename << " ......." << std::endl;
  std::cout << std::endl;

  //declaring variables to be used in ROOT file
  double chmV[Nchan][1024]; //This is the voltage recorded on channel 1 in mV
  float time[Nchan][1024];  //This is the time of a sample in ns with time[0] = 0
  int event = 0;
  double RMS[Nchan];  //RMS of pedestal
  double amp[Nchan];
  double mean[Nchan];
  double integral[Nchan];

  //Defining the function we will use to fit the pulses (Landau function)
  //  TF1 *fn1 = new TF1("ourfn", "landau(0)");
  TString s;
  //Initializing the Tree and branches in the ROOT file
  TTree *pulse = new TTree("pulse", "This is a tree");
  s.Form("chmV[%i][1024]/D",Nchan);
  TBranch *ch_b = pulse->Branch("chmV", &chmV, s);
  s.Form("time[%i][1024]/F",Nchan);
  TBranch *time_b = pulse->Branch("time", &time, s);  
  TBranch *event_b = pulse->Branch("event", &event, "event/I");
  s.Form("RMS[%i]/D",Nchan);
  TBranch *RMS_b = pulse->Branch("RMS", &RMS, s);
  s.Form("amp[%i]/D",Nchan);
  TBranch *amp_b = pulse->Branch("amp", &amp, s);
  s.Form("mean[%i]/D",Nchan);
  TBranch *mean_b = pulse->Branch("mean", &mean,s);
  s.Form("integral[%i]/D",Nchan);
  TBranch *integral_b = pulse->Branch("integral", &integral, s);  
