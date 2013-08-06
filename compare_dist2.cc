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

 void compare_gen( char* const level,int const &norme, int const &window, int const &dology, int const &dostack, int const &cut);
 void  compare_reco(int const &norme, int const &window, int const &dology, int const &dostack, int const &cut, int const &categ); 
 // compare_gen(sim_level[1],0,2,0,0,-1);
 // compare_reco(0,2,0,0,-1,-1);

   for (int cut=-1; cut<n_cuttheta;cut++) {
     for (int dostack=0; dostack<2;dostack++) {
       for (int window=0;window<4;window++) {
	 for (int dology=0;dology<2;dology++) {
	   for (int norme=0;norme<2;norme++) {
	     if( (window==1) || (norme && dostack) ) continue;
	     compare_gen("gen",norme,window,dology,dostack,cut);
	     for (int categ=-1;categ<4;categ++) {
	       compare_reco(norme,window,dology,dostack,cut,categ);
	     }
	   }
	 }
       }
     }
   }

   cout << "Went up to the end" << endl;
 return 0;  
}




//#####################################################################
//####################################  GEN  #################################
//#######################################################################
void  compare_gen(char* const level, int const &norme, int const &window, int const &dology, int const &dostack, int const &cut) {
  cout << "in compare gen" << endl;
  setTDRStyle();
  gROOT->ForceStyle();
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
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
  TH1F *hist_proc[3][n_kinvar]={{0}};   
  for (int count_variable=0; count_variable<n_kinvar;count_variable++) {
    for (int count_process=0;count_process<3;count_process++){

      if (cut>-1  && count_variable<2) sprintf(buffer,"hist_%scuttheta%d_%s_%s",kinvarval[count_variable],cuttheta[cut],process[count_process],level); //if cut and no costheta
      else   sprintf(buffer,"hist_%s_%s_%s",kinvarval[count_variable],process[count_process],level);
      if (!count_variable && window) continue;// avoid mass distribution with mass cut
      hist_proc[count_process][count_variable]=(TH1F*) file_result->Get(buffer)->Clone();
      hist_proc[count_process][count_variable]->Rebin(4);
      if (norme) hist_proc[count_process][count_variable]->Scale(1/hist_proc[count_process][count_variable]->Integral());// if norme integral=1    
      
      //else cout << "didn't create mass_bkg" << window << endl;     
      
    }
  }
  THStack *stack=0;
  for (int count_variable=0;count_variable<n_kinvar;count_variable++ ) {
    if (!count_variable && window) continue;// avoid using the previously non created histograms
    if (count_variable==2 && cuttheta[cut]) continue;
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
    // legend->SetShadowColor(0);
//     legend->SetFillColor(0);
//     legend->SetBorderSize(0);
    legend->AddEntry((TObject *)0,"Generator","");
    sprintf(buffer,"yields  %1.2e",hist_proc[0][count_variable]->Integral()+hist_proc[1][count_variable]->Integral()+hist_proc[2][count_variable]->Integral());
    legend->AddEntry((TObject *)0,buffer,"");
    for (int counter_process=0;counter_process<3;counter_process++) {
      sprintf(buffer,"%s  %1.2e",process[counter_process],hist_proc[counter_process][count_variable]->Integral());    
      legend->AddEntry(hist_proc[counter_process][count_variable],buffer,"lpe");
    }
    legend->Draw();
    
    char logbuff[10]="",stackbuff[10]="",buffcut[30]="";
    if (dology)     sprintf(logbuff,"_logy");
    if (dostack)  sprintf(stackbuff,"_stack");
    if (cuttheta[cut]) sprintf(buffcut,"cuttheta%d",cuttheta[cut]);
    if (BATCH) {
      sprintf(buffer,"compare_process_%s%s_%s_win%d_norm%d%s%s.png",kinvarval[count_variable],buffcut,level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"compare_process_%s%s_%s_win%d_norm%d%s%s.pdf",kinvarval[count_variable],buffcut,level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"compare_process_%s%s_%s_win%d_norm%d%s%s.root",kinvarval[count_variable],buffcut,level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
    }
    else {
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/png/compare_process_%s%s_%s_win%d_norm%d%s%s.png",kinvarval[count_variable],buffcut,level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/pdf/compare_process_%s%s_%s_win%d_norm%d%s%s.pdf",kinvarval[count_variable],buffcut,level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
      sprintf(buffer,"/afs/cern.ch/work/c/cgoudet/private/plot/root/compare_process_%s%s_%s_win%d_norm%d%s%s.root",kinvarval[count_variable],buffcut,level,window,norme,stackbuff,logbuff);
      canvas->SaveAs(buffer);
    }
    
    stack->Delete();
  
  }
  canvas->Close();
  file_result->Close();
  cout << "compared gen " << buffer << endl;
}  



//#####################################################################
//####################################  RECO  #################################
//#######################################################################
void  compare_reco(int const &norme, int const &window, int const &dology, int const &dostack, int const &cut, int const &categ) {


  setTDRStyle();
  gROOT->ForceStyle();
  cout << "in compare reco" << endl;
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  TFile *file_result=0;
  if (BATCH) file_result=new TFile("kin_dist.root");
  else file_result=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
  

  //char for title 
  char  process[3][100]={"ggh","vbf","bkg"};
  if (window) {
    for (int i=0; i<3; i++) {
      sprintf(process[i],"%s%d",process[i],window);
    }
  }
  if (dology) canvas->SetLogy(1);    
  else canvas->SetLogy(0);


  TLatex latex;
  latex.SetNDC(1);
  latex.SetTextSize(0.03);
  char buffer[100]="",bufferpng[10]="",bufferpdf[10]="",bufferroot[10]="",bufferaddress[100]="",buffercut[20]="";;
  if (! BATCH) {
    sprintf(bufferaddress,"/afs/cern.ch/work/c/cgoudet/private/plot/");
    sprintf(bufferpng,"png/");
    sprintf(bufferpdf,"pdf/");
    sprintf(bufferroot,"root/");
  }	
  if (cut>-1) sprintf(buffercut,"cuttheta%d",cuttheta[cut]);

  //Create histograms [process][variable]
  cout << "create histogram" << endl;
  TH1F *hist_proc[n_kinvar][3]={{0}};   
  for (int kinvar=0; kinvar<n_kinvar;kinvar++) {
    for (int proc=0;proc<3;proc++) {
      if (cut>-1 && kinvar==2) continue;
      if (window && !kinvar) continue;
      
      if (cut==-1) sprintf(buffer,"hist_%s_",kinvarval[kinvar]);
      else sprintf(buffer,"hist_%scuttheta%d_",kinvarval[kinvar],cuttheta[cut]);
      
      if (categ<-0.5) sprintf(buffer,"%s%s_reco",buffer,process[proc]);
      else sprintf(buffer,"%scateg%d_%s_reco",buffer,categ,process[proc]);

      cout << buffer << endl;      

      hist_proc[kinvar][proc]=(TH1F*) file_result->Get(buffer)->Clone();
      hist_proc[kinvar][proc]->Rebin(4);
      if (norme) hist_proc[kinvar][proc]->Scale(1/hist_proc[kinvar][proc]->Integral());// if norme integral=1    
    }	
  }
  cout << "created" << endl;
  
  THStack *stack=0;
  for (int kinvar=0;kinvar<n_kinvar;kinvar++ ) {
    if (cut>-1 && kinvar==2) continue;
    if (window && !kinvar) continue;
    
    stack=new THStack("stack","stack");
    //Adding histograms to the stack
    for (int proc=2;proc>-1;proc--) { // Decreasing not to cut top of background
      hist_proc[kinvar][proc]->SetLineColor(proc+2);
      hist_proc[kinvar][proc]->SetMarkerColor(proc+2);      
      stack->Add(hist_proc[kinvar][proc]);
    }

    if (!dostack) stack->Draw("nostack");
    else stack->Draw();
    
    sprintf(buffer,"dN/d%s",kinvartitle[kinvar]);
    stack->GetYaxis()->SetTitle(buffer);
    stack->GetXaxis()->SetTitle(kinvartitle[kinvar]);

    TLegend *legend=new TLegend(0.8,0.8,1,1);
    //       legend->SetShadowColor(0);
    //       legend->SetFillColor(0);
    //       legend->SetBorderSize(0);
    cout << "added" << endl;    
    legend->AddEntry((TObject *) 0,"Reconstructed","");
    sprintf(buffer,"yields  %1.2e",hist_proc[kinvar][0]->Integral()+hist_proc[kinvar][1]->Integral()+hist_proc[kinvar][2]->Integral());
    legend->AddEntry((TObject *) 0,buffer,"");
    cout << "drawn" << endl;        


 for (int proc=0;proc<3;proc++) {
      sprintf(buffer,"%s  %1.2e",process[proc],hist_proc[kinvar][proc]->Integral());
      legend->AddEntry(hist_proc[kinvar][proc],buffer,"lpe");
    }

    legend->Draw();

    char logbuff[10]="",stackbuff[10]="",category[20]="";
    if (dology)     sprintf(logbuff,"_logy");
    if (dostack)  sprintf(stackbuff,"_stack");
    if (categ>-0.5) sprintf(category,"_categ%d",categ);


    sprintf(buffer,"%s%scompare_process_%s%s%s_reco_win%d_norm%d%s%s.png",bufferaddress,bufferpng,kinvarval[kinvar],buffercut,category,window,norme,stackbuff,logbuff);
    canvas->SaveAs(buffer);
    sprintf(buffer,"%s%scompare_process_%s%s%s_reco_win%d_norm%d%s%s.pdf",bufferaddress,bufferpdf,kinvarval[kinvar],buffercut,category,window,norme,stackbuff,logbuff);
    canvas->SaveAs(buffer);
    sprintf(buffer,"%s%scompare_process_%s%s%s_reco_win%d_norm%d%s%s.root",bufferaddress,bufferroot,kinvarval[kinvar],buffercut,category,window,norme,stackbuff,logbuff);
    canvas->SaveAs(buffer);
    
    stack->Delete();
  }


  canvas->Close();
  file_result->Close();
  cout << "compared reco " << buffer  << endl;
}  




