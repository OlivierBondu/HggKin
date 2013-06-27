#include "include_root.h"
#include "include_roofit.h"
#include "param_kin.h"
#include "setTDRStyle.h"
#include <iostream>

using namespace std;

int main() {
   setTDRStyle();
  //#########MENU
  int const menu_bkg=0;//0:land; 1:logn; 
  int const menu_bkg_pol=1; // degree of polynome
  int const menu_ggh=0;//0:land; 1:logn; 
  int const menu_ggh_pol=1; // degree of polynome
  int const menu_vbf=0;//0:land; 1:logn; 2:tsallis
  int const menu_vbf_pol=1; // degree of polynome
  char const *menu_cut=""; //"", cuttheta{0.200,0.375,0.550,0.750}
  int const dosumm=0; // if 1, do the fit sgn+bkg

  TFile *file_result=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
  RooRealVar pt("pt","pt",0,200);

  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.05);
  char buffer[100];
  TLine *line=new TLine(0,1,200,1);
  line->SetLineColor(kRed);
  line->SetLineStyle(9);
  

  //########BKG
   
  TCanvas *canvas_bkg=new TCanvas("canvas_bkg","canvas_bkg");
  TPad *pad_fit_bkg=new TPad("pad_fit_bkg","pad_fit_bkg",0,0.3,1,1);
  TPad *ratio_pad_bkg=new TPad("ratio_pad_bkg","ratio_pad_bkg",0,0,1,0.3);
  pad_fit_bkg->SetBottomMargin(0.05);
  pad_fit_bkg->Draw();
  canvas_bkg->cd();
  ratio_pad_bkg->SetTopMargin(0);
  ratio_pad_bkg->Draw();
  
  RooPlot* frame_bkg=pt.frame();

RooRealVar mean_land_bkg("mean_land_bkg","mean_land_bkg",10,0,200);
  RooRealVar sigma_land_bkg("sigma_land_bkg","sigma_land_bkg",2,0,100);
  RooLandau *land_bkg=new RooLandau("land_bkg","land_bkg",pt,mean_land_bkg,sigma_land_bkg);

 RooRealVar coef0_logn_bkg("coef0_logn_bkg","coef0_logn_bkg",0,1,100);
  RooRealVar coef1_logn_bkg("coef1_logn_bkg","coef1_logn_bkg",1,1,10);
  RooRealVar coef2_logn_bkg("coef2_logn_bkg","coef2_logn_bkg",0,-30,30);
  RooRealVar coef3_logn_bkg("coef3_logn_bkg","coef3_logn_bkg",3,1,10);
  RooGenericPdf *logn_bkg=new RooGenericPdf("logn_bkg","TMath::LogNormal(pt,coef1_logn_bkg,coef2_logn_bkg,coef3_logn_bkg)", RooArgSet(pt,coef1_logn_bkg,coef2_logn_bkg,coef3_logn_bkg));


  RooRealVar coef0_pol_bkg("coef0_pol_bkg","coef0_pol_bkg",0,-100,100);
  RooRealVar coef1_pol_bkg("coef1_pol_bkg","coef1_pol_bkg",2,-100,100);
  RooRealVar coef2_pol_bkg("coef2_pol_bkg","coef2_pol_bkg",2,-100,100);
  RooRealVar coef3_pol_bkg("coef3_pol_bkg","coef3_pol_bkg",3,-100,100);
  RooPolynomial *pol2_bkg;

  sprintf(buffer,"hist_pt%s_bkg3_gen",menu_cut);
  cout << "before get" << endl;
  TH1F* hist_bkg=(TH1F*) file_result->Get(buffer);
  hist_bkg->Sumw2();
  cout << "got hist_bkg" << endl;
  //  hist_bkg->Sumw2();
  RooDataHist *bkg=new RooDataHist("bkg","bkg",pt,hist_bkg);
  bkg->plotOn(frame_bkg);

  RooProdPdf *model_bkg;
  char buffer_savebkg[100];

  switch (2*menu_bkg_pol+menu_bkg) {
  case 0 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglandpol0");
    break;  
    
  case 1 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglognpol0");
    break;  
    
  case 2 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglandpol1");
    break;  
  case 3 : 

    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglognpol1");
    break;  
  case 4 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg,coef2_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglandpol2");
    break;  
  case 5 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg,coef2_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglognpol2");
    break;  
  }


  model_bkg->fitTo(*bkg);
  model_bkg->plotOn(frame_bkg);


  TH1F *ratio_bkg=(TH1F*) model_bkg->createHistogram("ratio_bkg", pt,RooFit::Binning(hist_bkg->GetNbinsX(),0,200));
  
   ratio_bkg->Scale(hist_bkg->Integral());
  for (int i=0;i<hist_bkg->GetNbinsX();i++) {
    if (ratio_bkg->GetBinContent(i)>0) {
      ratio_bkg->SetBinContent(i,hist_bkg->GetBinContent(i)/ratio_bkg->GetBinContent(i));
    }}
  
  ratio_bkg->Sumw2();
  coef0_logn_bkg.setConstant(1);
  coef1_logn_bkg.setConstant(1);
  coef2_logn_bkg.setConstant(1);
  coef3_logn_bkg.setConstant(1);
  mean_land_bkg.setConstant(1);
  sigma_land_bkg.setConstant(1);
  coef0_pol_bkg.setConstant(1);
  coef1_pol_bkg.setConstant(1);
  coef2_pol_bkg.setConstant(1);
  
  ratio_pad_bkg->cd();
  //ratio_bkg->GetYaxis()->SetRangeUser(0,2);
  ratio_bkg->Draw("P");
  line->Draw("SAME");
  
  pad_fit_bkg->cd();
  //  pad_fit_bkg->SetLogy(1);
  frame_bkg->Draw();  
  

  sprintf(buffer,"bkg fit : #chi^{2}=%2.2f",frame_bkg->chiSquare());
  latex.DrawLatex(0.3,0.96,buffer);
  
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/fit%s_%s.png",menu_cut,buffer_savebkg);
  canvas_bkg->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit%s_%s.pdf",menu_cut,buffer_savebkg);
  canvas_bkg->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/fit%s_%s.root",menu_cut,buffer_savebkg);
  canvas_bkg->SaveAs(buffer);
  
  
    
  //########GGH
  TCanvas *canvas_ggh=new TCanvas("canvas_ggh","canvas_ggh");
  TPad *pad_fit_ggh=new TPad("pad_fit_ggh","pad_fit_ggh",0,0.3,1,1);
  TPad *ratio_pad_ggh=new TPad("ratio_pad_ggh","ratio_pad_ggh",0,0,1,0.3);
  pad_fit_ggh->SetBottomMargin(0.05);
  pad_fit_ggh->Draw();
  canvas_ggh->cd();
  ratio_pad_ggh->SetTopMargin(0);
  ratio_pad_ggh->Draw();
  
  RooPlot* frame_ggh=pt.frame();

  RooRealVar mean_land_ggh("mean_land_ggh","mean_land_ggh",10,0,200);
  RooRealVar sigma_land_ggh("sigma_land_ggh","sigma_land_ggh",2,0,100);
  RooLandau *land_ggh=new RooLandau("land_ggh","land_ggh",pt,mean_land_ggh,sigma_land_ggh);

  RooRealVar coef0_logn_ggh("coef0_logn_ggh","coef0_logn_ggh",0,1,100);
  RooRealVar coef1_logn_ggh("coef1_logn_ggh","coef1_logn_ggh",1,1,10);
  RooRealVar coef2_logn_ggh("coef2_logn_ggh","coef2_logn_ggh",0,-30,30);
  RooRealVar coef3_logn_ggh("coef3_logn_ggh","coef3_logn_ggh",3,1,10);
  RooGenericPdf *logn_ggh=new RooGenericPdf("logn_ggh","TMath::LogNormal(pt,coef1_logn_ggh,coef2_logn_ggh,coef3_logn_ggh)", RooArgSet(pt,coef1_logn_ggh,coef2_logn_ggh,coef3_logn_ggh));


  RooRealVar coef0_pol_ggh("coef0_pol_ggh","coef0_pol_ggh",0,-100,100);
  RooRealVar coef1_pol_ggh("coef1_pol_ggh","coef1_pol_ggh",2,-100,100);
  RooRealVar coef2_pol_ggh("coef2_pol_ggh","coef2_pol_ggh",2,-100,100);
  RooRealVar coef3_pol_ggh("coef3_pol_ggh","coef3_pol_ggh",3,-100,100);
  RooPolynomial *pol2_ggh;

  sprintf(buffer,"hist_pt%s_ggh_gen",menu_cut);
  TH1F* hist_ggh=(TH1F*) file_result->Get(buffer);
  hist_ggh->Sumw2();
  RooDataHist *ggh=new RooDataHist("ggh","ggh",pt,hist_ggh);
  ggh->plotOn(frame_ggh);

  RooProdPdf *model_ggh;

  char buffer_saveggh[100];
  switch (2*menu_ggh_pol+menu_ggh) {
  case 0 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",pt,RooArgList(coef0_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*land_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlandpol0");
    break;  
    
  case 1 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",pt,RooArgList(coef0_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*logn_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlognpol0");
    break;  
    
  case 2 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*land_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlandpol1");
    break;  
  case 3 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*logn_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlognpol1");
    break;  
  case 4 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh,coef2_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*land_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlandpol2");
    break;  
  case 5 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh,coef2_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*logn_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlognpol2");
    break;  
  }


  model_ggh->fitTo(*ggh);
  model_ggh->plotOn(frame_ggh);


  TH1F *ratio_ggh=(TH1F*)model_ggh->createHistogram("ratio_ggh", pt,RooFit::Binning(hist_ggh->GetNbinsX(),0,200));

  ratio_ggh->Scale(hist_ggh->Integral());
  for (int i=0;i<hist_ggh->GetNbinsX();i++) {
    if (ratio_ggh->GetBinContent(i)>0) {
      ratio_ggh->SetBinContent(i,hist_ggh->GetBinContent(i)/ratio_ggh->GetBinContent(i));
    }}
  
  ratio_ggh->Sumw2();
  coef0_logn_ggh.setConstant(1);
  coef1_logn_ggh.setConstant(1);
  coef2_logn_ggh.setConstant(1);
  coef3_logn_ggh.setConstant(1);
  mean_land_ggh.setConstant(1);
  sigma_land_ggh.setConstant(1);
  coef0_pol_ggh.setConstant(1);
  coef1_pol_ggh.setConstant(1);
  coef2_pol_ggh.setConstant(1);
  
  ratio_pad_ggh->cd();
  ratio_ggh->GetYaxis()->SetRangeUser(0,2);
  ratio_ggh->Draw("P");
  line->Draw("SAME");
  
  pad_fit_ggh->cd();
  //  pad_fit_ggh->SetLogy(1);
  frame_ggh->Draw();  
  

  sprintf(buffer,"ggh fit : #chi^{2}=%2.2f",frame_ggh->chiSquare());
  latex.DrawLatex(0.3,0.96,buffer);
  
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/fit%s_%s.png",menu_cut,buffer_saveggh);
  canvas_ggh->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit%s_%s.pdf",menu_cut,buffer_saveggh);
  canvas_ggh->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/fit%s_%s.root",menu_cut,buffer_saveggh);
  canvas_ggh->SaveAs(buffer);
    
  //########VBF
  TCanvas *canvas_vbf=new TCanvas("canvas_vbf","canvas_vbf");
  TPad *pad_fit_vbf=new TPad("pad_fit_vbf","pad_fit_vbf",0,0.3,1,1);
  TPad *ratio_pad_vbf=new TPad("ratio_pad_vbf","ratio_pad_vbf",0,0,1,0.3);
  pad_fit_vbf->SetBottomMargin(0.05);
  pad_fit_vbf->Draw();
  canvas_vbf->cd();
  ratio_pad_vbf->SetTopMargin(0);
  ratio_pad_vbf->Draw();
  
  RooPlot* frame_vbf=pt.frame();

  RooRealVar mean_land_vbf("mean_land_vbf","mean_land_vbf",10,0,200);
  RooRealVar sigma_land_vbf("sigma_land_vbf","sigma_land_vbf",2,0,100);
  RooLandau *land_vbf=new RooLandau("land_vbf","land_vbf",pt,mean_land_vbf,sigma_land_vbf);

  RooRealVar coef0_logn_vbf("coef0_logn_vbf","coef0_logn_vbf",0,1,100);
  RooRealVar coef1_logn_vbf("coef1_logn_vbf","coef1_logn_vbf",1,1,50);
  RooRealVar coef2_logn_vbf("coef2_logn_vbf","coef2_logn_vbf",0,-50,50);
  RooRealVar coef3_logn_vbf("coef3_logn_vbf","coef3_logn_vbf",3,1,50);
  RooGenericPdf *logn_vbf=new RooGenericPdf("logn_vbf","TMath::LogNormal(pt,coef1_logn_vbf,coef2_logn_vbf,coef3_logn_vbf)", RooArgSet(pt,coef1_logn_vbf,coef2_logn_vbf,coef3_logn_vbf));

  RooRealVar n("n","n",1,0.1,100);
  RooRealVar T("T","T",1,0.1,100);
  RooRealVar mass("mass","mass",100,0,200);
  RooGenericPdf *tsallis_vbf=new RooGenericPdf("tsallis_vbf","pt*pow(1+(sqrt(mass*mass+pt*pt)-mass)/n/T,-n)",RooArgSet(pt,n,T,mass));

  RooRealVar coef0_pol_vbf("coef0_pol_vbf","coef0_pol_vbf",0,-100,100);
  RooRealVar coef1_pol_vbf("coef1_pol_vbf","coef1_pol_vbf",2,-100,100);
  RooRealVar coef2_pol_vbf("coef2_pol_vbf","coef2_pol_vbf",2,-100,100);
  RooRealVar coef3_pol_vbf("coef3_pol_vbf","coef3_pol_vbf",3,-100,100);
  RooPolynomial *pol2_vbf;

  sprintf(buffer,"hist_pt%s_vbf_gen",menu_cut);
  TH1F* hist_vbf=(TH1F*) file_result->Get(buffer);
  hist_vbf->Sumw2();
  RooDataHist *vbf=new RooDataHist("vbf","vbf",pt,hist_vbf);
  vbf->plotOn(frame_vbf);

  RooProdPdf *model_vbf;

  char buffer_savevbf[100];
  switch (3*menu_vbf_pol+menu_vbf) {
  case 0 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",pt,RooArgList(coef0_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*land_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflandpol0");
    break;  
    
  case 1 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",pt,RooArgList(coef0_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*logn_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflognpol0");
    break;  
  case 2 :
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",pt,RooArgList(coef0_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*tsallis_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbftalpol0");
    break;  
  case 3 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*land_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflandpol1");
    break;  
  case 4 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*logn_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflognpol1");
    break;  
   case 5 :
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*tsallis_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbftalpol1");
    break;  
  case 6 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf,coef2_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*land_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflandpol2");
    break;  
  case 7 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf,coef2_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*logn_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflognpol2");
    break;  
 case 8 :
   pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf,coef2_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*tsallis_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbftalpol2");
    break;  
}


  model_vbf->fitTo(*vbf);
  model_vbf->plotOn(frame_vbf);


  TH1F *ratio_vbf=(TH1F*)model_vbf->createHistogram("ratio_vbf", pt,RooFit::Binning(hist_vbf->GetNbinsX(),0,200));
  cout << "bug creating ratio" << endl;
   ratio_vbf->Scale(hist_vbf->Integral());
  for (int i=0;i<hist_vbf->GetNbinsX();i++) {
    //cout << ratio_vbf->GetBinContent(i) << " " ;
    if (ratio_vbf->GetBinContent(i)>0) ratio_vbf->SetBinContent(i,hist_vbf->GetBinContent(i)/ratio_vbf->GetBinContent(i));
    //    cout << ratio_vbf->GetBinContent(i) << endl;

}
  
  ratio_vbf->Sumw2();
  coef0_logn_vbf.setConstant(1);
  coef1_logn_vbf.setConstant(1);
  coef2_logn_vbf.setConstant(1);
  coef3_logn_vbf.setConstant(1);
  mean_land_vbf.setConstant(1);
  sigma_land_vbf.setConstant(1);
  coef0_pol_vbf.setConstant(1);
  coef1_pol_vbf.setConstant(1);
  coef2_pol_vbf.setConstant(1);
  n.setConstant(1);
  T.setConstant(1);
  mass.setConstant(1);


  ratio_pad_vbf->cd();
  ratio_vbf->GetYaxis()->SetRangeUser(0,2);
  ratio_vbf->Draw("P");
  line->Draw("SAME");

  
  pad_fit_vbf->cd();
  pad_fit_vbf->SetLogy(1);
  frame_vbf->Draw();  
  

  sprintf(buffer,"vbf fit : #chi^{2}=%2.2f",frame_vbf->chiSquare());
  latex.DrawLatex(0.3,0.96,buffer);
  
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/fit%s_%s.png",menu_cut,buffer_savevbf);
  canvas_vbf->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit%s_%s.pdf",menu_cut,buffer_savevbf);
  canvas_vbf->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/fit%s_%s.root",menu_cut,buffer_savevbf);
  canvas_vbf->SaveAs(buffer);


  
  //###########################DATA
  if ( dosumm) {
  TCanvas *canvas_data=new TCanvas("canvas_data","canvas_data");
  TPad *pad_fit_data=new TPad("pad_fit_data","pad_fit_data",0,0.3,1,1);
  TPad *pad_ratio_data=new TPad("ratio_pad_data","ratio_pad_data",0,0,1,0.3);
  pad_fit_data->SetBottomMargin(0.05);
  pad_fit_data->Draw();
  canvas_data->cd();
  pad_ratio_data->SetTopMargin(0);
  pad_ratio_data->Draw();


  RooPlot *frame_dataggh=pt.frame();
  frame_dataggh->GetXaxis()->SetTitle("");
  RooPlot *frame_datavbf=pt.frame();
  frame_datavbf->GetXaxis()->SetTitle("");

  RooRealVar compo_sgn("compo_sgn","compo_sgn",0,0,1);
  RooAddPdf *model_data;
  //------------------------------------------
    

model_data=new RooAddPdf("model_data","model_data",RooArgList(*model_ggh,*model_bkg),compo_sgn);
  TH1F *hist_data=(TH1F*) hist_ggh->Clone();
  hist_data->Add(hist_bkg);
  hist_data->Sumw2();
  RooDataHist *data=new RooDataHist("data","data",pt,hist_data);
  model_data->fitTo(*data);

  TH1F *ratio_data=(TH1F*)model_data->createHistogram("ratio_data",pt,RooFit::Binning(hist_data->GetNbinsX(),0,200));

  ratio_data->Scale(hist_data->Integral());
  //ratio_data->Sumw2();
  for (int i=0;i<hist_data->GetNbinsX();i++) {
    if (ratio_data->GetBinContent(i)>0) ratio_data->SetBinContent(i,hist_data->GetBinContent(i)/ratio_data->GetBinContent(i));
  }

    ratio_data->Sumw2();
  data->plotOn(frame_dataggh);
  model_data->plotOn(frame_dataggh);
  model_data->plotOn(frame_dataggh,RooFit::Components("model_ggh"),RooFit::LineStyle(kDashed),RooFit::LineColor(kRed));  
  model_data->plotOn(frame_dataggh,RooFit::Components("model_bkg"),RooFit::LineStyle(kDashed),RooFit::LineColor(kGreen));  
  
  


  pad_ratio_data->cd();
  ratio_data->GetYaxis()->SetRangeUser(0,2);
  ratio_data->Draw("P");
  line->Draw();

  pad_fit_data->cd();
  //  pad_fit_data->SetLogy(1);
  frame_dataggh->Draw();  

  sprintf(buffer,"chi2=%2.2f",frame_dataggh->chiSquare());
  latex.DrawLatex(0.7,0.8,buffer);
  sprintf(buffer,"compo signal : %2.2e",compo_sgn.getVal());
  latex.DrawLatex(0.15,0.96,buffer);
  sprintf(buffer, "expected : %2.2e",hist_ggh->Integral()/hist_data->Integral());
  latex.DrawLatex(0.6,0.96,buffer);



  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/fit%s_data_%s_%s.png",menu_cut,buffer_savebkg,buffer_saveggh);    
  canvas_data->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit%s_data_%s_%s.pdf",menu_cut,buffer_savebkg,buffer_saveggh);
  canvas_data->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/fit%s_data_%s_%s.root",menu_cut,buffer_savebkg,buffer_saveggh);
  canvas_data->SaveAs(buffer);

  hist_data->Delete();
  data->Delete();
  ratio_data->Delete();
  model_data->Delete();  

//--------------------------------------

  model_data=new RooAddPdf("model_data","model_data",RooArgList(*model_vbf,*model_bkg),compo_sgn);
  hist_data=(TH1F*) hist_vbf->Clone();
  hist_data->Sumw2();
  hist_data->Add(hist_bkg);
  data=new RooDataHist("data","data",pt,hist_data);
  model_data->fitTo(*data);

  ratio_data=(TH1F*) model_data->createHistogram("ratio_data",pt,RooFit::Binning(hist_data->GetNbinsX(),0,200));

  ratio_data->Scale(hist_data->Integral());
  for (int i=0;i<hist_data->GetNbinsX();i++) {
    if (ratio_data->GetBinContent(i)>0) ratio_data->SetBinContent(i,hist_data->GetBinContent(i)/ratio_data->GetBinContent(i));
  }
  ratio_data->Sumw2();

  data->plotOn(frame_datavbf);
  model_data->plotOn(frame_datavbf);
  model_data->plotOn(frame_datavbf,RooFit::Components("model_vbf"),RooFit::LineStyle(kDashed),RooFit::LineColor(kRed));  
  model_data->plotOn(frame_datavbf,RooFit::Components("model_bkg"),RooFit::LineStyle(kDashed),RooFit::LineColor(kGreen));  
  
  


  pad_ratio_data->cd();
  ratio_data->GetYaxis()->SetRangeUser(0,2);
  ratio_data->Draw("P");
  line->Draw();

  pad_fit_data->cd();
  //  pad_fit_data->SetLogy(1);
  frame_datavbf->Draw();  

  sprintf(buffer,"chi2=%2.2f",frame_datavbf->chiSquare());
  latex.DrawLatex(0.7,0.8,buffer);
  sprintf(buffer,"compo signal : %2.2e",compo_sgn.getVal());
  latex.DrawLatex(0.15,0.96,buffer);
  sprintf(buffer, "expected : %2.2e",hist_vbf->Integral()/hist_data->Integral());
  latex.DrawLatex(0.6,0.96,buffer);



  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/fit%s_data_%s_%s.png",menu_cut,buffer_savebkg,buffer_savevbf);    
  canvas_data->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit%s_data_%s_%s.pdf",menu_cut,buffer_savebkg,buffer_savevbf);
  canvas_data->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/fit%s_data_%s_%s.root",menu_cut,buffer_savebkg,buffer_savevbf);
  canvas_data->SaveAs(buffer);
  }
  //Deleting all pointers
  
  
  
  file_result->Close();
  file_result->Delete();

return 0;
}
