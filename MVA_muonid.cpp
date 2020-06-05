//root -l MVA_muonid.cpp\(\"category\"\)

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

//#include "TMVA/CrossValidation.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"

#include "MuonID_common.h"

using namespace TMVA;


void MVA_muonid(TString categ){
    //Check on input argument
    if(!categ.Contains("barrel") && !categ.Contains("endcap")){
        cout << "Please choose between any combination of 'barrel' and 'endcap'" << endl;
        return;
    }

    // Output file
    TFile *fout = new TFile("TMVA_"+TMVA_MuonID_outputpath+categ+".root", "RECREATE");

    std::vector<TTree*> sigTree, bkgTree;

    // Get the signal and background trees from TFile source(s);
    //signal
    TString treeName = "FinalTree";
    //Ds
    TFile *f_sig_ds = (TFile*)gROOT->GetListOfFiles()->FindObject(inputpath_MuonID_Ds);
    if (!f_sig_ds || !f_sig_ds->IsOpen()) {
         f_sig_ds = new TFile(inputpath_MuonID_Ds);
    }
    sigTree.push_back( (TTree*)f_sig_ds->Get(treeName));
    //B0
    TFile *f_sig_b0 = (TFile*)gROOT->GetListOfFiles()->FindObject(inputpath_MuonID_B0);
    if (!f_sig_b0 || !f_sig_b0->IsOpen()) {
        f_sig_b0 = new TFile(inputpath_MuonID_B0);
    }
    sigTree.push_back( (TTree*)f_sig_b0->Get(treeName));
    ////Bp
    //TFile *f_sig_bp = (TFile*)gROOT->GetListOfFiles()->FindObject(inputpath_MuonID_Bp);
    //if (!f_sig_bp || !f_sig_bp->IsOpen()) {
    //   f_sig_bp = new TFile(inputpath_MuonID_Bp);
    //}
    //sigTree.push_back( (TTree*)f_sig_bp->Get(treeName_sig));

    //background
    size_t n_bkg = sizeof(inputpath_MuonID_bkg)/sizeof(inputpath_MuonID_bkg[0]);
    //Loop on samples
    for(int j = 0; j<n_bkg; j++){
        TFile *f_bkg = (TFile*)gROOT->GetListOfFiles()->FindObject(inputpath_MuonID_bkg[j]);
        if (!f_bkg || !f_bkg->IsOpen()) {
            f_bkg = new TFile(inputpath_MuonID_bkg[j]);
        }
        bkgTree.push_back((TTree*)f_bkg->Get(treeName));
    }

    // Set the event weights per tree
    Double_t sigWeight  = 1.0;
    Double_t bkgWeight = 1.0;
    
    Factory *factory = new Factory("TMVA_new", fout, "");
    DataLoader *dataloader = new DataLoader(TMVA_MuonID_outputpath+categ);
   
    for(int i = 0; i<sigTree.size(); i++) {
        dataloader->AddSignalTree(sigTree.at(i), sigWeight);
    }
    for(int j = 0; j<bkgTree.size(); j++) {
        dataloader->AddBackgroundTree(bkgTree.at(j), bkgWeight);
    }

    // Spectators declaration
    for(int k = 0; k<var_MuonID_spec_names.size(); k++){
        dataloader->AddSpectator(var_MuonID_spec_def.at(k), var_MuonID_spec_names.at(k), "", 'F');
    }

    // Variables declaration
    for(int k = 0; k<var_MuonID_train_names.size(); k++){
        dataloader->AddVariable(var_MuonID_train_def.at(k), var_MuonID_train_names.at(k), "", 'F');
    }


    //signal muons
    TCut cutS = "abs(mu_simPdgId)==13 && abs(mu_simMotherPdgId)==15"; 
    //bkg muons = pions, kaon, muons frmo decay in flight
    TCut cutB_pi = "( abs(mu_simPdgId) == 211 )"; //pion
    TCut cutB_k = "( abs(mu_simPdgId) == 321 )"; //kaon
    TCut cutB_mu_from_pi = "( abs(mu_simPdgId) == 13 && abs(mu_simMotherPdgId) == 211 )"; //true mu from pion decay
    TCut cutB_mu_from_k = "( abs(mu_simPdgId) == 13 && abs(mu_simMotherPdgId) == 321 )"; //true mu from kaon decay
    //running on global muons with associated simInfo
    TCut preselCut = "mu_simType != 0 && mu_isGlobal == 1 && mu_pt > 2 && abs(mu_eta)<2.4";
    //loose preselection cuts on input variables
    TCut cleanInputVar = "mu_combinedQuality_chi2LocalMomentum < 5000 &&"
                         "mu_combinedQuality_chi2LocalPosition < 1000 &&"
                         "mu_combinedQuality_trkRelChi2 < 20 &&"
                         "mu_combinedQuality_trkKink < 900 &&"
                         "mu_Numberofvalidtrackerhits > 0 &&"
                         "mu_Numberofvalidpixelhits > 0 &&"
                         "mu_timeAtIpInOutErr < 10 &&"
                         "mu_GLnormChi2 < 6000 &&"
                         "mu_innerTrack_normalizedChi2 < 40 &&"
                         "mu_innerTrack_validFraction > 0.5";

    //training separately on endcap and barrel
    TCut etarange = "abs(mu_eta)<0"; //always false
    TCut barrel = "abs(mu_eta)<1.2";
    TCut endcap = "abs(mu_eta)>=1.2";
    if(categ.Contains("endcap")) etarange = etarange || endcap;
    if(categ.Contains("barrel")) etarange = etarange || barrel;

    dataloader->SetBackgroundWeightExpression("ptetaWeight");

    TString prepareTrainTestOptions;
    if(categ=="barrel") { 
             prepareTrainTestOptions = ":SplitMode=Random"
                                       ":NormMode=NumEvents"
                                       ":nTrain_Signal=0"
                                       ":nTest_Signal=25000" //barrel
                                       ":nTrain_Background=0"
                                       ":nTest_Background=20000" //barrel
                                       ":!V";
                        }
    else if(categ=="endcap"){
             prepareTrainTestOptions = ":SplitMode=Random"
                                       ":NormMode=NumEvents"
                                       ":nTrain_Signal=0"
                                       ":nTest_Signal=52000" //endcap
                                       ":nTrain_Background=0"
                                       ":nTest_Background=34500" //endcap
                                       ":!V";
                        }
/* //barrel
                         : Signal     -- training and testing events: 84652 --> only test 25000
                         : Background -- training and testing events: 67453 --> only test 20000
   //endcap
                         : Signal     -- training and testing events: 172620 --> only test 52000
                         : Background -- training and testing events: 115088 --> only test 34500
*/

    dataloader->PrepareTrainingAndTestTree(preselCut&&cleanInputVar&&etarange&&cutS, preselCut&&cleanInputVar&&etarange&&(cutB_pi||cutB_k||cutB_mu_from_pi||cutB_mu_from_k), prepareTrainTestOptions);
    
    // Booking of MVA methods : BDT
    factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT", 
         "!H:!V:NTrees=1000:MinNodeSize=1.5%:MaxDepth=8:BoostType=RealAdaBoost:AdaBoostBeta=0.3:UseBaggedBoost:BaggedSampleFraction=0.05:SeparationType=GiniIndex:nCuts=-1" );
         //"!H:!V:NTrees=1000:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=50" );
         //"!H:!V:NTrees=1000:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=50" );

    // Booking of MVA methods : MLP
    //factory->BookMethod( dataloader, TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator" );
 
    // Training the MVA methods
    factory->TrainAllMethods();
    
    // Testing the MVA methods
    factory->TestAllMethods();
    
    // Evaluating the MVA methods
    factory->EvaluateAllMethods();

    // Save the output
    fout->Close();
    
    delete factory;
    delete dataloader;
    // Launch the GUI for the root macros
    if (!gROOT->IsBatch()){
        TMVAGui("TMVA_"+TMVA_MuonID_outputpath+categ+".root");
    }
    cout<<"Exiting ROOT"<<endl;
    gApplication->Terminate();
}
