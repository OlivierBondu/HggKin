#include <iostream>

//Root headers
#include "TFile.h"
#include "RooWorkspace.h"

//Roofit headers
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"

using namespace std;


int main() {

  //Create Workspace
  TFile *root_file=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/WS_SPlot.root","UPDATE");
  RooWorkspace *ws=new RooWorkspace ("ws_hgg","ws_hgg");


  int AddModel(RooWorkspace*);
  if ( AddModel(ws)) cout << "add_variables failed" << endl;


  ws->Write("",TObject::kOverwrite);

  return 0;
}

//###############################################"
int AddModel(RooWorkspace *ws) {

  RooRealVar *dipho_mass=new RooRealVar("dipho_mass","dipho_mass",120,100,180);
  RooRealVar *dipho_pt=new RooRealVar("dipho_pt","dipho_pt",100,0,200);


  //Model of ggh+vbf = Gauss_vbf+gauss_ggh
  RooRealVar *mean_mass_ggh=new RooRealVar("mean_mass_ggh","mean_mass_ggh",126,123,129);
  RooRealVar *sigma_mass_ggh=new RooRealVar("sigma_mass_ggh","sigma_mass_ggh",1,0,3);
  RooGaussian *model_mass_ggh=new RooGaussian("model_mass_ggh","model_mass_ggh",*dipho_mass,*mean_mass_ggh,*sigma_mass_ggh);

  RooRealVar *mean_mass_vbf=new RooRealVar("mean_mass_vbf","mean_mass_vbf",126,123,129);
  RooRealVar *sigma_mass_vbf=new RooRealVar("sigma_mass_vbf","sigma_mass_vbf",1,0,3);
  RooGaussian *model_mass_vbf=new RooGaussian("model_mass_vbf","model_mass_vbf",*dipho_mass,*mean_mass_vbf,*sigma_mass_vbf);

  RooRealVar *compo_vbf_sgn=new RooRealVar("compo_vbf_sgn","compo_vbf_sgn",0.1,0,1);
  RooRealVar *compo_ggh_sgn=new RooRealVar("compo_ggh_sgn","compo_ggh_sgn",0.9,0,1);
  RooAddPdf *model_mass_sgn=new RooAddPdf("model_mass_sgn","model_mass_sgn",RooArgSet(*model_mass_ggh,*model_mass_vbf),RooArgSet(*compo_ggh_sgn,*compo_vbf_sgn));


  return 0;
}
