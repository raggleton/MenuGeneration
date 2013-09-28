#include "l1menu/MenuRateMuonScaling.h"

#include <stdexcept>
#include <TH1.h>
#include <TFile.h>

//
// The pimple was implicitly declared, still need to declare it properly.
//
namespace l1menu
{
	class MenuRateMuonScalingPrivateMembers
	{
	public:
		MenuRateMuonScalingPrivateMembers( std::shared_ptr<const l1menu::IMenuRate> pUnscaledRate ) : pUnscaledMenuRate(pUnscaledRate) {}
		std::vector<const l1menu::ITriggerRate*> triggerRates;
		std::shared_ptr<const l1menu::IMenuRate> pUnscaledMenuRate;
		std::unique_ptr<TH1> pMuonScalePtPlot;
		std::unique_ptr<TH1> pMuonScaleIsolationPlot;
	};
}

l1menu::MenuRateMuonScaling::MenuRateMuonScaling( std::shared_ptr<const l1menu::IMenuRate> pUnscaledMenuRate, const std::string& muonScalingFilename )
	: pImple_( new l1menu::MenuRateMuonScalingPrivateMembers(pUnscaledMenuRate) )
{
	std::unique_ptr<TFile> pMuonScalingFile( TFile::Open(muonScalingFilename.c_str()) );
	if( pMuonScalingFile==nullptr ) throw std::runtime_error( "Couldn't create MenuRateMuonScaling because couldn't open TFile "+muonScalingFilename );

	pImple_->pMuonScalePtPlot.reset( static_cast<TH1*>( pMuonScalingFile->Get("muonPtScale")->Clone() ) );
	pImple_->pMuonScalePtPlot->SetDirectory( nullptr ); // Make sure it's only held in memory. Might be the default for clones but not sure.
	if( pImple_->pMuonScalePtPlot==nullptr ) throw std::runtime_error( "Couldn't create MenuRateMuonScaling because couldn't get the TH1 \"muonPtScale\" from the  TFile" );

	pImple_->pMuonScaleIsolationPlot.reset( static_cast<TH1*>( pMuonScalingFile->Get("muonIsoScale")->Clone() ) );
	pImple_->pMuonScaleIsolationPlot->SetDirectory( nullptr ); // Make sure it's only held in memory.
	if( pImple_->pMuonScaleIsolationPlot==nullptr ) throw std::runtime_error( "Couldn't create MenuRateMuonScaling because couldn't get the TH1 \"muonIsoScale\" from the  TFile" );

}

l1menu::MenuRateMuonScaling::~MenuRateMuonScaling()
{
	// No operation
}

// The methods required by the IMenuRate interface
float l1menu::MenuRateMuonScaling::totalFraction() const
{
	return -1;
}

float l1menu::MenuRateMuonScaling::totalFractionError() const
{
	return -1;
}

float l1menu::MenuRateMuonScaling::totalRate() const
{
	return -1;
}

float l1menu::MenuRateMuonScaling::totalRateError() const
{
	return -1;
}

const std::vector<const l1menu::ITriggerRate*>& l1menu::MenuRateMuonScaling::triggerRates() const
{
	return pImple_->triggerRates;
}

