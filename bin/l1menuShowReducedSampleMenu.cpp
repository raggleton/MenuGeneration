#include <iostream>
#include <stdexcept>
#include "l1menu/ReducedSample.h"
#include "l1menu/tools/tools.h"

int main( int argc, char* argv[] )
{
	try
	{
		if( argc!=2 )
		{
			std::string executableName=argv[0];
			size_t lastSlashPosition=executableName.find_last_of('/');
			if( lastSlashPosition!=std::string::npos ) executableName=executableName.substr( lastSlashPosition+1, std::string::npos );
			std::cerr << "   Usage: " << executableName << " <ReducedSample filename>" << std::endl;
			return -1;
		}

		l1menu::ReducedSample sample( argv[1] );
		const l1menu::TriggerMenu& menu=sample.getTriggerMenu();
		l1menu::tools::dumpTriggerMenu( std::cout, menu );
	}
	catch( std::runtime_error& exception )
	{
		std::cerr << "Exception caught: " << exception.what() << std::endl;
		return -1;
	}

	return 0;
}
