#include <iostream>
#include "param_kin.h"
#include "setTDRStyle.h"

#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TROOT.h"

#define BATCH 1

using namespace std;


int main() { 

 void compare_process( char* const level,int const &norme, int const &window, int const &dology, int const &dostack, int const &cut);
 char *sim_level[2]={"gen","reco"};

 //  compare_process(sim_level[0],1,0,1,1,200);
 
   for (int cut=-1; cut<n_cuttheta;cut++) {
     for (int dostack=0; dostack<2;dostack++) {
       for (int window=0;window<4;window++) {
	 for (int dology=0;dology<2;dology++) {
	   for (int norme=0;norme<2;norme++) {
	     if( !(window==1) && !(norme && dostack)) compare_process(sim_level[0],norme,window,dology,dostack,cut);
	   }
	 }
       }
     }
   }


 return 0;  
}




//#####################################################################
//#####################################################################
void  compare_process(char* const level, int const &norme, int const &window, int const &dology, int const &dostack, int const &cut) {
  setTDRStyle();
  gROOT->ForceStyle();
  TCanvas *canvas=new TCanvas("canvas","canvas");
  TFile *file_result=0;
  if (BATCH) file_result=new TFile("kin_dist.root");
  else file_result=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");

  //char for title 
  char  buffer_proc[10]="bkg";
  if (window) sprintf(buffer_proc,"bkg%d",window);
  char  *process[3]={"ggh","vbf",buffer_proc};


  if (dology) canvas->SetLogy(1);    
  else canvas->SetLogy(0);


  TLatex latex;
  latex.SetNDC(1);
  latex.SetTextSize(0.03);
  char buffer[100];
  //Create histograms [process][variable]
  cout << "create histogram" << endl;
  TH1F *hist_proc[3][n_kinvar]={{0}};   
  for (int count_variable=0; count_variable<n_kinvar;count_variable++) {
    for (int count_process=0;count_process<3;count_process++){
      if (cut>-1  && count_variable<2) sprintf(buffer,"hist_%scuttheta%d_%s_%s",kinvarval[count_variable],cuttheta[cut],process[count_process],level); //if cut and no costheta
      else   sprintf(buffer,"hist_%s_%s_%s",kinvarval[count_variable],process[count_process],level);
      cout << buffer << endl;
      if (count_variable || !window) {// avoid mass distribution with mass cut
	hist_proc[count_process][count_variable]=(TH1F*) file_result->Get(buffer)->Clone();
       	hist_proc[count_process][count_variable]->Rebin(4);
	if (norme) hist_proc[count_process][count_variable]->Scale(1/hist_proc[count_process][count_variable]->Integral());// if norme integral=1    
      }	
      //else cout << "didn't create mass_bkg" << window << endl;     
      
    }
  }
  cout << "before loop" << endl;
  THStack *stack=0;
  for (int count_variable=0;count_variable<n_kinvar;count_variable++ ) {
    if (count_variable || !window) {// avoid using the previously non created histograms
      stack=new THStack("stack","stack");
      //Adding histograms to the stack
      for (int count_process=2;count_process>-1;count_process--) { // Decreasing not to cut to of background
	hist_proc[count_process][count_variable]->SetLineColor(count_process+2);
	hist_proc[count_process][count_variable]->SetMarkerColor(count_process+2);      
	stack->Add(hist_proc[count_process][count_variable]);
      }
      
      if (!dostack) stack->Draw("nostack");
      else stack->Draw();

      sprintf(buffer,"dN/d%s",kinvartitle[count_variable]);
      stack->GetYaxis()->SetTitle(buffer);
      stack->GetXaxis()->SetTitle(kinvartitle[count_variable]);

      TLegend *legend=new TLegend(0.8,0.8,1,1);
      legend->SetShadowColor(0);
      legend->SetFillColor(0);
      legend->SetBorderSize(0);
    for (int counter_process=0;counter_process<3;counter_process++) {
      legend->AddEntry(hist_proc[counter_process][count_variable],process[counter_process],"lpe");
    }
    legend->Draw();

    char logbuff[10]="",stackbuff[10]="";
    if (dology)     sprintf(logbuff,"_logy");
    if (dostack)  sprintf(stackbuff,"_stack");

    if (BATCH) {
    if (!(cut==-1) && count_variable<n_kinvar-1) { // if   cut && no costheta
      sprintf(buffer,"compare_process_%scuttheta%d_%s_win%d_norm%d%s%s.png",kinvarval[count_variable],cuttheta[cut],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"compare_process_%scuttheta%d_%s_win%d_norm%d%s%s.pdf",kinvarval[count_variable],cuttheta[cut],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"compare_process_%scuttheta%d_%s_win%d_norm%d%s%s.root",kinvarval[count_variable],cuttheta[cut],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
    }
    else {
      sprintf(buffer,"compare_process_%s_%s_win%d_norm%d%s%s.png",kinvarval[count_variable],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"compare_process_%s_%s_win%d_norm%d%s%s.pdf",kinvarval[count_variable],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"compare_process_%s_%s_win%d_norm%d%s%s.root",kinvarval[count_variable],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
    }
    }
    else {
    if (!(cut==-1) && count_variable<n_kinvar-1) { // if   cut && no costheta
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/compare_process_%scuttheta%d_%s_win%d_norm%d%s%s.png",kinvarval[count_variable],cuttheta[cut],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/compare_process_%scuttheta%d_%s_win%d_norm%d%s%s.pdf",kinvarval[count_variable],cuttheta[cut],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/compare_process_%scuttheta%d_%s_win%d_norm%d%s%s.root",kinvarval[count_variable],cuttheta[cut],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
    }
    else {
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/compare_process_%s_%s_win%d_norm%d%s%s.png",kinvarval[count_variable],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/compare_process_%s_%s_win%d_norm%d%s%s.pdf",kinvarval[count_variable],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/compare_process_%s_%s_win%d_norm%d%s%s.root",kinvarval[count_variable],level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
    }
    }
    stack->Delete();
    }
 }
  canvas->Close();
  file_result->Close();
}  



