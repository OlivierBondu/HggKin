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
  stream_integral.open("/afs/cern.ch/work/c/cgoudet/private/data/hist_integral.txt",fstream::out);

  //################################################
  
  TFile *file=0;
  TFile *file_result = new TFile("/afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root","UPDATE");
  
  TTree *tree=0;
  TTree *tree_result=0;
  

  //Creating variables for the analysis
  TLorentzVector *gamma1=new TLorentzVector();
  TLorentzVector *gamma2=new TLorentzVector();
  TLorentzVector *gamma_pair=new TLorentzVector();
  TH1F *hist_cuttheta[n_study][n_cuttheta]={{0}};
  
  int nentry=0;
  char buffer [100],dummy[100];
  char const *gen_variables[8]={"gh_g1_p4_pt","gh_g1_p4_eta","gh_g1_p4_phi","gh_g1_p4_mass","gh_g2_p4_pt","gh_g2_p4_eta","gh_g2_p4_phi","gh_g2_p4_mass"};//useful variables
  float variables[8];//g1_pt,g1_eta,g1_phi,g1_mass,g2_pt,g2_eta,g2_phi,g2_mass;
  float dipho_pt,dipho_mass,dipho_ctheta;

  float GetCosTheta(TLorentzVector *g1, TLorentzVector *g2) ;



  //#######################GGH
  if (GGH_GEN) {
    file = new TFile("/afs/cern.ch/work/c/cgoudet/private/data/SMHiggs_m125.root");
    file_result->cd();
    tree  =(TTree *) file->Get("ggh_m125_8TeV");


    tree_result=new TTree("tree_ggh","tree_ggh");
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");



    TH1F *hist_ggh_gen[1][n_kinvar];
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      sprintf(dummy,"hist_%s_ggh_gen",kinvarval[kinvar]);
      hist_ggh_gen[0][kinvar]=new TH1F(dummy,dummy,n_bins[kinvar],binning[kinvar][0],binning[kinvar][1]);
      hist_ggh_gen[0][kinvar]->Sumw2();
      hist_ggh_gen[0][kinvar]->GetXaxis()->SetTitle(kinvarval[kinvar]);
      hist_ggh_gen[0][kinvar]->GetYaxis()->SetTitle("events");
    }
    
    for (int study=0;study<n_study;study++) {
      for (int i=0;i<n_cuttheta;i++) {
	

	sprintf(dummy,"hist_%scuttheta%d_ggh_gen",kinvarval[study], cuttheta[i]);
	hist_cuttheta[study][i]=new TH1F(dummy,dummy,n_bins[study],binning[study][0],binning[study][1]);
	hist_cuttheta[study][i]->Sumw2();
	hist_cuttheta[study][i]->GetXaxis()->SetTitle(kinvarval[study]);
	hist_cuttheta[study][i]->GetYaxis()->SetTitle("events");
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
      *gamma_pair=*gamma1+*gamma2;//contains kinematical properties of the Diphoton system
      dipho_ctheta=GetCosTheta(gamma1,gamma2);
      dipho_mass=gamma_pair->M();
      dipho_pt=gamma_pair->Pt();
      tree_result->Fill();

      hist_ggh_gen[0][1]->Fill(dipho_pt);
      hist_ggh_gen[0][0]->Fill(dipho_mass);
      hist_ggh_gen[0][2]->Fill(dipho_ctheta);
	for (int k=0;k<n_cuttheta;k++) {

	  if (dipho_ctheta > cuttheta[k]/1000.){
	    hist_cuttheta[1][k]->Fill(dipho_pt);//Fill histograms with cutted data
	    hist_cuttheta[0][k]->Fill(dipho_mass);//Fill histograms with cutted data

	  }
	}
    }

    //Normalizing simulated event to expected number of events
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      hist_ggh_gen[0][kinvar]->Scale(0.019629*2.28e-3*19.52*96290/hist_ggh_gen[0][kinvar]->GetEntries());//BR*L*sig*br/Ngen
      stream_integral <<   hist_ggh_gen[0][kinvar]->GetTitle() << " " << hist_ggh_gen[0][kinvar]->Integral() << endl;
      hist_ggh_gen[0][kinvar]->Write("",TObject::kOverwrite);
    }

    for (int study=0;study<n_study;study++) {
      for (int i=0;i<n_cuttheta;i++) {
	hist_cuttheta[study][i]->Scale(0.019629*2.28e-3*19.52*96290/hist_ggh_gen[0][study]->GetEntries());
	stream_integral <<   hist_cuttheta[study][i]->GetTitle() << " " << hist_cuttheta[study][i]->Integral() << endl;
	hist_cuttheta[study][i]->Write("",TObject::kOverwrite);
	hist_cuttheta[study][i]->Delete();    
      }
    }
    
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      hist_ggh_gen[0][kinvar]->Delete();
    }

    tree_result->Write("",TObject::kOverwrite);
    tree_result->Delete();
    tree->Delete();
    file->Close();
    file->Delete();
    cout << "ggh_gen done" << endl;
  }
  //#######################VBF_GEN#########################
  if (VBF_GEN) {
    file = new TFile("/afs/cern.ch/work/c/cgoudet/private/data/SMHiggs_m125.root");
    file_result->cd();
    tree  =(TTree *) file->Get("vbf_m125_8TeV");
    
    tree_result=new TTree("tree_vbf","tree_vbf");
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");

    TH1F *hist_vbf_gen[1][n_kinvar]={{0}};
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      sprintf(dummy,"hist_%s_vbf_gen",kinvarval[kinvar]);
      hist_vbf_gen[0][kinvar]=new TH1F(dummy,dummy,n_bins[kinvar],binning[kinvar][0],binning[kinvar][1]);
      hist_vbf_gen[0][kinvar]->Sumw2();
      hist_vbf_gen[0][kinvar]->GetXaxis()->SetTitle(kinvarval[kinvar]);
      hist_vbf_gen[0][kinvar]->GetYaxis()->SetTitle("events");
    }
    for (int study=0;study<n_study;study++) {
      for (int i=0;i<n_cuttheta;i++) {

	sprintf(dummy,"hist_%scuttheta%d_vbf_gen",kinvarval[study],cuttheta[i]);
	hist_cuttheta[study][i]=new TH1F(dummy,dummy,n_bins[study],binning[study][0],binning[study][1]);
	hist_cuttheta[study][i]->Sumw2();
	hist_cuttheta[study][i]->GetXaxis()->SetTitle(kinvarval[study]);
	hist_cuttheta[study][i]->GetYaxis()->SetTitle("events");
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
      *gamma_pair=*gamma1+*gamma2;//contains kinematical properties of the Diphoton system
      dipho_ctheta=GetCosTheta(gamma1,gamma2);
      dipho_mass=gamma_pair->M();
      dipho_pt=gamma_pair->Pt();
      tree_result->Fill();
      
      hist_vbf_gen[0][1]->Fill(dipho_pt);
      hist_vbf_gen[0][0]->Fill(dipho_mass);
      hist_vbf_gen[0][2]->Fill(dipho_ctheta);
      for (int k=0;k<n_cuttheta;k++) {

	if (dipho_ctheta > cuttheta[k]/1000.) {
	  hist_cuttheta[1][k]->Fill(dipho_pt);//Fill histograms with cutted data
	  hist_cuttheta[0][k]->Fill(dipho_mass);

	}
      }
    }
    

    //Normalizing simulated event to expected number of events
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      hist_vbf_gen[0][kinvar]->Scale(0.019629*2.28e-3*1.578*99855/hist_vbf_gen[0][kinvar]->GetEntries());//BR*L*sig*br/Ngen
      stream_integral <<   hist_vbf_gen[0][kinvar]->GetTitle() << " " << hist_vbf_gen[0][kinvar]->Integral() << endl;
      hist_vbf_gen[0][kinvar]->Write("",TObject::kOverwrite); 
    }    
    for (int study=0;study<n_study;study++) {
      for (int i=0;i<n_cuttheta;i++) {
	hist_cuttheta[study][i]->Scale(0.019629*2.28e-3*1.578*99855/hist_vbf_gen[0][study]->GetEntries());
	stream_integral <<   hist_cuttheta[study][i]->GetTitle() << " " << hist_cuttheta[study][i]->Integral() << endl;
	hist_cuttheta[study][i]->Write("",TObject::kOverwrite);
	hist_cuttheta[study][i]->Delete();    
      }
    }

    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      hist_vbf_gen[0][kinvar]->Delete();
    }
    

    tree_result->Write("",TObject::kOverwrite);
    tree_result->Delete();
    tree->Delete();
    file->Close();
    file->Delete();

    cout << "vbf_gen done" << endl;
  }
  //#####################################BKG
  if (BKG_GEN) {
    
    file = new TFile("/afs/cern.ch/work/c/cgoudet/private/data/DiPhotons.root");
    file_result->cd();
    tree =(TTree *) file->Get("diphojet_8TeV");

    tree_result=new TTree("tree_bkg","tree_bkg");
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");

    TH1F* hist_bkg[n_kinvar]; //Distribution of inclusive data
    TH1F* hist_bkg_window[n_window][n_kinvar-1];// Distributions of variables with mass windows centered on 125GeV
    TH1F* hist_bkg_cut[n_window][n_cuttheta];  //Distributions of pt with mass and theta cut
    

    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      sprintf(dummy,"hist_%s_bkg_gen",kinvarval[kinvar]);
      hist_bkg[kinvar]=new TH1F(dummy,dummy,n_bins[kinvar],binning[kinvar][0],binning[kinvar][1]);
      hist_bkg[kinvar]->Sumw2();
      hist_bkg[kinvar]->GetXaxis()->SetTitle(kinvarval[kinvar]);
      hist_bkg[kinvar]->GetYaxis()->SetTitle("events");
      if (kinvar) {
	for (int window=0;window<n_window;window++) {
	  sprintf(dummy,"hist_%s_bkg%d_gen",kinvarval[kinvar],windowval[window]);
	  hist_bkg_window[window][kinvar-1]=new TH1F(dummy,dummy,n_bins[kinvar],binning[kinvar][0],binning[kinvar][1]);
	  hist_bkg_window[window][kinvar-1]->Sumw2();
	  hist_bkg_window[window][kinvar-1]->GetXaxis()->SetTitle(kinvarval[kinvar]);
	  hist_bkg_window[window][kinvar-1]->GetYaxis()->SetTitle("events");
	}
      }
    }

    for (int cut=0;cut <n_cuttheta;cut++) {
      for (int window=0;window<n_window;window++) {

	sprintf(dummy,"hist_%scuttheta%d_bkg%d_gen","pt",cuttheta[cut],windowval[window]);
	hist_bkg_cut[window][cut]=new TH1F(dummy,dummy,n_bins[1],binning[1][0],binning[1][1]);
	hist_bkg_cut[window][cut]->Sumw2();
	hist_bkg_cut[window][cut]->GetXaxis()->SetTitle("pT");
	hist_bkg_cut[window][cut]->GetYaxis()->SetTitle("events");      
      }
      for (int study=0;study<n_study;study++) {
	sprintf(dummy,"hist_%scuttheta%d_bkg_gen",kinvarval[study],cuttheta[cut]);
	hist_cuttheta[study][cut]=new TH1F(dummy,dummy,n_bins[study],binning[study][0],binning[study][1]);
	hist_cuttheta[study][cut]->Sumw2();
	hist_cuttheta[study][cut]->GetXaxis()->SetTitle(kinvarval[study]);
	hist_cuttheta[study][cut]->GetYaxis()->SetTitle("events");      
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
      if (gamma_pair->M() > 180 || gamma_pair->M() < 100) continue;
      dipho_ctheta=GetCosTheta(gamma1,gamma2);
      dipho_mass=gamma_pair->M();
      dipho_pt=gamma_pair->Pt();
      tree_result->Fill();

      
      hist_bkg[0]->Fill(dipho_mass);
      hist_bkg[1]->Fill(dipho_pt);
      hist_bkg[2]->Fill(dipho_ctheta);
    
      for (int window=0;window<n_window; window++) {
	hist_bkg_window[window][0]->Fill(dipho_pt);
	hist_bkg_window[window][1]->Fill(dipho_ctheta);
      }

      for (int cut=0;cut<n_cuttheta;cut++){
	if (dipho_ctheta>cuttheta[cut]/1000.) {
	  hist_cuttheta[0][cut]->Fill(dipho_mass);
	  hist_cuttheta[1][cut]->Fill(dipho_pt);
	  for (int window=0;window<n_window;window++) {
	    if (dipho_mass > 125-windowval[window]/2. && dipho_mass < 125+windowval[window]/2.)
	      hist_bkg_cut[window][cut]->Fill(dipho_pt);
	  }
	}
      }
    }
    

    //Scaling and normalizing hist to expected number of event
	for (int cut=0;cut<n_cuttheta;cut++) {
	  for (int study=0;study<n_study;study++) {
	      hist_cuttheta[study][cut]->Scale(2.28e-3*1.15*75.39*1133995/hist_bkg[study]->GetEntries()); //BR*L*sigma/Ngen	  
	      stream_integral <<   hist_cuttheta[study][cut]->GetTitle() << " " << hist_cuttheta[study][cut]->Integral() << endl;
	      hist_cuttheta[study][cut]->Write("",TObject::kOverwrite);
	      hist_cuttheta[study][cut]->Delete();
	  }
	  for (int window=0;window<n_window;window++) {
	      hist_bkg_cut[window][cut]->Scale(2.28e-3*1.15*75.39*1133995/hist_bkg[0]->GetEntries()); //BR*L*sigma/Ngen	  
	      stream_integral <<   hist_bkg_cut[window][cut]->GetTitle() << " " << hist_bkg_cut[window][cut]->Integral() << endl;
	      hist_bkg_cut[window][cut]->Write("",TObject::kOverwrite);
	      hist_bkg_cut[window][cut]->Delete();
	  }
	}

	  for (int kinvar=0;kinvar<n_kinvar;kinvar++) {    
	    if(kinvar) 
	      for (int window=0;window<n_window;window++) {
		hist_bkg_window[window][kinvar-1]->Scale(2.28e-3*1.15*75.39*1133995/hist_bkg[kinvar]->GetEntries()); //BR*L*sigma/Ngen	  
		stream_integral <<   hist_bkg_window[window][kinvar-1]->GetTitle() << " " << hist_bkg_window[window][kinvar-1]->Integral() << endl;
		hist_bkg_window[window][kinvar-1]->Write("",TObject::kOverwrite);
		hist_bkg_window[window][kinvar-1]->Delete();
	      }
	    hist_bkg[kinvar]->Scale(2.28e-3*1.15*75.39*1133995/hist_bkg[kinvar]->GetEntries()); //BR*L*sigma/Ngen	  
	    stream_integral <<   hist_bkg[kinvar]->GetTitle() << " " << hist_bkg[kinvar]->Integral() << endl;
	    hist_bkg[kinvar]->Write("",TObject::kOverwrite);
	    hist_bkg[kinvar]->Delete();
	}
	

	  tree_result->Write("",TObject::kOverwrite);
	  tree_result->Delete();
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
