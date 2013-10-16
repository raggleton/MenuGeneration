#ifndef l1menu_TriggerMenu_h
#define l1menu_TriggerMenu_h

#include <memory>
#include <vector>
#include "l1menu/TriggerTable.h"

//
// Forward declarations
//
namespace l1menu
{
	class ITrigger;
	class L1TriggerDPGEvent;
	class MenuFitter;
	class MenuScan;
	class TriggerMenu;
	namespace tools
	{
		class XMLElement;
		std::unique_ptr<l1menu::TriggerMenu> loadMenu( const std::string& filename );
	} // end of namespace tools
} // end of namespace l1menu

namespace l1menu
{
	/** @brief A collection of ITriggers that make up a menu.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date sometime around May 2013
	 */
	class TriggerMenu
	{
		friend class l1menu::MenuFitter;
	public:
		TriggerMenu();
		virtual ~TriggerMenu();
		TriggerMenu( const TriggerMenu& otherTriggerMenu );
		TriggerMenu( TriggerMenu&& otherTriggerMenu ) noexcept;
		TriggerMenu& operator=( const TriggerMenu& otherTriggerMenu );
		TriggerMenu& operator=( TriggerMenu&& otherTriggerMenu ) noexcept;

		ITrigger& addTrigger( const std::string& triggerName );
		ITrigger& addTrigger( const std::string& triggerName, unsigned int version );
		/** @brief Copies the given trigger including all parameters.
		 *
		 * @param[in] triggerToCopy    The trigger to copy.
		 * @return                     A reference to the trigger just added. Note that since it's copied,
		 *                             this is not the same as the reference supplied as input.
		 */
		ITrigger& addTrigger( const ITrigger& triggerToCopy );

		size_t numberOfTriggers() const;
		ITrigger& getTrigger( size_t position );
		const ITrigger& getTrigger( size_t position ) const;

		std::unique_ptr<l1menu::ITrigger> getTriggerCopy( size_t position ) const;

		bool apply( const l1menu::L1TriggerDPGEvent& event ) const;

	protected:
		//
		// All of these methods are deprecated in favour of going through the functions
		// in l1menu::tools. Until I fully move the functionality to there I'll protect
		// the methods and call them from the required functions.
		//
		friend std::unique_ptr<l1menu::TriggerMenu> l1menu::tools::loadMenu( const std::string& filename );

		virtual void loadMenuFromFile( const std::string& filename );
		void saveMenuToFile( const std::string& filename ) const;
		void saveToXML( l1menu::tools::XMLElement& parentElement ) const;
		void restoreFromXML( const l1menu::tools::XMLElement& parentElement );
	protected:
		void loadMenuInOldFormat( std::ifstream& file );
		/** This takes a single line from the old format file, but split into the different columns. */
		bool addTriggerFromOldFormat( const std::vector<std::string>& columns );

		TriggerTable& triggerTable_;
		std::vector< std::unique_ptr<l1menu::ITrigger> > triggers_;
		std::vector<bool> triggerResults_; ///< @brief Stores the result of each trigger for the last call of "apply"
	};

}
#endif
