#include <sanitizer/lsan_interface.h>

// Mesa's GPU drivers (libgallium) leave a few blocks unreachable at exit, allocated on the way to a render
// texture, and nothing in the engine can free them.

#if defined(__has_feature)
#	if __has_feature(address_sanitizer)
#		define MOOT_LEAK_SANITIZER 1
#	endif
#elif defined(__SANITIZE_ADDRESS__)
#	define MOOT_LEAK_SANITIZER 1
#endif

#ifdef MOOT_LEAK_SANITIZER

extern "C" const char* __lsan_default_suppressions()
{
	return "leak:libgallium\n";
}

#endif
