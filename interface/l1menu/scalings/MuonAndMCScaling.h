#ifndef l1menu_MuonAndMCScaling_h
#define l1menu_MuonAndMCScaling_h

#include "l1menu/IScaling.h"
#include <memory>

namespace l1menu
{
	namespace scalings
	{
		/** @brief Implementation of IScaling that scales for muon pt assignment, muon isolation, and data
		 * to Monte Carlo differences.
		 *
		 * Ideally the muon pt assignment and isolation scaling would be in one class, and the data to Monte
		 * Carlo scaling would be in another. Because of the practicalities of how this is done I'm not convinced
		 * this would give as good results though.
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 29/Sep/2013
		 */
		class MuonAndMCScaling : public l1menu::IScaling
		{
		public:
			MuonAndMCScaling( const std::string& muonScalingFilename, const std::string& monteCarloRatesFilename, const std::string& dataRatesFilename );

			virtual ~MuonAndMCScaling();
			virtual std::string briefDescription();
			virtual std::string detailedDescription();
			virtual std::unique_ptr<l1menu::TriggerRatePlot> scaleTriggerRatePlot( const l1menu::TriggerRatePlot& unscaledPlot );
			virtual std::unique_ptr<l1menu::MenuRatePlots> scaleMenuRatePlots( const l1menu::MenuRatePlots& unscaledPlots );
			virtual std::shared_ptr<l1menu::IMenuRate> scaleMenuRate( const l1menu::IMenuRate& unscaledMenuRate );
		private:
			std::unique_ptr<class MuonAndMCScalingPrivateMembers> pImple_;
		};

	} // end of namespace scalings
} // end of namespace l1menu

#endif
