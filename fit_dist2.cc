#include "setTDRStyle.h"
#include <iostream>
#include <fstream>
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "TTree.h"
#include "RooBernstein.h"
#include "RooGenericPdf.h"
#include "RooProdPdf.h"
#include "RooPlot.h"
#include "TLine.h"
#include "TFile.h"
#include "TCanvas.h"
#include "RooLandau.h"
#include "TPad.h"
#include "TH1F.h"
#include "TLatex.h"

#define NBINS 100
#define BATCH 1

using namespace std;



int main() {


  int fit_gen(char const *process,int const &menu,int const &menu_pol,char const *menu_cut,int const &window);
  int fit_reco(char const *process,int const &menu,int const &menu_pol,char const *menu_cut,int const &window,int const &categ);


  
  char* cutval[5]={"","200","375","550","750"};

  //  fit_gen("vbf",2,1,"",0);
  //  fit_reco("ggh",0,0,"",3,0);

  for (int cut=0;cut<5;cut++) {
    for (int menu=0;menu<3;menu++) {
      for (int menu_pol=0;menu_pol<3;menu_pol++) {
	//fit_gen("ggh",menu,menu_pol,cutval[cut],0);
	//fit_gen("vbf",menu,menu_pol,cutval[cut],0);
	for (int window=0; window<4;window++) {
	  if (window==1) continue;	  	
	  //fit_gen("bkg",menu,menu_pol,cutval[cut],window);
	  for (int categ=0;categ<3;categ++) {
	    //fit_reco("ggh",menu,menu_pol,cutval[cut],window,categ);
	    //fit_reco("vbf",menu,menu_pol,cutval[cut],window,categ);
	    fit_reco("bkg",menu,menu_pol,cutval[cut],window,categ);
	  }
	  for (int categ=3;categ<5;categ++) {
	    //fit_reco("ggh",menu,menu_pol,cutval[cut],window,categ);
	    //fit_reco("vbf",menu,menu_pol,cutval[cut],window,categ);
	    fit_reco("bkg",menu,menu_pol,cutval[cut],window,categ);
	  }
	}
      }
    }
   }  
   cout << "Went up to the end" << endl;   
 
  return 0;
}
  //##############################################################################################
  //#######################################  GEN  #######################################################
  //##############################################################################################

int fit_gen(char const *process,int const &menu,int const &menu_pol,char const *menu_cut, int const &window){
  cout << "in fit_gen" << endl;
  setTDRStyle();
  TFile *file_result=0;
  if (BATCH) file_result=new TFile("kin_dist.root");
  else file_result=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
  file_result->cd();
  RooRealVar dipho_pt("dipho_pt","p_{T #gamma #gamma}",0,200,"GeV");
  RooRealVar dipho_mass("dipho_mass","m_{#gamma#gamma}",100,180, "GeV");
  RooRealVar dipho_ctheta("dipho_ctheta","cos(#theta *)",0,1);
  RooRealVar weight("weight","weight",0,1);


  char tmp_process[100]="";
  sprintf(tmp_process,"%s",process);
  if (!strcmp(process,"bkg") && window) sprintf(tmp_process,"%s%d",tmp_process,window); 

  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.05);
  char buffer[200];
  TLine *line=new TLine(0,1,200,1);
  line->SetLineColor(kRed);
  line->SetLineStyle(9);


  TCanvas *canvas=new TCanvas("canvas","canvas");
  TPad *pad_fit=new TPad("pad_fit","pad_fit",0,0.3,1,1);
  pad_fit->SetBottomMargin(0.05);
  pad_fit->Draw();
  canvas->cd();
  TPad *ratio_pad=new TPad("ratio_pad","ratio_pad",0,0,1,0.3);  
  ratio_pad->SetTopMargin(0);
  ratio_pad->Draw();
  
  RooPlot* frame=dipho_pt.frame();

  RooRealVar mean_land("mean_land","mean_land",10,0,200);
  RooRealVar sigma_land("sigma_land","sigma_land",2,0,100);
  RooLandau *land=new RooLandau("land","land",dipho_pt,mean_land,sigma_land);

  RooRealVar coef1_logn("coef1_logn","coef1_logn",1,1,50);
  RooRealVar coef2_logn("coef2_logn","coef2_logn",-30,-60,30);
  RooRealVar coef3_logn("coef3_logn","coef3_logn",3,1,50);
  RooGenericPdf *logn=new RooGenericPdf("logn","TMath::LogNormal(dipho_pt,coef1_logn,coef2_logn,coef3_logn)", RooArgSet(dipho_pt,coef1_logn,coef2_logn,coef3_logn));

  RooRealVar coefN_tsallis("coefN_tsallis","coefN_tsallis",30,2,100);
  RooRealVar coefT_tsallis("coefT_tsallis","coefT_tsallis",90,2,100);
  RooRealVar coefM_tsallis("coefM_tsallis","coefM_tsallis",180,0,210);
  RooGenericPdf *tsallis=new RooGenericPdf("tsallis","(coefN_tsallis-1)*(coefN_tsallis-2)/coefN_tsallis/coefT_tsallis/(coefN_tsallis*coefT_tsallis+coefM_tsallis*(coefN_tsallis-2))*dipho_pt*TMath::Power(1+(sqrt(coefM_tsallis*coefM_tsallis+dipho_pt*dipho_pt)-coefM_tsallis)/coefN_tsallis/coefT_tsallis,-coefN_tsallis)",RooArgSet(dipho_pt,coefN_tsallis,coefM_tsallis,coefT_tsallis));

  RooRealVar coef0_pol("coef0_pol","coef0_pol",5,0,100);
  RooRealVar coef1_pol("coef1_pol","coef1_pol",2,0,100);
  RooRealVar coef2_pol("coef2_pol","coef2_pol",2,0,100);
  RooRealVar coef3_pol("coef3_pol","coef3_pol",3,0,100);
  RooBernstein *pol2;

  sprintf(buffer,"tree_gen_%s",process);
  TTree *tree=(TTree*) file_result->Get(buffer);
  tree->GetEntries();
  cout << "got tree" << endl;
  RooDataSet *dataset=0;
  pad_fit->cd();
  TH1F *hist=0;

  char buffer2[100];
  cout << "before sprintf" << endl;
  sprintf(buffer2,"");
  sprintf(buffer,"");


  if (strcmp(menu_cut,""))    {
    sprintf(buffer,"dipho_ctheta > %s/1000.", menu_cut);
    sprintf(buffer2," && ");
  }
  if (window)     {
    sprintf(buffer,"%s %s dipho_mass > 125-%d/2. && dipho_mass < 125+%d/2.",buffer,buffer2,window,window);
    sprintf(buffer2," && ");
  }



  dataset=new RooDataSet("dataset","dataset",tree,RooArgSet(dipho_mass,weight,dipho_mass,dipho_ctheta,dipho_pt),buffer,"weight");
  cout << "created dataset" << endl;
  sprintf(buffer2,"dipho_pt>>hist(%d,0,200)",NBINS);
  tree->Draw(buffer2,buffer);
  hist=(TH1F*) gDirectory->Get("hist");
  hist->Sumw2();  
  cout << "hist got" << endl;
  
  dataset->plotOn(frame);

  RooProdPdf *model;
  char buffer_save[100];
  switch (3*menu_pol+menu) {
  case 0 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol));
    model =new RooProdPdf("model","model",RooArgList(*land,*pol2));
    sprintf(buffer_save,"%slandpol0",tmp_process);
    break;  
    
  case 1 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol));
    model =new RooProdPdf("model","model",RooArgList(*logn,*pol2));
    sprintf(buffer_save,"%slognpol0",tmp_process);
    break;  
  case 2 :
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol));
    model =new RooProdPdf("model","model",RooArgList(*tsallis,*pol2));
    sprintf(buffer_save,"%stsallispol0",tmp_process);
    break;  
  case 3 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol));
    model =new RooProdPdf("model","model",RooArgList(*land,*pol2));
    sprintf(buffer_save,"%slandpol1",tmp_process);
    break;  
  case 4 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol));
    model =new RooProdPdf("model","model",RooArgList(*logn,*pol2));
    sprintf(buffer_save,"%slognpol1",tmp_process);
    break;  
   case 5 :
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol));
    model =new RooProdPdf("model","model",RooArgList(*tsallis,*pol2));
    sprintf(buffer_save,"%stsallispol1",tmp_process);
    break;  
  case 6 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol,coef2_pol));
    model =new RooProdPdf("model","model",RooArgList(*land,*pol2));
    sprintf(buffer_save,"%slandpol2",tmp_process);
    break;  
  case 7 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol,coef2_pol));
    model =new RooProdPdf("model","model",RooArgList(*logn,*pol2));
    sprintf(buffer_save,"%slognpol2",tmp_process);
    break;  
 case 8 :
   pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol,coef2_pol));
    model =new RooProdPdf("model","model",RooArgList(*tsallis,*pol2));
    sprintf(buffer_save,"%stsallispol2",tmp_process);
    break;  
}


  model->fitTo(*dataset);
  model->plotOn(frame);
  cout << "plotted" << endl;  
  cout << "test hist" << endl;


  TH1F *ratio=(TH1F*) model->createHistogram("ratio", dipho_pt,RooFit::Binning(hist->GetNbinsX(),0,200));
  cout << "created ratio" << endl;  

  cout << ratio->GetNbinsX() << " " << hist->GetNbinsX() << endl;
  
  ratio->Scale(hist->Integral());
  ratio->Sumw2();
  ratio->Divide(hist);
  cout << "modified ratio" << endl;
 
  
  coef1_logn.setConstant(1);
  coef2_logn.setConstant(1);
  coef3_logn.setConstant(1);
  mean_land.setConstant(1);
  sigma_land.setConstant(1);
  coef0_pol.setConstant(1);
  coef1_pol.setConstant(1);
  coef2_pol.setConstant(1);
  coefN_tsallis.setConstant(1);
  coefT_tsallis.setConstant(1);
  coefM_tsallis.setConstant(1);


  
  ratio_pad->cd();
  ratio->GetYaxis()->SetRangeUser(0,2);
  ratio->Draw("P");
  line->Draw("SAME");

  pad_fit->cd();
  //  pad_fit->SetLogy(1);
  frame->GetXaxis()->SetTitle("");  
  frame->Draw();  


  //canvas->Update();
  if (!strcmp(menu_cut,"")) sprintf(buffer,"%s_gen : #chi^{2}=%2.2f",buffer_save,frame->chiSquare());
  else sprintf(buffer,"%s_cuttheta%s_gen : #chi^{2}=%2.2f",buffer_save,menu_cut,frame->chiSquare());
  latex.DrawLatex(0.25,0.96,buffer);

  fstream stream; 
  char bufferpng[20]="",bufferpdf[20]="",bufferroot[20]="",bufferdata[20]="",bufferaddress[100]="";
  sprintf(buffer,""); 
  sprintf(buffer2,"");
  
  if (! BATCH) {
    sprintf(bufferaddress,"/afs/cern.ch/work/c/cgoudet/private/");
    sprintf(bufferpdf,"plot/pdf/");
    sprintf(bufferpng,"plot/png/");
    sprintf(bufferroot,"plot/root/");
    sprintf(bufferdata,"data/");
  }
  if (strcmp(menu_cut,""))   sprintf(buffer2,"_cuttheta%s",menu_cut);
  sprintf(buffer,"%s%sfit_%s%s_gen.png",bufferaddress,bufferpng,buffer_save,buffer2);
  canvas->SaveAs(buffer);
  sprintf(buffer,"%s%sfit_%s%s_gen.pdf",bufferaddress,bufferpdf,buffer_save,buffer2);
  canvas->SaveAs(buffer);
  sprintf(buffer,"%s%sfit_%s%s_gen.root",bufferaddress,bufferroot,buffer_save,buffer2);
  canvas->SaveAs(buffer);

  sprintf(buffer,"%s%sresult_fit_%s_gen.txt",bufferaddress,bufferdata,tmp_process);
  stream.open(buffer, fstream::out | fstream::app );
  stream << buffer_save  << buffer2 << " " << frame->chiSquare() << endl;
  stream.close();

  canvas->Delete();
  file_result->Close();
  cout << "end fit_gen" << endl;
  return 1;


  }
//##################################################################################################
//##########################################  RECO  ########################################################
//##################################################################################################
int fit_reco(char const *process,int const &menu,int const &menu_pol,char const *menu_cut, int const &window, int const &categ){
  cout << "in fit_reco" << endl;
  setTDRStyle();
  TFile *file_result=0;
  if (BATCH) file_result=new TFile("kin_dist.root");
  else file_result=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
  file_result->cd();
  RooRealVar dipho_pt("dipho_pt","p_{T #gamma #gamma}",0,200,"GeV");
  RooRealVar dipho_mass("dipho_mass","m_{#gamma#gamma}",100,180, "GeV");
  RooRealVar dipho_ctheta("dipho_ctheta","cos(#theta *)",0,1);
  RooRealVar weight("weight","weight",0,1);
  RooRealVar category("category","category",-1,6);

  char tmp_process[100]="";
  sprintf(tmp_process,"%s",process);
  if (window) sprintf(tmp_process,"%s%d",tmp_process,window); 


  char buffer[200];
  TCanvas *canvas=new TCanvas("canvas","canvas");
  TPad *pad_fit=new TPad("pad_fit","pad_fit",0,0.3,1,1);
  pad_fit->SetBottomMargin(0.05);
  pad_fit->Draw();
  canvas->cd();
  TPad *ratio_pad=new TPad("ratio_pad","ratio_pad",0,0,1,0.3);  
  ratio_pad->SetTopMargin(0);
  ratio_pad->Draw();
  
  RooPlot* frame=dipho_pt.frame();

  RooRealVar mean_land("mean_land","mean_land",10,0,200);
  RooRealVar sigma_land("sigma_land","sigma_land",2,0,100);
  RooLandau *land=new RooLandau("land","land",dipho_pt,mean_land,sigma_land);

  RooRealVar coef1_logn("coef1_logn","coef1_logn",1,1,50);
  RooRealVar coef2_logn("coef2_logn","coef2_logn",-30,-60,30);
  RooRealVar coef3_logn("coef3_logn","coef3_logn",3,1,50);
  RooGenericPdf *logn=new RooGenericPdf("logn","TMath::LogNormal(dipho_pt,coef1_logn,coef2_logn,coef3_logn)", RooArgSet(dipho_pt,coef1_logn,coef2_logn,coef3_logn));

  RooRealVar coefN_tsallis("coefN_tsallis","coefN_tsallis",30,2,100);
  RooRealVar coefT_tsallis("coefT_tsallis","coefT_tsallis",90,2,100);
  RooRealVar coefM_tsallis("coefM_tsallis","coefM_tsallis",180,0,210);
  RooGenericPdf *tsallis=new RooGenericPdf("tsallis","(coefN_tsallis-1)*(coefN_tsallis-2)/coefN_tsallis/coefT_tsallis/(coefN_tsallis*coefT_tsallis+coefM_tsallis*(coefN_tsallis-2))*dipho_pt*TMath::Power(1+(sqrt(coefM_tsallis*coefM_tsallis+dipho_pt*dipho_pt)-coefM_tsallis)/coefN_tsallis/coefT_tsallis,-coefN_tsallis)",RooArgSet(dipho_pt,coefN_tsallis,coefM_tsallis,coefT_tsallis));

  RooRealVar coef0_pol("coef0_pol","coef0_pol",5,0,100);
  RooRealVar coef1_pol("coef1_pol","coef1_pol",2,0,100);
  RooRealVar coef2_pol("coef2_pol","coef2_pol",2,0,100);
  RooRealVar coef3_pol("coef3_pol","coef3_pol",3,0,100);
  RooBernstein *pol2;

  cout << "before tree" << endl;
  sprintf(buffer,"tree_reco_%s",process);

  file_result->ls("tree*");
  TTree *tree=(TTree*) file_result->Get(buffer);
  tree->GetEntries();
  cout << "got tree" << endl;
  RooDataSet *dataset=0;
  pad_fit->cd();
  TH1F *hist=0;

  char buffer2[100];
  cout << "before sprintf" << endl;
  sprintf(buffer2,"");
  sprintf(buffer,"");


  if (strcmp(menu_cut,""))    {
    sprintf(buffer,"dipho_ctheta > %s/1000.", menu_cut);
    sprintf(buffer2," && ");
  }
  if (window)   {
    sprintf(buffer,"%s %s dipho_mass > 125-%d/2. && dipho_mass < 125+%d/2.",buffer,buffer2,window,window);
    sprintf(buffer2, " && ");
  }
  if (categ<3.5) sprintf(buffer,"%s %s category<%d+0.1 && category > %d-0.1",buffer,buffer2,categ,categ);


  dataset=new RooDataSet("dataset","dataset",tree,RooArgSet(dipho_mass,weight,dipho_mass,dipho_ctheta,dipho_pt,category),buffer,"weight");
  cout << "created dataset" << endl;
  sprintf(buffer2,"dipho_pt>>hist(%d,0,200)",NBINS);
  tree->Draw(buffer2,buffer);
  hist=(TH1F*) gDirectory->Get("hist");
  hist->Sumw2();  
  cout << "hist got" << endl;
  
  dataset->plotOn(frame);

  RooProdPdf *model;
  char buffer_save[100];
  switch (3*menu_pol+menu) {
  case 0 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol));
    model =new RooProdPdf("model","model",RooArgList(*land,*pol2));
    sprintf(buffer_save,"%slandpol0",tmp_process);
    break;  
    
  case 1 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol));
    model =new RooProdPdf("model","model",RooArgList(*logn,*pol2));
    sprintf(buffer_save,"%slognpol0",tmp_process);
    break;  
  case 2 :
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol));
    model =new RooProdPdf("model","model",RooArgList(*tsallis,*pol2));
    sprintf(buffer_save,"%stsallispol0",tmp_process);
    break;  
  case 3 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol));
    model =new RooProdPdf("model","model",RooArgList(*land,*pol2));
    sprintf(buffer_save,"%slandpol1",tmp_process);
    break;  
  case 4 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol));
    model =new RooProdPdf("model","model",RooArgList(*logn,*pol2));
    sprintf(buffer_save,"%slognpol1",tmp_process);
    break;  
   case 5 :
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol));
    model =new RooProdPdf("model","model",RooArgList(*tsallis,*pol2));
    sprintf(buffer_save,"%stsallispol1",tmp_process);
    break;  
  case 6 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol,coef2_pol));
    model =new RooProdPdf("model","model",RooArgList(*land,*pol2));
    sprintf(buffer_save,"%slandpol2",tmp_process);
    break;  
  case 7 : 
    pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol,coef2_pol));
    model =new RooProdPdf("model","model",RooArgList(*logn,*pol2));
    sprintf(buffer_save,"%slognpol2",tmp_process);
    break;  
 case 8 :
   pol2=new RooBernstein("pol2","pol2",dipho_pt,RooArgList(coef0_pol,coef1_pol,coef2_pol));
    model =new RooProdPdf("model","model",RooArgList(*tsallis,*pol2));
    sprintf(buffer_save,"%stsallispol2",tmp_process);
    break;  
}


  model->fitTo(*dataset);
  model->plotOn(frame);
  cout << "plotted" << endl;  
  cout << "test hist" << endl;


  TH1F *ratio=(TH1F*) model->createHistogram("ratio", dipho_pt,RooFit::Binning(hist->GetNbinsX(),0,200));
  cout << "created ratio" << endl;  

  cout << ratio->GetNbinsX() << " " << hist->GetNbinsX() << endl;
  
  ratio->Scale(hist->Integral());
  ratio->Sumw2();
  ratio->Divide(hist);
  cout << "modified ratio" << endl;
 
  
  coef1_logn.setConstant(1);
  coef2_logn.setConstant(1);
  coef3_logn.setConstant(1);
  mean_land.setConstant(1);
  sigma_land.setConstant(1);
  coef0_pol.setConstant(1);
  coef1_pol.setConstant(1);
  coef2_pol.setConstant(1);
  coefN_tsallis.setConstant(1);
  coefT_tsallis.setConstant(1);
  coefM_tsallis.setConstant(1);


  
  ratio_pad->cd();
  ratio->GetYaxis()->SetRangeUser(0,2);
  ratio->Draw("P");
  TLine *line=new TLine(0,1,200,1);  line->SetLineColor(kRed);  line->SetLineStyle(9);
  line->Draw("SAME");

  pad_fit->cd();
  //  pad_fit->SetLogy(1);
  frame->GetXaxis()->SetTitle("");  
  frame->Draw();  


  //canvas->Update();
  TLatex latex;  latex.SetNDC();  latex.SetTextSize(0.05);  
  if (!strcmp(menu_cut,"")) sprintf(buffer,"%s_reco : #chi^{2}=%2.2f",buffer_save,frame->chiSquare());
  else sprintf(buffer,"%s_cuttheta%s_reco : #chi^{2}=%2.2f",buffer_save,menu_cut,frame->chiSquare());
  latex.DrawLatex(0.25,0.96,buffer);
  
  fstream stream; 
  char bufferpng[20]="",bufferpdf[20]="",bufferroot[20]="",bufferdata[20]="",bufferaddress[100]="",buffercat[20]="";
  sprintf(buffer,"");sprintf(buffer2,"");
  if (! BATCH) {
    sprintf(bufferaddress,"/afs/cern.ch/work/c/cgoudet/private/");
    sprintf(bufferpdf,"plot/pdf/");
    sprintf(bufferpng,"plot/png/");
    sprintf(bufferroot,"plot/root/");
    sprintf(bufferdata,"data/");
  }
  if (strcmp(menu_cut,""))   sprintf(buffer2,"_cuttheta%s",menu_cut);
  if (categ<3.5) sprintf(buffercat,"_categ%d",categ);
  sprintf(buffer,"%s%sfit_%s%s%s_reco.png",bufferaddress,bufferpng,buffer_save,buffer2,buffercat);
  canvas->SaveAs(buffer);
  sprintf(buffer,"%s%sfit_%s%s%s_reco.pdf",bufferaddress,bufferpdf,buffer_save,buffer2,buffercat);
  canvas->SaveAs(buffer);
  sprintf(buffer,"%s%sfit_%s%s%s_reco.root",bufferaddress,bufferroot,buffer_save,buffer2,buffercat);
  canvas->SaveAs(buffer);

  sprintf(buffer,"%s%sresult_fit_%s%s_reco.txt",bufferaddress,bufferdata,tmp_process,buffercat);
  stream.open(buffer, fstream::out | fstream::app );
  stream << buffer_save  << buffer2 << " " << frame->chiSquare() << endl;
  stream.close();

  canvas->Delete();
  file_result->Close();
  cout << "end fit_reco" << endl;
  return 1;}
  

