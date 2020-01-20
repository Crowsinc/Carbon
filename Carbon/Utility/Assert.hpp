#pragma once

#include <string>

// This file includes custom assert functions which are removed when building for release mode
// Note that we are using defines instead of functions because the assertion condition can be 
// quite expensive at points. With defines, we can remove the assertion generation entirely 
// without having to rely on compiler optimization, which is not standard. 

#ifdef _DEBUG

namespace cbn
{
	void _Assert(const bool assertion, const std::string& message);

	void _AssertIf(const bool condition, const bool assertion,const std::string& message);
}

#define CBN_Assert(assertion, message) cbn::_Assert(assertion, message)
#define CBN_AssertIf(condition, assertion, message) cbn::_AssertIf(condition, assertion, message)

#else

#define CBN_Assert(assertion, message)
#define CBN_AssertIf(consition, assertion, message)

#endif
#pragma once
