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
#define NBINS 10
#define WIDTH 10

using namespace std;
using namespace RooStats;
using namespace RooFit;

int main() {
  //#############menu
  int const menu_cut[5]={0,200,375,550,750};

  //############def
  int AddModel(RooWorkspace*, int const &cut=0, int const &categ=-1); // Add pdf to workspace and pre-fit them
  int AddData(RooWorkspace*, int const &cut=0, int const &categ=-1); // Add simulated events ro workspace
  int DoSPlot(RooWorkspace*, int const &cut=0, int const &categ=-1); // Create SPlot object
  int MakePlot(RooWorkspace*, int const &cut=0, int const &categ=-1); // Create and save result and check plots

  TFile *root_file=0;
  if (BATCH) root_file=new TFile("WS_SPlot.root","UPDATE"); //File to store the workspace
  else root_file=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/WS_SPlot.root","UPDATE");
  RooWorkspace *ws=0;
  char buffer[100];

   int i=0;
   int categ=-1;
   //    for (int i=0; i<5;i++) {
   // for (int categ=-1; categ<4;categ++) {
      sprintf(buffer,"ws_pt");
      if (i) sprintf(buffer,"%s%d",buffer,menu_cut[i]);
      if (categ>-1) sprintf(buffer,"%s_categ%d",buffer,categ);
      sprintf(buffer,"%s_recoggh",buffer);
      ws=new RooWorkspace(buffer,buffer);
      AddModel(ws,menu_cut[i],categ);
      AddData(ws,menu_cut[i],categ);
      DoSPlot(ws,menu_cut[i],categ);
      MakePlot(ws,menu_cut[i],categ);
      root_file->cd();
      ws->Write("",TObject::kOverwrite);
      ws->Delete();  
      // } 
//       }

  root_file->Close();
  cout << "Went up to the end" << endl;
  return 0;
}


//######################################################################################################################################
//######################################################################################################################################
//######################################################################################################################################
int AddModel(RooWorkspace *ws, int  const &cut=0, int const &categ=0) {
  setTDRStyle(); 
  TFile *file_kin=0;
  if (BATCH) file_kin=new TFile("kin_dist.root");
  else file_kin=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
  
  
  RooRealVar *dipho_mass=new RooRealVar("dipho_mass","m_{#gamma #gamma}",110,180,"GeV/c^{2}");
  RooRealVar *dipho_pt=new RooRealVar("dipho_pt","p_{T#gamma#gamma}",0,NBINS*WIDTH,"GeV/c");
  RooRealVar *dipho_ctheta=new RooRealVar("dipho_ctheta","cos(#theta *)",0,1);
  RooRealVar *weight=new RooRealVar("weight","weight",0,10);
  RooRealVar *category=new RooRealVar("category","category",-1,5);
  RooPlot *frame_mass=0,*frame_pt=0;
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  char buffer[100],buffer2[100];  



  //Model of ggh =gauss_mass * tsallispol2
  cout << "#####  Model ggH" << endl;
  //create model of background and fit  
  RooRealVar *mean1_mass_ggh=new RooRealVar("mean1_mass_ggh","mean1_mass_ggh",123,120,130);
  RooRealVar *sigma1_mass_ggh=new RooRealVar("sigma1_mass_ggh","sigma1_mass_ggh",1,0,5);
  RooGaussian *model1_mass_ggh=new RooGaussian("model1_mass_ggh","model1_mass_ggh",*dipho_mass,*mean1_mass_ggh,*sigma1_mass_ggh);

  RooRealVar *mean2_mass_ggh=new RooRealVar("mean2_mass_ggh","mean2_mass_ggh",126,120,130);
  RooRealVar *sigma2_mass_ggh=new RooRealVar("sigma2_mass_ggh","sigma2_mass_ggh",2,0,5);
  RooGaussian *model2_mass_ggh=new RooGaussian("model2_mass_ggh","model2_mass_ggh",*dipho_mass,*mean2_mass_ggh,*sigma2_mass_ggh);

  RooRealVar *compo_mass=new RooRealVar("compo_mass","compo_mass",0,1);
  RooAddPdf *model_mass_ggh=new RooAddPdf("model_mass_ggh","model_mass_ggh",RooArgSet(*model1_mass_ggh,*model2_mass_ggh),*compo_mass);
  
  TTree*  tree=(TTree*) file_kin->Get("tree_reco_ggh");
  //select data according cuts on theta
  sprintf(buffer,""); sprintf(buffer2,"");
  if (cut>0) {
    sprintf(buffer, "dipho_ctheta > %1.3f",cut/1000.);   
    sprintf(buffer2," && ");
  }
  if (categ>-1) {
    sprintf(buffer,"%s%s category > %2.2f && category < %2.2f",buffer,buffer2,categ-0.1,categ+0.1);
    sprintf(buffer2," && ");
  }
  RooDataSet*  sim_gen=new RooDataSet("sim_gen","sim_gen",tree , RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category,*weight), buffer,"weight");
  sprintf(buffer,"%s%s dipho_mass < 126.5 && dipho_mass > 123.5",buffer,buffer2);
  RooDataSet*  sim_dum=new RooDataSet("sim_dum","sim_dum",tree , RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category,*weight), buffer,"weight");
  RooRealVar *exp_ggh_yield=new RooRealVar("exp_ggh_yield","exp_ggh_yield",sim_gen->sumEntries());
  model_mass_ggh->fitTo(*sim_gen,Range(110,140));


  //set variable range to +-3 sigmas to ease splot fitting
  //   mean1_mass_ggh->setRange(mean1_mass_ggh->getVal()-2*mean1_mass_ggh->getError(),mean1_mass_ggh->getVal()+2*mean1_mass_ggh->getError());
  //   sigma1_mass_ggh->setRange(sigma1_mass_ggh->getVal()-2*sigma1_mass_ggh->getError(),sigma1_mass_ggh->getVal()+2*sigma1_mass_ggh->getError());
  //   sigma2_mass_ggh->setRange(sigma2_mass_ggh->getVal()-2*sigma2_mass_ggh->getError(),sigma2_mass_ggh->getVal()+2*sigma2_mass_ggh->getError());
  //   mean2_mass_ggh->setRange(mean2_mass_ggh->getVal()-2*mean2_mass_ggh->getError(),mean2_mass_ggh->getVal()+2*mean2_mass_ggh->getError());
  //   compo_mass->setRange(compo_mass->getVal()-2*compo_mass->getError(),compo_mass->getVal()+2*compo_mass->getError());

  compo_mass->setConstant(1);
  mean1_mass_ggh->setConstant(1);
  sigma1_mass_ggh->setConstant(1);
  mean2_mass_ggh->setConstant(1);
  sigma2_mass_ggh->setConstant(1);
  
  //Check plot
  frame_mass=dipho_mass->frame(110,140,15);
  sim_gen->plotOn(frame_mass);
  model_mass_ggh->plotOn(frame_mass);
  frame_mass->Draw();
  canvas->SaveAs("frame_massggh_reco.pdf");
  frame_mass->Delete();



  RooRealVar *coef0_pol_pt_ggh=new RooRealVar("coef0_pol_pt_ggh","coef0_pol_pt_ggh",10,-1,100);
  RooRealVar *coef1_pol_pt_ggh=new RooRealVar("coef1_pol_pt_ggh","coef1_pol_pt_ggh",7,-1,100);
  RooRealVar *coef2_pol_pt_ggh=new RooRealVar("coef2_pol_pt_ggh","coef2_pol_pt_ggh",7,-1,100);
  RooBernstein *pol_pt_ggh=new RooBernstein("pol_pt_ggh","pol_pt_ggh",*dipho_pt,RooArgSet(*coef0_pol_pt_ggh, *coef1_pol_pt_ggh, *coef2_pol_pt_ggh));
  RooRealVar *coefM_tsallis_pt_ggh=new RooRealVar("coefM_tsallis_pt_ggh","coefM_tsallis_pt_ggh",1,0,300);
  RooRealVar *coefT_tsallis_pt_ggh=new RooRealVar("coefT_tsallis_pt_ggh","coefT_tsallis_pt_ggh",5,1,50);
  RooRealVar *coefN_tsallis_pt_ggh=new RooRealVar("coefN_tsallis_pt_ggh","coefN_tsallis_pt_ggh",3,2,30);
  RooGenericPdf *tsallis_pt_ggh=new RooGenericPdf("tsallis_pt_ggh","(coefN_tsallis_pt_ggh-1)*(coefN_tsallis_pt_ggh-2)/coefN_tsallis_pt_ggh/coefT_tsallis_pt_ggh/(coefN_tsallis_pt_ggh*coefT_tsallis_pt_ggh+(coefN_tsallis_pt_ggh-2)*coefM_tsallis_pt_ggh)*dipho_pt*pow(1+(sqrt(coefM_tsallis_pt_ggh*coefM_tsallis_pt_ggh+dipho_pt*dipho_pt)-coefM_tsallis_pt_ggh)/coefN_tsallis_pt_ggh/coefT_tsallis_pt_ggh,-coefN_tsallis_pt_ggh)",RooArgSet(*dipho_pt,*coefN_tsallis_pt_ggh,*coefM_tsallis_pt_ggh,*coefT_tsallis_pt_ggh));
  
  RooProdPdf *model_pt_ggh=new RooProdPdf("model_pt_ggh","model_pt_ggh",RooArgList(*pol_pt_ggh,*tsallis_pt_ggh));
  model_pt_ggh->fitTo(*sim_gen);
  
//     coef0_pol_pt_ggh->setRange(coef0_pol_pt_ggh->getVal()-2*coef0_pol_pt_ggh->getError(),coef0_pol_pt_ggh->getVal()+2*coef0_pol_pt_ggh->getError());
//     coef1_pol_pt_ggh->setRange(coef1_pol_pt_ggh->getVal()-2*coef1_pol_pt_ggh->getError(),coef1_pol_pt_ggh->getVal()+2*coef1_pol_pt_ggh->getError());
//     coef2_pol_pt_ggh->setRange(coef2_pol_pt_ggh->getVal()-2*coef2_pol_pt_ggh->getError(),coef2_pol_pt_ggh->getVal()+2*coef2_pol_pt_ggh->getError());
//     coefM_tsallis_pt_ggh->setRange(coefM_tsallis_pt_ggh->getVal()-2*coefM_tsallis_pt_ggh->getError(),coefM_tsallis_pt_ggh->getVal()+2*coefM_tsallis_pt_ggh->getError());
//     coefN_tsallis_pt_ggh->setRange(coefN_tsallis_pt_ggh->getVal()-2*coefN_tsallis_pt_ggh->getError(),coefN_tsallis_pt_ggh->getVal()+2*coefN_tsallis_pt_ggh->getError());
//     coefT_tsallis_pt_ggh->setRange(coefT_tsallis_pt_ggh->getVal()-2*coefT_tsallis_pt_ggh->getError(),coefT_tsallis_pt_ggh->getVal()+2*coefT_tsallis_pt_ggh->getError());

  coef0_pol_pt_ggh->setConstant(1);
  coef1_pol_pt_ggh->setConstant(1);
  coef2_pol_pt_ggh->setConstant(1);
  coefM_tsallis_pt_ggh->setConstant(1);
  coefN_tsallis_pt_ggh->setConstant(1);
  coefT_tsallis_pt_ggh->setConstant(1);

  //Check plot
//   frame_pt=dipho_pt->frame();
//   sim_gen->plotOn(frame_pt);
//   model_pt_ggh->plotOn(frame_pt);
//   frame_pt->Draw();
//   canvas->SaveAs("frame_ptggh_reco.pdf");
//   frame_pt->Delete();

  sim_gen->Delete();
  tree->Delete();
  tree=0;
  
  RooProdPdf *model_ggh=new RooProdPdf("model_ggh","model_ggh",RooArgList(*model_pt_ggh,*model_mass_ggh));
  
 

  //#################################
  cout << "#####   Model bkg" << endl;
  RooRealVar *coef0_bern_mass_bkg=new RooRealVar("coef0_bern_mass_bkg","coef0_bern_mass_bkg",10,0,100);
  RooRealVar *coef1_bern_mass_bkg=new RooRealVar("coef1_bern_mass_bkg","coef1_bern_mass_bkg",10,0,100);
  RooRealVar *coef2_bern_mass_bkg=new RooRealVar("coef2_bern_mass_bkg","coef2_bern_mass_bkg",10,0,100);
  RooRealVar *coef3_bern_mass_bkg=new RooRealVar("coef3_bern_mass_bkg","coef3_bern_mass_bkg",10,0,100);
  RooBernstein *model_mass_bkg=new RooBernstein("model_mass_bkg","model_mass_bkg",*dipho_mass,RooArgSet(*coef0_bern_mass_bkg,*coef1_bern_mass_bkg,*coef2_bern_mass_bkg,*coef3_bern_mass_bkg));
  
  tree=(TTree*) file_kin->Get("tree_reco_bkg");
  sprintf(buffer,""); sprintf(buffer2,"");
  if (cut>0) {
    sprintf(buffer, "dipho_ctheta > %1.3f",cut/1000.);   
    sprintf(buffer2," && ");
  }
  if (categ>-1) {
    sprintf(buffer,"%s%s category > %2.2f && category < %2.2f",buffer,buffer2,categ-0.1,categ+0.1);
    sprintf(buffer2," && ");
  }
  
  sim_gen=new RooDataSet("sim_gen","sim_gen",tree , RooArgSet(*dipho_pt,*dipho_mass,*dipho_ctheta,*weight,*category),buffer,"weight");
  sprintf(buffer,"%s%s dipho_mass >123.5 && dipho_mass<126.5",buffer,buffer2);
RooDataSet*  sim_gendum=new RooDataSet("sim_gendum","sim_gendum",tree , RooArgSet(*dipho_pt,*dipho_mass,*dipho_ctheta,*weight,*category),buffer,"weight");
  sim_dum->append(*sim_gendum);
  model_mass_bkg->fitTo(*sim_gen);


//   coef0_bern_mass_bkg->setRange(coef0_bern_mass_bkg->getVal()- 2*coef0_bern_mass_bkg->getError(),coef0_bern_mass_bkg->getVal()+ 2*coef0_bern_mass_bkg->getError());
//   coef1_bern_mass_bkg->setRange(coef1_bern_mass_bkg->getVal()- 2*coef1_bern_mass_bkg->getError(),coef1_bern_mass_bkg->getVal()+ 2*coef1_bern_mass_bkg->getError());
//   coef2_bern_mass_bkg->setRange(coef2_bern_mass_bkg->getVal()- 2*coef2_bern_mass_bkg->getError(),coef2_bern_mass_bkg->getVal()+ 2*coef2_bern_mass_bkg->getError());
//   coef3_bern_mass_bkg->setRange(coef3_bern_mass_bkg->getVal()- 2*coef3_bern_mass_bkg->getError(),coef3_bern_mass_bkg->getVal()+ 2*coef3_bern_mass_bkg->getError());

  coef0_bern_mass_bkg->setConstant(1);
  coef1_bern_mass_bkg->setConstant(1);
  coef2_bern_mass_bkg->setConstant(1);
  coef3_bern_mass_bkg->setConstant(1);

  //Check plot
  frame_mass=dipho_mass->frame(110,180,35);
  sim_gen->plotOn(frame_mass);
  model_mass_bkg->plotOn(frame_mass);
  frame_mass->Draw();
  canvas->SaveAs("frame_massbkg_reco.pdf");
  frame_mass->Delete();



  RooRealVar *coef0_pol_pt_bkg=new RooRealVar("coef0_pol_pt_bkg","coef0_pol_pt_bkg",14,1,100);
  RooRealVar *coef1_pol_pt_bkg=new RooRealVar("coef1_pol_pt_bkg","coef1_pol_pt_bkg",9,1,100);
  RooRealVar *coef2_pol_pt_bkg=new RooRealVar("coef2_pol_pt_bkg","coef2_pol_pt_bkg",34,1,100);
  RooBernstein *pol_pt_bkg=new RooBernstein("pol_pt_bkg","pol_pt_bkg",*dipho_pt,RooArgSet(*coef0_pol_pt_bkg,*coef1_pol_pt_bkg,*coef2_pol_pt_bkg));
  RooRealVar *coefM_tsallis_pt_bkg=new RooRealVar("coefM_tsallis_pt_bkg","coefM_tsallis_pt_bkg",1,0,10);
  RooRealVar *coefT_tsallis_pt_bkg=new RooRealVar("coefT_tsallis_pt_bkg","coefT_tsallis_pt_bkg",6,1,15);
  RooRealVar *coefN_tsallis_pt_bkg=new RooRealVar("coefN_tsallis_pt_bkg","coefN_tsallis_pt_bkg",3,2,15);
  RooGenericPdf *tsallis_pt_bkg=new RooGenericPdf("tsallis_pt_bkg","(coefN_tsallis_pt_bkg-1)*(coefN_tsallis_pt_bkg-2)/coefN_tsallis_pt_bkg/coefT_tsallis_pt_bkg/(coefN_tsallis_pt_bkg*coefT_tsallis_pt_bkg+(coefN_tsallis_pt_bkg-2)*coefM_tsallis_pt_bkg)*dipho_pt*pow(1+(sqrt(coefM_tsallis_pt_bkg*coefM_tsallis_pt_bkg+dipho_pt*dipho_pt)-coefM_tsallis_pt_bkg)/coefN_tsallis_pt_bkg/coefT_tsallis_pt_bkg,-coefN_tsallis_pt_bkg)",RooArgSet(*dipho_pt,*coefN_tsallis_pt_bkg,*coefM_tsallis_pt_bkg,*coefT_tsallis_pt_bkg));
  
  RooProdPdf *model_pt_bkg=new RooProdPdf("model_pt_bkg","model_pt_bkg",RooArgSet(*tsallis_pt_bkg,*pol_pt_bkg));
  model_pt_bkg->fitTo(*sim_gen);
  

//     coef0_pol_pt_bkg->setRange(coef0_pol_pt_bkg->getVal()-2*coef0_pol_pt_bkg->getError(),coef0_pol_pt_bkg->getVal()+2*coef0_pol_pt_bkg->getError());
//     coef1_pol_pt_bkg->setRange(coef1_pol_pt_bkg->getVal()-2*coef1_pol_pt_bkg->getError(),coef1_pol_pt_bkg->getVal()+2*coef1_pol_pt_bkg->getError());
//     coef2_pol_pt_bkg->setRange(coef2_pol_pt_bkg->getVal()-2*coef2_pol_pt_bkg->getError(),coef2_pol_pt_bkg->getVal()+2*coef2_pol_pt_bkg->getError());
//     coefM_tsallis_pt_bkg->setRange(coefM_tsallis_pt_bkg->getVal()-2*coefM_tsallis_pt_bkg->getError(),coefM_tsallis_pt_bkg->getVal()+2*coefM_tsallis_pt_bkg->getError());
//     coefN_tsallis_pt_bkg->setRange(coefN_tsallis_pt_bkg->getVal()-2*coefN_tsallis_pt_bkg->getError(),coefN_tsallis_pt_bkg->getVal()+2*coefN_tsallis_pt_bkg->getError());
//      coefT_tsallis_pt_bkg->setRange(coefT_tsallis_pt_bkg->getVal()-2*coefT_tsallis_pt_bkg->getError(),coefT_tsallis_pt_bkg->getVal()+2*coefT_tsallis_pt_bkg->getError());

  coef0_pol_pt_bkg->setConstant(1);
  coef1_pol_pt_bkg->setConstant(1);
  coef2_pol_pt_bkg->setConstant(1);
  coefM_tsallis_pt_bkg->setConstant(1);
  coefN_tsallis_pt_bkg->setConstant(1);
  coefT_tsallis_pt_bkg->setConstant(1);


  //Check plot
  frame_pt=dipho_pt->frame();
  sim_gen->plotOn(frame_pt);
  model_pt_bkg->plotOn(frame_pt,Name("ModelPolFull"));
  tsallis_pt_bkg->plotOn(frame_pt,LineColor(kRed),LineStyle(kDashed),Name("ModelPolTsallis"));
  //  tsallis_pt_bkg->fitTo(*sim_gen);
  //tsallis_pt_bkg->plotOn(frame_pt,LineColor(kGreen),Name("ModelTsallisSeul"));
  frame_pt->Draw();
  TLegend legend(0.6,0.8,1,1);
  legend.AddEntry("ModelPolFull","Full Model with Polynom","l");
  legend.AddEntry("ModelPolTsallis","Tsallis Comp. with Polynom","l");
  //legend.AddEntry("ModelTsallisSeul","Tsallis Only Model","l");
  legend.Draw();
  frame_pt->SetTitle("");
  canvas->SaveAs("frame_ptbkg_reco.pdf");
  frame_pt->Delete();
  
  frame_pt=dipho_pt->frame();
  pol_pt_bkg->plotOn(frame_pt);
  frame_pt->Draw();
  canvas->SaveAs("frame_polptbkg_reco.pdf");
  frame_pt->Delete();

  sim_gen->Delete();
  sim_gen=new RooDataSet("sim_gen","sim_gen",tree , RooArgSet(*dipho_pt,*dipho_mass,*dipho_ctheta,*weight,*category),buffer,"weight");
  sim_dum->append(*sim_gen);


  tree->Delete();

  RooProdPdf *model_bkg=new RooProdPdf("model_bkg","model_bkg",RooArgSet(*model_mass_bkg,*model_pt_bkg));


  //Combine the models 
  RooRealVar *ggh_yield=new RooRealVar("ggh_yield","ggh_yield",1,0,500);
  RooRealVar *bkg_yield=new RooRealVar("bkg_yield","bkg_yield",100,1,200000);


  //Direct Fit extraction
//   char buffer_path[100]="",buffer_file[2][3][10]={{""}},buffercut[10]="",buffercateg[30]="";
//   sprintf(buffer_file[1][0],"png");
//   sprintf(buffer_file[1][1],"pdf");
//   sprintf(buffer_file[1][2],"root");
//   if (! BATCH) {
//     sprintf(buffer_path,"/afs/cern.ch/work/c/cgoudet/private/plot/");
//     sprintf(buffer_file[0][0],"png/");
//     sprintf(buffer_file[0][1],"pdf/");
//     sprintf(buffer_file[0][2],"root/");
//   }
//   if (cut) sprintf(buffercut,"%d",cut);
//   if (categ>-1) sprintf(buffercateg,"_categ%d",categ);

//   RooAddPdf *model_fittot=new RooAddPdf("model_fittot","model_fittot",RooArgList(*model_pt_ggh,*model_pt_bkg),RooArgList(*ggh_yield,*bkg_yield));  
//   model_fittot->fitTo(*sim_dum);
//   RooPlot *frame_fit=dipho_pt->frame();
//   sim_dum->plotOn(frame_fit,Name("data"),MarkerColor(1),LineColor(1));
//   model_fittot->plotOn(frame_fit,Name("model"));
//   model_fittot->plotOn(frame_fit,LineColor(kRed),LineStyle(kDashed),Components("model_pt_bkg"),Name("bkg"));
//   model_fittot->plotOn(frame_fit,LineColor(kGreen),LineStyle(kDashed),Components("model_pt_ggh"),Name("ggh"));
//   frame_fit->Draw(); 
//   TLegend *legendfit=new TLegend(0.6,0.7,1,1 );
//   legendfit->SetTextSize(0.04);
//   legendfit->AddEntry("","Reconstruction level","");
//   legendfit->AddEntry("data","MC events","lpe");
//   legendfit->AddEntry("model","Full Model Fit","l");
//   legendfit->AddEntry("ggh","ggH component","l");
//   legendfit->AddEntry("bkg","bkg component","l");
//   legendfit->Draw();
 
//   TLatex latex;
//   latex.SetNDC(1);
//   latex.DrawLatex(0.3,0.88,"ggh yield");
//   sprintf(buffer,"fitted : %3.0f",ggh_yield->getVal());
//   latex.DrawLatex(0.3,0.83,buffer);
//   sprintf(buffer,"expected : %3.0f",exp_ggh_yield->getVal());
//   latex.DrawLatex(0.3,0.78,buffer);
//   for (int file=0; file<3; file++ ) {
//     sprintf(buffer,"%s%sfitextraction_ggh%s%s_reco.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
//     canvas->SaveAs(buffer);
//   }
//   model_fittot->Delete();

  RooAddPdf *model_gghbkg= new RooAddPdf("model_gghbkg","model_gghbkg",RooArgList(*model_ggh,*model_bkg),RooArgList(*ggh_yield,*bkg_yield));  
  // Import all pdf and variables (implicitely) into workspace
  ws->import(*weight);
  ws->import(*category);
  ws->import(*dipho_ctheta);
  ws->import(*model_gghbkg);

  ws->import(*exp_ggh_yield);
  file_kin->Close();
  cout << "end AddModel" <<  endl;
  return 0;
}

//######################################################################################################
//######################################################################################################
//######################################################################################################
int AddData(RooWorkspace* ws, int const &cut=0, int const &categ=0) {

  RooRealVar *dipho_mass=ws->var("dipho_mass");
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *dipho_ctheta=ws->var("dipho_ctheta");
  RooRealVar *weight=ws->var("weight");
  RooRealVar *category=ws->var("category");
  char buffer[100],buffer2[100];
  TFile *file_kin=0;
  if (BATCH) file_kin=new TFile("kin_dist.root");
  else file_kin=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");

  //Create the cut formula and gather data
  sprintf(buffer,""); sprintf(buffer2,"");
  if (cut>0) {
    sprintf(buffer,"dipho_ctheta>%d/1000.",cut);
    sprintf(buffer2," && ");
  }
  if (categ>-1) sprintf(buffer,"%s%s category > %2.2f && category < %2.2f",buffer,buffer2,categ-0.1,categ+0.1);
  TTree *tree =(TTree *) file_kin->Get("tree_reco_bkg");
  RooDataSet *sim_gghbkg=new RooDataSet("sim_gghbkg","sim_gghbkg",tree,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*weight,*category),buffer,"weight");
  tree->Delete();
  RooRealVar *exp_bkg_yield=new RooRealVar("exp_bkg_yield","exp_bkg_yield",sim_gghbkg->sumEntries());

  tree=(TTree*) file_kin->Get("tree_reco_ggh");
  RooDataSet *sim=new RooDataSet("gghbkg","gghbkg",tree,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*weight,*category),buffer, "weight"); //Put cut tree into the dataset with weight
  cout << "sim entries " << sim->sumEntries() << endl;
  sim_gghbkg->append(*sim);  
  tree->Delete();

  

  
  //Check Plot
//   TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
//   RooPlot *framemass=dipho_mass->frame();
//   sim_gghbkg->plotOn(framemass, Binning(80,100,180));
//   framemass->Draw();
//   canvas->SaveAs("frame_mass_reco.pdf");
//   RooPlot *framept=dipho_pt->frame();
//   sim_gghbkg->plotOn(framept);
//   framept->Draw();
//   canvas->SaveAs("frame_pt_reco.pdf");  
    
  //import dataset into workspace
  ws->import(*sim_gghbkg);
  ws->import(*exp_bkg_yield);


  file_kin->Close();
  cout << "end adddata" << endl;
  return 0;}
    


//###################################################################################################
//###################################################################################################
//###################################################################################################
int DoSPlot(RooWorkspace* ws, int const &cut=0, int const &categ=-1) {
  cout << "in DoSplot" << endl;
  setTDRStyle();
  RooAbsPdf *model_gghbkg=ws->pdf("model_gghbkg");
  RooDataSet *sim_gghbkg=(RooDataSet*) ws->data("sim_gghbkg");
  RooRealVar *dipho_mass=ws->var("dipho_mass");  
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *exp_ggh_yield=ws->var("exp_ggh_yield");
  RooRealVar *weight=ws->var("weight");

  //Fixing discriminant parameters
//   RooRealVar *compo_mass=ws->var("compo_mass");
//   compo_mass->setConstant(1);
//   RooRealVar* mean1_mass_ggh=ws->var("mean1_mass_ggh");
//   mean1_mass_ggh->setConstant(1);
//   RooRealVar* sigma1_mass_ggh=ws->var("sigma1_mass_ggh");
//   sigma2_mass_ggh->setConstant(1);
//   RooRealVar* mean2_mass_ggh=ws->var("mean2_mass_ggh");
//   mean2_mass_ggh->setConstant(1);
//   RooRealVar* sigma2_mass_ggh=ws->var("sigma2_mass_ggh");
//   sigma1_mass_ggh->setConstant(1);
//   RooRealVar *coef0_pol_pt_ggh=ws->var("coef0_pol_pt_ggh");
//   coef0_pol_pt_ggh->setConstant(1);
//   RooRealVar *coef1_pol_pt_ggh=ws->var("coef1_pol_pt_ggh");
//   coef1_pol_pt_ggh->setConstant(1);
//   RooRealVar *coef2_pol_pt_ggh=ws->var("coef2_pol_pt_ggh");
//   coef2_pol_pt_ggh->setConstant(1);
//   RooRealVar *coefM_tsallis_pt_ggh=ws->var("coefM_tsallis_pt_ggh");
//   coefM_tsallis_pt_ggh->setConstant(1);
//   RooRealVar *coefN_tsallis_pt_ggh=ws->var("coefN_tsallis_pt_ggh");
//   coefN_tsallis_pt_ggh->setConstant(1);
//   RooRealVar *coefT_tsallis_pt_ggh=ws->var("coefT_tsallis_pt_ggh");
//   coefT_tsallis_pt_ggh->setConstant(1);
//   cout << "set constant" << endl;
//   RooRealVar *coef0_bern_mass_bkg=ws->var("coef0_bern_mass_bkg");
//   coef0_bern_mass_bkg->setConstant(1);
//   RooRealVar *coef1_bern_mass_bkg=ws->var("coef1_bern_mass_bkg");
//   coef1_bern_mass_bkg->setConstant(1);
//   RooRealVar *coef2_bern_mass_bkg=ws->var("coef2_bern_mass_bkg");
//   coef2_bern_mass_bkg->setConstant(1);
//   RooRealVar *coef3_bern_mass_bkg=ws->var("coef3_bern_mass_bkg");
//   coef3_bern_mass_bkg->setConstant(1);
//   RooRealVar *coef0_pol_pt_bkg=ws->var("coef0_pol_pt_bkg");
//   coef0_pol_pt_bkg->setConstant(1);
//   RooRealVar *coef1_pol_pt_bkg=ws->var("coef1_pol_pt_bkg");
//   coef1_pol_pt_bkg->setConstant(1);
//   RooRealVar *coef2_pol_pt_bkg=ws->var("coef2_pol_pt_bkg");
//   coef2_pol_pt_bkg->setConstant(1);
//   RooRealVar *coefM_tsallis_pt_bkg=ws->var("coefM_tsallis_pt_bkg");
//   coefM_tsallis_pt_bkg->setConstant(1);
//   RooRealVar *coefN_tsallis_pt_bkg=ws->var("coefN_tsallis_pt_bkg");
//   coefN_tsallis_pt_bkg->setConstant(1);
//   RooRealVar *coefT_tsallis_pt_bkg=ws->var("coefT_tsallis_pt_bkg");
//   coefT_tsallis_pt_bkg->setConstant(1);


  RooRealVar *ggh_yield=ws->var("ggh_yield");
  RooRealVar *bkg_yield=ws->var("bkg_yield");

  model_gghbkg->fitTo(*sim_gghbkg);  
 
  // Check plot
  char buffer_path[100]="",buffer_file[2][3][10]={{""}},buffercut[10]="",buffercateg[30]="";
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
  if (categ>-1) sprintf(buffercateg,"_categ%d",categ);


  TLatex latex; latex.SetNDC(1); char buffer[100];
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  TLegend *legend=new TLegend(0.7,0.7,1,1);
  legend->SetTextSize(0.04);
  RooPlot* frame_mass=dipho_mass->frame(110,180,35); frame_mass->UseCurrentStyle();
  frame_mass->GetYaxis()->SetLabelSize(0.03);
  sim_gghbkg->plotOn(frame_mass,Name("data"));
  model_gghbkg->plotOn(frame_mass,Name("full"));
  model_gghbkg->plotOn(frame_mass, Components("model_ggh"), LineColor(kGreen), LineStyle(kDashed),Name("ggh"));
  model_gghbkg->plotOn(frame_mass, Components("model_bkg"), LineColor(kRed), LineStyle(kDashed),Name("bkg"));
  frame_mass->Draw();
  latex.DrawLatex(0.45,0.86,"ggh yield");
  sprintf(buffer,"fitted = %3.0f",ggh_yield->getVal()); 
  latex.DrawLatex(0.45,0.81,buffer);
  sprintf(buffer,"expected = %3.0f",exp_ggh_yield->getVal());
  latex.DrawLatex(0.45,0.76,buffer);
  legend->AddEntry(frame_mass->findObject("data"),"MC reco","lpe");
  legend->AddEntry(frame_mass->findObject("full"),"Full Fit","l");
  legend->AddEntry(frame_mass->findObject("ggh"),"ggh Fit","l");
  legend->AddEntry(frame_mass->findObject("bkg"),"bkg Fit","l");
  legend->Draw();

  if (cut) {
    sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
    canvas->cd();
    latex.DrawLatex(0.05,0.96,buffer);
  }
  if(categ>-1) {
    sprintf(buffer,"category %d",categ);
    canvas->cd();
    latex.DrawLatex(0.4,0.96,buffer);
  }
  for (int file=0; file<3; file++ ) {
    sprintf(buffer,"%s%sSPlotInput_pt%s%s_recoggh.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
    canvas->SaveAs(buffer);
  }


  RooPlot* frame_pt=dipho_pt->frame();
  frame_pt=dipho_pt->frame(NBINS);
  sim_gghbkg->plotOn(frame_pt);
  model_gghbkg->plotOn(frame_pt);
  model_gghbkg->plotOn(frame_pt, Components("model_ggh"), LineColor(kGreen), LineStyle(kDashed));
  model_gghbkg->plotOn(frame_pt, Components("model_bkg"), LineColor(kRed), LineStyle(kDashed));
  frame_pt->Draw();
  canvas->SaveAs("frameDoSPlotPt_reco.pdf");
  frame_pt->Delete();



  RooDataSet *simW_gghbkg=new RooDataSet(*sim_gghbkg,"simW_gghbkg");
  SPlot *splot_gghbkg=new SPlot("splot_gghbkg","splot_gghbkg", *simW_gghbkg, model_gghbkg, RooArgList(*ggh_yield, *bkg_yield),RooArgSet(*dipho_pt));  //Create splot
  ws->import(*simW_gghbkg,Rename("simW_gghbkg"));



  canvas->Close();
  cout << "end DoSPlot" << endl;
  return 0;
}
//#############################################################################################"
//#############################################################################################"
//#############################################################################################"
int MakePlot(RooWorkspace* ws, int const &cut=0, int const &categ=0) {

  setTDRStyle();
  // collect usefull variables
  RooAbsPdf *model=ws->pdf("model_gghbkg");
  RooDataSet *simu= (RooDataSet*) ws->data("sim_gghbkg");
  RooRealVar *dipho_mass=ws->var("dipho_mass");
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *bkg_yield=ws->var("bkg_yield");
  RooRealVar *ggh_yield=ws->var("ggh_yield");
  RooRealVar *exp_ggh_yield=ws->var("exp_ggh_yield");
  RooDataSet *simW=(RooDataSet*) ws->data("simW_gghbkg");

  TLatex latex;
  latex.SetNDC();  
  char buffer[100];
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  TPad *pad_up=new TPad("pad_up","pad_up",0,0.3,1,1);
  TPad *pad_down=new TPad("pad_down","pad_down",0,0,1,0.3);
  pad_down->SetTopMargin(0);
  pad_down->SetBottomMargin(0.35);
  pad_up->SetTopMargin(0.07);
  pad_up->SetBottomMargin(0);
  TLegend *legend=new TLegend(0.8,0.7,1,1);

  //define path prefixes  for saving canvas
  char buffer_path[100]="",buffer_file[2][3][10]={{""}},buffercut[10]="",buffercateg[30]="";
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
  if (categ>-1) sprintf(buffercateg,"_categ%d",categ);

  // plot weighted events and first fitA
  RooPlot *frame_pt=dipho_pt->frame(NBINS);
  RooDataSet *sim_Wbkg=new RooDataSet("sim_Wbkg","sim_Wbkg",simW,*simW->get(),0,"bkg_yield_sw");
  RooDataSet *sim_Wggh=new RooDataSet("sim_Wggh","sim_Wggh",simW,*simW->get(),0,"ggh_yield_sw");
  sim_Wbkg->plotOn(frame_pt,MarkerColor(1),LineColor(1),DataError(RooAbsData::SumW2),Name("sim_Wbkg"),Binning(NBINS,0,dipho_pt->getMax()));// plot weighted ggh events
  model->plotOn(frame_pt,Components("model_bkg"),LineColor(4),Name("model_bkg"));// background component of the total fit
  sim_Wggh->plotOn(frame_pt,MarkerColor(2),LineColor(2),DataError(RooAbsData::SumW2),Name("sim_Wggh"),Binning(NBINS,0,dipho_pt->getMax()));// plot weighted ggh events
  model->plotOn(frame_pt,Components("model_ggh"),LineColor(3),Normalization(sim_Wbkg->sumEntries(),RooAbsReal::NumEvent),Name("model_ggh"));//ggh component of total fit 

  canvas->cd();
  frame_pt->UseCurrentStyle();
  frame_pt->Draw();
  legend=new TLegend(0.6,0.7,1,1);
  legend->SetTextSize(0.04);
  legend->AddEntry("","Reconstruction Level","");
  legend->AddEntry("sim_Wggh", "ggh Weighted Events","lpe");
  legend->AddEntry("sim_Wbkg", "bkg Weighted Events","lpe");
  legend->AddEntry("model_ggh","Non Weighted ggh Fit","l");
  legend->AddEntry("model_bkg","Non Weighted Bkg Fit","l");
  legend->Draw();


  if (cut) {
    sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
    canvas->cd();
    latex.DrawLatex(0.05,0.96,buffer);
  }
  if (categ>-1) {
    sprintf(buffer,"category %d",categ);
    canvas->cd();
    latex.DrawLatex(0.4,0.96,buffer);
  }
  for (int file=0; file<3; file++ ) {
    sprintf(buffer,"%s%ssplot_pt%s%s_recogghbkg.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
    canvas->SaveAs(buffer);
  }
  cout << "weighted drawn" << endl;
  frame_pt->Delete();
  canvas->Clear();
  canvas->SaveAs("canvas.pdf");
  cout << "canvas _cleared" << endl;


  // plot weighted signal events and first fit
  pad_up->Draw();  
  pad_up->cd();
  frame_pt=dipho_pt->frame(NBINS);
  sim_Wggh->plotOn(frame_pt,MarkerColor(2),LineColor(2),DataError(RooAbsData::SumW2),Name("sim_Wggh"),Binning(NBINS,0,dipho_pt->getMax()));// plot weighted ggh events
  model->plotOn(frame_pt,Components("model_ggh"),LineColor(3),Normalization(sim_Wbkg->sumEntries(),RooAbsReal::NumEvent),Name("model_ggh"));//ggh component of total fit 
  frame_pt->UseCurrentStyle();
  frame_pt->GetXaxis()->SetTitleSize(0);
  frame_pt->GetXaxis()->SetLabelSize(0);
  frame_pt->Draw();

  legend=new TLegend(0.6,0.75,1,1);
  legend->SetTextSize(0.05);
  legend->AddEntry("","Reconstruction Level","");
  legend->AddEntry("sim_Wggh", "ggH Weighted Events","lpe");
  legend->AddEntry("model_ggh","Non Weighted ggH Fit","l");
  legend->Draw();

  TH1F *hist_fit=(TH1F*) model->createHistogram("hist_fit",*dipho_pt,Binning(NBINS,0,NBINS*WIDTH),ConditionalObservables(*dipho_mass));
  hist_fit->Scale(sim_Wggh->sumEntries());
  TH1F *hist_set=new TH1F("hist_set","hist_set",NBINS,0,NBINS*WIDTH);
  sim_Wggh->fillHistogram(hist_set,*dipho_pt);  
  hist_set->SetTitle("test");
  TLine *line=new TLine(0,0,NBINS*WIDTH,0);
  line->SetLineColor(kRed);
  hist_set->Add(hist_fit,-1);
  hist_set->Divide(hist_fit);  
  hist_set->GetYaxis()->SetTitle("#frac{N_{MC}-N_{fit}}{N_{fit}}");
  hist_set->GetYaxis()->SetTitleOffset(0.3);
  hist_set->GetXaxis()->SetTitle(frame_pt->GetXaxis()->GetTitle());
  //  hist_set->GetYaxis()->SetRangeUser(-1,1);
  cout << "divided" << endl;
  hist_set->GetXaxis()->SetLabelSize(0.15);
  hist_set->GetYaxis()->SetLabelSize(0.1);
  hist_set->GetXaxis()->SetTitleSize(0.15);
  hist_set->GetXaxis()->SetTitleOffset(1);
  hist_set->GetYaxis()->SetTitleSize(0.15);
  
  canvas->cd();
  pad_down->Draw();
  pad_down->cd();
  hist_set->Draw();
  line->Draw("SAME");

  if (cut) {
    sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
    canvas->cd();
    latex.DrawLatex(0.05,0.96,buffer);
  }
  if(categ>-1) {
    sprintf(buffer,"category %d",categ);
    canvas->cd();
    latex.DrawLatex(0.4,0.96,buffer);
  }
  for (int file=0; file<3; file++ ) {
    sprintf(buffer,"%s%ssplot_pt%s%s_recoggh.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
    canvas->SaveAs(buffer);
  }
  cout << "weigthed ggh drawn" << endl;

  cout << "end MakePlot" << endl;
  return 0;}
    
