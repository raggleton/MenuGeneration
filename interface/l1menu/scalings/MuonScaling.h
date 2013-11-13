#ifndef l1menu_MuonScaling_h
#define l1menu_MuonScaling_h

#include "l1menu/IScaling.h"
#include <memory>

namespace l1menu
{
	namespace scalings
	{
		/** @brief Implementation of IScaling that scales for better muon pT assignment and isolation
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 17/Oct/2013
		 */
		class MuonScaling : public l1menu::IScaling
		{
		public:
			/** @brief Constructor using only filename for muon scaling plots. Won't allow you to scale IMenuRates.
			 *
			 * To scale an IMenuRate, the unscaled rate plots are required. So if you use this constructor and then
			 * try and call scale( const l1menu::IMenuRate& unscaledMenuRate ) an exception will be thrown.
			 */
			MuonScaling( const std::string& muonScalingFilename );

			/** @brief Constructor with filenames for muon scaling file, and unscaled rate plots file. This will give full functionality.
			 */
			MuonScaling( const std::string& muonScalingFilename, const std::string& unscaledRatesFilename );

			virtual ~MuonScaling();
			virtual std::string briefDescription();
			virtual std::string detailedDescription();
			virtual std::unique_ptr<l1menu::TriggerRatePlot> scale( const l1menu::TriggerRatePlot& unscaledPlot );
			virtual std::unique_ptr<l1menu::MenuRatePlots> scale( const l1menu::MenuRatePlots& unscaledPlots );
			virtual std::unique_ptr<l1menu::IMenuRate> scale( const l1menu::IMenuRate& unscaledMenuRate );
		private:
			std::unique_ptr<class MuonScalingPrivateMembers> pImple;
		};

	} // end of namespace scalings
} // end of namespace l1menu

#endif
