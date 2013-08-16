#include <iostream>
#include "setTDRStyle.h"

//Root headers
#include "TFile.h"
#include "RooWorkspace.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TH1F.h"
#include "TLine.h"

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
#include "RooFFTConvPdf.h"

#include "RooStats/SPlot.h"

#define BATCH 1 // On batch mode, have to change loading and saving path
#define NBINS 40
#define WIDTH 5
using namespace std;
using namespace RooStats;
using namespace RooFit;

int main() {
  //#############menu
  int const menu_cut[5]={0,200,375,550,750};

  //############def
  int AddModel(RooWorkspace*, int const &cut=0); // Add pdf to workspace and pre-fit them
  int AddData(RooWorkspace*, int const &cut=0); // Add simulated events ro workspace
  int DoSPlot(RooWorkspace*); // Create SPlot object
  int MakePlot(RooWorkspace*, int const &cut=0); // Create and save result and check plots

  TFile *root_file=0;
  if (BATCH) root_file=new TFile("WS_SPlot_gen.root","UPDATE"); //File to store the workspace
  else root_file=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/WS_SPlot_gen.root","UPDATE");
  RooWorkspace *ws=0;
  char buffer[100];

  //    int i=0;
  for (int i=0;i<5;i++) {
    root_file->cd();   
    if (i)    sprintf(buffer,"ws_hgg_splot%d_gen",menu_cut[i]);
    else sprintf(buffer,"ws_hgg");
    ws=new RooWorkspace (buffer,buffer);
    if (AddModel(ws,menu_cut[i])) cout << "AddModel failed" << endl;
    if (AddData(ws,menu_cut[i])) cout << "AddData failed" << endl;
    if (DoSPlot(ws)) cout << "DoSPlot failes" << endl;
    cout << "DoSPlot succeded" << endl;
    if (MakePlot(ws,menu_cut[i])) cout << "Plotting failed" << endl;
    root_file->cd();
    ws->Write("",TObject::kOverwrite);
    ws->Delete();  
  }
  root_file->Close();
  cout << "went up to the end" << endl;
  return 0;
}


//######################################################################################################################################
//######################################################################################################################################
//######################################################################################################################################
int AddModel(RooWorkspace *ws, int  const &cut=0) {
  setTDRStyle(); 
  TFile *file_kin=0;
  if (BATCH) file_kin=new TFile("kin_dist.root");
  else file_kin=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
  
  
  RooRealVar *dipho_mass=new RooRealVar("dipho_mass","m_{#gamma #gamma}",110,180,"GeV/c^{2}");
  RooRealVar *dipho_pt=new RooRealVar("dipho_pt","p_{T#gamma#gamma}",NBINS,0,NBINS*WIDTH,"GeV/c");
  RooRealVar *dipho_ctheta=new RooRealVar("dipho_ctheta","cos(#theta *)",0.5,0,1);
  RooRealVar *weight=new RooRealVar("weight","weight",0.1);
  RooPlot *frame_mass=0,*frame_pt=0;
  TTree *tree=0;
  RooDataSet *sim_gen=0;
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  char buffer[100];  


  //Model of ggh =gauss_mass * tsallispol2
  cout << "#####  Model ggH" << endl;
  //create model of background and fit  
  RooRealVar *mean_mass_ggh=new RooRealVar("mean_mass_ggh","mean_mass_ggh",124.95,124.9,125.1);
  RooRealVar *sigma_mass_ggh=new RooRealVar("sigma_mass_ggh","sigma_mass_ggh",0.03,1e-3,5e-2);
  RooGaussian *model_mass_ggh=new RooGaussian("model_mass_ggh","model_mass_ggh",*dipho_mass,*mean_mass_ggh,*sigma_mass_ggh);


  tree=(TTree*) file_kin->Get("tree_gen_ggh");
  if (cut) {  //select data according cuts on theta
    sprintf(buffer, "dipho_ctheta > %1.3f",cut/1000.);   
    sim_gen=new RooDataSet("sim_gen","sim_gen",tree , RooArgSet(*dipho_mass,*dipho_ctheta,*weight), buffer,"weight");
  }
  else sim_gen=new RooDataSet("sim_gen","sim_gen", tree, RooArgSet(*dipho_mass,*dipho_pt,*weight),"","weight");
  RooRealVar *exp_ggh_yield=new RooRealVar("exp_ggh_yield","exp_ggh_yield",sim_gen->sumEntries());
  RooDataSet *sim_dum=new RooDataSet(*sim_gen,"sim_dum");
  cout << "bug" << endl;

  model_mass_ggh->fitTo(*sim_gen,Range(124.95,125.05));
  cout << "tree" << endl;

  //set variable range to +-3 sigmas to ease splot fitting
  mean_mass_ggh->setRange(mean_mass_ggh->getVal()-2*mean_mass_ggh->getError(),mean_mass_ggh->getVal()+2*mean_mass_ggh->getError());
  sigma_mass_ggh->setRange(sigma_mass_ggh->getVal()-2*sigma_mass_ggh->getError(),sigma_mass_ggh->getVal()+2*sigma_mass_ggh->getError());
  
  //Check plot
  frame_mass=dipho_mass->frame(124.95,125.05,20);
  sim_gen->plotOn(frame_mass);
  model_mass_ggh->plotOn(frame_mass);
  frame_mass->Draw();
  canvas->SaveAs("frame_massgghzoom_gen.pdf");
  frame_mass->Delete();


  frame_mass=dipho_mass->frame(110,140,15);
  sim_gen->plotOn(frame_mass);
  model_mass_ggh->plotOn(frame_mass);
  frame_mass->Draw();
  canvas->SaveAs("frame_massggh_gen.pdf");
  frame_mass->Delete();

  sim_gen->Delete();
  sim_gen=0;


  RooRealVar *coef0_pol_pt_ggh=new RooRealVar("coef0_pol_pt_ggh","coef0_pol_pt_ggh",7,-1,100);
  RooRealVar *coef1_pol_pt_ggh=new RooRealVar("coef1_pol_pt_ggh","coef1_pol_pt_ggh",7,-1,100);
  RooRealVar *coef2_pol_pt_ggh=new RooRealVar("coef2_pol_pt_ggh","coef2_pol_pt_ggh",7,-1,100);
  RooBernstein *pol_pt_ggh=new RooBernstein("pol_pt_ggh","pol_pt_ggh",*dipho_pt,RooArgSet(*coef0_pol_pt_ggh, *coef1_pol_pt_ggh, *coef2_pol_pt_ggh));

  RooRealVar *coefM_tsallis_pt_ggh=new RooRealVar("coefM_tsallis_pt_ggh","coefM_tsallis_pt_ggh",125,0,300);
  RooRealVar *coefT_tsallis_pt_ggh=new RooRealVar("coefT_tsallis_pt_ggh","coefT_tsallis_pt_ggh",10,0,50);
  RooRealVar *coefN_tsallis_pt_ggh=new RooRealVar("coefN_tsallis_pt_ggh","coefN_tsallis_pt_ggh",5,2,30);
  RooGenericPdf *tsallis_pt_ggh=new RooGenericPdf("tsallis_pt_ggh","(coefN_tsallis_pt_ggh-1)*(coefN_tsallis_pt_ggh-2)/coefN_tsallis_pt_ggh/coefT_tsallis_pt_ggh/(coefN_tsallis_pt_ggh*coefT_tsallis_pt_ggh+(coefN_tsallis_pt_ggh-2)*coefM_tsallis_pt_ggh)*dipho_pt*pow(1+(sqrt(coefM_tsallis_pt_ggh*coefM_tsallis_pt_ggh+dipho_pt*dipho_pt)-coefM_tsallis_pt_ggh)/coefN_tsallis_pt_ggh/coefT_tsallis_pt_ggh,-coefN_tsallis_pt_ggh)",RooArgSet(*dipho_pt,*coefN_tsallis_pt_ggh,*coefM_tsallis_pt_ggh,*coefT_tsallis_pt_ggh));
  
  cout << "pt" << endl;
  RooProdPdf *model_pt_ggh=new RooProdPdf("model_pt_ggh","model_pt_ggh",RooArgList(*pol_pt_ggh,*tsallis_pt_ggh));
  //  RooGenericPdf *model_pt_ggh=tsallis_pt_ggh;
  
  if (cut) sprintf(buffer,"dipho_ctheta > %1.3f", cut/1000.); 
  else sprintf(buffer,"");
  sim_gen=new RooDataSet("sim_gen","sim_gen", tree, RooArgSet(*dipho_pt,*dipho_ctheta, *weight),buffer,"weight");
  model_pt_ggh->fitTo(*sim_gen);

  // coef0_pol_pt_ggh->setRange(coef0_pol_pt_ggh->getVal()-2*coef0_pol_pt_ggh->getError(),coef0_pol_pt_ggh->getVal()+2*coef0_pol_pt_ggh->getError());
//   coef1_pol_pt_ggh->setRange(coef1_pol_pt_ggh->getVal()-2*coef1_pol_pt_ggh->getError(),coef1_pol_pt_ggh->getVal()+2*coef1_pol_pt_ggh->getError());
//   coef2_pol_pt_ggh->setRange(coef2_pol_pt_ggh->getVal()-2*coef2_pol_pt_ggh->getError(),coef2_pol_pt_ggh->getVal()+2*coef2_pol_pt_ggh->getError());
//   coefM_tsallis_pt_ggh->setRange(coefM_tsallis_pt_ggh->getVal()-2*coefM_tsallis_pt_ggh->getError(),coefM_tsallis_pt_ggh->getVal()+2*coefM_tsallis_pt_ggh->getError());
//   coefN_tsallis_pt_ggh->setRange(coefN_tsallis_pt_ggh->getVal()-2*coefN_tsallis_pt_ggh->getError(),coefN_tsallis_pt_ggh->getVal()+2*coefN_tsallis_pt_ggh->getError());
//   coefT_tsallis_pt_ggh->setRange(coefT_tsallis_pt_ggh->getVal()-2*coefT_tsallis_pt_ggh->getError(),coefT_tsallis_pt_ggh->getVal()+2*coefT_tsallis_pt_ggh->getError());

  coef0_pol_pt_ggh->setConstant(1);
  coef1_pol_pt_ggh->setConstant(1);
  coef2_pol_pt_ggh->setConstant(1);
  coefM_tsallis_pt_ggh->setConstant(1);
  coefN_tsallis_pt_ggh->setConstant(1);
  coefT_tsallis_pt_ggh->setConstant(1);

  

  //Check plot
//     frame_pt=dipho_pt->frame();
//     sim_gen->plotOn(frame_pt);
//     model_pt_ggh->plotOn(frame_pt);
//     frame_pt->Draw();
//     canvas->SaveAs("frame_ptggh_gen.pdf");
//     frame_pt->Delete();
  
  sim_gen->Delete();
  tree->Delete();

  
  RooProdPdf *model_ggh=new RooProdPdf("model_ggh","model_ggh",RooArgList(*model_pt_ggh,*model_mass_ggh));
  
 

  //#################################
  cout << "#####   Model bkg" << endl;
  RooRealVar *coef0_bern_mass_bkg=new RooRealVar("coef0_bern_mass_bkg","coef0_bern_mass_bkg",10,0,100);
  RooRealVar *coef1_bern_mass_bkg=new RooRealVar("coef1_bern_mass_bkg","coef1_bern_mass_bkg",10,0,100);
  RooRealVar *coef2_bern_mass_bkg=new RooRealVar("coef2_bern_mass_bkg","coef2_bern_mass_bkg",10,0,100);
  RooRealVar *coef3_bern_mass_bkg=new RooRealVar("coef3_bern_mass_bkg","coef3_bern_mass_bkg",10,0,100);
  RooBernstein *model_mass_bkg=new RooBernstein("model_mass_bkg","model_mass_bkg",*dipho_mass,RooArgSet(*coef0_bern_mass_bkg,*coef1_bern_mass_bkg,*coef2_bern_mass_bkg,*coef3_bern_mass_bkg));
  
  tree=(TTree*) file_kin->Get("tree_gen_bkg");

    if (cut)  sprintf(buffer,"dipho_ctheta > %1.3f",cut/1000.);
    else   sprintf(buffer,"");
    sim_gen=new RooDataSet("data","data", tree, RooArgSet( *dipho_mass, *dipho_ctheta,*weight), buffer,"weight");

    cout << " attendu" << endl;
    cout << sim_gen->sumEntries() << endl;
    cout << sim_dum->sumEntries() << endl;
    sim_dum->append(*sim_gen);
    model_mass_bkg->fitTo(*sim_gen);


    coef0_bern_mass_bkg->setRange(coef0_bern_mass_bkg->getVal()- 2*coef0_bern_mass_bkg->getError(),coef0_bern_mass_bkg->getVal()+ 2*coef0_bern_mass_bkg->getError());
    coef1_bern_mass_bkg->setRange(coef1_bern_mass_bkg->getVal()- 2*coef1_bern_mass_bkg->getError(),coef1_bern_mass_bkg->getVal()+ 2*coef1_bern_mass_bkg->getError());
    coef2_bern_mass_bkg->setRange(coef2_bern_mass_bkg->getVal()- 2*coef2_bern_mass_bkg->getError(),coef2_bern_mass_bkg->getVal()+ 2*coef2_bern_mass_bkg->getError());
    coef3_bern_mass_bkg->setRange(coef3_bern_mass_bkg->getVal()- 2*coef3_bern_mass_bkg->getError(),coef3_bern_mass_bkg->getVal()+ 2*coef3_bern_mass_bkg->getError());

  //Check plot
    frame_mass=dipho_mass->frame();
    sim_gen->plotOn(frame_mass,Binning(80,100,180));
    model_mass_bkg->plotOn(frame_mass);
    frame_mass->Draw();
    canvas->SaveAs("frame_massbkg_gen.pdf");
    frame_mass->Delete();

    sim_gen->Delete();

  RooRealVar *coef0_pol_pt_bkg=new RooRealVar("coef0_pol_pt_bkg","coef0_pol_pt_bkg",14,0.001,50);
  RooRealVar *coef1_pol_pt_bkg=new RooRealVar("coef1_pol_pt_bkg","coef1_pol_pt_bkg",0.7,0.001,50);
  RooRealVar *coef2_pol_pt_bkg=new RooRealVar("coef2_pol_pt_bkg","coef2_pol_pt_bkg",0.7,0.001,50);

  RooBernstein *pol_pt_bkg=new RooBernstein("pol_pt_bkg","pol_pt_bkg",*dipho_pt,RooArgSet(*coef0_pol_pt_bkg,*coef1_pol_pt_bkg,*coef2_pol_pt_bkg));


  RooRealVar *theta_logn_pt_bkg=new RooRealVar("theta_logn_pt_bkg","theta_logn_pt_bkg",-1,-30,30);
  RooRealVar *sigma_logn_pt_bkg=new RooRealVar("sigma_logn_pt_bkg","sigma_logn_pt_bkg",6,0,300);
  RooRealVar *M_logn_pt_bkg=new RooRealVar("M_logn_pt_bkg","M_logn_pt_bkg",6,0,300);
  RooGenericPdf *logn_pt_bkg=new RooGenericPdf("logn_pt_bkg","TMath::LogNormal(dipho_pt,sigma_logn_pt_bkg,theta_logn_pt_bkg,M_logn_pt_bkg)",RooArgSet(*dipho_pt, *M_logn_pt_bkg, * theta_logn_pt_bkg, *sigma_logn_pt_bkg));

  RooRealVar *coefM_tsallis_pt_bkg=new RooRealVar("coefM_tsallis_pt_bkg","coefM_tsallis_pt_bkg",0.0001,-1,300);
  RooRealVar *coefT_tsallis_pt_bkg=new RooRealVar("coefT_tsallis_pt_bkg","coefT_tsallis_pt_bkg",6,0,50);
  RooRealVar *coefN_tsallis_pt_bkg=new RooRealVar("coefN_tsallis_pt_bkg","coefN_tsallis_pt_bkg",3,2,30);
  RooGenericPdf *tsallis_pt_bkg=new RooGenericPdf("tsallis_pt_bkg","(coefN_tsallis_pt_bkg-1)*(coefN_tsallis_pt_bkg-2)/coefN_tsallis_pt_bkg/coefT_tsallis_pt_bkg/(coefN_tsallis_pt_bkg*coefT_tsallis_pt_bkg+(coefN_tsallis_pt_bkg-2)*coefM_tsallis_pt_bkg)*dipho_pt*pow(1+(sqrt(coefM_tsallis_pt_bkg*coefM_tsallis_pt_bkg+dipho_pt*dipho_pt)-coefM_tsallis_pt_bkg)/coefN_tsallis_pt_bkg/coefT_tsallis_pt_bkg,-coefN_tsallis_pt_bkg)",RooArgSet(*dipho_pt,*coefN_tsallis_pt_bkg,*coefM_tsallis_pt_bkg,*coefT_tsallis_pt_bkg));
  
  RooProdPdf *model_pt_bkg=new RooProdPdf("model_pt_bkg","model_pt_bkg",RooArgSet(*tsallis_pt_bkg,*pol_pt_bkg));
  

  if (cut) sprintf(buffer,"dipho_ctheta > %1.3f", cut/1000.);
  else     sprintf(buffer,"");
  sim_gen=new RooDataSet("sim_gen","sim_gen", tree, RooArgSet(*dipho_pt,*dipho_mass,*dipho_ctheta,*weight),buffer,"weight");
  model_pt_bkg->fitTo(*sim_gen);
  

//   coef0_pol_pt_bkg->setRange(coef0_pol_pt_bkg->getVal()-2*coef0_pol_pt_bkg->getError(),coef0_pol_pt_bkg->getVal()+2*coef0_pol_pt_bkg->getError());
//   coef1_pol_pt_bkg->setRange(coef1_pol_pt_bkg->getVal()-2*coef1_pol_pt_bkg->getError(),coef1_pol_pt_bkg->getVal()+2*coef1_pol_pt_bkg->getError());
//   coef2_pol_pt_bkg->setRange(coef2_pol_pt_bkg->getVal()-2*coef2_pol_pt_bkg->getError(),coef2_pol_pt_bkg->getVal()+2*coef2_pol_pt_bkg->getError());
//   coefM_tsallis_pt_bkg->setRange(coefM_tsallis_pt_bkg->getVal()-2*coefM_tsallis_pt_bkg->getError(),coefM_tsallis_pt_bkg->getVal()+2*coefM_tsallis_pt_bkg->getError());
//   coefN_tsallis_pt_bkg->setRange(coefN_tsallis_pt_bkg->getVal()-2*coefN_tsallis_pt_bkg->getError(),coefN_tsallis_pt_bkg->getVal()+2*coefN_tsallis_pt_bkg->getError());
//   coefT_tsallis_pt_bkg->setRange(coefT_tsallis_pt_bkg->getVal()-2*coefT_tsallis_pt_bkg->getError(),coefT_tsallis_pt_bkg->getVal()+2*coefT_tsallis_pt_bkg->getError());

  coef0_pol_pt_bkg->setConstant(1);
  coef1_pol_pt_bkg->setConstant(1);
  coef2_pol_pt_bkg->setConstant(1);
  coefM_tsallis_pt_bkg->setConstant(1);
  coefN_tsallis_pt_bkg->setConstant(1);
  coefT_tsallis_pt_bkg->setConstant(1);
  

  

  //Check plot
//   frame_pt=dipho_pt->frame();
//   sim_gen->plotOn(frame_pt);
//   model_pt_bkg->plotOn(frame_pt);
//   frame_pt->Draw();
//   canvas->SaveAs("frame_otbkg_gen.pdf");
//   frame_pt->Delete();

  sim_gen->Delete();
  tree->Delete();

  RooProdPdf *model_bkg=new RooProdPdf("model_bkg","model_bkg",RooArgSet(*model_mass_bkg,*model_pt_bkg));


  //Combine the models 
  RooRealVar *ggh_yield=new RooRealVar("ggh_yield","ggh_yield",500,10,1000);
  RooRealVar *bkg_yield=new RooRealVar("bkg_yield","bkg_yield",100000,100,200000);
  

  RooAddPdf *model_fittot=new RooAddPdf("model_fittot","model_fittot",RooArgList(*model_pt_ggh,*model_pt_bkg),RooArgList(*ggh_yield,*bkg_yield));
  model_fittot->fitTo(*sim_dum);
  RooPlot *frame_fit=dipho_pt->frame();
  sim_dum->plotOn(frame_fit,Name("data"),MarkerColor(1),LineColor(1));
  model_fittot->plotOn(frame_fit,Name("model"));
  model_fittot->plotOn(frame_fit,LineColor(kRed),LineStyle(kDashed),Components("model_pt_bkg"),Name("bkg"));
  model_fittot->plotOn(frame_fit,LineColor(kGreen),LineStyle(kDashed),Components("model_pt_ggh"),Name("ggh"));
  frame_fit->Draw();
  TLegend *legendfit=new TLegend(0.6,0.7,1,1 );
  legendfit->SetTextSize(0.04);
  legendfit->AddEntry("","Generator level","");
  legendfit->AddEntry("data","MC events","lpe");
  legendfit->AddEntry("model","Full Model Fit","l");
  legendfit->AddEntry("ggh","ggH component","l");
  legendfit->AddEntry("bkg","bkg component","l");
  legendfit->Draw();

  TLatex latex;
  latex.SetNDC(1);
  latex.DrawLatex(0.3,0.88,"ggh yield");
  sprintf(buffer,"fitted : %3.0f",ggh_yield->getVal());
  latex.DrawLatex(0.3,0.83,buffer);
  sprintf(buffer,"expected : %3.0f",exp_ggh_yield->getVal());
  latex.DrawLatex(0.3,0.78,buffer);
  canvas->SaveAs("fit_gen.pdf");

  model_fittot->Delete();

  RooAddPdf *model_tot= new RooAddPdf("model_tot","model_tot",RooArgList(*model_ggh,*model_bkg),RooArgList(*ggh_yield,*bkg_yield));  

  // Import all pdf and variables (implicitely) into workspace
  ws->import(*weight);
  ws->import(*dipho_ctheta);
  ws->import(*model_gghbkg);

  file_kin->Close();

  cout << "end AddModel" <<  endl;

  return 0;
}

//######################################################################################################
//######################################################################################################
//######################################################################################################
int AddData(RooWorkspace* ws, int const &cut=0) {

  RooRealVar *dipho_mass=ws->var("dipho_mass");
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *dipho_ctheta=ws->var("dipho_ctheta");
  RooRealVar *weight=ws->var("weight");

  char buffer[100]="dipho_mass<180 && dipho_mass>100.";

  TFile *file_kin=0;
  if (BATCH) file_kin=new TFile("kin_dist.root");
  else file_kin=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");

  TTree *tree=(TTree*) file_kin->Get("tree_gen_ggh");
  if (cut) sprintf(buffer,"%s && dipho_ctheta > %1.3f",buffer,cut/1000.); // create cut string for data set
  RooDataSet *sim_gen_ggh=new RooDataSet("sim_gen_ggh","sim_gen_ggh",tree,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*weight),buffer, "weight"); //Put cut tree into the dataset with weight
  RooRealVar *exp_ggh_yield=new RooRealVar("exp_ggh_yield","exp_ggh_yield",sim_gen_ggh->sumEntries());


  tree->Delete();


  //Check Plot
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  RooPlot *framemass=dipho_mass->frame();
  sim_gen->plotOn(framemass, Binning(160,100,180));
  framemass->Draw();
  canvas->SaveAs("frame_mass_gen.pdf");
//   RooPlot *framept=dipho_pt->frame();
//   sim_gen_ggh->plotOn(framept);
//   framept->Draw();
//   canvas->SaveAs("frame_pt_gen.pdf");  

  //import dataset into workspace
  ws->import(*sim_gen,RooFit::Rename("sim_gen"));
  ws->import(*exp_ggh_yield);
  file_kin->Close();

  return 0;
}


//###################################################################################################
//###################################################################################################
//###################################################################################################
int DoSPlot(RooWorkspace* ws) {
  setTDRStyle();
  RooAbsPdf *model_gghbkg=ws->pdf("model_gghbkg");
  RooDataSet *sim_gen=(RooDataSet*) ws->data("sim_gen_gghbkg");
  RooRealVar *dipho_mass=ws->var("dipho_mass");  
  RooRealVar *dipho_pt=ws->var("dipho_pt");

  RooRealVar *exp_ggh_yield=ws->var("exp_ggh_yield");




  //Fixing discriminant parameters
  RooRealVar *mean_mass_ggh=ws->var("mean_mass_ggh");
  RooRealVar *sigma_mass_ggh=ws->var("sigma_mass_ggh");
  RooRealVar *coef0_bern_mass_bkg=ws->var("coef0_bern_mass_bkg");
  RooRealVar *coef1_bern_mass_bkg=ws->var("coef1_bern_mass_bkg");
  RooRealVar *coef2_bern_mass_bkg=ws->var("coef2_bern_mass_bkg");
  RooRealVar *coef3_bern_mass_bkg=ws->var("coef3_bern_mass_bkg");
  RooRealVar *ggh_yield=ws->var("ggh_yield");
  RooRealVar *bkg_yield=ws->var("bkg_yield");

  mean_mass_ggh->setConstant(1);
  sigma_mass_ggh->setConstant(1);
  coef0_bern_mass_bkg->setConstant(1);
  coef1_bern_mass_bkg->setConstant(1);
  coef2_bern_mass_bkg->setConstant(1);
  coef3_bern_mass_bkg->setConstant(1);


  model->fitTo(*sim_gen);

  // Check plot
  TLatex latex; latex.SetNDC(1); char buffer[100];
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  TLegend *legend=new TLegend(0.8,0.85,1,1);
  RooPlot* frame_mass=dipho_mass->frame(110,180,35); frame_mass->UseCurrentStyle();
  frame_mass->GetYaxis()->SetLabelSize(0.03);
  sim_gen->plotOn(frame_mass,Name("data"));
  model->plotOn(frame_mass,Name("full"));
  model->plotOn(frame_mass, Components("model_ggh"), LineColor(kGreen), LineStyle(kDashed),Name("ggh"));
  model->plotOn(frame_mass, Components("model_bkg"), LineColor(kRed), LineStyle(kDashed),Name("bkg"));
  frame_mass->Draw();
  latex.DrawLatex(0.5,0.86,"ggh yield");
  sprintf(buffer,"fitted = %3.0f",ggh_yield->getVal()); 
  latex.DrawLatex(0.5,0.81,buffer);
  sprintf(buffer,"expected = %3.0f",exp_ggh_yield->getVal());
  latex.DrawLatex(0.5,0.76,buffer);
  legend->AddEntry(frame_mass->findObject("data"),"MC gen","lpe");
  legend->AddEntry(frame_mass->findObject("full"),"Full Fit","l");
  legend->AddEntry(frame_mass->findObject("ggh"),"ggh Fit","l");
  legend->AddEntry(frame_mass->findObject("bkg"),"bkg Fit","l");
  legend->Draw();
  canvas->SaveAs("frameDoSplotMass_gen.pdf");
  frame_mass->Delete();

  RooPlot* frame_pt=dipho_pt->frame();
  frame_pt=dipho_pt->frame();
  sim_gen->plotOn(frame_pt);
  model->plotOn(frame_pt);
  model->plotOn(frame_pt, Components("model_ggh"), LineColor(kGreen), LineStyle(kDashed));
  model->plotOn(frame_pt, Components("model_bkg"), LineColor(kRed), LineStyle(kDashed));
  frame_pt->Draw();
  canvas->SaveAs("frameDoSplotPt_gen.pdf");
  frame_pt->Delete();
  canvas->Close();

  RooDataSet *sim_genW=new RooDataSet(*sim_gen,"sim_genW");			 
  SPlot *splot_gen=new SPlot("splot_gen","splot_gen", *sim_genW, model, RooArgList(*ggh_yield, *bkg_yield),*dipho_pt);  //Create splot
  ws->import(*sim_genW,Rename("sim_genW"));


  cout << "end DoSPlot" << endl;
  return 0;
}

//#############################################################################################"
//#############################################################################################"
//#############################################################################################"
int MakePlot(RooWorkspace* ws, int const &cut=0) {

  setTDRStyle();

  RooRealVar *dipho_mass=ws->var("dipho_mass");
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooAbsPdf *model=ws->pdf("model_gghbkg");
  RooDataSet *sim_gen= (RooDataSet*) ws->data("sim_gen_gghbkg");
  RooDataSet *sim_genW=(RooDataSet*) ws->data("sim_genW_gghbkg");
  RooRealVar *bkg_yield=ws->var("bkg_yield");
  RooRealVar *ggh_yield=ws->var("ggh_yield");
  TLatex latex;
  latex.SetNDC();  
  char buffer[100];


  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  TPad *pad_down=new TPad("pad_down","pad_down",0,0,1,0.3);
  TPad *pad_up=new TPad("pad_up","pad_up",0,0.3,1,1);
  TLegend *legend=new TLegend(0.7,0.7,1,1);
  pad_down->Draw();
  pad_up->Draw();

  //define path prefixes  for saving canvas
  char buffer_path[100]="",buffer_file[2][3][10]={{""}},buffercut[10]="";
  sprintf(buffer_file[1][0],"png");
  sprintf(buffer_file[1][1],"pdf");
  sprintf(buffer_file[1][2],"root");
  if (! BATCH) {
    sprintf(buffer_path,"/afs/cern.ch/work/c/cgoudet/private/plot/");
    sprintf(buffer_file[0][0],"png/");
    sprintf(buffer_file[0][1],"pdf/");
    sprintf(buffer_file[0][2],"root/");
  }
  if (cut) sprintf(buffercut,"%d",cut);


  //Check plots of the fit before splot
  RooPlot *frame_up=dipho_pt->frame();
  RooPlot *frame_down=dipho_mass->frame();
  sim_gen->plotOn(frame_up,Name("sim_gen"),Binning(NBINS,0,200),MarkerColor(kBlack));
  model->plotOn(frame_up, LineColor(kBlue),Name("model"));
  model->plotOn(frame_up,Components("model_ggh"),LineColor(kGreen),LineStyle(kDashed),Name("model_ggh"));
  model->plotOn(frame_up,Components("model_bkg"),LineColor(kRed),LineStyle(kDashed),Name("model_bkg"));

  sim_gen->plotOn(frame_down,Binning(40,100,180),MarkerColor(kBlack));
  model->plotOn(frame_down,LineColor(kBlue));
  model->plotOn(frame_down,Components("model_ggh"),LineColor(kGreen),LineStyle(kDashed));
  model->plotOn(frame_down,Components("model_bkg"),LineColor(kRed),LineStyle(kDashed));

  pad_up->cd();

  frame_up->UseCurrentStyle();
  frame_up->Draw();
  pad_down->cd();
  frame_down->UseCurrentStyle();
  frame_down->Draw();
  legend->SetTextSize(0.04);
  legend->AddEntry(frame_up->findObject("sim_gen"),"Inclusive Generated Events","lpe");
  legend->AddEntry(frame_up->findObject("model"),"Inclusive Fit","l");
  legend->AddEntry(frame_up->findObject("model_ggh"),"ggH Component","l");
  legend->AddEntry(frame_up->findObject("model_bkg"),"bkg Component","l");
  legend->Draw();

  sprintf(buffer,"signal yields = %3.2f",ggh_yield->getVal());
  latex.DrawLatex(0.2,0.96,buffer);
  if (cut)  {
    sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
    canvas->cd();    
    latex.DrawLatex(0.4,0.49,buffer);
  }
  
    
  //saving canvas
  for (int file=0;file<3;file++) {
    sprintf(buffer,"%s%ssplot_gen_unweighted_gghbkg%s.%s",buffer_path,buffer_file[0][file],buffercut,buffer_file[1][file]);  
    canvas->SaveAs(buffer);
    }  

    frame_up->Delete();
    frame_down->Delete();
    legend->Delete();
  
  cout << "plotted non weighted" << endl;



  // plot weighted events and first fit
  frame_up=dipho_pt->frame();
  RooDataSet *sim_gen_Wggh=new RooDataSet("sim_gen_Wggh","sim_gen_Wggh",sim_genW,*sim_genW->get(),0,"ggh_yield_sw");
  sim_gen_Wggh->plotOn(frame_up,MarkerColor(2),LineColor(2),DataError(RooAbsData::SumW2),Name("sim_gen_Wggh"),Binning(NBINS,0,200));// plot weighted ggh events
  RooDataSet *sim_gen_Wbkg=new RooDataSet("sim_gen_Wbkg","sim_gen_Wbkg",sim_genW,*sim_genW->get(),0,"bkg_yield_sw");
  sim_gen_Wbkg->plotOn(frame_up,MarkerColor(1),LineColor(1),DataError(RooAbsData::SumW2),Name("sim_gen_Wbkg"),Binning(NBINS,0,200)); // plot weighted background events
   model->plotOn(frame_up,Components("model_bkg"),LineColor(4),Name("model_bkg"));// background component of the total fit
   model->plotOn(frame_up,Components("model_ggh"),LineColor(3),Name("model_ggh"));//ggh component of total fit 


  canvas->cd();
  frame_up->UseCurrentStyle();
  frame_up->Draw();
  legend=new TLegend(0.6,0.7,1,1);
  legend->SetTextSize(0.04);
  legend->AddEntry("","Generator Level","");
  legend->AddEntry("sim_gen_Wggh", "ggH Weighted Events","lpe");
  legend->AddEntry("sim_gen_Wbkg", "bkg Weighted Events","lpe");
  legend->AddEntry("model_ggh","Non Weighted ggH Fit","l");
  legend->AddEntry("model_bkg","Non Weighted Bkg Fit","l");
  legend->Draw();

  if (cut) {
      sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
      canvas->cd();
      latex.DrawLatex(0.4,0.96,buffer);
  }

  //saving canvas
  for (int file=0; file<3; file++ ) {
    sprintf(buffer,"%s%ssplot_gen_gghbkg%s.%s",buffer_path,buffer_file[0][file],buffercut,buffer_file[1][file]);
      canvas->SaveAs(buffer);
  }
  cout << "wieghted drawn" << endl;

  frame_up->Delete();

  // plot weighted signal events and first fit
  frame_up=dipho_pt->frame();
  sim_gen_Wggh->plotOn(frame_up,MarkerColor(2),LineColor(2),DataError(RooAbsData::SumW2),Name("sim_gen_Wggh"),Binning(NBINS,0,200));// plot weighted ggh events
  model->plotOn(frame_up,Components("model_ggh"),LineColor(3),Normalization(sim_gen_Wbkg->sumEntries(),RooAbsReal::NumEvent),Name("model_ggh"));//ggh component of total fit 
  cout << "sim_gen_ggh sum entries " << sim_gen_Wggh->sumEntries() << endl;
   canvas->cd();
   canvas->Clear();
   cout << "canvas cleared" << endl;  
   pad_up=new TPad("pad_up","pad_up",0,0.3,1,1);
   pad_up->SetBottomMargin(0);
   pad_up->Draw();
   pad_up->cd();
   cout << "pad accepted" << endl;
   frame_up->UseCurrentStyle();
   frame_up->GetXaxis()->SetTitleSize(0);
   frame_up->GetXaxis()->SetLabelSize(0);
   frame_up->Draw();
  legend=new TLegend(0.6,0.75,1,1);
  legend->SetTextSize(0.05);
  legend->AddEntry("","Generator Level","");
  legend->AddEntry("sim_gen_Wggh", "ggH Weighted Events","lpe");
  legend->AddEntry("model_ggh","Non Weighted ggH Fit","l");
  legend->Draw();

  cout << "drawn on pad up" << endl;
  TH1F *hist_fit=(TH1F*) model->createHistogram("hist_fit",*dipho_pt,Binning(NBINS,0,NBINS*WIDTH),ConditionalObservables(*dipho_mass));
  hist_fit->Scale(sim_gen_Wggh->sumEntries());
  cout << "hist_fit created" << endl;

  TH1F *hist_set=new TH1F("hist_set","hist_set",NBINS,0,NBINS*WIDTH);
  sim_gen_Wggh->fillHistogram(hist_set,*dipho_pt);
  hist_set->SetTitle("test");
  cout << "hist_set done" << endl;
  TLine *line=new TLine(0,0,NBINS*WIDTH,0);
  line->SetLineColor(kRed);
  hist_set->Add(hist_fit,-1);
  hist_set->Divide(hist_fit);
  hist_set->GetYaxis()->SetTitle("#frac{N_{MC}-N_{fit}}{N_{fit}}");
  hist_set->GetYaxis()->SetTitleOffset(0.3);
  hist_set->GetXaxis()->SetTitle(frame_up->GetXaxis()->GetTitle());
  //  hist_set->GetYaxis()->SetRangeUser(-1,1);                                                                                                              
  cout << "divided" << endl;
  hist_set->GetXaxis()->SetLabelSize(0.15);
  hist_set->GetYaxis()->SetLabelSize(0.1);
  hist_set->GetXaxis()->SetTitleSize(0.15);
  hist_set->GetXaxis()->SetTitleOffset(1);
  hist_set->GetYaxis()->SetTitleSize(0.15);

  canvas->cd();
  pad_down=new TPad("pad_down","pad_down",0,0,1,0.3);
  pad_down->SetTopMargin(0);
  pad_down->SetBottomMargin(0.35);
  pad_down->Draw();
  pad_down->cd();
  hist_set->Draw();
  line->Draw("SAME");



  if (BATCH) {
    if(cut) {
      sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
      canvas->cd();
      latex.DrawLatex(0.4,0.96,buffer);
 }

  for (int file=0; file<3; file++ ) {
    sprintf(buffer,"%s%ssplot_gen_ggh%s.%s",buffer_path,buffer_file[0][file],buffercut,buffer_file[1][file]);
      canvas->SaveAs(buffer);
  }

  cout << "weigthed ggh drawn" << endl;




  return 0;
}
