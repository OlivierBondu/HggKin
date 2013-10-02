// Original file by C. Goudet
// Adapted by O. Bondu (Aug 2013)
// C++ headers
#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
// ROOT Headers
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TLine.h"
#include "TError.h"
// RooFit headers
#include "RooConstVar.h"
#include "RooFormulaVar.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGenericPdf.h"
#include "RooProdPdf.h"
#include "RooBernstein.h"
// RooStats headers
#include "RooWorkspace.h"
#include "RooStats/SPlot.h"
// plot style
#include "setTDRStyle.h"
// definitions
#define BATCH 0 // On batch mode, have to change loading and saving path
#define NBINS 10
#define WIDTH 10
#define DEBUG 0
#define KFAC 3
// namespaces
using namespace std;
using namespace RooStats;
using namespace RooFit;
namespace po = boost::program_options;
// functions declaration
int AddModel(string inputfile, RooWorkspace*, int type, bool doBlind, string cut, string cut_name, string category, string category_name, TCanvas *c = 0);		// Add pdf to workspace and pre-fit them
int AddData(RooWorkspace*, int type, bool doBlind, string cut, string cut_name, string category, string category_name, TCanvas *c = 0);				// Add simulated events to workspace
int DoSPlot(RooWorkspace*, int const &cut = 0, int const &categ = -1, TCanvas *c = 0);				// Create SPlot object
int MakePlot(RooWorkspace*, int const &cut = 0, int const &categ = -1, bool doBlind = true, string sample = "BlindMC_reco", TCanvas *c = 0);				// Create and save result and check plots

int main(int argc, char* argv[])
{
// verbosity settings
	if(!DEBUG)
	{
		gErrorIgnoreLevel = kWarning; // ROOT
		RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING); // ROOFIT
		RooMsgService::instance().setSilentMode(true);
	}
// plots
	TGaxis::SetMaxDigits(3);
	TCanvas *canvas = new TCanvas("canvas","canvas",800,800);
// input parameters and option parsing
	bool doBlind;
	int type;
	string inputfile;
	copy(argv, argv + argc, ostream_iterator<char*>(cout, " "));
	cout << endl;

	try
	{
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("inputfile,i", po::value<string>(&inputfile)->default_value("kin_dist.root"), "input root file")
			("doBlind", po::value<bool>(&doBlind)->default_value(true), "blind output distributions")
			("type", po::value<int>(&type)->default_value(0), "-1= mc gen ; 0= mc reco ; 1= data")
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

	string sample;
	if( type == -1 ) sample = "MC_Gen";
	if( type == 0 ) sample = "MC_reco";
	if( type == 1 ) sample = "Data";
	if( doBlind ) sample = "Blind" + sample;


	//################################################
	//#############menu
	vector<string> cuts; cuts.clear();
	vector<string> cuts_name; cuts_name.clear();
	cuts.push_back("1"); cuts_name.push_back("nocut"); // no cut
//	cuts.push_back("dipho_ctheta > 0.200"); cuts_name.push_back("costheta_GT_0p200"); 
//	cuts.push_back("dipho_ctheta > 0.375"); cuts_name.push_back("costheta_GT_0p375");
//	cuts.push_back("dipho_ctheta > 0.550"); cuts_name.push_back("costheta_GT_0p550");
//	cuts.push_back("dipho_ctheta > 0.750"); cuts_name.push_back("costheta_GT_0p750");
	int const menu_cut[5]={0,200,375,550,750};

	vector<string> category; category.clear();
	vector<string> category_name; category_name.clear();
	category.push_back("1"); category_name.push_back("nocat"); // inclusive category
//	category.push_back("category > -0.1 && category < 0.1"); category_name.push_back("cat0");
//	category.push_back("category >	0.9 && category < 1.1"); category_name.push_back("cat1");
//	category.push_back("category >	1.9 && category < 2.1"); category_name.push_back("cat2");
//	category.push_back("category >	2.9 && category < 3.1"); category_name.push_back("cat3");

	TFile *root_file=0;
	root_file = new TFile( Form("WS_SPlot_%s.root", sample.c_str()), "RECREATE");
	RooWorkspace *ws=0;

// FIXME setup a loop on MC_Reco MC_Gen Data as asked for

	int i=0; // no cut on cos theta*
	int categ=-1; // inclusive category
	for( unsigned int icut = 0 ; icut < cuts.size() ; icut++ )
	{
		if(DEBUG) cout << "Processing cuts[" << icut << "]= " << cuts[icut] << endl;
		for( unsigned int icategory = 0 ; icategory < category.size() ; icategory++ )
		{
			if(DEBUG) cout << "Processing category[" << icategory << "]= " << category[icategory] << endl; 
			string ws_name = "ws_" + sample + "_" + cuts_name[icut] + "_" + category_name[icategory];
			ws = new RooWorkspace(ws_name.c_str(), ws_name.c_str());
			AddModel(inputfile, ws, type, doBlind, cuts[icut], cuts_name[icut], category[icategory], category[icategory], canvas);
			AddData(ws, type, doBlind, cuts[icut], cuts_name[icut], category[icategory], category[icategory], canvas);
			DoSPlot(ws, menu_cut[i], categ, canvas);
			MakePlot(ws, menu_cut[i], categ, doBlind, sample, canvas);
			root_file->cd();
			ws->Write("",TObject::kOverwrite);
			ws->Delete();	
		} // end of loop over categories
	} // end of loop over cuts

	root_file->Close();
	cout << "Went up to the end" << endl;
	return 0;
}

//######################################################################################################################################
//######################################################################################################################################
//######################################################################################################################################
int AddModel(string inputfile, RooWorkspace *ws, int type, bool doBlind, string cut, string cut_name, string cat, string cat_name, TCanvas *canvas)
{
	cout << "\t#####\tAddModel\t#####" << endl;
	string selection = cut + " && " + cat;
	setTDRStyle(); 
	TFile *file_kin=0;
	file_kin = new TFile(inputfile.c_str());
	
	RooRealVar *dipho_mass = new RooRealVar("dipho_mass","m_{#gamma #gamma}",100.,180.,"GeV/c^{2}");
	RooRealVar *dipho_pt = new RooRealVar("dipho_pt","p_{T#gamma#gamma}",0,NBINS*WIDTH,"GeV/c");
	RooRealVar *dipho_ctheta = new RooRealVar("dipho_ctheta","cos(#theta *)",0,1);
	RooRealVar *category = new RooRealVar("category","category",-1,5);
	dipho_mass->setBins(80);
	dipho_mass->setRange("SB_high", 135., 180.);
	dipho_mass->setRange("SB_low", 100., 115.);

	//Model of sig =2 gaussians
//	cout << "#####	Model sig" << endl;
	//create model of background and fit	

	RooRealVar *mean1_sig = new RooRealVar("mean1_sig","mean1_sig",128.,122.,128.);
	RooRealVar *sigma1_sig = new RooRealVar("sigma1_sig","sigma1_sig",9.,0.,10.);
	RooGaussian *model1_sig = new RooGaussian("model1_sig","model1_sig",*dipho_mass,*mean1_sig,*sigma1_sig);

	RooRealVar *mean2_sig = new RooRealVar("mean2_sig","mean2_sig",120.,120.,130.);
	RooRealVar *sigma2_sig = new RooRealVar("sigma2_sig","sigma2_sig",9.,0.,10.);
	RooGaussian *model2_sig = new RooGaussian("model2_sig","model2_sig",*dipho_mass,*mean2_sig,*sigma2_sig);

	RooRealVar *compo1_sig = new RooRealVar("compo1_sig","compo1_sig",0.,1.);
	RooAddPdf *model_sig = new RooAddPdf("model_sig","model_sig",RooArgSet(*model1_sig,*model2_sig),RooArgSet(*compo1_sig));

	TChain* chain_sig = 0;
	TTree* tree_sig = 0;
	if( type < 0 )
	{
		tree_sig = (TTree*) file_kin->Get("tree_gen_ggh");
		chain_sig = new TChain("tree_gen_ggh");
		chain_sig->Add(file_kin->GetName());
		chain_sig->Add( Form("%s/%s", file_kin->GetName(), "tree_gen_vbf") );
	} else {
		tree_sig = (TTree*) file_kin->Get("tree_reco_ggh");
		chain_sig = new TChain("tree_reco_ggh");
		chain_sig->Add(file_kin->GetName());
		chain_sig->Add( Form("%s/%s", file_kin->GetName(), "tree_reco_vbf") );
		if(KFAC) for(int ifac = 0 ; ifac < KFAC ; ifac++) chain_sig->Add( Form("%s/%s", file_kin->GetName(), "tree_reco_ggh") );
	}

	RooRealVar *weight = new RooRealVar("weight", "weight",0,100);
	RooDataSet*	sim_sig = new RooDataSet("sim_sig", "sim_sig", chain_sig , RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category,*weight), selection.c_str(), "weight");
//	RooDataSet*	sim_sig__ = new RooDataSet("sim_sig__", "sim_sig__", tree_sig , RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category,*weight), selection.c_str());
//	tree_sig->Delete();

//	if( type < 0 ) tree_sig = (TTree*) file_kin->Get("tree_gen_vbf");
//	else tree_sig = (TTree*) file_kin->Get("tree_reco_vbf");
	RooDataSet *dataset_vbf = new RooDataSet("dataset_vbf", "dataset_vbf", tree_sig, RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category,*weight), selection.c_str(), "weight");
//	sim_sig->append(*dataset_vbf);

	tree_sig->Delete(); tree_sig = 0;
	dataset_vbf->Delete();
	model_sig->fitTo(*sim_sig, Range(110,140), SumW2Error(kTRUE));

	//Check plot
	RooPlot *framesig = dipho_mass->frame();
	sim_sig->plotOn(framesig, Binning(80), Invisible());
	model_sig->plotOn(framesig, LineColor(kBlue), LineStyle(kDashed));
//	RooDataSet* dataset_SMH = (RooDataSet *) sim_sig->reduce("(dipho_mass<115 || dipho_mass>135)");
//	dataset_SMH->plotOn(framesig, Invisible());
//	dipho_mass->setRange("SB_low", 100., 115.);
//	dipho_mass->setRange("SB_high", 135., 180.);
	if(doBlind) sim_sig->plotOn(framesig, Binning(80), MarkerColor(kBlue), CutRange("SB_low"), CutRange("SB_high"));
	else sim_sig->plotOn(framesig, Binning(80), MarkerColor(kBlue));
	framesig->SetMinimum(1e-5);	
	framesig->Draw();
//	canvas->SaveAs("frame_masssig_data.pdf");
	canvas->SaveAs(Form("pdf/prefit_mass_%sMC_signal.pdf", doBlind ? "Blind" : "" ));
	canvas->SaveAs(Form("root/prefit_mass_%sMC_signal.root", doBlind ? "Blind" : "" ));
	canvas->SaveAs(Form("gif/prefit_mass_%sMC_signal.gif", doBlind ? "Blind" : "" ));
//	cout << framesig->chiSquare() << endl;
	framesig->Delete();

//	ws->import(*dataset_SMH,Rename("sim_sig"));

	//#################################
	// Modelackgroubd Bern4 
//	cout << "#####	 Model bkg" << endl;
	RooRealVar *coef0_bern_bkg = new RooRealVar("coef0_bern_bkg","coef0_bern_bkg",0.001,0,1);
	RooRealVar *coef1_bern_bkg = new RooRealVar("coef1_bern_bkg","coef1_bern_bkg",0.001,0,1);
	RooRealVar *coef2_bern_bkg = new RooRealVar("coef2_bern_bkg","coef2_bern_bkg",0.001,0,1);
	RooRealVar *coef3_bern_bkg = new RooRealVar("coef3_bern_bkg","coef3_bern_bkg",0.001,0,1);
	RooBernstein *model_bkg = new RooBernstein("model_bkg","model_bkg",*dipho_mass,RooArgSet(*coef0_bern_bkg,*coef1_bern_bkg,*coef2_bern_bkg,*coef3_bern_bkg));

	TTree * tree_bkg = 0;
	TChain * chain_bkg = 0;
	if(type == -1)
	{
		tree_bkg = (TTree*) file_kin->Get("tree_gen_bkg");
		chain_bkg = new TChain("tree_gen_bkg");
		chain_bkg->Add(file_kin->GetName());
		chain_bkg->Add( Form("%s/%s", file_kin->GetName(), "tree_gen_ggh") );
		chain_bkg->Add( Form("%s/%s", file_kin->GetName(), "tree_gen_vbf") );
	} else if(type == 0) {
		tree_bkg = (TTree*) file_kin->Get("tree_reco_bkg");
		chain_bkg = new TChain("tree_reco_bkg");
		chain_bkg->Add(file_kin->GetName());
		chain_bkg->Add( Form("%s/%s", file_kin->GetName(), "tree_reco_ggh") );
		chain_bkg->Add( Form("%s/%s", file_kin->GetName(), "tree_reco_vbf") );
		if(KFAC) for(int ifac = 0 ; ifac < KFAC ; ifac++) chain_bkg->Add( Form("%s/%s", file_kin->GetName(), "tree_reco_ggh") );
	}	else if(type == 1) {
		tree_bkg = (TTree*) file_kin->Get("tree_data");
		chain_bkg = new TChain("tree_data");
		chain_bkg->Add(file_kin->GetName());
	}
//	int nentries = tree_bkg->GetEntries();
//	cout << "after tree_bkg" << endl;

	RooDataSet*	data = 0;
	if( type > 0 ) data = new RooDataSet("data","data", chain_bkg , RooArgSet(*dipho_mass,*dipho_ctheta,*category), selection.c_str());
	else data = new RooDataSet("data","data", chain_bkg , RooArgSet(*dipho_mass,*dipho_ctheta,*category,*weight), selection.c_str(), "weight");
//	RooDataSet* blinddata = (RooDataSet*) data->reduce("(dipho_mass<115 || dipho_mass>135)");
	tree_bkg->Delete(); tree_bkg = 0;
	model_bkg->fitTo(*data, SumW2Error(kTRUE));
//	 coef0_bern_bkg->setRange(coef0_bern_bkg->getVal()-1*coef0_bern_bkg->getError(),coef0_bern_bkg->getVal()+1*coef0_bern_bkg->getError());
//	 coef1_bern_bkg->setRange(coef1_bern_bkg->getVal()-1*coef1_bern_bkg->getError(),coef1_bern_bkg->getVal()+1*coef1_bern_bkg->getError());
//	 coef2_bern_bkg->setRange(coef2_bern_bkg->getVal()-1*coef2_bern_bkg->getError(),coef2_bern_bkg->getVal()+1*coef2_bern_bkg->getError());
//	 coef3_bern_bkg->setRange(coef3_bern_bkg->getVal()-1*coef3_bern_bkg->getError(),coef3_bern_bkg->getVal()+1*coef3_bern_bkg->getError());
	
 //Check plot
	RooPlot *frame = dipho_mass->frame(100,180,80);
	data->plotOn(frame, Invisible());
	model_bkg->plotOn(frame, LineColor(kRed), LineStyle(kDashed));
//	if(doBlind) blinddata->plotOn(frame);
	if(doBlind) data->plotOn(frame, CutRange("SB_low"), CutRange("SB_high"));
//	if(doBlind) blinddata->plotOn(frame, CutRange("SB_high"));
	else data->plotOn(frame);
//	model_bkg->plotOn(frame);
	frame->SetMinimum(1.e-5);
	frame->Draw();
	canvas->SaveAs(Form("pdf/prefit_mass_%sBackground.pdf", doBlind ? "Blind" : "" ));
	canvas->SaveAs(Form("gif/prefit_mass_%sBackground.gif", doBlind ? "Blind" : "" ));
	canvas->SaveAs(Form("root/prefit_mass_%sBackground.root", doBlind ? "Blind" : "" ));
//	canvas->SaveAs("frame_massbkg_data.pdf");
//	cout << frame->chiSquare() << endl;
	frame->Delete();


	RooRealVar *sig_yield = new RooRealVar("sig_yield","sig_yield",0., 500. * (1. + KFAC));
	RooRealVar *bkg_yield = new RooRealVar("bkg_yield","bkg_yield",0.,2.e5);
	RooAddPdf *model_sigbkg= new RooAddPdf("model_sigbkg","model_sigbkg",RooArgList(*model_sig,*model_bkg),RooArgList(*sig_yield,*bkg_yield));	
	// Import all pdf and variables (implicitely) into workspace

	ws->import(*category);
	ws->import(*dipho_ctheta);
	ws->import(*dipho_pt);
	ws->import(*dipho_mass);
	ws->import(*sim_sig);
	ws->import(*model_sigbkg);
	file_kin->Close();
//	canvas->Close();
	canvas->Clear();
//	cout << "end AddModel" <<	endl;
	return 0;
}


//######################################################################################################
//######################################################################################################
//######################################################################################################
int AddData(RooWorkspace *ws, int type, bool doBlind, string cut, string cut_name, string cat, string cat_name, TCanvas *canvas)
{
	cout << "\t#####\tAddData\t#####" << endl;
	if(DEBUG) cout << "workspace print" << endl;
	if(DEBUG) ws->Print();
	if(DEBUG) cout << "Initialization" << endl;
	string selection = cut + " && " + cat;
//	cout << "in AddData" << endl;
	RooRealVar *dipho_mass = ws->var("dipho_mass");
	RooRealVar *dipho_pt = ws->var("dipho_pt");
	RooRealVar *dipho_ctheta = ws->var("dipho_ctheta");
	RooRealVar *category = ws->var("category");
	RooRealVar *weight = new RooRealVar("weight", "weight",0.,1.e5);
	TFile *file_kin=0;
	file_kin = new TFile("kin_dist.root");

	if(DEBUG) cout << "Create trees and chains" << endl;
	TChain * chain_data = 0;
	TTree *tree_data = 0;
	if(type == -1)
	{
		tree_data = (TTree *) file_kin->Get("tree_gen_bkg");
		chain_data = new TChain("tree_gen_bkg");
		chain_data->Add(file_kin->GetName());
	} else if(type == 0) {
		tree_data = (TTree *) file_kin->Get("tree_reco_bkg");
		chain_data = new TChain("tree_reco_bkg");
		chain_data->Add(file_kin->GetName());
		chain_data->Add( Form("%s/%s", file_kin->GetName(), "tree_reco_ggh") );
		chain_data->Add( Form("%s/%s", file_kin->GetName(), "tree_reco_vbf") );
		if(KFAC) for(int ifac = 0 ; ifac < KFAC ; ifac++) chain_data->Add( Form("%s/%s", file_kin->GetName(), "tree_reco_ggh") );
	} else if(type == 1) {
		tree_data = (TTree *) file_kin->Get("tree_data");
		chain_data = new TChain("tree_data");
		chain_data->Add(file_kin->GetName());
	}
	if(DEBUG) cout << "dumb test" << endl;
	if(DEBUG) chain_data->Draw("dipho_mass", "weight");
	if(DEBUG) canvas->Print("test.pdf");
//	cout << "tree test" << tree_data->GetEntries() << endl;
//	RooDataSet *dataset = new RooDataSet("dataset","dataset",tree_data,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category), selection.c_str());
	if(DEBUG) cout << "Creating RooDataSet (type= " << type << " )" << endl;
	RooDataSet *dataset = 0;
	if( type > 0 )
		dataset = new RooDataSet("dataset","dataset",chain_data,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category), selection.c_str());
	else
	{
		if(DEBUG) cout << "HERE" << endl;
		dataset = new RooDataSet("dataset","dataset",chain_data,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category, *weight), selection.c_str(), "weight");
	}
//	else dataset = new RooDataSet("dataset","dataset",chain_data,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category), selection.c_str());
//	if(DEBUG) cout << "Creating blinded RooDataSet" << endl;
//	string blindselection = selection + " && (dipho_mass<115 || dipho_mass>135)";	
//	RooDataSet *dataset_blind = new RooDataSet("dataset_blind","dataset_blind",tree_data,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category),blindselection.c_str());
//	RooDataSet *dataset_blind = new RooDataSet("dataset_blind","dataset_blind",chain_data,RooArgSet(*dipho_mass,*dipho_pt,*dipho_ctheta,*category),blindselection.c_str());
//	cout << "dataset created " << endl;

	//variable for blinding correction
//	RooRealVar *ntree_data = new RooRealVar("ntree_data","ntree_data",tree_data->GetEntries(),0,tree_data->GetEntries());
//	RooRealVar *ntree_data = new RooRealVar("ntree_data","ntree_data",chain_data->GetEntries(),0,chain_data->GetEntries());

	if(DEBUG) cout << "Delete stuff" << endl;
	chain_data->Delete();		
	tree_data->Delete();		
	//Check Plot
//	 RooPlot *framemass = dipho_mass->frame(100,180,40);
//	 dataset->plotOn(framemass);
//	 framemass->Draw();
//	 canvas->SaveAs("frame_mass_reco.pdf");
//	 RooPlot *framept = dipho_pt->frame(0,NBINS*WIDTH,NBINS);
//	 dataset->plotOn(framept);
//	 framept->Draw();
//	 canvas->SaveAs("frame_pt_reco.pdf");	
//		canvas->Clear();
		
	//import dataset into workspace
	if(DEBUG) cout << "import datasets into workspace" << endl;
	ws->import(*dataset);
//	ws->import(*dataset_blind);
//	ws->import(*ntree_data);

	if(DEBUG) cout << "closing input file" << endl;
	file_kin->Close();
	if(DEBUG) cout << "exiting AddData" << endl;
	return 0;
}
		


//###################################################################################################
//###################################################################################################
//###################################################################################################
int DoSPlot(RooWorkspace* ws, int const &cut, int const &categ, TCanvas *canvas)
{
	cout << "\t#####\tDoSPlot\t#####" << endl;
//	cout << "in DoSplot" << endl;
	setTDRStyle();
	RooAbsPdf *model_sigbkg = ws->pdf("model_sigbkg");
	RooDataSet *dataset = (RooDataSet*) ws->data("dataset");
//	RooDataSet *dataset_blind = (RooDataSet *) ws->data("dataset_blind");
	RooRealVar *dipho_mass = ws->var("dipho_mass");	

	//Fixing discriminant parameters
	 RooRealVar *coef0_bern_bkg = ws->var("coef0_bern_bkg");	 coef0_bern_bkg->setConstant(1);
	 RooRealVar *coef1_bern_bkg = ws->var("coef1_bern_bkg");	 coef1_bern_bkg->setConstant(1);
	 RooRealVar *coef2_bern_bkg = ws->var("coef2_bern_bkg");	 coef2_bern_bkg->setConstant(1);
	 RooRealVar *coef3_bern_bkg = ws->var("coef3_bern_bkg");	 coef3_bern_bkg->setConstant(1);
	 RooRealVar *compo1_sig = ws->var("compo1_sig");	 compo1_sig->setConstant(1);
	 RooRealVar* mean1_sig = ws->var("mean1_sig");	 mean1_sig->setConstant(1);
	 RooRealVar* sigma1_sig = ws->var("sigma1_sig");	 sigma1_sig->setConstant(1);
	 RooRealVar* mean2_sig = ws->var("mean2_sig");	 mean2_sig->setConstant(1);
	 RooRealVar* sigma2_sig = ws->var("sigma2_sig");	 sigma2_sig->setConstant(1);
	 
	 RooRealVar *sig_yield = ws->var("sig_yield");
	 RooRealVar *bkg_yield = ws->var("bkg_yield");
	 
//	 cout << "Model variables retrieved and set constant" << endl;

	 model_sigbkg->fitTo(*dataset, SumW2Error(kTRUE));	
	// Check plot
	char buffer_file[2][3][10]={{""}},buffercut[10]="",buffercateg[30]="";
	sprintf(buffer_file[1][0],"gif");
	sprintf(buffer_file[1][1],"pdf");
	sprintf(buffer_file[1][2],"root");
	if (! BATCH) {
		sprintf(buffer_file[0][0],"gif/");
		sprintf(buffer_file[0][1],"pdf/");
		sprintf(buffer_file[0][2],"root/");
	}
	if (cut) sprintf(buffercut,"%d",cut);
	if (categ>-1) sprintf(buffercateg,"_categ%d",categ);




//	cout << "starting splot" << endl;
	RooDataSet *Wdataset = new RooDataSet(*dataset,"Wdataset");
	SPlot("splot_sigbkg","splot_sigbkg", *Wdataset, model_sigbkg, RooArgList(*sig_yield, *bkg_yield));	//Create splot
	ws->import(*Wdataset,Rename("Wdataset"));
//	cout << sig_yield->getVal() << endl;

	TLatex latex; latex.SetNDC(1); char buffer[100];
	TLegend *legend = new TLegend(0.6,0.6,0.89,0.89);
	legend->SetFillColor(0);
	legend->SetBorderSize(0);
//	legend->SetTextSize(0.04);
	RooPlot* frame_mass = dipho_mass->frame(100,180,40);
//	frame_mass->UseCurrentStyle();
//	frame_mass->GetYaxis()->SetLabelSize(0.03);
	dataset->plotOn(frame_mass,Name("data"));
//	 model_sigbkg->plotOn(frame_mass,Name("full"));
//	 model_sigbkg->plotOn(frame_mass, Components("model_sig"), LineColor(kGreen), LineStyle(kDashed),Name("sig"));
	model_sigbkg->plotOn(frame_mass, Components("model_bkg"), LineColor(kRed), LineStyle(kDashed),Name("bkg"),Normalization(dataset->sumEntries(),RooAbsReal::NumEvent));
	model_sigbkg->plotOn(frame_mass, Components("model_sig"), LineColor(kBlue), LineStyle(kDashed),Name("sig"),Normalization(dataset->sumEntries(),RooAbsReal::NumEvent));
	model_sigbkg->plotOn(frame_mass, LineColor(kRed),Name("sig+bkg"),Normalization(dataset->sumEntries(),RooAbsReal::NumEvent));
	frame_mass->SetMinimum(1.e-5);
	frame_mass->Draw();
	//	latex.DrawLatex(0.45,0.86,"sig yield");
	sprintf(buffer,"fitted = %3.0f",sig_yield->getVal()); 
	//latex.DrawLatex(0.45,0.81,buffer);
	legend->AddEntry(frame_mass->findObject("data"),"data","lpe");
	//	legend->AddEntry(frame_mass->findObject("full"),"Full Fit","l");
	// legend->AddEntry(frame_mass->findObject("sig"),"sig Fit","l");
	legend->AddEntry(frame_mass->findObject("sig+bkg"), "sig+bkg fit", "l");
	legend->AddEntry(frame_mass->findObject("bkg"), "bkg fit", "l");
	legend->AddEntry(frame_mass->findObject("sig"), "sig fit", "l");
	legend->Draw();

	if (cut) {
		sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
		canvas->cd();
		latex.DrawLatex(0.05,0.96,buffer);
	}
	if(categ>-1) {
		sprintf(buffer,"category %d",categ);
		canvas->cd();
		latex.DrawLatex(0.4,0.96,buffer);
	}
	for (int file=0; file<3; file++ ) {
		sprintf(buffer,"%sSPlotInput_pt%s%s_datasig.%s",buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
		canvas->SaveAs(buffer);
	}
	

//	canvas->Close();
	canvas->Clear();
//	cout << "end DoSPlot" << endl;
	return 0;
}

//#############################################################################################"
//#############################################################################################"
//#############################################################################################"
int MakePlot(RooWorkspace* ws, int const &cut, int const &categ, bool doBlind, string sample, TCanvas *canvas)
{
	cout << "\t#####\tMakePlot\t#####" << endl;
	setTDRStyle();
	if(DEBUG) cout << "Initialization" << endl;
	// collect useful variables
	RooRealVar *dipho_pt = ws->var("dipho_pt");
	RooRealVar *dipho_mass = ws->var("dipho_mass");
	RooRealVar *sig_yield = ws->var("sig_yield");
	RooRealVar *bkg_yield = ws->var("bkg_yield");
	RooDataSet *Wdataset = (RooDataSet*) ws->data("Wdataset");
	RooDataSet *dataset = (RooDataSet*) ws->data("dataset");
	dipho_mass->setRange("SB_low",100,115);
	dipho_mass->setRange("SB_high",135,180);
//	if(doBlind) dataset = (RooDataSet*) ws->data("dataset_blind");
//	else dataset = (RooDataSet*) ws->data("dataset");

	if(DEBUG) cout << "display settings" << endl;
	TLatex latex;
	latex.SetNDC();	
	char buffer[100];
	TPad *pad_up = new TPad("pad_up","pad_up",0,0.3,1,1);
	TPad *pad_down = new TPad("pad_down","pad_down",0,0,1,0.3);
	pad_down->SetTopMargin(0);
	pad_down->SetBottomMargin(0.35);
	pad_up->SetTopMargin(0.07);
	pad_up->SetBottomMargin(0);
	TLegend *legend=0;

	//define path prefixes	for saving canvas
	char buffer_file[2][3][10]={{""}},buffercut[10]="",buffercateg[30]="";
	sprintf(buffer_file[1][0],"gif");
	sprintf(buffer_file[1][1],"pdf");
	sprintf(buffer_file[1][2],"root");
	if (! BATCH) {
		sprintf(buffer_file[0][0],"gif/");
		sprintf(buffer_file[0][1],"pdf/");
		sprintf(buffer_file[0][2],"root/");
	}
	if (cut) sprintf(buffercut,"%d",cut);
	if (categ>-1) sprintf(buffercateg,"_categ%d",categ);
	//----------------------------


	if(DEBUG) cout << "prepare stuff for blinded plots" << endl;
	//compute integral of pdf in sidebands for later normalization of blind plots
	RooAbsReal *int_sig;
	RooAbsReal *int_bkg;
//	RooRealVar *ntree_data;
	RooDataSet *sw_bkg;
	RooDataSet *sw_sig;
	if(doBlind)
	{
		if(DEBUG) cout << "treatment for blinded stuff" << endl;
		RooAddPdf *model_sig = (RooAddPdf *) ws->pdf("model_sig");
		int_sig = model_sig->createIntegral(*dipho_mass,Range("SB_low,SB_high"),NormSet(*dipho_mass));
//		cout << "int_sig->getVal()= " << int_sig->getVal() << endl;
		RooAddPdf *model_bkg = (RooAddPdf *) ws->pdf("model_bkg");
		int_bkg = model_bkg->createIntegral(*dipho_mass,Range("SB_low,SB_high"),NormSet(*dipho_mass));
//		cout << "int_bkg->getVal()= " << int_bkg->getVal() << endl;

//		ntree_data = (RooRealVar *) ws->var("ntree_data");
//		cout << ntree_data->getVal() << endl;
		//Creating functions for corrected weights
		RooRealVar *sig_yield_sw = new RooRealVar("sig_yield_sw","sig_yield_sw",0.,-100. * (1. + KFAC), 100. * (1. + KFAC));
		RooRealVar *bkg_yield_sw = new RooRealVar("bkg_yield_sw","bkg_yield_sw",0.,-100.,100.);
	
		RooDataSet *dum_swbkg = new RooDataSet("dum_swbkg","dum_swbkg",Wdataset,*Wdataset->get(),"dipho_mass<115 || dipho_mass>135","bkg_yield_sw");
		RooRealVar *sum_swbkg = new RooRealVar("sum_swbkg","sum_swbkg",dum_swbkg->sumEntries());
		RooDataSet *dum_swsig = new RooDataSet("dum_swsig","dum_swsig",Wdataset,*Wdataset->get(),"dipho_mass<115 || dipho_mass>135","sig_yield_sw");
		RooRealVar *sum_swsig = new RooRealVar("sum_swsig","sum_swsig",dum_swsig->sumEntries());
	
		RooFormulaVar *sw_blindsig = new RooFormulaVar("sw_blindsig","sw_blindsig","@0*@1*@2/@3",RooArgSet(*sig_yield_sw,*sig_yield,*int_sig,*sum_swsig));
		Wdataset->addColumn(*sw_blindsig); 
		RooFormulaVar *sw_blindbkg = new RooFormulaVar("sw_blindbkg","sw_blindbkg","@0*@1*@2/@3",RooArgSet(*bkg_yield_sw,*bkg_yield,*int_bkg,*sum_swbkg));
		Wdataset->addColumn(*sw_blindbkg);	
	
		//create datasets weighted with corrected weights
		//Create datasets blinded sweighted
		sw_bkg = new RooDataSet("sw_bkg","sw_bkg",Wdataset,*Wdataset->get(),"dipho_mass<115 || dipho_mass>135","sw_blindbkg");
		sw_sig = new RooDataSet("sw_sig","sw_sig",Wdataset,*Wdataset->get(),"dipho_mass<115 || dipho_mass>135","sw_blindsig");
		cout << "blinded sweighted dataset created" << endl;
//		cout << "somme weight : " << sw_bkg->sumEntries()+sw_sig->sumEntries() << "	 " << Wdataset->sumEntries() << " " << endl;
	
	}
	if(DEBUG) cout << "datasets to be used for plotting" << endl;
	sw_bkg = new RooDataSet("sw_bkg","sw_bkg",Wdataset,*Wdataset->get(),"","bkg_yield_sw");
	sw_sig = new RooDataSet("sw_sig","sw_sig",Wdataset,*Wdataset->get(),"","sig_yield_sw");
	//---------------------------------


	//Plotting bkg sPlot output
	if(DEBUG) cout << "Plotting bkg sPlot output" << endl; 
	pad_up->Draw();
	pad_up->cd();
	RooPlot *frame_pt = dipho_pt->frame(NBINS);
	sw_bkg->plotOn(frame_pt,MarkerSize(1.5),MarkerColor(1),LineColor(1),DataError(RooAbsData::SumW2),Name("sw_bkg"),Binning(NBINS,0,dipho_pt->getMax()));
	dataset->plotOn(frame_pt,MarkerColor(4),LineColor(4),DataError(RooAbsData::SumW2),Name("blind_bkg"),Binning(NBINS,0,dipho_pt->getMax()));
//	cout << "sum	"	<< dataset->sumEntries() << endl;
//	frame_pt->UseCurrentStyle();
	frame_pt->Draw();

	if(DEBUG) cout << "prepare legend" << endl;
	legend = new TLegend(0.6,0.6,0.89,0.89);
	legend->SetFillColor(0);
	legend->SetBorderSize(0);
//	legend->SetTextSize(0.05);
//	legend->AddEntry("","Data","");
	legend->AddEntry("sw_bkg", "Data * sWeight(background)","lpe");
	legend->AddEntry("blind_bkg",Form("Background%s", doBlind ? " blinded" : ""),"lpe");
	legend->Draw();

	if(DEBUG) cout << "create ratio histogram" << endl;
	// create ratio histogram (sw-data)/data
	TH1F *hist_fit = new TH1F("hist_fit","hist_fit",NBINS,0,NBINS*WIDTH);
	dataset->fillHistogram(hist_fit,*dipho_pt);
	TH1F *hist_set = new TH1F("hist_set","hist_set",NBINS,0,NBINS*WIDTH);
	sw_bkg->fillHistogram(hist_set,*dipho_pt);	
	TLine *line = new TLine(0,0,NBINS*WIDTH,0);
	line->SetLineColor(kRed);
	hist_set->Add(hist_fit,-1);
	hist_set->Divide(hist_fit);	

	if(DEBUG) cout << "redo axis stuff for the ratios" << endl;
	hist_set->GetYaxis()->SetTitle("#frac{N_{sPlot}-N_{dat}}{N_{dat}}");
	hist_set->GetYaxis()->SetTitleOffset(0.35);
	hist_set->GetXaxis()->SetTitle(frame_pt->GetXaxis()->GetTitle());
//	hist_set->GetXaxis()->SetLabelSize(0.15);
	hist_set->GetYaxis()->SetLabelSize(0.1);
//	hist_set->GetXaxis()->SetTitleSize(0.15);
//	hist_set->GetXaxis()->SetTitleOffset(1);
	hist_set->GetYaxis()->SetTitleSize(0.15);
	
	if(DEBUG) cout << "draw the ratio hist" << endl;
	canvas->cd();
	pad_down->Draw();
	pad_down->cd();
	hist_set->Draw();
	line->Draw("SAME");

	if(DEBUG) cout << "add legend for categories" << endl;
	if (cut) {
		sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
		canvas->cd();
		latex.DrawLatex(0.05,0.96,buffer);
	}
	if (categ>-1) {
		sprintf(buffer,"category %d",categ);
		canvas->cd();
		latex.DrawLatex(0.4,0.96,buffer);
	}
	if(DEBUG) cout << "save plot" << endl;
	for (int file=0; file<3; file++ ) {
		sprintf(buffer,"%ssplot_pt%s%s_datasigbkg.%s",buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
		canvas->SaveAs(buffer);
	}

	if(DEBUG) cout << "cleaning a bit all the remaining stuff" << endl;
	frame_pt->Delete();
	legend->Delete();
 
	hist_fit->Delete();
	hist_set->Delete();	

//	cout << "sw bkg drawn" << endl;

	if(DEBUG) cout << "Plotting sig sPlot output" << endl;
	// plot weighted signal events and first fit
	pad_up->cd();
	frame_pt = dipho_pt->frame(NBINS);
	sw_sig->plotOn(frame_pt,MarkerColor(kRed),LineColor(kRed),DataError(RooAbsData::SumW2),Name("sw_sig"),Binning(NBINS,0,dipho_pt->getMax()));// plot weighted sig events
	RooDataSet *sim_sig = (RooDataSet *) ws->data("sim_sig");
	sim_sig->plotOn(frame_pt,MarkerColor(kGreen),LineColor(kGreen),DataError(RooAbsData::SumW2),Name("sig_sm"),Binning(NBINS,0,dipho_pt->getMax()));

//	frame_pt->UseCurrentStyle();
//	frame_pt->GetXaxis()->SetTitleSize(0);
//	frame_pt->GetXaxis()->SetLabelSize(0);
	frame_pt->SetMinimum(1.e-5);
	frame_pt->Draw();

	if(DEBUG) cout << "prepare legend" << endl;
	legend = new TLegend(0.6,0.6,0.89,0.89);
	legend->SetFillColor(0);
	legend->SetBorderSize(0);
//	legend->SetTextSize(0.05);
//	legend->AddEntry("","Data","");
	legend->AddEntry("sw_sig", "Data #times sWeight(signal)","lpe");
	legend->AddEntry("sig_sm","SM Higgs MC","lpe");
	legend->Draw();

	if(DEBUG) cout << "create ratio histogram" << endl;
	//compare signal with SMModel
	hist_fit = new TH1F("hist_fit","hist_fit",NBINS,0,NBINS*WIDTH);
	sim_sig->fillHistogram(hist_fit,*dipho_pt);
	hist_set = new TH1F("hist_set","hist_set",NBINS,0,NBINS*WIDTH);
	sw_sig->fillHistogram(hist_set,*dipho_pt);	
	hist_set->SetTitle("test");
	//	line = new TLine(0,0,NBINS*WIDTH,0);
	line->SetLineColor(kRed);
	hist_set->Add(hist_fit,-1);
	hist_set->Divide(hist_fit);	
	hist_set->GetYaxis()->SetTitle("#frac{N_{MC}-N_{fit}}{N_{fit}}");
	hist_set->GetYaxis()->SetTitleOffset(0.3);
	hist_set->GetXaxis()->SetTitle(frame_pt->GetXaxis()->GetTitle());
	//	hist_set->GetYaxis()->SetRangeUser(-1,1);
//	cout << "divided" << endl;
//	hist_set->GetXaxis()->SetLabelSize(0.15);
	hist_set->GetYaxis()->SetLabelSize(0.1);
//	hist_set->GetXaxis()->SetTitleSize(0.15);
//	hist_set->GetXaxis()->SetTitleOffset(1);
	hist_set->GetYaxis()->SetTitleSize(0.15);
	
	if(DEBUG) cout << "draw the ratio hist" << endl;
	canvas->cd();
	pad_down->Draw();
	pad_down->cd();
	hist_set->Draw();
	line->Draw("SAME");

//	cout << "hist_set drawn" << endl;

	if(DEBUG) cout << "add legend for categories" << endl;
	if (cut) {
		sprintf(buffer,"cos(#theta*) > %1.3f",cut/1000.);
		canvas->cd();
		latex.DrawLatex(0.05,0.96,buffer);
	}
	if(categ>-1) {
		sprintf(buffer,"category %d",categ);
		canvas->cd();
		latex.DrawLatex(0.4,0.96,buffer);
	}
	if(DEBUG) cout << "save plot" << endl;
	for (int file=0; file<3; file++ ) {
		sprintf(buffer,"%ssplot_pt%s%s_datasig.%s",buffer_file[0][file],buffercut,buffercateg,buffer_file[1][file]);
		canvas->SaveAs(buffer);
	}
//	cout << "weigthed sig drawn" << endl;
	canvas->Clear();
//	cout << "end MakePlot" << endl;
	return 0;
}
		
