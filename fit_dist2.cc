using namespace std;

TFile *file_result=new TFile("../data/kin_dist.root");
//  char const *menu_cut="";//"", cuttheta{0.200,0.375,0.550,0.750}
int fit_dist2() {

  char *cutval[5]={"","cuttheta0.200","cuttheta0.375","cuttheta0.550","cuttheta0.750"}
  int dofit(int const &menu_bkg, int const &menu_pol_bkg, int const &menu_sgn, int const &menu_pol_sgn,char const *menu_cut);

  for (int menubkg=0; menubkg<2;menubkg++) {
    for (int menupolbkg=0; menupolbkg<2; menupolbkg++) {
      for (int menusgn=0;menusgn<1;menusgn++) {
	for (int menupolsgn=0;menupolsgn<2;menupolsgn++) {
	  for (int cut=0;cut<5;cut++) {
	    if (! dofit(menubkg,menupolbkg,menusgn,menupolsgn,cutval[cut])) cout << "erreur" << endl;;
	  }}}}
  }
}

int dofit(int const &menu_bkg,int const &menu_bkg_pol, int const &menu_sgn, int const &menu_pol_sgn,char const *menu_cut) {

  RooRealVar pt("pt","pt",0,200);

  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.05);
  char buffer[100];
  TLine *line=new TLine(0,1,200,1);
  line->SetLineColor(kRed);
  line->SetLineStyle(9);
  
  
  //########BKG
  TCanvas *canvas_bkg=new TCanvas("canvas_bkg","canvas_bkg");
  TPad *pad_fit_bkg=new TPad("pad_fit_bkg","pad_fit_bkg",0,0.3,1,1);
  TPad *ratio_pad_bkg=new TPad("ratio_pad_bkg","ratio_pad_bkg",0,0,1,0.3);
  pad_fit_bkg->SetBottomMargin(0.05);
  pad_fit_bkg->Draw();
  canvas_bkg->cd();
  ratio_pad_bkg->SetTopMargin(0);
  ratio_pad_bkg->Draw();
  
  RooPlot* frame_bkg=pt.frame();

  RooRealVar mean_land_bkg("mean_land_bkg","mean_land_bkg",10,0,200);
  RooRealVar sigma_land_bkg("sigma_land_bkg","sigma_land_bkg",2,0,100);
  RooLandau *land_bkg=new RooLandau("land_bkg","land_bkg",pt,mean_land_bkg,sigma_land_bkg);

  RooRealVar coef0_logn_bkg("coef0_logn_bkg","coef0_logn_bkg",0,1,100);
  RooRealVar coef1_logn_bkg("coef1_logn_bkg","coef1_logn_bkg",1,1,100);
  RooRealVar coef2_logn_bkg("coef2_logn_bkg","coef2_logn_bkg",0,-30,30);
  RooRealVar coef3_logn_bkg("coef3_logn_bkg","coef3_logn_bkg",3,1,100);
  RooGenericPdf *logn_bkg=new RooGenericPdf("logn_bkg","TMath::LogNormal(pt,coef1_logn_bkg,coef2_logn_bkg,coef3_logn_bkg)", RooArgSet(pt,coef1_logn_bkg,coef2_logn_bkg,coef3_logn_bkg));


  RooRealVar coef0_pol_bkg("coef0_pol_bkg","coef0_pol_bkg",0,-100,100);
  RooRealVar coef1_pol_bkg("coef1_pol_bkg","coef1_pol_bkg",2,-100,100);
  RooRealVar coef2_pol_bkg("coef2_pol_bkg","coef2_pol_bkg",2,-100,100);
  RooRealVar coef3_pol_bkg("coef3_pol_bkg","coef3_pol_bkg",3,-100,100);
  RooPolynomial *pol2_bkg;

  sprintf(buffer,"hist_pt%s_bkg3_gen",menu_cut);
  TH1F* hist_bkg=(TH1F*) file_result->Get(buffer);
  hist_bkg->Sumw2();
  RooDataHist *bkg=new RooDataHist("bkg","bkg",pt,hist_bkg);
  bkg->plotOn(frame_bkg);

  RooProdPdf *model_bkg;
  char buffer_savebkg[100];

  switch (2*menu_bkg_pol+menu_bkg) {
  case 0 : 
    pol2=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglandpol0");
    break;  
    
  case 1 : 
    pol2=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglognpol0");
    break;  
    
  case 2 : 
    pol2=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglandpol1");
    break;  
  case 3 : 
    pol2=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglognpol1");
    break;  
  case 4 : 
    pol2=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg,coef2_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*land_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglandpol2");
    break;  
  case 5 : 
    pol2=new RooPolynomial("pol2_bkg","pol2_bkg",pt,RooArgList(coef0_pol_bkg,coef1_pol_bkg,coef2_pol_bkg));
    model_bkg =new RooProdPdf("model_bkg","model_bkg",RooArgList(*logn_bkg,*pol2_bkg));
    sprintf(buffer_savebkg,"bkglognpol2");
    break;  
  }


  model_bkg->fitTo(*bkg);
  model_bkg->plotOn(frame_bkg);


  TH1F *ratio_bkg=model_bkg->createHistogram("ratio_bkg", pt,RooFit::Binning((int)hist_bkg->GetNbinsX(),[0,200]));

  ratio_bkg->Scale(hist_bkg->Integral());
  for (int i=0;i<hist_bkg->GetNbinsX();i++) {
    if (ratio_bkg->GetBinContent(i)>0) {
      ratio_bkg->SetBinContent(i,hist_bkg->GetBinContent(i)/ratio_bkg->GetBinContent(i));
    }}
  
 
  coef0_logn_bkg.setConstant(1);
  coef1_logn_bkg.setConstant(1);
  coef2_logn_bkg.setConstant(1);
  coef3_logn_bkg.setConstant(1);
  mean_land_bkg.setConstant(1);
  sigma_land_bkg.setConstant(1);
  coef0_pol_bkg.setConstant(1);
  coef1_pol_bkg.setConstant(1);
  coef2_pol_bkg.setConstant(1);
  
  ratio_pad_bkg->cd();
  ratio_bkg->GetYaxis()->SetRangeUser(0,2);
  ratio_bkg->Draw("P");
  line->Draw("SAME");
  
  pad_fit_bkg->cd();
  pad_fit_bkg->SetLogy(1);
  frame_bkg->Draw();  
  

  sprintf(buffer,"bkg fit : #chi^{2}=%2.2f",frame_bkg->chiSquare());
  latex.DrawLatex(0.3,0.96,buffer);
  
  sprintf(buffer,"../plot/png/fit%s_%s.png",menu_cut,buffer_savebkg);
  canvas_bkg->SaveAs(buffer);
  sprintf(buffer,"../plot/pdf/fit%s_%s.pdf",menu_cut,buffer_savebkg);
  canvas_bkg->SaveAs(buffer);
  sprintf(buffer,"../plot/root/fit%s_%s.root",menu_cut,buffer_savebkg);
  canvas_bkg->SaveAs(buffer);
  


  //########SGN
  TCanvas *canvas_sgn=new TCanvas("canvas_sgn","canvas_sgn");
  TPad *pad_fit_sgn=new TPad("pad_fit_sgn","pad_fit_sgn",0,0.3,1,1);
  TPad *ratio_pad_sgn=new TPad("ratio_pad_sgn","ratio_pad_sgn",0,0,1,0.3);
  pad_fit_sgn->SetBottomMargin(0.05);
  pad_fit_sgn->Draw();
  canvas_sgn->cd();
  ratio_pad_sgn->SetTopMargin(0);
  ratio_pad_sgn->Draw();
  
  RooPlot* frame_sgn=pt.frame();

  RooRealVar mean_land_sgn("mean_land_sgn","mean_land_sgn",10,0,200);
  RooRealVar sigma_land_sgn("sigma_land_sgn","sigma_land_sgn",2,0,100);
  RooLandau *land_sgn=new RooLandau("land_sgn","land_sgn",pt,mean_land_sgn,sigma_land_sgn);

  RooRealVar coef0_logn_sgn("coef0_logn_sgn","coef0_logn_sgn",0,1,100);
  RooRealVar coef1_logn_sgn("coef1_logn_sgn","coef1_logn_sgn",1,1,100);
  RooRealVar coef2_logn_sgn("coef2_logn_sgn","coef2_logn_sgn",0,-30,30);
  RooRealVar coef3_logn_sgn("coef3_logn_sgn","coef3_logn_sgn",3,1,100);
  RooGenericPdf *logn_sgn=new RooGenericPdf("logn_sgn","TMath::LogNormal(pt,coef1_logn_sgn,coef2_logn_sgn,coef3_logn_sgn)", RooArgSet(pt,coef1_logn_sgn,coef2_logn_sgn,coef3_logn_sgn));


  RooRealVar coef0_pol_sgn("coef0_pol_sgn","coef0_pol_sgn",0,-100,100);
  RooRealVar coef1_pol_sgn("coef1_pol_sgn","coef1_pol_sgn",2,-100,100);
  RooRealVar coef2_pol_sgn("coef2_pol_sgn","coef2_pol_sgn",2,-100,100);
  RooRealVar coef3_pol_sgn("coef3_pol_sgn","coef3_pol_sgn",3,-100,100);
  RooPolynomial *pol2_sgn;

  sprintf(buffer,"hist_pt%s_ggh_gen",menu_cut);
  TH1F* hist_sgn=(TH1F*) file_result->Get(buffer);
  hist_sgn->Sumw2();
  RooDataHist *sgn=new RooDataHist("sgn","sgn",pt,hist_sgn);
  sgn->plotOn(frame_sgn);

  RooProdPdf *model_sgn;

  char buffer_savesgn[100];
  switch (2*menu_pol_sgn+menu_sgn) {
  case 0 : 
    pol2=new RooPolynomial("pol2_sgn","pol2_sgn",pt,RooArgList(coef0_pol_sgn));
    model_sgn =new RooProdPdf("model_sgn","model_sgn",RooArgList(*land_sgn,*pol2_sgn));
    sprintf(buffer_savesgn,"sgnlandpol0");
    break;  
    
  case 1 : 
    pol2=new RooPolynomial("pol2_sgn","pol2_sgn",pt,RooArgList(coef0_pol_sgn));
    model_sgn =new RooProdPdf("model_sgn","model_sgn",RooArgList(*logn_sgn,*pol2_sgn));
    sprintf(buffer_savesgn,"sgnlognpol0");
    break;  
    
  case 2 : 
    pol2=new RooPolynomial("pol2_sgn","pol2_sgn",pt,RooArgList(coef0_pol_sgn,coef1_pol_sgn));
    model_sgn =new RooProdPdf("model_sgn","model_sgn",RooArgList(*land_sgn,*pol2_sgn));
    sprintf(buffer_savesgn,"sgnlandpol1");
    break;  
  case 3 : 
    pol2=new RooPolynomial("pol2_sgn","pol2_sgn",pt,RooArgList(coef0_pol_sgn,coef1_pol_sgn));
    model_sgn =new RooProdPdf("model_sgn","model_sgn",RooArgList(*logn_sgn,*pol2_sgn));
    sprintf(buffer_savesgn,"sgnlognpol1");
    break;  
  case 4 : 
    pol2=new RooPolynomial("pol2_sgn","pol2_sgn",pt,RooArgList(coef0_pol_sgn,coef1_pol_sgn,coef2_pol_sgn));
    model_sgn =new RooProdPdf("model_sgn","model_sgn",RooArgList(*land_sgn,*pol2_sgn));
    sprintf(buffer_savesgn,"sgnlandpol2");
    break;  
  case 5 : 
    pol2=new RooPolynomial("pol2_sgn","pol2_sgn",pt,RooArgList(coef0_pol_sgn,coef1_pol_sgn,coef2_pol_sgn));
    model_sgn =new RooProdPdf("model_sgn","model_sgn",RooArgList(*logn_sgn,*pol2_sgn));
    sprintf(buffer_savesgn,"sgnlognpol2");
    break;  
  }


  model_sgn->fitTo(*sgn);
  model_sgn->plotOn(frame_sgn);


  TH1F *ratio_sgn=model_sgn->createHistogram("ratio_sgn", pt,RooFit::Binning((int)hist_sgn->GetNbinsX(),[0,200]));

  ratio_sgn->Scale(hist_sgn->Integral());
  for (int i=0;i<hist_sgn->GetNbinsX();i++) {
    if (ratio_sgn->GetBinContent(i)>0) {
      ratio_sgn->SetBinContent(i,hist_sgn->GetBinContent(i)/ratio_sgn->GetBinContent(i));
    }}
  
 
  coef0_logn_sgn.setConstant(1);
  coef1_logn_sgn.setConstant(1);
  coef2_logn_sgn.setConstant(1);
  coef3_logn_sgn.setConstant(1);
  mean_land_sgn.setConstant(1);
  sigma_land_sgn.setConstant(1);
  coef0_pol_sgn.setConstant(1);
  coef1_pol_sgn.setConstant(1);
  coef2_pol_sgn.setConstant(1);
  
  ratio_pad_sgn->cd();
  ratio_sgn->GetYaxis()->SetRangeUser(0,2);
  ratio_sgn->Draw("P");
  line->Draw("SAME");
  
  pad_fit_sgn->cd();
  pad_fit_sgn->SetLogy(1);
  frame_sgn->Draw();  
  

  sprintf(buffer,"sgn fit : #chi^{2}=%2.2f",frame_sgn->chiSquare());
  latex.DrawLatex(0.3,0.96,buffer);
  
  sprintf(buffer,"../plot/png/fit%s_%s.png",menu_cut,buffer_savesgn);
  canvas_sgn->SaveAs(buffer);
  sprintf(buffer,"../plot/pdf/fit%s_%s.pdf",menu_cut,buffer_savesgn);
  canvas_sgn->SaveAs(buffer);
  sprintf(buffer,"../plot/root/fit%s_%s.root",menu_cut,buffer_savesgn);
  canvas_sgn->SaveAs(buffer);

 //###########################DATA

  TCanvas *canvas_data=new TCanvas("canvas_data","canvas_data");
  TPad *pad_fit_data=new TPad("pad_fit_data","pad_fit_data",0,0.3,1,1);
  TPad *pad_ratio_data=new TPad("ratio_pad_data","ratio_pad_data",0,0,1,0.3);
  pad_fit_data->SetBottomMargin(0.05);
  pad_fit_data->Draw();
  canvas_data->cd();
  pad_ratio_data->SetTopMargin(0);
  pad_ratio_data->Draw();


  RooPlot *frame_data=pt.frame();
  frame_data->GetXaxis()->SetTitle("");

  RooRealVar compo_sgn("compo_sgn","compo_sgn",0,0,1);
  RooAddPdf *model_data=new RooAddPdf("model_data","model_data",RooArgList(*model_sgn,*model_bkg),compo_sgn);

  TH1F *hist_data=(TH1F*) hist_sgn->Clone();
  hist_data->Sumw2();
  hist_data->Add(hist_bkg);
  RooDataHist *data=new RooDataHist("data","data",pt,hist_data);


  TH1F *ratio_data=model_data->createHistogram("ratio_data",pt,RooFit::Binning(hist_data->GetNbinsX(),[0,200]));
  ratio_data->Scale(hist_data->Integral());
  for (int i=0;i<hist_data->GetNbinsX();i++) {
    if (ratio_data->GetBinContent(i)>0) ratio_data->SetBinContent(i,hist_data->GetBinContent(i)/ratio_data->GetBinContent(i));
  }

  model_data->fitTo(*data);
  data->plotOn(frame_data);
  model_data->plotOn(frame_data);
  model_data->plotOn(frame_data,RooFit::Components("model_sgn"),RooFit::LineStyle(kDashed),RooFit::LineColor(kRed));  
  model_data->plotOn(frame_data,RooFit::Components("model_bkg"),RooFit::LineStyle(kDashed),RooFit::LineColor(kGreen));  
  
  


  pad_ratio_data->cd();
  ratio_data->GetYaxis()->SetRangeUser(0,2);
  ratio_data->Draw("P");
  line->Draw();

  pad_fit_data->cd();
  pad_fit_data->SetLogy(1);
  frame_data->Draw();  

  sprintf(buffer,"chi2=%2.2f",frame_data->chiSquare());
  latex.DrawLatex(0.7,0.8,buffer);
  sprintf(buffer,"compo signal : %2.2e",compo_sgn.getVal());
  latex.DrawLatex(0.15,0.96,buffer);
  sprintf(buffer, "expected : %2.2e",hist_sgn->Integral()/hist_data->Integral());
  latex.DrawLatex(0.6,0.96,buffer);



  sprintf(buffer,"../plot/png/fit%s_data_%s_%s.png",menu_cut,buffer_savebkg,buffer_savesgn);    
  canvas_data->SaveAs(buffer);
  sprintf(buffer,"../plot/pdf/fit%s_data_%s_%s.pdf",menu_cut,buffer_savebkg,buffer_savesgn);
  canvas_data->SaveAs(buffer);
  sprintf(buffer,"../plot/root/fit%s_data_%s_%s.root",menu_cut,buffer_savebkg,buffer_savesgn);
  canvas_data->SaveAs(buffer);

  return 1;



}
