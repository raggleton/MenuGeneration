#ifndef l1menu_IScaling_h
#define l1menu_IScaling_h

#include <string>
#include <memory>

//
// Forward declarations
//
namespace l1menu
{
	class TriggerRatePlot;
	class IMenuRate;
	class MenuRatePlots;
}


namespace l1menu
{
	/** @brief Abstract interface for scalings to rate plots and menu rates.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 29/Sep/2013
	 */
	class IScaling
	{
	public:
		virtual ~IScaling() {}

		/// @brief A brief overview of what the scaling does.
		virtual std::string briefDescription() = 0;

		/// @brief A detailed description of what the scaling does, including all scaling parameters.
		virtual std::string detailedDescription() = 0;

		virtual std::unique_ptr<l1menu::TriggerRatePlot> scale( const l1menu::TriggerRatePlot& unscaledPlot ) = 0;

		virtual std::unique_ptr<l1menu::MenuRatePlots> scale( const l1menu::MenuRatePlots& unscaledPlots ) = 0;

		virtual std::unique_ptr<l1menu::IMenuRate> scale( const l1menu::IMenuRate& unscaledMenuRate ) = 0;
	};

} // end of namespace l1menu

#endif
