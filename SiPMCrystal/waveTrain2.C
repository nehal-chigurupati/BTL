const int MAX_WAVES = 500;
const double firstPeak = 0.511;
const double secondPeak = 1.275;

vector<TH1D*> readWave2(TString file) {
  TString outfilestring = Form("%s",file.Data());
  std::ifstream fin( outfilestring.Data() );
  std::string line;
  vector<TH1D *> ret;
  for(int i=0; i!=MAX_WAVES; ++i) {
    TH1D *tmp = new TH1D( Form("Wave_%d",i), Form("Wave_%d;Time  (ps);Signal  (mV)",i),
			  10002,-2.480426e02,2.52027e02);
    ret.push_back( tmp );
  }
  // removing header
  for(int n=0;n!=3;++n) {
    if(!std::getline(fin,line)) break;
  }
  for(int n=0;n!=500;++n) {
    if(!std::getline(fin,line)) break;
  }
  for(int n=0;n!=1;++n) {
    if(!std::getline(fin,line)) break;
  }
  for(int x=0;x!=MAX_WAVES;++x) {
    for(int y=0;y!=10002;++y) {
      if(!std::getline(fin,line)) break;
      TString rline = line;
      TObjArray *lst = rline.Tokenize(",");
      double ix =  ((TObjString*) lst->At(0))->GetString().Atof()*1e9; // nS
      double iv =  ((TObjString*) lst->At(1))->GetString().Atof()*1e3; // mV
      ret[x]->SetBinContent(y+1,iv);
    }
    if(x>=MAX_WAVES) break;
  }
  return ret;
}

int waveTrain(TString dirname="20200920_Na22OldSiPM/C1--na22-test--00000.csv", int col=0) {
  gStyle->SetOptStat(0);
  std::cout << "Scanning for waveform files... " << std::endl;
  vector<TH1D*> fAll;
  fAll = readWave2(dirname);

  int color[4] = { kOrange-3, kCyan-3, kMagenta-3, kGreen-3 };
  TH2D *axis = new TH2D("axis","OSC;Time  (ns);Signal  (mV)",100,-2.480426e02,2.52027e02,100,-100,+50);
  TH1D *integral = new TH1D("charge","charge;mV*nS",100,-1.1e5,0);
  //TH1D *integral = new TH1D("charge","charge;mV*pS",100,-1e5,0);
  //TH1D *integral = new TH1D("charge","charge;mV*pS",100,-6e4,0);
  integral->SetLineColor(kBlack);
  integral->SetFillColor(color[col]);
  TCanvas *main = new TCanvas();
  axis->Draw();
  for(int i=0; i!=MAX_WAVES; ++i) {
    fAll[i]->SetFillColor(kWhite);
    fAll[i]->SetLineColor(color[col]);
    fAll[i]->Draw("SAME");
    int binCenter = fAll[i]->GetMinimumBin();
    int binMin = binCenter - 1000;//500;//100;
    int binMax = binCenter + 5000;//2500;//500;
    cout << binMin << " " << binCenter << " " << binMax << "    ";
    cout << fAll[i]->GetBinCenter(binMin) << " ";
    cout << fAll[i]->GetBinCenter(binCenter)<< " ";
    cout << fAll[i]->GetBinCenter(binMax) << endl;
    double val = fAll[i]->Integral( binMin, binMax );
    integral->Fill(val);
  }
  main->SaveAs( Form("%s_OSC.png",dirname.Data()), "PNG" );
  TCanvas *main2 = new TCanvas();
  integral->Draw();
  TF1 *fit = new TF1("fit",Form("[0]*TMath::Gaus(x,[1],[2],1)+[3]*TMath::Gaus(x,[1]*%f,[4],1)",secondPeak/firstPeak));
  fit->SetParLimits(0,1,1e6);
  fit->SetParLimits(3,1,1e6);
  fit->SetParLimits(2,1,1e5);
  fit->SetParLimits(4,1,1e5);
  fit->SetParLimits(1,-5e4,-2e4);
  integral->Fit(fit);
  main2->SaveAs( Form("%s_CHARGE.pdf",dirname.Data()), "PDF" );

  return 0;
}
