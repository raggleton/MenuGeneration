#ifndef l1menu_L1TriggerDPGEvent_h
#define l1menu_L1TriggerDPGEvent_h

#include <memory>
#include "l1menu/IEvent.h"



// data formats
#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1HFRingsFwd.h"
#include "DataFormats/L1Trigger/interface/L1HFRings.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"

#include "UserCode/L1TriggerDPG/interface/L1AnalysisL1Extra.h"

// Forward declarations
namespace L1Analysis
{
	class L1AnalysisDataFormat;
}


namespace l1menu
{
	/** @brief Wrapper for the event format. Bundles L1AnalysisDataFormat and the trigger bits into one class.
	 *
	 * Currently just wraps L1Analysis::L1AnalysisDataFormat (from UserCode/L1TriggerDPG) and a boolean array
	 * for the trigger bits into one class so that it can easily be passed around. Also useful because it can
	 * be passed around and have some operations done on it by code that has no knowledge of L1Analysis package.
	 *
	 * Later on I might wrap the L1AnalysisDataFormat more fully so that everything can be done without
	 * knowledge of L1AnalysisDataFormat, just using this lightweight interface.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 21/May/2013
	 */
	class L1TriggerDPGEvent : public l1menu::IEvent
	{
	public:
		L1TriggerDPGEvent();
		L1TriggerDPGEvent( const l1menu::ISample& parentSample );
		L1TriggerDPGEvent( const L1TriggerDPGEvent& otherEvent );
		L1TriggerDPGEvent( L1TriggerDPGEvent&& otherEvent ) noexcept;
		L1TriggerDPGEvent& operator=( const L1TriggerDPGEvent& otherEvent );
		L1TriggerDPGEvent& operator=( L1TriggerDPGEvent&& otherEvent ) noexcept;
		virtual ~L1TriggerDPGEvent();

		virtual L1Analysis::L1AnalysisDataFormat& rawEvent();
		virtual const L1Analysis::L1AnalysisDataFormat& rawEvent() const;
		virtual bool* physicsBits(); ///< @brief A 128 element array of the physics bits
		virtual const bool* physicsBits() const; ///< @brief Const access to the 128 element array of the physics bits.

		// manual setters
		virtual void setRun( int run );
		virtual void setLS( int ls );
		virtual void setEventNum( int eventNum );
		virtual void setWeight( float weight );
		virtual void setL1Bits( bool * bits );

		virtual void setEG( edm::Handle<l1extra::L1EmParticleCollection> nonIsoEm, edm::Handle<l1extra::L1EmParticleCollection> isoEm );		
		virtual void setJets( edm::Handle<l1extra::L1JetParticleCollection> cenJet, edm::Handle<l1extra::L1JetParticleCollection> fwdJet );
		virtual void setTaus( edm::Handle<l1extra::L1JetParticleCollection> tauJet, edm::Handle<l1extra::L1JetParticleCollection> isoTauJet );		
		virtual void setETSums( edm::Handle<l1extra::L1EtMissParticleCollection> mets ); // do ET total, |MET|
		virtual void setHTSums( edm::Handle<l1extra::L1EtMissParticleCollection> mhts ); // do HT total, |MHT|
		// virtual void setHFring( edm::Handle<l1extra::L1HFRingsCollection> hfRings );
		virtual void setMuons( edm::Handle<l1extra::L1MuonParticleCollection> muon);

		//
		// These are the methods required by the l1menu::IEvent interface.
		//
		virtual bool passesTrigger( const l1menu::ITrigger& trigger ) const;
		virtual float weight() const;
		virtual const l1menu::ISample& sample() const;
	protected:
		/** @brief Hide implementation details in a pimple.
		 * In particular I don't want any code that includes this file to be dependent
		 * on L1AnalysisDataFormat.h. */
		std::unique_ptr<class L1TriggerDPGEventPrivateMembers> pImple_;
	};

} // end of namespace l1menu


#endif
