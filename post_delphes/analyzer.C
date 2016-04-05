

/*********************************************************/
/*** TODO :
            --- Removed deltaR cut : safe ?
            --- Add variables : EtaProd_jj, jet_isPU
            --- Check variables : Zeppetaj3, ZeppetaZZ ***/
/*********************************************************/

#include "TH1.h"
#include "TSystem.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TString.h"
#include "TClonesArray.h"

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

//#ifdef __CLING__
//R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include "external/ExRootAnalysis/ExRootResult.h"
//#endif

using namespace std;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



#define DEBUG false
#define MZ_PDG 91.1876
#define MEL_PDG 5.10998928e-04
#define MMU_PDG 0.1134289267




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



class ExRootResult;
class ExRootTreeReader;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------




struct cut_on_overlap
{
    double min_DR_jetlep;
    double min_DR_lepSF;
    double min_DR_lepNotSF;
};





//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------




struct cut_on_jets
{
    double max_eta_forwardjet;
    double max_eta_centraljet;
    double min_pt_centraljet;
    double min_pt_forwardjet;
};





//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------




struct cut_on_leptons
{
    double max_eta;
    double min_pt;
    double max_iso;
};



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



struct cut_on_Zbosons
{
    double min_mass;
    double max_mass;
    double minPTlep1;
    double minPTlep2;
};



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



struct Cuts
{
    cut_on_leptons electron;
    cut_on_leptons muon;
    cut_on_jets jet;
    cut_on_Zbosons Z1;
    cut_on_Zbosons Z2;
    cut_on_overlap OR;
};



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



void set_cuts(Cuts& current_cuts)
{
    if(DEBUG) cout << "Begin set_cuts function" << endl;
    current_cuts.electron.min_pt = 7;
    current_cuts.electron.max_eta = 2.47;
    current_cuts.electron.max_iso = 0.1;
    if(DEBUG) cout << "Cuts on electrons are defined" << endl;
    
    current_cuts.muon.min_pt = 6;
    current_cuts.muon.max_eta = 2.7;
    current_cuts.muon.max_iso = 0.1;
    if(DEBUG) cout << "Cuts on muons are defined" << endl;
    
    current_cuts.jet.min_pt_centraljet = 25;
    current_cuts.jet.min_pt_forwardjet = 30;
    current_cuts.jet.max_eta_centraljet = 2.4;
    current_cuts.jet.max_eta_forwardjet = 4.5;
    if(DEBUG) cout << "Cuts on jets are defined" << endl;
    
    current_cuts.Z1.min_mass = 50;
    current_cuts.Z1.max_mass = 106;
    current_cuts.Z1.minPTlep1 = 20;
    current_cuts.Z1.minPTlep2 = 15;
    if(DEBUG) cout << "Cuts on Z1 are defined" << endl;
    current_cuts.Z2.min_mass = 12;
    current_cuts.Z2.max_mass = 115;
    current_cuts.Z2.minPTlep1 = 10;
    current_cuts.Z2.minPTlep2 = -1; //Will accept anything for the last lepton !
    if(DEBUG) cout << "Cuts on Z2 are defined" << endl;
    
    current_cuts.OR.min_DR_jetlep = 0.2;
    current_cuts.OR.min_DR_lepSF = -1; //0.1;
    current_cuts.OR.min_DR_lepNotSF = -1; //0.2;
    if(DEBUG) cout << "Cuts on the overlap removal are defined" << endl;
    
    if(DEBUG) cout << "End set_cuts function" << endl;
}




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



struct myOutputs
{
    map<TString, TObject*> map_objs_toBeSaved;
    
    TH1 *fCutFlow;
    TH1 *fNbLeptonBeforeKinSel;
    TH1 *fNbLeptonAfterKinSel;
    int cutflowVar;
    bool cutFlowTitleReady;
    
    TTree *fOutTree;
    vector<TLorentzVector> lepton4v; //!
    vector<unsigned int> leptonOrigin; //! //TODO
    vector<unsigned int> leptonType; //! //TODO
    vector<TLorentzVector> jet4v; //!
    vector<TLorentzVector> z4v; //!
    TLorentzVector h4v; //!
    TLorentzVector dijet4v; //!
    vector<int> lepton_flavour; //! //TODO
    vector<int> lepton_mother; //! //TODO
    int Njets; //!
    double pT_Hjj; //!
    double minDelR_jZ; //!
    double DelPhi_Hjj; //!
    double DelEta_Hjj; //!
    double DelPhi_jj; //!
    double DelR_jj; //!
    double DelY_jj; //!
    double Mjj; //!
    double DelPt_jj; //!
    double DelPt_ZZ; //!
    double Mjets; //!
    double Zeppetaj3; //!
    double ZeppetaZZ; //1
    double weight; //!
    
    int nevents;
};




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



void clear_previous_event(myOutputs *plots)
{
    if(DEBUG) cout << "Begin clear_previous_event function" << endl;
    
    /*** Things needed for all events ***/
    plots->Njets = 0;
    plots->cutflowVar = 0;
    plots->weight = 0;
    plots->dijet4v.SetPxPyPzE(-999, -999, -999, -999);
    plots->minDelR_jZ = -999;
    plots->DelPhi_Hjj = -999;
    plots->DelEta_Hjj = -999;
    plots->DelPhi_jj = -999;
    plots->DelR_jj = -999;
    plots->DelY_jj = -999;
    plots->Mjj = -999;
    plots->DelPt_jj = -999;
    plots->DelPt_ZZ = -999;
    plots->pT_Hjj = -999;
    plots->Mjets = -999;
    plots->Zeppetaj3 = -999;
    plots->ZeppetaZZ = -999;
    
    /*** Things not needed for the first event (vectors not yet created) ***/
    if(plots->nevents != 0)
    {
        plots->jet4v.clear();
        
        plots->lepton4v.clear();
        plots->lepton_flavour.clear();
        plots->lepton_mother.clear();
        
        plots->z4v.clear();
    }
    
    plots->nevents++;
    
    if(DEBUG) cout << "End clear_previous_event function" << endl;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



void do_cutflow(myOutputs *plots, TString binTitle)
{
    if(DEBUG) cout << "Begin do_cutflow function, step "+binTitle << endl;
    plots->fCutFlow->Fill(plots->cutflowVar);
    if(!plots->cutFlowTitleReady) plots->fCutFlow->GetXaxis()->SetBinLabel(plots->cutflowVar+1, binTitle);
    plots->cutflowVar++;
    if(DEBUG) cout << "End do_cutflow function, step "+binTitle << endl;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



void BookHistograms(myOutputs *plots)
{
    if(DEBUG) cout << "Begin BookHistograms function" << endl;
    
    plots->fCutFlow = new TH1F("cutflow", "Cutflow", 30, -0.5, 29.5);
    plots->map_objs_toBeSaved.insert( make_pair<TString, TObject*> ("cutflow",plots->fCutFlow) );
    
    plots->fNbLeptonBeforeKinSel = new TH1F("NbLeptonBeforeKinSel","NbLeptonBeforeKinSel",10,-0.5,9.5);
    plots->map_objs_toBeSaved.insert( make_pair<TString, TObject*> ("NbLeptonBeforeKinSel",plots->fNbLeptonBeforeKinSel) );
    
    plots->fNbLeptonAfterKinSel = new TH1F("NbLeptonAfterKinSel","NbLeptonAfterKinSel",10,-0.5,9.5);
    plots->map_objs_toBeSaved.insert( make_pair<TString, TObject*> ("NbLeptonAfterKinSel",plots->fNbLeptonAfterKinSel) );
    
    
    plots->fOutTree = new TTree("HiggsTree","HiggsTree");
    plots->map_objs_toBeSaved.insert( make_pair<TString, TObject*> ("HiggsTree",plots->fOutTree) );
    plots->fOutTree->Branch("lepton_4v",&plots->lepton4v);
    plots->fOutTree->Branch("lepton_flavour",&plots->lepton_flavour); //TODO
    plots->fOutTree->Branch("jet_4v",&plots->jet4v);
    plots->fOutTree->Branch("jet_n",&plots->Njets);
    plots->fOutTree->Branch("weight",&plots->weight);
    plots->fOutTree->Branch("Zbosons_4v",&plots->z4v);
    plots->fOutTree->Branch("Hbosons_4v",&plots->h4v);
    plots->fOutTree->Branch("dijet_4v",&plots->dijet4v);
    plots->fOutTree->Branch("minDelR_jZ",&plots->minDelR_jZ);
    plots->fOutTree->Branch("DelPhi_Hjj",&plots->DelPhi_Hjj);
    plots->fOutTree->Branch("DelEta_Hjj",&plots->DelEta_Hjj);
    plots->fOutTree->Branch("DelPhi_jj",&plots->DelPhi_jj);
    plots->fOutTree->Branch("DelR_jj",&plots->DelR_jj);
    plots->fOutTree->Branch("DelY_jj",&plots->DelY_jj);
    plots->fOutTree->Branch("Mjj",&plots->Mjj);
    plots->fOutTree->Branch("DelPt_jj",&plots->DelPt_jj);
    plots->fOutTree->Branch("DelPt_ZZ",&plots->DelPt_ZZ);
    plots->fOutTree->Branch("pT_Hjj",&plots->pT_Hjj);
    plots->fOutTree->Branch("Mjets",&plots->Mjets);
    plots->fOutTree->Branch("Zeppetaj3",&plots->Zeppetaj3);
    plots->fOutTree->Branch("ZeppetaZZ",&plots->ZeppetaZZ);
    
    plots->cutFlowTitleReady = false;
    plots->nevents = 0;
    
    if(DEBUG) cout << "End BookHistograms function" << endl;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



void WriteOutputFile(TString outfilename, myOutputs *plots)
{
    if(DEBUG) cout << "Begin WriteOutputFile function" << endl;
    
    TFile outfile(outfilename, "RECREATE");
    for(auto current_object : plots->map_objs_toBeSaved) outfile.Add(current_object.second, kTRUE);
    outfile.Write();
    outfile.Close();
    
    if(DEBUG) cout << "End WriteOutputFile function" << endl;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



template<typename T> void select_leptons( vector<T*> *veclep, cut_on_leptons& cutlep )
{
    if(DEBUG) cout << "Begin select_leptons function" << endl;
    
    while (true)
    {
        auto lep_i = veclep->begin(), lep_e = veclep->end();
        
        for( ; lep_i != lep_e ; ++lep_i)
        {
            if( ( (*lep_i)->PT > cutlep.min_pt )&&( fabs((*lep_i)->Eta) < cutlep.max_eta ) ) continue;
            if( (*lep_i)->IsolationVar > cutlep.max_iso ) continue;
            
            veclep->erase(lep_i);
            break;
        }
        
        if(lep_i == lep_e) break;
    }
    
    if(DEBUG) cout << "End select_leptons function" << endl;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



void select_jets(vector<Jet*> *vecjet, cut_on_jets& cutjet)
{
    if(DEBUG) cout << "Begin select_jets function" << endl;
    
    while (true)
    {
        auto jet_i = vecjet->begin(), jet_e = vecjet->end();
        
        for( ; jet_i != jet_e ; ++jet_i)
        {
            if( ( (*jet_i)->PT > cutjet.min_pt_centraljet )&&( fabs((*jet_i)->Eta) < cutjet.max_eta_centraljet ) ) continue;
            if( ( (*jet_i)->PT > cutjet.min_pt_forwardjet )&&( fabs((*jet_i)->Eta) < cutjet.max_eta_forwardjet ) ) continue;
            
            vecjet->erase(jet_i);
            break;
        }
        
        if(jet_i == jet_e) break;
    }
    
    if(DEBUG) cout << "End select_jets function" << endl;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



TLorentzVector select_zboson(TLorentzVector& lep1, TLorentzVector& lep2, cut_on_Zbosons& ZCuts)
{
    if(DEBUG) cout << "Begin select_zboson function" << endl;
    TLorentzVector z_4v = lep1 + lep2;
    TLorentzVector default_return(-999999,-999999,-999999,-999999);
    
    if( (z_4v.M() < ZCuts.min_mass)||(z_4v.M() > ZCuts.max_mass) ) return default_return;
    if( (lep1.Pt() < ZCuts.minPTlep1)&&(lep2.Pt() < ZCuts.minPTlep1) ) return default_return;
    if( (lep1.Pt() < ZCuts.minPTlep2)||(lep2.Pt() < ZCuts.minPTlep2) ) return default_return;
    
    if(DEBUG) cout << "End select_zboson function" << endl;
    return z_4v;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



void overlap_removal(vector<Jet*>& vecjet, vector<Electron*>& vecel, vector<Muon*>& vecmu, cut_on_overlap& cutOR)
{
    if(DEBUG) cout << "Begin overlap_removal function" << endl;
    for(auto current_jet = vecjet.begin() ; current_jet != vecjet.end() ; )
    {
        bool remove_this_jet = false;
        TLorentzVector tmpJetP4(0,0,0,0);
        tmpJetP4.SetPtEtaPhiM( (*current_jet)->PT, (*current_jet)->Eta, (*current_jet)->Phi, (*current_jet)->Mass );
        
        if(DEBUG) cout << "Start loop on electrons" << endl;
        for(auto current_el = vecel.begin() ; current_el != vecel.end() ; ++current_el) {
            TLorentzVector tmpElP4(0,0,0,0);
            tmpElP4.SetPtEtaPhiM( (*current_el)->PT, (*current_el)->Eta, (*current_el)->Phi, MEL_PDG);
            
            if( tmpElP4.DeltaR( tmpJetP4 ) < cutOR.min_DR_jetlep ) {
                remove_this_jet = true;
                break;
            }
        }
        
        if(DEBUG) cout << "Start loop on muons" << endl;
        for(auto current_mu = vecmu.begin() ; current_mu != vecmu.end() ; ++current_mu) {
            TLorentzVector tmpMuP4(0,0,0,0);
            tmpMuP4.SetPtEtaPhiM( (*current_mu)->PT, (*current_mu)->Eta, (*current_mu)->Phi, MMU_PDG);
            
            if( tmpMuP4.DeltaR( tmpJetP4 ) < cutOR.min_DR_jetlep ) {
                remove_this_jet = true;
                break;
            }
        }
        
        if(DEBUG) cout << "Remove the jet" << endl;
        if( remove_this_jet ) current_jet = vecjet.erase(current_jet);
        else ++current_jet;
    }
    if(DEBUG) cout << "End overlap_removal function" << endl;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



vector<int> pair_leptons_into_Zs(vector<Electron*>& electrons, vector<Muon*>& muons, vector<TLorentzVector>& out_lep4v)
{
    if(DEBUG) cout << "Begin pair_leptons_into_Zs function" << endl;
    
    out_lep4v = vector<TLorentzVector> (4, TLorentzVector(0,0,0,0));
    vector<int> list_leptons(4, -1);
    double bestMZtest = 9999*1000;
    
    for(int id_Z = 0 ; id_Z < 2 ; id_Z++) //Find on leading/subleading Z mass
    {
        //Find the best electrons pairs
        int id1 = 0, id2 = 0;
        for(vector<Electron*>::iterator first_electron = electrons.begin() ; first_electron != electrons.end() ; ++first_electron)
        {
            vector<Electron*>::iterator el_i = electrons.begin(), el_e = electrons.end();
            
            for(vector<Electron*>::iterator second_electron = find(el_i, el_e, *first_electron) ; second_electron != el_e ; ++second_electron)
            {
                if(*first_electron == *second_electron) continue; //Pass the first second electron
                if( (id_Z != 0)&&( (id1 == list_leptons.at(0))&&(id2 == list_leptons.at(1)) ) ) continue; //Pass the electrons attached to the first Z when building the second
                if( (*first_electron)->Charge * (*second_electron)->Charge > 0) continue; //Pass the same-sign pairs
                
                out_lep4v[0+id_Z*2].SetPtEtaPhiM( (*first_electron)->PT, (*first_electron)->Eta, (*first_electron)->Phi, MEL_PDG );
                out_lep4v[1+id_Z*2].SetPtEtaPhiM( (*second_electron)->PT, (*second_electron)->Eta, (*second_electron)->Phi, MEL_PDG );
                
                double currentMZtest = fabs( (out_lep4v[0+id_Z*2]+out_lep4v[1+id_Z*2]).M() - MZ_PDG);
                if(currentMZtest < bestMZtest) {
                    list_leptons.at(0+id_Z*2) = id1;
                    list_leptons.at(1+id_Z*2) = id2;
                }
                
                id2++;
            }
            
            id2 = 0;
            id1++;
        }
        
        //Find the best muons pairs
        id1 = 100, id2 = 100;
        for(vector<Muon*>::iterator first_muon = muons.begin() ; first_muon != muons.end() ; ++first_muon)
        {
            vector<Muon*>::iterator mu_i = muons.begin(), mu_e = muons.end();
            
            for(vector<Muon*>::iterator second_muon = find(mu_i, mu_e, *first_muon) ; second_muon != mu_e ; ++second_muon)
            {
                if(*first_muon == *second_muon) continue; //Pass the first second muon
                if( (id_Z != 0)&&( (id1 == list_leptons.at(0))&&(id2 == list_leptons.at(1)) ) ) continue; //Pass the muons attached to the first Z when building the second
                if( (*first_muon)->Charge * (*second_muon)->Charge > 0) continue; //Pass the same-sign pairs
                
                out_lep4v[0+id_Z*2].SetPtEtaPhiM( (*first_muon)->PT, (*first_muon)->Eta, (*first_muon)->Phi, MMU_PDG );
                out_lep4v[1+id_Z*2].SetPtEtaPhiM( (*second_muon)->PT, (*second_muon)->Eta, (*second_muon)->Phi, MMU_PDG );
                
                double currentMZtest = fabs( (out_lep4v[0+id_Z*2]+out_lep4v[1+id_Z*2]).M() - MZ_PDG);
                if(currentMZtest < bestMZtest) {
                    list_leptons.at(0+id_Z*2) = id1;
                    list_leptons.at(1+id_Z*2) = id2;
                }
                
                id2++;
            }
            
            id2 = 100;
            id1++;
        }
    }
    
    if(DEBUG) cout << "End pair_leptons_into_Zs function" << endl;
    return list_leptons;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



bool checkQuadDR(vector<TLorentzVector>& lepLorentzVectors, vector<int> particleidentifier, cut_on_overlap& cutOR)
{
    if(DEBUG) cout << "Begin checkQuadDR function" << endl;
    for(int id1 = 0 ; id1 < lepLorentzVectors.size() ; id1++)
    for(int id2 = id1+1 ; id2 < lepLorentzVectors.size() ; id2++)
    {
        double minDR;
        if( particleidentifier.at(id1)/100 == particleidentifier.at(id2)/100 ) minDR = cutOR.min_DR_lepSF;
        else minDR = cutOR.min_DR_lepNotSF;
        
        if( lepLorentzVectors.at(id1).DeltaR( lepLorentzVectors.at(id2) ) < minDR ) return false;
    }
    
    if(DEBUG) cout << "End checkQuadDR function" << endl;
    return true;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



void order_jets_byPT(vector<TLorentzVector>& jets_LV)
{
    vector<TLorentzVector> result_vector;
    
    for (auto current_LV : jets_LV)
    {
        double current_pt = current_LV.Pt();
        
        bool inserted = false;
        for(unsigned int id_j = 0 ; id_j < result_vector.size() ; ++id_j)
        {
            double test_pt = result_vector.at(id_j).Pt();
            if(current_pt > test_pt) result_vector.insert( result_vector.begin()+id_j, current_LV );
        }
        
        if(!inserted) result_vector.push_back( current_LV );
    }
    
    jets_LV = result_vector;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



void AnalyseEvents(ExRootTreeReader *treeReader, myOutputs *plots, Cuts* cuts)
{
    if(DEBUG) cout << "Begin AnalyseEvents function" << endl;
    
    Long64_t allEntries = treeReader->GetEntries();
    
    if(DEBUG) cout << "Recover Weights" << endl;
    TClonesArray *branchEvent = treeReader->UseBranch("Event");
    if(DEBUG) cout << "Recover jets" << endl;
    TClonesArray *branchJet = treeReader->UseBranch("Jet");
    if(DEBUG) cout << "Recover muons" << endl;
    TClonesArray *branchElectron = treeReader->UseBranch("Electron");
    if(DEBUG) cout << "Recover Muons" << endl;
    TClonesArray *branchMuon = treeReader->UseBranch("Muon");
    if(DEBUG) cout << "Recover Particles" << endl;
    TClonesArray *branchParticle = treeReader->UseBranch("Particle");
    if(DEBUG) cout << "Every particle have been recovered" << endl;
    
    cout << "** Chain contains " << allEntries << " events" << endl;
    
    Long64_t entry;
    
    //---------------------------------------------
    // Loop over all events
    for(entry = 0; entry < allEntries; ++entry)
    {
        vector<Jet*> jets;
        vector<Muon*> muons;
        vector<Electron*> electrons;
        
        clear_previous_event(plots);
        
        
        //---------------------------------------------
        // Load selected branches with data from specified event
        if(DEBUG) cout << __FILE__ << " " << __LINE__ << " read the current entry" << endl;
        treeReader->ReadEntry(entry);
        if(DEBUG) cout << __FILE__ << " " << __LINE__ << " recover the current weight" << endl;
        if(DEBUG) cout << __FILE__ << " " << __LINE__ << " nb of weights : " << branchEvent->GetSize() << endl;
        plots->weight = (branchEvent->GetSize() == 0) ? 1 : ((LHEFEvent*)branchEvent->At(0))->Weight; // TO DO : Support for multiple weights ?
        do_cutflow(plots, "Initial");
        
        //---------------------------------------------
        // Recover all the electrons, muons then jets
        for(int idel = 0 ; idel < branchElectron->GetEntries() ; idel++) electrons.push_back((Electron*)branchElectron->At(idel));
        for(int idmu = 0 ; idmu < branchMuon->GetEntries() ; idmu++) muons.push_back((Muon*)branchMuon->At(idmu));
        for(int idjet = 0 ; idjet < branchJet->GetEntries() ; idjet++) jets.push_back((Jet*)branchJet->At(idjet));
        if(DEBUG) cout << "Nb of leptons = "<< branchElectron->GetEntries() + branchMuon->GetEntries() << " " << electrons.size() + muons.size() << endl;
        
        //---------------------------------------------
        // Check the number of leptons
        plots->fNbLeptonBeforeKinSel->Fill(electrons.size() + muons.size());
        if( (electrons.size() + muons.size()) < 4 ) continue;
        do_cutflow(plots, "4 leptons");
        
        //---------------------------------------------
        // Apply PT and Eta cuts on leptons
        select_leptons<Electron> (&electrons, cuts->electron);
        select_leptons<Muon> (&muons, cuts->muon);
        plots->fNbLeptonAfterKinSel->Fill(electrons.size() + muons.size());
        if( (electrons.size() + muons.size()) < 4 ) continue;
        do_cutflow(plots, "Kin. Sel.");
        
        //---------------------------------------------
        // Apply PT and Eta cuts on jets + do lep-jet OR
        select_jets(&jets, cuts->jet);
        plots->Njets = jets.size();
        for(auto current_jet : jets) {
            plots->jet4v.push_back( TLorentzVector(0,0,0,0) );
            plots->jet4v.back().SetPtEtaPhiM( current_jet->PT, current_jet->Eta, current_jet->Phi, current_jet->Mass );
        }
        
        order_jets_byPT(plots->jet4v);
        overlap_removal(jets, electrons, muons, cuts->OR);
        if( (electrons.size() + muons.size()) < 4 ) continue;
        do_cutflow(plots, "jet-lep OR");
        
        //---------------------------------------------
        // Pair the leptons to give Z1 and Z2
        vector<int> lepton_list = pair_leptons_into_Zs(electrons, muons, plots->lepton4v); //id for muons is 100+actual_id
        if( (lepton_list.at(0) == -1)||(lepton_list.at(1) == -1)||(lepton_list.at(2) == -1)||(lepton_list.at(3) == -1) ) continue;
        do_cutflow(plots, "SFOS");
        
        if (!checkQuadDR(plots->lepton4v, lepton_list, cuts->OR)) continue;
        do_cutflow(plots, "lep-lep OR");
        
        //---------------------------------------------
        // Build the Z 4vectors and do Zmass cuts
        plots->z4v.push_back( select_zboson(plots->lepton4v.at(0), plots->lepton4v.at(1), cuts->Z1) );
        if(plots->z4v.back().E() < 0) continue;
        do_cutflow(plots, "MZ1");
        
        plots->z4v.push_back( select_zboson(plots->lepton4v.at(2), plots->lepton4v.at(3), cuts->Z2) );
        if(plots->z4v.back().E() < 0) continue;
        do_cutflow(plots, "MZ2");
        
        //---------------------------------------------
        // Fill some variables !
        
        for(unsigned int id = 0 ; id < lepton_list.size() ; id++) {
            if(lepton_list.at(id)/100 == 1) plots->lepton_flavour.push_back( muons.at(lepton_list.at(id)%100)->Charge * (-13) );
            else plots->lepton_flavour.push_back( electrons.at(lepton_list.at(id)%100)->Charge * (-11) );
        }
        
        //Build the Higgs
        plots->h4v = plots->z4v.at(0) + plots->z4v.at(1);
        
        plots->DelPt_ZZ = plots->z4v.at(0).Pt() - plots->z4v.at(1).Pt();
        
        TLorentzVector alljetssum4v(0,0,0,0);
        for(auto current_jet4v : plots->jet4v) alljetssum4v = alljetssum4v + current_jet4v;
        plots->Mjets = alljetssum4v.M();
        
        //Build things with jets
        if (plots->Njets > 0)
        {
            double minDRzj = 999999;
            for (auto current_jet : plots->jet4v)
            for (auto current_z : plots->z4v)
            {
                double currentDR = current_z.DeltaR(current_z);
                if (currentDR < minDRzj) minDRzj = currentDR;
            }
            plots->minDelR_jZ = minDRzj;
        }
        
        if(plots->Njets > 1)
        {
            plots->dijet4v = plots->jet4v[0]+plots->jet4v[1];
            
            plots->Mjj = plots->dijet4v.M();
            plots->DelPhi_jj = plots->jet4v[0].DeltaPhi(plots->jet4v[1]);
            plots->DelY_jj = plots->jet4v[0].Rapidity() - plots->jet4v[1].Rapidity();
            plots->DelR_jj = plots->jet4v[0].DeltaR(plots->jet4v[1]);
            plots->DelPt_jj = plots->jet4v[0].Pt() - plots->jet4v[1].Pt();
            
            plots->DelPhi_Hjj = plots->h4v.DeltaPhi(plots->dijet4v);
            plots->DelEta_Hjj = plots->h4v.Eta() - plots->dijet4v.Eta();
            
            plots->pT_Hjj = (plots->dijet4v + plots->h4v).Pt();
            
            plots->ZeppetaZZ = plots->h4v.Eta() - ((plots->jet4v[1].Eta()+plots->jet4v[0].Eta())/2.);
        }
        
        if(plots->Njets > 2)
        {
            plots->Zeppetaj3 = plots->jet4v[2].Eta() - ((plots->jet4v[1].Eta()+plots->jet4v[0].Eta())/2.);
        }
        
        //------------------------------------
        //---          End of event
        //------------------------------------
        plots->cutFlowTitleReady = true;
        plots->fOutTree->Fill();
    }
    
    cout << plots->nevents << " have been read" << endl;
    if(DEBUG) cout << "End AnalyseEvents function" << endl;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



void analyzer(TString inFileName, TString outFileName)
{
  //gSystem->Load("/Users/cyril/Travail/NecessaryApps/delphes/libDelphes.so");
  //TString input_file_name = "/Users/cyril/Travail/HZZ4l/AnomalousCouplings/SampleProduction/AnalyzeDelphes/testout.root";
    cout << "----- Read the delphes output -----" << endl;
    TString inputFile = inFileName+"/Delphes";
    TChain *chain = new TChain("Delphes");
    chain->Add(inputFile);
    
    cout << "----- Create the ExRootTreeReader -----" << endl;
    cout << "--- Number of events from the TChain " << endl; cout << chain->GetEntriesFast() << endl;
    ExRootTreeReader *treeReader = new ExRootTreeReader(chain);
    cout << "--- Number of events from ExRootTreeReader " << endl; cout << treeReader->GetEntries() << endl;
    
    cout << "----- Define the output wanted -----" << endl;
    myOutputs *plots = new myOutputs;
    cout << "----- Define the cuts wanted -----" << endl;
    Cuts myCuts; set_cuts(myCuts);
    cout << "----- Book the histograms and trees -----" << endl;
    BookHistograms(plots);
    
    cout << "----- Do the analysis -----" << endl;
    AnalyseEvents(treeReader, plots, &myCuts);
    
    cout << "----- Write out the wanted analyzed information -----" << endl;
    WriteOutputFile(outFileName, plots);
    
    cout << "*** Now Exiting... ***" << endl;
    
    delete plots;
    delete treeReader;
    delete chain;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



