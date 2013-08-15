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
#include "TH1F.h"
#include "THStack.h"

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

#define BATCH 0 // On batch mode, have to change loading and saving path
#define NBINS 40

using namespace std;
using namespace RooStats;
using namespace RooFit;

int main() {
  //#############menu
  int const menu_cut[5]={0,200,375,550,750};

  //############def
  int AddModel(RooWorkspace*, int const &cut=0, int const &categ=0); // Add pdf to workspace and pre-fit them
  int DoSPlot(RooWorkspace*); // Create SPlot object
  int MakePlot(RooWorkspace*, int const &cut=0, int const &categ=0); // Create and save result and check plots

  TFile *root_file=0;
  if (BATCH) root_file=new TFile("WS_SPlot.root","UPDATE"); //File to store the workspace
  else root_file=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/WS_SPlot.root","UPDATE");
  RooWorkspace *ws=0;
  char buffer[100];

   int i=0;
  int categ=-1;
//   for (int i=0; i<5;i++) {
//     for (int categ=-1; categ<4;categ++) {
      sprintf(buffer,"ws_hgg_splot_reco");
      if (i) sprintf(buffer,"%s%d",buffer,menu_cut[i]);
      if (categ>-1) sprintf(buffer,"%scateg%d",buffer,categ);
      ws=new RooWorkspace(buffer,buffer);
      AddModel(ws,menu_cut[i],categ);
       DoSPlot(ws);
//       MakePlot(ws,menu_cut[i],categ);
      root_file->cd();
      ws->Write("",TObject::kOverwrite);
      ws->Delete();  
//     } 
//   }

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
  
  
  RooRealVar *dipho_mass=new RooRealVar("dipho_mass","m_{#gamma #gamma}",100,180,"GeV/c^{2}");
  RooRealVar *dipho_pt=new RooRealVar("dipho_pt","p_{T#gamma#gamma}",100,0,200,"GeV/c");
  RooRealVar *dipho_ctheta=new RooRealVar("dipho_ctheta","cos(#theta *)",0.5,0,1);
  RooRealVar *weight=new RooRealVar("weight","weight",0,10);
  RooRealVar *category=new RooRealVar("category","category",-1,5);
  RooPlot *frame_mass=0,*frame_pt=0;
  TTree *tree=0;
  RooDataSet *sim_gen=0;
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  char buffer[100],buffer2[100];  
  RooDataSet *simu=new RooDataSet("simu","simu",RooArgSet(*dipho_pt,*dipho_mass,*dipho_ctheta,*category,*weight),"weight");
  TLatex latex;
  latex.SetNDC();

  cout << "#####  Model ggH" << endl;
  //create model of background and fit : sum of 2 gaussian
  RooRealVar *mean1_mass_ggh=new RooRealVar("mean1_mass_ggh","mean1_mass_ggh",123,120,130);
  RooRealVar *sigma1_mass_ggh=new RooRealVar("sigma1_mass_ggh","sigma1_mass_ggh",1,0,5);
  RooGaussian *model1_mass_ggh=new RooGaussian("model1_mass_ggh","model1_mass_ggh",*dipho_mass,*mean1_mass_ggh,*sigma1_mass_ggh);
  RooRealVar *mean2_mass_ggh=new RooRealVar("mean2_mass_ggh","mean2_mass_ggh",126,120,130);
  RooRealVar *sigma2_mass_ggh=new RooRealVar("sigma2_mass_ggh","sigma2_mass_ggh",2,0,5);
  RooGaussian *model2_mass_ggh=new RooGaussian("model2_mass_ggh","model2_mass_ggh",*dipho_mass,*mean2_mass_ggh,*sigma2_mass_ggh);
  RooRealVar *compo_mass=new RooRealVar("compo_mass","compo_mass",0,1);
  RooAddPdf *model_mass_ggh=new RooAddPdf("model_mass_ggh","model_mass_ggh",RooArgSet(*model1_mass_ggh,*model2_mass_ggh),*compo_mass);
  
  tree=(TTree*) file_kin->Get("tree_reco_ggh");
  //select data according cuts on theta
  sprintf(buffer,""); sprintf(buffer2,"");
  if (cut>0) {
    sprintf(buffer, "dipho_ctheta > %1.3f",cut/1000.);   
    sprintf(buffer2," && ");
  }
  if (categ>-1) sprintf(buffer,"%s%s category > %2.2f && category < %2.2f",buffer,buffer2,categ-0.1,categ+0.1);
  sim_gen=new RooDataSet("sim_gen","sim_gen",tree , RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category,*weight), buffer,"weight");
  float dummy=sim_gen->sumEntries();
  model_mass_ggh->fitTo(*sim_gen,Range(110,140));
  simu->append(*sim_gen);

//   //set variable range to +-3 sigmas to ease splot fitting
//   mean1_mass_ggh->setRange(mean1_mass_ggh->getVal()-2*mean1_mass_ggh->getError(),mean1_mass_ggh->getVal()+2*mean1_mass_ggh->getError());
//   sigma1_mass_ggh->setRange(sigma1_mass_ggh->getVal()-2*sigma1_mass_ggh->getError(),sigma1_mass_ggh->getVal()+2*sigma1_mass_ggh->getError());
//   sigma2_mass_ggh->setRange(sigma2_mass_ggh->getVal()-2*sigma2_mass_ggh->getError(),sigma2_mass_ggh->getVal()+2*sigma2_mass_ggh->getError());
//   mean2_mass_ggh->setRange(mean2_mass_ggh->getVal()-2*mean2_mass_ggh->getError(),mean2_mass_ggh->getVal()+2*mean2_mass_ggh->getError());
//   compo_mass->setRange(compo_mass->getVal()-2*compo_mass->getError(),compo_mass->getVal()+2*compo_mass->getError());
  //set variable range to +-3 sigmas to ease splot fitting
  mean1_mass_ggh->setConstant(1);
  sigma1_mass_ggh->setConstant(1);
  sigma2_mass_ggh->setConstant(1);
  mean2_mass_ggh->setConstant(1);
  compo_mass->setConstant(1);
  
  //Check plot
  frame_mass=dipho_mass->frame();
  sim_gen->plotOn(frame_mass,Binning(30,110,140));
  model_mass_ggh->plotOn(frame_mass);
  frame_mass->Draw();
  canvas->SaveAs("frame_massggh_reco.pdf");
  frame_mass->Delete();




  //Create pt distribution of ggh for comparison.
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
  
  // coef0_pol_pt_ggh->setRange(coef0_pol_pt_ggh->getVal()-2*coef0_pol_pt_ggh->getError(),coef0_pol_pt_ggh->getVal()+2*coef0_pol_pt_ggh->getError());
  // coef1_pol_pt_ggh->setRange(coef1_pol_pt_ggh->getVal()-2*coef1_pol_pt_ggh->getError(),coef1_pol_pt_ggh->getVal()+2*coef1_pol_pt_ggh->getError());
  // coef2_pol_pt_ggh->setRange(coef2_pol_pt_ggh->getVal()-2*coef2_pol_pt_ggh->getError(),coef2_pol_pt_ggh->getVal()+2*coef2_pol_pt_ggh->getError());
  // coefM_tsallis_pt_ggh->setRange(coefM_tsallis_pt_ggh->getVal()-2*coefM_tsallis_pt_ggh->getError(),coefM_tsallis_pt_ggh->getVal()+2*coefM_tsallis_pt_ggh->getError());
  // coefN_tsallis_pt_ggh->setRange(coefN_tsallis_pt_ggh->getVal()-2*coefN_tsallis_pt_ggh->getError(),coefN_tsallis_pt_ggh->getVal()+2*coefN_tsallis_pt_ggh->getError());
  // coefT_tsallis_pt_ggh->setRange(coefT_tsallis_pt_ggh->getVal()-2*coefT_tsallis_pt_ggh->getError(),coefT_tsallis_pt_ggh->getVal()+2*coefT_tsallis_pt_ggh->getError());

  coef0_pol_pt_ggh->setConstant(1);
  coef1_pol_pt_ggh->setConstant(1);
  coef2_pol_pt_ggh->setConstant(1);
  coefM_tsallis_pt_ggh->setConstant(1);
  coefN_tsallis_pt_ggh->setConstant(1);
  coefT_tsallis_pt_ggh->setConstant(1);
  

  //Check plot
  frame_pt=dipho_pt->frame();
  sim_gen->plotOn(frame_pt);
  model_pt_ggh->plotOn(frame_pt);
  frame_pt->Draw();
  canvas->SaveAs("frame_ptggh_reco.pdf");
  frame_pt->Delete();


  sim_gen->Delete();
  tree->Delete();
  tree=0;
  

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
  if (categ>-1) sprintf(buffer,"%s%s category > %2.2f && category < %2.2f",buffer,buffer2,categ-0.1,categ+0.1);
  cout << buffer << endl;
  cout << "tree entries  " <<   tree->GetEntries() << endl;
  cout << "coupures  " << buffer << endl;
  sim_gen=new RooDataSet("sim_gen","sim_gen",tree , RooArgSet(*dipho_pt,*dipho_mass,*dipho_ctheta,*weight,*category),buffer,"weight");
  cout << "bkg entries   " << sim_gen->numEntries() << endl;
  cout << "bkg weight  " << sim_gen->sumEntries() << endl;

  model_mass_bkg->fitTo(*sim_gen);
  simu->append(*sim_gen);

//     coef0_bern_mass_bkg->setRange(coef0_bern_mass_bkg->getVal()- 2*coef0_bern_mass_bkg->getError(),coef0_bern_mass_bkg->getVal()+ 2*coef0_bern_mass_bkg->getError());
//     coef1_bern_mass_bkg->setRange(coef1_bern_mass_bkg->getVal()- 2*coef1_bern_mass_bkg->getError(),coef1_bern_mass_bkg->getVal()+ 2*coef1_bern_mass_bkg->getError());
//     coef2_bern_mass_bkg->setRange(coef2_bern_mass_bkg->getVal()- 2*coef2_bern_mass_bkg->getError(),coef2_bern_mass_bkg->getVal()+ 2*coef2_bern_mass_bkg->getError());
//     coef3_bern_mass_bkg->setRange(coef3_bern_mass_bkg->getVal()- 2*coef3_bern_mass_bkg->getError(),coef3_bern_mass_bkg->getVal()+ 2*coef3_bern_mass_bkg->getError());

    coef0_bern_mass_bkg->setConstant(1);
    coef1_bern_mass_bkg->setConstant(1);
    coef2_bern_mass_bkg->setConstant(1);
    coef3_bern_mass_bkg->setConstant(1);


  //Check plot
    frame_mass=dipho_mass->frame();
    sim_gen->plotOn(frame_mass,Binning(80,100,180));
    model_mass_bkg->plotOn(frame_mass);
    frame_mass->Draw();
    canvas->SaveAs("frame_massbkg_reco.pdf");
    frame_mass->Delete();






  sim_gen->Delete();
  tree->Delete();


  //Combine the models 
  RooRealVar *ggh_yield=new RooRealVar("ggh_yield","ggh_yield",500,2,1000);
  RooRealVar *bkg_yield=new RooRealVar("bkg_yield","bkg_yield",50000,100,200000);
  RooAddPdf *model_mass_gghbkg= new RooAddPdf("model_mass_gghbkg","model_mass_gghbkg",RooArgList(*model_mass_ggh,*model_mass_bkg),RooArgList(*ggh_yield,*bkg_yield));  
  model_mass_gghbkg->fitTo(*simu);
  //Check plot
  frame_mass=dipho_mass->frame();
  simu->plotOn(frame_mass,Binning(80,100,180),Name("data"));
  model_mass_gghbkg->plotOn(frame_mass,Name("full"));
  model_mass_gghbkg->plotOn(frame_mass,Components("model_mass_bkg"),LineColor(kRed),LineStyle(kDashed),Name("bkg"));
  model_mass_gghbkg->plotOn(frame_mass,Components("model_mass_ggh"),LineColor(kGreen),LineStyle(kDashed),Name("ggh"));
  frame_mass->Draw();
  sprintf(buffer,"ggh yields");
  latex.DrawLatex(0.4,0.88,buffer);
  sprintf(buffer,"fitted : %3.0f",ggh_yield->getVal());
  latex.DrawLatex(0.4,0.84,buffer);
  sprintf(buffer,"expected : %3.0f",dummy);
  latex.DrawLatex(0.4,0.80,buffer);
  canvas->SaveAs("splot_unweightedmass.pdf");
  frame_mass->Delete();
  







  // Import all pdf and variables (implicitely) into workspace
  ws->import(*category);
  ws->import(*dipho_ctheta);
  ws->import(*model_mass_gghbkg);
  ws->import(*model_pt_ggh);
  file_kin->Close();
  cout << "end AddModel" <<  endl;

  return 0;
}

//######################################################################################################
//######################################################################################################
//######################################################################################################
int DoSPlot(RooWorkspace* ws) {
  cout << "in DoSplot" << endl;
  setTDRStyle();

  RooAbsPdf *model_mass_gghbkg=ws->pdf("model_mass_gghbkg");
  RooDataSet *simu=(RooDataSet*) ws->data("simu");
  RooRealVar *dipho_mass=ws->var("dipho_mass");  
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *dipho_ctheta=ws->var("dipho_ctheta");
  RooRealVar *category=ws->var("category");
  RooRealVar *weight=ws->var("weight");
  TCanvas *canvas=new TCanvas();
  RooAbsPdf *model_mass_ggh=ws->pdf("model_mass_ggh");
  RooRealVar *ggh_yield=ws->var("ggh_yield");
  RooRealVar *bkg_yield=ws->var("bkg_yield");
  RooAbsPdf *model_pt_ggh=ws->pdf("model_pt_ggh");



  //Create histograms from the distribution models of mass
  TH1F *hist_ggh=(TH1F *)model_mass_ggh->createHistogram("hist_ggh",*dipho_mass,Binning(160,100,180));
  hist_ggh->Draw();
  canvas->SaveAs("fit_ggh.pdf");
  TH1F *hist_full=(TH1F *)model_mass_gghbkg->createHistogram("hist_full",*dipho_mass,Binning(160,100,180));
  hist_full->Draw();
  canvas->SaveAs("fit_full.pdf");
  TH1F *hist_pt_ggh=new TH1F("hist_pt_ggh","hist_pt_ggh",NBINS,0,200);
  hist_pt_ggh->SetMarkerColor(kBlue);
  hist_pt_ggh->SetLineColor(kBlue);
  hist_pt_ggh->Scale(ggh_yield->getVal());

  //Create Pt histogram
  TH1F *hist_pt=new TH1F("hist_pt","hist_pt",NBINS,0.,200.);
  hist_pt->Sumw2();
  TH1F *hist_pt_compoggh=new TH1F ("hist_pt_compoggh","hist_pt_compoggh",NBINS,0,200);
  hist_pt_compoggh->SetMarkerColor(kRed);
  hist_pt_compoggh->SetLineColor(kRed);
  hist_pt->Sumw2();

  //get trees and attribute branches to variable
  TFile *file_kin=0;
  if (BATCH) file_kin=new TFile("kin_dist.root");
  else file_kin=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");

  float pt,mass,w,ctheta,expected_ggh=0;
  int bin,categ;

  TTree *tree=(TTree *) file_kin->Get("tree_reco_ggh");
  int nentries=tree->GetEntries();
  tree->SetBranchAddress("dipho_pt",&pt);
  tree->SetBranchAddress("dipho_ctheta",&ctheta);
  tree->SetBranchAddress("dipho_mass",&mass);
  tree->SetBranchAddress("category",&categ);
  tree->SetBranchAddress("weight",&w);

  //Pass through all entries and give them the right weight and fill histogram
  for (int i=0; i<nentries; i++) {
    expected_ggh+=w;
    tree->GetEntry(i);
    bin=hist_ggh->FindBin(mass);
    hist_pt->Fill(pt,w*ggh_yield->getVal()*hist_ggh->GetBinContent(bin)/hist_full->GetBinContent(bin)/(ggh_yield->getVal()+bkg_yield->getVal()));
    hist_pt_compoggh->Fill(pt,w*ggh_yield->getVal()*hist_ggh->GetBinContent(bin)/hist_full->GetBinContent(bin)/(ggh_yield->getVal()+bkg_yield->getVal()));
    hist_pt_ggh->Fill(pt,w);
  }
  tree->Delete();

  tree=(TTree *) file_kin->Get("tree_reco_bkg");
  nentries=tree->GetEntries();
  tree->SetBranchAddress("dipho_pt",&pt);
  tree->SetBranchAddress("dipho_ctheta",&ctheta);
  tree->SetBranchAddress("dipho_mass",&mass);
  tree->SetBranchAddress("category",&categ);
  tree->SetBranchAddress("weight",&w);

  //Pass through all entries and give them the right weight and fill histogram
  for (int i=0; i<nentries; i++) {
    tree->GetEntry(i);
    bin=hist_ggh->FindBin(mass);
    hist_pt->Fill(pt,w*ggh_yield->getVal()*hist_ggh->GetBinContent(bin)/hist_full->GetBinContent(bin)/(ggh_yield->getVal()+bkg_yield->getVal()));
  }
  tree->Delete();




  // Check plot
  TLatex latex; latex.SetNDC(1); char buffer[100];
  THStack *stack=new THStack("stack","stack");
  TLegend legend(0.6,0.9,1,1);
  legend.SetFillColor(0);

  stack->Add(hist_pt);
  hist_pt_ggh->Scale(ggh_yield->getVal()/hist_pt_ggh->Integral());
  stack->Add(hist_pt_ggh);
  stack->Add(hist_pt_compoggh);

  legend.AddEntry(hist_pt_ggh,"MC normalized to fitted ggh yield","lpe");
  legend.AddEntry(hist_pt,"Extrapolated p_{T} Distribution","lpe");
  legend.AddEntry(hist_pt_compoggh,"Contribution of signal events","lpe");
  stack->Draw("Enostack");
  stack->GetHistogram()->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  stack->GetHistogram()->GetYaxis()->SetTitle("dN/dp_{T}");
  legend.Draw();
  latex.DrawLatex(0.6,0.8,"ggh yield");
  sprintf(buffer,"fitted : %3.0f",ggh_yield->getVal());
  latex.DrawLatex(0.6,0.75,buffer);
  sprintf(buffer,"expected : %3.0f",expected_ggh);
  latex.DrawLatex(0.6,0.7,buffer);
  canvas->SaveAs("pt_dist.pdf");

			 
  cout << "end DoSPlot" << endl;
  return 0;
}

//#############################################################################################"
//#############################################################################################"
//#############################################################################################"
int MakePlot(RooWorkspace* ws, int const &cut=0, int const &categ=0) {

  setTDRStyle();
  // collect usefull variables
  RooRealVar *dipho_mass=ws->var("dipho_mass");
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooAbsPdf *model=ws->pdf("model_gghbkg");
  RooDataSet *sim= (RooDataSet*) ws->data("sim_gghbkg");
  RooDataSet *simW=(RooDataSet*) ws->data("simW_gghbkg");
  RooRealVar *bkg_yield=ws->var("bkg_yield");
  RooRealVar *ggh_yield=ws->var("ggh_yield");
  RooRealVar *exp_ggh_yield=ws->var("exp_ggh_yield");

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

  //Check plots of the fit before splot

  RooPlot *frame_down=dipho_mass->frame();
  sim->plotOn(frame_down,Binning(40,100,180),MarkerColor(kBlack),Name("sim"));
  model->plotOn(frame_down,LineColor(kBlue),Name("model"));
  model->plotOn(frame_down,Components("model_ggh"),LineColor(kGreen),LineStyle(kDashed),Name("model_ggh"));
  model->plotOn(frame_down,Components("model_bkg"),LineColor(kRed),LineStyle(kDashed),Name("model_bkg"));


  canvas->cd();  
  frame_down->UseCurrentStyle();
  frame_down->Draw();
  
  legend->AddEntry(frame_down->findObject("sim"),"Inclusive Generated Events","lpe");
  legend->AddEntry(frame_down->findObject("model"),"Inclusive Fit","l");
  legend->AddEntry(frame_down->findObject("model_ggh"),"ggH Component","l");
  legend->AddEntry(frame_down->findObject("model_bkg"),"bkg Component","l");
  legend->Draw();

  sprintf(buffer,"signal yields");
  latex.DrawLatex(0.4,0.88,buffer);
  sprintf(buffer,"fitted : %1.2e",ggh_yield->getVal());
  latex.DrawLatex(0.4,0.84,buffer);
  sprintf(buffer,"expected : %1.2e",exp_ggh_yield->getVal());
  latex.DrawLatex(0.4,0.80,buffer);

  if (cut)  {
    sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
    canvas->cd();    
    latex.DrawLatex(0.2,0.96,buffer);
  }

  if (categ>-1)  {
    sprintf(buffer,"category %d",categ);
    canvas->cd();    
    latex.DrawLatex(0.6,0.96,buffer);
  }
  
    
  //saving canvas
  for (int file=0;file<3;file++) {
    sprintf(buffer,"%s%ssplot_unweightedmass_gghbkg%s%s_reco.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);  
    canvas->SaveAs(buffer);
    }  

    frame_down->Delete();
    legend->Delete();


  RooPlot *frame_up=dipho_pt->frame();
  sim->plotOn(frame_up,Name("sim"),Binning(NBINS,0,dipho_pt->getMax()),MarkerColor(kBlack));
  model->plotOn(frame_up, LineColor(kBlue),Name("model"));
  model->plotOn(frame_up,Components("model_ggh"),LineColor(kGreen),LineStyle(kDashed),Name("model_ggh"));
  model->plotOn(frame_up,Components("model_bkg"),LineColor(kRed),LineStyle(kDashed),Name("model_bkg"));

  canvas->cd();
  frame_up->UseCurrentStyle();
  frame_up->Draw();

  legend=new TLegend(0.8,0.7,1,1);    
  legend->AddEntry(frame_up->findObject("sim"),"Inclusive Generated Events","lpe");
  legend->AddEntry(frame_up->findObject("model"),"Inclusive Fit","l");
  legend->AddEntry(frame_up->findObject("model_ggh"),"ggH Component","l");
  legend->AddEntry(frame_up->findObject("model_bkg"),"bkg Component","l");
  legend->Draw();

  sprintf(buffer,"signal yields");
  latex.DrawLatex(0.4,0.88,buffer);
  sprintf(buffer,"fitted : %1.2e",ggh_yield->getVal());
  latex.DrawLatex(0.4,0.84,buffer);
  sprintf(buffer,"expected : %1.2e",exp_ggh_yield->getVal());
  latex.DrawLatex(0.4,0.80,buffer);

  if (cut)  {
    sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
    canvas->cd();    
    latex.DrawLatex(0.2,0.96,buffer);
  }

  if (categ>-1)  {
    sprintf(buffer,"category %d",categ);
    canvas->cd();    
    latex.DrawLatex(0.6,0.96,buffer);
  }
  
    
  //saving canvas
  for (int file=0;file<3;file++) {
    sprintf(buffer,"%s%ssplot_unweightedpt_gghbkg%s%s_reco.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);  
    canvas->SaveAs(buffer);
    }  

    frame_up->Delete();
    legend->Delete();
  
  cout << "plotted non weighted" << endl;



  // plot weighted events and first fit
  frame_up=dipho_pt->frame();
  RooDataSet *sim_Wggh=new RooDataSet("sim_Wggh","sim_Wggh",simW,*simW->get(),0,"ggh_yield_sw");
  sim_Wggh->plotOn(frame_up,MarkerColor(2),DataError(RooAbsData::SumW2),Name("sim_Wggh"),Binning(NBINS,0,dipho_pt->getMax()));// plot weighted ggh events
  RooDataSet *sim_Wbkg=new RooDataSet("sim_Wbkg","sim_Wbkg",simW,*simW->get(),0,"bkg_yield_sw");
  sim_Wbkg->plotOn(frame_up,MarkerColor(1),DataError(RooAbsData::SumW2),Name("sim_Wbkg"),Binning(NBINS,0,dipho_pt->getMax())); // plot weighted background events
  model->plotOn(frame_up,Components("model_bkg"),LineColor(4),Name("model_bkg"));// background component of the total fit
  model->plotOn(frame_up,Components("model_ggh"),LineColor(3),Name("model_ggh"));//ggh component of total fit 


  canvas->cd();
  //  canvas->SetLogy();
  frame_up->UseCurrentStyle();
  frame_up->Draw();
  legend=new TLegend(0.8,0.7,1,1);
  legend->AddEntry("sim_Wggh", "ggH Weighted Events","lpe");
  legend->AddEntry("sim_Wbkg", "bkg Weighted Events","lpe");
  legend->AddEntry("model_ggh","Non Weighted ggH Fit","l");
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
      latex.DrawLatex(0.5,0.96,buffer);
  }

  //saving canvas
  for (int file=0; file<3; file++ ) {
    sprintf(buffer,"%s%ssplot_gghbkg%s%s_reco.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
      canvas->SaveAs(buffer);
  }
  cout << "wieghted drawn" << endl;

  frame_up->Delete();

  // plot weighted signal events and first fit
  frame_up=dipho_pt->frame();
  sim_Wggh->plotOn(frame_up,MarkerColor(2),DataError(RooAbsData::SumW2),Name("sim_Wggh"),Binning(NBINS,0,dipho_pt->getMax()));// plot weighted ggh events
  model->plotOn(frame_up,Components("model_ggh"),LineColor(3),Normalization(sim_Wbkg->sumEntries(),RooAbsReal::NumEvent),Name("model_ggh"));//ggh component of total fit 
  canvas->cd();
  //   canvas->SetLogy();
  frame_up->UseCurrentStyle();
  frame_up->Draw();
  legend=new TLegend(0.8,0.9,1,1);
  legend->AddEntry("sim_Wggh", "ggH Weighted Events","lpe");
  legend->AddEntry("model_ggh","Non Weighted ggH Fit","l");
  legend->Draw();

 if(cut) {
      sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
      canvas->cd();
      latex.DrawLatex(0.05,0.96,buffer);
 }
 if(categ>-1) {
   sprintf(buffer,"category %d",categ);
   canvas->cd();
   latex.DrawLatex(0.5,0.96,buffer);
 }

  for (int file=0; file<3; file++ ) {
    sprintf(buffer,"%s%ssplot_ggh%s%s_reco.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
      canvas->SaveAs(buffer);
  }

  cout << "weigthed ggh drawn" << endl;




  return 0;}
    
