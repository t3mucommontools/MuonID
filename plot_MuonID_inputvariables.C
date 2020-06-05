#include "TH1F.h"
#include <cmath>
#include <string> 
#include "../MuonID_common.h"

void plot_MuonID_inputvariables() 
{
    //number of signal samples
    size_t nbkg = sizeof(inputpath_MuonID_bkg)/sizeof(inputpath_MuonID_bkg[0]);
    //open input files
    //bkg
    TChain *tbkg = new TChain("FinalTree");
    for(auto i=0; i<nbkg; i++){
        tbkg->Add(inputpath_MuonID_bkg[i]);
        std::cout<<"Opened input file: "<<inputpath_MuonID_bkg[i]<<std::endl;
    }
    //signal sgn 
    TChain *tsgn = new TChain("FinalTree");
    tsgn->Add(inputpath_MuonID_Ds);
    tsgn->Add(inputpath_MuonID_B0);
    //tsgn->Add(inputpath_MuonID_Bp);
    std::cout<<"Opened input file: "<<inputpath_MuonID_Ds<<std::endl;
    std::cout<<"Opened input file: "<<inputpath_MuonID_B0<<std::endl;
    //std::cout<<"Opened input file: "<<inputpath_MuonID_Bp<<std::endl;

    //numbero of input variables
    //int n = var_MuonID_train_names.size();
    TString var[]  = {
                                      //Muon reconstruction
                                     "mu_combinedQuality_chi2LocalMomentum",
                                     "mu_combinedQuality_chi2LocalPosition",
                                     "mu_combinedQuality_staRelChi2",
                                     "mu_combinedQuality_trkRelChi2",
                                     "mu_combinedQuality_globalDeltaEtaPhi",
                                     "mu_combinedQuality_trkKink", //"log_mu_combinedQuality_trkKink",
                                     "mu_combinedQuality_glbKink", //"log_mu_combinedQuality_glbKink",
                                     "mu_combinedQuality_glbTrackProbability",
                                   
                                      //collection of hits in the HitPattern
                                     "mu_Numberofvalidtrackerhits", //Valid Tracker Hits
                                     "mu_Numberofvalidpixelhits",
                                     "mu_trackerLayersWithMeasurement",
                                     "mu_GLhitPattern_numberOfValidMuonHits",
                                     "mu_validMuonHitComb", //Hits in DT, CSC, RPC
                                   
                                      //muon track reconstruction
                                     "mu_numberOfMatchedStations",
                                     "mu_segmentCompatibility",
                                     "mu_timeAtIpInOut",
                                     "mu_timeAtIpInOutErr",
                                   
                                     //general track properties
                                      "mu_GLnormChi2",
                                      "mu_innerTrack_normalizedChi2",
                                      "mu_outerTrack_normalizedChi2",
                                      "mu_innerTrack_validFraction", //Inner Valid Fraction
                                      "mu_QInnerOuter",
                                      //"", //dxyRef
                                      //"", //dzRef
                                      
                                      //custom variables track multiplicity

                                      //isolation variables
                                      "mu_emEt03",
                                      "mu_hadEt03",
                                      "mu_nJets03",
                                      "mu_nTracks03",
                                      "mu_sumPt03",
                                      "mu_hadVetoEt03", 
                                      "mu_emVetoEt03",
                                      "mu_trackerVetoPt03" 
                                      };
    int n = sizeof(var)/sizeof(var[0]);
    cout<<n<<" input variables"<<endl;

    TH1F *hbkg[n];
    TH1F *hsgn[n];

    TString binning;

    //signal muons
    TCut cutS = "abs(mu_simPdgId)==13 && abs(mu_simMotherPdgId)==15"; 
    //bkg muons = pions, kaon, muons frmo decay in flight
    TCut cutB_pi = "( abs(mu_simPdgId) == 211 )"; //pion
    TCut cutB_k = "( abs(mu_simPdgId) == 321 )"; //kaon
    TCut cutB_mu_from_pi = "( abs(mu_simPdgId) == 13 && abs(mu_simMotherPdgId) == 211 )"; //true mu from pion decay
    TCut cutB_mu_from_k = "( abs(mu_simPdgId) == 13 && abs(mu_simMotherPdgId) == 321 )"; //true mu from kaon decay
    //running on global muons with associated simInfo
    TCut preselCutS = "mu_simType != 0 && mu_isGlobal == 1 && mu_pt > 2 && abs(mu_eta)<2.4";
    TCut preselCutB = "ptetaWeight*(mu_simType != 0 && mu_isGlobal == 1 && mu_pt > 2 && abs(mu_eta)<2.4)";
    //loose preselection cuts on input variables
    TCut cleanInputVar = "mu_combinedQuality_staRelChi2 > 0 &&"
                         "mu_combinedQuality_trkRelChi2 > 0 &&"
                         "mu_GLhitPattern_numberOfValidMuonHits > 0 &&"
                         "mu_validMuonHitComb > 0 &&"
                         "mu_segmentCompatibility > 0.05";

    //Loop on variables
    for(int i = 0; i<n; i++){
        TString varname = var[i];
        //TString varname = var_MuonID_train_names.at(i);
        cout<<"Input variable "<<varname<<endl;
        TString s = std::to_string(i);

        binning = "";
        if(varname=="mu_combinedQuality_chi2LocalMomentum") binning = "(400,0,8000)";
        if(varname=="mu_combinedQuality_glbKink") binning = "(500,0,10000)";
        if(varname=="mu_GLnormChi2") binning = "(200,0,6000)";
        if(varname=="mu_emEt03") binning = "(100,0,300)";
        if(varname=="mu_hadEt03") binning = "(110,0,350)";
        if(varname=="mu_nJets03") binning = "(5,0,5)";
        if(varname=="mu_nTracks03") binning = "(40,0,40)";
        if(varname=="mu_sumPt03") binning = "(150,0,1000)";
        if(varname=="mu_emVetoEt03") binning = "(50,0,200)";
        if(varname=="mu_hadVetoEt03") binning = "(50,0,200)";
        if(varname=="mu_trackerVetoPt03") binning = "(60,0,300)";
 
        tbkg->Draw(varname+">>hbkg"+s+binning, preselCutB&&(cutB_pi||cutB_k||cutB_mu_from_pi||cutB_mu_from_k) );
        hbkg[i] = (TH1F *)gDirectory->Get("hbkg"+s);

        tsgn->Draw(varname+">>hsgn"+s+binning, preselCutS&&cutS);
        hsgn[i] = (TH1F *)gDirectory->Get("hsgn"+s);
 
        //plot categories on same canvas - bkg
        TCanvas *c1 = new TCanvas("c1","c1",150,10,990,660);
        gStyle->SetOptTitle(0);
        gStyle->SetOptStat(0);
        hbkg[i]->SetLineColor(kGreen+3);
        hsgn[i]->SetLineColor(kBlack);
    
        Double_t min = std::min(hbkg[i]->GetXaxis()->GetXmin(),hsgn[i]->GetXaxis()->GetXmin() );
        min = min - min*0.1;
        Double_t max = std::max(hbkg[i]->GetXaxis()->GetXmax(),hsgn[i]->GetXaxis()->GetXmax() );
        max = max + max*0.1;
    
        hbkg[i]->GetXaxis()->SetRangeUser(min, max);
        hbkg[i]->GetXaxis()->SetTitle(varname);
    
        hbkg[i]->Draw("");
        hsgn[i]->Draw("same");
    
        TLegend*leg = new TLegend(0.4,0.7,0.7,0.9);
        leg->AddEntry(hbkg[i],"MuonID "+varname+" bkg","f");
        leg->AddEntry(hsgn[i],"MuonID "+varname+" sgn","f");
        leg->Draw();
    
        c1->SetLogy();
        c1->Update();
        c1->SaveAs("../plots/"+TMVA_MuonID_inputpath+varname+".png");
    }
    cout<<"Exiting ROOT"<<endl;
    gApplication->Terminate();
    return 0;
}

