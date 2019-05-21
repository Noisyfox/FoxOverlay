#include "GlobalConfiguration.h"


// Make sure to be consistent with constants in GlobalConfiguration.cs file
static const LPCTSTR SHARED_MEM_NAME = _T("Global\\FoxOverlay_Configuration");
static const LPCTSTR SHARED_MEM_MUTEX_NAME = _T("Global\\FoxOverlay_Configuration_Mutex");

std::shared_ptr<GlobalConfiguration> GlobalConfiguration::load()
{
	OutputDebugString(_T("Loading global configuration...\n"));
	// TODO: Acquire the mutex before open the shared memory

	auto hMap = OpenFileMapping(FILE_MAP_READ, FALSE, SHARED_MEM_NAME);
	if (!hMap)
	{
		return nullptr;
	}

	// TODO: read the config

	CloseHandle(hMap);

	return nullptr;
}
