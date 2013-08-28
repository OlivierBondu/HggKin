// Original file by C. Goudet
// Adapted by O. Bondu (Aug 2013)
// C++ headers
#include <iostream>
// ROOT Headers
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TH1F.h"
#include "TLine.h"
// RooFit headers
#include "RooConstVar.h"
#include "RooFormulaVar.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooDataSet.h"
#include "RooGenericPdf.h"
#include "RooProdPdf.h"
#include "RooBernstein.h"
// RooStats headers
#include "RooWorkspace.h"
#include "RooStats/SPlot.h"
// plot style
#include "setTDRStyle.h"
// definitions
#define BATCH 0 // On batch mode, have to change loading and saving path
#define NBINS 10
#define WIDTH 10
// namespaces
using namespace std;
using namespace RooStats;
using namespace RooFit;
// functions declaration
int AddModel(RooWorkspace*, int const &cut=0, int const &categ=-1); // Add pdf to workspace and pre-fit them
int AddData(RooWorkspace*, int const &cut=0, int const &categ=-1); // Add simulated events ro workspace
int DoSPlot(RooWorkspace*, int const &cut=0, int const &categ=-1); // Create SPlot object
int MakePlot(RooWorkspace*, int const &cut=0, int const &categ=-1); // Create and save result and check plots


int main(int argc, char* argv[]) {
  //#############menu
  int const menu_cut[5]={0,200,375,550,750};

  TFile *root_file=0;
  if (BATCH) root_file=new TFile("WS_SPlot.root","UPDATE"); //File to store the workspace
  else root_file=new TFile("WS_SPlot.root","UPDATE");
  RooWorkspace *ws=0;
  char buffer[100];

  int i=0;
  int categ=-1;
//     for (int i=0; i<5;i++) {
//   for (int categ=-1; categ<4;categ++) {
      sprintf(buffer,"ws_pt");
      if (i) sprintf(buffer,"%s%d",buffer,menu_cut[i]);
      if (categ>-1) sprintf(buffer,"%s_categ%d",buffer,categ);
      sprintf(buffer,"%s_datasgn",buffer);
      ws=new RooWorkspace(buffer,buffer);
      AddModel(ws,menu_cut[i],categ);
      AddData(ws,menu_cut[i],categ);
       DoSPlot(ws,menu_cut[i],categ);
      MakePlot(ws,menu_cut[i],categ);
      root_file->cd();
      ws->Write("",TObject::kOverwrite);
      ws->Delete();  
//       } 
//       }

  root_file->Close();
  cout << "Went up to the end" << endl;
  return 0;
}


//######################################################################################################################################
//######################################################################################################################################
//######################################################################################################################################
int AddModel(RooWorkspace *ws, int  const &cut, int const &categ) {
  setTDRStyle(); 
  TFile *file_kin=0;
  if (BATCH) file_kin=new TFile("kin_dist.root");
  else file_kin=new TFile("kin_dist.root");
  
  
  RooRealVar *dipho_mass=new RooRealVar("dipho_mass","m_{#gamma #gamma}",100,180,"GeV/c^{2}");
  RooRealVar *dipho_pt=new RooRealVar("dipho_pt","p_{T#gamma#gamma}",0,NBINS*WIDTH,"GeV/c");
  RooRealVar *dipho_ctheta=new RooRealVar("dipho_ctheta","cos(#theta *)",0,1);
  RooRealVar *category=new RooRealVar("category","category",-1,5);
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  char buffer[100],buffer2[100];  



  //Model of sgn =2 gaussians
  cout << "#####  Model sgn" << endl;
  //create model of background and fit  

  RooRealVar *mean1_sgn=new RooRealVar("mean1_sgn","mean1_sgn",128,122,128);
  RooRealVar *sigma1_sgn=new RooRealVar("sigma1_sgn","sigma1_sgn",9,0,10);
  RooGaussian *model1_sgn=new RooGaussian("model1_sgn","model1_sgn",*dipho_mass,*mean1_sgn,*sigma1_sgn);

  RooRealVar *mean2_sgn=new RooRealVar("mean2_sgn","mean2_sgn",120,120,130);
  RooRealVar *sigma2_sgn=new RooRealVar("sigma2_sgn","sigma2_sgn",9,0,10);
  RooGaussian *model2_sgn=new RooGaussian("model2_sgn","model2_sgn",*dipho_mass,*mean2_sgn,*sigma2_sgn);

  RooRealVar *compo1_sgn=new RooRealVar("compo1_sgn","compo1_sgn",0,1);
  RooAddPdf *model_sgn=new RooAddPdf("model_sgn","model_sgn",RooArgSet(*model1_sgn,*model2_sgn),RooArgSet(*compo1_sgn));


  TTree *tree=(TTree*) file_kin->Get("tree_reco_ggh");
  sprintf(buffer,""); sprintf(buffer2,"");
  if (cut>0) {
    sprintf(buffer, "dipho_ctheta > %1.3f",cut/1000.);   
    sprintf(buffer2," && ");
  }
  if (categ>-1) {
    sprintf(buffer,"%s%s category > %2.2f && category < %2.2f",buffer,buffer2,categ-0.1,categ+0.1);
    sprintf(buffer2," && ");
  }
  RooRealVar *weight=new RooRealVar("weight","weight",0,100);
  RooDataSet*  sim_sgn=new RooDataSet("sim_sgn","sim_sgn",tree , RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category,*weight),buffer,"weight");
  tree->Delete();

  tree=(TTree *) file_kin->Get("tree_reco_vbf");
  RooDataSet *sgn_sm=new RooDataSet("sgn_sm","sgn_sm",tree,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category,*weight),buffer,"weight");
  sim_sgn->append(*sgn_sm);

  sgn_sm->Delete();
  model_sgn->fitTo(*sim_sgn,Range(110,140));



 //Check plot
RooPlot *framesgn=dipho_mass->frame(100,180,40);  
// sim_sgn->plotOn(framesgn,MarkerColor(kRed),MarkerSize(1.5));
//  model_sgn->plotOn(framesgn);
 sgn_sm=(RooDataSet *) sim_sgn->reduce("(dipho_mass<115 || dipho_mass>135)");
 sgn_sm->plotOn(framesgn,MarkerColor(kBlue));
  framesgn->Draw();
  canvas->SaveAs("frame_masssgn_data.pdf");
  cout << framesgn->chiSquare() << endl;
  framesgn->Delete();

  ws->import(*sgn_sm,Rename("sim_sgn"));

  //#################################
  // Modelackgroubd Bern4 
  cout << "#####   Model bkg" << endl;
  RooRealVar *coef0_bern_bkg=new RooRealVar("coef0_bern_bkg","coef0_bern_bkg",0.001,0,1);
  RooRealVar *coef1_bern_bkg=new RooRealVar("coef1_bern_bkg","coef1_bern_bkg",0.001,0,1);
  RooRealVar *coef2_bern_bkg=new RooRealVar("coef2_bern_bkg","coef2_bern_bkg",0.001,0,1);
  RooRealVar *coef3_bern_bkg=new RooRealVar("coef3_bern_bkg","coef3_bern_bkg",0.001,0,1);
  RooBernstein *model_bkg=new RooBernstein("model_bkg","model_bkg",*dipho_mass,RooArgSet(*coef0_bern_bkg,*coef1_bern_bkg,*coef2_bern_bkg,*coef3_bern_bkg));
  
  tree=(TTree*) file_kin->Get("tree_data");
  int nentries=tree->GetEntries();
  cout << "after tree" << endl;
  sprintf(buffer,""); sprintf(buffer2,"");
  if (cut>0) {
    sprintf(buffer, "dipho_ctheta > %1.3f",cut/1000.);   
    sprintf(buffer2," && ");
  }
  if (categ>-1) {
    sprintf(buffer,"%s%s category > %2.2f && category < %2.2f",buffer,buffer2,categ-0.1,categ+0.1);
    sprintf(buffer2," && ");
  }
  RooDataSet*  data=new RooDataSet("data","data",tree , RooArgSet(*dipho_mass,*dipho_ctheta,*category),buffer);
  tree->Delete();
  model_bkg->fitTo(*data);
//   coef0_bern_bkg->setRange(coef0_bern_bkg->getVal()-1*coef0_bern_bkg->getError(),coef0_bern_bkg->getVal()+1*coef0_bern_bkg->getError());
//   coef1_bern_bkg->setRange(coef1_bern_bkg->getVal()-1*coef1_bern_bkg->getError(),coef1_bern_bkg->getVal()+1*coef1_bern_bkg->getError());
//   coef2_bern_bkg->setRange(coef2_bern_bkg->getVal()-1*coef2_bern_bkg->getError(),coef2_bern_bkg->getVal()+1*coef2_bern_bkg->getError());
//   coef3_bern_bkg->setRange(coef3_bern_bkg->getVal()-1*coef3_bern_bkg->getError(),coef3_bern_bkg->getVal()+1*coef3_bern_bkg->getError());
  
  // dipho_mass->setRange("low_sideband",100,122);  
//   dipho_mass->setRange("high_sideband",128,180);  
//   model_bkg->fitTo(*data,Range("high_sideband")); 
  
 //Check plot
  RooPlot *frame=dipho_mass->frame(100,180,16);
  data->plotOn(frame);
  model_bkg->plotOn(frame);
  frame->Draw();
  canvas->SaveAs("frame_massbkg_data.pdf");
  cout << frame->chiSquare() << endl;
  frame->Delete();


  RooRealVar *sgn_yield=new RooRealVar("sgn_yield","sgn_yield",1,500);
  RooRealVar *bkg_yield=new RooRealVar("bkg_yield","bkg_yield",1,2e5);
  RooAddPdf *model_sgnbkg= new RooAddPdf("model_sgnbkg","model_sgnbkg",RooArgList(*model_sgn,*model_bkg),RooArgList(*sgn_yield,*bkg_yield));  
  // Import all pdf and variables (implicitely) into workspace

  ws->import(*category);
  ws->import(*dipho_ctheta);
  ws->import(*model_sgnbkg);
  ws->import(*dipho_pt);
  file_kin->Close();
  canvas->Close();
  cout << "end AddModel" <<  endl;
  return 0;
}

//######################################################################################################
//######################################################################################################
//######################################################################################################
int AddData(RooWorkspace* ws, int const &cut, int const &categ) {
  cout << "in AddData" << endl;
  RooRealVar *dipho_mass=ws->var("dipho_mass");
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *dipho_ctheta=ws->var("dipho_ctheta");
  RooRealVar *category=ws->var("category");
  char buffer[100],buffer2[100];
  TFile *file_kin=0;
  if (BATCH) file_kin=new TFile("kin_dist.root");
  else file_kin=new TFile("kin_dist.root");

  //Create the cut formula and gather data
  sprintf(buffer,""); sprintf(buffer2,"");
  if (cut>0) {
    sprintf(buffer,"dipho_ctheta>%d/1000.",cut);
    sprintf(buffer2," && ");
  }
  if (categ>-1) {
    sprintf(buffer,"%s%s category > %2.2f && category < %2.2f",buffer,buffer2,categ-0.1,categ+0.1);
    sprintf(buffer2," && ");
  }

  cout << "buffer : " << buffer << endl;
  TTree *tree_data =(TTree *) file_kin->Get("tree_data");
  cout << "tree test" << tree_data->GetEntries() << endl;
  RooDataSet *dataset=new RooDataSet("dataset","dataset",tree_data,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category),buffer);
  sprintf (buffer,"%s%s (dipho_mass<115 || dipho_mass>135)",buffer,buffer2);  
  RooDataSet *dataset_blind=new RooDataSet("dataset_blind","dataset_blind",tree_data,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category),buffer);
  cout << "dataset created " << endl;

  //variable for blinding correction
  RooRealVar *ntree_data=new RooRealVar("ntree_data","ntree_data",tree_data->GetEntries(),0,tree_data->GetEntries());

  tree_data->Delete();    
  //Check Plot
//   TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
//   RooPlot *framemass=dipho_mass->frame(100,180,40);
//   dataset->plotOn(framemass);
//   framemass->Draw();
//   canvas->SaveAs("frame_mass_reco.pdf");
//   RooPlot *framept=dipho_pt->frame(0,NBINS*WIDTH,NBINS);
//   dataset->plotOn(framept);
//   framept->Draw();
//   canvas->SaveAs("frame_pt_reco.pdf");  
    
  //import dataset into workspace
  ws->import(*dataset);
  ws->import(*dataset_blind);
  ws->import(*ntree_data);

  file_kin->Close();
  cout << "end adddata" << endl;
  return 0;}
    


//###################################################################################################
//###################################################################################################
//###################################################################################################
int DoSPlot(RooWorkspace* ws, int const &cut, int const &categ) {
  cout << "in DoSplot" << endl;
  setTDRStyle();
  RooAbsPdf *model_sgnbkg=ws->pdf("model_sgnbkg");
  RooDataSet *dataset=(RooDataSet*) ws->data("dataset");
  RooDataSet *dataset_blind=(RooDataSet *) ws->data("dataset_blind");
  RooRealVar *dipho_mass=ws->var("dipho_mass");  
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *exp_sgn_yield=ws->var("exp_sgn_yield");
  RooRealVar *weight=ws->var("weight");

  //Fixing discriminant parameters
   RooRealVar *coef0_bern_bkg=ws->var("coef0_bern_bkg");   coef0_bern_bkg->setConstant(1);
   RooRealVar *coef1_bern_bkg=ws->var("coef1_bern_bkg");   coef1_bern_bkg->setConstant(1);
   RooRealVar *coef2_bern_bkg=ws->var("coef2_bern_bkg");   coef2_bern_bkg->setConstant(1);
   RooRealVar *coef3_bern_bkg=ws->var("coef3_bern_bkg");   coef3_bern_bkg->setConstant(1);
   RooRealVar *compo1_sgn=ws->var("compo1_sgn");   compo1_sgn->setConstant(1);
   RooRealVar* mean1_sgn=ws->var("mean1_sgn");   mean1_sgn->setConstant(1);
   RooRealVar* sigma1_sgn=ws->var("sigma1_sgn");   sigma1_sgn->setConstant(1);
   RooRealVar* mean2_sgn=ws->var("mean2_sgn");   mean2_sgn->setConstant(1);
   RooRealVar* sigma2_sgn=ws->var("sigma2_sgn");   sigma2_sgn->setConstant(1);
   
   RooRealVar *sgn_yield=ws->var("sgn_yield");
   RooRealVar *bkg_yield=ws->var("bkg_yield");
   
   cout << "Model variables retrieved and set constant" << endl;

   model_sgnbkg->fitTo(*dataset);  
  // Check plot
  char buffer_path[100]="",buffer_file[2][3][10]={{""}},buffercut[10]="",buffercateg[30]="";
  sprintf(buffer_file[1][0],"png");
  sprintf(buffer_file[1][1],"pdf");
  sprintf(buffer_file[1][2],"root");
  if (! BATCH) {
    sprintf(buffer_path,"plot/");
    sprintf(buffer_file[0][0],"png/");
    sprintf(buffer_file[0][1],"pdf/");
    sprintf(buffer_file[0][2],"root/");
  }
  if (cut) sprintf(buffercut,"%d",cut);
  if (categ>-1) sprintf(buffercateg,"_categ%d",categ);




  cout << "starting splot" << endl;
  RooDataSet *Wdataset=new RooDataSet(*dataset,"Wdataset");
  SPlot *splot_sgnbkg=new SPlot("splot_sgnbkg","splot_sgnbkg", *Wdataset, model_sgnbkg, RooArgList(*sgn_yield, *bkg_yield));  //Create splot
  ws->import(*Wdataset,Rename("Wdataset"));
  cout << sgn_yield->getVal() << endl;

  TLatex latex; latex.SetNDC(1); char buffer[100];
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  TLegend *legend=new TLegend(0.7,0.7,1,1);
  legend->SetTextSize(0.04);
  RooPlot* frame_mass=dipho_mass->frame(100,180,40); frame_mass->UseCurrentStyle();
  frame_mass->GetYaxis()->SetLabelSize(0.03);
  dataset_blind->plotOn(frame_mass,Name("data"));
//   model_sgnbkg->plotOn(frame_mass,Name("full"));
//   model_sgnbkg->plotOn(frame_mass, Components("model_sgn"), LineColor(kGreen), LineStyle(kDashed),Name("sgn"));
  model_sgnbkg->plotOn(frame_mass, Components("model_bkg"), LineColor(kRed), LineStyle(kDashed),Name("bkg"),Normalization(dataset->sumEntries(),RooAbsReal::NumEvent));
  frame_mass->Draw();
  //  latex.DrawLatex(0.45,0.86,"sgn yield");
  sprintf(buffer,"fitted = %3.0f",sgn_yield->getVal()); 
  //latex.DrawLatex(0.45,0.81,buffer);
  legend->AddEntry(frame_mass->findObject("data"),"Data","lpe");
  //  legend->AddEntry(frame_mass->findObject("full"),"Full Fit","l");
  // legend->AddEntry(frame_mass->findObject("sgn"),"sgn Fit","l");
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
    sprintf(buffer,"%s%sSPlotInput_pt%s%s_datasgn.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
    canvas->SaveAs(buffer);
  }
  

  canvas->Close();
  cout << "end DoSPlot" << endl;
  return 0;
}

//#############################################################################################"
//#############################################################################################"
//#############################################################################################"
int MakePlot(RooWorkspace* ws, int const &cut, int const &categ) {

  setTDRStyle();
  // collect usefull variables
  RooAbsPdf *model=ws->pdf("model_sgnbkg");
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *dipho_mass=ws->var("dipho_mass");
  RooRealVar *sgn_yield=ws->var("sgn_yield");
  RooRealVar *bkg_yield=ws->var("bkg_yield");
  RooDataSet *Wdataset=(RooDataSet*) ws->data("Wdataset");
  RooDataSet *dataset_blind=(RooDataSet*) ws->data("dataset_blind");

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
  TLegend *legend=0;

  //define path prefixes  for saving canvas
  char buffer_path[100]="",buffer_file[2][3][10]={{""}},buffercut[10]="",buffercateg[30]="";
  sprintf(buffer_file[1][0],"png");
  sprintf(buffer_file[1][1],"pdf");
  sprintf(buffer_file[1][2],"root");
  if (! BATCH) {
    sprintf(buffer_path,"plot/");
    sprintf(buffer_file[0][0],"png/");
    sprintf(buffer_file[0][1],"pdf/");
    sprintf(buffer_file[0][2],"root/");
  }
  if (cut) sprintf(buffercut,"%d",cut);
  if (categ>-1) sprintf(buffercateg,"_categ%d",categ);
  //----------------------------



  //compute integral of pdf in sidebands
  dipho_mass->setRange("low_SB",100,115);
  dipho_mass->setRange("high_SB",135,180);

  RooAddPdf *model_sgn=(RooAddPdf *) ws->pdf("model_sgn");
  RooAbsReal *int_sgn=model_sgn->createIntegral(*dipho_mass,Range("low_SB,high_SB"),NormSet(*dipho_mass));
  cout << "shn integral  " << int_sgn->getVal() << endl;
  RooAddPdf *model_bkg=(RooAddPdf *) ws->pdf("model_bkg");
  RooAbsReal *int_bkg=model_bkg->createIntegral(*dipho_mass,Range("low_SB,high_SB"),NormSet(*dipho_mass));

  RooRealVar *ntree_data=(RooRealVar *) ws->var("ntree_data");
  cout << ntree_data->getVal() << endl;
  //Creating functions for corrected weights
  RooRealVar *sgn_yield_sw=new RooRealVar("sgn_yield_sw","sgn_yield_sw",0,-100,100);
  RooRealVar *bkg_yield_sw=new RooRealVar("bkg_yield_sw","bkg_yield_sw",0,-100,100);

  RooDataSet *dum_swbkg=new RooDataSet("dum_swbkg","dum_swbkg",Wdataset,*Wdataset->get(),"dipho_mass<115 || dipho_mass>135","bkg_yield_sw");
  RooRealVar *sum_swbkg=new RooRealVar("sum_swbkg","sum_swbkg",dum_swbkg->sumEntries());
  RooDataSet *dum_swsgn=new RooDataSet("dum_swsgn","dum_swsgn",Wdataset,*Wdataset->get(),"dipho_mass<115 || dipho_mass>135","sgn_yield_sw");
  RooRealVar *sum_swsgn=new RooRealVar("sum_swsgn","sum_swsgn",dum_swsgn->sumEntries());

  RooFormulaVar *sw_blindsgn=new RooFormulaVar("sw_blindsgn","sw_blindsgn","@0*@1*@2/@3",RooArgSet(*sgn_yield_sw,*sgn_yield,*int_sgn,*sum_swsgn));
  Wdataset->addColumn(*sw_blindsgn); 
  RooFormulaVar *sw_blindbkg=new RooFormulaVar("sw_blindbkg","sw_blindbkg","@0*@1*@2/@3",RooArgSet(*bkg_yield_sw,*bkg_yield,*int_bkg,*sum_swbkg));
  Wdataset->addColumn(*sw_blindbkg);  

//   RooFormulaVar *sw_blindsgn=new RooFormulaVar("sw_blindsgn","sw_blindsgn","@0*@1*@2/@3",RooArgSet(*sgn_yield_sw,*sgn_yield,*int_sgn,*ntree_data));
//   Wdataset->addColumn(*sw_blindsgn); 
//   RooFormulaVar *sw_blindbkg=new RooFormulaVar("sw_blindbkg","sw_blindbkg","@0*@1*@2/@3",RooArgSet(*bkg_yield_sw,*bkg_yield,*int_bkg,*ntree_data));
//   Wdataset->addColumn(*sw_blindbkg);  

 
  //create datasets weighted with corrected weights
  //Create datasets blinded sweighted
  RooDataSet *swblind_bkg=new RooDataSet("swblind_bkg","swblind_bkg",Wdataset,*Wdataset->get(),"dipho_mass<115 || dipho_mass>135","sw_blindbkg");
  RooDataSet *swblind_sgn=new RooDataSet("swblind_sgn","swblind_sgn",Wdataset,*Wdataset->get(),"dipho_mass<115 || dipho_mass>135","sw_blindsgn");
  cout << "blinded sweighted dataset created" << endl;
  cout << "somme weight : " << swblind_bkg->sumEntries()+swblind_sgn->sumEntries() << "   " << Wdataset->sumEntries() << " " << endl;
  //---------------------------------


  //Plotting bkg sPlot output
 
  pad_up->Draw();
  pad_up->cd();
  RooPlot *frame_pt=dipho_pt->frame(NBINS);
  swblind_bkg->plotOn(frame_pt,MarkerSize(1.5),MarkerColor(1),LineColor(1),DataError(RooAbsData::SumW2),Name("swblind_bkg"),Binning(NBINS,0,dipho_pt->getMax()));
  dataset_blind->plotOn(frame_pt,MarkerColor(4),LineColor(4),DataError(RooAbsData::SumW2),Name("blind_bkg"),Binning(NBINS,0,dipho_pt->getMax()));
  cout << "sum  "  << dataset_blind->sumEntries() << endl;
  frame_pt->UseCurrentStyle();
  frame_pt->Draw();

  legend=new TLegend(0.6,0.7,1,1);
  legend->SetTextSize(0.05);
  legend->AddEntry("","Data","");
  legend->AddEntry("swblind_bkg", "sweighted background","lpe");
  legend->AddEntry("blind_bkg","Background blinded","lpe");
  legend->Draw();

  // creata ratio histogram (sw-data)/data
  TH1F *hist_fit=new TH1F("hist_fit","hist_fit",NBINS,0,NBINS*WIDTH);
  dataset_blind->fillHistogram(hist_fit,*dipho_pt);
  TH1F *hist_set=new TH1F("hist_set","hist_set",NBINS,0,NBINS*WIDTH);
  swblind_bkg->fillHistogram(hist_set,*dipho_pt);  
  TLine *line=new TLine(0,0,NBINS*WIDTH,0);
  line->SetLineColor(kRed);
  hist_set->Add(hist_fit,-1);
  hist_set->Divide(hist_fit);  

  hist_set->GetYaxis()->SetTitle("#frac{N_{sPlot}-N_{dat}}{N_{dat}}");
  hist_set->GetYaxis()->SetTitleOffset(0.35);
  hist_set->GetXaxis()->SetTitle(frame_pt->GetXaxis()->GetTitle());
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
  if (categ>-1) {
    sprintf(buffer,"category %d",categ);
    canvas->cd();
    latex.DrawLatex(0.4,0.96,buffer);
  }
  for (int file=0; file<3; file++ ) {
    sprintf(buffer,"%s%ssplot_pt%s%s_datasgnbkg.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
    canvas->SaveAs(buffer);
  }

  frame_pt->Delete();
  legend->Delete();
 
  hist_fit->Delete();
  hist_set->Delete();  

  cout << "sw bkg drawn" << endl;


  // plot weighted signal events and first fit
  pad_up->cd();
  frame_pt=dipho_pt->frame(NBINS);
  swblind_sgn->plotOn(frame_pt,MarkerColor(2),LineColor(2),DataError(RooAbsData::SumW2),Name("swblind_sgn"),Binning(NBINS,0,dipho_pt->getMax()));// plot weighted sgn events
  RooDataSet *sim_sgn=(RooDataSet *) ws->data("sim_sgn");
  sim_sgn->plotOn(frame_pt,MarkerColor(3),LineColor(3),DataError(RooAbsData::SumW2),Name("sgn_sm"),Binning(NBINS,0,dipho_pt->getMax()));

  frame_pt->UseCurrentStyle();
  frame_pt->GetXaxis()->SetTitleSize(0);
  frame_pt->GetXaxis()->SetLabelSize(0);
  frame_pt->Draw();

  legend=new TLegend(0.6,0.75,1,1);
  legend->SetTextSize(0.05);
  legend->AddEntry("","Data","");
  legend->AddEntry("swblind_sgn", "sweighted sgn","lpe");
  legend->AddEntry("sgn_sm","SM Higgs MC","lpe");
  legend->Draw();

  //compare signal with SMModel
  hist_fit=new TH1F("hist_fit","hist_fit",NBINS,0,NBINS*WIDTH);
  sim_sgn->fillHistogram(hist_fit,*dipho_pt);
  hist_set=new TH1F("hist_set","hist_set",NBINS,0,NBINS*WIDTH);
  swblind_sgn->fillHistogram(hist_set,*dipho_pt);  
  hist_set->SetTitle("test");
  //  line=new TLine(0,0,NBINS*WIDTH,0);
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
    sprintf(buffer,"%s%ssplot_pt%s%s_datasgn.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
    canvas->SaveAs(buffer);
  }
  cout << "weigthed sgn drawn" << endl;

  cout << "end MakePlot" << endl;
  return 0;}
    
