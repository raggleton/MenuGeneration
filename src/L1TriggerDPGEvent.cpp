#include <vector>
#include <algorithm>

#include "l1menu/L1TriggerDPGEvent.h"
#include "l1menu/ITrigger.h"
#include "UserCode/L1TriggerUpgrade/interface/L1AnalysisDataFormat.h"

namespace l1menu
{
	class L1TriggerDPGEventPrivateMembers
	{
	public:
		L1TriggerDPGEventPrivateMembers() : pParentSample_(0) {}
		L1TriggerDPGEventPrivateMembers( const l1menu::ISample* pParentSample ) : pParentSample_(pParentSample) {}
		L1Analysis::L1AnalysisDataFormat rawEvent;
		bool physicsBits[128];
		float weight;
		const l1menu::ISample* pParentSample_;
	};
}

l1menu::L1TriggerDPGEvent::L1TriggerDPGEvent(): pImple_( new L1TriggerDPGEventPrivateMembers() )
{
	// Blank constructor, designed to be used with set methods for manual entry
}

l1menu::L1TriggerDPGEvent::L1TriggerDPGEvent( const l1menu::ISample& parentSample ) : pImple_( new L1TriggerDPGEventPrivateMembers(&parentSample) )
{
	pImple_->weight=1;
}

l1menu::L1TriggerDPGEvent::L1TriggerDPGEvent( const L1TriggerDPGEvent& otherEvent ) : pImple_( new L1TriggerDPGEventPrivateMembers(*otherEvent.pImple_) )
{
	// No operation besides the initialiser list
}

l1menu::L1TriggerDPGEvent::L1TriggerDPGEvent( L1TriggerDPGEvent&& otherEvent ) noexcept : pImple_( std::move(otherEvent.pImple_) )
{
	// No operation besides the initialiser list
}

l1menu::L1TriggerDPGEvent& l1menu::L1TriggerDPGEvent::operator=( const L1TriggerDPGEvent& otherEvent )
{
	*pImple_=*otherEvent.pImple_;
	return *this;
}

l1menu::L1TriggerDPGEvent& l1menu::L1TriggerDPGEvent::operator=( L1TriggerDPGEvent&& otherEvent ) noexcept
{
	pImple_=std::move(otherEvent.pImple_);
	return *this;
}

l1menu::L1TriggerDPGEvent::~L1TriggerDPGEvent()
{
	// No operation
}

L1Analysis::L1AnalysisDataFormat& l1menu::L1TriggerDPGEvent::rawEvent()
{
	return pImple_->rawEvent;
}

const L1Analysis::L1AnalysisDataFormat& l1menu::L1TriggerDPGEvent::rawEvent() const
{
	return pImple_->rawEvent;
}

bool* l1menu::L1TriggerDPGEvent::physicsBits()
{
	return pImple_->physicsBits;
}

const bool* l1menu::L1TriggerDPGEvent::physicsBits() const
{
	return pImple_->physicsBits;
}

void l1menu::L1TriggerDPGEvent::setRun( int run )
{
	pImple_->rawEvent.Run = run;
}

void l1menu::L1TriggerDPGEvent::setLS( int ls )
{
	pImple_->rawEvent.LS = ls;
}

void l1menu::L1TriggerDPGEvent::setEventNum( int eventNum )
{
	pImple_->rawEvent.Event = eventNum;
}

void l1menu::L1TriggerDPGEvent::setWeight( float weight )
{
	pImple_->weight=weight;
}

void l1menu::L1TriggerDPGEvent::setL1Bits( bool * bits)
{

	// Passing arrays is always a bit dodgy. Should probably do this better - this assumes 128 bits! 
	// set the pimpl member bits to argument bits
	std::copy(bits, bits+128, pImple_->physicsBits);
	
}

// Manual setters
// If you change the DataFormat you might need to change this!
// instead of storing 2 collections for isolated and non-isolated obj (former is subset of latter),
// add in a bool flag to indicate if a particular EG is isolated or not.
// Could add in generic method for looping over 2 collections, but need specifics for each scenario
void l1menu::L1TriggerDPGEvent::setEG( edm::Handle<l1extra::L1EmParticleCollection> nonIsoEm, edm::Handle<l1extra::L1EmParticleCollection> isoEm )
{
	for(l1extra::L1EmParticleCollection::const_iterator it=nonIsoEm->begin(); it!=nonIsoEm->end(); it++)
	{  
		pImple_->rawEvent.Etel.push_back(it->et());
		pImple_->rawEvent.Etael.push_back(it->eta());
		pImple_->rawEvent.Phiel.push_back(it->phi());
		pImple_->rawEvent.Bxel.push_back(it->bx());
		pImple_->rawEvent.Nele++;

		// test if same object in the isolated list
		bool iso = false;
		for(l1extra::L1EmParticleCollection::const_iterator itIso=isoEm->begin(); itIso!=isoEm->end(); itIso++)
		{
			if ( it->eta() == itIso->eta() && it->phi() == itIso->phi() )
			{
				// remove the iso obj from the iso collection to make future loops faster
				// isoEm->erase(itIso);
				iso = true;
				break;
			}
		}
		pImple_->rawEvent.Isoel.push_back(iso);
      
    }
}		

void l1menu::L1TriggerDPGEvent::setJets( edm::Handle<l1extra::L1JetParticleCollection> cenJet, edm::Handle<l1extra::L1JetParticleCollection> fwdJet )
{
	// TODO(Robin): Do I need to clean/mark those jets which are also taus? 
	// In which case might require merging setJets and setTaus

	// Do cen jets first
	for(l1extra::L1JetParticleCollection::const_iterator it=cenJet->begin(); it!=cenJet->end(); it++)
	{
		pImple_->rawEvent.Etjet.push_back(it->et());
		pImple_->rawEvent.Etajet.push_back(it->eta());
		pImple_->rawEvent.Phijet.push_back(it->phi());
		pImple_->rawEvent.Bxjet.push_back(it->bx());
		pImple_->rawEvent.Fwdjet.push_back(false);
		pImple_->rawEvent.Taujet.push_back(true);
		pImple_->rawEvent.Njet++;
    }

    // Do forward jets
	for(l1extra::L1JetParticleCollection::const_iterator it=fwdJet->begin(); it!=fwdJet->end(); it++)
	{
		pImple_->rawEvent.Etjet.push_back(it->et());
		pImple_->rawEvent.Etajet.push_back(it->eta());
		pImple_->rawEvent.Phijet.push_back(it->phi());
		pImple_->rawEvent.Bxjet.push_back(it->bx());
		pImple_->rawEvent.Fwdjet.push_back(true);
		pImple_->rawEvent.Taujet.push_back(true);
		pImple_->rawEvent.Njet++;
    }

}		

void l1menu::L1TriggerDPGEvent::setTaus( edm::Handle<l1extra::L1JetParticleCollection> tauJet, edm::Handle<l1extra::L1JetParticleCollection> isoTauJet )
{
	for(l1extra::L1JetParticleCollection::const_iterator it=tauJet->begin(); it!=tauJet->end(); it++)
	{
		pImple_->rawEvent.Etjet.push_back(it->et());
		pImple_->rawEvent.Etajet.push_back(it->eta());
		pImple_->rawEvent.Phijet.push_back(it->phi());
		pImple_->rawEvent.Bxjet.push_back(it->bx());
		pImple_->rawEvent.Fwdjet.push_back(false);
		pImple_->rawEvent.Taujet.push_back(true);
		pImple_->rawEvent.Njet++;

		// test if same object in the isolated list
		bool iso = false;
		for(l1extra::L1JetParticleCollection::const_iterator itIso=isoTauJet->begin(); itIso!=isoTauJet->end(); itIso++)
		{
			if ( it->eta() == itIso->eta() && it->phi() == itIso->phi() )
			{
				// remove the iso obj from the iso collection to make future loops faster
				// isoEm->erase(itIso);
				iso = true;
				break;
			}
		}
		pImple_->rawEvent.isoTaujet.push_back(iso);
    } // TODO - ISO TAUS
}		

void l1menu::L1TriggerDPGEvent::setETSums( edm::Handle<l1extra::L1EtMissParticleCollection> mets )
{
	//ET Total, |MET, PhiETM
	for(l1extra::L1EtMissParticleCollection::const_iterator it=mets->begin(); it!=mets->end(); it++) {
	    pImple_->rawEvent.ETT = it->etTotal(); 
	    pImple_->rawEvent.ETM = it->et();
	    pImple_->rawEvent.PhiETM = it->phi();
	    // pImple_->rawEvent.metBx. push_back( it->bx() );
	    // pImple_->rawEvent.nMet++;
	}
	// Overflow not available 
	pImple_->rawEvent.OvETT = false;
	pImple_->rawEvent.OvETM = false;
	
} 

void l1menu::L1TriggerDPGEvent::setHTSums( edm::Handle<l1extra::L1EtMissParticleCollection> mhts )
{
	//HT Total, |MHT|, PhiHTM
	
	// TODO(Robin): check with Mark to see it this is correct - am I meant to use the calculateHTT and calculateHTM methods in FullSample?
	for(l1extra::L1EtMissParticleCollection::const_iterator it=mhts->begin(); it!=mhts->end(); it++) {
		pImple_->rawEvent.HTT = it->etTotal();
		pImple_->rawEvent.HTM = it->et();
		pImple_->rawEvent.PhiHTM = it->phi();
		// pImple_->rawEvent.mhtBx. push_back( it->bx() );
		// pImple_->rawEvent.nMht++;
	}
	// Overflow not available
	pImple_->rawEvent.OvHTT = false;
	pImple_->rawEvent.OvHTM = false;
	
} 

/*
void l1menu::L1TriggerDPGEvent::setHFring( edm::Handle<l1extra::L1HFRingsCollection> hfRings )
{
	//ETT, ETM, PhiETM, HTT, HTM, PhiHTM
   pImple_->rawEvent.hfEtSum.resize(4);
   pImple_->rawEvent.hfBitCnt.resize(4);
   pImple_->rawEvent.hfBx.resize(4);
  
    for(unsigned int i=0; i<4; ++i)  
    {
		if (hfRings->size()==0) continue;

		pImple_->rawEvent.hfEtSum[i] = (hfRings->begin()->hfEtSum((l1extra::L1HFRings::HFRingLabels) i));
		pImple_->rawEvent.hfBitCnt[i] = (hfRings->begin()->hfBitCount((l1extra::L1HFRings::HFRingLabels) i));
		pImple_->rawEvent.hfBx[i] = hfRings->begin()->bx();
    }
	
	std::cout << "This method not implemented yet. You shouldn't be calling it!" << std::endl;
} 
*/


void l1menu::L1TriggerDPGEvent::setMuons( edm::Handle<l1extra::L1MuonParticleCollection> muon)
{

	for(l1extra::L1MuonParticleCollection::const_iterator it=muon->begin(); it!=muon->end(); it++)
	{
		pImple_->rawEvent.Ptmu.push_back(it->et());
		pImple_->rawEvent.Etamu.push_back(it->eta());
		pImple_->rawEvent.Phimu.push_back(it->phi());
		// pImple_->rawEvent.muonChg.push_back(it->charge());
		// pImple_->rawEvent.muonMip.push_back(it->isMip());
		// pImple_->rawEvent.muonFwd.push_back(it->isForward());
		// pImple_->rawEvent.muonRPC.push_back(it->isRPC());
		// pImple_->rawEvent.Isomu.push_back(it->isIsolated());
		pImple_->rawEvent.Isomu.push_back(false);
		pImple_->rawEvent.Bxmu.push_back(it->bx());
		pImple_->rawEvent.Qualmu.push_back(it->gmtMuonCand().quality());
		pImple_->rawEvent.Nmu++;
    }
}


bool l1menu::L1TriggerDPGEvent::passesTrigger( const l1menu::ITrigger& trigger ) const
{
	// This is an IEvent method, but ITrigger has a method that can
	// handle L1TriggerDPGEvent concrete objects, so call that
	// reflexively.
	return trigger.apply( *this );
}

float l1menu::L1TriggerDPGEvent::weight() const
{
	return pImple_->weight;
}

const l1menu::ISample& l1menu::L1TriggerDPGEvent::sample() const
{
	return *pImple_->pParentSample_;
}
