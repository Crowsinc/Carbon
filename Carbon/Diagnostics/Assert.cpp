#include "Assert.hpp"

#include <Windows.h>

namespace cbn
{

	//-------------------------------------------------------------------------------------

	void _Assert(const bool assertion, const std::string& message)
	{
		if(!assertion)
		{
#ifdef _WIN32
			MessageBox(nullptr, message.c_str(), "Assert Failed", MB_OK | MB_ICONERROR);
			exit(1);
#else
			std::cerr << "Assert Failed: " << message << std::endl;
			std::cin.get();
			exit(1);
#endif
		}
	}

	//-------------------------------------------------------------------------------------

	void _AssertIf(const bool condition, const bool assertion, const std::string& message)
	{
		if(condition)
		{
			_Assert(assertion, message);
		}
	}

	//-------------------------------------------------------------------------------------

}
