#include "include_roofit.h"
#include "include_root.h"
#include "setTDRStyle.h"
#include <fstream>
#include <iostream>

using namespace std;


int main() {
  int fit_bkg(int const &menu_bkg,int const &menu_pol_bkg,char const *menu_cut,int const &menu_window);
  int fit_ggh(int const &menu_ggh,int const &menu_pol_ggh,char const *menu_cut);
  int fit_vbf(int const &menu_vbf,int const &menu_pol_vbf,char const *menu_cut);

  char* cutval[5]={"","cuttheta0.200","cuttheta0.375","cuttheta0.550","cuttheta0.750"};
  fstream stream;
  stream.open("/afs/cern.ch/work/c/cgoudet/private/data/result_fit.txt",fstream::out|fstream::trunc);
  stream.close();


  for (int cut=0;cut<5;cut++) {
    for (int menu=0;menu<3;menu++) {
      for (int menu_pol=0;menu_pol<3;menu_pol++) {
	
	fit_bkg(menu,menu_pol,cutval[cut],3);
	fit_ggh(menu,menu_pol,cutval[cut]);
	fit_vbf(menu,menu_pol,cutval[cut]);
      }
    }
  }  

 
  return 0;
  
}
//#################################################################################################
//#################################################################################################
int fit_bkg(int const &menu_bkg,int const &menu_pol_bkg,char const *menu_cut,int const &menu_window){
  setTDRStyle();
  TFile *file_result=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
  RooRealVar pt("pt","pt",0,200);
  
  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.05);
  char buffer[100];
  TLine *line=new TLine(0,1,200,1);
  line->SetLineColor(kRed);
  line->SetLineStyle(9);

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
  RooRealVar coef1_logn_bkg("coef1_logn_bkg","coef1_logn_bkg",1,1,100);
  RooRealVar coef2_logn_bkg("coef2_logn_bkg","coef2_logn_bkg",0,-30,30);
  RooRealVar coef3_logn_bkg("coef3_logn_bkg","coef3_logn_bkg",3,1,100);
  RooGenericPdf *logn_bkg=new RooGenericPdf("logn_bkg","TMath::LogNormal(pt,coef1_logn_bkg,coef2_logn_bkg,coef3_logn_bkg)", RooArgSet(pt,coef1_logn_bkg,coef2_logn_bkg,coef3_logn_bkg));

  RooRealVar mass_bkg("mass_bkg","mass_bkg",125,0,200);
  RooRealVar n_bkg("n_bkg","n_bkg",1,0.1,100);
  RooRealVar T_bkg("T_bkg","T_bkg",1,0.1,100);
  RooGenericPdf *tallis_bkg=new RooGenericPdf("tallis_bkg","pt*pow(1+(sqrt(mass_bkg*mass_bkg+pt*pt)-mass_bkg)/n_bkg/T_bkg,-n_bkg)",RooArgSet(pt,n_bkg,mass_bkg,T_bkg));

  RooRealVar coef0_pol_bkg("coef0_pol_bkg","coef0_pol_bkg",0,-100,100);
  RooRealVar coef1_pol_bkg("coef1_pol_bkg","coef1_pol_bkg",2,-100,100);
  RooRealVar coef2_pol_bkg("coef2_pol_bkg","coef2_pol_bkg",2,-100,100);
  RooRealVar coef3_pol_bkg("coef3_pol_bkg","coef3_pol_bkg",3,-100,100);
  RooPolynomial *pol_bkg;

  if (menu_window)  sprintf(buffer,"hist_pt%s_bkg%s_gen",menu_cut,menu_window);
  else   sprintf(buffer,"hist_pt%s_bkg_gen",menu_cut);

  TH1F* hist_bkg=(TH1F*) file_result->Get(buffer);
  hist_bkg->Sumw2();
  RooDataHist *bkg=new RooDataHist("bkg","bkg",pt,hist_bkg);
  bkg->plotOn(frame_bkg);

  RooProdPdf *model_bkg;

  char buffer_savebkg[100];
  if (window) sprintf(buffer,"%d",window);
  else sprintf(buffer,"");

  switch (3*menu_pol_bkg+menu_bkg) {
  case 0 : 
   pol_bkg=new RooPolynomial("pol_bkg","pol_bkg",pt,RooArgList(coef0_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol_bkg));
    sprintf(buffer_savebkg,"bkg%slandpol0",buffer);
    break;  
  case 1 : 
    pol_bkg=new RooPolynomial("pol_bkg","pol_bkg",pt,RooArgList(coef0_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol_bkg));
    sprintf(buffer_savebkg,"bkg%slognpol0",buffer);
    break;  
  case 2 : 
    pol_bkg=new RooPolynomial("pol_bkg","pol_bkg",pt,RooArgList(coef0_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*tallis_bkg,*pol_bkg));
    sprintf(buffer_savebkg,"bkg%stallispol0",buffer);
    break;  
  case 3 : 
    pol_bkg=new RooPolynomial("pol_bkg","pol_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol_bkg));
    sprintf(buffer_savebkg,"bkg%slandpol1",buffer);
    break;  
  case 4 : 
    pol_bkg=new RooPolynomial("pol_bkg","pol_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol_bkg));
    sprintf(buffer_savebkg,"bkg%slognpol1",buffer);
    break;  
  case 5 : 
    pol_bkg=new RooPolynomial("pol_bkg","pol_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*tallis_bkg,*pol_bkg));
    sprintf(buffer_savebkg,"bkg%stallispol1",buffer);
    break;  
  case 6 : 
    pol_bkg=new RooPolynomial("pol_bkg","pol_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg,coef2_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol_bkg));
    sprintf(buffer_savebkg,"bkg%slandpol2",buffer);
    break;  
  case 7 : 
    pol_bkg=new RooPolynomial("pol_bkg","pol_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg,coef2_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol_bkg));
    sprintf(buffer_savebkg,"bkg%slognpol2",buffer);
    break;  
  case 8 : 
    pol_bkg=new RooPolynomial("pol_bkg","pol_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg,coef2_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*tallis_bkg,*pol_bkg));
    sprintf(buffer_savebkg,"bkg%stallispol2",buffer);
    break;  
  }


  model_bkg->fitTo(*bkg);
  model_bkg->plotOn(frame_bkg);


  TH1F *ratio_bkg=(TH1F*)model_bkg->createHistogram("ratio_bkg", pt,RooFit::Binning(hist_bkg->GetNbinsX(),0,200));

  ratio_bkg->Scale(hist_bkg->Integral());
  for (int i=0;i<hist_bkg->GetNbinsX();i++) {
    if (ratio_bkg->GetBinContent(i)>0) {
      ratio_bkg->SetBinContent(i,hist_bkg->GetBinContent(i)/ratio_bkg->GetBinContent(i));
    }}
  
 
  coef0_logn_bkg.setConstant(1);
  coef1_logn_bkg.setConstant(1);
  coef2_logn_bkg.setConstant(1);
  coef3_logn_bkg.setConstant(1);
  mean_land_bkg.setConstant(1);
  sigma_land_bkg.setConstant(1);
  coef0_pol_bkg.setConstant(1);
  coef1_pol_bkg.setConstant(1);
  coef2_pol_bkg.setConstant(1);
  mass_bkg.setConstant(1);
  n_bkg.setConstant(1);
  T_bkg.setConstant(1);
 
  ratio_pad_bkg->cd();
  ratio_bkg->GetYaxis()->SetRangeUser(0,2);
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

  sprintf(buffer,"%s_%s",buffer_savebkg,menu_cut);
  cout << "chi2 :" << endl;  
cout << buffer << " " << frame_bkg->chiSquare() << endl;
  fstream stream;
  stream.open("/afs/cern.ch/work/c/cgoudet/private/data/result_fit.txt",fstream::out|fstream::app);
  stream << buffer << " " << frame_bkg->chiSquare() << endl;
  stream.close();


  model_bkg->Delete();
  ratio_bkg->Delete();
  land_bkg->Delete();
  logn_bkg->Delete();
  pol_bkg->Delete();
  tallis_bkg->Delete();
  line->Delete();
  frame_bkg->Delete();
  file_result->Close();
  file_result->Delete();
  pad_fit_bkg->Delete();
  ratio_pad_bkg->Delete();
  canvas_bkg->Close();


  return 1;
  }

 

  //########GGH

  int fit_ggh(int const &menu_ggh,int const &menu_pol_ggh,char const *menu_cut){
  setTDRStyle();
  TFile *file_result=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
  RooRealVar pt("pt","pt",0,200);
  
  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.05);
  char buffer[100];
  TLine *line=new TLine(0,1,200,1);
  line->SetLineColor(kRed);
  line->SetLineStyle(9);

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
  RooRealVar coef1_logn_ggh("coef1_logn_ggh","coef1_logn_ggh",1,1,100);
  RooRealVar coef2_logn_ggh("coef2_logn_ggh","coef2_logn_ggh",0,-30,30);
  RooRealVar coef3_logn_ggh("coef3_logn_ggh","coef3_logn_ggh",3,1,100);
  RooGenericPdf *logn_ggh=new RooGenericPdf("logn_ggh","TMath::LogNormal(pt,coef1_logn_ggh,coef2_logn_ggh,coef3_logn_ggh)", RooArgSet(pt,coef1_logn_ggh,coef2_logn_ggh,coef3_logn_ggh));

  RooRealVar mass_ggh("mass_ggh","mass_ggh",125,0,200);
  RooRealVar n_ggh("n_ggh","n_ggh",1,0.1,100);
  RooRealVar T_ggh("T_ggh","T_ggh",1,0.1,100);
  RooGenericPdf *tallis_ggh=new RooGenericPdf("tallis_ggh","pt*pow(1+(sqrt(mass_ggh*mass_ggh+pt*pt)-mass_ggh)/n_ggh/T_ggh,-n_ggh)",RooArgSet(pt,n_ggh,mass_ggh,T_ggh));

  RooRealVar coef0_pol_ggh("coef0_pol_ggh","coef0_pol_ggh",0,-100,100);
  RooRealVar coef1_pol_ggh("coef1_pol_ggh","coef1_pol_ggh",2,-100,100);
  RooRealVar coef2_pol_ggh("coef2_pol_ggh","coef2_pol_ggh",2,-100,100);
  RooRealVar coef3_pol_ggh("coef3_pol_ggh","coef3_pol_ggh",3,-100,100);
  RooPolynomial *pol_ggh;

  sprintf(buffer,"hist_pt%s_ggh_gen",menu_cut);
  TH1F* hist_ggh=(TH1F*) file_result->Get(buffer);
  hist_ggh->Sumw2();
  RooDataHist *ggh=new RooDataHist("ggh","ggh",pt,hist_ggh);
  ggh->plotOn(frame_ggh);

  RooProdPdf *model_ggh;

  char buffer_saveggh[100];
  switch (3*menu_pol_ggh+menu_ggh) {
  case 0 : 
    pol_ggh=new RooPolynomial("pol_ggh","pol_ggh",pt,RooArgList(coef0_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*land_ggh,*pol_ggh));
    sprintf(buffer_saveggh,"gghlandpol0");
    break;  
  case 1 : 
    pol_ggh=new RooPolynomial("pol_ggh","pol_ggh",pt,RooArgList(coef0_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*logn_ggh,*pol_ggh));
    sprintf(buffer_saveggh,"gghlognpol0");
    break;  
  case 2 : 
    pol_ggh=new RooPolynomial("pol_ggh","pol_ggh",pt,RooArgList(coef0_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*tallis_ggh,*pol_ggh));
    sprintf(buffer_saveggh,"gghtallispol0");
    break;  
  case 3 : 
    pol_ggh=new RooPolynomial("pol_ggh","pol_ggh",pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*land_ggh,*pol_ggh));
    sprintf(buffer_saveggh,"gghlandpol1");
    break;  
  case 4 : 
    pol_ggh=new RooPolynomial("pol_ggh","pol_ggh",pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*logn_ggh,*pol_ggh));
    sprintf(buffer_saveggh,"gghlognpol1");
    break;  
  case 5 : 
    pol_ggh=new RooPolynomial("pol_ggh","pol_ggh",pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*tallis_ggh,*pol_ggh));
    sprintf(buffer_saveggh,"gghtallispol1");
    break;  
  case 6 : 
    pol_ggh=new RooPolynomial("pol_ggh","pol_ggh",pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh,coef2_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*land_ggh,*pol_ggh));
    sprintf(buffer_saveggh,"gghlandpol2");
    break;  
  case 7 : 
    pol_ggh=new RooPolynomial("pol_ggh","pol_ggh",pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh,coef2_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*logn_ggh,*pol_ggh));
    sprintf(buffer_saveggh,"gghlognpol2");
    break;  
  case 8 : 
    pol_ggh=new RooPolynomial("pol_ggh","pol_ggh",pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh,coef2_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*tallis_ggh,*pol_ggh));
    sprintf(buffer_saveggh,"gghtallispol2");
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
  
 
  coef0_logn_ggh.setConstant(1);
  coef1_logn_ggh.setConstant(1);
  coef2_logn_ggh.setConstant(1);
  coef3_logn_ggh.setConstant(1);
  mean_land_ggh.setConstant(1);
  sigma_land_ggh.setConstant(1);
  coef0_pol_ggh.setConstant(1);
  coef1_pol_ggh.setConstant(1);
  coef2_pol_ggh.setConstant(1);
  mass_ggh.setConstant(1);
  n_ggh.setConstant(1);
  T_ggh.setConstant(1);
 
  ratio_pad_ggh->cd();
  ratio_ggh->GetYaxis()->SetRangeUser(0,2);
  ratio_ggh->Draw("P");
  line->Draw("SAME");
  
  pad_fit_ggh->cd();
  //pad_fit_ggh->SetLogy(1);
  frame_ggh->Draw();  
  

  sprintf(buffer,"ggh fit : #chi^{2}=%2.2f",frame_ggh->chiSquare());
  latex.DrawLatex(0.3,0.96,buffer);
  
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/fit%s_%s.png",menu_cut,buffer_saveggh);
  canvas_ggh->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit%s_%s.pdf",menu_cut,buffer_saveggh);
  canvas_ggh->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/fit%s_%s.root",menu_cut,buffer_saveggh);
  canvas_ggh->SaveAs(buffer);

  sprintf(buffer,"%s_%s",buffer_saveggh,menu_cut);
  cout << "chi2 : " << endl;
  cout << buffer << " " << frame_ggh->chiSquare() << endl;
  fstream stream;
  stream.open("/afs/cern.ch/work/c/cgoudet/private/data/result_fit.txt",fstream::out|fstream::app);
  stream << buffer << " " << frame_ggh->chiSquare() << endl;
  stream.close();


  model_ggh->Delete();
  ratio_ggh->Delete();
  land_ggh->Delete();
  logn_ggh->Delete();
  pol_ggh->Delete();
  tallis_ggh->Delete();
  line->Delete();
  frame_ggh->Delete();
  file_result->Close();
  file_result->Delete();
  pad_fit_ggh->Delete();
  ratio_pad_ggh->Delete();
  canvas_ggh->Close();


  return 1;
  }

 
  //########VBF
  int fit_vbf(int const &menu_vbf,int const &menu_pol_vbf,char const *menu_cut){
  setTDRStyle();
  TFile *file_result=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
  RooRealVar pt("pt","pt",0,200);
  
  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.05);
  char buffer[100];
  TLine *line=new TLine(0,1,200,1);
  line->SetLineColor(kRed);
  line->SetLineStyle(9);

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
  RooRealVar coef1_logn_vbf("coef1_logn_vbf","coef1_logn_vbf",1,1,100);
  RooRealVar coef2_logn_vbf("coef2_logn_vbf","coef2_logn_vbf",0,-30,30);
  RooRealVar coef3_logn_vbf("coef3_logn_vbf","coef3_logn_vbf",3,1,100);
  RooGenericPdf *logn_vbf=new RooGenericPdf("logn_vbf","TMath::LogNormal(pt,coef1_logn_vbf,coef2_logn_vbf,coef3_logn_vbf)", RooArgSet(pt,coef1_logn_vbf,coef2_logn_vbf,coef3_logn_vbf));

  RooRealVar mass_vbf("mass_vbf","mass_vbf",125,0,200);
  RooRealVar n_vbf("n_vbf","n_vbf",1,0.1,100);
  RooRealVar T_vbf("T_vbf","T_vbf",1,0.1,100);
  RooGenericPdf *tallis_vbf=new RooGenericPdf("tallis_vbf","pt*pow(1+(sqrt(mass_vbf*mass_vbf+pt*pt)-mass_vbf)/n_vbf/T_vbf,-n_vbf)",RooArgSet(pt,n_vbf,mass_vbf,T_vbf));

  RooRealVar coef0_pol_vbf("coef0_pol_vbf","coef0_pol_vbf",0,-100,100);
  RooRealVar coef1_pol_vbf("coef1_pol_vbf","coef1_pol_vbf",2,-100,100);
  RooRealVar coef2_pol_vbf("coef2_pol_vbf","coef2_pol_vbf",2,-100,100);
  RooRealVar coef3_pol_vbf("coef3_pol_vbf","coef3_pol_vbf",3,-100,100);
  RooPolynomial *pol_vbf;

  sprintf(buffer,"hist_pt%s_vbf_gen",menu_cut);
  TH1F* hist_vbf=(TH1F*) file_result->Get(buffer);
  hist_vbf->Sumw2();
  RooDataHist *vbf=new RooDataHist("vbf","vbf",pt,hist_vbf);
  vbf->plotOn(frame_vbf);

  RooProdPdf *model_vbf;

  char buffer_savevbf[100];
  switch (3*menu_pol_vbf+menu_vbf) {
  case 0 : 
    pol_vbf=new RooPolynomial("pol_vbf","pol_vbf",pt,RooArgList(coef0_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*land_vbf,*pol_vbf));
    sprintf(buffer_savevbf,"vbflandpol0");
    break;  
  case 1 : 
    pol_vbf=new RooPolynomial("pol_vbf","pol_vbf",pt,RooArgList(coef0_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*logn_vbf,*pol_vbf));
    sprintf(buffer_savevbf,"vbflognpol0");
    break;  
  case 2 : 
    pol_vbf=new RooPolynomial("pol_vbf","pol_vbf",pt,RooArgList(coef0_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*tallis_vbf,*pol_vbf));
    sprintf(buffer_savevbf,"vbftallispol0");
    break;  
  case 3 : 
    pol_vbf=new RooPolynomial("pol_vbf","pol_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*land_vbf,*pol_vbf));
    sprintf(buffer_savevbf,"vbflandpol1");
    break;  
  case 4 : 
    pol_vbf=new RooPolynomial("pol_vbf","pol_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*logn_vbf,*pol_vbf));
    sprintf(buffer_savevbf,"vbflognpol1");
    break;  
  case 5 : 
    pol_vbf=new RooPolynomial("pol_vbf","pol_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*tallis_vbf,*pol_vbf));
    sprintf(buffer_savevbf,"vbftallispol1");
    break;  
  case 6 : 
    pol_vbf=new RooPolynomial("pol_vbf","pol_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf,coef2_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*land_vbf,*pol_vbf));
    sprintf(buffer_savevbf,"vbflandpol2");
    break;  
  case 7 : 
    pol_vbf=new RooPolynomial("pol_vbf","pol_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf,coef2_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*logn_vbf,*pol_vbf));
    sprintf(buffer_savevbf,"vbflognpol2");
    break;  
  case 8 : 
    pol_vbf=new RooPolynomial("pol_vbf","pol_vbf",pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf,coef2_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*tallis_vbf,*pol_vbf));
    sprintf(buffer_savevbf,"vbftallispol2");
    break;  
  }


  model_vbf->fitTo(*vbf);
  model_vbf->plotOn(frame_vbf);


  TH1F *ratio_vbf=(TH1F*)model_vbf->createHistogram("ratio_vbf", pt,RooFit::Binning(hist_vbf->GetNbinsX(),0,200));

  ratio_vbf->Scale(hist_vbf->Integral());
  for (int i=0;i<hist_vbf->GetNbinsX();i++) {
    if (ratio_vbf->GetBinContent(i)>0) {
      ratio_vbf->SetBinContent(i,hist_vbf->GetBinContent(i)/ratio_vbf->GetBinContent(i));
    }}
  
 
  coef0_logn_vbf.setConstant(1);
  coef1_logn_vbf.setConstant(1);
  coef2_logn_vbf.setConstant(1);
  coef3_logn_vbf.setConstant(1);
  mean_land_vbf.setConstant(1);
  sigma_land_vbf.setConstant(1);
  coef0_pol_vbf.setConstant(1);
  coef1_pol_vbf.setConstant(1);
  coef2_pol_vbf.setConstant(1);
  mass_vbf.setConstant(1);
  n_vbf.setConstant(1);
  T_vbf.setConstant(1);
 
  ratio_pad_vbf->cd();
  ratio_vbf->GetYaxis()->SetRangeUser(0,2);
  ratio_vbf->Draw("P");
  line->Draw("SAME");
  
  pad_fit_vbf->cd();
  //  pad_fit_vbf->SetLogy(1);
  frame_vbf->Draw();  
  

  sprintf(buffer,"vbf fit : #chi^{2}=%2.2f",frame_vbf->chiSquare());
  latex.DrawLatex(0.3,0.96,buffer);
  
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/fit%s_%s.png",menu_cut,buffer_savevbf);
  canvas_vbf->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit%s_%s.pdf",menu_cut,buffer_savevbf);
  canvas_vbf->SaveAs(buffer);
  sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/fit%s_%s.root",menu_cut,buffer_savevbf);
  canvas_vbf->SaveAs(buffer);

  sprintf(buffer,"%s_%s",buffer_savevbf,menu_cut);
  cout << "chi2 : " << endl;
  cout << buffer << " " << frame_vbf->chiSquare() << endl;
  fstream stream;
  stream.open("/afs/cern.ch/work/c/cgoudet/private/data/result_fit.txt",fstream::out|fstream::app);
  stream << buffer << " " << frame_vbf->chiSquare() << endl;
  stream.close();

  model_vbf->Delete();
  ratio_vbf->Delete();
  land_vbf->Delete();
  logn_vbf->Delete();
  pol_vbf->Delete();
  tallis_vbf->Delete();
  line->Delete();
  frame_vbf->Delete();
  file_result->Close();
  file_result->Delete();
  pad_fit_vbf->Delete();
  ratio_pad_vbf->Delete();
  canvas_vbf->Close();


  return 1;
  }



//################################################

