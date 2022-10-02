const double firstPeakG  = 0.5110;
const double secondPeakG = 1.2745;
const int CHANNELS = 4;
const int SAMPLES = 1024;
bool debug = false;
//double minAmplitude[4] = {-480,-480,-480,-480};
//double maxAmplitude[4] = {+480,+480,+480,+480};
//double minAmplitude[4] = {+130,+120,+200,+210};
//double maxAmplitude[4] = {+220,+210,+350,+360};
//double minAmplitude[4] = {  +0,  +0,+200,+210};
//double maxAmplitude[4] = {+480,+480,+350,+360};
double cHDR2 = 340;//145;
double cS12  = 270;
double rHDR2 = 80;//25
double rS12  = 40;

//=========================
int binMaximum(double list[SAMPLES]) {
  int bin = -1;
  double max = -1e10;
  for(int i=0; i!=SAMPLES; ++i)
    if(list[i]>max) {
      max = list[i];
      bin = i;
    }
  return bin;
}
//=========================
int binRisingThreshold(double list[SAMPLES], double thr, int iSt=0, int iEn=SAMPLES) {
  int bin=-1;
  for(int i=iSt; i<iEn; ++i)
    if( list[i] > thr ) {
      bin = i;
      break;
    }
  return bin;
}
//=========================
double findTimeForFixAmplitude(double amp[SAMPLES], double tim[SAMPLES], double thr, int iSt=0, int iEn=SAMPLES) {
  // look from left to right. Asummes rising!
  int bin=-1;
  for(int i=iSt; i<iEn; ++i)
    if( amp[i+1] > thr ) {
      bin = i;
      break;
    }
  if( (bin<1)||(bin>SAMPLES-2) )
    return 0;
  double time = tim[bin] + (thr-amp[bin])/(amp[bin+1]-amp[bin])*(tim[bin+1]-tim[bin]);
  //cout << "TIME   " << tim[bin] << "  " << time << " || fc " << (thr-amp[bin])/(amp[bin+1]-amp[bin]);
  //cout << " || [ " << amp[bin] << " " << thr << " " << amp[bin+1] << "]" << endl;
  return time;
}
//=========================
//=========================
//=========================
int timeDRS(int evt=-1, TString inputfile="20201002_hdr2/HDR43.3_S1270_Na22.dat",
	    int dofit=0) {
  std::cout << "=====================" << std::endl;
  std::cout << "= Source file: " << inputfile.Data() << std::endl;
  std::cout << "=====================" << std::endl;
  std::ifstream inf( inputfile.Data() , std::ifstream::binary);
  
  int color[4] = { kOrange-3, kCyan-3, kMagenta-3, kGreen-3 };
  int nevt = 0;

  ////////////////////////////
  // RUN HEADER
  char  *c4Tmp = new char [4];
  char  *c2Tmp = new char [2];
  int   iTmp;
  inf.read(c4Tmp, 4);
  if(inf.rdstate()!=0) {
    std::cout << " Error while reading file. Bye bye" << std::endl;
    return 1;
  }
  std::cout << c4Tmp << " | "; // DRSN
  inf.read(c4Tmp, 4);
  std::cout << c4Tmp << " | "; // TIME
  inf.read(c2Tmp, 2);
  std::cout << c2Tmp << " | "; // B#
  inf.read((char*) &iTmp, 2);
  std::cout << iTmp << std::endl; // BoardNumber
  //looping over each channel getting time
  float tch[CHANNELS][SAMPLES];  // Timing calibration
  for(int ich=0; ich!=CHANNELS; ++ich) {
    inf.read(c4Tmp, 4);
    std::cout << "CHN" << ich << ": " << c4Tmp << std::endl; // C00i
    inf.read((char*) &tch[ich], 4*SAMPLES); // Effective time bin width in ns encoded in 4-byte floating point format
  }
  // moving to axis
  double *taxis = new double[SAMPLES+1];
  taxis[0] = 0;
  for(int isa=0; isa!=SAMPLES; ++isa) {
    taxis[isa+1] = taxis[isa] + tch[0][isa];
  }
  std::cout << "   TIME-AXIS [ " << taxis[0] << ", " << taxis[SAMPLES] << " ]" << std::endl;

  // DONE READING HEADER
  // at this point I know the time configuration for each channel. Good!
  TH1D *hWaveCapture[CHANNELS];
  TH1D *hAmplitude[CHANNELS];
  TH1D *hAmplitudeTime[CHANNELS];
  TH1D *hAmplitudeTime100[CHANNELS];
  TH1D *hAmplitudeTime80[CHANNELS];
  TH1D *hAmplitudeTime50[CHANNELS];
  TH1D *hAmplitudeTime20[CHANNELS];
  TH1D *hAmplitudeTimeLIN[CHANNELS];
  TH2F *hAmplitude_VS_Time[CHANNELS];
  TH2F *hPlusA_VS_PlusT[2];
  TH2F *hPlusA_VS_MinusT[2];
  TH1D *hTimePlus[2];
  TH1D *hTimeMinus[2];
  for(int ich=0; ich!=CHANNELS; ++ich) {
    hWaveCapture[ich] = new TH1D( Form("WaveCapture%d",ich), Form("WaveCapture%d",ich), SAMPLES-1, taxis );
    hAmplitude[ich] = new TH1D( Form("Amplitude%d",ich), Form("Amplitude%d;mV",ich), 1000, -500, +500 );
    hAmplitudeTime[ich] = new TH1D( Form("AmplitudeTime%d",ich), Form("AmplitudeTime%d;channel (0-1023)",ich), SAMPLES, -0.5, SAMPLES-0.5 );
    //different trials
    hAmplitudeTime100[ich] = new TH1D( Form("Time100_%d",ich), Form("Time100_%d;ns",ich), SAMPLES/2, 85, 155 );
    hAmplitudeTime80[ich]  = new TH1D( Form("Time80_%d",ich),  Form("Time80_%d;ns",ich),  SAMPLES, 85, 155 );
    hAmplitudeTime50[ich]  = new TH1D( Form("Time50_%d",ich),  Form("Time50_%d;ns",ich),  SAMPLES, 85, 155 );
    hAmplitudeTime20[ich]  = new TH1D( Form("Time20_%d",ich),  Form("Time20_%d;ns",ich),  SAMPLES, 85, 155 );
    hAmplitudeTimeLIN[ich] = new TH1D( Form("TimeLIN_%d",ich), Form("TimeLIN_%d;ns",ich), SAMPLES, 85, 155 );
    //
    hAmplitude_VS_Time[ich] = new TH2F( Form("Amplitude_VS_Time%d",ich), Form("Amplitude_VS_Time%d;mV;ns",ich),
					100,0,500,100,0,120);
  }
  hTimePlus[0] = new TH1D( "TimePlus01", "TimePlus01;ns", SAMPLES, 0, 2*taxis[SAMPLES-1] );
  hTimePlus[1] = new TH1D( "TimePlus23", "TimePlus23;ns", SAMPLES, 0, 2*taxis[SAMPLES-1] ); 
  hPlusA_VS_PlusT[0] = new TH2F( "PlusA_VS_PlusT01", "PlusA_VS_PlusT01",100,0,600,100,0,+210);
  hPlusA_VS_PlusT[1] = new TH2F( "PlusA_VS_PlusT23", "PlusA_VS_PlusT23",100,0,800,100,0,+210);
  hTimeMinus[0] = new TH1D( "TimeMinus01", "TimeMinus01;ns", 200, -50, +50);
  hTimeMinus[1] = new TH1D( "TimeMinus23", "TimeMinus23;ns", 200, -50, +50);
  hPlusA_VS_MinusT[0] = new TH2F( "PlusA_VS_MinusT01", "PlusA_VS_MinusT01",100,0,600,100,-30,+30);
  hPlusA_VS_MinusT[1] = new TH2F( "PlusA_VS_MinusT23", "PlusA_VS_MinusT23",100,0,800,100,-30,+30);
  TH2F *hAvgAHDR2_VS_AvgAS12 = new TH2F( "AvgAHDR2_VS_AvgAS12", "AvgAHDR2_VS_AvgAS12;HDR2 (mV);S12 (mV)",100,0,500,100,0,500);
  TH2F *hT20_0_VS_1 = new TH2F( "T20_0_VS_1", "T20_0_VS_1;T0 (ns);T1 (ns)",
				int((110-90)/tch[0][0]),90,110,
				int((110-90)/tch[0][0]),90,110);
  TH2F *hT20_2_VS_3 = new TH2F( "T20_2_VS_3", "T20_2_VS_3;T2 (ns);T3 (ns)",
				int((110-90)/tch[0][0]),90,110,
				int((110-90)/tch[0][0]),90,110);

  TH2F *hT20_HDR2_VS_S12 = new TH2F( "T20_HDR2_VS_S12", "T20_HDR2_VS_S12;HDR2 (ns);S12 (ns)",
				     int((110-90)/tch[0][0]),90,110,
				     int((110-90)/tch[0][0]),90,110);
  ////////////////////////////
  // TRAILER
  char *c5Tmp = new char[5];
  short s8Tmp[8];
  ushort ch[CHANNELS][SAMPLES]; //reading from file
  double chmV[CHANNELS][SAMPLES]; //This is the voltage recorded on channel 1 in mV
  double chns[CHANNELS][SAMPLES]; //This is the time recorded on channel 1 in ns
  while(!inf.eof()) {
    if (nevt%10000==0) {
      std::cout << "Processing event #" << nevt << " ";
      std::cout << inf.rdstate() << std::endl;
    }
    //EVENT HEADER
    inf.read(c4Tmp, 4);
    if(debug) std::cout << c4Tmp << " | "; //EHDR
    inf.read((char *) &iTmp, 4);
    if(debug) std::cout << iTmp << " | ["; //EventNumber
    inf.read((char *) &iTmp, 2);
    if(debug) std::cout << iTmp << "/"; //Year
    inf.read((char *) &iTmp, 2);
    if(debug) std::cout << iTmp << "/"; //Month
    inf.read((char *) &iTmp, 2);
    if(debug) std::cout << iTmp << " "; //Day
    inf.read((char *) &iTmp, 2);
    if(debug) std::cout << iTmp << ":"; //Hour
    inf.read((char *) &iTmp, 2);
    if(debug) std::cout << iTmp << ":"; //Minute
    inf.read((char *) &iTmp, 2);
    if(debug) std::cout << iTmp << "::"; //Second
    inf.read((char *) &iTmp, 2);
    if(debug) std::cout << iTmp << "] | "; //Milisecond
    inf.read((char *) &iTmp, 2);
    if(debug) std::cout << iTmp << " | "; //RangeCenter in mV
    int rc = iTmp;
    inf.read(c2Tmp, 2);
    if(debug) std::cout << c2Tmp << " "; // B#
    inf.read((char*) &iTmp, 2);
    if(debug) std::cout << iTmp << " | "; // BoardNumber
    inf.read(c2Tmp, 2);
    if(debug) std::cout << c2Tmp << " "; // T#
    inf.read((char*) &iTmp, 2);
    if(debug) std::cout << iTmp << std::endl; // TriggerCell
    int tcell = iTmp; // very important for time allignment
    //Reading the values of each sample in the buffer for an event
    double refTime[4];
    double refAmplitude[4];
    for(int ich=0; ich!=CHANNELS; ++ich) {
      inf.read(c4Tmp, 4); // C00i
      inf.read((char*) &iTmp, 4); // Scaler for channel in Hz
      inf.read((char*) &ch[ich], SAMPLES*2); // SAMPLES*2 (2-byte integer. 0=RC-0.5V and 65535=RC+0.5V. RC see header.
      //translate to human
      for(int isa=0; isa!=SAMPLES; ++isa) {
	//Converting raw value to mV
	chmV[ich][isa] = -1*(1000*(ch[ich][isa]/65535.-0.5)+rc);
	//std::cout << "******> " << chmV[ich][isa] << std::endl;
	hWaveCapture[ich]->SetBinContent(isa+1,chmV[ich][isa]);
	//Computing real time
	if(0) { //takes a lot of time (disabling it for now)
	  chns[ich][isa] = 0;
	  for(int jsa=0; jsa<isa; ++jsa) {
	    chns[ich][isa] += tch[ich][(jsa+tcell)%SAMPLES];
	  }
	}
	//cout << " *** " << isa << " *** " << taxis[isa] << " VS " << chns[ich][isa] << endl;
      }
    }
    //allign cell #0 of all channels // copied from read_binary.cpp DRS source code
    if(0) { //takes a lot of time (disabling it for now)
      double t1, t2, dt;
      t1 = chns[0][(SAMPLES-tcell)%SAMPLES];
      for(int chn=1 ; chn!=CHANNELS; ++chn) {
	t2 = chns[chn][(SAMPLES-tcell)%SAMPLES];
	dt = t1 - t2;
	for(int i=0 ; i!=SAMPLES ; ++i)
	  chns[chn][i] += dt;
      }
    }
    //on-the-fly analysis
    // FAST REJECTION OF FULL EVENT
    bool goodEvent = true;
    for(int ich=0; ich!=CHANNELS; ++ich) {
      int bin = binMaximum( chmV[ich] );
      double amplitude = chmV[ich][bin];
      refAmplitude[ich] = amplitude;
      // apply cut on amplitude
      //if( (!goodEvent)||(amplitude<minAmplitude[ich])||(amplitude>maxAmplitude[ich]) ) {
      //	goodEvent = false;
      //	break;
      //}
    }
    double avgHDR2 = (refAmplitude[0] + refAmplitude[1])/2;
    double avgS12 = (refAmplitude[2] + refAmplitude[3])/2;
    double rad2Event = pow( (avgHDR2-cHDR2)/rHDR2 ,2) + pow( (avgS12-cS12)/rS12 ,2);
    if(rad2Event>1) goodEvent= false;
    // 
    if(goodEvent) {
      for(int ich=0; ich!=CHANNELS; ++ich) {
	int bin = binMaximum( chmV[ich] );
	double amplitude = chmV[ich][bin];
	// SIMPLE PLOTS
	hAmplitudeTime[ich]->Fill( bin );
	hAmplitude[ich]->Fill( amplitude );
	double sqt = 0;
	double sq = 0;
	for(int acc=bin-10; acc!=bin+20; ++acc) { // -10 (5ns) +20 (10ns)
	  sq += chmV[ich][acc];	
	  sqt += chmV[ich][acc]*taxis[acc]; // fix-time!
	}
	hAmplitudeTime100[ich]->Fill( sqt/sq );
	double amplitudeAVG = sq/31;
	//cout << " AMPL " << amplitude << "  " << amplitudeAVG << endl;
	double time80 = findTimeForFixAmplitude( chmV[ich], taxis, 0.80*amplitudeAVG ); //fix-time!
	double time20 = findTimeForFixAmplitude( chmV[ich], taxis, 0.20*amplitudeAVG ); //fix-time!
	double time50 = findTimeForFixAmplitude( chmV[ich], taxis, 0.50*amplitudeAVG ); //fix-time!
	int binThr;
	//binThr = binRisingThreshold( chmV[ich], 0.80*amplitudeAVG );
	//hAmplitudeTime80[ich]->Fill( taxis[binThr] ); //fix-time!
	hAmplitudeTime80[ich]->Fill( time80 );
	//binThr = binRisingThreshold( chmV[ich], 0.20*amplitudeAVG );
	//hAmplitudeTime20[ich]->Fill( taxis[binThr] ); //fix-time!
	hAmplitudeTime20[ich]->Fill( time20 ); //fix-time!
	binThr = binRisingThreshold( chmV[ich], 0.50*amplitudeAVG );
	//hAmplitudeTime50[ich]->Fill( taxis[binThr] ); //fix-time!
	hAmplitudeTime50[ich]->Fill( time50 ); //fix-time!
	//cout << " *CHN " << ich <<  " BIN " << bin << " REFTIME " << chns[ich][bin];
	//cout << " ANOTHER " << hWaveCapture[ich]->GetBinCenter(bin) << endl;
	if(0) 
	  refTime[ich] = chns[ich][binThr];
	else
	  refTime[ich] = taxis[binThr]; // fix-time!
	refAmplitude[ich] = amplitude;
	hAmplitude_VS_Time[ich]->Fill( refAmplitude[ich], refTime[ich] );
      }
      // CORRELATION PLOTS
      hT20_0_VS_1->Fill( refTime[0], refTime[1] );
      hT20_2_VS_3->Fill( refTime[2], refTime[3] );
      hT20_HDR2_VS_S12->Fill( 0.5*(refTime[0]+refTime[1]), 0.5*(refTime[2]+refTime[3])  );

      
      hTimePlus[0]->Fill( refTime[0]+refTime[1] );
      hTimePlus[1]->Fill( refTime[2]+refTime[3] );
      hTimeMinus[0]->Fill( refTime[0]-refTime[1] );
      hTimeMinus[1]->Fill( refTime[2]-refTime[3] );
      hPlusA_VS_PlusT[0]->Fill( refAmplitude[0]+refAmplitude[1], refTime[0]+refTime[1] );
      hPlusA_VS_PlusT[1]->Fill( refAmplitude[2]+refAmplitude[3], refTime[2]+refTime[3] );
      hPlusA_VS_MinusT[0]->Fill( refAmplitude[0]+refAmplitude[1], refTime[0]-refTime[1] );
      hPlusA_VS_MinusT[1]->Fill( refAmplitude[2]+refAmplitude[3], refTime[2]-refTime[3] );
      hAvgAHDR2_VS_AvgAS12->Fill( (refAmplitude[0]+refAmplitude[1])/2, (refAmplitude[2]+refAmplitude[3])/2  );
      
    }
    if(evt==nevt) break;
    nevt++;
  }
  inf.close();
  std::cout << "FOUND " << nevt << " EVENTS." << std::endl;

  //gStyle->SetOptStat(0);

  TCanvas *main0 = new TCanvas();
  TCanvas *main1 = new TCanvas();
  TCanvas *main2 = new TCanvas();
  TCanvas *main3 = new TCanvas();
  main0->Divide(2,2);
  main1->Divide(2,2);
  main2->Divide(2,2);
  main3->Divide(2,2);
  for(int ich=0; ich!=CHANNELS; ++ich) {
    main0->cd(ich+1);
    hWaveCapture[ich]->Draw();
    main1->cd(ich+1);
    hAmplitude[ich]->Draw();
    main2->cd(ich+1);
    hAmplitudeTime[ich]->Draw();
    main3->cd(ich+1);
    hAmplitude_VS_Time[ich]->Draw("colz");
  }

  TCanvas *main100 = new TCanvas();
  main100->Divide(2,2);
  TLegend *leg100[4];
  TF1 *fitT100[CHANNELS];
  TF1 *fitT80[CHANNELS];
  TF1 *fitT50[CHANNELS];
  TF1 *fitT20[CHANNELS];
  for(int ich=0; ich!=CHANNELS; ++ich) {
    main100->cd(ich+1);
    hAmplitudeTime20[ich]->SetLineColor(kBlue-3);
    hAmplitudeTime50[ich]->SetLineColor(kGreen-3);
    hAmplitudeTime80[ich]->SetLineColor(kOrange-3);
    hAmplitudeTime100[ich]->SetLineColor(kRed-3);

    fitT100[ich] = new TF1( Form("fitT100_%d",ich), "gaus" );
    fitT100[ich]->SetLineColor(kRed-3);
    hAmplitudeTime100[ich]->Rebin(4);
    hAmplitudeTime100[ich]->Fit( fitT100[ich], "I" );
    fitT80[ich] = new TF1( Form("fitT80_%d",ich), "gaus" );
    fitT80[ich]->SetLineColor(kOrange-3);
    hAmplitudeTime80[ich]->Fit( fitT80[ich], "I" );
    fitT50[ich] = new TF1( Form("fitT50_%d",ich), "gaus" );
    fitT50[ich]->SetLineColor(kGreen-3);
    hAmplitudeTime50[ich]->Fit( fitT50[ich], "I" );
    fitT20[ich] = new TF1( Form("fitT20_%d",ich), "gaus" );
    fitT20[ich]->SetLineColor(kBlue-3);
    hAmplitudeTime20[ich]->Fit( fitT20[ich], "I" );

    hAmplitudeTime100[ich]->Draw();
    hAmplitudeTime80[ich]->Draw("SAME");
    hAmplitudeTime50[ich]->Draw("SAME");
    hAmplitudeTime20[ich]->Draw("SAME");
    leg100[ich] = new TLegend(0.26,0.6,0.66,0.9,Form("Channel %d",ich));
    leg100[ich]->AddEntry( hAmplitudeTime100[ich],Form("Time@100%% -> #sigma = %.0f ps",fitT100[ich]->GetParameter(2)*1000) );
    leg100[ich]->AddEntry( hAmplitudeTime80[ich], Form("Time@80%% -> #sigma = %.0f ps",fitT80[ich]->GetParameter(2)*1000) );
    leg100[ich]->AddEntry( hAmplitudeTime50[ich], Form("Time@50%% -> #sigma = %.0f ps",fitT50[ich]->GetParameter(2)*1000) );
    leg100[ich]->AddEntry( hAmplitudeTime20[ich], Form("Time@20%% -> #sigma = %.0f ps",fitT20[ich]->GetParameter(2)*1000) );
    leg100[ich]->Draw();
    hAmplitudeTime100[ich]->GetYaxis()->SetRangeUser(0, 1.5*hAmplitudeTime100[ich]->GetMaximum() );
  }
  
  TCanvas *main10 = new TCanvas();
  main10->Divide(2,2);
  main10->cd(1); hTimePlus[0]->Draw();
  main10->cd(2); hTimePlus[1]->Draw();
  main10->cd(3); hTimeMinus[0]->Draw();
  main10->cd(4); hTimeMinus[1]->Draw();

  TCanvas *main11 = new TCanvas();
  main11->Divide(2,2);
  TProfile *proPAPT01 = (TProfile*) hPlusA_VS_PlusT[0]->ProfileX("proPAPT01");
  TProfile *proPAPT23 = (TProfile*) hPlusA_VS_PlusT[1]->ProfileX("proPAPT23");
  TProfile *proPAMT01 = (TProfile*) hPlusA_VS_MinusT[0]->ProfileX("proPAMT01");
  TProfile *proPAMT23 = (TProfile*) hPlusA_VS_MinusT[1]->ProfileX("proPAMT23");
  main11->cd(1); hPlusA_VS_PlusT[0]->Draw("colz"); proPAPT01->Draw("SAME");
  main11->cd(2); hPlusA_VS_PlusT[1]->Draw("colz"); proPAPT23->Draw("SAME");
  main11->cd(3); hPlusA_VS_MinusT[0]->Draw("colz"); proPAMT01->Draw("SAME");
  main11->cd(4); hPlusA_VS_MinusT[1]->Draw("colz"); proPAMT23->Draw("SAME");

  TCanvas *main12 = new TCanvas();
  hAvgAHDR2_VS_AvgAS12->Draw("colz");
  
  TCanvas *main13 = new TCanvas();
  main13->Divide(2);
  main13->cd(1); hT20_0_VS_1->Draw("colz");
  main13->cd(2); hT20_2_VS_3->Draw("colz");

  TCanvas *main14 = new TCanvas();
  hT20_HDR2_VS_S12->Draw("colz");
  
  return 0;
}
