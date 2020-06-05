#include "TH1F.h"
#include "TGraph2D.h"
#include "TLine.h"
#include "TCanvas.h"
#include <cmath>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <algorithm>
#include "../MuonID_common.h"

using namespace std;

double TH1_integral(TH1F *h, float xmin, float xmax){
    TAxis *axis = h->GetXaxis();
    int bmin = axis->FindBin(xmin);
    int bmax = axis->FindBin(xmax);
    double integral = h->Integral(bmin,bmax);
    integral -= h->GetBinContent(bmin)*(xmin-axis->GetBinLowEdge(bmin))/axis->GetBinWidth(bmin);
    integral -= h->GetBinContent(bmax)*(axis->GetBinUpEdge(bmax)-xmax)/ axis->GetBinWidth(bmax);

    return integral;
}

double log_significance(double S, double B){
    double significance = 0;
    significance = sqrt(2*( (S+B) * log( 1+S/B ) - S));
    //cout<<"log sign is "<<significance<<" while S/sqrt(S + B) gives "<< S/sqrt(S + B) <<endl;
    return significance;
}

TGraph* plot_ROC(TString categ, TH1F *h_signal, TH1F *h_bkg){

    float bdt_min = std::min(h_signal->GetXaxis()->GetXmin(), h_bkg->GetXaxis()->GetXmin());
    float bdt_max = std::max(h_signal->GetXaxis()->GetXmax(), h_bkg->GetXaxis()->GetXmax());
    int n = 200; //number of points
    float step = abs(bdt_max-bdt_min)/(n*1.0);

    Double_t x[n], y[n];
    double n_true_positive = 0;
    double n_false_positive = 0;
    double n_true_negative_rej = 0;
    double n_false_negative_rej = 0;
    float bdt_wp = 0;

    for(int i = 0; i<n; i++){
        bdt_wp = bdt_min + step*i;
        
        n_true_positive  = TH1_integral(h_signal, bdt_wp, bdt_max);
        n_false_positive = TH1_integral(h_bkg, bdt_wp, bdt_max);
        n_true_negative_rej  = TH1_integral(h_bkg,    bdt_min, bdt_wp);
        n_false_negative_rej = TH1_integral(h_signal, bdt_min, bdt_wp);
    
        x[i] = n_true_positive/(n_true_positive+n_false_negative_rej);
        y[i] = n_true_negative_rej/(n_false_positive+n_true_negative_rej);
    }

    TGraph* gr = new TGraph(n,x,y);
    gr->SetTitle("ROC;Signal Efficiency;Background rejection");
    return gr;
}

void ROC_plot_compare(TString categ){

    bool apply_weights = true;
    TString TMVA_filename = TMVA_MuonID_inputpath+categ;
    TFile *f_tmva = new TFile("/lustrehome/fsimone/MuonID_study/TMVA_"+TMVA_MuonID_inputpath+categ+".root","READ");
    cout<<"Opened TMVA file"<<endl;
    TTree *tTrain = (TTree*)f_tmva->Get(TMVA_filename+"/TrainTree");
    //TTree *tTest = (TTree*)f_tmva->Get(TMVA_filename+"/TestTree");

    TH1F *hMuonID_signal;
    TH1F *hMuonID_bkg;
    TH1F *hSoftMVA_signal;
    TH1F *hSoftMVA_bkg;

    TString binning = "(200, -1, 1)";
    TString w = "";
    if(apply_weights) w = "weight*";
    TString phasespace = "(mu_pt>4)";
    //TString phasespace = "(mu_pt<4)";
    TString softMVAvalid = "(mu_SoftMVA>-1)";
    //TString softMVAvalid = "1";
 
    tTrain->Draw("BDT>>hMuonID_signal"+binning, phasespace+"*(classID==0)*"+softMVAvalid);
    tTrain->Draw("BDT>>hMuonID_bkg"+binning,    phasespace+"*(classID==1)*"+softMVAvalid);
    tTrain->Draw("mu_SoftMVA>>hSoftMVA_signal"+binning, w+phasespace+"*(classID==0)*"+softMVAvalid);
    tTrain->Draw("mu_SoftMVA>>hSoftMVA_bkg"+binning,    w+phasespace+"*(classID==1)*"+softMVAvalid);

    hMuonID_signal  = (TH1F *)gDirectory->Get("hMuonID_signal"); 
    hMuonID_bkg     = (TH1F *)gDirectory->Get("hMuonID_bkg"); 
    hSoftMVA_signal = (TH1F *)gDirectory->Get("hSoftMVA_signal"); 
    hSoftMVA_bkg    = (TH1F *)gDirectory->Get("hSoftMVA_bkg"); 

    TGraph* ROC_MuonID =  plot_ROC(categ, hMuonID_signal, hMuonID_bkg);   
    TGraph* ROC_SoftMVA = plot_ROC(categ, hSoftMVA_signal, hSoftMVA_bkg);   

    TString out_filename = categ+phasespace+softMVAvalid;
    out_filename=out_filename.ReplaceAll("(","_");
    out_filename=out_filename.ReplaceAll(")","_");
    out_filename=out_filename.ReplaceAll(">","_above_");
    out_filename=out_filename.ReplaceAll("<","_below_");

    TCanvas *c1 = new TCanvas("c1","c1",150,10,990,660);
    hMuonID_signal->Draw();
    hMuonID_bkg->Draw("same");
    c1->SaveAs("../plots/MuonIDscore_"+out_filename+".png");

    TCanvas *c2 = new TCanvas("c2","c2",150,10,990,660);
    hSoftMVA_signal->Draw();
    hSoftMVA_bkg->Draw("same");
    c2->SaveAs("../plots/SoftMVAscore_"+out_filename+".png");

    TCanvas *c3 = new TCanvas("c3","c3",150,10,990,660);
    ROC_MuonID->SetLineColor(kRed);
    ROC_SoftMVA->SetLineColor(kBlue);
    ROC_MuonID->Draw();
    ROC_SoftMVA->Draw("same");
    //position of legend = bot left
    Double_t x1 = 0.1;
    Double_t x2 = 0.5;
    Double_t y1 = 0.1;
    Double_t y2 = 0.3;
    TLegend*leg = new TLegend(x1, y1, x2, y2);
    leg->AddEntry(ROC_MuonID,"ROC_MuonID_"+categ+phasespace);
    leg->AddEntry(ROC_SoftMVA,"ROC_SoftMVA_"+categ+phasespace);
    leg->Draw();
    c3->SaveAs("../plots/ROC_comparison_"+out_filename+".png");
}
