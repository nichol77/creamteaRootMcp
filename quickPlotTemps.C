

void quickPlotTemps()
{

  ifstream TempFile("/tmp/theTemps.txt");
  Double_t times[100000];
  Double_t temps[4][100000];
  Int_t numPoints=0;
  while(TempFile >> times[numPoints]) {
    for(int i=0;i<4;i++)
      TempFile >> temps[i][numPoints];
    numPoints++;
  }

  TGraph *gr1 = new TGraph(numPoints,times,temps[0]);
  gr1->Draw("ap");

//   TF1 *fitty = new TF1("fitty",tempFunc,1.277723e9,1.27773e9,4);
//   fitty->SetParameters(40,1.277725568e9,2256,0.5);
//   //  fitty->Draw("al");
//   gr1->Fit("fitty","QR");

//   std::cout << fitty->Eval(1.27773e9) << "\n";
  

}

Double_t tempFunc(Double_t *x, Double_t *par)
{

  Double_t tempMax=par[0];
  Double_t t0=par[1];
  Double_t tau=par[2];
  Double_t scale=par[3];

  return tempMax-scale*TMath::Exp(-1*(x[0]-t0)/tau);

}
