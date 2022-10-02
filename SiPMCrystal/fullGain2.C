const int nnn = 5;

TF1 *fit1, *fit2;

void LoadSiPMS12data() {
  TFile *rfile = new TFile("../S12data/sipm_spec_input_S12572-015.root");
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
  TF1 *enf = (TF1*) rfile->Get("fENF_vs_OV");
  cout << enf->GetFormula()->GetExpFormula() << endl;
  double p0E = enf->GetParameter(0);
  double p1E = enf->GetParameter(1);
  double p2E = enf->GetParameter(2);

  cout << " PDE at x=2: " << p4P*(p0P+2*p1P+p2P*pow(2,2)+p3P*pow(2,3)) << endl;
  fit1 = new TF1("fit1",Form("1.6e-10*(%f+x*%f+%f*pow(x,2)+%f*pow(x,3))*%f/100*(%f+x*%f)*(%f+%f*x+%f*x*x)*16.8",p0P,p1P,p2P,p3P,p4P,p0G,p1G,p0E,p1E,p2E),0.5,4.5);
  fit2 = new TF1("fit2",Form("1.6e-10*(%f+x*%f+%f*pow(x,2)+%f*pow(x,3))*%f/100*(%f+x*%f)*(%f+%f*x+%f*x*x)*16.8",p0P,p1P,p2P,p3P,p4P,p0G,p1G,p0E,p1E,p2E),0.5,4.5);
}

int fullGain2() {
  LoadSiPMS12data();
  //gain->Draw();
  
  double xx1[nnn] = {70-64, 70-64, 70-64, 70-64, 70-64 };
  double xx2[nnn] = {70-64, 70-64, 70-64, 70-64, 70-64 };
  double yy1[nnn] = {0.908317,0.893979,0.821007,0.820994,0.887527};
  double yy2[nnn] = {0.877130,0.851716,0.867764,0.871384,0.856554};

  for(int i=1; i!=4; ++i) 
    cout << i << "V -> " << 0.9/fit1->Eval(i) << endl;
  
  return 0;
}
