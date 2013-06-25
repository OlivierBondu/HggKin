#include "include_root.h"
#include <fstream>
#include "param_kin.h"
#include <iostream>
using namespace std;

#define GGH_GEN 1
#define VBF_GEN 1
#define BKG_GEN 1
#define GGH_REC 0
#define VBF_REC 0
#define BKG_REC 0


int main() {


  fstream stream_integral;
  stream_integral.open("../data/hist_integral.txt",fstream::out);
  //cout << "started" << endl;  

  //################################################
  //  gROOT->ProcessLine(".L param_kin.cc");

  
  TFile *file=0;
  TFile *file_result = new TFile("../data/kin_dist.root","UPDATE");
  
  TTree *tree=0;

  

  //Creating variables for the analysis
  TLorentzVector *gamma1=new TLorentzVector();
  TLorentzVector *gamma2=new TLorentzVector();
  TLorentzVector *gamma_pair=new TLorentzVector();
    TH1F *hist_ptcuttheta[n_cuttheta]={0};
  
  int nentry=0;
  char buffer [100],dummy[100];
  char const *gen_variables[8]={"gh_g1_p4_pt","gh_g1_p4_eta","gh_g1_p4_phi","gh_g1_p4_mass","gh_g2_p4_pt","gh_g2_p4_eta","gh_g2_p4_phi","gh_g2_p4_mass"};//useful variables
  float variables[8];//g1_pt,g1_eta,g1_phi,g1_mass,g2_pt,g2_eta,g2_phi,g2_mass;
  float costeta;


  float GetCosTheta(TLorentzVector *g1, TLorentzVector *g2) ;



  //#######################Analyse of the ggh_gen
  if (GGH_GEN) {
    file = new TFile("../data/SMHiggs_m125.root");
    file_result->cd();
    tree  =(TTree *) file->Get("ggh_m125_8TeV");
   
 
    TH1F *hist_ggh_gen[1][n_kinvar];
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      sprintf(dummy,"hist_%s_ggh_gen",kinvarval[kinvar]);
      hist_ggh_gen[0][kinvar]=new TH1F(dummy,dummy,200,0,200-199*kinvar);
      hist_ggh_gen[0][kinvar]->Sumw2();
      hist_ggh_gen[0][kinvar]->GetXaxis()->SetTitle(kinvarval[kinvar]);
      hist_ggh_gen[0][kinvar]->GetYaxis()->SetTitle("events");
    }
    

    for (int i=0;i<n_cuttheta;i++) {
      sprintf(dummy,"hist_ptcuttheta%1.3f_ggh_gen",cuttheta[i]);
      hist_ptcuttheta[i]=new TH1F(dummy,dummy,200,0,200);
      hist_ptcuttheta[i]->Sumw2();
      hist_ptcuttheta[i]->GetXaxis()->SetTitle("pt");
      hist_ptcuttheta[i]->GetYaxis()->SetTitle("events");
    }
    
    
    nentry=tree->GetEntries();
    tree->SetBranchStatus("*",0);// selection of useful branches
    for (int i=0; i<8; i++) {
      sprintf(buffer,"%s",gen_variables[i]);
      tree->SetBranchStatus(buffer,1);  
      tree->SetBranchAddress(buffer,&variables[i]);
    }
    
    for (int i=0; i<nentry; i++) {
      tree->GetEntry(i);
      gamma1->SetPtEtaPhiM(variables[0],variables[1],variables[2],variables[3]);
      gamma2->SetPtEtaPhiM(variables[4],variables[5],variables[6],variables[7]);
      *gamma_pair=*gamma1+*gamma2;//contains kinematical properties of the Diphoton system
      costeta=GetCosTheta(gamma1,gamma2);
      
      hist_ggh_gen[0][0]->Fill(gamma_pair->Pt());
      hist_ggh_gen[0][1]->Fill(costeta);
      for (int k=0;k<n_cuttheta;k++) {
	if (costeta > cuttheta[k]) hist_ptcuttheta[k]->Fill(gamma_pair->Pt());//Fill histograms with cutted data
      }
    }
    
    //Normalizing simulated event to expected number of events
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      hist_ggh_gen[0][kinvar]->Scale(0.019629*2.28e-3*19.52*96290/hist_ggh_gen[0][kinvar]->GetEntries());//BR*L*sig*br/Ngen
      stream_integral <<   hist_ggh_gen[0][kinvar]->GetTitle() << " " << hist_ggh_gen[0][kinvar]->Integral() << endl;
      hist_ggh_gen[0][kinvar]->Write("",TObject::kOverwrite);
    }
    
    for (int i=0;i<n_cuttheta;i++) {
      hist_ptcuttheta[i]->Scale(0.019629*2.28e-3*19.52*96290/hist_ggh_gen[0][0]->GetEntries());
      stream_integral <<   hist_ptcuttheta[i]->GetTitle() << " " << hist_ptcuttheta[i]->Integral() << endl;
      hist_ptcuttheta[i]->Write("",TObject::kOverwrite);
      hist_ptcuttheta[i]->Delete();    
}

    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      hist_ggh_gen[0][kinvar]->Delete();
    }


    tree->Delete();
    file->Close();
    file->Delete();
    cout << "ggh_gen done" << endl;
  }
  //#######################VBF_GEN#########################
  if (VBF_GEN) {
    file = new TFile("../data/SMHiggs_m125.root");
    file_result->cd();
    tree  =(TTree *) file->Get("vbf_m125_8TeV");
    
    TH1F *hist_vbf_gen[1][n_kinvar]={{0}};
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      sprintf(dummy,"hist_%s_vbf_gen",kinvarval[kinvar]);
      hist_vbf_gen[0][kinvar]=new TH1F(dummy,dummy,200,0,200-199*kinvar);
      hist_vbf_gen[0][kinvar]->Sumw2();
      hist_vbf_gen[0][kinvar]->GetXaxis()->SetTitle(kinvarval[kinvar]);
      hist_vbf_gen[0][kinvar]->GetYaxis()->SetTitle("events");
    }
    for (int i=0;i<n_cuttheta;i++) {
      sprintf(dummy,"hist_ptcuttheta%1.3f_vbf_gen",cuttheta[i]);
      hist_ptcuttheta[i]=new TH1F(dummy,dummy,200,0,200);
      hist_ptcuttheta[i]->Sumw2();
      hist_ptcuttheta[i]->GetXaxis()->SetTitle("pt");
      hist_ptcuttheta[i]->GetYaxis()->SetTitle("events");
    }
    
    nentry=tree->GetEntries();
    tree->SetBranchStatus("*",0);// selection of useful branches
    for (int i=0; i<8; i++) {
      sprintf(buffer,"%s",gen_variables[i]);
      tree->SetBranchStatus(buffer,1);  
      tree->SetBranchAddress(buffer,&variables[i]);
    }


    for (int i=0; i<nentry; i++) {
      tree->GetEntry(i);
      gamma1->SetPtEtaPhiM(variables[0],variables[1],variables[2],variables[3]);
      gamma2->SetPtEtaPhiM(variables[4],variables[5],variables[6],variables[7]);
      *gamma_pair=*gamma1+*gamma2;//contains kinematical properties of the Diphoton system
      costeta=GetCosTheta(gamma1,gamma2);
      
      hist_vbf_gen[0][0]->Fill(gamma_pair->Pt());
      hist_vbf_gen[0][1]->Fill(costeta);
      for (int k=0;k<n_cuttheta;k++) {
	if (costeta > cuttheta[k]) hist_ptcuttheta[k]->Fill(gamma_pair->Pt());//Fill histograms with cutted data
      }
    }
    

    //Normalizing simulated event to expected number of events
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      hist_vbf_gen[0][kinvar]->Scale(0.019629*2.28e-3*19.52*96290/hist_vbf_gen[0][kinvar]->GetEntries());//BR*L*sig*br/Ngen
      stream_integral <<   hist_vbf_gen[0][kinvar]->GetTitle() << " " << hist_vbf_gen[0][kinvar]->Integral() << endl;
      hist_vbf_gen[0][kinvar]->Write("",TObject::kOverwrite); 
    }    
    for (int i=0;i<n_cuttheta;i++) {
      hist_ptcuttheta[i]->Scale(0.019629*2.28e-3*19.52*96290/hist_vbf_gen[0][0]->GetEntries());
      stream_integral <<   hist_ptcuttheta[i]->GetTitle() << " " << hist_ptcuttheta[i]->Integral() << endl;
      hist_ptcuttheta[i]->Write("",TObject::kOverwrite);
      hist_ptcuttheta[i]->Delete();    
    }

    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      hist_vbf_gen[0][kinvar]->Delete();
    }
    


    tree->Delete();
    file->Close();
    file->Delete();

    cout << "vbf_gen done" << endl;
  }
  //#####################################Analyse of the bkg_gen
  if (BKG_GEN) {
    
    file = new TFile("../data/DiPhotons.root");
    file_result->cd();
    tree =(TTree *) file->Get("diphojet_8TeV");

    TH1F* hist_bkg[n_window][2];
    TH1F* hist_bkg_cut[n_window][n_cuttheta];
    
    for (int window=0;window<n_window;window++) {
      for (int kinvar=0;kinvar<2;kinvar++) {
	sprintf(dummy,"hist_%s_bkg%d_gen",kinvarval[kinvar],windowval[window]);
	if (!kinvar)    hist_bkg[window][kinvar]=new TH1F(dummy,dummy,200,0,200);
	else hist_bkg[window][kinvar]=new TH1F(dummy,dummy,200,0,1);
	hist_bkg[window][kinvar]->Sumw2();
	hist_bkg[window][kinvar]->GetXaxis()->SetTitle(kinvarval[kinvar]);
	hist_bkg[window][kinvar]->GetYaxis()->SetTitle("events");
      }
      for (int k=0;k<n_cuttheta;k++) {
	sprintf(dummy,"hist_ptcuttheta%0.3f_bkg%d_gen",cuttheta[k],windowval[window]);
	hist_bkg_cut[window][k]=new TH1F(dummy,dummy,200,0,200);
	hist_bkg_cut[window][k]->Sumw2();
	hist_bkg_cut[window][k]->GetXaxis()->SetTitle("pt");
	hist_bkg_cut[window][k]->GetYaxis()->SetTitle("events");
      }
    }

    
    nentry=tree->GetEntries();
    tree->SetBranchStatus("*",0);// selection of useful branches
    for (int i=0; i<8; i++) {
      sprintf(buffer,"%s",gen_variables[i]);
      tree->SetBranchStatus(buffer,1);  
      tree->SetBranchAddress(buffer,&variables[i]);
    }
    

    for (int i=0; i<nentry; i++) {
      tree->GetEntry(i);
      gamma1->SetPtEtaPhiM(variables[0],variables[1],variables[2],variables[3]);
      gamma2->SetPtEtaPhiM(variables[4],variables[5],variables[6],variables[7]);
      *gamma_pair=*gamma1+*gamma2;
      costeta=GetCosTheta(gamma1,gamma2);
      
      
      for (int window=0;window<n_window;window++) {
	if (!window) {
	  hist_bkg[window][0]->Fill(gamma_pair->Pt());
	  hist_bkg[window][1]->Fill(costeta);
	  for (int c=0; c< n_cuttheta; c++) {
	    if (costeta > cuttheta[c])   hist_bkg_cut[window][c]->Fill(gamma_pair->Pt());
	  }
	}
	else  if (gamma_pair->M()<125+windowval[window]/2. && gamma_pair->M()>125-windowval[window]/2.) {
	  hist_bkg[window][0]->Fill(gamma_pair->Pt());
	  hist_bkg[window][1]->Fill(costeta);
	  for (int c=0;c<n_cuttheta;c++) {
	    if (costeta > cuttheta[c])   hist_bkg_cut[window][c]->Fill(gamma_pair->Pt());
	  }
	}
      }
    }
    

    
    for (int window=0;window<n_window;window++) {
      for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
	hist_bkg[window][kinvar]->Scale(2.28e-3*1.15*75.39*1133995/hist_bkg[0][kinvar]->GetEntries()); //BR*L*sigma/Ngen
	stream_integral <<   hist_bkg[window][kinvar]->GetTitle() << " " << hist_bkg[window][kinvar]->Integral() << endl;
	hist_bkg[window][kinvar]->Write("",TObject::kOverwrite);
      }
      for (int k=0;k<n_cuttheta;k++) {
	hist_bkg_cut[window][k]->Scale(2.28e-3*1.15*75.39*1133995/hist_bkg[0][0]->GetEntries());
	stream_integral <<   hist_bkg_cut[window][k]->GetTitle() << " " << hist_bkg_cut[window][k]->Integral() << endl;
	hist_bkg_cut[window][k]->Write("",TObject::kOverwrite);
	hist_bkg_cut[window][k]->Delete();
      }
    }
    for (int window=0;window<n_window; window++) {
      for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
	hist_bkg[window][kinvar]->Delete();
      }
    }
    tree->Delete();
    file->Close();
    file->Delete();  
    cout << "bkg_gen done" << endl;
}


  stream_integral.close();
  file_result->Close();

  

  return 0;
}

//#############################################################
//################################################################
float GetCosTheta(TLorentzVector *g1, TLorentzVector *g2) {
  
  TLorentzVector *p1=new TLorentzVector();//Tev
  TLorentzVector *p2=new TLorentzVector();
  p1->SetPxPyPzE(0,0,4,4); // properties of beams in the lab referential
  p2->SetPxPyPzE(0,0,-4,4);
  TLorentzVector *syst=new TLorentzVector();
  *syst=(*g1)+(*g2);
  
  TVector3 *boost=new TVector3();
  *boost=- syst->BoostVector();//Get the boost of diphoton system
  double  x=  boost->X();
  double  y=  boost->Y();
  double  z=  boost->Z();
  
  //put all vectors into the referentiel of the diphoton system  
  p1->Boost(x,y,z);
  p2->Boost(x,y,z);
  g1->Boost(x,y,z);
  
  
  
  TVector3 *bissec=new TVector3(); 
  *bissec= p2->Vect().Unit() -  p1->Vect().Unit();
  bissec->Unit();  
  
  return fabs(cos(bissec->Angle(g1->Vect().Unit())));
}
