const int MAX_WAVES = 500;
const double firstPeak = 0.511;
const double secondPeak = 1.275;

int waveTrain(TString file="20200920_Na22OldSiPM/C1--na22-test--00000.csv",bool nofit=false) {
  int col=0;
  gStyle->SetOptStat(0);
  double minX = -2.480426e02;
  double maxX = +2.520270e02;
  TH1D* fTmp = new TH1D( "Wave", "Wave;Time  (ns);Signal  (mV)", 10002, minX, maxX );
  int color[4] = { kOrange-3, kCyan-3, kMagenta-3, kGreen-3 };
  TH2D *axis = new TH2D("axis",Form("File: %s;Time  (ns);Signal  (mV)",file.Data()),300,minX,maxX,150,-100,+50);
  TH1D *integral = new TH1D("charge",Form("File: %s;charge  (a.u.)",file.Data()),50,-2.1e5,0);
  std::cout << "Scanning for waveform files... " << std::endl;
  std::ifstream fin( file.Data() );
  std::string line;
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
  double meanrms = 0;
  int nsamp = 2000;
  for(int x=0;x!=MAX_WAVES;++x) {
    double xCheckMin, xCheckMax;
    fTmp->Reset();
    double sxx = 0;
    for(int y=0;y!=10002;++y) {
      if(!std::getline(fin,line)) break;
      TString rline = line;
      TObjArray *lst = rline.Tokenize(",");
      double ix =  ((TObjString*) lst->At(0))->GetString().Atof()*1e9; // nS
      double iv =  ((TObjString*) lst->At(1))->GetString().Atof()*1e3; // mV
      fTmp->SetBinContent(y+1,iv);
      axis->Fill(ix,iv);
      if(y==0) xCheckMin = ix;
      if(y==10001) xCheckMax = ix;
      if(y<nsamp) {
	sxx += iv*iv;
      }
    }
    meanrms += sqrt( (sxx/nsamp) ); // adding rms of wave x
    int binCenter = fTmp->GetMinimumBin();
    int binMin = binCenter - 1000;//500;//100;
    int binMax = binCenter + 5000;//2500;//500;
    //cout << binMin << " " << binCenter << " " << binMax << "    ";
    //cout << fTmp->GetBinCenter(binMin) << " ";
    //cout << fTmp->GetBinCenter(binCenter)<< " ";
    //cout << fTmp->GetBinCenter(binMax) << endl;
    double val = fTmp->Integral( binMin, binMax );
    integral->Fill(val);
    if( abs((xCheckMax-xCheckMin) - (maxX-minX)) > 0.1 )
      std::cout << Form("ERROR: in wave %d || expected range[%.1f,%.1f] vs read[%.1f,%.1f]",x,minX,maxX,xCheckMin,xCheckMax) << std::endl;
    if(x>=MAX_WAVES) break;
    if(x%100==0) std::cout << x << " waves processed so far..." << std::endl;
  }
  meanrms /= MAX_WAVES;
  std::cout << "<RMS> = " << meanrms << std::endl;
  //TH1D *integral = new TH1D("charge","charge;mV*pS",100,-1e5,0);
  //TH1D *integral = new TH1D("charge","charge;mV*pS",100,-6e4,0);
  integral->SetLineColor(color[col]);
  integral->SetMarkerColor(color[col]);
  integral->SetMarkerStyle(24);

  TLatex *tex = new TLatex();
  TBox *lin = new TBox();

  TCanvas *main = new TCanvas();
  main->SetLogz(1);
  axis->Draw("colz");
  tex->SetTextSize(0.04);
  tex->SetTextColor( kBlack );
  tex->DrawLatexNDC(0.13,0.18, Form("< RMS_{%d} > = %.2f mV",nsamp,meanrms) );
  double nsamp_x = fTmp->GetXaxis()->GetBinLowEdge(nsamp+1);
  cout << " ***** " << nsamp_x << endl;
  lin->SetFillColor(kBlack);
  lin->DrawBox(minX,-90,nsamp_x,-95);  
  main->SaveAs( Form("%s_OSC.png",file.Data()), "PNG" );
  TCanvas *main2 = new TCanvas();
  integral->Draw("E");
  if(!nofit) {
    TF1 *fit = new TF1("fit",Form("[4]*([0]*TMath::Gaus(x,[1],[2],1)+(1-[0])*TMath::Gaus(x,[1]*%f,[3],1))",secondPeak/firstPeak),-1.1e5,0);
    double scale = MAX_WAVES*integral->GetBinWidth(1);
    fit->SetParameter(4,scale); fit->SetParLimits(4,scale*0.8,scale*1.2);
    fit->SetParameter(0,0.6); fit->SetParLimits(0,0.5,0.7);
    fit->SetParameter(2,1e4); fit->SetParLimits(2,7e3,1.5e4);
    fit->SetParameter(3,1e4); fit->SetParLimits(3,7e3,1.5e4);
    fit->SetParameter(1,-3e4); fit->SetParLimits(1,-5e4,-1e4);
    integral->Fit(fit,"l");
    double chi2 =  fit->GetChisquare() / fit->GetNDF();
    for(int i=0;i!=10;++i) {
      chi2 =  fit->GetChisquare() / fit->GetNDF();
      cout << "last chi2 " << chi2 << endl;
      if(chi2<2) break;
      integral->Fit(fit,"emi");
    }
    TF1 *fit1 = new TF1("firstComp",Form("%f*TMath::Gaus(x,%f,%f,1)",fit->GetParameter(4)*fit->GetParameter(0),fit->GetParameter(1),fit->GetParameter(2)),-1.1e5,0);
    TF1 *fit2 = new TF1("seconComp",Form("%f*TMath::Gaus(x,%f,%f,1)",fit->GetParameter(4)*(1-fit->GetParameter(0)),fit->GetParameter(1)*secondPeak/firstPeak,fit->GetParameter(3)),-1.1e5,0);
    fit1->SetLineColor( kBlack );
    fit2->SetLineColor( kBlack );
    fit1->Draw("same");
    fit2->Draw("same");
    fit->Draw("same");
    tex->SetTextColor( kRed-3 );
    tex->SetTextSize(0.045);
    tex->DrawLatexNDC(0.13,0.82, Form("eN ( a gauss( #mu_{1}, #sigma_{1} ) + (1-a) gaus( #mu_{1}*#frac{1.275}{0.511}, #sigma_{2} ) )"));
    tex->SetTextSize(0.04);
    tex->SetTextColor( kBlack );
    tex->DrawLatexNDC(0.13,0.75, Form( "e = %.0f%%",fit->GetParameter(4)*100/scale));
    tex->DrawLatexNDC(0.13,0.70, Form( "a = %.0f%%",fit->GetParameter(0)*100));
    tex->DrawLatexNDC(0.13,0.65, Form( "#sigma_{1} = %.0f",fit->GetParameter(2)));
    tex->DrawLatexNDC(0.13,0.60, Form( "#sigma_{2} = %.0f",fit->GetParameter(3)));
    tex->DrawLatexNDC(0.13,0.18, Form( "#chi^{2}/NDF = %.2f",chi2));
    tex->SetTextColor( kBlue-3 );
    tex->DrawLatexNDC(0.13,0.55, Form( "#mu_{1} = %.0f",fit->GetParameter(1)));
  }
  main2->SaveAs( Form("%s_CHARGE.pdf",file.Data()), "PDF" );

  return 0;
}
