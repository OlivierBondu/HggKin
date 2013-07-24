#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TLorentzVector.h"


#include <fstream>
#include "param_kin.h"
#include <iostream>
using namespace std;

#define GGH_GEN 0 
#define VBF_GEN 0
#define BKG_GEN 0
#define RECO 1


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
  float dipho_pt,dipho_mass,dipho_ctheta, weight;
  int isEB=0,category,r9=0;
  TH1F *hist_cuttheta[n_study][n_cuttheta]={{0}}; 
  int nentry=0;
  char buffer [100],dummy[100];
  
  char const *gen_variables[9]={"gh_g1_p4_pt","gh_g1_p4_eta","gh_g1_p4_phi","gh_g1_p4_mass","gh_g2_p4_pt","gh_g2_p4_eta","gh_g2_p4_phi","gh_g2_p4_mass","weight"};//useful variables
  float gen_values[9];//g1_pt,g1_eta,g1_phi,g1_mass,g2_pt,g2_eta,g2_phi,g2_mass,weight;
  char const *reco_variables[17]={"ph1_pt","ph1_eta","ph1_phi","ph1_e","ph1_r9","ph2_pt","ph2_eta","ph2_phi","ph2_e","ph2_r9","PhotonsMass","weight","pu_weight","ph1_ciclevel","ph1_isEB","ph2_ciclevel","ph2_isEB"};
  float reco_fvalues[13];
  int reco_ivalues[4];

  float GetCosTheta(TLorentzVector *g1, TLorentzVector *g2) ;


  //###########################################################
  //#######################GGH_GEN
  //#############################################################
  if (GGH_GEN) {
    file = new TFile("/afs/cern.ch/work/c/cgoudet/private/data/SMHiggs_m125.root");
    file_result->cd();
    tree  =(TTree *) file->Get("ggh_m125_8TeV");


    tree_result=new TTree("tree_gen_ggh","tree_gen_ggh");
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");
    tree_result->Branch("weight",&weight,"weight/F");

    //definitions of all histograms
    TH1F *hist_ggh_gen[1][n_kinvar];
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      sprintf(dummy,"hist_%s_ggh_gen",kinvarval[kinvar]);
      hist_ggh_gen[0][kinvar]=new TH1F(dummy,dummy,n_bins[kinvar],binning[kinvar][0],binning[kinvar][1]);
      hist_ggh_gen[0][kinvar]->Sumw2();
      hist_ggh_gen[0][kinvar]->GetXaxis()->SetTitle(kinvartitle[kinvar]);
      sprintf(dummy,"dN/d%s",kinvartitle[kinvar]);
      hist_ggh_gen[0][kinvar]->GetYaxis()->SetTitle(dummy);
    }
    for (int study=0;study<n_study;study++) {
      for (int i=0;i<n_cuttheta;i++) {
	
	sprintf(dummy,"hist_%scuttheta%d_ggh_gen",kinvarval[study], cuttheta[i]);
	hist_cuttheta[study][i]=new TH1F(dummy,dummy,n_bins[study],binning[study][0],binning[study][1]);
	hist_cuttheta[study][i]->Sumw2();
	hist_cuttheta[study][i]->GetXaxis()->SetTitle(kinvartitle[study]);
	sprintf(dummy,"dN/d%s",kinvartitle[study]);
	hist_cuttheta[study][i]->GetYaxis()->SetTitle(dummy);
      }
    }
    
    nentry=tree->GetEntries();
    tree->SetBranchStatus("*",0);// selection of useful branches
    for (int i=0; i<9; i++) {
      sprintf(buffer,"%s",gen_variables[i]);
      tree->SetBranchStatus(buffer,1);  
      tree->SetBranchAddress(buffer,&gen_values[i]);
    }
    

    for (int i=0; i<nentry; i++) {
      tree->GetEntry(i);
      gamma1->SetPtEtaPhiM(gen_values[0],gen_values[1],gen_values[2],gen_values[3]);
      gamma2->SetPtEtaPhiM(gen_values[4],gen_values[5],gen_values[6],gen_values[7]);
      *gamma_pair=*gamma1+*gamma2;//contains kinematical properties of the Diphoton system
      if (gamma_pair->M()>180 || gamma_pair->M()<100) continue;
      dipho_ctheta=GetCosTheta(gamma1,gamma2);
      dipho_mass=gamma_pair->M();
      dipho_pt=gamma_pair->Pt();
      weight=gen_values[8];
      tree_result->Fill();

      hist_ggh_gen[0][1]->Fill(dipho_pt);
      hist_ggh_gen[0][0]->Fill(dipho_mass);
      hist_ggh_gen[0][2]->Fill(dipho_ctheta);
	for (int cut=0;cut<n_cuttheta;cut++) {

	  if (dipho_ctheta > cuttheta[cut]/1000.){
	    hist_cuttheta[1][cut]->Fill(dipho_pt);//Fill histograms with cutted data
	    hist_cuttheta[0][cut]->Fill(dipho_mass);//Fill histograms with cutted data

	  }
	}
    }

    //Normalizing simulated event to expected number of events
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      hist_ggh_gen[0][kinvar]->Scale(19.52*2.28e-3*19620/96290);//sigma*BR*L/Ngen
      stream_integral <<   hist_ggh_gen[0][kinvar]->GetTitle() << " " << hist_ggh_gen[0][kinvar]->Integral() << endl;
      hist_ggh_gen[0][kinvar]->Write("",TObject::kOverwrite);
    }
    for (int study=0;study<n_study;study++) {
      for (int i=0;i<n_cuttheta;i++) {
	hist_cuttheta[study][i]->Scale(19.52*2.28e-3*19620/96290);//sigma*BR*L/Hgen
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
  //######################################################
  //#######################VBF_GEN#########################
  //#######################################################
  if (VBF_GEN) {
    file = new TFile("/afs/cern.ch/work/c/cgoudet/private/data/SMHiggs_m125.root");
    file_result->cd();
    tree  =(TTree *) file->Get("vbf_m125_8TeV");
    
    tree_result=new TTree("tree_gen_vbf","tree_gen_vbf");
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");
    tree_result->Branch("weight",&weight,"weight/F");

    //definitions of all histograms
    TH1F *hist_vbf_gen[1][n_kinvar]={{0}};
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      sprintf(dummy,"hist_%s_vbf_gen",kinvarval[kinvar]);
      hist_vbf_gen[0][kinvar]=new TH1F(dummy,dummy,n_bins[kinvar],binning[kinvar][0],binning[kinvar][1]);
      hist_vbf_gen[0][kinvar]->Sumw2();
      hist_vbf_gen[0][kinvar]->GetXaxis()->SetTitle(kinvartitle[kinvar]);
      sprintf(dummy,"dN/d%s",kinvartitle[kinvar]);
      hist_vbf_gen[0][kinvar]->GetYaxis()->SetTitle(dummy);
    }
    for (int study=0;study<n_study;study++) {
      for (int i=0;i<n_cuttheta;i++) {
	sprintf(dummy,"hist_%scuttheta%d_vbf_gen",kinvarval[study],cuttheta[i]);
	hist_cuttheta[study][i]=new TH1F(dummy,dummy,n_bins[study],binning[study][0],binning[study][1]);
	hist_cuttheta[study][i]->Sumw2();
	hist_cuttheta[study][i]->GetXaxis()->SetTitle(kinvarval[study]);
	sprintf(dummy,"dN/d%s",kinvartitle[study]);
	hist_cuttheta[study][i]->GetYaxis()->SetTitle(dummy);
      }
    }
    
    nentry=tree->GetEntries();
    tree->SetBranchStatus("*",0);// selection of useful branches
    for (int i=0; i<9; i++) {
      sprintf(buffer,"%s",gen_variables[i]);
      tree->SetBranchStatus(buffer,1);  
      tree->SetBranchAddress(buffer,&gen_values[i]);
    }


    for (int i=0; i<nentry; i++) {
      tree->GetEntry(i);
      gamma1->SetPtEtaPhiM(gen_values[0],gen_values[1],gen_values[2],gen_values[3]);
      gamma2->SetPtEtaPhiM(gen_values[4],gen_values[5],gen_values[6],gen_values[7]);
      *gamma_pair=*gamma1+*gamma2;//contains kinematical properties of the Diphoton system
      if (gamma_pair->M()<100 || gamma_pair->M()>180) continue;
      dipho_ctheta=GetCosTheta(gamma1,gamma2);
      dipho_mass=gamma_pair->M();
      dipho_pt=gamma_pair->Pt();
      weight=gen_values[8];
      tree_result->Fill();
      
      hist_vbf_gen[0][1]->Fill(dipho_pt);
      hist_vbf_gen[0][0]->Fill(dipho_mass);
      hist_vbf_gen[0][2]->Fill(dipho_ctheta);
      for (int cut=0;cut<n_cuttheta;cut++) {
	if (dipho_ctheta > cuttheta[cut]/1000.) {
	  hist_cuttheta[1][cut]->Fill(dipho_pt);//Fill histograms with cutted data
	  hist_cuttheta[0][cut]->Fill(dipho_mass);
	}
      }
    }
    

    //Normalizing simulated event to expected number of events
    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      hist_vbf_gen[0][kinvar]->Scale(1.578*2.28e-3*19620/99855);//sigma*BR*L/Ngen
      stream_integral <<   hist_vbf_gen[0][kinvar]->GetTitle() << " " << hist_vbf_gen[0][kinvar]->Integral() << endl;
      hist_vbf_gen[0][kinvar]->Write("",TObject::kOverwrite); 
    }    
    for (int study=0;study<n_study;study++) {
      for (int i=0;i<n_cuttheta;i++) {
	hist_cuttheta[study][i]->Scale(1.578*2.28e-3*19620/99855);
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
  //###################################################################
  //#####################################BKG_GEN
  //####################################################################
  if (BKG_GEN) {
    
    file = new TFile("/afs/cern.ch/work/c/cgoudet/private/data/DiPhotons.root");
    file_result->cd();
    tree =(TTree *) file->Get("diphojet_8TeV");

    tree_result=new TTree("tree_gen_bkg","tree_gen_bkg");
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");
    tree_result->Branch("weight",&weight,"weight/F");

    TH1F* hist_bkg[n_kinvar]; //Distribution of inclusive data
    TH1F* hist_bkg_window[n_window][n_kinvar-1];// Distributions of variables with mass windows centered on 125GeV
    TH1F* hist_bkg_cut[n_window][n_cuttheta];  //Distributions of pt with mass and theta cut
    

    for (int kinvar=0;kinvar<n_kinvar;kinvar++) {
      sprintf(dummy,"hist_%s_bkg_gen",kinvarval[kinvar]);
      hist_bkg[kinvar]=new TH1F(dummy,dummy,n_bins[kinvar],binning[kinvar][0],binning[kinvar][1]);
      hist_bkg[kinvar]->Sumw2();
      hist_bkg[kinvar]->GetXaxis()->SetTitle(kinvartitle[kinvar]);
      sprintf(dummy,"dN/d%s",kinvartitle[kinvar]);
      hist_bkg[kinvar]->GetYaxis()->SetTitle(dummy);
      if (kinvar) {
	for (int window=0;window<n_window;window++) {
	  sprintf(dummy,"hist_%s_bkg%d_gen",kinvarval[kinvar],windowval[window]);
	  hist_bkg_window[window][kinvar-1]=new TH1F(dummy,dummy,n_bins[kinvar],binning[kinvar][0],binning[kinvar][1]);
	  hist_bkg_window[window][kinvar-1]->Sumw2();
	  hist_bkg_window[window][kinvar-1]->GetXaxis()->SetTitle(kinvartitle[kinvar]);
	  sprintf(dummy,"dN/d%s",kinvartitle[kinvar]);
	  hist_bkg_window[window][kinvar-1]->GetYaxis()->SetTitle(dummy);
	}
      }
    }
    for (int cut=0;cut <n_cuttheta;cut++) {
      for (int window=0;window<n_window;window++) {
	sprintf(dummy,"hist_%scuttheta%d_bkg%d_gen","pt",cuttheta[cut],windowval[window]);
	hist_bkg_cut[window][cut]=new TH1F(dummy,dummy,n_bins[1],binning[1][0],binning[1][1]);
	hist_bkg_cut[window][cut]->Sumw2();
	hist_bkg_cut[window][cut]->GetXaxis()->SetTitle(kinvartitle[1]);
	sprintf(dummy,"dN/d%s",kinvartitle[1]);
	hist_bkg_cut[window][cut]->GetYaxis()->SetTitle("events");      
      }
      for (int study=0;study<n_study;study++) {
	sprintf(dummy,"hist_%scuttheta%d_bkg_gen",kinvarval[study],cuttheta[cut]);
	hist_cuttheta[study][cut]=new TH1F(dummy,dummy,n_bins[study],binning[study][0],binning[study][1]);
	hist_cuttheta[study][cut]->Sumw2();
	hist_cuttheta[study][cut]->GetXaxis()->SetTitle(kinvarval[study]);
	sprintf(dummy,"dN/d%s",kinvartitle[1]);
	hist_cuttheta[study][cut]->GetYaxis()->SetTitle(dummy);      
      }
    }

    
    nentry=tree->GetEntries();
    tree->SetBranchStatus("*",0);// selection of useful branches
    for (int i=0; i<9; i++) {
      sprintf(buffer,"%s",gen_variables[i]);
      tree->SetBranchStatus(buffer,1);  
      tree->SetBranchAddress(buffer,&gen_values[i]);
    }
    

    for (int i=0; i<nentry; i++) {
      tree->GetEntry(i);
      gamma1->SetPtEtaPhiM(gen_values[0],gen_values[1],gen_values[2],gen_values[3]);
      gamma2->SetPtEtaPhiM(gen_values[4],gen_values[5],gen_values[6],gen_values[7]);
      *gamma_pair=*gamma1+*gamma2;
      if (gamma_pair->M() <100 || gamma_pair->M()>180) continue;
      dipho_ctheta=GetCosTheta(gamma1,gamma2);
      dipho_mass=gamma_pair->M();
      dipho_pt=gamma_pair->Pt();
      weight=gen_values[8];
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
	      hist_cuttheta[study][cut]->Scale(75.39*1.15*19620/1133995); //sigma*k*L/Ngen	  
	      stream_integral <<   hist_cuttheta[study][cut]->GetTitle() << " " << hist_cuttheta[study][cut]->Integral() << endl;
	      hist_cuttheta[study][cut]->Write("",TObject::kOverwrite);
	      hist_cuttheta[study][cut]->Delete();
	  }
	  for (int window=0;window<n_window;window++) {
	      hist_bkg_cut[window][cut]->Scale(75.39*1.15*19620/1133995); //sigma*k*L/Ngen
	      stream_integral <<   hist_bkg_cut[window][cut]->GetTitle() << " " << hist_bkg_cut[window][cut]->Integral() << endl;
	      hist_bkg_cut[window][cut]->Write("",TObject::kOverwrite);
	      hist_bkg_cut[window][cut]->Delete();
	  }
	}

	  for (int kinvar=0;kinvar<n_kinvar;kinvar++) {    
	    if(kinvar) 
	      for (int window=0;window<n_window;window++) {
		hist_bkg_window[window][kinvar-1]->Scale(75.39*1.15*19620/1133995); //sigma*k*L/Ngen
		stream_integral <<   hist_bkg_window[window][kinvar-1]->GetTitle() << " " << hist_bkg_window[window][kinvar-1]->Integral() << endl;
		hist_bkg_window[window][kinvar-1]->Write("",TObject::kOverwrite);
		hist_bkg_window[window][kinvar-1]->Delete();
	      }
	    hist_bkg[kinvar]->Scale(75.39*1.15*19620/1133995); //sigma*k*L/Ngen
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
  

  

   //################################################################################################################
  //########################################### RECO #######################################################
  //#######################################################################################################################
  if (RECO) {
    cout << "in reco" << endl;
  TH1F *hist_reco[5][n_kinvar]={{0}};// distributions without mass or theta cut
  TH1F* histcut_reco[n_study][n_cuttheta][5]={{{0}}};// mass and pt with cut theta
  TH1F* histwindow_reco[n_window][n_cuttheta][5]={{{0}}};//pt with all cuts
  TH1F *hist_ctheta_window[2][n_window][5]={{{0}}};//costheta with mass cut
  char *process[3]={"ggh","vbf","bkg"};

  for (int proc=0; proc<3; proc++) {
    switch (proc) 
      {
      case 2:
	file = new TFile("/afs/cern.ch/work/c/cgoudet/private/data/DiPhotons.root");
	tree  =(TTree *) file->Get("diphojet_8TeV");
	file_result->cd();
	break;
      case 0:
	file = new TFile("/afs/cern.ch/work/c/cgoudet/private/data/SMHiggs_m125.root");
	file_result->cd();
	tree  =(TTree *) file->Get("ggh_m125_8TeV");
	break;
      case 1:
	file = new TFile("/afs/cern.ch/work/c/cgoudet/private/data/SMHiggs_m125.root");
	file_result->cd();
	tree  =(TTree *) file->Get("vbf_m125_8TeV");
	break;      
}
    sprintf(buffer,"tree_reco_%s",process[proc]);
    tree_result=new TTree(buffer,buffer);
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");
    tree_result->Branch("weight",&weight,"weight/F");
    tree_result->Branch("category",&category,"category/b");


    for (int categ=0; categ<5; categ++) {
      for (int kinvar=0; kinvar<n_kinvar ; kinvar++) {
	if (categ) sprintf(buffer,"hist_%s_categ%d_%s_reco",kinvarval[kinvar],categ,process[proc]);
	else 	sprintf(buffer,"hist_%s_%s_reco",kinvarval[kinvar],process[proc]);
	hist_reco[categ][kinvar]=new TH1F(buffer,buffer,n_bins[kinvar],binning[kinvar][0],binning[kinvar][1]);
	hist_reco[categ][kinvar]->Sumw2();
	hist_reco[categ][kinvar]->GetXaxis()->SetTitle(kinvartitle[kinvar]);
	sprintf(buffer,"dN/d%s",kinvartitle[kinvar]);
	hist_reco[categ][kinvar]->GetYaxis()->SetTitle(buffer);
      }}

    for (int study=0; study<n_study; study++) {
      for (int cut=0; cut<n_cuttheta; cut++) {
	for (int categ=0; categ<5; categ++) {//0:inclusive; 1:EBEB (both_photons r9>.94); 2:EBEB !(r9); 3:!(EBEB) (r9); 4:!(EBEB) !(r9)
	  if (categ)	  sprintf(buffer,"hist_%scuttheta%d_categ%d_%s_reco",kinvarval[study],cuttheta[cut],categ,process[proc]);
	  else 	  sprintf(buffer,"hist_%scuttheta%d_%s_reco",kinvarval[study],cuttheta[cut],process[proc]);
	  histcut_reco[study][cut][categ]=new TH1F(buffer,buffer,n_bins[study],binning[study][0],binning[study][1]);
	  histcut_reco[study][cut][categ]->Sumw2();
	  histcut_reco[study][cut][categ]->GetXaxis()->SetTitle(kinvartitle[study]);
	  sprintf(buffer,"dN/d%s",kinvartitle[study]);
	  histcut_reco[study][cut][categ]->GetYaxis()->SetTitle(buffer);
	}}}
    



    for (int window=0; window<n_window; window++) {
      for (int categ=0; categ<5; categ++) {
	for (int kinvar=0; kinvar<2; kinvar++) {
	  
	  if (categ)	sprintf(buffer,"hist_%s_categ%d_%s%d_reco",kinvarval[kinvar+1],categ,process[proc],windowval[window]);
	  else sprintf(buffer,"hist_%s_%s%d_reco",kinvarval[kinvar+1],process[proc],windowval[window]);
	  hist_ctheta_window[kinvar][window][categ]=new TH1F(buffer,buffer,n_bins[kinvar+1],binning[kinvar+1][0],binning[kinvar+1][1]);
	  hist_ctheta_window[kinvar][window][categ]->Sumw2();
	  hist_ctheta_window[kinvar][window][categ]->GetXaxis()->SetTitle(kinvartitle[kinvar+1]);
	  sprintf(buffer,"dN/d%s",kinvartitle[kinvar+1]);
	  hist_ctheta_window[kinvar][window][categ]->GetYaxis()->SetTitle(buffer);
	}
      

	for (int cut=0; cut<n_cuttheta; cut++) {
	  if (categ)	sprintf(buffer,"hist_ptcuttheta%d_categ%d_%s%d_reco",cuttheta[cut],categ,process[proc],windowval[window]);
	  else sprintf(buffer,"hist_ptcuttheta%d_%s%d_reco",cuttheta[cut],process[proc],windowval[window]);
	  histwindow_reco[window][cut][categ]=new TH1F(buffer,buffer,n_bins[1],binning[1][0],binning[1][1]);
	  histwindow_reco[window][cut][categ]->Sumw2();
	  histwindow_reco[window][cut][categ]->GetXaxis()->SetTitle(kinvartitle[1]);
	  sprintf(buffer,"dN/d%s",kinvartitle[1]);
	  histwindow_reco[window][cut][categ]->GetYaxis()->SetTitle(buffer);
	}
      }
    }
    
    nentry=tree->GetEntries();
    tree->SetBranchStatus("*",0);// selection of useful branches
    
    for (int i=0; i<13; i++) {
      sprintf(buffer,"%s",reco_variables[i]);
      tree->SetBranchStatus(buffer,1);  
      tree->SetBranchAddress(buffer,&reco_fvalues[i]);
  }
  for (int i=0;i<4;i++) {
    sprintf(buffer,"%s",reco_variables[13+i]);
    tree->SetBranchStatus(buffer,1);  
    tree->SetBranchAddress(buffer,&reco_ivalues[i]);
  }


  for (int i=0;i<nentry;i++) {
    tree->GetEntry(i);
    if (reco_fvalues[0]<40.*reco_fvalues[10]/120.) continue; //ph1_pt< 40*PhotonsMass/120.
    if (reco_fvalues[5]<25.) continue; //ph2_pt<25.
    if ((reco_ivalues[0]<4) || (reco_ivalues[2]<4) ) continue; //(ph1_ciclevel<4 ) || (phi2_ciclevel<4)
    if (reco_fvalues[10]<100. || reco_fvalues[10]>180.) continue;// 100<mgg<180

    gamma1->SetPtEtaPhiE(reco_fvalues[0],reco_fvalues[1],reco_fvalues[2],reco_fvalues[3]);
    gamma2->SetPtEtaPhiE(reco_fvalues[5],reco_fvalues[6],reco_fvalues[7],reco_fvalues[8]);
    *gamma_pair=*gamma1+*gamma2;
    dipho_mass=gamma_pair->M();
    dipho_pt=gamma_pair->Pt();
    dipho_ctheta=GetCosTheta(gamma1,gamma2);
    weight=reco_fvalues[11]*reco_fvalues[12];
    isEB= (reco_ivalues[1] && reco_ivalues[3]) ? 1 : 0 ;
    r9= (reco_fvalues[4]>0.94 && reco_fvalues[9]>0.94) ? 1 : 0 ;
    category=4-2*isEB-r9;
    tree_result->Fill();


    
    hist_reco[0][0]->Fill(dipho_mass,weight);
    hist_reco[0][1]->Fill(dipho_pt,weight);
    hist_reco[0][2]->Fill(dipho_ctheta,weight);
    hist_reco[category][0]->Fill(dipho_mass,weight);
    hist_reco[category][1]->Fill(dipho_pt,weight);
    hist_reco[category][2]->Fill(dipho_ctheta,weight);
    for (int cut=0; cut<n_cuttheta; cut++) {
      if (dipho_ctheta>cuttheta[cut]/1000.) {
	histcut_reco[0][cut][0]->Fill(dipho_mass,weight);
	histcut_reco[1][cut][0]->Fill(dipho_pt,weight);
	histcut_reco[0][cut][category]->Fill(dipho_mass,weight);
	histcut_reco[1][cut][category]->Fill(dipho_pt,weight);
      }
    }
    
    for (int window=0; window<n_window; window++) {
      if (dipho_mass>125+windowval[window]/2. || dipho_mass<125-windowval[window]/2.) continue;
      hist_ctheta_window[1][window][category]->Fill(dipho_ctheta,weight);
      hist_ctheta_window[1][window][0]->Fill(dipho_ctheta,weight);
      hist_ctheta_window[0][window][category]->Fill(dipho_pt,weight);
      hist_ctheta_window[0][window][0]->Fill(dipho_pt,weight);
      for (int cut=0; cut<n_cuttheta; cut++ ) {
	if (dipho_ctheta<cuttheta[cut]/1000.) continue;
	histwindow_reco[window][cut][0]->Fill(dipho_pt,weight);
	histwindow_reco[window][cut][category]->Fill(dipho_pt,weight);
      }
      
    }
  }

  for (int categ=0; categ<5; categ++) {
    for (int kinvar=0; kinvar<n_kinvar; kinvar++) {
      stream_integral << hist_reco[categ][kinvar]->GetTitle() << " " << hist_reco[categ][kinvar]->Integral() << endl;
      hist_reco[categ][kinvar]->Write("",TObject::kOverwrite);
      hist_reco[categ][kinvar]->Delete();
    }
    for (int cut=0; cut<n_cuttheta; cut++) {
      for (int study=0; study<n_study; study++) {
	stream_integral << histcut_reco[study][cut][categ]->GetTitle() << " " << histcut_reco[study][cut][categ]->Integral() << endl;
	histcut_reco[study][cut][categ]->Write("",TObject::kOverwrite);
	histcut_reco[study][cut][categ]->Delete();
      }
      for (int window=0; window<n_window; window++) {
	stream_integral << histwindow_reco[window][cut][categ]->GetTitle() << " " << histwindow_reco[window][cut][categ]->Integral() << endl;
	histwindow_reco[window][cut][categ]->Write("",TObject::kOverwrite);
	histwindow_reco[window][cut][categ]->Delete();
      }
    }
    for (int window=0; window<n_window; window++) {
      for (int kinvar=0; kinvar<2; kinvar++) {
	stream_integral << hist_ctheta_window[kinvar][window][categ]->GetTitle() << " " << hist_ctheta_window[kinvar][window][categ]->Integral() << endl;
	hist_ctheta_window[kinvar][window][categ]->Write("",TObject::kOverwrite);
	hist_ctheta_window[kinvar][window][categ]->Delete();
      }
    }
  }


  tree_result->Write("",TObject::kOverwrite);
  tree_result->Delete();
  file->Close();
  file->Delete();

  sprintf(buffer,"%s reco done",process[proc]);
  cout << buffer << endl; 
  

  }  
  }


  stream_integral.close();
  file_result->Close();
  return 0;
  }



//################################################################  
//################################################################
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
