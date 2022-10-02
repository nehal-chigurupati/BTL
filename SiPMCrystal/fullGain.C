const int nnn = 5;

const double nominal = 28; //more than 28 photons per kev

TF1 *fit1;
TF1 *fit2;

double gainS141601315PS(double x) { // gain as a function of overvoltage valid x = from 3.5 to 6.5 at 25degC
  //https://www.hamamatsu.com/resources/pdf/ssd/s14160-1310ps_etc_kapd1070e.pdf
  return -1899.97 + x*82179.6;
}


double gainS141601310PS(double x) { // gain as a function of overvoltage valid x = from 3.5 to 7.5 at 25degC
  //https://www.hamamatsu.com/resources/pdf/ssd/s14160-1310ps_etc_kapd1070e.pdf
  return -2029.01 + x*35338.7;
}

void LoadSiPMdata() {
  TFile *rfile = new TFile("../HDR2data/sipm_spec_input_HDR2-015-v2-1e13.root");
  rfile->ls();
  TF1 *gai = (TF1*) rfile->Get("fGain_vs_OV");
  double p0G = gai->GetParameter(0);
  double p1G = gai->GetParameter(1);
  TF1 *pde = (TF1*) rfile->Get("fPDE_LYSO_vs_OV");
  double p0P = pde->GetParameter(0);
  double p1P = pde->GetParameter(1);
  double p2P = pde->GetParameter(2);
  double p3P = pde->GetParameter(3);
  double p4P = pde->GetParameter(4);
  double p5P = pde->GetParameter(5);
  TF1 *enf = (TF1*) rfile->Get("fENF_vs_OV");
  double p0E = enf->GetParameter(0);
  double p1E = enf->GetParameter(1);

  cout << " PDE at x=2: " << (p0P+2*p1P+p2P*pow(2,2)+p3P*pow(2,3)+p4P*pow(2,4)+p5P*pow(2,5)) << endl;
  //gain = new TF1("gain",Form("([0]+[1]*x)*(%f+x*%f)",p0,p1),0.5,4.5);
  fit1 = new TF1("fit1",Form("[0]*1.6e-10*(%f+x*%f+%f*pow(x,2)+%f*pow(x,3)+%f*pow(x,4)+%f*pow(x,5))/100*(%f+x*%f)*(1+%f*x+%f*x*x)*16.8",p0P,p1P,p2P,p3P,p4P,p5P,p0G,p1G,p0E,p1E),0.5,4.5);
  fit2 = new TF1("fit2",Form("[0]*1.6e-10*(%f+x*%f+%f*pow(x,2)+%f*pow(x,3)+%f*pow(x,4)+%f*pow(x,5))/100*(%f+x*%f)*(1+%f*x+%f*x*x)*16.8",p0P,p1P,p2P,p3P,p4P,p5P,p0G,p1G,p0E,p1E),0.5,4.5);
}

int fullGain() {
  LoadSiPMdata();
  //gain->Draw();
  
  new TCanvas();
  gStyle->SetOptStat(0);
  double x1[nnn] = {40.0-38.3, 40.3-38.3, 41.3-38.3, 42.3-38.3, 43.3-38.3 };
  double x2[nnn] = {40.0-38.5, 40.3-38.5, 41.3-38.5, 42.3-38.5, 43.3-38.5 };
  double y1[nnn] = {0.175714,  0.254424,  0.58549,   0.955707,  1.35697   }; //fC
  double y2[nnn] = {0.158536,  0.224759,  0.52907,   0.878962,  1.25277   };

  double xx1[nnn] = {70-64, 70-64, 70-64, 70-64, 70-64 };
  double xx2[nnn] = {70-64, 70-64, 70-64, 70-64, 70-64 };
  double yy1[nnn] = {0.908317,0.893979,0.821007,0.820994,0.887527};
  double yy2[nnn] = {0.877130,0.851716,0.867764,0.871384,0.856554};
  
  double z1[nnn] = {5.51741e-4,6.51303e-4,1.51481e-3,2.48752e-3,3.54901e-3};
  double z2[nnn] = {4.23737e-4,5.90237e-4,1.37845e-3,2.28250e-3,3.26661e-3};

  double fI1[nnn] = {0.95,    0.96,    0.98,    0.99,    1.00};
  double fI2[nnn] = {1.00,    1.00,    1.00,    1.01,    1.02};
  double fA1[nnn] = {0.002465,0.002486,0.002564,0.002595,0.002641};
  double fA2[nnn] = {0.002735,0.002671,0.002699,0.002703,0.002730};

  for(int i=0; i!=nnn; ++i) {
    y1[i] /= fI1[i];
    y2[i] /= fI2[i];
    z1[i] /= fA1[i];
    z2[i] /= fA2[i];
  }

  cout << "OV  || pC" << endl;
  for(int i=0; i!=nnn; ++i) {
    cout << Form("%.1f || %6.1f",x1[i],y1[i]*1000) << endl;
  }
  cout << "OV  || pC" << endl;
  for(int i=0; i!=nnn; ++i) {
    cout << Form("%.1f || %6.1f",x2[i],y2[i]*1000) << endl;
  }
  
  TGraph *gr1 = new TGraph(nnn,x1,y1);
  TGraph *gr2 = new TGraph(nnn,x2,y2);
  TGraph *gr1A = new TGraph(nnn,x1,z1);
  TGraph *gr2A = new TGraph(nnn,x2,z2);
  gr1->SetMarkerStyle(24);
  gr2->SetMarkerStyle(24);
  gr1->SetMarkerColor(kRed-3);
  gr2->SetMarkerColor(kBlue-3);
  gr1->SetLineColor(kRed-3);
  gr2->SetLineColor(kBlue-3);
  gr1A->SetMarkerStyle(25);
  gr2A->SetMarkerStyle(25);
  gr1A->SetMarkerColor(kRed-3);
  gr2A->SetMarkerColor(kBlue-3);
  gr1A->SetLineColor(kRed-3);
  gr2A->SetLineColor(kBlue-3);

  fit1->SetLineColor( kGreen-3);
  fit2->SetLineColor( kGreen-3);
  
  TH2D *axis  = new TH2D("axis","Na22 Source on HDR2s;Overvoltage;Gamma Peak  (nC)",100,1.0,5.5,100,-0.1,1.6);

  new TCanvas();
  axis->Draw();
  gr1A->Draw("Psame");
  gr2A->Draw("Psame");
  gr1->Draw("Psame");
  gr2->Draw("Psame");
  gr1->Fit(fit1);
  gr2->Fit(fit2);
  gr1->Fit(fit1);
  gr2->Fit(fit2);

  TLegend *leg = new TLegend(0.1,0.68,0.7,0.9);
  leg->AddEntry(gr1,"HDR2-11   Vbr = 38.3");
  leg->AddEntry(gr2,"HDR2-12   Vbr = 38.5");
  leg->AddEntry(fit1, "[0] * e * PDE(OV) * GAIN(OV) * ENF(OV) * 16.8" );
  leg->Draw();

  TLatex *tex = new TLatex();
  double photons = 0.5*(fit1->GetParameter(0)+fit2->GetParameter(0));
  tex->DrawTextNDC(0.12,0.6,Form("[0] = %.1f",photons));
  tex->DrawTextNDC(0.12,0.12,Form("NoPhotonsPerMeV@SiPM ([0]/0.511) = %.0f",photons/0.511));
  
  return 0;
}
