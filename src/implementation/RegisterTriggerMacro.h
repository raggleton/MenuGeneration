#ifndef l1menu_implementation_RegisterTriggerMacro_h
#define l1menu_implementation_RegisterTriggerMacro_h

#include "l1menu/TriggerTable.h"

/* Macro that registers the ITrigger subclass named in the trigger table.
 *
 * This macro works by creating a new class called <triggername>Factory that calls TriggerTable::registerTrigger()
 * in its constructor. It also has a static method that creates an instance of the ITrigger subclass, this is the
 * function pointer that is supplied to the TriggerTable.
 *
 * A single instance of the <triggername>Factory is instantiated at global scope (within whatever namespace the
 * macro was called in) so that the trigger will be registered before control passes to main, or whatever the user
 * entry point is.
 */
#define DEFINE_TRIGGER_FACTORY( NAME ) class NAME##Factory \
	{ \
	public: \
		NAME##Factory( void (*pFunction)()=NULL ) \
		{ \
			NAME temporaryInstance; \
			TriggerTable::instance().registerTrigger( temporaryInstance.name(), temporaryInstance.version(), (&this->createTrigger) ); \
			if( pFunction ) (*pFunction)(); \
		} \
		static std::unique_ptr<l1menu::ITrigger> createTrigger() \
		{ \
			return std::unique_ptr<l1menu::ITrigger>(new NAME); \
		} \
	}; \

#define REGISTER_TRIGGER( NAME ) DEFINE_TRIGGER_FACTORY( NAME ) \
	NAME##Factory NAME##FactoryOnlyInstance;

#define REGISTER_TRIGGER_AND_CUSTOMISE( NAME, CUSTOM_FUNCTION_POINTER ) DEFINE_TRIGGER_FACTORY( NAME ) \
	NAME##Factory NAME##FactoryOnlyInstance( CUSTOM_FUNCTION_POINTER );

#endif
