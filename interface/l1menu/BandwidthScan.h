#ifndef l1menu_BandwidthScan_h
#define l1menu_BandwidthScan_h

#include <memory>
#include <vector>

// Forward declarations
namespace l1menu
{
	class ISample;
	class ITrigger;
	class TriggerMenu;
	class IMenuRate;
}

namespace l1menu
{
	/** @brief Steadily increases thresholds to create curves of how the total bandwidth changes.
	 *
	 * Not fully implemented yet so doesn't really do anything.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 26/Sep/2013
	 */
	class BandwidthScan
	{
	public:
		BandwidthScan( const l1menu::ISample& sample );
		virtual ~BandwidthScan();
		std::vector<const l1menu::IMenuRate*> scan( float fromBandwidth, float toBandwidth );
		void loadMenuFromFile( const std::string& filename );
	private:
		std::unique_ptr<class BandwidthScanPrivateMembers> pImple_;
	};

}
#endif
