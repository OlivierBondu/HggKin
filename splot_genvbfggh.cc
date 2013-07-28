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
  if (BATCH) root_file=new TFile("WS_SPlot.root","UPDATE"); //File to store the workspace
  else root_file=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/WS_SPlot.root","UPDATE");
  RooWorkspace *ws=0;
  char buffer[100];


  int i=3;
  //  for (int i=0;i<5;i++) {
  if (i)    sprintf(buffer,"ws_hgg_%d",menu_cut[i]);
  else sprintf(buffer,"ws_hgg");
  ws=new RooWorkspace (buffer,buffer);
  if (AddModel(ws,menu_cut[i])) cout << "AddModel failed" << endl;
  if (AddData(ws,menu_cut[i])) cout << "AddData failed" << endl;
  if (DoSPlot(ws)) cout << "DoSPlot failes" << endl;
  if (MakePlot(ws,menu_cut[i])) cout << "Plotting failed" << endl;
  root_file->cd();
  ws->Write("",TObject::kOverwrite);
  ws->Delete();  
  //}
  root_file->Close();
  cout << "Went up to the end" << endl;
  return 0;
}


//######################################################################################################################################
//######################################################################################################################################
//######################################################################################################################################
int AddModel(RooWorkspace *ws, int  const &cut=0) {
 
  TFile *file_kin=0;
  if (BATCH) file_kin=new TFile("kin_dist.root");
  else file_kin=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
  
  
  RooRealVar *dipho_mass=new RooRealVar("dipho_mass","m_{#gamma #gamma}",110,180,"GeV/c^{2}");
  RooRealVar *dipho_pt=new RooRealVar("dipho_pt","p_{T#gamma#gamma}",100,0,200,"GeV/c");
  RooRealVar *dipho_ctheta=new RooRealVar("dipho_ctheta","cos(#theta *)",0.5,0,1);
  RooRealVar *weight=new RooRealVar("weight","weight",0.1);
  RooPlot *frame_mass=0,*frame_pt=0;
  TTree *tree=0;
  RooDataSet *sim_gen=0;
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  char buffer[100];  


  //Model of vbf =gauss_mass * tsallispol2
  cout << "#####  Model vbf" << endl;
  //create model of background and fit  
  RooRealVar *mean_mass_vbf=new RooRealVar("mean_mass_vbf","mean_mass_vbf",123,120,130);
  RooRealVar *sigma_mass_vbf=new RooRealVar("sigma_mass_vbf","sigma_mass_vbf",3e-2,0,1);
  RooGaussian *model_mass_vbf=new RooGaussian("model_mass_vbf","model_mass_vbf",*dipho_mass,*mean_mass_vbf,*sigma_mass_vbf);


  tree=(TTree*) file_kin->Get("tree_gen_vbf");
//select data according cuts on theta
  if (cut) sprintf(buffer, "dipho_ctheta > %1.3f",cut/1000.);   
  else sprintf(buffer,"");
  sim_gen=new RooDataSet("sim_gen","sim_gen",tree , RooArgSet(*dipho_mass,*dipho_ctheta,*weight), buffer,"weight");
  model_mass_vbf->fitTo(*sim_gen);


  //set variable range to +-3 sigmas to ease splot fittin
  mean_mass_vbf->setRange(mean_mass_vbf->getVal()-2*mean_mass_vbf->getError(),mean_mass_vbf->getVal()+2*mean_mass_vbf->getError());
  sigma_mass_vbf->setRange(sigma_mass_vbf->getVal()-2*sigma_mass_vbf->getError(),sigma_mass_vbf->getVal()+2*sigma_mass_vbf->getError());
  
  //Check plot
//   frame_mass=dipho_mass->frame();
//   sim_gen->plotOn(frame_mass);
//   model_mass_vbf->plotOn(frame_mass);
//   model_mass_vbf->plotOn(frame_mass,Components("model_smear_mass_vbf"),LineColor(kGreen),LineStyle(kDashed));
//   frame_mass->Draw();
//   canvas->SaveAs("frame_massvbf.pdf");
//   frame_mass->Delete();

  sim_gen->Delete();
  sim_gen=0;


  RooRealVar *coef0_pol_pt_vbf=new RooRealVar("coef0_pol_pt_vbf","coef0_pol_pt_vbf",7,-1,100);
  RooRealVar *coef1_pol_pt_vbf=new RooRealVar("coef1_pol_pt_vbf","coef1_pol_pt_vbf",7,-1,100);
  RooRealVar *coef2_pol_pt_vbf=new RooRealVar("coef2_pol_pt_vbf","coef2_pol_pt_vbf",7,-1,100);
  RooBernstein *pol_pt_vbf=new RooBernstein("pol_pt_vbf","pol_pt_vbf",*dipho_pt,RooArgSet(*coef0_pol_pt_vbf, *coef1_pol_pt_vbf, *coef2_pol_pt_vbf));

  RooRealVar *coefM_tsallis_pt_vbf=new RooRealVar("coefM_tsallis_pt_vbf","coefM_tsallis_pt_vbf",125,0,300);
  RooRealVar *coefT_tsallis_pt_vbf=new RooRealVar("coefT_tsallis_pt_vbf","coefT_tsallis_pt_vbf",10,0,50);
  RooRealVar *coefN_tsallis_pt_vbf=new RooRealVar("coefN_tsallis_pt_vbf","coefN_tsallis_pt_vbf",5,2,30);
  RooGenericPdf *tsallis_pt_vbf=new RooGenericPdf("tsallis_pt_vbf","(coefN_tsallis_pt_vbf-1)*(coefN_tsallis_pt_vbf-2)/coefN_tsallis_pt_vbf/coefT_tsallis_pt_vbf/(coefN_tsallis_pt_vbf*coefT_tsallis_pt_vbf+(coefN_tsallis_pt_vbf-2)*coefM_tsallis_pt_vbf)*dipho_pt*pow(1+(sqrt(coefM_tsallis_pt_vbf*coefM_tsallis_pt_vbf+dipho_pt*dipho_pt)-coefM_tsallis_pt_vbf)/coefN_tsallis_pt_vbf/coefT_tsallis_pt_vbf,-coefN_tsallis_pt_vbf)",RooArgSet(*dipho_pt,*coefN_tsallis_pt_vbf,*coefM_tsallis_pt_vbf,*coefT_tsallis_pt_vbf));
  
  
  RooProdPdf *model_pt_vbf=new RooProdPdf("model_pt_vbf","model_pt_vbf",RooArgList(*pol_pt_vbf,*tsallis_pt_vbf));
  //  RooGenericPdf *model_pt_vbf=tsallis_pt_vbf;
  
  if (cut) sprintf(buffer,"dipho_ctheta > %1.3f", cut/1000.); 
  else sprintf(buffer,"");
  sim_gen=new RooDataSet("sim_gen","sim_gen", tree, RooArgSet(*dipho_pt,*dipho_ctheta, *weight),buffer,"weight");
  model_pt_vbf->fitTo(*sim_gen);

  coef0_pol_pt_vbf->setRange(coef0_pol_pt_vbf->getVal()-2*coef0_pol_pt_vbf->getError(),coef0_pol_pt_vbf->getVal()+2*coef0_pol_pt_vbf->getError());
  coef1_pol_pt_vbf->setRange(coef1_pol_pt_vbf->getVal()-2*coef1_pol_pt_vbf->getError(),coef1_pol_pt_vbf->getVal()+2*coef1_pol_pt_vbf->getError());
  coef2_pol_pt_vbf->setRange(coef2_pol_pt_vbf->getVal()-2*coef2_pol_pt_vbf->getError(),coef2_pol_pt_vbf->getVal()+2*coef2_pol_pt_vbf->getError());
  coefM_tsallis_pt_vbf->setRange(coefM_tsallis_pt_vbf->getVal()-2*coefM_tsallis_pt_vbf->getError(),coefM_tsallis_pt_vbf->getVal()+2*coefM_tsallis_pt_vbf->getError());
  coefN_tsallis_pt_vbf->setRange(coefN_tsallis_pt_vbf->getVal()-2*coefN_tsallis_pt_vbf->getError(),coefN_tsallis_pt_vbf->getVal()+2*coefN_tsallis_pt_vbf->getError());
  coefT_tsallis_pt_vbf->setRange(coefT_tsallis_pt_vbf->getVal()-2*coefT_tsallis_pt_vbf->getError(),coefT_tsallis_pt_vbf->getVal()+2*coefT_tsallis_pt_vbf->getError());
  

  //Check plot
//     frame_pt=dipho_pt->frame();
//     sim_gen->plotOn(frame_pt);
//     model_pt_vbf->plotOn(frame_pt);
//     frame_pt->Draw();
//     canvas->SaveAs("frame_ptvbf.pdf");
//     frame_pt->Delete();
  
  sim_gen->Delete();
  tree->Delete();

  
  RooProdPdf *model_vbf=new RooProdPdf("model_vbf","model_vbf",RooArgList(*model_pt_vbf,*model_mass_vbf));
  
 

  //#################################
  cout << "#####   Model ggh" << endl;
  RooRealVar *mean_mass_ggh=new RooRealVar("mean_mass_ggh","mean_mass_ggh",123,120,130);
  RooRealVar *sigma_mass_ggh=new RooRealVar("sigma_mass_ggh","sigma_mass_ggh",3e-2,0,1);
  RooGaussian *model_mass_ggh=new RooGaussian("model_mass_ggh","model_mass_ggh",*dipho_mass,*mean_mass_ggh,*sigma_mass_ggh);

  
  tree=(TTree*) file_kin->Get("tree_gen_ggh");
  if (cut)  sprintf(buffer,"dipho_ctheta > %1.3f",cut/1000.);
  else   sprintf(buffer,"");
  sim_gen=new RooDataSet("data","data", tree, RooArgSet( *dipho_mass, *dipho_ctheta,*weight), buffer,"weight");
  model_mass_ggh->fitTo(*sim_gen);
  
  mean_mass_ggh->setRange(mean_mass_ggh->getVal()-2*mean_mass_ggh->getError(),mean_mass_ggh->getVal()+2*mean_mass_ggh->getError());
  sigma_mass_ggh->setRange(sigma_mass_ggh->getVal()-2*sigma_mass_ggh->getError(),sigma_mass_ggh->getVal()+2*sigma_mass_ggh->getError());
  
  //Check plot
//     frame_mass=dipho_mass->frame();
//     sim_gen->plotOn(frame_mass,Binning(80,100,180));
//     model_mass_ggh->plotOn(frame_mass);
//     frame_mass->Draw();
//     canvas->SaveAs("frame_massggh.pdf");
//     frame_mass->Delete();

    sim_gen->Delete();

  RooRealVar *coef0_pol_pt_ggh=new RooRealVar("coef0_pol_pt_ggh","coef0_pol_pt_ggh",14,0.001,50);
  RooRealVar *coef1_pol_pt_ggh=new RooRealVar("coef1_pol_pt_ggh","coef1_pol_pt_ggh",0.7,0.001,50);
  RooRealVar *coef2_pol_pt_ggh=new RooRealVar("coef2_pol_pt_ggh","coef2_pol_pt_ggh",0.7,0.001,50);

  RooBernstein *pol_pt_ggh=new RooBernstein("pol_pt_ggh","pol_pt_ggh",*dipho_pt,RooArgSet(*coef0_pol_pt_ggh,*coef1_pol_pt_ggh,*coef2_pol_pt_ggh));


  RooRealVar *theta_logn_pt_ggh=new RooRealVar("theta_logn_pt_ggh","theta_logn_pt_ggh",-1,-30,30);
  RooRealVar *sigma_logn_pt_ggh=new RooRealVar("sigma_logn_pt_ggh","sigma_logn_pt_ggh",6,0,300);
  RooRealVar *M_logn_pt_ggh=new RooRealVar("M_logn_pt_ggh","M_logn_pt_ggh",6,0,300);
  RooGenericPdf *logn_pt_ggh=new RooGenericPdf("logn_pt_ggh","TMath::LogNormal(dipho_pt,sigma_logn_pt_ggh,theta_logn_pt_ggh,M_logn_pt_ggh)",RooArgSet(*dipho_pt, *M_logn_pt_ggh, * theta_logn_pt_ggh, *sigma_logn_pt_ggh));

  RooRealVar *coefM_tsallis_pt_ggh=new RooRealVar("coefM_tsallis_pt_ggh","coefM_tsallis_pt_ggh",0.0001,-1,300);
  RooRealVar *coefT_tsallis_pt_ggh=new RooRealVar("coefT_tsallis_pt_ggh","coefT_tsallis_pt_ggh",6,0,50);
  RooRealVar *coefN_tsallis_pt_ggh=new RooRealVar("coefN_tsallis_pt_ggh","coefN_tsallis_pt_ggh",3,2,30);
  RooGenericPdf *tsallis_pt_ggh=new RooGenericPdf("tsallis_pt_ggh","(coefN_tsallis_pt_ggh-1)*(coefN_tsallis_pt_ggh-2)/coefN_tsallis_pt_ggh/coefT_tsallis_pt_ggh/(coefN_tsallis_pt_ggh*coefT_tsallis_pt_ggh+(coefN_tsallis_pt_ggh-2)*coefM_tsallis_pt_ggh)*dipho_pt*pow(1+(sqrt(coefM_tsallis_pt_ggh*coefM_tsallis_pt_ggh+dipho_pt*dipho_pt)-coefM_tsallis_pt_ggh)/coefN_tsallis_pt_ggh/coefT_tsallis_pt_ggh,-coefN_tsallis_pt_ggh)",RooArgSet(*dipho_pt,*coefN_tsallis_pt_ggh,*coefM_tsallis_pt_ggh,*coefT_tsallis_pt_ggh));
  
  RooProdPdf *model_pt_ggh=new RooProdPdf("model_pt_ggh","model_pt_ggh",RooArgSet(*tsallis_pt_ggh,*pol_pt_ggh));
  

  if (cut) sprintf(buffer,"dipho_ctheta > %1.3f", cut/1000.);
  else     sprintf(buffer,"");
  sim_gen=new RooDataSet("sim_gen","sim_gen", tree, RooArgSet(*dipho_pt,*dipho_mass,*dipho_ctheta,*weight),buffer,"weight");
  model_pt_ggh->fitTo(*sim_gen);
  

  coef0_pol_pt_ggh->setRange(coef0_pol_pt_ggh->getVal()-2*coef0_pol_pt_ggh->getError(),coef0_pol_pt_ggh->getVal()+2*coef0_pol_pt_ggh->getError());
  coef1_pol_pt_ggh->setRange(coef1_pol_pt_ggh->getVal()-2*coef1_pol_pt_ggh->getError(),coef1_pol_pt_ggh->getVal()+2*coef1_pol_pt_ggh->getError());
  coef2_pol_pt_ggh->setRange(coef2_pol_pt_ggh->getVal()-2*coef2_pol_pt_ggh->getError(),coef2_pol_pt_ggh->getVal()+2*coef2_pol_pt_ggh->getError());
  coefM_tsallis_pt_ggh->setRange(coefM_tsallis_pt_ggh->getVal()-2*coefM_tsallis_pt_ggh->getError(),coefM_tsallis_pt_ggh->getVal()+2*coefM_tsallis_pt_ggh->getError());
  coefN_tsallis_pt_ggh->setRange(coefN_tsallis_pt_ggh->getVal()-2*coefN_tsallis_pt_ggh->getError(),coefN_tsallis_pt_ggh->getVal()+2*coefN_tsallis_pt_ggh->getError());
  coefT_tsallis_pt_ggh->setRange(coefT_tsallis_pt_ggh->getVal()-2*coefT_tsallis_pt_ggh->getError(),coefT_tsallis_pt_ggh->getVal()+2*coefT_tsallis_pt_ggh->getError());
     
  

  //Check plot
//   frame_pt=dipho_pt->frame();
//   sim_gen->plotOn(frame_pt);
//   model_pt_ggh->plotOn(frame_pt);
//   frame_pt->Draw();
//   canvas->SaveAs("frame_otggh.pdf");
//   frame_pt->Delete();

  sim_gen->Delete();
  tree->Delete();

  RooProdPdf *model_ggh=new RooProdPdf("model_ggh","model_ggh",RooArgSet(*model_mass_ggh,*model_pt_ggh));


  //Combine the models 
  RooRealVar *vbf_yield=new RooRealVar("vbf_yield","vbf_yield",100,10,1000);
  RooRealVar *ggh_yield=new RooRealVar("ggh_yield","ggh_yield",101,10,500);
  
  RooAddPdf *model_vbfggh= new RooAddPdf("model_vbfggh","model_vbfggh",RooArgList(*model_vbf,*model_ggh),RooArgList(*vbf_yield,*ggh_yield));  

  // Import all pdf and variables (implicitely) into workspace
  ws->import(*weight);
  ws->import(*dipho_ctheta);
  ws->import(*model_vbfggh);

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
  char buffer[100]="dipho_mass<180 && dipho_mass >100",buff_cut[100];
  TFile *file_kin=0;
  if (BATCH) file_kin=new TFile("kin_dist.root");
  else file_kin=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");

  if (cut) sprintf(buff_cut,"dipho_ctheta>%d/1000.",cut);
  else sprintf(buff_cut,"");
  TTree *tree =(TTree *) file_kin->Get("tree_gen_ggh");
  RooDataSet *sim_gen_vbfggh=new RooDataSet("sim_gen_vbfggh","sim_gen_vbfggh",tree,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*weight),buff_cut,"weight");
  tree->Delete();


  tree=(TTree*) file_kin->Get("tree_gen_vbf");
  RooDataSet *sim_gen=new RooDataSet("vbfggh","vbfggh",tree,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*weight),buff_cut, "weight"); //Put cut tree into the dataset with weight
  sim_gen_vbfggh->append(*sim_gen);  
  tree->Delete();

  

  
  //Check Plot
//     TCanvas *canvas=new TCanvas();
//     RooPlot *framemass=dipho_mass->frame();
//     sim_gen->plotOn(framemass, Binning(40,100,180));
//     framemass->Draw();
//     canvas->SaveAs("frame_mass.pdf");
//     RooPlot *framept=dipho_pt->frame();
//     sim_gen_vbf->plotOn(framept);
//     framept->Draw();
//     canvas->SaveAs("frame_pt.pdf");  
    
  //import dataset into workspace
  ws->import(*sim_gen_vbfggh);
  file_kin->Close();

  return 0;
}


//###################################################################################################
//###################################################################################################
//###################################################################################################
int DoSPlot(RooWorkspace* ws) {
  setTDRStyle();
  RooAbsPdf *model_vbfggh=ws->pdf("model_vbfggh");
  RooDataSet *sim_gen_vbfggh=(RooDataSet*) ws->data("sim_gen_vbfggh");
  RooRealVar *dipho_mass=ws->var("dipho_mass");  
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooDataSet *sim_genW_vbfggh=new RooDataSet(*sim_gen_vbfggh,"sim_genW_vbfggh");



  //Fixing discriminant parameters
  RooRealVar *mean_mass_vbf=ws->var("mean_mass_vbf");
  RooRealVar *sigma_mass_vbf=ws->var("sigma_mass_vbf");
  RooRealVar *mean_mass_ggh=ws->var("mean_mass_ggh");
  RooRealVar *sigma_mass_ggh=ws->var("sigma_mass_ggh");
  RooRealVar *vbf_yield=ws->var("vbf_yield");
  RooRealVar *ggh_yield=ws->var("ggh_yield");

  mean_mass_vbf->setConstant();
  sigma_mass_vbf->setConstant();
  mean_mass_ggh->setConstant();
  sigma_mass_ggh->setConstant();

  model_vbfggh->fitTo(*sim_gen_vbfggh);


  // Check plot
  TLatex latex; latex.SetNDC(1); char buffer[100];
  TCanvas *canvas=new TCanvas();
  RooPlot* frame_mass=dipho_mass->frame(); frame_mass->UseCurrentStyle();
  sim_gen_vbfggh->plotOn(frame_mass,Range(124.9,125.1));
  model_vbfggh->plotOn(frame_mass);
  model_vbfggh->plotOn(frame_mass, Components("model_vbf"), LineColor(kGreen), LineStyle(kDashed));
  model_vbfggh->plotOn(frame_mass, Components("model_ggh"), LineColor(kRed), LineStyle(kDashed));
  frame_mass->Draw();
  sprintf(buffer,"vbf_yields=%3.2f   #chi^2=%3.2f",vbf_yield->getVal(),frame_mass->chiSquare()); 
  latex.DrawLatex(0.6,0.9,buffer);
  canvas->SaveAs("frameDoSplotMass.pdf");
  frame_mass->Delete();

  //   RooPlot* frame_pt=dipho_pt->frame();
  //   frame_pt=dipho_pt->frame();
  //   sim_gen->plotOn(frame_pt);
  //   model->plotOn(frame_pt);
  //   model->plotOn(frame_pt, Components("model_vbf"), LineColor(kGreen), LineStyle(kDashed));
  //   model->plotOn(frame_pt, Components("model_ggh"), LineColor(kRed), LineStyle(kDashed));
  //   frame_pt->Draw();
  //   canvas->SaveAs("frameDoSplotPt.pdf");
  //   frame_pt->Delete();
  //   canvas->Close();

			 
			 
  SPlot *splot_vbfggh=new SPlot("splot_vbfggh","splot_vbfggh", *sim_genW_vbfggh, model_vbfggh, RooArgList(*vbf_yield, *ggh_yield));  //Create splot

  ws->import(*sim_genW_vbfggh);


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
  RooAbsPdf *model=ws->pdf("model_vbfggh");
  RooDataSet *sim_gen= (RooDataSet*) ws->data("sim_gen_vbfggh");
  RooDataSet *sim_genW=(RooDataSet*) ws->data("sim_genW_vbfggh");
  RooRealVar *ggh_yield=ws->var("ggh_yield");
  RooRealVar *vbf_yield=ws->var("vbf_yield");
  TLatex latex;
  latex.SetNDC();  
  char buffer[100];


  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  TPad *pad_down=new TPad("pad_down","pad_down",0,0,1,0.5);
  TPad *pad_up=new TPad("pad_up","pad_up",0,0.5,1,1);
  TLegend *legend=new TLegend(0.8,0.7,1,1);
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
  sim_gen->plotOn(frame_up,Name("sim_gen"),Binning(100,0,200),MarkerColor(kBlack));
  model->plotOn(frame_up, LineColor(kBlue),Name("model"));
  model->plotOn(frame_up,Components("model_vbf"),LineColor(kGreen),LineStyle(kDashed),Name("model_vbf"));
  model->plotOn(frame_up,Components("model_ggh"),LineColor(kRed),LineStyle(kDashed),Name("model_ggh"));

  sim_gen->plotOn(frame_down,Binning(40,100,180),MarkerColor(kBlack));
  model->plotOn(frame_down,LineColor(kBlue));
  model->plotOn(frame_down,Components("model_vbf"),LineColor(kGreen),LineStyle(kDashed));
  model->plotOn(frame_down,Components("model_ggh"),LineColor(kRed),LineStyle(kDashed));

  pad_up->cd();
  //  pad_up->SetLogy(1);
  frame_up->UseCurrentStyle();
  frame_up->Draw();
  pad_down->cd();
  //  pad_down->SetLogy(1);
  frame_down->UseCurrentStyle();
  frame_down->Draw();

  legend->AddEntry(frame_up->findObject("sim_gen"),"Inclusive Generated Events","lpe");
  legend->AddEntry(frame_up->findObject("model"),"Inclusive Fit","l");
  legend->AddEntry(frame_up->findObject("model_vbf"),"vbf Component","l");
  legend->AddEntry(frame_up->findObject("model_ggh"),"ggh Component","l");
  legend->Draw();

  sprintf(buffer,"signal yields = %3.2f",vbf_yield->getVal());
  latex.DrawLatex(0.2,0.96,buffer);
  if (cut)  {
    sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
    canvas->cd();    
    latex.DrawLatex(0.4,0.49,buffer);
  }
  
    
  //saving canvas
  for (int file=0;file<3;file++) {
    sprintf(buffer,"%s%ssplot_gen_unweighted_vbfggh%s.%s",buffer_path,buffer_file[0][file],buffercut,buffer_file[1][file]);  
    canvas->SaveAs(buffer);
    }  

    frame_up->Delete();
    frame_down->Delete();
    legend->Delete();
  
  cout << "plotted non weighted" << endl;



  // plot weighted events and first fit
  frame_up=dipho_pt->frame();
  RooDataSet *sim_gen_Wvbf=new RooDataSet("sim_gen_Wvbf","sim_gen_Wvbf",sim_genW,*sim_genW->get(),0,"vbf_yield_sw");
   sim_gen_Wvbf->plotOn(frame_up,MarkerColor(2),DataError(RooAbsData::SumW2),Name("sim_gen_Wvbf"),Binning(100,0,200));// plot weighted vbf events
  RooDataSet *sim_gen_Wggh=new RooDataSet("sim_gen_Wggh","sim_gen_Wggh",sim_genW,*sim_genW->get(),0,"ggh_yield_sw");
  sim_gen_Wggh->plotOn(frame_up,MarkerColor(1),DataError(RooAbsData::SumW2),Name("sim_gen_Wggh"),Binning(100,0,200)); // plot weighted background events
   model->plotOn(frame_up,Components("model_ggh"),LineColor(4),Name("model_ggh"));// background component of the total fit
   model->plotOn(frame_up,Components("model_vbf"),LineColor(3),Name("model_vbf"));//vbf component of total fit 


  canvas->cd();
  //  canvas->SetLogy();
  frame_up->UseCurrentStyle();
  frame_up->Draw();
  legend=new TLegend(0.8,0.7,1,1);
  legend->AddEntry("sim_gen_Wvbf", "vbf Weighted Events","lpe");
  legend->AddEntry("sim_gen_Wggh", "ggh Weighted Events","lpe");
  legend->AddEntry("model_vbf","Non Weighted vbf Fit","l");
  legend->AddEntry("model_ggh","Non Weighted Ggh Fit","l");
  legend->Draw();

  if (cut) {
      sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
      canvas->cd();
      latex.DrawLatex(0.4,0.96,buffer);
  }

  //saving canvas
  for (int file=0; file<3; file++ ) {
    sprintf(buffer,"%s%ssplot_gen_vbfggh%s.%s",buffer_path,buffer_file[0][file],buffercut,buffer_file[1][file]);
      canvas->SaveAs(buffer);
  }
  cout << "wieghted drawn" << endl;

  frame_up->Delete();

  // plot weighted signal events and first fit
  frame_up=dipho_pt->frame();
  sim_gen_Wvbf->plotOn(frame_up,MarkerColor(2),DataError(RooAbsData::SumW2),Name("sim_gen_Wvbf"),Binning(100,0,200));// plot weighted vbf events
  model->plotOn(frame_up,Components("model_vbf"),LineColor(3),Normalization(sim_gen_Wggh->sumEntries(),RooAbsReal::NumEvent),Name("model_vbf"));//vbf component of total fit 
  canvas->cd();
  //   canvas->SetLogy();
  frame_up->UseCurrentStyle();
  frame_up->Draw();
  legend=new TLegend(0.8,0.9,1,1);
  legend->AddEntry("sim_gen_Wvbf", "vbf Weighted Events","lpe");
  legend->AddEntry("model_vbf","Non Weighted vbf Fit","l");
  legend->Draw();

 if(cut) {
      sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
      canvas->cd();
      latex.DrawLatex(0.4,0.96,buffer);
 }

  for (int file=0; file<3; file++ ) {
    sprintf(buffer,"%s%ssplot_gen_vbf%s.%s",buffer_path,buffer_file[0][file],buffercut,buffer_file[1][file]);
      canvas->SaveAs(buffer);
  }

  cout << "weigthed vbf drawn" << endl;




  return 0;
}
