#ifndef __L1Analysis_L1AnalysisDataFormat_H__
#define __L1Analysis_L1AnalysisDataFormat_H__

#include <vector>
// #include <inttypes.h>
#include <TROOT.h>

namespace L1Analysis
{
  struct L1AnalysisDataFormat
  {
    L1AnalysisDataFormat(){Reset();};
    ~L1AnalysisDataFormat(){};
    
    void Reset()
    { 

      //Event Info
      Run = -1;
      Event = -1;
      LS = -1;


      //EG info
      Nele = 0;
      Bxel.clear();
      Etel.clear();
      Phiel.clear();
      Etael.clear();
      Isoel.clear();

      NTkEM = 0;
      BxTkEM.clear();
      EtTkEM.clear();
      PhiTkEM.clear();
      EtaTkEM.clear();

      NTkEle = 0;
      BxTkEle.clear();
      EtTkEle.clear();
      PhiTkEle.clear();
      EtaTkEle.clear();
      IsoTkEle.clear();

      Njet = 0;
      Bxjet.clear();
      Etjet.clear();
      Phijet.clear();
      Etajet.clear();
      Taujet.clear();
      isoTaujet.clear();
      Fwdjet.clear();  

      NTkJet = 0;
      BxTkJet.clear();
      EtTkJet.clear();
      PhiTkJet.clear();
      EtaTkJet.clear();
      TauTkJet.clear();
      isoTauTkJet.clear();
      FwdTkJet.clear();

      Nmu = 0;
      Bxmu.clear();
      Ptmu.clear();
      Phimu.clear();
      Etamu.clear();
      Qualmu.clear();
      Isomu.clear();

      NTkMu = 0;
      BxTkMu.clear();
      PtTkMu.clear();
      PhiTkMu.clear();
      EtaTkMu.clear();
      QualTkMu.clear();
      IsoTkMu.clear();

      // ------ ETT, ETM, HTT and HTM from PSB14:
      ETT = -1.;
      OvETT = false;
      HTT = -1.;
      OvHTT = false;
      ETM = -1.;
      PhiETM = -1;
      OvETM = false;
      HTM = -1.;
      PhiHTM = -1;
      OvHTM = false;

      TkETM = -1;
      PhiTkETM = -1;
    }
   
    // ---- L1AnalysisDataFormat information.

    //
    int Run;
    int Event;
    int LS;

    // EG info
    int Nele;
    std::vector<int>    Bxel;
    std::vector<float>  Etel;
    std::vector<float>  Phiel;
    std::vector<float>  Etael;
    std::vector<bool>   Isoel;

    // L1TkEmParticles - L1EG obj, isolated wrt L1Tracks, ie isolated photons
    int NTkEM;
    std::vector<int>    BxTkEM;
    std::vector<float>  EtTkEM;
    std::vector<float>  PhiTkEM;
    std::vector<float>  EtaTkEM;

    // L1TkElectrons - L1EG matched to L1Track
    int NTkEle;
    std::vector<int>    BxTkEle;
    std::vector<float>  EtTkEle;
    std::vector<float>  PhiTkEle;
    std::vector<float>  EtaTkEle;
    std::vector<bool>   IsoTkEle;

    // Jet info
    int Njet;
    std::vector<int>    Bxjet;
    std::vector<float>  Etjet;
    std::vector<float>  Phijet;
    std::vector<float>  Etajet;
    std::vector<bool>   Taujet;
    std::vector<bool>   isoTaujet;
    std::vector<bool>   Fwdjet;

    // L1TkJets - not finished yet
    int NTkJet;
    std::vector<int>    BxTkJet;
    std::vector<float>  EtTkJet;
    std::vector<float>  PhiTkJet;
    std::vector<float>  EtaTkJet;
    std::vector<bool>   TauTkJet;
    std::vector<bool>   isoTauTkJet;
    std::vector<bool>   FwdTkJet;

    // Mu info
    int Nmu;
    std::vector<int>    Bxmu;
    std::vector<float>  Ptmu;
    std::vector<float>  Phimu;
    std::vector<float>  Etamu;
    std::vector<int>    Qualmu;
    std::vector<float>  Isomu;

    // L1TkMuons - naive deltaR matchign between GMT L1Muons and L1Tracks
    int NTkMu;
    std::vector<int>    BxTkMu;
    std::vector<float>  PtTkMu;
    std::vector<float>  PhiTkMu;
    std::vector<float>  EtaTkMu;
    std::vector<int>    QualTkMu;
    std::vector<float>  IsoTkMu;

    // ------ ETT, ETM, HTT and HTM :
    float ETT;
    bool OvETT;

    float HTT;
    bool OvHTT;

    float ETM;
    int PhiETM;
    bool OvETM;

    float HTM;
    int PhiHTM;
    bool OvHTM;

    // L1TkEtMissParticle - MET as obtained from L1Tracks from L1TrackPrimaryVertex
    float TkETM;
    int PhiTkETM;

  }; 
} 
#endif


