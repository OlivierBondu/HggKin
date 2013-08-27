// Original kin_dist.cc by C. Goudet
// Adapted by O. Bondu (Aug 2013)
// ROOT Headers
#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
// C++ headers
#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
// namespaces
using namespace std;
namespace po = boost::program_options;

// functions declaration
float GetCosTheta(TLorentzVector *g1, TLorentzVector *g2);


int main(int argc, char* argv[])
{
// input parameters and option parsing
	bool doSignal;
	bool doBackground;
	bool doData;
	bool doGen;
	bool doReco;
	copy(argv, argv + argc, ostream_iterator<char*>(cout, " "));
	cout << endl;

	try
	{
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("doSig", po::value<bool>(&doSignal)->default_value(true), "process signal mc trees")
			("doBkg", po::value<bool>(&doBackground)->default_value(false), "process background mc trees")
			("doData", po::value<bool>(&doData)->default_value(false), "process data trees")
			("doGen", po::value<bool>(&doGen)->default_value(false), "process gen-level info")
			("doReco", po::value<bool>(&doReco)->default_value(true), "process reco-level info")
		;
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
		if (vm.count("help")) {
			cout << desc << "\n";
			return 1;
		}
	} catch(exception& e) {
		cerr << "error: " << e.what() << "\n";
		return 1;
	} catch(...) {
		cerr << "Exception of unknown type!\n";
	}



  //------------Analysis choices of parameters and cuts
  //   kinematical variables
  int const n_kinvar=3;
  float const binning[n_kinvar][2]={{100,180},{0,200},{0,1}};
  //-------------  

  fstream stream_integral;

  //################################################
  
  TFile *file=0;
  TFile *file_result = new TFile("/afs/cern.ch/work/o/obondu/Higgs/CMSSW_5_3_9_patch1_HggKin/src/HggKin/kin_dist.root","UPDATE");
  
  TTree *tree=0;
  TTree *tree_result=0;

  //Creating variables for the analysis
  TLorentzVector *gamma1=new TLorentzVector();
  TLorentzVector *gamma2=new TLorentzVector();
  TLorentzVector *gamma_pair=new TLorentzVector();
  float dipho_pt,dipho_mass,dipho_ctheta, weight;
  int isEB=0,category,r9=0;
  int nentry=0;
  char buffer [100];
  
  char const *gen_variables[9]={"gh_g1_p4_pt","gh_g1_p4_eta","gh_g1_p4_phi","gh_g1_p4_mass","gh_g2_p4_pt","gh_g2_p4_eta","gh_g2_p4_phi","gh_g2_p4_mass","weight"};//useful variables
  float gen_values[9];//g1_pt,g1_eta,g1_phi,g1_mass,g2_pt,g2_eta,g2_phi,g2_mass,weight;
  char const *reco_variables[17]={"ph1_pt","ph1_eta","ph1_phi","ph1_e","ph1_r9","ph2_pt","ph2_eta","ph2_phi","ph2_e","ph2_r9","PhotonsMass","weight","pu_weight","ph1_ciclevel","ph1_isEB","ph2_ciclevel","ph2_isEB"};
  float reco_fvalues[13];
  int reco_ivalues[4];

//  float GetCosTheta(TLorentzVector *g1, TLorentzVector *g2) ;


  //###########################################################
  //#######################GGH_GEN
  //#############################################################
  if (doSignal && doGen) {
    file = new TFile("/afs/cern.ch/work/o/obondu/Higgs/CMSSW_5_3_9_patch1_HggKin/src/HggKin/SMHiggs_m125.root");
    file_result->cd();
    tree  =(TTree *) file->Get("ggh_m125_8TeV");


    tree_result=new TTree("tree_gen_ggh","tree_gen_ggh");
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");
    tree_result->Branch("weight",&weight,"weight/F");

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
      if(dipho_pt>binning[1][1]) continue;


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
  if (doSignal && doGen) {
    file = new TFile("/afs/cern.ch/work/o/obondu/Higgs/CMSSW_5_3_9_patch1_HggKin/src/HggKin/SMHiggs_m125.root");
    file_result->cd();
    tree  =(TTree *) file->Get("vbf_m125_8TeV");
    
    tree_result=new TTree("tree_gen_vbf","tree_gen_vbf");
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");
    tree_result->Branch("weight",&weight,"weight/F");

    
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
      if(dipho_pt>binning[1][1]) continue;
      
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
  if (doBackground && doGen) {
    
    file = new TFile("/afs/cern.ch/work/o/obondu/Higgs/CMSSW_5_3_9_patch1_HggKin/src/HggKin/DiPhotons.root");
    file_result->cd();
    tree =(TTree *) file->Get("diphojet_8TeV");

    tree_result=new TTree("tree_gen_bkg","tree_gen_bkg");
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");
    tree_result->Branch("weight",&weight,"weight/F");

    nentry=tree->GetEntries();
    tree->SetBranchStatus("*",0);// selection of useful branches
    for (int i=0; i<9; i++) {
      sprintf(buffer,"%s",gen_variables[i]);
      tree->SetBranchStatus(buffer,1);  
      tree->SetBranchAddress(buffer,&gen_values[i]);
    }
    
    cout << nentry << endl;
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
  if (doReco) {
    cout << "in reco" << endl;
  char *process[3]={"ggh","vbf","bkg"};

  for (int proc=0; proc<3; proc++) {
    switch (proc) 
      {
      case 2:
	file = new TFile("/afs/cern.ch/work/o/obondu/Higgs/CMSSW_5_3_9_patch1_HggKin/src/HggKin/DiPhotons.root");
	tree  =(TTree *) file->Get("diphojet_8TeV");
	file_result->cd();
	break;
      case 0:
	file = new TFile("/afs/cern.ch/work/o/obondu/Higgs/CMSSW_5_3_9_patch1_HggKin/src/HggKin/SMHiggs_m125.root");
	file_result->cd();
	tree  =(TTree *) file->Get("ggh_m125_8TeV");
	break;
      case 1:
	file = new TFile("/afs/cern.ch/work/o/obondu/Higgs/CMSSW_5_3_9_patch1_HggKin/src/HggKin/SMHiggs_m125.root");
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
    tree_result->Branch("category",&category,"category/I");



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

    double totweight=0;
//     float phopt=0;
//     tree->SetBranchStatus("dipho_pt",1);
//     tree->SetBranchAddress("dipho_pt",&phopt);




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
    //      cout << dipho_pt << " " << phopt << "     " << dipho_mass << " " << reco_fvalues[10] << endl;
    //cout << reco_fvalues[11] << " " << reco_fvalues[12] << " " << weight << endl;
      totweight+= weight;
    isEB= (reco_ivalues[1] && reco_ivalues[3]) ? 1 : 0 ;
    r9= (reco_fvalues[4]>0.94 && reco_fvalues[9]>0.94) ? 1 : 0 ;
    category=3-2*isEB-r9;
    tree_result->Fill();
      if(dipho_pt>binning[1][1]) continue;


  }


  tree_result->Write("",TObject::kOverwrite);
  tree_result->Delete();
  file->Close();
  file->Delete();

  sprintf(buffer,"%s reco done",process[proc]);
  cout << buffer << endl; 
  cout << totweight << endl;  

  }  
  }
//################################################################  
//################################################################
//################################################################

  if (doData) {
    cout << "in data" << endl;

    TFile *file_input=new TFile("/afs/cern.ch/work/o/obondu/Higgs/CMSSW_5_3_9_patch1_HggKin/src/HggKin/data.root");
    TTree *tree_input=(TTree *) file_input->Get("Data");
    file_result->cd();

    //Create branches of output tree
    tree_result=new TTree("tree_data","tree_data");
    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");
    tree_result->Branch("category",&category,"category/I");

    // Activate branches of interest in the input tree
    nentry=tree_input->GetEntries();
    tree_input->SetBranchStatus("*",0);// selection of useful branches
    char const *data_variables[15]={"ph1_pt","ph1_eta","ph1_phi","ph1_e","ph1_r9","ph2_pt","ph2_eta","ph2_phi","ph2_e","ph2_r9","PhotonsMass","ph1_ciclevel","ph1_isEB","ph2_ciclevel","ph2_isEB"};
    float data_fvalues[11];
    int data_ivalues[4];
    
    for (int i=0; i<11; i++) {
      tree_input->SetBranchStatus(data_variables[i],1);  
      tree_input->SetBranchAddress(data_variables[i],&data_fvalues[i]);
    }
    for (int i=0;i<4;i++) {
    tree_input->SetBranchStatus(data_variables[11+i],1);  
    tree_input->SetBranchAddress(data_variables[11+i],&data_ivalues[i]);
    }
    
    //Fill output tree according to cuts
    for (int i=0;i<nentry;i++) {
      tree_input->GetEntry(i);
      if (data_fvalues[0]<40.*data_fvalues[10]/120.) continue; //ph1_pt> 40*PhotonsMass/120.
      if (data_fvalues[5]<25.) continue; //ph2_pt>25.
      if ((data_ivalues[0]<4) || (data_ivalues[2]<4) ) continue; //(ph1_ciclevel>4 ) && (phi2_ciclevel>4)
      if (data_fvalues[10]<100. || data_fvalues[10]>180.) continue;// 100<mgg<180
      gamma1->SetPtEtaPhiE(data_fvalues[0],data_fvalues[1],data_fvalues[2],data_fvalues[3]);
      gamma2->SetPtEtaPhiE(data_fvalues[5],data_fvalues[6],data_fvalues[7],data_fvalues[8]);
      *gamma_pair=*gamma1+*gamma2;
      dipho_mass=gamma_pair->M();
      dipho_pt=gamma_pair->Pt();
      dipho_ctheta=GetCosTheta(gamma1,gamma2);

      isEB= (data_ivalues[1] && data_ivalues[3]) ? 1 : 0 ;
      r9= (data_fvalues[4]>0.94 && data_fvalues[9]>0.94) ? 1 : 0 ;
      category=3-2*isEB-r9;
      tree_result->Fill();
    }
    cout <<    tree_result->GetEntries() << endl;
    tree_result->Write("",TObject::kOverwrite);
    tree_result->Delete();
    tree_input->Delete();
    file_input->Close();
    file_input->Delete();

  cout << "Data done"  << endl; 

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
  p1->SetPxPyPzE(0,0,4000,4000); // properties of beams in the lab referential
  p2->SetPxPyPzE(0,0,-4000,4000);
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
