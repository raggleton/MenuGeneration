#ifndef l1menu_MCDataScaling_h
#define l1menu_MCDataScaling_h

#include "l1menu/IScaling.h"
#include <memory>

namespace l1menu
{
	namespace scalings
	{
		/** @brief Implementation of IScaling that scales for differences between Monte Carlo and Data.
		 *
		 * Requires two sets of trigger rate plots, one from data and one from Monte Carlo that should match the
		 * data. Results are then scaled by the ratio between the two for each trigger.
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 16/Oct/2013
		 */
		class MCDataScaling : public l1menu::IScaling
		{
		public:
			/** @brief Constructing using only filenames for Monte Carlo and data rate plots. If this constructor is used then you can't scale IMenuRates.
			 *
			 * To scale an IMenuRate, the unscaled rate plots are required. So if you use the constructor and then
			 * try and call scale( const l1menu::IMenuRate& unscaledMenuRate ) an exception will be thrown.
			 */
			MCDataScaling( const std::string& monteCarloRatesFilename, const std::string& dataRatesFilename );

			/** @brief Constructor with filenames for Monte Carlo, data, and unscaled rate plots. This will give full functionality.
			 */
			MCDataScaling( const std::string& monteCarloRatesFilename, const std::string& dataRatesFilename, const std::string& unscaledRatesFilename );

			virtual ~MCDataScaling();
			virtual std::string briefDescription();
			virtual std::string detailedDescription();
			virtual std::unique_ptr<l1menu::TriggerRatePlot> scale( const l1menu::TriggerRatePlot& unscaledPlot );
			virtual std::unique_ptr<l1menu::MenuRatePlots> scale( const l1menu::MenuRatePlots& unscaledPlots );
			virtual std::unique_ptr<l1menu::IMenuRate> scale( const l1menu::IMenuRate& unscaledMenuRate );
		private:
			std::unique_ptr<class MCDataScalingPrivateMembers> pImple;
		};

	} // end of namespace scalings
} // end of namespace l1menu

#endif
