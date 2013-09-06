#ifndef l1menu_TriggerTable_h
#define l1menu_TriggerTable_h

#include <memory>
#include <string>
#include <vector>
#include <map>

// Forward declarations
namespace l1menu
{
	class ITrigger;
}

namespace l1menu
{
	/** @brief A singleton that can create ITrigger instances of a given type and version.
	 *
	 * This is a global store for all available triggers. Code in each implementation of ITrigger
	 * will register the trigger in the TriggerTable, and then instances can be created by calls to
	 * getTrigger. Triggers have version numbers, with higher numbers considered more recent. If no
	 * version is specified when requesting a new trigger the most recent implementation is supplied.
	 *
	 * Suggested histogram binning for any trigger parameters can also be stored and retrieved to
	 * aid in plotting.
	 *
	 * Uses the Meyer's singleton pattern, the instance can be retrieved with the instance() static
	 * method.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 21/May/2013
	 */
	class TriggerTable
	{
	public:
		/** @brief A convenience class to package a trigger name and version together. */
		struct TriggerDetails
		{
			std::string name;
			unsigned int version;
			bool operator==( const TriggerDetails& otherTriggerDetails ) const;
		};
	public:
		/** @brief The only way to get an instance of the trigger table. */
		static TriggerTable& instance();

		/** @brief Get the latest version of the trigger with the supplied name. */
		std::unique_ptr<l1menu::ITrigger> getTrigger( const std::string& name ) const;

		/** @brief Get a specific version of the trigger with the supplied name. */
		std::unique_ptr<l1menu::ITrigger> getTrigger( const std::string& name, unsigned int version ) const;
		std::unique_ptr<l1menu::ITrigger> getTrigger( const TriggerDetails& details ) const;

		/** @brief Provides a copy of the supplied trigger, with the correct version and also copyies the parameters. */
		std::unique_ptr<l1menu::ITrigger> copyTrigger( const l1menu::ITrigger& triggerToCopy ) const;

		/** @brief List the triggers available.
		 *
		 * Returns a vector of pairs, where first is the trigger name and second is the trigger version.
		 */
		std::vector<l1menu::TriggerTable::TriggerDetails> listTriggers() const;

		/** @brief List the triggers available.
		 *
		 * Used by the REGISTER_TRIGGER macro in RegisterTriggerMacro.h to register triggers in the table.
		 * @param[in] name                     The name of the trigger.
		 * @param[in] version                  The version. Higher numbers are considered more recent.
		 * @param[in] creationFunctionPointer  A function pointer to a function with no parameters that returns an unique_ptr of the new trigger.
		 */
		void registerTrigger( const std::string& name, unsigned int version, std::unique_ptr<l1menu::ITrigger> (*creationFunctionPointer)() );
		void registerSuggestedBinning( const std::string& triggerName, const std::string& parameterName, unsigned int numberOfBins, float lowerEdge, float upperEdge );

		unsigned int getSuggestedNumberOfBins( const std::string& triggerName, const std::string& parameterName ) const;
		float getSuggestedLowerEdge( const std::string& triggerName, const std::string& parameterName ) const;
		float getSuggestedUpperEdge( const std::string& triggerName, const std::string& parameterName ) const;
	private:
		TriggerTable();
		~TriggerTable();
		TriggerTable( const TriggerTable& otherTriggerTable ) = delete;
		TriggerTable( TriggerTable&& otherTriggerTable ) = delete;
		TriggerTable& operator=( const TriggerTable& otherTriggerTable ) = delete;
		TriggerTable& operator=( TriggerTable&& otherTriggerTable ) = delete;

		/** @brief Hide the private members in a pimple. Google "pimple idiom" if you've not seen this before. */
		std::unique_ptr<class TriggerTablePrivateMembers> pImple_;
	};

}
#endif
