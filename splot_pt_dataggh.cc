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
#include "RooGenericPdf.h"
#include "RooProdPdf.h"
#include "RooBernstein.h"


#include "RooStats/SPlot.h"

#define BATCH 0 // On batch mode, have to change loading and saving path
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
  if (BATCH) root_file=new TFile("WS_SPlot_data.root","UPDATE"); //File to store the workspace
  else root_file=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/WS_SPlot_data.root","UPDATE");
  RooWorkspace *ws=0;
  char buffer[100];

  int i=0;
  int categ=-1;
//     for (int i=0; i<5;i++) {
//   for (int categ=-1; categ<4;categ++) {
      sprintf(buffer,"ws_hgg_splot");
      if (i) sprintf(buffer,"%s%d",buffer,menu_cut[i]);
      if (categ>-1) sprintf(buffer,"%s_categ%d",buffer,categ);
      sprintf(buffer,"%s_data",buffer);
      ws=new RooWorkspace(buffer,buffer);
      AddModel(ws,menu_cut[i],categ);
      AddData(ws,menu_cut[i],categ);
       DoSPlot(ws,menu_cut[i],categ);
       MakePlot(ws,menu_cut[i],categ);
//       root_file->cd();
//       ws->Write("",TObject::kOverwrite);
//       ws->Delete();  
//       } 
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
  
  
  RooRealVar *dipho_mass=new RooRealVar("dipho_mass","m_{#gamma #gamma}",100,180,"GeV/c^{2}");
  RooRealVar *dipho_pt=new RooRealVar("dipho_pt","p_{T#gamma#gamma}",0,NBINS*WIDTH,"GeV/c");
  RooRealVar *dipho_ctheta=new RooRealVar("dipho_ctheta","cos(#theta *)",0,1);
  RooRealVar *category=new RooRealVar("category","category",-1,5);
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  char buffer[100],buffer2[100];  



  //Model of ggh =2 gaussians
  cout << "#####  Model ggH" << endl;
  //create model of background and fit  
  RooRealVar *mean1_ggh=new RooRealVar("mean1_ggh","mean1_ggh",124,124,126);
  RooRealVar *sigma1_ggh=new RooRealVar("sigma1_ggh","sigma1_ggh",1,0,3);
  RooGaussian *model1_ggh=new RooGaussian("model1_ggh","model1_ggh",*dipho_mass,*mean1_ggh,*sigma1_ggh);

  RooRealVar *mean2_ggh=new RooRealVar("mean2_ggh","mean2_ggh",126,120,130);
  RooRealVar *sigma2_ggh=new RooRealVar("sigma2_ggh","sigma2_ggh",2,0,5);
  RooGaussian *model2_ggh=new RooGaussian("model2_ggh","model2_ggh",*dipho_mass,*mean2_ggh,*sigma2_ggh);

  RooRealVar *compo_ggh=new RooRealVar("compo_ggh","compo_ggh",0,1);
  //  RooAddPdf *model_ggh=new RooAddPdf("model_ggh","model_ggh",RooArgSet(*model1_ggh,*model2_ggh),*compo_ggh);
  RooGaussian *model_ggh=new RooGaussian(*model1_ggh,"model_ggh");

  
  //#################################
  // model backgroubd Bern3 
  cout << "#####   Model bkg" << endl;
  RooRealVar *coef0_bern_bkg=new RooRealVar("coef0_bern_bkg","coef0_bern_bkg",0.001,0,1);
  RooRealVar *coef1_bern_bkg=new RooRealVar("coef1_bern_bkg","coef1_bern_bkg",0.001,0,1);
  RooRealVar *coef2_bern_bkg=new RooRealVar("coef2_bern_bkg","coef2_bern_bkg",0.001,0,1);
  RooRealVar *coef3_bern_bkg=new RooRealVar("coef3_bern_bkg","coef3_bern_bkg",0.001,0,1);
  RooBernstein *model_bkg=new RooBernstein("model_bkg","model_bkg",*dipho_mass,RooArgSet(*coef0_bern_bkg,*coef1_bern_bkg,*coef2_bern_bkg,*coef3_bern_bkg));
  
 TTree *tree=(TTree*) file_kin->Get("tree_data");
 cout << "before tree" << endl;
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
  // Blind the signal regiion for background fit
  //  sprintf(buffer,"%s%s (dipho_mass >128 || dipho_mass<122)",buffer,buffer2);
  cout << buffer << endl;   
  RooDataSet*  data_blind=new RooDataSet("data_blind","data_blind",tree , RooArgSet(*dipho_mass,*dipho_ctheta,*category),buffer);
  tree->Delete();
  model_bkg->fitTo(*data_blind);
  coef0_bern_bkg->setRange(coef0_bern_bkg->getVal()-1*coef0_bern_bkg->getError(),coef0_bern_bkg->getVal()+1*coef0_bern_bkg->getError());
  coef1_bern_bkg->setRange(coef1_bern_bkg->getVal()-1*coef1_bern_bkg->getError(),coef1_bern_bkg->getVal()+1*coef1_bern_bkg->getError());
  coef2_bern_bkg->setRange(coef2_bern_bkg->getVal()-1*coef2_bern_bkg->getError(),coef2_bern_bkg->getVal()+1*coef2_bern_bkg->getError());
  coef3_bern_bkg->setRange(coef3_bern_bkg->getVal()-1*coef3_bern_bkg->getError(),coef3_bern_bkg->getVal()+1*coef3_bern_bkg->getError());
  
//   dipho_mass->setRange("low_sideband",100,122);  
//   dipho_mass->setRange("high_sideband",128,180);  
//   model_bkg->fitTo(*data_blind,Range("high_sideband")); 
  
 //Check plot
  RooPlot *frame=dipho_mass->frame(100,180,16);
  data_blind->plotOn(frame);
  model_bkg->plotOn(frame);
  frame->Draw();
  canvas->SaveAs("frame_massbkg_data.pdf");
  cout << frame->chiSquare() << endl;
  frame->Delete();


  RooRealVar *ggh_yield=new RooRealVar("ggh_yield","ggh_yield",1,500);
  RooRealVar *bkg_yield=new RooRealVar("bkg_yield","bkg_yield",1,2e5);
  RooAddPdf *model_gghbkg= new RooAddPdf("model_gghbkg","model_gghbkg",RooArgList(*model_ggh,*model_bkg),RooArgList(*ggh_yield,*bkg_yield));  
  // Import all pdf and variables (implicitely) into workspace

  ws->import(*category);
  ws->import(*dipho_ctheta);
  ws->import(*model_gghbkg);
  ws->import(*dipho_pt);
  file_kin->Close();
  canvas->Close();
  cout << "end AddModel" <<  endl;
  return 0;
}

//######################################################################################################
//######################################################################################################
//######################################################################################################
int AddData(RooWorkspace* ws, int const &cut=0, int const &categ=0) {
  cout << "in AddData" << endl;
  RooRealVar *dipho_mass=ws->var("dipho_mass");
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *dipho_ctheta=ws->var("dipho_ctheta");
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
  cout << "buffer : " << buffer << endl;
  TTree *tree_data =(TTree *) file_kin->Get("tree_data");
  cout << "tree test" << tree_data->GetEntries() << endl;
  RooDataSet *dataset=new RooDataSet("dataset","dataset",tree_data,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category),buffer);
  cout << "dataset created " << endl;
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
  RooDataSet *dataset=(RooDataSet*) ws->data("dataset");
  RooRealVar *dipho_mass=ws->var("dipho_mass");  
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *exp_ggh_yield=ws->var("exp_ggh_yield");
  RooRealVar *weight=ws->var("weight");

  //Fixing discriminant parameters
   RooRealVar *coef0_bern_bkg=ws->var("coef0_bern_bkg");
   coef0_bern_bkg->setConstant(1);
   RooRealVar *coef1_bern_bkg=ws->var("coef1_bern_bkg");
   coef1_bern_bkg->setConstant(1);
   RooRealVar *coef2_bern_bkg=ws->var("coef2_bern_bkg");
   coef2_bern_bkg->setConstant(1);
   RooRealVar *coef3_bern_bkg=ws->var("coef3_bern_bkg");
   coef3_bern_bkg->setConstant(1);


  RooRealVar *ggh_yield=ws->var("ggh_yield");
  RooRealVar *bkg_yield=ws->var("bkg_yield");

  model_gghbkg->fitTo(*dataset);  


//    RooRealVar *compo_ggh=ws->var("compo_ggh");
//    compo_ggh->setConstant(1);
   RooRealVar* mean1_ggh=ws->var("mean1_ggh");
   mean1_ggh->setConstant(1);
   RooRealVar* sigma1_ggh=ws->var("sigma1_ggh");
   sigma1_ggh->setConstant(1);
//    RooRealVar* mean2_ggh=ws->var("mean2_ggh");
//    mean2_ggh->setConstant(1);
//    RooRealVar* sigma2_ggh=ws->var("sigma2_ggh");
//    sigma2_ggh->setConstant(1);
 
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
  RooPlot* frame_mass=dipho_mass->frame(100,180,16); frame_mass->UseCurrentStyle();
  frame_mass->GetYaxis()->SetLabelSize(0.03);
  dataset->plotOn(frame_mass,Name("data"));
  model_gghbkg->plotOn(frame_mass,Name("full"));
  model_gghbkg->plotOn(frame_mass, Components("model_ggh"), LineColor(kGreen), LineStyle(kDashed),Name("ggh"));
  model_gghbkg->plotOn(frame_mass, Components("model_bkg"), LineColor(kRed), LineStyle(kDashed),Name("bkg"));
  frame_mass->Draw();
  latex.DrawLatex(0.45,0.86,"ggh yield");
  sprintf(buffer,"fitted = %3.0f",ggh_yield->getVal()); 
  latex.DrawLatex(0.45,0.81,buffer);
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
    sprintf(buffer,"%s%sframeDoSPlotMass_ggh%s%s_data.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
    canvas->SaveAs(buffer);
  }


  cout << "starting splot" << endl;
  RooDataSet *Wdataset=new RooDataSet(*dataset,"Wdataset");
  SPlot *splot_gghbkg=new SPlot("splot_gghbkg","splot_gghbkg", *Wdataset, model_gghbkg, RooArgList(*ggh_yield, *bkg_yield));  //Create splot
  ws->import(*Wdataset,Rename("Wdataset"));
  cout << ggh_yield->getVal() << endl;



  legend=new TLegend(0.7,0.7,1,1);
  legend->SetTextSize(0.04);
  frame_mass=dipho_mass->frame(110,180,35); frame_mass->UseCurrentStyle();
  frame_mass->GetYaxis()->SetLabelSize(0.03);
  model_gghbkg->plotOn(frame_mass,Name("full"));
  model_gghbkg->plotOn(frame_mass, Components("model_ggh"), LineColor(kGreen), LineStyle(kDashed),Name("ggh"));
  model_gghbkg->plotOn(frame_mass, Components("model_bkg"), LineColor(kRed), LineStyle(kDashed),Name("bkg"));
  frame_mass->Draw();
  latex.DrawLatex(0.45,0.86,"ggh yield");
  sprintf(buffer,"fitted = %3.0f",ggh_yield->getVal()); 
  latex.DrawLatex(0.45,0.81,buffer);
  legend->AddEntry(frame_mass->findObject("data"),"MC reco","lpe");
  legend->AddEntry(frame_mass->findObject("full"),"Full Fit","l");
  legend->AddEntry(frame_mass->findObject("ggh"),"ggh Fit","l");
  legend->AddEntry(frame_mass->findObject("bkg"),"bkg Fit","l");
  legend->Draw();
  canvas->SaveAs("frame_splot.pdf");
  

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
  RooRealVar *dipho_pt=ws->var("dipho_pt");
  RooRealVar *dipho_mass=ws->var("dipho_mass");
  RooRealVar *ggh_yield=ws->var("ggh_yield");
  RooDataSet *Wdataset=(RooDataSet*) ws->data("Wdataset");

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
  RooDataSet *sim_Wbkg=new RooDataSet("sim_Wbkg","sim_Wbkg",Wdataset,*Wdataset->get(),0,"bkg_yield_sw");
  RooDataSet *sim_Wggh=new RooDataSet("sim_Wggh","sim_Wggh",Wdataset,*Wdataset->get(),0,"ggh_yield_sw");
  sim_Wbkg->plotOn(frame_pt,MarkerColor(1),LineColor(1),DataError(RooAbsData::SumW2),Name("sim_Wbkg"),Binning(NBINS,0,dipho_pt->getMax()));// plot weighted ggh events
  //  model->plotOn(frame_pt,Components("model_bkg"),LineColor(4),Name("model_bkg"));// background component of the total fit
  sim_Wggh->plotOn(frame_pt,MarkerColor(2),LineColor(2),DataError(RooAbsData::SumW2),Name("sim_Wggh"),Binning(NBINS,0,dipho_pt->getMax()));// plot weighted ggh events
  //  model->plotOn(frame_pt,Components("model_ggh"),LineColor(3),Normalization(sim_Wbkg->sumEntries(),RooAbsReal::NumEvent),Name("model_ggh"));//ggh component of total fit 

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
    sprintf(buffer,"%s%ssplot_gghbkg%s%s_reco.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
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
  //  model->plotOn(frame_pt,Components("model_ggh"),LineColor(3),Normalization(sim_Wbkg->sumEntries(),RooAbsReal::NumEvent),Name("model_ggh"));//ggh component of total fit 
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
    sprintf(buffer,"%s%ssplot_ggh%s%s_reco.%s",buffer_path,buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
    canvas->SaveAs(buffer);
  }
  cout << "weigthed ggh drawn" << endl;

  cout << "end MakePlot" << endl;
  return 0;}
    
