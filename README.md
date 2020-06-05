# MuonID
MVA-based muonID

## Training - samples:
* background = any **global** muon in events from BdToKK, BdToKPi, BdToPiPi, BsToKK, BsToPiPi MC samples having:
  * pt > 2 GeV
  * abs(eta) < 2.4
  * valid MC info attached to the reco muon, i.e. *mu_simType != 0*
  * matching at simulation level with pi or kaon reconstructed as muon or *true* muon from pi or K decay:

|                                                                   | abs(eta) < 1.2 | abs(eta) >= 1.2 |
|-------------------------------------------------------------------|----------------|-----------------|
| pion: abs(simPdgId)==211                                          | 5.1%           | 1.5%            |
| kaon: abs(simPdgId)==321                                          | 0.9%           | 0.3%            |
| mu from pion decay: abs(simPdgId)==13 && abs(simMotherPdgId)==211 | 26.7%          | 40.5%           |
| mu from K decay: abs(simPdgId)==13 && abs(simMotherPdgId)==321    | 67.2%          | 57.7%           |
| Total bkg size                                                    | 67453          | 115088          |

* signal = events from DsTau3Mu and B0Tau3Mu MC samples with:
  * at least one triplet candidate per event
  * event fires *HLT_DoubleMu3_TkMu_DsTau3Mu*
  * triplet matches with tau->3mu at generator level
  * if N.triplet>1, best SV chi2 is chosen
  * trailing muon (mu3) is used, with requirement:
    * **global** muon
    * pt > 2 GeV
    * abs(eta) < 2.4

|                                                                   | abs(eta) < 1.2 | abs(eta) >= 1.2 |
|-------------------------------------------------------------------|----------------|-----------------|
| Total signal size                                                 | 84652          | 172620          |

Code for muon selection is here: https://github.com/rvenditti/Tau3MuSearch/blob/master/MuonID_study/ntupleClass_muonid.C

## Training - phasespace reweighting:
https://github.com/t3mucommontools/MuonID/Phase_space_reweighing_tools/PT_reweighting.cpp computes the signal/background ratio between the muon PT-ETA 2D histograms to be used for phasespace reweighting

## Training - BDT input variables:
Defined in https://github.com/t3mucommontools/MuonID/MuonID_common.h
```C++
std::vector<TString> var_MuonID_train_def = {
                                      //Muon reconstruction
                                      "mu_combinedQuality_chi2LocalMomentum>250?250:mu_combinedQuality_chi2LocalMomentum",
                                      "mu_combinedQuality_chi2LocalPosition>50?50:mu_combinedQuality_chi2LocalPosition",
                                      "mu_combinedQuality_staRelChi2>50?50:mu_combinedQuality_staRelChi2",
                                      "mu_combinedQuality_trkRelChi2>50?50:mu_combinedQuality_trkRelChi2",
                                      "mu_combinedQuality_globalDeltaEtaPhi",
                                      "log(mu_combinedQuality_trkKink)",
                                      "log(mu_combinedQuality_glbKink)",
                                      "mu_combinedQuality_glbTrackProbability>150?150:mu_combinedQuality_glbTrackProbability",

                                      //collection of hits in the HitPattern
                                      "mu_Numberofvalidpixelhits",
                                      "mu_trackerLayersWithMeasurement",
                                      "mu_validMuonHitComb", //Hits in DT, CSC, RPC 

                                      //muon track reconstruction
                                      "mu_numberOfMatchedStations",
                                      "mu_segmentCompatibility",
                                      "mu_timeAtIpInOutErr", //to be studied

                                      //general track properties
                                      "mu_GLnormChi2>50?50:mu_GLnormChi2",
                                      "mu_innerTrack_normalizedChi2>50?50:mu_innerTrack_normalizedChi2",
                                      "mu_outerTrack_normalizedChi2>80?80:mu_outerTrack_normalizedChi2",
                                      "mu_innerTrack_validFraction", //Inner Valid Fraction
```
```C++
    std::vector<TString> var_MuonID_spec_def = {
                                      "mu_eta",
                                      "mu_pt",
                                      "mu_phi",
                                      "mu_SoftMVA"
                                      };
```
## Training - settings:
* BDT training is done separately for barrel and endcap regions
  * barrel = abs(eta) < 1.2
  * endcap = abs(eta) >= 1.2
* All settings are in https://github.com/t3mucommontools/MuonID/MVA_muonid.cpp

## Evaluation:
latest version of weights: 
* https://github.com/t3mucommontools/MuonID/MuonMVA_04june_TLWM_barrel/weights/
* https://github.com/t3mucommontools/MuonID/MuonMVA_04june_TLWM_endcap/weights/

Example of code for evaluation:
warning: 
* this assumes the variable names match with ones used in input tree
* complete path to weights defined globally
```C++
TString weight_path_barrel
TString weight_path_endcap
```
```C++
void filltree_MuonID(TTree *tin, TTree *tout, Double_t &value_MuonID){

    //Create input TTreeReader
    cout<<"Accessing to input tree"<<endl;
    TTreeReader treeReader(tin);

    //create the TMVA::Reader
    TMVA::Tools::Instance();
    TMVA::Reader *MVAreader_barrel = new TMVA::Reader("!Color:!Silent:!V");
    TMVA::Reader *MVAreader_endcap = new TMVA::Reader("!Color:!Silent:!V");

    //number of spectators
    size_t n_spec = var_MuonID_spec_def.size();
    std::vector<Float_t> var_spec;
    for(int j = 0; j<n_spec; j++) var_spec.push_back(0);
    // Spectators declaration
    for(int k = 0; k<n_spec; k++){
        MVAreader_barrel->TMVA::Reader::AddSpectator(var_MuonID_def_names.at(k), &var_spec.at(k));
        MVAreader_endcap->TMVA::Reader::AddSpectator(var_MuonID_def_names.at(k), &var_spec.at(k));
        cout<<var_MuonID_spec_def.at(k)<<endl;
    }
    //number of variables used for training
    size_t n_train = var_MuonID_train_def.size();
    std::vector<Float_t> var_train;
    for(int j = 0; j<n_train; j++) var_train.push_back(0);
    // Variables declaration
    for(int k = 0; k<n_train; k++){
        MVAreader_barrel->TMVA::Reader::AddVariable(var_MuonID_train_def.at(k), &var_train.at(k));
        MVAreader_endcap->TMVA::Reader::AddVariable(var_MuonID_train_def.at(k), &var_train.at(k));
        cout<<var_MuonID_train_def.at(k)<<endl;
    }
    TString category = "barrel";
    //Book the MVA method used for MuonID
    Bool_t weightfileExists = (gSystem->AccessPathName(weight_path_barrel) == kFALSE);
    if (weightfileExists) {
       MVAreader_barrel->TMVA::Reader::BookMVA(method, weight_path_barrel);
       cout<<"Using weights in "<<weight_path_barrel<<endl;
    } else {
       std::cout << "Weightfile " <<weight_path_barrel<<" for method " << method << " not found."
                    " Did you run TMVACrossValidation with a specified splitExpr?" << std::endl;
       exit(0);
    }
    category = "endcap";
    //Book the MVA method used for MuonID
    weightfileExists = (gSystem->AccessPathName(weight_path_endcap) == kFALSE);
    if (weightfileExists) {
       MVAreader_endcap->TMVA::Reader::BookMVA(method, weight_path_endcap);
       cout<<"Using weights in "<<weight_path_endcap<<endl;
    } else {
       std::cout << "Weightfile " <<weight_path_endcap<<" for method " << method << " not found."
                    " Did you run TMVACrossValidation with a specified splitExpr?" << std::endl;
       exit(0);
    }

    //Read branches
    std::vector<TTreeReaderValue<double>> reader_spec;
    for(int k = 0; k<n_spec; k++){
        reader_spec.emplace_back(treeReader, var_MuonID_spec_names.at(k));
    }
    std::vector<TTreeReaderValue<double>> reader_train;
    for(int k = 0; k<n_train; k++){
        reader_train.emplace_back(treeReader, var_MuonID_train_names.at(k));
    }
    TTreeReaderValue<double> reader_mueta(treeReader, "mu_eta");

    //prepare branch in output tree
    //Loop on input Tree
    Double_t score = 0;
    while (treeReader.Next()) {
        for(int k = 0; k<n_spec; k++){
            var_spec.at(k)= *reader_spec.at(k);
        }
        for(int k = 0; k<n_train; k++){
            var_train.at(k) = *reader_train.at(k);
        }
        
        //Evaluate method(s) and fill histogram or MiniTree
        if(abs(*reader_mueta) < 1.2) score = MVAreader_barrel->EvaluateMVA(method);
        else score  = MVAreader_endcap->EvaluateMVA(method);
        value_MuonID = score;
        tout->Fill();
    }
    delete MVAreader_barrel;
    delete MVAreader_endcap;
}
```
