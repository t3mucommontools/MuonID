#include <iostream>
#include <vector>

//TMVA Training options
    TString TMVA_MuonID_outputpath = "MuonMVA_04june_TLWM_"; //name to give to TMVA output files
    //change it to perform 5-fold Cross Validation
    bool doCV_MuonID = false;
    TString method_MuonID = "BDT";
    TString TMVA_MuonID_weightfilename = "/weights/TMVA_new_BDT.weights.xml"; //name given training BDT in "normal" way
    
   // if(doCV_MuonID)
   //TString method_MuonID = "BDTG";
   //TString TMVA_MuonID_weightfilename = "/weights/TMVACrossValidation_BDTG.weights.xml"; //name given training BDT with crossvalidation
    
//TMVA Evaluating options
    TString TMVA_MuonID_inputpath = "MuonMVA_04june_TLWM_";  //name to load TMVA results for evaluation

//data rootfiles
    TString inputpath_MuonID_bkg[] = {
    "/lustre/cms/store/user/fsimone/MuonID/Analysis/20200427_1244/AnalysedTree_MC_2018BdToKK_muonid0.root",           //Bd KK
    "/lustre/cms/store/user/fsimone/MuonID/Analysis/20200427_1246/AnalysedTree_MC_2018BdToPiPi_muonid0.root",         //Bd PiPi
    "/lustre/cms/store/user/fsimone/MuonID/Analysis/20200427_1351/AnalysedTree_MC_2018BdToKPi_muonid0.root",          //Bd KPi
    "/lustre/cms/store/user/fsimone/MuonID/Analysis/20200427_1246/AnalysedTree_MC_2018BdToKPi_2_muonid0.root",        //Bd KPi_2
    "/lustre/cms/store/user/fsimone/MuonID/Analysis/20200427_1348/AnalysedTree_MC_2018BsToKK_muonid0.root",           //Bs KK
    "/lustre/cms/store/user/fsimone/MuonID/Analysis/20200427_1349/AnalysedTree_MC_2018BsToKK_2_muonid_27april.root",  //Bs KK_2
    "/lustre/cms/store/user/fsimone/MuonID/Analysis/20200427_1350/AnalysedTree_MC_2018BsToPiPi_muonid0.root"          //Bs PiPi
    };

    TString inputpath_MuonID_Ds = 
    "/lustre/cms/store/user/fsimone/MuonID/Analysis/20200422_2320/AnalysedTree_MC_2018Ds_muonid_22april.root";
    TString inputpath_MuonID_B0 = 
    "/lustre/cms/store/user/fsimone/MuonID/Analysis/20200422_2320/AnalysedTree_MC_2018B0_muonid_22april.root";
    TString inputpath_MuonID_Bp = "";


//Coefficients for signal normalisation


//TMVA settings
// Variables
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
                                      //"mu_Numberofvalidtrackerhits", //Valid Tracker Hits
                                      "mu_Numberofvalidpixelhits",
                                      "mu_trackerLayersWithMeasurement",
                                      //"mu_GLhitPattern_numberOfValidMuonHits",
                                      "mu_validMuonHitComb", //Hits in DT, CSC, RPC 

                                      //muon track reconstruction
                                      "mu_numberOfMatchedStations",
                                      "mu_segmentCompatibility",
                                      //"mu_timeAtIpInOut", //to be studied
                                      "mu_timeAtIpInOutErr", //to be studied

                                      //general track properties
                                      "mu_GLnormChi2>50?50:mu_GLnormChi2",
                                      "mu_innerTrack_normalizedChi2>50?50:mu_innerTrack_normalizedChi2",
                                      "mu_outerTrack_normalizedChi2>80?80:mu_outerTrack_normalizedChi2",
                                      "mu_innerTrack_validFraction", //Inner Valid Fraction
                                      //"mu_QInnerOuter"
                                      //"", //dxyRef
                                      //"", //dzRef

                                      //custom variables track multiplicity
                                      };


    std::vector<TString> var_MuonID_train_names = {
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
                                     // "mu_Numberofvalidtrackerhits", //Valid Tracker Hits
                                      "mu_Numberofvalidpixelhits",
                                      "mu_trackerLayersWithMeasurement",
                                     //"mu_GLhitPattern_numberOfValidMuonHits",
                                      "mu_validMuonHitComb", //Hits in DT, CSC, RPC
                                   
                                     //muon track reconstruction
                                      "mu_numberOfMatchedStations",
                                      "mu_segmentCompatibility",
                                     //"mu_timeAtIpInOut",
                                      "mu_timeAtIpInOutErr",
                                   
                                     //general track properties
                                     "mu_GLnormChi2",
                                     "mu_innerTrack_normalizedChi2",
                                     "mu_outerTrack_normalizedChi2",
                                     "mu_innerTrack_validFraction", //Inner Valid Fraction
                                     //"mu_QInnerOuter"
                                     //"", //dxyRef
                                     //"", //dzRef
                                      
                                     //custom variables track multiplicity
                                     };

    std::vector<TString> var_MuonID_spec_def = {
                                      "mu_eta",
                                      "mu_pt",
                                      "mu_phi",
                                    //  "mu_simPdgId",
                                    //  "mu_simMotherPdgId",
                                    //  "mu_simType",
                                    //  "mu_isGlobal",
                                      "mu_SoftMVA"
                                      //"ptetaWeight"
                                      };

    std::vector<TString> var_MuonID_spec_names = {
                                      "mu_eta",
                                      "mu_pt",
                                      "mu_phi",
                                    //  "mu_simPdgId",
                                    //  "mu_simMotherPdgId",
                                    //  "mu_simType",
                                    //  "mu_isGlobal",
                                      "mu_SoftMVA"
                                      //"ptetaWeight"
                                      };
