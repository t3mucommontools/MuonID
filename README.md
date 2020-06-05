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
MuonID/Phase_space_reweighing_tools/PT_reweighting.cpp computes the signal/background ratio between the muon PT-ETA 2D histograms to be used for phasespace reweighting

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
* All settings are in https://github.com/t3mucommontools/MuonID/MVA_muonid.cpp

## Evaluation:
latest version of weights: 
* https://github.com/t3mucommontools/MuonID/MuonMVA_04june_TLWM_barrel/weights/
* https://github.com/t3mucommontools/MuonID/MuonMVA_04june_TLWM_endcap/weights/
