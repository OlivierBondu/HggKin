#include <iostream>
#include <fstream>
#include "include_root.h"
#include "param_kin.h"
#include "setTDRStyle.h"

#define norme 0 // 0 expected number of event; 1 normalized to 1
#define window 0 //{0,2,3} GeV centered on 125 ; 0:no cut
#define dology 1 
#define dostack 1
#define cut -1

using namespace std;

char * const type_data="gen";


int main() { 
  
  void compare_process(char *  const type);

  compare_process(type_data); 
    
    return 0;  
}




//#####################################################################
//#####################################################################
void  compare_process(char* const type) {
  setTDRStyle();
  gROOT->ForceStyle();

  TCanvas *canvas=new TCanvas("canvas","canvas");
  TFile *file_result=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");

  //char for title 
  char  buffer_proc[10];
  if (window) sprintf(buffer_proc,"bkg%d",window);
  else sprintf(buffer_proc,"bkg");
  char  *process[3]={"ggh","vbf",buffer_proc};
  char  *var_title[n_kinvar]={"mass","pt","cos(#theta*)"};


  if (dology) canvas->SetLogy(1);    
  else canvas->SetLogy(0);


  TLatex latex;
  latex.SetNDC(1);
  latex.SetTextSize(0.03);
  char buffer[100];
  //Create histograms [process][variable]
  TH1F *hist_proc[3][n_kinvar]={{0}};  
  
  for (int count_variable=0; count_variable<n_kinvar;count_variable++) {
    for (int count_process=0;count_process<3;count_process++){
      if (cut>-1  && count_variable<2) sprintf(buffer,"hist_%scuttheta%0.3f_%s_%s",kinvarval[count_variable],cuttheta[cut],process[count_process],type); //if cut and no costheta
      else   sprintf(buffer,"hist_%s_%s_%s",kinvarval[count_variable],process[count_process],type);
      cout << buffer << endl;
      if (count_variable || !window) {
	hist_proc[count_process][count_variable]=(TH1F*) file_result->Get(buffer)->Clone();
	hist_proc[count_process][count_variable]->Rebin(4);
	if (norme) hist_proc[count_process][count_variable]->Scale(1/hist_proc[count_process][count_variable]->Integral());// if norme integral=1    
      }	
      else cout << "didn't create mass_bkg" << window << endl;     
      
    }
  }

  THStack *stack=0;
  for (int count_variable=0;count_variable<n_kinvar;count_variable++ ) {
    if (count_variable || !window) {
      stack=new THStack("stack","stack");
      //Adding histograms to the stack
      for (int count_process=2;count_process>-1;count_process--) { // window
	hist_proc[count_process][count_variable]->SetLineColor(count_process+2);
	hist_proc[count_process][count_variable]->SetMarkerColor(count_process+2);      
	stack->Add(hist_proc[count_process][count_variable]);
      }
      
      if (!dostack) stack->Draw("nostack");
      else stack->Draw();

    stack->GetYaxis()->SetTitle("events");
    stack->GetXaxis()->SetTitle(var_title[count_variable]);

    for (int i=0;i<3;i++) {
      latex.SetTextColor(i+2);
      latex.DrawLatex(0.8,0.91-0.05*i,process[i]);

    }

    char logbuff[10]="",stackbuff[10]="";
    if (dology)     sprintf(logbuff,"_logy");
    if (dostack)  sprintf(stackbuff,"_stack");


    if (!(cut==-1) && count_variable<n_kinvar-1) { // if   cut && no costheta
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/compare_process_%scuttheta%0.3f_%s_win%d_norm%d%s%s.png",kinvarval[count_variable],cuttheta[cut],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/compare_process_%scuttheta%0.3f_%s_win%d_norm%d%s%s.pdf",kinvarval[count_variable],cuttheta[cut],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/compare_process_%scuttheta%0.3f_%s_win%d_norm%d%s%s.root",kinvarval[count_variable],cuttheta[cut],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
    }
    else {
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/compare_process_%s_%s_win%d_norm%d%s%s.png",kinvarval[count_variable],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/compare_process_%s_%s_win%d_norm%d%s%s.pdf",kinvarval[count_variable],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/compare_process_%s_%s_win%d_norm%d%s%s.root",kinvarval[count_variable],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
    }
    stack->Delete();
    }
 }
}  

