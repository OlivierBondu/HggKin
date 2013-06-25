#include <iostream>
#include "include_root.h"
#include "param_kin.cc"



using namespace std;
TCanvas *canvas=new TCanvas("canvas","canvas");
TFile *file_result=new TFile("../data/kin_dist.root");



int compare_dist2() { 

 void compare_process( char* const type,int const norme, int const window, int const dology, int const dostack, int const cut);
  for (int cut=-1;cut<n_cuttheta;cut++) {
  for (int dostack=0; dostack<2;dostack++) {
    for (int window=0;window<4;window++) {
      for (int dology=0;dology<2;dology++) {
	for (int norme=0;norme<2;norme++) {
	  if( !(window==1) && !(norme && dostack)) compare_process("gen",norme,window,dology,dostack,cut);
	}
      }}
    }
  }
  file_result->Close();  
  return 0;  
}




//#####################################################################
//#####################################################################
void compare_process(char* const type,int const norme, int const window, int const dology,int const dostack, int const cut) {
  char buffer_proc[100];
  sprintf(buffer_proc,"bkg%d",window);
  char *process[3]={"ggh","vbf",buffer_proc};
  char *var[2]={"pt","costheta"};
  char *var_title[2]={"pt","cos(#theta*)"};

  if (dology) canvas->SetLogy(1);    
  else canvas->SetLogy(0);

  TLatex latex;
  latex.SetNDC(1);
  latex.SetTextSize(0.03);
  char buffer[100];


  TH1F *hist_proc[3][2]={{0}};  
  for (int i=0;i<3;i++){
    for (int j=0; j<2;j++) {
      if (cut>-1 && !(i==1) && j==0) sprintf(buffer,"hist_%scuttheta%0.3f_%s_%s",var[j],cuttheta[cut],process[i],type);
      else   sprintf(buffer,"hist_%s_%s_%s",var[j],process[i],type);
      hist_proc[i][j]=(TH1F*) file_result->Get(buffer)->Clone();
      hist_proc[i][j]->Rebin(5);
      if (norme) hist_proc[i][j]->Scale(1/hist_proc[i][j]->Integral());
    }
  }


  THStack *stack=0;
 

  for (int j=0;j<2;j++) {
    stack=    new THStack("stack","stack");
    
    for (int i=2;i>-1;i--) {
      hist_proc[i][j]->SetLineColor(i+2);
      hist_proc[i][j]->SetMarkerColor(i+2);
      stack->Add(hist_proc[i][j]);
      
    }

    if (!dostack) stack->Draw("nostack");
    else stack->Draw();


    stack->GetYaxis()->SetTitle("events");
    //    stack->GetYaxis()->SetTitleOffset(0.9);
    stack->GetXaxis()->SetTitle(var_title[j]);

    for (int i=0;i<3;i++) {
      latex.SetTextColor(i+2);
      latex.DrawLatex(0.25+i*0.2,0.91,process[i]);
    }

    char logbuff[10]="",stackbuff[10]="";    
    if (dology)    sprintf(logbuff,"_logy");
    if (dostack) sprintf(stackbuff , "_stack");
    

    if (!(cut==-1) && j==0) {
      sprintf(buffer,"../plot/png/compare_process_%scuttheta%0.3f_%s_win%d_norm%d%s%s.png",var[j],cuttheta[cut],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"../plot/pdf/compare_process_%scuttheta%0.3f_%s_win%d_norm%d%s%s.pdf",var[j],cuttheta[cut],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"../plot/root/compare_process_%scuttheta%0.3f_%s_win%d_norm%d%s%s.root",var[j],cuttheta[cut],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
    }
    else {
      sprintf(buffer,"../plot/png/compare_process_%s_%s_win%d_norm%d%s%s.png",var[j],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"../plot/pdf/compare_process_%s_%s_win%d_norm%d%s%s.pdf",var[j],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"../plot/root/compare_process_%s_%s_win%d_norm%d%s%s.root",var[j],type,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
    }

  }

}
  



