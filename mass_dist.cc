#include "setTDRStyle.h"

#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TAxis.h"

#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooPlot.h"

using namespace std;
using namespace RooFit;

int main() {

  void mass_dist(char* const level,int const categ);

  for (int cat=0;cat<5;cat++) {
  mass_dist("reco",cat);
  }
  cout << "Went up to the End" << endl;
  return 0;
}

void mass_dist(char* const level, int const categ) {
  setTDRStyle();

  TFile *file_data=new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root");
 
  RooRealVar *dipho_mass=new RooRealVar("dipho_mass","m_{#gamma #gamma}",100,180,"GeV/c^{2}");
  RooRealVar *category=new RooRealVar("category","category",-1,5);
  RooRealVar *weight=new RooRealVar("weight","weight",-1,5);

  char buffer[100];

  sprintf(buffer,"tree_%s_bkg",level);
  TTree *tree=(TTree*) file_data->Get(buffer);
  sprintf(buffer,"");
  if (categ<3.5) sprintf(buffer,"category>%2.2f && category <%2.2f",categ-0.1,categ+0.1);
  RooDataSet *mass_set=new RooDataSet("mass_set","mass_set",tree,RooArgSet(*dipho_mass,*category,*weight),buffer,"weight");
  tree->Delete();

  sprintf(buffer,"tree_%s_ggh",level);
  tree=(TTree*) file_data->Get(buffer);
  sprintf(buffer,"");
  if (categ<3.5) sprintf(buffer,"category>%2.2f && category <%2.2f",categ-0.1,categ+0.1);
  RooDataSet *dummy_set=new RooDataSet("dummy_set","dummy_set",tree,RooArgSet(*dipho_mass,*category,*weight),buffer,"weight");
  mass_set->append(*dummy_set);
  tree->Delete();
  dummy_set->Delete();

  sprintf(buffer,"tree_%s_vbf",level);
  tree=(TTree*) file_data->Get(buffer);
  sprintf(buffer,"");
  if (categ<3.5) sprintf(buffer,"category>%2.2f && category <%2.2f",categ-0.1,categ+0.1);
  dummy_set=new RooDataSet("dummy_set","dummy_set",tree,RooArgSet(*dipho_mass,*category,*weight),buffer,"weight");
  mass_set->append(*dummy_set);
  tree->Delete();
  dummy_set->Delete();

  RooPlot *frame=dipho_mass->frame();
  mass_set->plotOn(frame,Binning(40,100,180));
  frame->GetYaxis()->SetTitle("dN/dm_{#gamma#gamma}");
  TCanvas *canvas=new TCanvas("canvas","canvas",800,600);
  frame->Draw();

  char bufferpath[100],bufferpng[100],bufferpdf[100],bufferroot[100],buffercat[100];
  sprintf(bufferpath,"/afs/cern.ch/work/c/cgoudet/private/plot/");
  sprintf(bufferpdf,"pdf");
  sprintf(bufferpng,"png");
  sprintf(bufferroot,"root");
  sprintf(buffercat,"");
  if (categ<3.8 && !strcmp(level,"reco")) sprintf(buffercat,"_categ%d",categ);
  sprintf(buffer,"%s%s/mass_dist%s_reco.%s",bufferpath,bufferpdf,buffercat,bufferpdf);
  canvas->SaveAs(buffer);
  sprintf(buffer,"%s%s/mass_dist%s_reco.%s",bufferpath,bufferpng,buffercat,bufferpng);
  canvas->SaveAs(buffer);
  sprintf(buffer,"%s%s/mass_dist%s_reco.%s",bufferpath,bufferroot,buffercat,bufferroot);
  canvas->SaveAs(buffer);


  file_data->Close();
  canvas->Close();

cout  << "End mass_dist" << endl;
}
