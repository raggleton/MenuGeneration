#include <vector>
#include <algorithm>

#include "l1menu/L1TriggerDPGEvent.h"
#include "l1menu/ITrigger.h"
#include "UserCode/L1TriggerUpgrade/interface/L1AnalysisDataFormat.h"
#include "UserCode/L1TriggerDPG/interface/L1AnalysisGMTDataFormat.h"

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

		// taken from FullSample.cpp:
		int phiINjetCoord( double phi );
		int etaINjetCoord( double eta );
		double calculateHTT( const L1Analysis::L1AnalysisDataFormat& event );
		double calculateHTM( const L1Analysis::L1AnalysisDataFormat& event );
	
	private:
		static const size_t PHIBINS;
		static const double PHIBIN[];
		static const size_t ETABINS;
		static const double ETABIN[];

		double degree( double radian );
	};
}

const size_t l1menu::L1TriggerDPGEventPrivateMembers::PHIBINS=18;
const double l1menu::L1TriggerDPGEventPrivateMembers::PHIBIN[]={10,30,50,70,90,110,130,150,170,190,210,230,250,270,290,310,330,350};
const size_t l1menu::L1TriggerDPGEventPrivateMembers::ETABINS=23;
const double l1menu::L1TriggerDPGEventPrivateMembers::ETABIN[]={-5.,-4.5,-4.,-3.5,-3.,-2.172,-1.74,-1.392,-1.044,-0.696,-0.348,0,0.348,0.696,1.044,1.392,1.74,2.172,3.,3.5,4.,4.5,5.};

double l1menu::L1TriggerDPGEventPrivateMembers::degree( double radian )
{
	if( radian<0 ) return 360.+(radian/M_PI*180.);
	else return radian/M_PI*180.;
}

int l1menu::L1TriggerDPGEventPrivateMembers::phiINjetCoord( double phi )
{
	size_t phiIdx=0;
	double phidegree=degree( phi );
	for( size_t idx=0; idx<PHIBINS; idx++ )
	{
		if( phidegree>=PHIBIN[idx] and phidegree<PHIBIN[idx+1] ) phiIdx=idx;
		else if( phidegree>=PHIBIN[PHIBINS-1] || phidegree<=PHIBIN[0] ) phiIdx=idx;
	}
	phiIdx=phiIdx+1;
	if( phiIdx==18 ) phiIdx=0;
	return int( phiIdx );
}

int l1menu::L1TriggerDPGEventPrivateMembers::etaINjetCoord( double eta )
{
	size_t etaIdx=0.;
	for( size_t idx=0; idx<ETABINS; idx++ )
	{
		if( eta>=ETABIN[idx] and eta<ETABIN[idx+1] ) etaIdx=idx;
	}
	return int( etaIdx );
}

double l1menu::L1TriggerDPGEventPrivateMembers::calculateHTT( const L1Analysis::L1AnalysisDataFormat& event )
{
	double httValue=0.;

	// Calculate our own HT and HTM from the jets that survive the double jet removal.
	for( int i=0; i<event.Njet; i++ )
	{
		if( event.Bxjet.at( i )==0 && !event.Taujet.at(i) )
		{
			if( event.Etajet.at( i )>4 and event.Etajet.at( i )<17 )
			{
				httValue+=event.Etjet.at( i );
			} //in proper eta range
		} //correct beam crossing
	} //loop over cleaned jets

	return httValue;
}

double l1menu::L1TriggerDPGEventPrivateMembers::calculateHTM( const L1Analysis::L1AnalysisDataFormat& event )
{
	double htmValue=0.;
	double htmValueX=0.;
	double htmValueY=0.;

	// Calculate our own HT and HTM from the jets that survive the double jet removal.
	for( int i=0; i<event.Njet; i++ )
	{
		if( event.Bxjet.at( i )==0 && !event.Taujet.at(i) )
		{
			if( event.Etajet.at( i )>4 and event.Etajet.at( i )<17 )
			{

				//  Get the phi angle  towers are 0-17 (this is probably not real mapping but OK for just magnitude of HTM
				float phi=2*M_PI*(event.Phijet.at( i )/18.);
				htmValueX+=cos( phi )*event.Etjet.at( i );
				htmValueY+=sin( phi )*event.Etjet.at( i );

			} //in proper eta range
		} //correct beam crossing
	} //loop over cleaned jets

	htmValue=sqrt( htmValueX*htmValueX+htmValueY*htmValueY );

	return htmValue;
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

// Manual setters, implementation of FullSample::fillSample()
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
	// ATM just mark all jets as not tau jets!!
	// In which case might require merging setJets and setTaus

	// Do cen jets first
	for(l1extra::L1JetParticleCollection::const_iterator it=cenJet->begin(); it!=cenJet->end(); it++)
	{
		// For each jet look for a possible duplicate if so remove it.
		// Need to do for forward jets as well??
		bool duplicate=false;
		for( l1extra::L1JetParticleCollection::const_iterator itDup=cenJet->begin(); itDup<it; itDup++ )
		{
			if(    it->bx()  == itDup->bx()
				&& it->et()  == itDup->et()
				&& it->eta() == itDup->eta()
				&& it->phi() == itDup->phi() )
			{
				duplicate=true;
				printf("Duplicate jet found and removed \n");
			}
		}

		if( !duplicate )
		{
			pImple_->rawEvent.Etjet.push_back(it->et());
			pImple_->rawEvent.Etajet.push_back(pImple_->etaINjetCoord(it->eta())); // stores eta bin index NOT real eta!! - make switchable?
			pImple_->rawEvent.Phijet.push_back(pImple_->phiINjetCoord(it->phi())); // stores phi bin index, NOT real phi!!
			pImple_->rawEvent.Bxjet.push_back(it->bx());
			// pImple_->rawEvent.Fwdjet.push_back(false); // COMMENT OUT IF JET ETA FIX
			// JET ETA FIX:
			// Some Jets with eta>3 has appeared in central jet list.  Move them by hand
			//  This is a problem in Stage 2 Jet code.
			(fabs( it->eta() )>=3.0) ? pImple_->rawEvent.Fwdjet.push_back(true) : pImple_->rawEvent.Fwdjet.push_back(false);
			pImple_->rawEvent.Taujet.push_back(false);
			pImple_->rawEvent.Njet++;
		}
    }

    // Do forward jets
	for(l1extra::L1JetParticleCollection::const_iterator it=fwdJet->begin(); it!=fwdJet->end(); it++)
	{
		pImple_->rawEvent.Etjet.push_back(it->et());
		pImple_->rawEvent.Etajet.push_back(pImple_->etaINjetCoord(it->eta()));
		pImple_->rawEvent.Phijet.push_back(pImple_->phiINjetCoord(it->phi()));
		pImple_->rawEvent.Bxjet.push_back(it->bx());
		pImple_->rawEvent.Fwdjet.push_back(true);
		pImple_->rawEvent.Taujet.push_back(false);
		pImple_->rawEvent.Njet++;
    }

}		


void l1menu::L1TriggerDPGEvent::setTaus( edm::Handle<l1extra::L1JetParticleCollection> tauJet, edm::Handle<l1extra::L1JetParticleCollection> isoTauJet )
{
	// Note you don't need to pass it a real isoTau collection - it will check for it below
	for(l1extra::L1JetParticleCollection::const_iterator it=tauJet->begin(); it!=tauJet->end(); it++)
	{
		// remove duplicates
		bool duplicate=false;
		for( l1extra::L1JetParticleCollection::const_iterator itDup=tauJet->begin(); itDup<it; itDup++ )
		{
			if(    it->bx()  == itDup->bx()
				&& it->et()  == itDup->et()
				&& it->eta() == itDup->eta()
				&& it->phi() == itDup->phi() )
			{
				duplicate=true;
				printf("Duplicate tau found and removed \n");
			}
		}

		if( !duplicate )
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
			if (isoTauJet.product() != NULL) 
			{
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
			}
			pImple_->rawEvent.isoTaujet.push_back(iso);
		}
    }
}		

void l1menu::L1TriggerDPGEvent::setETSums( edm::Handle<l1extra::L1EtMissParticleCollection> mets )
{
	//ET Total, |MET, PhiETM
	for(l1extra::L1EtMissParticleCollection::const_iterator it=mets->begin(); it!=mets->end(); it++) 
	{
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
	for(l1extra::L1EtMissParticleCollection::const_iterator it=mhts->begin(); it!=mhts->end(); it++) 
	{
		if (it->bx() == 0)
		{
			pImple_->rawEvent.HTT = pImple_->calculateHTT( pImple_->rawEvent );
			pImple_->rawEvent.HTM = pImple_->calculateHTM( pImple_->rawEvent );
			pImple_->rawEvent.PhiHTM = 0.;
			// pImple_->rawEvent.mhtBx. push_back( it->bx() );
			// pImple_->rawEvent.nMht++;
		}
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

// For L1Extra muons
void l1menu::L1TriggerDPGEvent::setMuons( edm::Handle<l1extra::L1MuonParticleCollection> muon )
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
		pImple_->rawEvent.Isomu.push_back(it->isIsolated());
		// pImple_->rawEvent.Isomu.push_back(false);
		pImple_->rawEvent.Bxmu.push_back(it->bx());
		pImple_->rawEvent.Qualmu.push_back(it->gmtMuonCand().quality());
		pImple_->rawEvent.Nmu++;
    }
}

// For re-emulated GMT muons
void l1menu::L1TriggerDPGEvent::setMuons( edm::Handle<L1GlobalTriggerReadoutRecord> gtrr )
// void l1menu::L1TriggerDPGEvent::setMuons( edm::Handle<L1MuGMTReadoutCollection> reEmulMuon )
{
	// methods taken from L1AnalysisGMT.cc, ...
	// const L1MuGMTReadoutCollection * gmtRC = (gtrr->muCollectionRefProd());
	const edm::RefProd<L1MuGMTReadoutCollection> gmtRC = (gtrr->muCollectionRefProd());

	std::vector<L1MuGMTReadoutRecord> const & gmt_records = (gmtRC.product())->getRecords();
	std::cout << gmt_records.empty() << std::endl;
	// for(std::vector<L1MuGMTReadoutRecord>::const_iterator igmtrr=gmt_records.begin(); igmtrr!=gmt_records.end(); igmtrr++) 
	// { // loop over bunch crossings

	//     std::vector<L1MuGMTExtendedCand> exc = igmtrr->getGMTCands();
	// 	for(std::vector<L1MuGMTExtendedCand>::const_iterator gmt_iter=exc.begin(); gmt_iter!=exc.end(); gmt_iter++)
	// 	{ // loop over muon candidates within a bunch crossing
	// 		if(!(*gmt_iter).empty())
	// 		{
	// 			pImple_->rawEvent.Ptmu.push_back(gmt_iter->ptValue());
	// 			pImple_->rawEvent.Etamu.push_back(gmt_iter->etaValue());
	// 			pImple_->rawEvent.Phimu.push_back(gmt_iter->phiValue());
	// 			// pImple_->rawEvent.muonChg.push_back(gmt_iter->charge());
	// 			// pImple_->rawEvent.muonMip.push_back(gmt_iter->mip());
	// 			// pImple_->rawEvent.muonFwd.push_back(it->isForward());
	// 			// pImple_->rawEvent.muonRPC.push_back(gmt_iter->isRPC());
	// 			// pImple_->rawEvent.Isomu.push_back(it->isIsolated());
	// 			pImple_->rawEvent.Isomu.push_back(gmt_iter->isol());
	// 			pImple_->rawEvent.Bxmu.push_back(gmt_iter->bx());
	// 			pImple_->rawEvent.Qualmu.push_back(gmt_iter->quality());
	// 			pImple_->rawEvent.Nmu++;
	// 		}
	// 	}
	// }
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
