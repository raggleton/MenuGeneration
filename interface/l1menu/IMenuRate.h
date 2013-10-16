#ifndef l1menu_IMenuRate_h
#define l1menu_IMenuRate_h

#include <vector>
#include <memory>

//
// Forward declarations
//
namespace l1menu
{
	class ITriggerRate;
	namespace tools
	{
		class XMLElement;
	}
}

namespace l1menu
{
	/** @brief Interface to the rates for a collection; individually, total and (eventually) correlations.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 24/Jun/2013
	 */
	struct IMenuRate
	{
	public:
		virtual ~IMenuRate() {}

		/** @brief The fraction of events that passed all triggers, 1 being all events and 0 no events. */
		virtual float totalFraction() const = 0;
		virtual float totalFractionError() const = 0;
		virtual float totalRate() const = 0;
		virtual float totalRateError() const = 0;

		virtual const std::vector<const l1menu::ITriggerRate*>& triggerRates() const = 0;

//		virtual void save( std::ostream& outputStream ) const = 0;
//		virtual void convertToXML( l1menu::tools::XMLElement& parentElement ) const = 0;
//		static std::unique_ptr<l1menu::IMenuRate> load( const std::string& filename );
	};

} // end of namespace l1menu

#endif
