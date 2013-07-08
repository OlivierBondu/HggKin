#include "include_roofit.h"
#include "include_root.h"
#include "setTDRStyle.h"
#include <iostream>
#include <fstream>
#include "RooDataSet.h"

using namespace std;


int main() {

  int fit_bkg(int const &menu_bkg,int const &menu_pol_bkg,char const *menu_cut,int const &menu_window);
  int fit_ggh(int const &menu_ggh,int const &menu_pol_ggh,char const *menu_cut);
  int fit_vbf(int const &menu_vbf,int const &menu_pol_vbf,char const *menu_cut);


  fstream stream;
  stream.open("result_fit.txt",fstream::out|fstream::trunc);
  stream.close();

  char* cutval[5]={"","200","375","550","750"};

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
  TFile *file_result=new TFile("kin_dist.root");
  RooRealVar dipho_pt("dipho_pt","dipho_pt",0,200);
  RooRealVar dipho_mass("dipho_mass","dipho_mass",0,600);
  RooRealVar dipho_ctheta("dipho_ctheta","dipho_ctheta",0,1);

  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.05);
  char buffer[100];
  TLine *line=new TLine(0,1,200,1);
  line->SetLineColor(kRed);
  line->SetLineStyle(9);


  TCanvas *canvas_bkg=new TCanvas("canvas_bkg","canvas_bkg");
  TPad *pad_fit_bkg=new TPad("pad_fit_bkg","pad_fit_bkg",0,0.3,1,1);
  pad_fit_bkg->SetBottomMargin(0.05);
  pad_fit_bkg->Draw();
  canvas_bkg->cd();
  TPad *ratio_pad_bkg=new TPad("ratio_pad_bkg","ratio_pad_bkg",0,0,1,0.3);  
  ratio_pad_bkg->SetTopMargin(0);
  ratio_pad_bkg->Draw();
  
  RooPlot* frame_bkg=dipho_pt.frame();

  RooRealVar mean_land_bkg("mean_land_bkg","mean_land_bkg",10,0,200);
  RooRealVar sigma_land_bkg("sigma_land_bkg","sigma_land_bkg",2,0,100);
  RooLandau *land_bkg=new RooLandau("land_bkg","land_bkg",dipho_pt,mean_land_bkg,sigma_land_bkg);

  RooRealVar coef1_logn_bkg("coef1_logn_bkg","coef1_logn_bkg",1,1,50);
  RooRealVar coef2_logn_bkg("coef2_logn_bkg","coef2_logn_bkg",0,-60,30);
  RooRealVar coef3_logn_bkg("coef3_logn_bkg","coef3_logn_bkg",3,1,50);
  RooGenericPdf *logn_bkg=new RooGenericPdf("logn_bkg","TMath::LogNormal(dipho_pt,coef1_logn_bkg,coef2_logn_bkg,coef3_logn_bkg)", RooArgSet(dipho_pt,coef1_logn_bkg,coef2_logn_bkg,coef3_logn_bkg));

  RooRealVar coefN_tsallis_bkg("coefN_tsallis_bkg","coefN_tsallis_bkg",30,2,100);
  RooRealVar coefT_tsallis_bkg("coefT_tsallis_bkg","coefT_tsallis_bkg",90,2,100);
  RooRealVar coefM_tsallis_bkg("coefM_tsallis_bkg","coefM_tsallis_bkg",180,0,210);
  RooGenericPdf *tsallis_bkg=new RooGenericPdf("tsallis_bkg","(coefN_tsallis_bkg-1)*(coefN_tsallis_bkg-2)/coefN_tsallis_bkg/coefT_tsallis_bkg/(coefN_tsallis_bkg*coefT_tsallis_bkg+coefM_tsallis_bkg*(coefN_tsallis_bkg-2))*dipho_pt*TMath::Power(1+(sqrt(coefM_tsallis_bkg*coefM_tsallis_bkg+dipho_pt*dipho_pt)-coefM_tsallis_bkg)/coefN_tsallis_bkg/coefT_tsallis_bkg,-coefN_tsallis_bkg)",RooArgSet(dipho_pt,coefN_tsallis_bkg,coefM_tsallis_bkg,coefT_tsallis_bkg));

  RooRealVar coef0_pol_bkg("coef0_pol_bkg","coef0_pol_bkg",0,-100,100);
  RooRealVar coef1_pol_bkg("coef1_pol_bkg","coef1_pol_bkg",2,-100,100);
  RooRealVar coef2_pol_bkg("coef2_pol_bkg","coef2_pol_bkg",2,-100,100);
  RooRealVar coef3_pol_bkg("coef3_pol_bkg","coef3_pol_bkg",3,-100,100);
  RooPolynomial *pol2_bkg;

  TTree *tree_bkg=(TTree*) file_result->Get("tree_bkg");
  tree_bkg->Print();
  pad_fit_bkg->cd();
  RooDataSet *dataset_bkg=0;
  if (menu_window && !strcmp(menu_cut,"")) {
    sprintf(buffer,"dipho_mass<%3.2f && dipho_mass > %3.2f", 125+menu_window/2.,125-menu_window/2.);
    dataset_bkg=new RooDataSet("dataset_bkg","dataset_bkg",tree_bkg,RooArgSet(dipho_pt,dipho_mass),buffer);
    tree_bkg->Draw("dipho_pt>>hist_bkg(300,0,200)",buffer);
	    }
  else if (menu_window && strcmp(menu_cut,"")) {
    sprintf(buffer,"dipho_mass<%3.2f && dipho_mass > %3.2f && dipho_ctheta>%s/1000.", 125+menu_window/2.,125-menu_window/2.,menu_cut);
    dataset_bkg=new RooDataSet("dataset_bkg","dataset_bkg",tree_bkg,RooArgSet(dipho_pt,dipho_mass,dipho_ctheta),buffer);
    tree_bkg->Draw("dipho_pt>>hist_bkg(300,0,200)",buffer);  
}
  else if (strcmp(menu_cut,"")) {
    sprintf(buffer,"dipho_ctheta>%s/1000.",menu_cut);
    dataset_bkg=new RooDataSet("dataset_bkg","dataset_bkg",tree_bkg,RooArgSet(dipho_pt,dipho_ctheta),buffer);
    tree_bkg->Draw("dipho_pt>>hist_bkg(300,0,200)",buffer);  
}
  else {
    dataset_bkg=new RooDataSet("dataset_bkg","dataset_bkg",tree_bkg,dipho_pt);
    tree_bkg->Draw("dipho_pt>>hist_bkg(300,0,200)");  
  }
  
dataset_bkg->plotOn(frame_bkg);
  RooProdPdf *model_bkg;


  char buffer_savebkg[100];
  switch (3*menu_pol_bkg+menu_bkg) {
  case 0 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",dipho_pt,RooArgList(coef0_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol2_bkg));
    if (menu_window)    sprintf(buffer_savebkg,"bkg%dlandpol0",menu_window);
    else sprintf(buffer_savebkg,"bkglandpol0");
    break;  
    
  case 1 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",dipho_pt,RooArgList(coef0_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol2_bkg));
    if (menu_window)    sprintf(buffer_savebkg,"bkg%dlognpol0",menu_window);
    else sprintf(buffer_savebkg,"bkglognpol0");
    break;  
  case 2 :
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",dipho_pt,RooArgList(coef0_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*tsallis_bkg,*pol2_bkg));
    if (menu_window)    sprintf(buffer_savebkg,"bkg%dtsallispol0",menu_window);
    else sprintf(buffer_savebkg,"bkgtsallispol0");
    break;  
  case 3 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",dipho_pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol2_bkg));
    if (menu_window)    sprintf(buffer_savebkg,"bkg%dlandpol1",menu_window);
    else sprintf(buffer_savebkg,"bkglandpol1");
    break;  
  case 4 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",dipho_pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglognpol1");
    if (menu_window)    sprintf(buffer_savebkg,"bkg%dlognpol1",menu_window);
    else sprintf(buffer_savebkg,"bkglognpol1");
    break;  
   case 5 :
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",dipho_pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*tsallis_bkg,*pol2_bkg));
    if (menu_window)     sprintf(buffer_savebkg,"bkg%dtsallispol1",menu_window);
    else     sprintf(buffer_savebkg,"bkgtsallispol1");
    break;  
  case 6 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",dipho_pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg,coef2_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol2_bkg));
    if (menu_window)     sprintf(buffer_savebkg,"bkg%dlandpol2",menu_window);
    else     sprintf(buffer_savebkg,"bkglandpol2");
    break;  
  case 7 : 
    pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",dipho_pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg,coef2_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol2_bkg));
    if (menu_window)     sprintf(buffer_savebkg,"bkg%dlognpol2",menu_window);
    else     sprintf(buffer_savebkg,"bkglognpol2");
    break;  
 case 8 :
   pol2_bkg=new RooPolynomial("pol2_bkg","pol2_bkg",dipho_pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg,coef2_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*tsallis_bkg,*pol2_bkg));
    if (menu_window)     sprintf(buffer_savebkg,"bkg%dtsallispol2",menu_window);
    else     sprintf(buffer_savebkg,"bkgtsallispol2");
    break;  
}


  model_bkg->fitTo(*dataset_bkg);
  model_bkg->plotOn(frame_bkg);
  cout << "plotted" << endl;  



  TH1F *hist_bkg=(TH1F*) gDirectory->Get("hist_bkg");
  hist_bkg->Sumw2();
  TH1F *ratio_bkg=(TH1F*) model_bkg->createHistogram("ratio_bkg", dipho_pt,RooFit::Binning(hist_bkg->GetNbinsX(),0,200));

  cout << ratio_bkg->GetNbinsX() << " " << hist_bkg->GetNbinsX() << endl;
  cout << "created ratio" << endl;
   ratio_bkg->Scale(hist_bkg->Integral());
  for (int i=0;i<hist_bkg->GetNbinsX()+1;i++) {

    //    cout << hist_bkg->GetBinCenter(i) << " " << ratio_bkg->GetBinContent(i) << " " << hist_bkg->GetBinContent(i) << " ";
    if (ratio_bkg->GetBinContent(i)>0) ratio_bkg->SetBinContent(i,hist_bkg->GetBinContent(i)/ratio_bkg->GetBinContent(i));
    //    cout << ratio_bkg->GetBinContent(i) << endl;

}
  cout << "modified ratio" << endl;
  ratio_bkg->Sumw2();
  coef1_logn_bkg.setConstant(1);
  coef2_logn_bkg.setConstant(1);
  coef3_logn_bkg.setConstant(1);
  mean_land_bkg.setConstant(1);
  sigma_land_bkg.setConstant(1);
  coef0_pol_bkg.setConstant(1);
  coef1_pol_bkg.setConstant(1);
  coef2_pol_bkg.setConstant(1);
  coefN_tsallis_bkg.setConstant(1);
  coefT_tsallis_bkg.setConstant(1);
  coefM_tsallis_bkg.setConstant(1);


  
  ratio_pad_bkg->cd();
  ratio_bkg->GetYaxis()->SetRangeUser(0,2);
  ratio_bkg->Draw("P");
  line->Draw("SAME");

  pad_fit_bkg->cd();
  //  pad_fit_bkg->SetLogy(1);
  frame_bkg->Draw();  


  //canvas_bkg->Update();
  if (!strcmp(menu_cut,"")) sprintf(buffer,"%s : #chi^{2}=%2.2f",buffer_savebkg,frame_bkg->chiSquare());
  else sprintf(buffer,"%s_cuttheta%s : #chi^{2}=%2.2f",buffer_savebkg,menu_cut,frame_bkg->chiSquare());
  latex.DrawLatex(0.25,0.96,buffer);

  char dummy_cut[40];
  if (strcmp(menu_cut,"")) sprintf(dummy_cut,"cuttheta%s",menu_cut);
  
  sprintf(buffer,"fit%s_%s.png",dummy_cut,buffer_savebkg);
  canvas_bkg->SaveAs(buffer);
  sprintf(buffer,"fit%s_%s.pdf",dummy_cut,buffer_savebkg);
  canvas_bkg->SaveAs(buffer);
  sprintf(buffer,"fit%s_%s.root",dummy_cut,buffer_savebkg);
  canvas_bkg->SaveAs(buffer);
  
  fstream stream;
  stream.open("result_fit.txt", fstream::out | fstream::app);
  stream << buffer_savebkg << "_" << dummy_cut << " " << frame_bkg->chiSquare() << endl;
  stream.close();

  file_result->Close();
  return 1;
  }


 
  //########GGH

  int fit_ggh(int const &menu_ggh,int const &menu_pol_ggh,char const *menu_cut){
  setTDRStyle();
  TFile *file_result=new TFile("kin_dist.root");
  RooRealVar dipho_pt("dipho_pt","dipho_pt",0,200);
  RooRealVar dipho_mass("dispho_mass","dipho_mass",100,0,600);  
  RooRealVar dipho_ctheta("dipho_ctheta","dipho_ctheta",0,1);
  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.05);
  char buffer[100];
  TLine *line=new TLine(0,1,200,1);
  line->SetLineColor(kRed);
  line->SetLineStyle(9);


  TCanvas *canvas_ggh=new TCanvas("canvas_ggh","canvas_ggh");
  TPad *pad_fit_ggh=new TPad("pad_fit_ggh","pad_fit_ggh",0,0.3,1,1);
  pad_fit_ggh->SetBottomMargin(0.05);
  pad_fit_ggh->Draw();
  canvas_ggh->cd();
  TPad *ratio_pad_ggh=new TPad("ratio_pad_ggh","ratio_pad_ggh",0,0,1,0.3);  
  ratio_pad_ggh->SetTopMargin(0);
  ratio_pad_ggh->Draw();
  
  RooPlot* frame_ggh=dipho_pt.frame();

  RooRealVar mean_land_ggh("mean_land_ggh","mean_land_ggh",10,0,200);
  RooRealVar sigma_land_ggh("sigma_land_ggh","sigma_land_ggh",2,0,100);
  RooLandau *land_ggh=new RooLandau("land_ggh","land_ggh",dipho_pt,mean_land_ggh,sigma_land_ggh);

  RooRealVar coef1_logn_ggh("coef1_logn_ggh","coef1_logn_ggh",1,1,50);
  RooRealVar coef2_logn_ggh("coef2_logn_ggh","coef2_logn_ggh",-30,-60,30);
  RooRealVar coef3_logn_ggh("coef3_logn_ggh","coef3_logn_ggh",3,1,50);
  RooGenericPdf *logn_ggh=new RooGenericPdf("logn_ggh","TMath::LogNormal(dipho_pt,coef1_logn_ggh,coef2_logn_ggh,coef3_logn_ggh)", RooArgSet(dipho_pt,coef1_logn_ggh,coef2_logn_ggh,coef3_logn_ggh));

  RooRealVar coefN_tsallis_ggh("coefN_tsallis_ggh","coefN_tsallis_ggh",30,2,100);
  RooRealVar coefT_tsallis_ggh("coefT_tsallis_ggh","coefT_tsallis_ggh",90,2,100);
  RooRealVar coefM_tsallis_ggh("coefM_tsallis_ggh","coefM_tsallis_ggh",180,0,210);
  RooGenericPdf *tsallis_ggh=new RooGenericPdf("tsallis_ggh","(coefN_tsallis_ggh-1)*(coefN_tsallis_ggh-2)/coefN_tsallis_ggh/coefT_tsallis_ggh/(coefN_tsallis_ggh*coefT_tsallis_ggh+coefM_tsallis_ggh*(coefN_tsallis_ggh-2))*dipho_pt*TMath::Power(1+(sqrt(coefM_tsallis_ggh*coefM_tsallis_ggh+dipho_pt*dipho_pt)-coefM_tsallis_ggh)/coefN_tsallis_ggh/coefT_tsallis_ggh,-coefN_tsallis_ggh)",RooArgSet(dipho_pt,coefN_tsallis_ggh,coefM_tsallis_ggh,coefT_tsallis_ggh));

  RooRealVar coef0_pol_ggh("coef0_pol_ggh","coef0_pol_ggh",0,-100,100);
  RooRealVar coef1_pol_ggh("coef1_pol_ggh","coef1_pol_ggh",2,-100,100);
  RooRealVar coef2_pol_ggh("coef2_pol_ggh","coef2_pol_ggh",2,-100,100);
  RooRealVar coef3_pol_ggh("coef3_pol_ggh","coef3_pol_ggh",3,-100,100);
  RooPolynomial *pol2_ggh;

  sprintf(buffer,"hist_dipho_pt%s_ggh_gen",menu_cut);
  TTree *tree_ggh=(TTree*) file_result->Get("tree_ggh");
  RooDataSet *dataset_ggh=0;
  pad_fit_ggh->cd();
  if (strcmp(menu_cut,"")) {
    sprintf(buffer,"dipho_ctheta > %s/1000.", menu_cut);
    dataset_ggh=new RooDataSet("dataset_ggh","dataset_ggh",tree_ggh,RooArgSet(dipho_pt,dipho_ctheta),buffer);
    tree_ggh->Draw("dipho_pt>>hist_ggh(300,0,200)",buffer);
  }
  else {
    dataset_ggh=new RooDataSet("dataset_ggh","dataset_ggh",tree_ggh,dipho_pt);
    tree_ggh->Draw("dipho_pt>>hist_ggh(300,0,200)");
  }

  dataset_ggh->plotOn(frame_ggh);

  RooProdPdf *model_ggh;

  char buffer_saveggh[100];
  switch (3*menu_pol_ggh+menu_ggh) {
  case 0 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",dipho_pt,RooArgList(coef0_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*land_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlandpol0");
    break;  
    
  case 1 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",dipho_pt,RooArgList(coef0_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*logn_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlognpol0");
    break;  
  case 2 :
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",dipho_pt,RooArgList(coef0_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*tsallis_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghtsallispol0");
    break;  
  case 3 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",dipho_pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*land_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlandpol1");
    break;  
  case 4 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",dipho_pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*logn_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlognpol1");
    break;  
   case 5 :
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",dipho_pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*tsallis_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghtsallispol1");
    break;  
  case 6 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",dipho_pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh,coef2_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*land_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlandpol2");
    break;  
  case 7 : 
    pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",dipho_pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh,coef2_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*logn_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghlognpol2");
    break;  
 case 8 :
   pol2_ggh=new RooPolynomial("pol2_ggh","pol2_ggh",dipho_pt,RooArgList(coef0_pol_ggh,coef1_pol_ggh,coef2_pol_ggh));
    model_ggh =new RooProdPdf("model_ggh","model_ggh",RooArgList(*tsallis_ggh,*pol2_ggh));
    sprintf(buffer_saveggh,"gghtsallispol2");
    break;  
}


  model_ggh->fitTo(*dataset_ggh);
  model_ggh->plotOn(frame_ggh);
  cout << "plotted" << endl;  


  TH1F *hist_ggh=(TH1F*) gDirectory->Get("hist_ggh");
  hist_ggh->Sumw2();
  TH1F *ratio_ggh=(TH1F*) model_ggh->createHistogram("ratio_ggh", dipho_pt,RooFit::Binning(hist_ggh->GetNbinsX(),0,200));
  
  cout << ratio_ggh->GetNbinsX() << " " << hist_ggh->GetNbinsX() << endl;
  cout << "created ratio" << endl;
  ratio_ggh->Scale(hist_ggh->Integral());
  for (int i=0;i<hist_ggh->GetNbinsX()+1;i++) {
    
    //    cout << hist_ggh->GetBinCenter(i) << " " << ratio_ggh->GetBinContent(i) << " " << hist_ggh->GetBinContent(i) << " ";
    if (ratio_ggh->GetBinContent(i)>0) ratio_ggh->SetBinContent(i,hist_ggh->GetBinContent(i)/ratio_ggh->GetBinContent(i));
    //    cout << ratio_ggh->GetBinContent(i) << endl;
    
}
  cout << "modified ratio" << endl;
  ratio_ggh->Sumw2();
  coef1_logn_ggh.setConstant(1);
  coef2_logn_ggh.setConstant(1);
  coef3_logn_ggh.setConstant(1);
  mean_land_ggh.setConstant(1);
  sigma_land_ggh.setConstant(1);
  coef0_pol_ggh.setConstant(1);
  coef1_pol_ggh.setConstant(1);
  coef2_pol_ggh.setConstant(1);
  coefN_tsallis_ggh.setConstant(1);
  coefT_tsallis_ggh.setConstant(1);
  coefM_tsallis_ggh.setConstant(1);


  
  ratio_pad_ggh->cd();
  ratio_ggh->GetYaxis()->SetRangeUser(0,2);
  ratio_ggh->Draw("P");
  line->Draw("SAME");

  pad_fit_ggh->cd();
  //  pad_fit_ggh->SetLogy(1);
  frame_ggh->Draw();  


  //canvas_ggh->Update();
  if (!strcmp(menu_cut,"")) sprintf(buffer,"%s : #chi^{2}=%2.2f",buffer_saveggh,frame_ggh->chiSquare());
  else sprintf(buffer,"%s_cuttheta%s : #chi^{2}=%2.2f",buffer_saveggh,menu_cut,frame_ggh->chiSquare());
  latex.DrawLatex(0.25,0.96,buffer);
  

  char dummy_cut[40]="";
  if (strcmp(menu_cut,""))   sprintf(dummy_cut,"cuttheta%s",menu_cut);
  sprintf(buffer,"fit%s_%s.png",dummy_cut,buffer_saveggh);
  canvas_ggh->SaveAs(buffer);
  sprintf(buffer,"fit%s_%s.pdf",dummy_cut,buffer_saveggh);
  canvas_ggh->SaveAs(buffer);
  sprintf(buffer,"fit%s_%s.root",dummy_cut,buffer_saveggh);
  canvas_ggh->SaveAs(buffer);


  fstream stream;
  stream.open("result_fit.txt",fstream::out|fstream::app);
  stream << buffer_saveggh << "_" << dummy_cut << " " << frame_ggh->chiSquare() << endl;
  stream.close();
  return 1;
  }

 
  //########VBF
  int fit_vbf(int const &menu_vbf,int const &menu_pol_vbf,char const *menu_cut){
  setTDRStyle();
  TFile *file_result=new TFile("kin_dist.root");
  RooRealVar dipho_pt("dipho_pt","dipho_pt",0,200);
  RooRealVar dipho_ctheta("dipho_ctheta","dipho_ctheta",0,1);  
  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.05);
  char buffer[100];
  TLine *line=new TLine(0,1,200,1);
  line->SetLineColor(kRed);
  line->SetLineStyle(9);

  TCanvas *canvas_vbf=new TCanvas("canvas_vbf","canvas_vbf");
  TPad *pad_fit_vbf=new TPad("pad_fit_vbf","pad_fit_vbf",0,0.3,1,1);
  pad_fit_vbf->SetBottomMargin(0.05);
  pad_fit_vbf->Draw();
  canvas_vbf->cd();
  TPad *ratio_pad_vbf=new TPad("ratio_pad_vbf","ratio_pad_vbf",0,0,1,0.3);  
  ratio_pad_vbf->SetTopMargin(0);
  ratio_pad_vbf->Draw();
  
  RooPlot* frame_vbf=dipho_pt.frame();

  RooRealVar mean_land_vbf("mean_land_vbf","mean_land_vbf",10,0,200);
  RooRealVar sigma_land_vbf("sigma_land_vbf","sigma_land_vbf",2,0,100);
  RooLandau *land_vbf=new RooLandau("land_vbf","land_vbf",dipho_pt,mean_land_vbf,sigma_land_vbf);

  RooRealVar coef1_logn_vbf("coef1_logn_vbf","coef1_logn_vbf",1,1,50);
  RooRealVar coef2_logn_vbf("coef2_logn_vbf","coef2_logn_vbf",0,-60,30);
  RooRealVar coef3_logn_vbf("coef3_logn_vbf","coef3_logn_vbf",3,1,50);
  RooGenericPdf *logn_vbf=new RooGenericPdf("logn_vbf","TMath::LogNormal(dipho_pt,coef1_logn_vbf,coef2_logn_vbf,coef3_logn_vbf)", RooArgSet(dipho_pt,coef1_logn_vbf,coef2_logn_vbf,coef3_logn_vbf));

  RooRealVar coefN_tsallis_vbf("coefN_tsallis_vbf","coefN_tsallis_vbf",30,2,100);
  RooRealVar coefT_tsallis_vbf("coefT_tsallis_vbf","coefT_tsallis_vbf",90,2,100);
  RooRealVar coefM_tsallis_vbf("coefM_tsallis_vbf","coefM_tsallis_vbf",180,0,210);
  RooGenericPdf *tsallis_vbf=new RooGenericPdf("tsallis_vbf","(coefN_tsallis_vbf-1)*(coefN_tsallis_vbf-2)/coefN_tsallis_vbf/coefT_tsallis_vbf/(coefN_tsallis_vbf*coefT_tsallis_vbf+coefM_tsallis_vbf*(coefN_tsallis_vbf-2))*dipho_pt*TMath::Power(1+(sqrt(coefM_tsallis_vbf*coefM_tsallis_vbf+dipho_pt*dipho_pt)-coefM_tsallis_vbf)/coefN_tsallis_vbf/coefT_tsallis_vbf,-coefN_tsallis_vbf)",RooArgSet(dipho_pt,coefN_tsallis_vbf,coefM_tsallis_vbf,coefT_tsallis_vbf));

  RooRealVar coef0_pol_vbf("coef0_pol_vbf","coef0_pol_vbf",0,-100,100);
  RooRealVar coef1_pol_vbf("coef1_pol_vbf","coef1_pol_vbf",2,-100,100);
  RooRealVar coef2_pol_vbf("coef2_pol_vbf","coef2_pol_vbf",2,-100,100);
  RooRealVar coef3_pol_vbf("coef3_pol_vbf","coef3_pol_vbf",3,-100,100);
  RooPolynomial *pol2_vbf;

  sprintf(buffer,"hist_dipho_pt%s_vbf_gen",menu_cut);
  TTree *tree_vbf=(TTree*) file_result->Get("tree_vbf");
  RooDataSet *dataset_vbf=0;
pad_fit_vbf->cd();

  if (strcmp(menu_cut,"")) {
    sprintf(buffer,"dipho_ctheta > %s/1000.", menu_cut);
    dataset_vbf=new RooDataSet("dataset_vbf","dataset_vbf",tree_vbf,RooArgSet(dipho_pt,dipho_ctheta),buffer);
    tree_vbf->Draw("dipho_pt>>hist_vbf(300,0,200)",buffer);
  }
  else {
    dataset_vbf=new RooDataSet("dataset_vbf","dataset_vbf",tree_vbf,dipho_pt);
    tree_vbf->Draw("dipho_pt>>hist_vbf(300,0,200)");
  }
  
  dataset_vbf->plotOn(frame_vbf);


  RooProdPdf *model_vbf;

  char buffer_savevbf[100];
  switch (3*menu_pol_vbf+menu_vbf) {
  case 0 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",dipho_pt,RooArgList(coef0_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*land_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflandpol0");
    break;  
    
  case 1 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",dipho_pt,RooArgList(coef0_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*logn_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflognpol0");
    break;  
  case 2 :
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",dipho_pt,RooArgList(coef0_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*tsallis_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbftsallispol0");
    break;  
  case 3 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",dipho_pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*land_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflandpol1");
    break;  
  case 4 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",dipho_pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*logn_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflognpol1");
    break;  
   case 5 :
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",dipho_pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*tsallis_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbftsallispol1");
    break;  
  case 6 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",dipho_pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf,coef2_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*land_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflandpol2");
    break;  
  case 7 : 
    pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",dipho_pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf,coef2_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*logn_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbflognpol2");
    break;  
 case 8 :
   pol2_vbf=new RooPolynomial("pol2_vbf","pol2_vbf",dipho_pt,RooArgList(coef0_pol_vbf,coef1_pol_vbf,coef2_pol_vbf));
    model_vbf =new RooProdPdf("model_vbf","model_vbf",RooArgList(*tsallis_vbf,*pol2_vbf));
    sprintf(buffer_savevbf,"vbftsallispol2");
    break;  
}


  model_vbf->fitTo(*dataset_vbf);
  model_vbf->plotOn(frame_vbf);
  cout << "plotted" << endl;  

  TH1F *hist_vbf=(TH1F*) gDirectory->Get("hist_vbf");
  hist_vbf->Sumw2();
  TH1F *ratio_vbf=(TH1F*) model_vbf->createHistogram("ratio_vbf", dipho_pt,RooFit::Binning(hist_vbf->GetNbinsX(),0,200));

  cout << ratio_vbf->GetNbinsX() << " " << hist_vbf->GetNbinsX() << endl;
  cout << "created ratio" << endl;
   ratio_vbf->Scale(hist_vbf->Integral());
  for (int i=0;i<hist_vbf->GetNbinsX()+1;i++) {

    //    cout << hist_vbf->GetBinCenter(i) << " " << ratio_vbf->GetBinContent(i) << " " << hist_vbf->GetBinContent(i) << " ";
    if (ratio_vbf->GetBinContent(i)>0) ratio_vbf->SetBinContent(i,hist_vbf->GetBinContent(i)/ratio_vbf->GetBinContent(i));
    //    cout << ratio_vbf->GetBinContent(i) << endl;

}
  cout << "modified ratio" << endl;
  ratio_vbf->Sumw2();
  coef1_logn_vbf.setConstant(1);
  coef2_logn_vbf.setConstant(1);
  coef3_logn_vbf.setConstant(1);
  mean_land_vbf.setConstant(1);
  sigma_land_vbf.setConstant(1);
  coef0_pol_vbf.setConstant(1);
  coef1_pol_vbf.setConstant(1);
  coef2_pol_vbf.setConstant(1);
  coefN_tsallis_vbf.setConstant(1);
  coefT_tsallis_vbf.setConstant(1);
  coefM_tsallis_vbf.setConstant(1);


  
  ratio_pad_vbf->cd();
  ratio_vbf->GetYaxis()->SetRangeUser(0,2);
  ratio_vbf->Draw("P");
  line->Draw("SAME");

  pad_fit_vbf->cd();
  //  pad_fit_vbf->SetLogy(1);
  frame_vbf->Draw();  


  //canvas_vbf->Update();
  if (!strcmp(menu_cut,"")) sprintf(buffer,"%s : #chi^{2}=%2.2f",buffer_savevbf,frame_vbf->chiSquare());
  else sprintf(buffer,"%s_%s : #chi^{2}=%2.2f",buffer_savevbf,menu_cut,frame_vbf->chiSquare());
  latex.DrawLatex(0.25,0.96,buffer);


  char dummy_cut[40]="";
  if (strcmp(menu_cut,"")) sprintf(dummy_cut,"cuttheta%s",menu_cut);  
  sprintf(buffer,"fit%s_%s.png",dummy_cut,buffer_savevbf);
  canvas_vbf->SaveAs(buffer);
  sprintf(buffer,"fit%s_%s.pdf",dummy_cut,buffer_savevbf);
  canvas_vbf->SaveAs(buffer);
  sprintf(buffer,"fit%s_%s.root",dummy_cut,buffer_savevbf);
  canvas_vbf->SaveAs(buffer);

 
  fstream stream;
  stream.open("result_fit.txt",fstream::out|fstream::app);
  stream << buffer_savevbf << "_" << dummy_cut << " " << frame_vbf->chiSquare() << endl;
  stream.close();


  return 1;
  }



//################################################

