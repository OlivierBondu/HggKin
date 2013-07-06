#include <iostream>

//Root headers
#include "TFile.h"
#include "RooWorkspace.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TMath.h"

//Roofit headers
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooDataSet.h"
#include "RooPolynomial.h"
#include "RooGenericPdf.h"
#include "RooProdPdf.h"
#include "RooBernstein.h"

using namespace std;


int main() {

  //Create Workspace

  RooWorkspace *ws=new RooWorkspace ("ws_hgg","ws_hgg");


  int AddModel(RooWorkspace*);
  int AddData(RooWorkspace*);
  int DoSPlot(RooWorkspace*);

  if (AddModel(ws)) cout << "AddModel failed" << endl;
  //  if (AddData(ws)) cout << "AddData failed" << endl;
  //if (DoSPlot(ws)) cout << "DoSPlot failes" << endl;
  cout << "DoSPlot succeded" << endl;
  TFile *root_file=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/WS_SPlot.root","UPDATE");
  //  ws->Print();
  //  ws->Write("",TObject::kOverwrite);
  root_file->Close();
  return 0;
}

//###############################################"
int AddModel(RooWorkspace *ws) {


  TFile *file_kin=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");


  RooRealVar *dipho_mass=new RooRealVar("dipho_mass","mass_dipho_mass",120,100,180);
  RooRealVar *dipho_pt=new RooRealVar("dipho_pt","dipho_pt",100,0,200);
  RooPlot *frame_mass=0,*frame_pt=0;
  TTree *tree=0;
  RooDataSet *data=0;
  TCanvas *canvas=new TCanvas();

  //Model of ggh =gauss_mass * tallispol1_pt
  cout << "#####  Model ggH" << endl;
 
  RooRealVar *mean_mass_ggh=new RooRealVar("mean_mass_ggh","mean_mass_ggh",125,100,180);
  RooRealVar *sigma_mass_ggh=new RooRealVar("sigma_mass_ggh","sigma_mass_ggh",3e-2,0,10);
  RooGaussian *model_mass_ggh=new RooGaussian("model_mass_ggh","model_mass_ggh",*dipho_mass,*mean_mass_ggh,*sigma_mass_ggh);
  tree=(TTree*) file_kin->Get("tree_ggh");
  data=new RooDataSet("data","data",tree , *dipho_mass);
  model_mass_ggh->fitTo(*data);
  frame_mass=dipho_mass->frame();
  data->plotOn(frame_mass);
  model_mass_ggh->plotOn(frame_mass);
  frame_mass->Draw();
  canvas->SaveAs("frame.png");
  data->Delete();
  tree->Delete();


  RooRealVar *coef0_pol_pt_ggh=new RooRealVar("coef0_pol_pt_ggh","coef0_pol_pt_ggh",3,-100,100);
  RooRealVar *coef1_pol_pt_ggh=new RooRealVar("coef1_pol_pt_ggh","coef1_pol_pt_ggh",0,-100,100);
  RooPolynomial *pol_pt_ggh=new RooPolynomial("pol_pt_ggh","pol_pt_ggh",*dipho_pt,RooArgSet(*coef0_pol_pt_ggh,*coef1_pol_pt_ggh));

  RooRealVar *coefM_tsallis_pt_ggh=new RooRealVar("coefM_tsallis_pt_ggh","coefM_tsallis_pt_ggh",125,0,300);
  RooRealVar *coefT_tsallis_pt_ggh=new RooRealVar("coefT_tsallis_pt_ggh","coefT_tsallis_pt_ggh",1,-100,100);
  RooRealVar *coefN_tsallis_pt_ggh=new RooRealVar("coefN_tsallis_pt_ggh","coefN_tsallis_pt_ggh",1,-100,100);
  RooGenericPdf *tsallis_pt_ggh=new RooGenericPdf("tsallis_pt_ggh","(coefN_tsallis_pt_ggh-1)*(coefN_tsallis_pt_ggh-2)/coefN_tsallis_pt_ggh/coefT_tsallis_pt_ggh/(coefN_tsallis_pt_ggh*coefT_tsallis_pt_ggh+(coefN_tsallis_pt_ggh-2)*coefM_tsallis_pt_ggh)*dipho_pt*pow(1+(sqrt(coefM_tsallis_pt_ggh*coefM_tsallis_pt_ggh+dipho_pt*dipho_pt)-coefM_tsallis_pt_ggh)/coefN_tsallis_pt_ggh/coefT_tsallis_pt_ggh,-coefN_tsallis_pt_ggh)",RooArgSet(*dipho_pt,*coefN_tsallis_pt_ggh,*coefM_tsallis_pt_ggh,*coefT_tsallis_pt_ggh));
  
  RooProdPdf *model_pt_ggh=new RooProdPdf("model_pt_ggh","model_pt_ggh",RooArgList(*pol_pt_ggh,*tsallis_pt_ggh));

  data=new RooDataSet("data_ggh","data_ggh",(TTree*) file_kin->Get("tree_ggh"),*dipho_pt);
  //  model_pt_ggh->fitTo(*data);

  TFile f("/afs/cern.ch/work/c/cgoudet/private/data/WS_SPlot.root","UPDATE");
  RooPlot* frame=dipho_pt->frame();
  frame->SetName("frame");  
  model_pt_ggh->plotOn(frame);
  data->plotOn(frame);  
  frame->Write("",TObject::kOverwrite);
  f.Close();
  data->Delete();


  RooProdPdf *model_ggh=new RooProdPdf("model_ggh","model_ggh",RooArgList(*model_pt_ggh,*model_mass_ggh));


  cout << "#####   Model bkg" << endl;
  RooRealVar *coef0_bern_mass_bkg=new RooRealVar("coef0_bern_mass_bkg","coef0_bern_mass_bkg",0,-50,50);
  RooRealVar *coef1_bern_mass_bkg=new RooRealVar("coef1_bern_mass_bkg","coef1_bern_mass_bkg",0,-50,50);
  RooRealVar *coef2_bern_mass_bkg=new RooRealVar("coef2_bern_mass_bkg","coef2_bern_mass_bkg",0,-50,50);
  RooRealVar *coef3_bern_mass_bkg=new RooRealVar("coef3_bern_mass_bkg","coef3_bern_mass_bkg",0,-50,50);
  RooPolynomial *model_mass_bkg=new RooPolynomial("model_mass_bkg","model_mass_bkg",*dipho_mass,RooArgSet(*coef0_bern_mass_bkg,*coef1_bern_mass_bkg,*coef2_bern_mass_bkg,*coef3_bern_mass_bkg));


  RooRealVar *coef0_pol_pt_bkg=new RooRealVar("coef0_pol_pt_bkg","coef0_pol_pt_bkg",0,-50,50);
  RooRealVar *coef1_pol_pt_bkg=new RooRealVar("coef1_pol_pt_bkg","coef1_pol_pt_bkg",0,-50,50);
  RooRealVar *coef2_pol_pt_bkg=new RooRealVar("coef2_pol_pt_bkg","coef2_pol_pt_bkg",0,-50,50);
  RooPolynomial *pol_pt_bkg=new RooPolynomial("pol_pt_bkg","pol_pt_bkg",*dipho_pt,RooArgSet(*coef0_pol_pt_bkg,*coef1_pol_pt_bkg,*coef2_pol_pt_bkg));

  RooRealVar *sigma_logn_pt_bkg=new RooRealVar("sigma_logn_pt_bkg","sigma_logn_pt_bkg",10,0,100);
  RooRealVar *theta_logn_pt_bkg=new RooRealVar("theta_logn_pt_bkg","theta_logn_pt_bkg",10,0,100);
  RooRealVar *coefM_logn_pt_bkg=new RooRealVar("coefM_logn_pt_bkg","coefM_logn_pt_bkg",10,0,100);
  RooGenericPdf *logn_pt_bkg=new RooGenericPdf("logn_pt_bkg","TMath::LogNormal(dipho_pt,sigma_logn_pt_bkg,theta_logn_pt_bkg,coefM_logn_pt_bkg)",RooArgSet(*dipho_pt,*sigma_logn_pt_bkg,*theta_logn_pt_bkg,*coefM_logn_pt_bkg));

  //  RooProdPdf *model_pt_bkg=new RooProdPdf("model_pt_bkg","model_pt_bkg",RooArgSet(*pol_pt_bkg,*logn_pt_bkg));
  //  RooProdPdf *model_bkg=new RooProdPdf("model_bkg","model_bkg",RooArgSet(*model_mass_bkg,*model_pt_bkg));

  RooAbsPdf *model_bkg=logn_pt_bkg;

  RooRealVar *ggh_yields=new RooRealVar("ggh_yields","ggh_yields",5e4,1e4,1e5);
  RooRealVar *bkg_yields=new RooRealVar("bkg_yields","bkg_yields",7e4,1e4,1e5);
  RooAddPdf *model_both= new RooAddPdf("model_both","model_both",RooArgList(*model_ggh,*model_bkg),RooArgList(*ggh_yields,*bkg_yields));



  


  ws->import(*model_both);
  file_kin->Close();



  return 0;
}

//###################################################
int AddData(RooWorkspace* ws) {
  cout << "Generated data not weighted yet" << endl;

  RooRealVar *dipho_mass=ws->var("dipho_mass");
  RooRealVar *dipho_pt=ws->var("dipho_pt");

  TFile *file_kin=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");

  TTree *tree=(TTree*) file_kin->Get("tree_ggh");
  RooDataSet *data_ggh=new RooDataSet("data_ggh","data_ggh",tree,RooArgSet(*dipho_mass,*dipho_pt));
  tree->Delete();
  tree=(TTree*) file_kin->Get("tree_bkg");
  RooDataSet *data=new RooDataSet("data","data",tree,RooArgSet(*dipho_mass,*dipho_pt));
   data->append(*data_ggh);



  ws->import(*data,RooFit::Rename("data"));

  file_kin->Close();

  return 0;
}


//###################################################
int DoSPlot(RooWorkspace* ws) {

  RooAbsPdf *model=ws->pdf("model_both");
  RooDataSet *data=(RooDataSet*) ws->data("data");
  

  
  //    model->fitTo(*data,RooFit::Extended());


  return 0;
}
