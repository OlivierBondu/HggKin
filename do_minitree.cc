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
// defines
#define DEBUG 0
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

  //################################################
  
  TFile *file=0;
  TFile *file_result = new TFile("kin_dist.root","UPDATE");
  
  TTree *tree=0;
  TTree *tree_result=0;

  //Creating variables for the analysis
  TLorentzVector *gamma1=new TLorentzVector();
  TLorentzVector *gamma2=new TLorentzVector();
  TLorentzVector *gamma_pair=new TLorentzVector();
  int isEB=0,category,r9=0;
  int nentry=0;
  char buffer [100];

	// input infos common to gen and reco cases
	float ph1_pt, ph1_eta, ph1_phi, ph1_e, ph1_mass;
	float ph2_pt, ph2_eta, ph2_phi, ph2_e, ph2_mass;
  float dipho_pt, dipho_mass, dipho_ctheta, weight;
	// input infos specific to reco case
	float ph1_r9, ph2_r9;
	float pu_weight;
	int ph1_ciclevel, ph1_isEB;
	int ph2_ciclevel, ph2_isEB;

	vector<int> sample_type; sample_type.clear();
	vector<int> sample_isReco; sample_isReco.clear();
	vector<string> sample_infile; sample_infile.clear();
	vector<string> sample_intree; sample_intree.clear();
	vector<string> sample_outtree; sample_outtree.clear();
	sample_type.push_back(-1); sample_isReco.push_back(0); sample_infile.push_back("SMHiggs_m125.root"); sample_intree.push_back("ggh_m125_8TeV"); sample_outtree.push_back("tree_gen_ggh");
	sample_type.push_back(-1); sample_isReco.push_back(0); sample_infile.push_back("SMHiggs_m125.root"); sample_intree.push_back("vbf_m125_8TeV"); sample_outtree.push_back("tree_gen_vbf");
	sample_type.push_back( 1); sample_isReco.push_back(0); sample_infile.push_back("DiPhotons.root"); sample_intree.push_back("diphojet_8TeV"); sample_outtree.push_back("tree_gen_bkg");
	sample_type.push_back(-1); sample_isReco.push_back(1); sample_infile.push_back("SMHiggs_m125.root"); sample_intree.push_back("ggh_m125_8TeV"); sample_outtree.push_back("tree_reco_ggh");
	sample_type.push_back(-1); sample_isReco.push_back(1); sample_infile.push_back("SMHiggs_m125.root"); sample_intree.push_back("vbf_m125_8TeV"); sample_outtree.push_back("tree_reco_vbf");
	sample_type.push_back( 1); sample_isReco.push_back(1); sample_infile.push_back("DiPhotons.root"); sample_intree.push_back("diphojet_8TeV"); sample_outtree.push_back("tree_reco_bkg");
	sample_type.push_back( 0); sample_isReco.push_back(1); sample_infile.push_back("data.root"); sample_intree.push_back("Data"); sample_outtree.push_back("tree_data");

	double totweight;
	int nselected;

	for( unsigned int isample=0 ; isample < sample_infile.size() ; isample++ )
	{
		int type = sample_type[isample];
		int isReco = sample_isReco[isample];
		if( DEBUG ) cout << "type= " << type << "\tisReco= " << isReco << endl;
		if( type < 0 && !doSignal ) continue;
		if( type == 0 && !doData ) continue;
		if( type > 0 && !doBackground ) continue;
		if( isReco == 0 && !doGen ) continue;
		if( isReco == 1 && !doReco ) continue;
		totweight = 0.;
		nselected = 0;

		file = new TFile( sample_infile[isample].c_str() );
		file_result->cd();
		tree = (TTree*) file->Get( sample_intree[isample].c_str() );

		tree_result = new TTree( sample_outtree[isample].c_str(), sample_outtree[isample].c_str() );

    tree_result->Branch("dipho_mass",&dipho_mass,"dipho_mass/F");
    tree_result->Branch("dipho_pt",&dipho_pt,"dipho_pt/F");
    tree_result->Branch("dipho_ctheta",&dipho_ctheta,"dipho_ctheta/F");
    tree_result->Branch("weight",&weight,"weight/F");
		if( isReco == 1 ) tree_result->Branch("category",&category,"category/I");

		nentry=tree->GetEntries();
		tree->SetBranchStatus("*",0);
		tree->SetBranchStatus(isReco == 0 ? "gh_g1_p4_pt" : "ph1_pt", 1);
		tree->SetBranchStatus(isReco == 0 ? "gh_g1_p4_eta" : "ph1_eta", 1);
		tree->SetBranchStatus(isReco == 0 ? "gh_g1_p4_phi" : "ph1_phi", 1);
		tree->SetBranchStatus(isReco == 0 ? "gh_g1_p4_mass" : "ph1_e", 1);
		tree->SetBranchStatus(isReco == 0 ? "gh_g2_p4_pt" : "ph2_pt", 1);
		tree->SetBranchStatus(isReco == 0 ? "gh_g2_p4_eta" : "ph2_eta", 1);
		tree->SetBranchStatus(isReco == 0 ? "gh_g2_p4_phi" : "ph2_phi", 1);
		tree->SetBranchStatus(isReco == 0 ? "gh_g2_p4_mass" : "ph2_e", 1);
		tree->SetBranchStatus("weight", 1);
		if( isReco == 1 ) tree->SetBranchStatus("pu_weight", 1);
		if( isReco == 1 ) tree->SetBranchStatus("ph1_r9", 1);
		if( isReco == 1 ) tree->SetBranchStatus("ph2_r9", 1);
		if( isReco == 1 ) tree->SetBranchStatus("ph1_ciclevel", 1);
		if( isReco == 1 ) tree->SetBranchStatus("ph2_ciclevel", 1);
		if( isReco == 1 ) tree->SetBranchStatus("ph1_isEB", 1);
		if( isReco == 1 ) tree->SetBranchStatus("ph2_isEB", 1);
		tree->SetBranchAddress(isReco == 0 ? "gh_g1_p4_pt" : "ph1_pt", &ph1_pt);
		tree->SetBranchAddress(isReco == 0 ? "gh_g1_p4_eta" : "ph1_eta", &ph1_eta);
		tree->SetBranchAddress(isReco == 0 ? "gh_g1_p4_phi" : "ph1_phi", &ph1_phi);
		tree->SetBranchAddress(isReco == 0 ? "gh_g1_p4_mass" : "ph1_e", isReco == 0 ? &ph1_mass : &ph1_e);
		tree->SetBranchAddress(isReco == 0 ? "gh_g2_p4_pt" : "ph2_pt", &ph2_pt);
		tree->SetBranchAddress(isReco == 0 ? "gh_g2_p4_eta" : "ph2_eta", &ph2_eta);
		tree->SetBranchAddress(isReco == 0 ? "gh_g2_p4_phi" : "ph2_phi", &ph2_phi);
		tree->SetBranchAddress(isReco == 0 ? "gh_g2_p4_mass" : "ph2_e", isReco == 0 ? &ph2_mass : &ph2_e);
		tree->SetBranchAddress("weight", &weight);
		if( isReco == 1 ) tree->SetBranchAddress("pu_weight", &pu_weight);
		if( isReco == 1 ) tree->SetBranchAddress("ph1_r9", &ph1_r9);
		if( isReco == 1 ) tree->SetBranchAddress("ph2_r9", &ph2_r9);
		if( isReco == 1 ) tree->SetBranchAddress("ph1_ciclevel", &ph1_ciclevel);
		if( isReco == 1 ) tree->SetBranchAddress("ph2_ciclevel", &ph2_ciclevel);
		if( isReco == 1 ) tree->SetBranchAddress("ph1_isEB", &ph1_isEB);
		if( isReco == 1 ) tree->SetBranchAddress("ph2_isEB", &ph2_isEB);

	
		for(int i=0; i < nentry ; i++)
		{
			tree->GetEntry(i);
			if( isReco == 0 )
			{
				gamma1->SetPtEtaPhiM(ph1_pt, ph1_eta, ph1_phi, ph1_mass);
				gamma2->SetPtEtaPhiM(ph2_pt, ph2_eta, ph2_phi, ph2_mass);
			} else {
				gamma1->SetPtEtaPhiE(ph1_pt, ph1_eta, ph1_phi, ph1_e);
				gamma2->SetPtEtaPhiE(ph2_pt, ph2_eta, ph2_phi, ph2_e);
			}
			*gamma_pair = *gamma1 + *gamma2;
			dipho_mass = gamma_pair->M();
			if( DEBUG ) cout << "ph1_e= " << gamma1->E() << "\tph2_e= " << gamma2->E() << endl;
			if( DEBUG ) cout << "ph1_pt= " << gamma1->Pt() << "\tph2_pt= " << gamma2->Pt() << "\tgg_mass= " << dipho_mass << endl;
			if( dipho_mass < 100 || dipho_mass > 180 ) continue;
			if( DEBUG ) cout << "\tselection: passing mgg cut" << endl;
			if( isReco == 1 )
			{ // apply full selection on reconstructed level
				if( ph1_pt < 40. * dipho_mass / 120. ) continue;
				if( DEBUG ) cout << "\tselection: passing ph1_pt cut" << endl;
				if( ph2_pt < 25. ) continue;
				if( DEBUG ) cout << "\tselection: passing ph2_pt cut" << endl;
				if( ph1_ciclevel < 4 || ph2_ciclevel < 4 ) continue;
				if( DEBUG ) cout << "\tselection: passing cic cut" << endl;
				weight = weight * pu_weight;
				if( type == 0 ) weight = 1;
				isEB = ph1_isEB && ph2_isEB;
				r9 = (ph1_r9 > .94) && (ph2_r9 > .94);
				category = 3 - 2 * isEB - r9;
			}
			totweight += weight;
			nselected++;
			dipho_ctheta = GetCosTheta(gamma1, gamma2);
			dipho_pt = gamma_pair->Pt();
			tree_result->Fill();
		}	

		tree_result->Write("",TObject::kOverwrite);
		tree_result->Delete();
		tree->Delete();
		file->Close();
		file->Delete();
		cout << sample_outtree[isample] << " done (events= " << nentry << " , selected= " << nselected << " , weighted= " << totweight << ")" << endl;

	}


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
